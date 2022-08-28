// system headers
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "PQ.h"

// input parameters
int psize = 16;			// The car-park capacity.
int inval = 3;			// The number of in-valets.
int outval = 2;			// The number of out-valets.
int qsize = 8;			// The capacity of the arrivals queue.
double expnum = 0.05;	// The expected number of arrivals.

/* Statistical Variables
*
* 	During the simulation, these variables will be used 
* 	to collect data on the performance of the car-park activities.
*/
int oc = 0;		// Current number of occupied slots in the parking space.
int nc = 0;		// Running total number of cars created during the simulation
int pk = 0; 	// Running total number of cars allowed to park
int rf = 0; 	// Running total number of cars not allowed to park
int nm = 0; 	// The number of cars currently acquired by in-valets
long sqw= 0; 	// Running sum of car-waiting times in the arrival queue
long spt= 0; 	// Running sum of car-parking durations
double ut = 0; 	// Current car-park space utilization

/* Time Variables
* 
* 	These variables will be used to print the times in the simulator's final report.
*/
time_t current_t;			// The current time of each loop in the monitor thread
time_t start_t;				// Time of simulator start
time_t received_sgnl_t;		// Time of received shutdown signal
time_t shtdown_t;			// Time the car park is shut down
time_t val_leav_t;			// Time of leaving the valets
time_t val_lft_t;			// Time of left the valets
time_t monitor_exit_t;		// Time of left the monitor
time_t stop_t;				// Simulator end time

/*	Mutex locks and Semaphores.	*/
sem_t empty;			// Counting semaphore for waiting while the parking is empty to avoid the busy-waiting.
sem_t full;				// Counting semaphore for waiting while the parking is full to avoid the busy-waiting.
pthread_mutex_t Qlock;	// Mutex lock for the arrivals queue
pthread_mutex_t PQlock;	// Mutex lock for the parking slots

/* tids */
pthread_t *in_tids;
pthread_t *out_tids;
pthread_t *mon_tids;

/* The thread which will simulate the in-valet behavior */
void *in_valet(int id);

/* The thread which will simulate the out-valet behavior */
void *out_valet(int id);

/* The thread which will simulate the monitor behavior */
void *mon();

/* Signal handling */
static volatile int stop = 0;
/* An event handler to stop the program when pressing ctrl c */ 
void handle(int sig){
	received_sgnl_t = time(NULL);
	stop = 1;
	shtdown_t = time(NULL);
}


/* Function takes a string as an input and return true if it is a whole number, false otherwise */
bool check_int(char inp[]);

/* Function takes a string as an input and return true if it is a float number, false otherwise */
bool check_float(char inp[]);


/*--- THE MAIN ---*/
int main(int argc, char *argv[]){
	
	/* Checking and validating arguments */
	if(argc > 6){
		printf("Please enter the argument currectly [psize inval outval qsize expnum]\n");
		exit(0);
	}else{
		for(int i=1; i<argc; i++){
			// Checking for the first argument, the car-park capacity.
			if(i == 1){
				if(check_int(argv[1])){
					int in = atoi(argv[1]);
					if(in<12) in = 12;
					if(in>40) in = 40;
					if(in%4 != 0) {printf("psize must be a multiple of 4 ...exiting\n"); exit(0);}
					psize = in;
				}else{
					printf("Please enter a valid integer number in the first argument, exiting...\n");
					exit(0);
				}

			}
			// Checking for the second argument, in-valet number.
			if(i == 2){
				if(check_int(argv[2])){
					int in = atoi(argv[2]);
					if(in<1) in = 1;
					if(in>6) in = 6;
					inval = in;
				}else{
					printf("Please enter a valid integer number in the second argument, exiting...\n");
					exit(0);
				}

			}
			// Checking for the third argument, out-valet number.
			if(i == 3){
				if(check_int(argv[3])){
					int in = atoi(argv[3]);
					if(in<1) in = 1;
					if(in>6) in = 6;
					outval = in;
				}else{
					printf("Please enter a valid integer number in the third argument, exiting...\n");
					exit(0);
				}

			}
			// Checking for the fourth argument, the capacity of the arrivals queue.
			if(i == 4){
				if(check_int(argv[4])){
					int in = atoi(argv[4]);
					if(in<3) in = 3;
					if(in>8) in = 8;
					qsize = in;
				}else{
					printf("Please enter a valid integer number in the fourtg argument, exiting...\n");
					exit(0);
				}
			}
			// Checking for the fifth argument, expected number of arrivals.
			if(i == 5){
				if(check_float(argv[5])){
					double in =(double) atof(argv[5]);
					if(in<0.01) in = 0.01;
					if(in>1.5) in = 1.5;
					expnum = in;
				}else{
					printf("Please enter a valid double number in the fifth argument, exiting...\n");
					exit(0);
				}
			}
		}
	}

	/* Init. queues */
	Qinit(qsize);
	PQinit(psize);

	/* Init semaphores and mutex locks */
	sem_init(&empty, 0, psize);
	sem_init(&full, 0, 0);

	while(1){if (pthread_mutex_init(&Qlock, NULL) == 0) break;}
	while(1){if (pthread_mutex_init(&PQlock, NULL) == 0) break;}

	/* Init. signal handler to abort the program when wanted */
	signal(SIGINT, handle);

	/* Init. the car park */
	int parkings_size;
	Car** parkings = PQiterator(&parkings_size);

	/* Init. the graphical system*/
	G2DInit(parkings, psize, inval, outval, PQlock);

	/* Get the default attributes for threads */
	pthread_attr_t attr;
    pthread_attr_init(&attr);

	// In-valets thread ids array
	pthread_t in_tid[inval]; 	
	/* Create in-valets threads */
	for (int i = 0; i < inval; i++){
		pthread_create(&in_tid[i], &attr, in_valet, i);
	}
	in_tids = in_tid; // Assigning addres to threads pointer

	// Out-valets thread ids array
	pthread_t out_tid[outval]; 	
    /* Create out-valets threads */
	for (int i = 0; i < outval; i++){
		pthread_create(&out_tid[i], &attr, out_valet, i);
	}
	out_tids = out_tid; // Assigning addres to threads pointer

	/* Create monitor thread */
	pthread_t mon_tid;
	pthread_create(&mon_tid, &attr, mon, NULL);
	mon_tids = mon_tid;

	int gen; 			// No. of generated cars in every loop

	/* Start the simulation */ 
	start_t = time(NULL);
	/* Main Thread
	* 	Entering an endless loop until receiving the signal.
	*	
	*	It will constantly create cars and enter them into the 
	*	arrival queue if it is not full.
	*/
	while(!stop){
		gen = newCars(expnum);		// get a random number with Poisson distribution
		for(int i=0; i<gen ; i++){
			Car *car = (Car*) malloc(sizeof(Car));	// Allocate memory for new car
			pthread_mutex_lock(&Qlock);  // Lock the arrival queue
			if (QisFull()){
				rf++;			// this car is not allowed to park
			}else{
				CarInit(car);	// init. a car 
				nc++;			// this car is allowed to park
				Qenqueue(car);	// the car waiting in arrival queue
			}
			float rd = (rand() % 20)/100.0;	// get a random value between 0 and 0.2
			updateStats(oc, nc, pk, rf, nm, sqw, spt, ut);
			sleep(rd);
			pthread_mutex_unlock(&Qlock);	// Unlock the arrival queue
			//updateStats(oc, nc, pk, rf, nm, sqw, spt, ut);
			signal(SIGINT, handle);	// signal handler
		}
		show();
		sleep(1);
	}/* End of simulation*/
	
	stop_t = time(NULL);
	val_leav_t = time(NULL);

	/* cancel in-valet and out-valet threads */
	for(int i = 0; i<inval; i++){
		pthread_cancel(in_tid[i]);
	}
	for(int i = 0; i<outval; i++){
		pthread_cancel(out_tid[i]);
	}
	val_lft_t = time(NULL);

	/* Cancel monitor thread */
	monitor_exit_t = time(NULL);
	pthread_cancel(mon_tid);

	/* Destroy Mutex locks and Semaphores. */
	pthread_mutex_destroy(&Qlock);
	pthread_mutex_destroy(&PQlock);
	sem_destroy(&empty);
	sem_destroy(&full);


	
	/* Print the final report of the simulation */
	printf("Received shutdown signal at:\t%s", ctime(&received_sgnl_t));
	printf("Car park is shutting down at:\t%s", ctime(&shtdown_t));
	printf("The valets are leaving at:\t%s", ctime(&val_leav_t));
	printf("Done. %d valets left at:\t\t%s", inval+outval, ctime(&val_lft_t));
	printf("Monitor exiting at:\t\t%s", ctime(&monitor_exit_t));

	printf("\nSimulator started at:\t\t%s", ctime(&start_t));
	printf("  Park Space Capacity was:\t%d\n", psize);
	printf("  Allowed queue length was:\t%d\n", qsize);
	printf("  Number of in valets was:\t%d\n", inval);
	printf("  Number of out valets was:\t%d\n", outval);
	printf("  Expected arrivals was:\t%.2f\n", expnum);
	printf("Simulator stopped at:\t\t%s", ctime(&stop_t));

	printf("\nCP Simulation was executed for:\t%ld seconds\n",stop_t - start_t);
	printf("Total number of cars processed:\t%d cars\n", nc+rf);
	printf("  Number of cars that parked:\t%d cars\n", pk);
	printf("  Number of cars turned away:\t%d cars\n", rf);
	printf("  Number of cars in transit:\t%d cars\n", nc-pk-Qsize());
	printf("  Number of cars still queued:\t%d cars\n", Qsize());
	printf("  Number of cars still parked:\t%d cars\n\n", PQsize());
	
	printf("Average queue waiting time:\t\t%.2f seconds\n", (float) sqw/pk);
	printf("Average parking time:\t\t%.2f seconds\n", (float) spt/(pk-PQsize()));
	printf("Percentage of park utilization:\t%.2f%%\n\n", ut);

	/* Free the dynamic memory allocated */
	Qfree();
	PQfree();

	finish();
}



/* The thread which will simulate the in-valet behavior */
void *in_valet(int id) {	
	while(1){
		pthread_mutex_lock(&Qlock);	// Lock the arrival queue
		Car* serveCar = Qpeek();
		if(!serveCar) {		// if the queue is empty
			float rd = (rand() % 20)/100.0;	// get a random value between 0 and 0.2
			sleep(rd);
			pthread_mutex_unlock(&Qlock);	// Unlock the arrival queue
			continue;
		}
		serveCar = Qserve();			// the valet fetch the car
		setViState(id, FETCH);
		setViCar(id, serveCar);
		serveCar->vid = id;				// set vid of the car
		float rd = (rand() % 20)/100.0;	// get a random value between 0 and 0.2
		sleep(rd);
		pthread_mutex_unlock(&Qlock);	// Unlock the arrival queue

		setViState(id, WAIT);
		sem_wait(&empty);
		pthread_mutex_lock(&PQlock);	// Lock the parking

		setViState(id, MOVE);
		float rd_pk = (rand() % 100)/100.0; // get a random value between 0 and 1

		nm++;
		pk++;
		sleep(rd_pk);

		(*serveCar).ptm = current_t;		// set the start time of parking
		
		
		sqw = sqw + (serveCar->ptm - serveCar->atm);
		if(!PQisFull()){
			float rd_pk = (rand() % 100)/100.0;	// get a random value between 0 and 1
			sleep(rd_pk);
			PQenqueue(serveCar);			// park the car
			oc++;
			printf("In-valet %d: Car %d is parking in slot %d\n",id,serveCar->cid,serveCar->sno);
		}
		rd = (rand() % 20)/100.0;			// get a random value between 0 and 0.2
		sleep(rd); 
		pthread_mutex_unlock(&PQlock);		// Unlock the parking
		setViState(id, READY);
		nm--;	

	}
	pthread_exit(0);
}



/* The thread which will simulate the out-valet behavior */
void *out_valet(int id) {	
	while(1){
		sem_wait(&full);
		setVoState(id, WAIT);
		pthread_mutex_lock(&PQlock);	// Lock the parking
	
		Car* serveCar = PQpeek();		// get minimum car's ltm
		if(serveCar){
			if(serveCar->ltm <= 0){		// it is time to out
				setVoState(id, MOVE);
				setVoCar(id, serveCar);
				float rd_pk = (rand() % 100)/100.0;			// get a random value between 0 and 1
				sleep(rd_pk);
				spt = spt + (current_t - serveCar->ptm);
				printf("Out-valet %d: Car at slot %d is leaving car park\n",id,serveCar->sno);
				free(PQserve());		// the valet take the car from parking and free the dynamic memory allocated
				serveCar=NULL;
				oc--;
				sem_post(&empty);
				rd_pk = (rand() % 100)/100.0;				// get a random value between 0 and 1
				sleep(rd_pk);
			}
		}
		float rd = (rand() % 20)/100.0;		// get a random value between 0 and 0.2
		sleep(rd);
		pthread_mutex_unlock(&PQlock);		// Unlock the parking
		setVoState(id, READY);
	}
	pthread_exit(0);
}

void *mon(){
	int parkings_size;
	Car **parkings;
	time_t prev_t = time(NULL);
	while(1){
		current_t = time(NULL);
		pthread_mutex_lock(&PQlock);			// Lock the arrival queue
		parkings = PQiterator(&parkings_size);	// get an array of acrs in the parking
		int duration = current_t - prev_t;		// calculate the time from the last iteration
		/* Print and update the state of the parking */
		printf("Monitor: Number of cars in carpark: %d\n",parkings_size);
		printf("Slot:\t|");
		for (int i = 0; i < psize; i++){
			printf("%d\t|",i+1);
		}
		printf("\n\t|");
		for (int i = 0; i < psize; i++){
			printf("\t|");
		}
		printf("\nPark:\t|");
		for (int i = 0; i < psize; i++) {
			if (parkings[i]) {
				parkings[i]->ltm = parkings[i]->ltm - duration; // update the ltm
				if (parkings[i]->ltm <=0) sem_post(&full);
				printf("%d\t|",parkings[i]->cid);
			}
			else printf("%d\t|",0);
		}
		printf("\n");
		float rd = (rand() % 20)/100.0;		// get a random value between 0 and 0.2
		sleep(rd);
		pthread_mutex_unlock(&PQlock);		// Unlock the arrival queue
		printf("-------------------------------------------------------------------\n");
		prev_t = current_t;
		ut = ( (double) spt/(psize*(current_t - start_t)))*100;
		sleep(1);
	}
}


/* function takes a string as an input and return true if it is a whole number, false otherwise */
bool check_int(char inp[]){
	for(int i=0; i<strlen(inp); i++){ // iterate through characters
		if(!isdigit(inp[i])) 	// check if the character is not a digit
			return 0;	// return false if not a digit
	}
	return 1; // return true if all characters are digits
}


/* function takes a string as an input and return true if it is a float number, false otherwise */
bool check_float(char inp[]){
	int dots = 0;
	for(int i=0; i<strlen(inp); i++){ // iterate through characters
		if(inp[i] == '.'){
			if(dots == 1)
				return 0;
			dots = 1;
			continue;
		}
		if(!isdigit(inp[i])) 	// check if the character is not a digit
			return 0;	// return false if not a digit
	}
	return 1; // return true if all characters are digits
}
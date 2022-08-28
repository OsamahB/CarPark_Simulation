#include <stdlib.h>
#include <stdio.h>
#include "PQ.h"

/**
     * Description: Function initializes the priority queue
     * @param n: the capacity
     * @postcondition: the queue is created
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
void PQinit(int n){
	carsPQ.capacity = n;
	carsPQ.count = 0;
	carsPQ.list =(Car**) calloc(n, sizeof(Car*));
	carsPQ.iter =(Car**) calloc(n, sizeof(Car*));
}


/**
     * Description: Function returns the capacity of the queue.
     * @postcondition: the capacity is returned.
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
int PQcapacity(){
	return carsPQ.capacity;
}


/**
     * Description: Function returns the size of the queue
     * @postcondition: the size is returned.
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
int PQsize(){
	return carsPQ.count;
}


/**
     * Description: Function frees the pointer and deletes the queue
     * @postcondition: queue is deleted, and pointers are free.
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
void PQfree(){
    carsPQ.count = 0;
    free(carsPQ.list);
    free(carsPQ.iter);
}


/**
     * Description: Function empties the queue
     * @postcondition: queue is empty.
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
void PQclear(){
	carsPQ.count = 0;
	//carsPQ.list =(Car**) realloc(carsPQ.list, 0*sizeof(Car*));
}


/**
     * Description: Function checks if the queue is empty
     * @postcondition: returns 1 if empty, 0 otherwise
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
bool PQisEmpty(){
	return carsPQ.count == 0;
}

/**
     * Description: Function checks if the queue is full
     * @postcondition: returns 1 if full, 0 otherwise
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
bool PQisFull(){
	return carsPQ.count == PQcapacity();
}

/**
     * Description: Function checks returns list of pointers to the cars in the queue.
     * @param *sz: pointer to the size, an output parameter will be updated in the function.
     * @postcondition: the list of cars’ pointers is returned, and the size is updated.
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
Car** PQiterator(int *sz){
	*sz = PQsize();
	for(int i=0; i<PQsize(); i++){
		carsPQ.iter[i] = carsPQ.list[i];
	}
	return carsPQ.iter;	
}

/**
     * Description: Function inserts an element to the priority queue
     * @param *car: a pointer to the car
     * @precondition: the queue is not full
     * @postcondition: the car is inserted, and the counter is incremented
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
void PQenqueue(Car *car){
	if (!PQisFull()){
		carsPQ.count++;
		for(int i=0; i<PQsize(); i++){
			if(!carsPQ.list[i]){
				car->sno = i + 1;
		  		carsPQ.list[i] = car;
				break;
			}
		}

		//carsPQ.list =(Car**) realloc(carsPQ.list, PQsize()*sizeof(Car *));
		//carsPQ.list[carsPQ.count-1] = car;
	}
}

/**
     * Description: Function removes the highest priority element from the queue
     * @precondition: the queue is not empty
     * @postcondition: the min. priority element is removed and the counter is decremented.
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
Car* PQserve(){
	if (!PQisEmpty()){
		time_t min = carsPQ.list[0]->ltm;
		int min_idx = 0;
		for(int i=0; i<PQsize(); i++){
			if((carsPQ.list[i]) && (carsPQ.list[i]->ltm <= min)){
		  	    min = carsPQ.list[i]->ltm;
			    min_idx = i;
			}
		}
		carsPQ.count--;
		Car* temp = carsPQ.list[min_idx];
		carsPQ.list[min_idx] = NULL;
				
		return temp;
	}
	return NULL;
}

/**
     * Description: Function retrieves the highest priority element from the queue without removing it.
     * @precondition: the queue is not empty
     * @postcondition: max. priority element is retrieved.
     * Author: Ibrahim Alfaris
     * Version: 1.0 16/4/2022
     */
Car* PQpeek(){
	if (!PQisEmpty()){
		time_t min = carsPQ.list[0]->ltm;
		int min_idx = 0;
		for(int i=0; i<PQsize(); i++){
			if(carsPQ.list[i]){
			    if(carsPQ.list[i]->ltm <= min){
			  	min = carsPQ.list[i]->ltm;
				min_idx = i;
			    }
			}
		}
		return carsPQ.list[min_idx];
	}
	return NULL;
}
#A --->

The difficulties that we encountered in this project:

1- Semaphores implmentaion. Due to poor implementation, the overall perfoemance of the simulation was
affected negatively. It lets the valets to serve cars from the queue to priority queue in unsuncronized way. 
However, we have applied some changes to enhance our implementation as well as the performance of the simulation.

2- Another issue is deadlocks, but as mentioned in the prevous point, improving semaphores implementation has helped
us to avoid deadlocks and busy-waiting. 



#B --->

The requirements that we have designed in our project:

1- It has mutual exclusion.
2- It uses semaphores for synchronization and mutual exclusion.
3- Cars cannot be removed from an empty car park or an empty slot.
4- Cars cannot be added to a full cat park or an occupied slot.
5- Threads pause for a random period (up tp 1s) before and after parking or remocing a car.
6- Threads pause for a random period (up tp 0.2s) in its critical section.
7- Cars will stay parked for a random period (from 1s to 180s)
8- Cars are not permitted to leave before the intended leaving time.
9- The monitor thread is periodically printing and graphically displaying the current 
status of the cat park.
10- The simulator will continue working until it is interrupted from the keyboard by control-c.

#C --->

All requirements were designed and realized.


#D --->

The project's build command:

" $ ./cp [psize inval outval qsize expnum]" NOTE: any item between [] is optional.

The command line arguments to the simulator are:
- psize:  The car-park capacity.
- inval:  The number of in-valets.
- outval: The number of out-valets.
- qsize:  The capacity of the arrivals queue.
- expnum: The expected number of arrival. 

The default values of the argument when invoking the simultor as "$ ./cp"
- psize = 16
- inval = 3
- outval= 2
- qsize = 8
- expnum= 0.05


#E --->
We remarked that semaphores and mutual exclusion should be delt carfully, otherwise they might 
create many issues such as deadlocks. Moreover, we thought that the implementation of 
utilization is the number of occuipied slots divied by the number of slots in the parking lot,
 but we were wrong about it. After which, we have changed it to become the sum of car-parking 
durations divided by the product of the number of slots in the parking lot and total time of 
simulation. 

/* bathroomSim
 * Andrew Morrison
 * Peter Christakos
 * 	takes in - bathroomSim nUsers meanLoopCount meanArrival meanStay
 *	use Box-Muller transform to create random numvers with a normal distribution
 * 	
 *	Calls Initialize
 *	Uses pthread_create to create all threads
 *	Waits until all threads have finished
 *	Calls finalized
 *
 *	Individual(Gender, arrival time, time to stay, loop count) - used to implement the threads
 * 		Gender - defined by random variable
 *		loop count - random based on the mean specified by the command line
 *		arrival and stay - random
 *			sleeps until arrival time
 *			invoke enter() - may have to wait a long time
 *			sleeps stay amount of time
 *			invokes leave
 *			repeat loop time amount of times
 *			each thread keeps track of how long it is in the bathroom
 *			after exiting thread prints:
 *				Its own thread number
 *				Its gender and number of loops
 *				The min average and max time spent in the queue */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Bathroom.h"

/* individual will create each thread individually, calling pthread_create to do so
 * It will create the thread with the function parameter specifications such as:
 *	g - a random num 0 or 1 that defines the gender of the thread
 *	arrival - the arrival time of the thread
 *	stay - the amount of time the thread will stay in the bathroom
 *	lCount - the number of times the thread will loop through the bathroom
 */
void *individual(pthread_cond_t vacant, pthread_mutex_t lock, struct br* brGlobal, int g, long arrival, long stay, int lCount, int threadNum)
{
	long minQueue, aveQueue, maxQueue, brTime; // variables to keep track of thread statistics
	int rc; // use for assert statements

	// lock
  	int rc = pthread_mutex_lock(&lock);
	checkERR(rc);
  	// unlock and wait for each thread to be created, then lock
 	rc = pthread_cond_wait(&vacant, &lock);
	if (rc == 0) 
	{
		// wait
	}
	// unlock
	pthread_mutex_unlock(&lock);

	for(int i = 0; i<lCount; i++)
	{
		// wait for arrival time
		usleep(arrival);

		while(1)
		{
			// check if can enter bathroom
			rc = pthread_mutex_lock(&lock);
			assert(rc == 0);
			if(enter(brGlobal, g)==1) // if can enter
			{
				// go to the bathroom
				rc = pthread_mutex_unlock(&lock);
				assert(rc == 0);

				usleep(stay);

				rc = pthread_mutex_lock(&lock);
				assert(rc == 0);
				leave(brGlobal);

				/* QUESTION: How do we handle broadcast so that when it signals the other threads,
				 * the lock is freed from this thread, and all other threads can grab the lock */
				if(brGlobal->gender == -1) // Bathroom is vacant so signal threads
				{
					//rc = pthread_mutex_lock(&broad); // make sure broadcast and unlock happen at the same time... Is this necessary?

					rc = pthread_cond_broadcast(&vacant); // broadcast to all other threads
					assert(rc == 0);
					// what if we pause here?
					rc = pthread_cond_unlock(&lock);
					assert(rc == 0);

					//rc = pthread_mutex_unlock(&broad); // is this necessary?
				}
				else
				{
					rc = pthread_mutex_unlock(&lock);
					assert(rc == 0);
					break;
				}
			}
			else
			{
				// wait for the bathroom to be vacant so one can enter
				rc = pthread_cond_wait(&vacant, &lock);
			}
		}
	}
}

int main(int argc, char* argv[])
{
	assert(argc == 5);
}
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
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

struct brGlobal
{
	int gender; // -1 for vacant, 0 for female, and 1 for male
	int mCount;
	int fCount;
	int totalUsages;
	long vacantTime;
	long occupiedTime;
	//int averageQL;
	//int avePeople;
};

// create characteristics of thread
// use enter() here
void *threadChar(void *arg) 
{
  printf("%s: Wow! You impregnated a functiont and made a baby thread! \n", (char *) arg);
  //create gender and incremement global counter
  if ((int gender = rand() % 2) == 1) 
  {
  	brGlobal.mCount++;
  } 
  else
  {
  	brGlobal.fCount++;
  }
  // create arrivall time
  // create stay time
  // create loop number (how many times it will go to the bathroom)
  return NULL;
}

// creates a thread with a gender, arrival time, stay time, loop
void individual()
{
	pthread_t thread;
	int rc;
	/* pthread_create(thread_pointer, extra variable attributes, 
	 *pointer to func call, pointer being passed as function arg)
	 */
	rc = pthread_create(&thread, NULL, threadChar, NULL); 
	assert(rc == 0);
}
/* Bathroom.c
 * Andrew Morrison
 * Peter Christakos */

#include "Bathroom.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
#include <sys/time.h>

struct br *brGlobal;
int totalCount = 1; // keep track of which order threads finish
struct timeval clockTime; // struct to access gettimeofday

// function to return status of bathroom
static int brStatus()
{
	if(brGlobal->mCount > 0)
	{
		assert(brGlobal->fCount == 0);
		return 1;
	} 
	else if(brGlobal->fCount > 0)
	{
		assert(brGlobal->mCount == 0);
		return 0;
	} 
	else return -1;
}

// lock and unlock the "lock" mutex for the enter fn
long enter(int g)
{

	struct timeval startTime, endTime;
	long elapsedTime = 0;
	startTime.tv_sec = 0;
	startTime.tv_usec = 0;
	endTime.tv_sec = 0;
	endTime.tv_usec = 0;
	
	pthread_mutex_lock(&brGlobal->lock);
	switch(brStatus())
	{
		case 1: // check if occupied by male
			if(g == 1)
			{
				brGlobal->mCount++;
				brGlobal->totalUsages++;
			}
			else if(g == 0)
			{
				gettimeofday(&startTime, NULL); // start time to show how long it wait
				while(brStatus() == 1)
				{
					pthread_cond_wait(&brGlobal->mVacant, &brGlobal->lock);
				}
				gettimeofday(&endTime, NULL); 
				brGlobal->fCount++;
				brGlobal->totalUsages++;
			}
			break;
		case 0: // check if occupied by female
			if(g == 0)
			{
				brGlobal->fCount++;
				brGlobal->totalUsages++;
			}
			else if(g == 1)
			{
				gettimeofday(&startTime, NULL); // start time to show how long it wait
				while(brStatus() == 0)
				{
					// wait if not
					pthread_cond_wait(&brGlobal->fVacant, &brGlobal->lock);

				}
				
				gettimeofday(&endTime, NULL); // timestamp to keep track of when it stops waiting
				// became male occupied
				brGlobal->mCount++;
				brGlobal->totalUsages++;
				
			}
			break;
		case -1: // empty case
			g == 1 ? brGlobal->mCount++ : brGlobal->fCount++;
			brGlobal->totalUsages++;
			gettimeofday(&brGlobal->occupiedStartTime, NULL);
			gettimeofday(&brGlobal->vacantEndTime, NULL);
			long temp = abs(((1000*brGlobal->vacantEndTime.tv_sec)-(1000*brGlobal->vacantStartTime.tv_sec)) + ((brGlobal->vacantEndTime.tv_usec/1000)-(brGlobal->vacantStartTime.tv_usec/1000)));
			if(temp>0 && temp <10000)
				{
					brGlobal->vacantTime += temp;
				}
			break;
	}
	pthread_mutex_unlock(&brGlobal->lock); // record elapsed time
	elapsedTime = abs(((endTime.tv_sec*1000) - (startTime.tv_sec*1000)) + ((endTime.tv_usec/1000) - (startTime.tv_usec/1000)));
	return elapsedTime;
}

// lock and unlock the vacant mutex for the leave fn
void leave()
{
	pthread_mutex_lock(&brGlobal->lock);
	switch(brStatus())
	{
		case 1: // male
			brGlobal->mCount--;
			if(brGlobal->mCount == 0)
			{
				pthread_cond_broadcast(&brGlobal->mVacant);
				gettimeofday(&brGlobal->vacantStartTime, NULL);
				gettimeofday(&brGlobal->occupiedEndTime, NULL);
				long temp = abs(((1000*brGlobal->occupiedEndTime.tv_sec)-(1000*brGlobal->occupiedStartTime.tv_sec)) + ((brGlobal->occupiedEndTime.tv_usec/1000)-(brGlobal->occupiedStartTime.tv_usec/1000)));
				if(temp>0 && temp<10000)
				{
					brGlobal->occupiedTime += temp;
					assert(brGlobal->fCount == 0 && brGlobal->mCount == 0);
				}
			}
			break;
		case 0: // female
			brGlobal->fCount--;
			if(brGlobal->fCount == 0)
			{
				pthread_cond_broadcast(&brGlobal->fVacant);
				gettimeofday(&brGlobal->vacantStartTime, NULL);
				gettimeofday(&brGlobal->occupiedEndTime, NULL);
				long temp = abs(((1000*brGlobal->occupiedEndTime.tv_sec)-(1000*brGlobal->occupiedStartTime.tv_sec)) + ((brGlobal->occupiedEndTime.tv_usec/1000)-(brGlobal->occupiedStartTime.tv_usec/1000)));
				if(temp>0 && temp < 10000)
				{

					brGlobal->occupiedTime += temp;
					assert(brGlobal->fCount == 0 && brGlobal->mCount == 0);
				}
			}
			break;
	}
	pthread_mutex_unlock(&brGlobal->lock);
}


void initialize()
{
 	brGlobal = (struct br *)malloc(sizeof(struct br));
 	brGlobal->gender = -1;
	brGlobal->mCount = 0;
	brGlobal->fCount = 0;
	brGlobal->totalUsages = 0;
	brGlobal->vacantTime = 0;
	brGlobal->occupiedTime = 0;
	pthread_mutex_init(&brGlobal->lock, NULL); // these two might not be necessary
	pthread_cond_init(&brGlobal->fVacant, NULL);
	pthread_cond_init(&brGlobal->fVacant, NULL);
}

/* Prints out all statistics and exits
 */
void finalize()
{
	printf("All Threads Completed!\n");
	printf("\n");
	printf("\n************* END OF PROGRAM STATS ************\n");
	printf("\nTotal Usages: %d\nVacant Time: %ld ms\nOccupied Time: %ld ms\n", brGlobal->totalUsages, brGlobal->vacantTime+1, brGlobal->occupiedTime);
}
/* Prints out statistics for each individiaul thread before it exits
 * Will print:
 *	its own thread number
 *	its gender and number of loops
 *	the min, average, and max time spent waiting in the queue
 */
void printStats(int gender, int threadNum, int lCount, long minTime, long aveTime, long maxTime)
{
  printf("\n~~~~~~~~~~~~~ THREAD [%d] STATISTICS~~~~~~~~~~~\n", threadNum+1);
  printf("This is Thread #%i to have completed\n", totalCount);
  gender == 1 ? printf("This Thread's Gender: Male\n") : printf("This Thread's Gender: Female\n");
  printf("Loop count: %d\n", lCount);
  printf("Min time spent in queue: %ld ms\n", minTime);
  printf("Ave time spent in queue: %ld ms\n", aveTime);
  printf("Max time spent in queue: %ld ms\n", maxTime);
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("\n");
  totalCount++;
}

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
		return 1;
	} 
	else if(brGlobal->fCount > 0)
	{
		return 0;
	} 
	else return -1;
}

// lock and unlock the "lock" mutex for the enter fn
long enter(int g)
{
	struct timeval startTime, elapsedTime;
	long elapsedTime = 0;
	
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
				while(brStatus() == 1)
				{
					sched_yield();
				}
				brGlobal->fCount++;
				brGlobal->totalUsages++;
			}
			break;
		case 0: // check if occupied by female
			if(g == 0)
			{
				// incrememnt stats if it is
				brGlobal->fCount++;
				brGlobal->totalUsages++;
			}
			else if(g == 1)
			{
				gettimeofday(&startTime, NULL); // start time to show how long it waits
				//startTime = (clockTime.tv_sec * 1000) + (clockTime.tv_usec / 1000); // convert to ms

				while(brStatus() == 0)
				{
					// wait if not
					sched_yield();
				}
				
				gettimeofday(&endTime, NULL); // timestamp to keep track of when it stops waiting
				//endTime = (clockTime.tv_sec * 1000) + (clockTime.tv_usec / 1000); // convert to ms
				
				// became male occupied
				brGlobal->mCount++;
				brGlobal->totalUsages++;
			}
			break;
		case -1: // empty case
			g == 1 ? brGlobal->mCount++ : brGlobal->fCount++;
			brGlobal->totalUsages++;
			break;
	}
	pthread_mutex_unlock(&brGlobal->lock);
	elapsedTime = abs(endTime.tv_usec - startTime.tv_usec); // record elapsed time
	return elapsedTime;
}

// lock and unlock the vacant mutex for the leave fn
void leave()
{
	pthread_mutex_lock(&brGlobal->vacant);
	switch(brStatus())
	{
		case 1: // male
			brGlobal->mCount--;
			break;
		case 0: // female
			brGlobal->fCount--;
			break;
	}
	pthread_mutex_unlock(&brGlobal->vacant);
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
	pthread_mutex_init(&brGlobal->vacant, NULL);
}

/* Prints out all statistics and exits
 */
void finalize()
{
	printf("\n************* END OF PROGRAM STATS ************\n");
	printf("\nTotal Usages: %d\nVacant Time: %ld\nOccupied Time: %ld\n", brGlobal->totalUsages, brGlobal->vacantTime, brGlobal->occupiedTime);
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
  printf("This is the %ith thread to have completed\n", totalCount);
  gender == 1 ? printf("Gender: Male\n") : printf("Gender: Female\n");
  printf("Loop count: %d\n", lCount);
  printf("Min time spent in queue: %ld\n", minTime);
  printf("Ave time spent in queue: %ld\n", aveTime);
  printf("Max time spent in queue: %ld\n", maxTime);
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("\n");
  totalCount++;
}

/* returns the gender */
int getGender()
{
  return brGlobal->gender;
}
int getMCount()
{
	return brGlobal->mCount;
}
int getFCount()
{
	return brGlobal->fCount;
}
int getTotalU()
{
	return brGlobal->totalUsages;
}

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
				//printf("StartTime: %ld\n", startTime.tv_sec*1000 + startTime.tv_usec/1000);
				while(brStatus() == 1)
				{
					//sched_yield();
				//	printf("waiting\n");
					pthread_cond_wait(&brGlobal->mVacant, &brGlobal->lock);
					brGlobal->mCount--;
				}
				gettimeofday(&endTime, NULL); 
				//printf("endTime: %ld\n", endTime.tv_sec*1000 + endTime.tv_usec/1000);
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
				gettimeofday(&startTime, NULL); // start time to show how long it wait
				//printf("StartTime: %ld\n", startTime.tv_sec*1000 + startTime.tv_usec/1000);
				while(brStatus() == 0)
				{
					// wait if not
					//sched_yield();
					pthread_cond_wait(&brGlobal->fVacant, &brGlobal->lock);
					brGlobal->mCount--;
					//printf("waiting\n");
				}
				
				gettimeofday(&endTime, NULL); // timestamp to keep track of when it stops waiting
				//printf("endTime: %ld\n", endTime.tv_sec*1000 + endTime.tv_usec/1000);
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
			if(temp>0 && temp <1000000)
				{
					//printf("temp: %ld", temp);
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
	pthread_mutex_lock(&brGlobal->vacant);
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
				if(temp>0 && temp<1000000)
				{
					//printf("temp: %ld", temp);
					brGlobal->occupiedTime += temp;
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
				if(temp>0 && temp < 1000000)
				{
					//printf("temp: %ld", temp);
					brGlobal->occupiedTime += temp;
				}
			}
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
	pthread_cond_init(&brGlobal->fVacant, NULL);
	pthread_cond_init(&brGlobal->fVacant, NULL);
}

/* Prints out all statistics and exits
 */
void finalize()
{
	printf("\n************* END OF PROGRAM STATS ************\n");
	printf("\nTotal Usages: %d\nVacant Time: %ld ms\nOccupied Time: %ld ms\n", brGlobal->totalUsages, brGlobal->vacantTime, brGlobal->occupiedTime);
}
/* Prints out statistics for each individiaul thread before it exits
 * Will print:
 *	its own thread number
 *	its gender and number of loops
 *	the min, average, and max time spent waiting in the queue
 */
void printStats(int gender, int threadNum, int lCount, long minTime, long aveTime, long maxTime)
{
  //pthread_mutex_lock(&brGlobal->lock);
  printf("\n~~~~~~~~~~~~~ THREAD [%d] STATISTICS~~~~~~~~~~~\n", threadNum+1);
  printf("This is the %ith thread to have completed\n", totalCount);
  gender == 1 ? printf("Gender: Male\n") : printf("Gender: Female\n");
  printf("Loop count: %d\n", lCount);
  printf("Min time spent in queue: %ld ms\n", minTime);
  printf("Ave time spent in queue: %ld ms\n", aveTime);
  printf("Max time spent in queue: %ld ms\n", maxTime);
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("\n");
  totalCount++;
  //pthread_mutex_unlock(&brGlobal->lock);
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

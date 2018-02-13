/* bathSim
 * Andrew Morrison
 * Peter Christakos
 * 	takes in - bathroomSim nUsers meanLoopCount meanArrival meanStay
 *	use Box-Muller transform to create random numvers with a normal distribution
 */
#include "Bathroom.h"
#ifndef M_PI
#define M_PI = #define M_PI 3.14159265358979323846
#endif

int god = 0;


/**************************************************************** STRUCTS **********************************************************************/

struct argstruct
{
	pthread_mutex_t printLock;
	pthread_mutex_t vacant;
	pthread_mutex_t lock;
	int gender;
	double arrival;
	double stay;
	int lCount;
	int threadNum;
};

/*********************************************************** HELPER FUNCTIONS **********************************************************************/

//use this for loop count, based on mean from command line
int loopRand(int meanLoopCount)
{
  double a = ((rand() % 10000) / 10000.0);
  double b = ((rand() % 10000) / 10000.0);
  double randNum = ((sqrt(-2 * log(a))) * cos(2*M_PI*b));
  //printf("STATS: randNum Val: [%lf]\n", randNum);
  if ((int)(randNum + meanLoopCount) == 0)
  {
  	return 1;
  }
  else
  {
  	//printf("LOOPRAND: %i\n", (int)randNum+meanLoopCount);
  	return (int)(randNum + meanLoopCount);
  }
}


// rng for arrival
double normalRand(double mean)
{
  double a = ((rand() % 10000) / 10000.0);
  double b = ((rand() % 10000) / 10000.0);
  double randNum = ((sqrt(-2 * log(a))) * cos(2*M_PI*b))/10;
  return randNum * (mean/2) + mean;
}

/***************************************************** INDIVIDUAL (THREAD SIMULATOR) *************************************************************/

/* individual
 * The thread will run with the given specifications:
 *	g - a random num 0 or 1 that defines the gender of the thread
 *	arrival - the arrival time of the thread
 *	stay - the amount of time the thread will stay in the bathroom
 *	lCount - the number of times the thread will loop through the bathroom
 */
void *individual(void* arguments)
{
	srand(time(NULL));
	struct argstruct *args = arguments;
	long total = 0;
	long qTime = 0;
	long aveQueue = 0;
	long maxQueue = 0;
	long minQueue = 999999999; 

	/* Should wait for all threads to be created, is this too slow? (prolly not) */
	pthread_mutex_lock(&args->lock);
	int try = 0;
	god--;
	while(god>0)
	{
		pthread_mutex_lock(&args->lock);
		sched_yield();
		pthread_mutex_unlock(&args->lock);
	}
	if(try == 0)
	{
		try++;
		/* Assign random variables to the thread */
		//printf("INDIVIDUAL: assigning random values...\n");
		args->lCount = loopRand(args->lCount);
		args->arrival = normalRand(args->arrival);
		//printf("random arrival time: %lf\n", args->arrival);
		args->stay = normalRand(args->stay);
		//printf("random arrival time: %lf\n", args->stay);
	}
	pthread_mutex_unlock(&args->lock);
	/* try is so that while the threads are waiting for other threads to be created, 
	 *it can at least assign random variables to speed it up slightly */

	/* Loop through the lCount times and simulate entering and leaving a bathroom */
	for(int i = 0; i<args->lCount; i++)
	{
		usleep(args->arrival);
		//printf("INDIV: Thread[%d] Arrival Time Value: %lf MS\n", args->threadNum+1, args->arrival);
		qTime = enter(args->gender);
		printf("thread %i, %ld\n", args->threadNum+1,qTime);
		usleep(args->stay);
		//printf("INDIV: Thread[%d] Stay Time Value: %lf MS\n", args->threadNum+1, args->stay);
		leave();
		//printf("INDIV: Thread[%d] After leave\n", args->threadNum+1);
		if (qTime < minQueue)
		{
			minQueue = qTime;
		}
		else if (qTime > maxQueue)
		{
			maxQueue = qTime;
		}
		total += qTime;
	}
	aveQueue = total/args->lCount;
	/* print statistics (do not want more than one thread printing at once so lock) */
	pthread_mutex_lock(&args->printLock);
	printf("Thread #%i Completed!\n", args->threadNum+1);
	printStats(args->gender, args->threadNum, args->lCount, minQueue, aveQueue, maxQueue);
	pthread_mutex_unlock(&args->printLock);

	return 0;
}

/**************************************************************** MAIN **********************************************************************/

int main(int argc, char* argv[])
{
  /************************************************************* VARIABLES *********************************************************************/
  /* check if valid inputs */
  assert(argc == 5);
  /* assign inputs */
  int nUsers;
  int meanLoopCount;
  double meanArrival;
  double meanStay;
  
  nUsers = atoi(argv[1]);
  //printf("nUsers:[%i]\n", nUsers);
  meanLoopCount = atoi(argv[2]);
  //printf("meanLoopCount:[%i]\n", meanLoopCount);
  meanArrival = atof(argv[3]);
  //printf("meanArrival:[%f]\n", meanArrival);
  meanStay = atof(argv[4]);
  //printf("meanStay:[%f]\n", meanStay);
  
  initialize();
  printf("Threads Initialized!\n");
  
  /* create general mutex and condition variable */
  pthread_mutex_t lock;
  pthread_mutex_t printLock;
  pthread_mutex_t vacant;
  pthread_mutex_init(&lock, NULL);
  pthread_mutex_init(&printLock, NULL);
  pthread_mutex_init(&vacant, NULL);

  /* args will hold all the thread information, and will be used to pass parameters to individual */
  /* This may cause a lot of memory accesses and slow down the threads considerably... */
  /* If this doesn't work try making it an array instead of a pointer */

	/* god and godLock will ensure all threads are created together */
   pthread_mutex_t godLock;
   pthread_mutex_init(&godLock, NULL);
   int god = 0;

   pthread_mutex_lock(&godLock);
   pthread_t tid[nUsers];
   //tid[] = (pthread_t *)malloc(sizeof(pthread_t *) * nUsers);
   struct argstruct *args = (struct argstruct *)malloc(nUsers*sizeof(struct argstruct));
 
  /************************************************************ BEGIN SIMULATION *******************************************************************/  
  for(int i=0; i<nUsers; i++) 
  {
	//pthread_t thread;
	int gender = rand() & 1;
	args[i].gender = gender;
	args[i].printLock = printLock;
	args[i].vacant = vacant;
	args[i].lock = lock;
	args[i].arrival = meanArrival;
	args[i].stay = meanStay;
	args[i].lCount = meanLoopCount;
	args[i].threadNum = i;
	//printf("TN; %d\n", args->threadNum);

	/* CREATE THE THREADS */
	//printf("About to create thread #%i\n", i+1);
	god++; // increment god (number of threads created)
	pthread_create(&tid[i], NULL, &individual, (void*) &args[i]);
	printf("Thread #%i Created!\n", i+1);
  }

  pthread_mutex_unlock(&godLock);
 
/************************************************************** END SIMULATION **********************************************************************/
 
  for(int i = 0; i<nUsers; i++)
  {
		pthread_join(tid[i], NULL);
  }
  finalize();
  printf("\n");
  printf("Goodbye, Commander\n");
  printf("What? Too Soon?\n");
  return 0;
}
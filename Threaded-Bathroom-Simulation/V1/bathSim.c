/* bathSim
 * Andrew Morrison
 * Peter Christakos
 To Do:
 * thread statistics
 * finalize / master thread stats
 * 'exhaustive' tests
 */ 
#include "Bathroom.h"
#ifndef M_PI
	#define M_PI = #define M_PI 3.14159265358979323846
#endif

/* Struct to hold arguments for pthread_create */
struct argstruct
{
	pthread_mutex_t printLock;
	pthread_cond_t vacant;
	pthread_mutex_t lock;
	int gender;
	useconds_t arrival;
	useconds_t stay;
	int lCount;
	int threadNum;
};

//use this for loop count, based on mean from command line
int loopRand(int meanLoopCount){
  int rangeLow, rangeHigh, mean, stdDev, randNum;
  mean = meanLoopCount;
  stdDev = mean/2;
  rangeLow = mean - (3*stdDev);
  rangeHigh = mean + (3*stdDev);
  if (rangeLow < 0){
    rangeLow = 0;
  }
  randNum = (sqrt(-2 * log(rangeLow))) * cos(2*M_PI*rangeHigh);
  printf("random number generated with looprand: %d", randNum);
  //randNum = (rangeHigh * drand48());
  return randNum;
}

// rng for arrival
useconds_t arrivalRand(int meanArrival){
  int rangeLow, rangeHigh, mean, stdDev;
  mean = meanArrival;
  stdDev = mean/2;
  rangeLow = mean - (3*stdDev);
  rangeHigh = mean + (3*stdDev);
  if (rangeLow < 0){
    rangeLow = 0;
  }
  useconds_t randNum;
  randNum = (sqrt(-2 * log(rangeLow))) * cos(2*M_PI*rangeHigh);
  printf("random number generated with arrivalrand: %d", randNum);
  //randNum = (rangeHigh * drand48());
  return randNum;
}

// rng for stay
useconds_t stayRand(int meanStay){
  int rangeLow, rangeHigh, mean, stdDev;
  mean = meanStay;
  stdDev = mean/2;
  rangeLow = mean - (3*stdDev);
  rangeHigh = mean + (3*stdDev);
  if (rangeLow < 0){
    rangeLow = 0;
  }
  useconds_t randNum;
  randNum = (sqrt(-2 * log(rangeLow))) * cos(2*M_PI*rangeHigh);
  printf("random number generated with stayrand: %d", randNum);
  //randNum = (rangeHigh * drand48());
  return randNum;
}

/* individual
 * The thread will run with the given specifications:
 *	g - a random num 0 or 1 that defines the gender of the thread
 *	arrival - the arrival time of the thread
 *	stay - the amount of time the thread will stay in the bathroom
 *	lCount - the number of times the thread will loop through the bathroom
 */
//pthread_mutex_t printLock, pthread_cond_t vacant, pthread_mutex_t lock, int gender, long arrival, long stay, int lCount, int threadNum
void *individual(void* arguments)
{
	struct argstruct *args = arguments;
	long minQueue, aveQueue, maxQueue, brTime; // variables to keep track of thread statistics
	useconds_t arrival = arrivalRand(args->arrival);
    useconds_t stay = stayRand(args->stay);
    int loopcount = loopRand(args->lCount);
    int rc; // use for assert statements

	/* Wait for all threads to be created so they all start at the same time */
  	rc = pthread_mutex_lock(&args->lock);
	assert(rc == 0);
 	rc = pthread_cond_wait(&args->vacant, &args->lock);
	if (rc == 0) 
	{
		// wait
	}
	pthread_mutex_unlock(&args->lock);

	// loop for lCount
	for(int i = 0; i<args->lCount; i++)
	{
		// wait for arrival time
		usleep(args->arrival);

		while(1)
		{
			// check if can enter bathroom
			rc = pthread_mutex_lock(&args->lock);
			assert(rc == 0);
			if(enter(args->gender)==1) // if can enter
			{
				// go to the bathroom
				rc = pthread_mutex_unlock(&args->lock);
				assert(rc == 0);

				usleep(args->stay);

				rc = pthread_mutex_lock(&args->lock);
				assert(rc == 0);
				leave();

				/* QUESTION: How do we handle broadcast so that when it signals the other threads,
				 * the lock is freed from this thread, and all other threads can grab the lock */
				if(getGender() == -1) // Bathroom is vacant so signal threads
				{
					//rc = pthread_mutex_lock(&broad); // make sure broadcast and unlock happen at the same time... Is this necessary?

					rc = pthread_cond_broadcast(&args->vacant); // broadcast to all other threads
					assert(rc == 0);
					// what if we pause here?
					rc = pthread_mutex_unlock(&args->lock);
					assert(rc == 0);

					//rc = pthread_mutex_unlock(&broad); // is this necessary?
				}
				else
				{
					rc = pthread_mutex_unlock(&args->lock);
					assert(rc == 0);
					break;
				}
			}
			else
			{
				// wait for the bathroom to be vacant so one can enter
				rc = pthread_cond_wait(&args->vacant, &args->lock);
			}
		}
	}

  /* print statistics (do not want more than one thread printing at once so lock) */
  pthread_mutex_lock(&args->printLock);
  printStats(); // give shit
  pthread_mutex_unlock(&args->printLock);
  return 0;
}

/* main */
int main(int argc, char* argv[])
{
  //check if valid inputs
  assert(argc == 5);
  //assign inputs
  int nUsers = *argv[1];
  int meanLoopCount = *argv[2]; //avg number of times	user repeats user bathroom loop
  long meanArrival = *argv[3];
  long meanStay = *argv[4];
  
  initialize();
  
  // create general mutex and condition variable
  pthread_mutex_t lock;
  pthread_mutex_t printLock;
  pthread_cond_t vacant;
  pthread_mutex_init(&lock, NULL);
  pthread_mutex_init(&printLock, NULL);
  pthread_cond_init(&vacant, NULL);

  // create n threads
  struct argstruct *args = malloc(sizeof (struct argstruct));
	args->printLock = printLock;
	args->vacant = vacant;
	args->lock = lock;
	args->arrival = meanArrival;
	args->stay = meanStay;
	args->lCount = meanLoopCount;
  
  pthread_t tid[nUsers];  
  for(int i=0; i<nUsers; i++) 
  {
	//pthread_t thread;
	int gender = rand() & 1;
	args->gender = gender;
	args->threadNum = i;
	// create a thread
	pthread_create(&tid[i], NULL, &individual, (void*) args);
  }
  
  /* broadcast threads to start execution */
  pthread_cond_broadcast(&vacant);
  
  /* Join all threads back together */
  for(int i = 0; i<nUsers; i++)
  {
		pthread_join(tid[i], NULL);
  }
  
  finalize(); // print master thrread statistics
  return 0;
}
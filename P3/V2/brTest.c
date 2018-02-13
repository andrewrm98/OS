/* Bathroom.c tester */

#include "Bathroom.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>

//use this for loop count, based on mean from command line
int loopRand(int meanLoopCount){
  double a = ((rand() % 10000) / 10000.0);
  double b = ((rand() % 10000) / 10000.0);
  double randNum = ((sqrt(-2 * log(a))) * cos(2*M_PI*b));
  //printf("STATS: randNum Val: [%lf]\n", randNum);
  return randNum + meanLoopCount;
}

// rng for arrival
double arrivalRand(double meanArrival){
  double a = ((rand() % 10000) / 10000.0);
  double b = ((rand() % 10000) / 10000.0);
  double randNum = ((sqrt(-2 * log(a))) * cos(2*M_PI*b))/10;
  return randNum + meanArrival;
}

// rng for stay
double stayRand(double meanStay){
  double a = ((rand() % 10000) / 10000.0);
  double b = ((rand() % 10000) / 10000.0);
  double randNum = ((sqrt(-2 * log(a))) * cos(2*M_PI*b))/10;
  return randNum + meanStay;
}


int main()
{
	srand(time(NULL));
	printf("~~~~~~~~~~~~~~~LETS TEST SOME SHIT~~~~~~~~~~~~~~~\n");
	// test the initialize function
	initialize();
/*
	printf("\ninitialize of brGlobal completed...\n");
	printf("BRGLOBAL INFO -> GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());

	//test enter and leave functions
	printf("\n~~~TESTING ENTER AND LEAVE FUNCTIONS~~~\n");
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	enter(0);
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	
	enter(0);
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	
	leave();
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	
	//brGlobal->gender = 0;

	enter(0);
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	
//	brGlobal->gender = 1;
	enter(1);
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	
	leave();
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	
	leave();
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	
	leave();
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
	
	enter(0);
	printf("GF: %i, MC: %i, FC: %i, TU, %i\n", getGender(), getMCount(), getFCount(), getTotalU());
*/
	//test random functions
	printf("\n~~~TESTING LOOPRAND FUNCTION~~~\n");
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 6, loopRand(6));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 6, loopRand(6));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 6, loopRand(6));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 6, loopRand(6));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 10, loopRand(10));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 15, loopRand(15));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 20, loopRand(20));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 25, loopRand(25));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 30, loopRand(30));
    
    printf("\n TESTING ARRIVAL/STAY RAND FUNCTIONS\n");
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.1, stayRand(0.1));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.2, stayRand(0.2));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.3, stayRand(0.3));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.4, stayRand(0.4));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.5, stayRand(0.5));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.6, stayRand(0.6));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.7, stayRand(0.7));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.8, stayRand(0.8));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 0.9, stayRand(0.9));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 1.1, stayRand(1.1));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 1.5, stayRand(1.5));
    printf("Successfully ran stayRand function with input: [%f] and output: [%lf]\n", 1.9, stayRand(1.9));
	
	return 0;

}
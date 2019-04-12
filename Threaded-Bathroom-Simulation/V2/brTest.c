/* Bathroom.c tester */

#include "Bathroom.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>

int loopRand(int meanLoopCount)
{
  //double a = ((rand() % 10000) / 10000.0);
  //double b = ((rand() % 10000) / 10000.0);
  double drand48();
  double a = drand48();
  double b = drand48();
  double randNum = ((sqrt(-2 * log(a))) * cos(2*M_PI*b));
  if ((int)(randNum * (meanLoopCount/2 ) + meanLoopCount) <= 0)
  {
  	return 1;
  }
  else
  {
  	return (int)(randNum *(meanLoopCount/2)+ meanLoopCount);
  }
}


// rng for arrival
double normalRand(double mean)
{
  double drand48();
  double a = drand48();
  double b = drand48();
  double randNum = ((sqrt(-2 * log(a))) * cos(2*M_PI*b))/10; //deadlock?
  return randNum * (mean/2) + mean;
}

int main()
{
	srand48(time(NULL));
	printf("\n~~~TESTING LOOPRAND FUNCTION~~~\n");
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 0, loopRand(0));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 0, loopRand(0));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 0, loopRand(0));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 5, loopRand(5));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 6, loopRand(6));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 6, loopRand(6));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 6, loopRand(6));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 6, loopRand(6));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 25, loopRand(25));
	printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 25, loopRand(25));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 25, loopRand(25));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 200, loopRand(200));
    printf("Successfully ran loopRand function with input: [%d] and output: [%i]\n", 200, loopRand(200));
    
    printf("\n TESTING ARRIVAL/STAY RAND FUNCTIONS\n");
    printf("Successfully ran stayRand function with input: [%i] and output: [%lf]\n", 2, normalRand(1));
    printf("Successfully ran stayRand function with input: [%i] and output: [%lf]\n", 2, normalRand(2));
    printf("Successfully ran stayRand function with input: [%i] and output: [%lf]\n", 2, normalRand(2));
    printf("Successfully ran stayRand function with input: [%i] and output: [%lf]\n", 2, normalRand(2));
    printf("Successfully ran stayRand function with input: [%lf] and output: [%lf]\n", 5.23, normalRand(5.23));
    printf("Successfully ran stayRand function with input: [%lf] and output: [%lf]\n", 6.45, normalRand(6.45));
    printf("Successfully ran stayRand function with input: [%lf] and output: [%lf]\n", 7.1, normalRand(7.1));
    printf("Successfully ran stayRand function with input: [%lf] and output: [%lf]\n", 8.2, normalRand(8.2));
    printf("Successfully ran stayRand function with input: [%i] and output: [%lf]\n", 9, normalRand(9));
    printf("Successfully ran stayRand function with input: [%lf] and output: [%lf]\n", 10.1, normalRand(10.1));
    printf("Successfully ran stayRand function with input: [%lf] and output: [%lf]\n", 11.5, normalRand(11.5));
    printf("Successfully ran stayRand function with input: [%lf] and output: [%lf]\n", 12.9, normalRand(12.9));
	
	return 0;

}
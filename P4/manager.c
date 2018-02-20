/*
 * Andrew Morrison
 * Peter Christakos
 * Project4 Manager.c Source Code
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// global guys
char memory[64]; //memory

// functions to make
int map (int process, char* instruction, int address, int value) { return 0; }
int store (int process, char* instruction, int address, int value) { return 0; }
int load (int process, char* instruction, int address, int value) { return 0; }

int main(int argc, char **argv)
{
	char *userInput[4]; 
 	int process_id;
	char* instruction_type;
	int virtual_address;
	int value;

   	while(!feof(stdin))
  	{
		// tokenize first input
   		char argsArr[4]; //store arguments in array
    	char * args = argsArr; // make pointer to array
    	const char s[2] = ","; // token to check for
    	char *token; // token creation

   		/* tokenize first input */
    	printf("Instruction? ");
    	fgets(args,32,stdin); 
    	token = strtok(args, s);
    	if (!token) { printf("ERROR: Invalid Input\n"); return 1; }
    	userInput[0] = token;
    	printf("userInput[0]: %s\n", token);


    	/* tokenize rest of inputs */
 		int i = 1;
    	while(i < 4) 
    	{
    		if (!token) { printf("ERROR: Invalid Input\n"); return 1; }
    		//printf("looping\n");
    		token = strtok(NULL, s);
    		userInput[i] = token;
    		printf("userInput[%i]: %s\n", i, token);
    		i++;
    	}

    	/* assign tokenized values */
    	process_id = atoi(userInput[0]);
		instruction_type = userInput[1];
		virtual_address = atoi(userInput[2]);
		value = atoi(userInput[3]);
		printf("process_id: %i\n instruction_type: %s\n virtual_address: %i\n value: %i\n", process_id, instruction_type, virtual_address, value);
  }
	return 1;
}
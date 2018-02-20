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

void map (int process, char* instruction, int address, int value);
void store (int process, char* instruction, int address, int value);
void load (int process, char* instruction, int address, int value);

// functions to make
void map (int process, char* instruction, int address, int value) { printf("Oh yea... the map function doesnt do shit\n"); } 
void store (int process, char* instruction, int address, int value) { printf("Oh yeas... the store function doesnt do shit\n"); }
void load (int process, char* instruction, int address, int value) { printf("Oh yea... the load function doesnt do shit\n"); }

/* runs selected instruction */
void masterFunction (int process_id, char* instruction_type, int virtual_address, int value) 
{
	if (!(strcmp(instruction_type, "map"))) 
	{ 
		map(process_id, instruction_type, virtual_address, value);
	}
	if (!(strcmp(instruction_type, "load"))) 
	{ 
		load(process_id, instruction_type, virtual_address, value);
	}
	if (!(strcmp(instruction_type, "store"))) 
	{ 
		store(process_id, instruction_type, virtual_address, value);
	}
}

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
    	if (!token) { printf("ERROR: Invalid Input\n"); return -1; } // check for null token
    	token = strtok(args, s);
    	userInput[0] = token;
    	printf("userInput[0]: %s\n", token);

    	/* tokenize rest of inputs */
 		int i = 1;
    	while(i < 4) 
    	{
    		token = strtok(NULL, s);
    		if (!token) { printf("ERROR: Invalid Input\n"); return -1; } // check for null token
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
  		masterFunction(process_id, instruction_type, virtual_address, value);	
	}
	return 1;
}
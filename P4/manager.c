/*
 * Andrew Morrison
 * Peter Christakos
 * Project4 manager.c Source Code
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/********************************************************* STRUCTS/ARRAYS ********************************************************************/

/* struct to assign bit size of each */
typedef struct {
    u_int8_t presentBit:1; //1 if present, 0 if on disk
    u_int8_t validBit:1; //1 if valid
    u_int8_t value:1; //1 if can be written, 0 if read-only
    u_int8_t page:3; //page number ie frame
    u_int8_t processId:4; //PID
    u_int8_t pageNum:4; //Location in memory
    u_int8_t instruction:2;
} pageEntry;


unsigned char memory[64]; //memory
pageEntry pageTable[5];

/********************************************************* FUNCTION DECLARATIONS *************************************************************/

int map (int process, char* instruction, int address, int value);
int store (int process, char* instruction, int address, int value);
int load (int process, char* instruction, int address, int value);
void initialize(); // init the page table

/******************************************************** HELPER FUNCTIONS *******************************************************************/
/* runs selected instruction */
void masterFunction (int process, char* instruction, int address, int value) 
{
	if (!(strcmp(instruction, "map")))        { map(process, instruction, address, value);   }
	else if (!(strcmp(instruction, "load")))  { load(process, instruction, address, value);  }
	else if (!(strcmp(instruction, "store"))) { store(process, instruction, address, value); }
	else { printf("ERROR: You Specified an Invalid Instruction\n"); }
}

void initialize()
{
	for (int i = 0; i < 4; i++) {
	        pageTable[i].validBit = 0;
	        pageTable[i].presentBit = 0;
	        pageTable[i].value = 0;
	        pageTable[i].page = -1;
	        pageTable[i].pageNum = 0;
	        pageTable[i].processId = i;
	    }
}
  
/*************************************************** INSTRUCTION FUNCTIONS *******************************************************************/

/* map tells the memory manager to allocate a physical page, i.e., it creates a mapping in the page table
between a virtual and physical address. The manager must determine the appropriate virtual page
number using the virtual address. For example, virtual address of 3 corresponds to virtual page
0. value argument represents the write permission for the page. If value=1 then the page is writeable
and readable. If value=0, then the page is only readable, i.e., all mapped pages are readable. These
permissions can be modified by using a second map instruction for the target page. */
int map (int process, char* instruction, int address, int value) { printf("Oh yea... the map function doesnt do shit\n"); return 1; } 

/* store instructs the memory manager to write the supplied value into the physical memory location
associated with the provided virtual address, performing translation and page swapping as necessary.
Note, page swapping is a requirement for part 2 only. */
int store (int process, char* instruction, int address, int value) { printf("Oh yeas... the store function doesnt do shit\n"); return 1; }

/* load instructs the memory manager to return the byte stored at the memory location specified by
virtual address. Like the store instruction, it is the memory manager’s responsibility to translate
and swap pages as needed. Note, the value parameter is not used for this instruction, but a dummy
value (e.g., 0) should always be provided. */
int load (int process, char* instruction, int address, int value) { printf("Oh yeas... the store function doesnt do shit\n"); return 1; }

/*********************************************************** MAIN ****************************************************************************/
int main(int argc, char **argv)
{
	initialize(); // initilialize structs
	char *userInput[4]; 
 	int process; //pid
	char* instruction; //instruction type
	int address; // virtual address
	int value; // value

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
    	//printf("userInput[0]: %s\n", token);

    	/* tokenize rest of inputs */
 		int i = 1;
    	while(i < 4) 
    	{
    		token = strtok(NULL, s);
    		if (!token) { printf("ERROR: Invalid Input\n"); return -1; } // check for null token
    		userInput[i] = token;
    		//printf("userInput[%i]: %s\n", i, token);
    		i++;
    	}

    	/* assign tokenized values */
    	process_id = atoi(userInput[0]);
		instruction_type = userInput[1];
		virtual_address = atoi(userInput[2]);
		value = atoi(userInput[3]);
		//printf("\tprocess: %i\n\tinstruction: %s\n\taddress: %i\n\tvalue: %i\n", process_id, instruction_type, virtual_address, value);
  		masterFunction(process, instruction, virtual, value);	
	}
	return 1;
}
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

int map (int pid, char* instruction, int address, int value); //finds a place in memory for a process
int store (int pid, char* instruction, int address, int value);
int load (int pid, char* instruction, int address, int value);
void modifyTable(int presentBit, int validBit, int value, int page, int pid, int pageNum); // enter info into the page table
void masterFunction(int pid, char* instruction, int address, int value); // runs selected instruction
void initialize(); // initializes pageTable

/******************************************************** HELPER FUNCTIONS *******************************************************************/
/* runs selected instruction */
void masterFunction (int process, char* instruction, int address, int value) 
{
	if (!(strcmp(instruction, "map")))        { map(process, instruction, address, value);   }
	else if (!(strcmp(instruction, "load")))  { load(process, instruction, address, value);  }
	else if (!(strcmp(instruction, "store"))) { store(process, instruction, address, value); }
	else { printf("ERROR: You Specified an Invalid Instruction\n"); }
}

void modifyTable(int presentBit, int validBit, int value, int page, int pid, int pageNum)
{
	pageTable[pid].presentBit = presentBit;
	pageTable[pid].validBit = validBit;
    pageTable[pid].value = value;
    pageTable[pid].page = page;
    pageTable[pid].pageNum = pageNum;
}

void initialize() 
{
	for (int i = 0; i<4; i++) { modifyTable(0, 0, 0, -1, i, 0); } // initialize every page entry
}
  
/*************************************************** INSTRUCTION FUNCTIONS *******************************************************************/

/* map tells the memory manager to allocate a physical page, i.e., it creates a mapping in the page table
between a virtual and physical address. The manager must determine the appropriate virtual page
number using the virtual address. For example, virtual address of 3 corresponds to virtual page
0. value argument represents the write permission for the page. If value=1 then the page is writeable
and readable. If value=0, then the page is only readable, i.e., all mapped pages are readable. These
permissions can be modified by using a second map instruction for the target page. */
int map (int pid, char* instruction, int address, int value) 
{ 
	int vacantLoc = -1; // variable to hold value of vacant spot in page table
	/* loop through first byte of every page to find empty page */
	for (int i = 0; i < 4; i++) 
	{ 
		if (!memory[i*16]) { vacantLoc = i; break; } // returns pageNumber of empty entry in memory (every page entry on 16th byte)
	} 
	if (vacantLoc != -1)
	{
		modifyTable(1, 1, value, vacantLoc, pid, vacantLoc);
        memory[vacantLoc*16] = vacantLoc; //sets vacantLoc to memory
        printf("Put page table for PID %d into physical frame %d\n\n", pid, vacantLoc);
	}
	else { printf("ERROR: No Empty Space in Page Table!"); return -1;} // swap will go here at some point
	return 1;
} 

/* store instructs the memory manager to write the supplied value into the physical memory location
associated with the provided virtual address, performing translation and page swapping as necessary.
Note, page swapping is a requirement for part 2 only. */
int store (int pid, char* instruction, int address, int value) { printf("Oh yeas... the store function doesnt do shit\n"); return 1; }

/* load instructs the memory manager to return the byte stored at the memory location specified by
virtual address. Like the store instruction, it is the memory manager’s responsibility to translate
and swap pages as needed. Note, the value parameter is not used for this instruction, but a dummy
value (e.g., 0) should always be provided. */
int load (int pid, char* instruction, int address, int value) { printf("Oh yeas... the store function doesnt do shit\n"); return 1; }

/*********************************************************** MAIN ****************************************************************************/
int main(int argc, char **argv)
{
	initialize(); // initialize page table
	char *userInput[4]; 
 	int pid; //pid
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
    	pid = atoi(userInput[0]);
		instruction = userInput[1];
		address = atoi(userInput[2]);
		value = atoi(userInput[3]);
  		masterFunction(pid, instruction, address, value);	
	}
	return 1;
}
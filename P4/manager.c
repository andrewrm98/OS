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
typedef struct 
{
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

int findPage(int address)
{
	int page; 

	if (address < 16)      { page = 0; }
 	else if (address < 32) { page = 1; }
 	else if (address < 48) { page = 2; }
 	else if (address < 64) { page = 3; }
 	return page;
}

  
/*************************************************** INSTRUCTION FUNCTIONS *******************************************************************/

/* searhces through memory array to allocate a location for an entry. Up to 4 can be made before a swap must be performed */ 
int map (int pid, char* instruction, int address, int value) 
{ 
	int vacantLoc = -1; // variable to hold value of vacant spot in page table
	int pnum = findPage(address); // used get page num from address
	
	/* loop through first byte of every page to find empty page */
	for (int i = 0; i < 4; i++) 
	{ 
		if (!memory[i*16]) 
		{ 
			vacantLoc = i; 
			break; 
		} 
	} 
	if (vacantLoc != -1)
	{
		modifyTable(1, 1, value, vacantLoc, pid, pnum);
        memory[vacantLoc*16] = vacantLoc; //sets vacantLoc to memory
        printf("Put page table for PID %d into physical frame %d\n\n", pid, pnum);
	}
	else 
	{   // swap will go here at some point
		printf("ERROR: No Empty Space in Page Table!"); 
		return -1;
	} 
	return 1;
} 

/* write value into memory location */
int store (int pid, char* instruction, int address, int value) { printf("Oh yea... the store function doesnt do shit\n"); return 1; }

/* return byte stored at memory locatoin */
int load (int pid, char* instruction, int address, int value) { printf("Oh yea... the load function doesnt do shit\n"); return 1; }

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
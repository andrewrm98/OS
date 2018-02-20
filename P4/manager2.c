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
    u_int8_t presentBit:1; 	//1 if present, 0 if on disk
    u_int8_t validBit:1; 	//1 if valid
    u_int8_t value:1; 		//1 if can be written, 0 if read-only
    u_int8_t page:1; 		//page number ie frame
} pageEntry;

typedef struct {
	int valid;
	int ptLoc;
} reg;

typedef struct {
	int valid;
	int values[4];
} page;

unsigned char memory[64]; 	// memory
reg ptRegister[4];	        // the page table registers
int freeTable[4] = {1,1,1,1};  // table of free physical frames... 1 for free, 0 for taken

/********************************************************* FUNCTION DECLARATIONS *************************************************************/

int map (int pid, int address, int value);                                                          //finds a spot in mem for a process
int store (int pid, int address, int value);
int load (int pid, int address, int value);
void modifyTable(pageEntry * currTable, int presentBit, int validBit, int value, int page, int id); // enter in  the page table
void masterFunction(int pid, char * instruction, int address, int value);                           // runs selected instruction
void initialize(pageEntry * currTable);                                                             // initializes pageTable

/******************************************************** HELPER FUNCTIONS *******************************************************************/
/* runs selected instruction */
void masterFunction (int process, char * instruction, int address, int value) 
{
	if (!(strcmp(instruction, "map")))        	{ map(process, address, value);   }
	else if (!(strcmp(instruction, "load")))  	{ load(process, address, value);  }
	else if (!(strcmp(instruction, "store"))) 	{ store(process, address, value); }
	else { printf("ERROR: You Specified an Invalid Instruction\n"); }
}

void modifyTable(pageEntry * currTable, int presentBit, int validBit, int value, int page, int id) //, int pageNum)
{
	currTable[id].presentBit = presentBit;
	currTable[id].validBit = validBit;
    currTable[id].value = value;			            // not indexing by pid anymore, index by virtualFrame #
    currTable[id].page = page;
}

void initialize(pageEntry * currTable) 
{
	for (int i = 0; i<4; i++) { modifyTable(currTable, 0, 0, -1, -1, i); } // initialize every page entry
}

int findFree()
{
	for(int i = 0; i<4; i++) { 
		if(freeTable[i] == 1) { freeTable[i] = 0; return i; }
	}
	return -1; 
}

int checkLoc(page currPage, int i)
{
	if(currPage.values[i] == -1) { return i; }			// check if the page is open
	else { return -1; }									// return -1 if the page is full	
}

void printMem()
{
	for (int i = 0; i < 64; i++)
	{
		if (memory[i]) { printf("Value at memory[%i]: %c\n", i, memory[i]); }
	}
}
  
/*************************************************** INSTRUCTION FUNCTIONS *******************************************************************/

/* map tells the memory manager to allocate a physical page, i.e., it creates a mapping in the page table
between a virtual and physical address. The manager must determine the appropriate virtual page
number using the virtual address. For example, virtual address of 3 corresponds to virtual page
0. value argument represents the write permission for the page. If value=1 then the page is writeable
and readable. If value=0, then the page is only readable, i.e., all mapped pages are readable. These
permissions can be modified by using a second map instruction for the target page. */
int map (int pid, int address, int value) 
{ 
	int virtualFrame = address/16; // virtual address frame
	pageEntry currTable[4];
	page currPage;
	initialize(currTable);
	int i;
	for(i = 0; i<4; i++) { currPage.values[i] = -1; }
	currPage.valid = -1;
	int physicalFrame;

	/*$$$ Get the Page Table Loaded $$$*/
	if(ptRegister[pid].valid == 1 && ptRegister[pid].ptLoc != -1) // if the table has been created
	{
		memcpy(&currTable, &memory[ptRegister[pid].ptLoc], 16);			// load the page table
		printf("Got page table for PID [%d] from physical address [%d]\n\n", pid, memory[ptRegister[pid].ptLoc]);
	}
	else if(ptRegister[pid].valid == 1 && ptRegister[pid].ptLoc == -1) // if the table is created but not in memory
	{
		/* Perform swapping */
		printf("Swapping not implemented yet...1\n");
	}	
	else																												// create a new page table for the new process
	{
		ptRegister[pid].valid = 1;																// this process now has a page table
		ptRegister[pid].ptLoc = findFree();												// set to its location in physical memory
		printf("value: %i\n", ptRegister[pid].ptLoc);

		if(ptRegister[pid].ptLoc != -1) 
		{ 
			memcpy(&memory[ptRegister[pid].ptLoc*16], &currTable, 16); 
			printf("New page table created and stored at memory location [%d]\n", ptRegister[pid].ptLoc*16);
		} // store the page table in memory
		else
		{
			/* Perform swapping */
			printf("This space is full...I should run the swap fn\n");
		}
	}

	/*$$$ Get the page loaded into memory $$$*/
	if((i = currTable[virtualFrame].validBit) == 0) { /* do nothing */ }	// if the requested virtual space is not in use
	else { i = -1; }

	if(i != -1) 												// if there is space in the page table
	{
		printf("There is space in the page table\n");
		if((physicalFrame = findFree()) != -1)					// find free spot in physical memory
		{
			modifyTable(currTable, 1, 1, value, physicalFrame, virtualFrame);	// add the new values for this PTE
			currPage.valid = 1;
			memcpy(&memory[physicalFrame*16], &currPage, 16);			// store the new page in physical memory
			printf("Virtual address space updated\n");
			printf("New page stored in phyiscal frame [%d]\n", physicalFrame);
		}
		else
		{
			/* Perform swapping */
			printf("Swapping not implemented yet...3\n");
		}

	}
	else { 
		printf("No room in virtual memory for process %d\n", pid); 
		return 1;
	} // no space in virtual memory for this process
	return 0;
} 


/* store instructs the memory manager to write the supplied value into the physical memory location
associated with the provided virtual address, performing translation and page swapping as necessary.
Note, page swapping is a requirement for part 2 only. */

int store (int pid, int address, int value) { 
	pageEntry currTable[4];
	int virtualAddress;
	//int offset;
	int i = 0;
	page currPage;
	for(i = 0; i<4; i++) { currPage.values[i] = -1; }
	currPage.valid = -1;
	int pg = 0;

	/* Check if the address is valid */
	if (ptRegister[pid].valid == 1)
	{
		/*$$$ Get a Page Table Loaded $$$ */
		memcpy(&currTable, &memory[ptRegister[pid].ptLoc], 16);		// load the page table
		if(currTable == NULL) {
			printf("ERROR: Invalid load\n");
			return -1;
		}

		if(i != -1)
		{
			virtualAddress = address%16;
			//offset = address%4;						      			// this assumes the input is an integer

			if((pg = currTable[virtualAddress].page) == -1) 
			{ 
				printf("ERROR: No room in processes [%d] virtual memory\n", pid); 
			}
			else         											// we have a place to store
			{
				memcpy(&currPage, &memory[pg*16], 16); 				// loads the page
				if((pg = checkLoc(currPage, virtualAddress)) -1)
				{
					currPage.values[pg] = value;			    	// load the value
				}
				else
				{
					printf("ERROR: No more memory available\n");
				}
			}
		}
		else
		{
			printf("ERROR: MAXIMUM PROCESSES EXCEEDED\n");
			return -1;
		}
	}
	else
	{
		/* Perform swapping */
		printf("Swapping not implemented yet...\n");
	}
	return 0;
}
/* load instructs the memory manager to return the byte stored at the memory location specified by
virtual address. Like the store instruction, it is the memory manager’s responsibility to translate
and swap pages as needed. Note, the value parameter is not used for this instruction, but a dummy
value (e.g., 0) should always be provided. */
int load (int pid, int address, int value) { printf("Oh yeas... the store function doesnt do shit\n"); return 1; }


/*********************************************************** MAIN ****************************************************************************/
int main(int argc, char **argv)
{
	char *userInput[4]; 
 	int pid; //pid
	char* instruction; //instruction type
	int address; // virtual address
	int value; // value
	for(int i=0; i<4; i++) { 
		ptRegister[i].valid=0;
		ptRegister[i].ptLoc=0;
	}

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
  		printMem();
	}
	return 1;
}
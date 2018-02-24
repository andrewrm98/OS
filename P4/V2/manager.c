/*
 * Andrew Morrison
 * Peter Christakos
 * Project4 manager.c Source Code
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/********************************************************* STRUCTS/ARRAYS ********************************************************************/

/* struct to assign bit size of each */
typedef struct {
    unsigned char presentBit; 	//1 if present, 0 if on disk
    unsigned char validBit; 	//1 if valid
    unsigned char value; 		//1 if can be written, 0 if read-only
    unsigned char page; 		//page number ie frame
} pageEntry;					// page tables are indexed by their virtual frame

typedef struct {				// Holds a resgister to point to a page table
	int valid;					// Is the page table valid
	int ptLoc;					// Where is the page table.. -1 for on disc
} reg;

typedef struct {				// holds a page of 16 unsigned chars
	unsigned char pid;			// pid of the calling process
	unsigned char values[15];	// each array element is like an "offset" within the page
} page;

unsigned char memory[64]; 		// memory
reg ptRegister[4];	       		// the page table registers, indexed by pid
int freeTable[4] = {1,1,1,1};  	// table of free physical frames... 1 for free, 0 for taken by page, 2 if taken by page table
unsigned char processLord[4];	// Keeps track of pids in physical memory for reference

static const char filename[] = "swapFile.txt";

/********************************************************* FUNCTION DECLARATIONS *************************************************************/

int map (int pid, int address, int value);                                                          //finds a spot in mem for a process
int store (int pid, int address, int value);														// stores given value at address in a page with pid.
int load (int pid, int address, int value);															// loads a value at address in a page with pid (value is meaningless)
pageEntry * modifyTable(pageEntry * currTable, int presentBit, int validBit, int value, int page, int id); // enter in  the page table
void masterFunction(int pid, char * instruction, int address, int value);                           // runs selected instruction
void initialize(pageEntry * currTable);                                                             // initializes pageTable
int findFree();                                                                                     // finds free loc in mem
int checkLoc();                                                                                     // checks if page location is open
void printTable(pageEntry* pageTable, int i);
void printPage(page currPage);
void printMem();																					// prints non-null values in memory array
/******************************************************** HELPER FUNCTIONS *******************************************************************/
/* runs selected instruction */
void masterFunction (int process, char * instruction, int address, int value) 
{
	if (!(strcmp(instruction, "map")))        	{ map(process, address, value);   }
	else if (!(strcmp(instruction, "load")))  	{ load(process, address, value);  }
	else if (!(strcmp(instruction, "store"))) 	{ store(process, address, value); }
	else if (!(strcmp(instruction, "print"))) 	{ printMem(); }
	else { printf("ERROR: You Specified an Invalid Instruction\n"); }
}

/* modifies the given table with the given information */
pageEntry* modifyTable(pageEntry * currTable, int presentBit, int validBit, int value, int page, int id)
{
	pageEntry * editedTable = malloc(4*sizeof(pageEntry));

	for(int i=0; i<4; i++)
	{
		editedTable[i].presentBit = currTable[i].presentBit;
		editedTable[i].validBit = currTable[i].validBit;
		editedTable[i].value = currTable[i].value;
		editedTable[i].page = currTable[i].page;
	}
	editedTable[id].presentBit = presentBit;
	editedTable[id].validBit = validBit;
    editedTable[id].value = value;			          						 // not indexing by pid anymore, index by virtualFrame #
    editedTable[id].page = page;
    return editedTable;
}

/* initializes the given table */
void initialize(pageEntry * currTable) 
{
	for (int i = 0; i<4; i++) { modifyTable(currTable, 0, 0, -1, -1, i); } // initialize every page entry
}

/* finds a free page in physical memory */
int findFree()
{
	for(int i = 0; i<4; i++) { 
		if(freeTable[i] == 1) { return i; }
	}
	return -1; 
}

/* checks the location at the offset (i) within the given page */
int checkLoc(page* currPage, int i)
{
	if(currPage->values[i] == '\0') { return i; }								// check if the page is open
	else { return -1; }														// return -1 if the page is full	
}

/* prints out a page table */
void printTable(pageEntry* pageTable, int i)
{
	printf("\tPage Table for process [%d]\n", processLord[i]);
	for(int i = 0; i< 4; i++)
	{
		if(pageTable[i].validBit == 1)
		{
			printf("\tPage [%d]\n", i);
			printf("\t\tPresent: %d\n", pageTable[i].presentBit);
			printf("\t\tValid: %d\n", pageTable[i].validBit);
			printf("\t\tValue: %d\n", pageTable[i].value);
			printf("\t\tPage: %d\n", pageTable[i].page);
		}
		else
		{
			printf("\tPage entry [%d] not valid\n", i);
		}
	}
}

/* prints out a page */
void printPage(page currPage)
{
	printf("\tPage for process: %d\n", currPage.pid);
	for(int i = 0; i<11; i++)
	{
		if(currPage.values[i] != '\0')
			printf("\t\tPage value [%d]: %d\n", i, currPage.values[i]);
	}
}

/* prints out the memory array */
void printMem()
{
	pageEntry currTable[4];
	initialize(currTable);
	//int pid = 0;
	page currPage;
	for(int i = 0; i<15; i++) { currPage.values[i] = '\0'; }

	for (int i = 0; i < 4; i++)
	{
		printf("[%d]\n", i);
		if(freeTable[i] != 1)
		{
			if(freeTable[i] == 2)
			{	
				memcpy(&currTable, &memory[i*16], 16);
				printTable(currTable, i);
			}
			else if (freeTable[i] == 0)
			{
				memcpy(&currPage, &memory[i*16], 16);
				printPage(currPage);
			}
		}
		else
		{
			printf("\tEmpty page\n");
		}

	}
}

/* Returns a physical frame to evict using a random strategy */
int findPageToEvict() {
 	int evictionNotice = rand() %4; 
 	//printf("Eviction notice in evict: %d\n", evictionNotice);
 	return evictionNotice; 
}

/* finds the virtual frame that macthes the physicalFrame */
int findVirtualFrame(pageEntry *pageTable, int physicalFrame)
{
	for(int i = 0; i<4; i++)
	{
		if (pageTable[i].page == physicalFrame)
		{
			return i;
		}
	}
	return -1;
}

  
/*************************************************** MAJOR INSTRUCTIONS *******************************************************************/

/****** SWAP ******/

int swapOut() // , int target)
{
	// 1 - find a page to evict
	// 2 - evict that page 
	// 3 - put the desired page in the page table

	int tries = 0;
	int evictionNotice = 0;
	pageEntry swapTable[4];
	page swapPage;
	unsigned char swap[16];
	int thisId;
	int i = 0;
	pageEntry evictedTable[4];
	int evictedVirtualFrame;			

	/* Find a page to evict, make sure pages are evicted before their page tables */
	while(1)																					
	{
		evictionNotice = findPageToEvict();
		if(evictionNotice < 0)
		{
			continue;
		}
		if(freeTable[evictionNotice] == 2)
		{
			//printf("Free table: %d\n", freeTable[evictionNotice]);
			memcpy(&swapTable, &memory[evictionNotice*16], 16);					// load the evicted page table or
			//printf("Hello\n");
			for(i = 0; i<4; i++)
			{
				//printf("Hello2\n");
				if(processLord[i] == processLord[evictionNotice] && i != evictionNotice)
				{
					//printf("found a page\n");
					i = -1;
					break;
				}
				//printf("Not this page\n");
			}
			if(i!=-1) { 
				//printf("got inside tho\n");
				memcpy(&swap, &swapTable, 16); 
				ptRegister[processLord[evictionNotice]].ptLoc = -1;
				freeTable[evictionNotice] = 1;
				//printf("Eviction notice: %d\n", evictionNotice);

				/* Write swap to file*/
				FILE* hdd;
				hdd = fopen(filename, "a");
				fprintf(hdd, "%d", memory[(evictionNotice*16)]);

				for(int i = 1; i < 16; i++){
					fprintf(hdd, " %d", memory[(evictionNotice*16) + i]);
				}
				fprintf(hdd, "\n");
				fclose(hdd);
				printf("Page table for process [%d] swapped out.\n", processLord[evictionNotice]);
				processLord[evictionNotice] = -1;
				return evictionNotice;
			}
			tries++;
		}
		else																// load the evicted page
		{
			printf("Free table: %d\n", freeTable[evictionNotice]);
			/* Copy the page to evict into swapPage */
			memcpy(&swapPage, &memory[evictionNotice*16], 16);
			thisId = processLord[evictionNotice];
			//printf("Eviction notice: %d\n", evictionNotice);
			//printf("PID&&&&&: %d\n", thisId);
			/* Copy the page table to evict into evictedTable */
			memcpy(&evictedTable, &ptRegister[thisId].ptLoc, 16);					// page table is guaranteed to be in memory
			evictedVirtualFrame = findVirtualFrame(evictedTable, evictionNotice);
			/* Copy swapPage into swap */
			memcpy(&swap, &swapPage, 16); 
			evictedTable[evictedVirtualFrame].presentBit = 0;
			evictedTable[evictedVirtualFrame].page = -1;
			/* Updating evicted page's table */
			memcpy(&memory[ptRegister[thisId].ptLoc*16], &evictedTable, 16);
			freeTable[evictionNotice] = 1;
			/* Write swap to file*/
			FILE* hdd;
			hdd = fopen(filename, "a");
			fprintf(hdd, "%d", memory[(evictionNotice*16)]);
			for(int i = 1; i < 16; i++){
				fprintf(hdd, " %d", memory[(evictionNotice*16) + i]);
			}
			fprintf(hdd, "\n");
			fclose(hdd);
			printf("Page for process [%d] swapped out.\n", processLord[evictionNotice]);
			processLord[evictionNotice] = -1;
			return evictionNotice; 
			tries++;
		}
	}
}

void swapIn(int pid, int virtualFrame)
{
	pageEntry currTable[4];
	unsigned char swap2[16];
	FILE * hdd;
	hdd = fopen(filename, "a");
	memcpy(&currTable, &memory[ptRegister[pid].ptLoc*16], 16);
	int evictionNotice = 0;
	for(int i = 0; i<4; i++) { if(freeTable[i]==1) evictionNotice = i; }

    FILE *swapFile = fopen ( filename, "r" );
    if (swapFile != NULL) {
       unsigned char line [32];
       while (fgets(line, sizeof line, swapFile) != NULL)
       {
          fputs (line, stdout);
          if(line[0]==pid)
          {
          	printf("Making progress\n");
          }
       }
       fclose (swapFile);
    }
    else
    {
       perror (filename); 
    }
}


/****** MAP ******/
int map (int pid, int address, int value) 
{ 
	printf("\n\n*** Mapping ***\n\n");
	int virtualFrame = address/16; 							// virtual address frame
	pageEntry currTable[4];
	page currPage;
	int evictionNotice;
	initialize(currTable);
	int i;
	for(i = 0; i<11; i++) { currPage.values[i] = '\0'; }
	currPage.pid = pid;
	int physicalFrame;
	//printf("Pid: %d\n", pid);

	/*$$$ Get the Page Table Loaded $$$*/
	if(ptRegister[pid].valid == 1 && ptRegister[pid].ptLoc != -1) 	// if the table has been created
	{
		memcpy(&currTable, &memory[ptRegister[pid].ptLoc*16], 16);														// load the page table
		printf("Got page table for PID [%d] from physical address [%d]\n\n", pid, memory[ptRegister[pid].ptLoc%16]);
	}
	else if(ptRegister[pid].valid == 1 && ptRegister[pid].ptLoc == -1) 												// if the table is created but not in memory
	{
		/* Perform swapping */
		//printf("Swapping (map1)\n");
		evictionNotice = swapOut();
		processLord[evictionNotice] = pid;
		swapIn(pid, -1);
	}	
	else															// create a new page table for the new process
	{
		ptRegister[pid].valid = 1;									// this process now has a page table
		ptRegister[pid].ptLoc = findFree();							// set to its location in physical memory
		//printf("value: %i\n", ptRegister[pid].ptLoc);
		//printf("Pid: %d\n", pid);
		if(ptRegister[pid].ptLoc == -1)
		{
			//printf("Swapping (map2)\n");
			swapOut();
			ptRegister[pid].ptLoc = findFree();
		}
		//printf("ptLoc: %d\n\n\n", ptRegister[pid].ptLoc);
		initialize(currTable);
		processLord[ptRegister[pid].ptLoc] = pid;
		memcpy(&memory[ptRegister[pid].ptLoc*16], &currTable, 16); 
		printf("New page table created and stored at memory location [%d]\n", ptRegister[pid].ptLoc*16);
		freeTable[ptRegister[pid].ptLoc] = 2;
		//printf("Pid: %d\n", pid);
	}

	/*$$$ Get the page loaded into memory $$$*/
	if((i = currTable[virtualFrame].validBit) != -1)			// if the requested virtual space is not in use 																												// if there is space in the page table
	{
		printf("There is space in the page table\n");

		while((physicalFrame = findFree()) == -1 && ptRegister[pid].ptLoc != physicalFrame)		// find free spot in physical memory
		{
			//printf("Swapping (map3)\n");
			swapOut();
			physicalFrame = findFree();
		}
		//printf("PF: %d\n", physicalFrame);													
		currTable[virtualFrame].page = physicalFrame;										// add the new values for this PTE
		currTable[virtualFrame].presentBit = 1;
		currTable[virtualFrame].validBit = 1;
    	currTable[virtualFrame].value = value;		          						 // not indexing by pid anymore, index by virtualFrame #
		//printf("&&&&&&&&&&&&&&Currtable pf: %d\n", currTable[virtualFrame].page);
		//printf("&&&&&&&&&&&&&&Currpage pid: %d\n", currPage.pid);
		processLord[physicalFrame] = pid;
		memcpy(&memory[physicalFrame*16], &currPage, 16);
		memcpy(&memory[ptRegister[pid].ptLoc*16], &currTable, 16);													// store the new page in physical memory
		printf("Virtual address space updated\n");		
		printf("New page stored in physical frame [%d] and virtual frame [%d]\n", physicalFrame, virtualFrame);
		freeTable[physicalFrame] = 0;

	}
	else 					// no space in virtual memory for this process
	{ 
		printf("No room in virtual memory for process %d\n", pid); 
		// printMem();
		return 1;
	} 
	//printf("done\n");
	//printMem();
	return 0;
}


/****** STORE  ******/
int store (int pid, int address, int value) { 
	printf("\n\n*** Storing ***\n\n");
	pageEntry currTable[4];
	initialize(currTable);
	int virtualFrame;
	int evictionNotice;
	int offset;
	int check;
	int i = 0;
	page currPage;
	for(i = 0; i<15; i++) { currPage.values[i] = '0'; }
	currPage.pid = pid;
	int pg = 0;

	/* Check if the address is valid */
	if(ptRegister[pid].ptLoc == -1)
	{
		//printf("Swapping (store1)\n");
		evictionNotice = swapOut();
		processLord[evictionNotice] = pid;
		swapIn(pid, -1);
	}
	if (ptRegister[pid].valid == 1)
	{
		/*$$$ Get a Page Table Loaded $$$ */
		memcpy(&currTable, &memory[ptRegister[pid].ptLoc*16], 16);				// load the page table
		printf("Page table loaded at location: %d\n", ptRegister[pid].ptLoc);
		if(currTable == NULL) {
			printf("ERROR: Invalid load\n");
			return -1;
		}
		virtualFrame = address/16;
		offset = address%16;													// this assumes the input is an integer
		/*if(currTable[virtualFrame].value < 1)
		{
			printf("ERROR: This file is not writable\n");
			printf("permissions: %d\n", currTable[virtualFrame].value);
			return -1;
		}*/
		//printf("Offset: %d\n", offset);
		//printf("Value before store: %d\n", currPage.values[offset]);

		if((pg = currTable[virtualFrame].page) == -1) 							// if no place to load
		{ 
			printf("ERROR: Nothing to load in processes [%d] virtual memory\n", pid); 
		}
		else if (currTable[virtualFrame].presentBit == 0)
		{
			//printf("Swapping (store2)\n");
			evictionNotice = swapOut();
			processLord[evictionNotice] = pid;
			swapIn(pid, virtualFrame);
		}
		else         															// we have a place to load
		{
			memcpy(&currPage, &memory[pg*16], 16); 								// loads the page
			if((check= checkLoc(&currPage, offset)) != -1)
			{
				currPage.values[offset] = value;		 						// store the value
				printf("*** Process [%d] ***\n", pid);
				printf("Virtual frame [%d]\n", virtualFrame);
				printf("Value stored at physical memLoc [%d] with value: %d\n", pg, currPage.values[offset]);
				memcpy(&memory[ptRegister[pid].ptLoc*16], &currTable, 16);
				//printf("ptLoc: %d\n", ptRegister[pid].ptLoc);
				memcpy(&memory[pg*16], &currPage, 16);
				//printf("pg*16: %d\n", pg*16);
				printf("Updated the page in physical memory\n");
			}
			else
			{
				printf("ERROR: No more memory available\n");
				// update page?
			}
		}
	}
	else
	{
		// not created
		printf("ERROR: Page not created\n");
		return -1;
	}
	return 0;
}


/****** LOAD ******/
int load (int pid, int address, int value) 
{ 
	printf("\n\n*** Loading ***\n\n");
	pageEntry* currTable = malloc(4*sizeof(pageEntry));
	int virtualFrame;
	int evictionNotice;
	int offset;
	int check;
	int i = 0;
	page currPage;
	for(i = 0; i<15; i++) { currPage.values[i] = '0'; }
	currPage.pid = pid;
	int pg = 0;
	
	if(ptRegister[pid].ptLoc == -1)
	{
		//printf("Swapping (load1)\n");
		evictionNotice = swapOut();
		processLord[evictionNotice] = pid;
		swapIn(pid, -1);
	}

	/* Check if the address is valid */
	if (ptRegister[pid].valid == 1)
	{
		/*$$$ Get a Page Table Loaded $$$ */
		memcpy(&currTable, &memory[ptRegister[pid].ptLoc], 16);							// load the page table
		printf("Page table loaded at location: %d\n", ptRegister[pid].ptLoc);
		if(currTable == NULL) {
			printf("ERROR: Invalid load\n");
			return -1;
		}

		if(i != -1)
		{
			virtualFrame = address/16;
			offset = address%16;
			//printf("Offset: %d\n", offset);
			//printf("Value before load: %d\n", currPage.values[offset]);						    

			if((pg = currTable[virtualFrame].page) == -1) 					// if no place to load
			{ 
					printf("*** Process [%d] ***\n", pid);
					printf("404: page not found (1)\n");
					printf("Virtual frame [%d]\n", virtualFrame);
					printf("Value NOT loaded at physical memLoc [%d] with value: %d\n", pg, currPage.values[offset]);
			}
			else if (currTable[virtualFrame].presentBit == 0)
			{
				//printf("Swapping (load2)\n");
				evictionNotice = swapOut();
				processLord[evictionNotice] = pid;
				swapIn(pid, virtualFrame);
			}
			else         												// we have a place to load
			{
				if((check = checkLoc(&currPage, offset)) == -1)
				{
					//value = currPage.values[offset];
					printf("*** Process [%d] ***\n", pid);
					printf("Virtual frame [%d]\n", virtualFrame);
					printf("Value loaded at physical memLoc [%d] with value: %d\n", pg, currPage.values[offset]);
				}
				else
				{
					printf("*** Process [%d] ***\n", pid);
					printf("404: page not found(2)\n");
					printf("Virtual frame [%d]\n", virtualFrame);
					printf("Value NOT loaded at physical memLoc [%d] with value: %d\n", pg, currPage.values[offset]);

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
		// not created
		printf("ERROR: Page not created\n");
		return -1;
	}

	return 0;
}


/*********************************************************** MAIN ****************************************************************************/
int main(int argc, char **argv)
{
	srand(time(NULL));
	/* Welcome mat */
	printf("******************************************************\n");
	printf("****** WELCOME TO VIRTUAL MEMORY SIMULATOR 2.0 	******\n");
	printf("******      (Please don't touch anything...)	******\n");
	printf("******************************************************\n\n");
	printf("Helpful Stuff:\n");
	printf("  1. Input format is : pid,instruction,address,value \n");
	printf("  2. Special Instruction: 'print' will print physical memory\n");
	printf("  3. Each page can store in offset 0-12\n\n");
	printf("                  *************\n");
	printf("                  * Have fun! *\n");
	printf("                  *************\n\n");
	char *userInput[4]; 
 	int pid; 															//pid
	char* instruction; 													//instruction type
	int address; 														// virtual address
	int value;															// value
	printf("Swapping Table to Output File\n");

	for(int i=0; i<4; i++) { 
		ptRegister[i].valid=0;
		ptRegister[i].ptLoc=0;
	}

   	while(1)
  	{
  		printf("back at it again\n");
   		char argsArr[4] = {0,0,0,0}; 									//store arguments in array
    	char * args = argsArr; 											// make pointer to array
    	const char s[2] = ","; 											// token to check for
    	char *token; 													// token creation

   		/* tokenize first input */ 
    	printf("\nInstruction? ");
    	if (fgets(args,32,stdin) == NULL) { printf("\nEnd of File Reached\n"); exit(1); }
    	token = strtok(args, s);
    	if (!token) { printf("ERROR: Invalid Input \n"); continue; }   // check for null token
    	userInput[0] = token;
    	printf("userInput[0]: %s\n", token);

    	/* tokenize rest of inputs */
 		int i = 1;
    	while(i < 4) 
    	{
    		token = strtok(NULL, s);
    		if (!token) { i = 5; } 										// check for null token
    		userInput[i] = token;
    		printf("userInput[%i]: %s\n", i, token);
    		i++;
    	}
    	if (i == 6) { printf("ERROR: Invalid Input\n"); continue; }
    	
    	/* assign tokenized values */
    	pid = atoi(userInput[0]);
		instruction = userInput[1];
		address = atoi(userInput[2]);
		value = atoi(userInput[3]);
		printf("\nYou Entered: PID: [%i], Instruction: '%s', address: [%i], value: [%i] \n", pid, instruction, address, value);
  		masterFunction(pid, instruction, address, value);
  		printf("yo\n");
	}
	return 0;
}
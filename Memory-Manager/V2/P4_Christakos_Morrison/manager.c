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

typedef struct {
	unsigned char present;		//1 if present, 0 on disk
	unsigned char valid;		//v1 if valid
	unsigned char value;		//1 if can be written, 0 if read-only
	unsigned char frame; 		//page number ie frame
} pageEntry;

typedef struct {
	pageEntry pages[4];
} reg;

typedef struct {
	unsigned char* pos[16];
} memPage;

typedef struct{
	unsigned char checkPage[4];
	unsigned char page1;
	unsigned char page2;
	unsigned char page3;
	unsigned char page4;
} freeL;

int tableLocs[4];
int fileLine = 1;
int pageEvict = 3;
int freeMem;
int useFrame = 0;
unsigned char memory[64];		//physical memory
freeL freeTable;				// list of free space
reg ptRegister[4];				// the page table registers, indexed by pid


/********************************************************* FUNCTION DECLARATIONS *************************************************************/

void masterFunction(int process, char* instr, int addr, int val);
int map(int process, int addr, int val);
int store(int process, int addr, int val);
int load(int process, int addr, int val);
int getTable(int pid);
int getFrame(int tableFrame, int addr, int process);
int getPages();
int swapOut(int evicter);
int swapIn(int whereIsIt, int type, int addr);


/******************************************************** HELPER FUNCTIONS *******************************************************************/

/* runs selected instruction */
void masterFunction (int process, char * instr, int addr, int val) 
{
	if (!(strcmp(instr, "map")))        	{ map(process, addr, val);   }
	else if (!(strcmp(instr, "load")))  	{ load(process, addr, val);  }
	else if (!(strcmp(instr, "store"))) 	{ store(process, addr, val); }
	else { printf("ERROR: You Specified an Invalid Instruction\n"); }
}


int getFrame(int tableFrame, int addr, int process)
{
	int freeSpot;
	int tFrame = getTable(process);
	printf("Process %d's address %d's table is at: %d\n", process, addr,tFrame);
	if(memory[(16*tFrame) +  (4*(addr/16)) + 3] == 100) 							//not created yet
	{
		memPage putInMem;
		freeSpot = getPages(process, addr);
		for(int i =0; i < 16; i++){
			putInMem.pos[i] = -1;
		}
		for(int i =0; i < 16; i++) { memory[(16*freeSpot) + i] = putInMem.pos[i]; }
		memory[(16*tFrame) +  (4*(addr/16)) + 3] = freeSpot;
		return freeSpot;
	}
	else if(memory[((16*tFrame) +  (4*(addr/16)) + 0)] == 0) 						//not in memory
	{
		int newLoc = swapIn(memory[(16*tFrame) +  (4*(addr/16)) + 3], tFrame, addr);//get where it is now located
		memory[(16*tFrame) +  (4*(addr/16)) + 3] = newLoc;							//update page table
		return newLoc;																//return its new location
	}
	else { return memory[(16*tFrame) +  (4*(addr/16)) + 3]; }
	return 1;
}

int getTable(int pid)
{
	if(tableLocs[pid] == 100) 														//not created yet
	{
		int freeSpot = getPages((pid * -1), 0, 0); 							//returns where to put the table
		tableLocs[pid] = freeSpot;
		for(int i =0; i < 4; i++)
		{
			memory[(16*freeSpot) + (4*i) + 0] = ptRegister[pid].pages[i].present;
			memory[(16*freeSpot) + (4*i) + 1] = ptRegister[pid].pages[i].valid;
			memory[(16*freeSpot) + (4*i) + 2] = ptRegister[pid].pages[i].value;
			memory[(16*freeSpot) + (4*i) + 3] = ptRegister[pid].pages[i].frame;
		}
		printf("Put page table for PID %d into physical frame %d\n", pid, freeSpot);
		return freeSpot;
																					//swap into memory(set location = negative location on disk)
	}
	else if(tableLocs[pid] < 0) 													//not in mem
	{
		int newLoc = swapIn((-1*tableLocs[pid]), (pid  * -1), 0);
		tableLocs[pid] = newLoc;
		printf("Put page table for process: %d in frame: %d\n", pid, newLoc);
		return newLoc;
	}		
	else { return tableLocs[pid]; } 												//in physical mem already
}

/* returns free page */
int getPages()
{
	for(int i = 0; i < 4; i++){														//free physical mem
		if(freeTable.checkPage[i] == 0){
			freeTable.checkPage[i] = 1;
			return i;
		}
	}

	pageEvict++;																	//set the next eviction to the next frame
	if((pageEvict + 1) >= 4){ pageEvict = 0; } 										//set the eviction page
	for(int i = 0; i < 4; i++)
	{
		if(tableLocs[i] == pageEvict) 												//if a ptable is up for eviction
		{
			int numMemPages = 0;
			int pFrame = tableLocs[i];
			for(int j = 0; j < 4; j++)
			{
				if (memory[(16 * pFrame) + (j*4) + 1] == 1) { numMemPages++; }
			}
			if (numMemPages > 0) { pageEvict++; }
		}
		if((pageEvict + 1) >= 4) { pageEvict = 0; } 							//set the eviction page
	}
	if((pageEvict + 1) >= 4){ pageEvict = 0; } 									//set the eviction page 

	swapOut(pageEvict);
																				//make space in frame pageEvict
	return pageEvict;															//no free memory
}

/*************************************************** SWAP FUNCTIONS *******************************************************************/

/* return negative numb if page table, positive if memPage */
int swapOut(int evicter) 
{
	int isT = 0;//if swapped out is a table
	for(int i = 0; i < 4; i++)
	{
		if(tableLocs[i] == evicter) { isT = 1; }
	}
	FILE* swapFile;
	swapFile = fopen("swapFile.txt", "a");
	fprintf(swapFile, "%d", memory[(evicter*16)]);
	for(int i = 1; i < 16; i++) { fprintf(swapFile, " %d", memory[(evicter*16) + i]); }
	fprintf(swapFile, "\n");
	fclose(swapFile);
	if(isT) 																	//if its a table
	{
		for(int i = 0; i < 4; i++) 												//check which pid its for
		{
			if(tableLocs[i] == evicter) { tableLocs[evicter] = (-1*fileLine); } //update its location marking it as in disk
		}
	}
	else 																		//if its a memPage
	{
		for(int i = 0; i < 4; i++) 												//go through table locations
		{
			if(tableLocs[i] >= 0) 												//if table i is present
			{
				for(int j = 0; j < 4; j++) 										//go through its memPages
				{
					if(memory[((16*i) + (4*j) + 3)] == evicter) 				//if its memPage j's frame is the one getting evicted
					{
						memory[((16*i) + (4*j) + 0)] = 0;						//update its present bit
						memory[((16*i) + (4*j) + 3)] = fileLine;					//update its frame bit
					}
				}
			}
		}
	}
	printf("Swapped out page %d to swapFile line: %d\n", evicter, fileLine);
	fileLine++;
	return fileLine;
}

/* return neg if  pid for table or pos if tableFrame for memPage */
int swapIn(int whereIsIt, int type, int addr)
{
	int freeSpot = getPages(type, addr);
	int actLoc = whereIsIt - 1;
	memPage reader;
	for(int i = 0; i < 16; i++){ reader.pos[i] = 0; }
	char ln[255][100];
	FILE *swapFile = fopen("swapFile.txt", "r");
	if(swapFile != NULL)
	{
		for(int i = 0; i < 255; i++)
		{
			if(fgets(ln[i], sizeof(ln[i]), swapFile) == NULL) { i = 255; }
		}
		char temp[4];
		int posI = 0;
		int index = 0;
		int charI = 0;
		for(int i = 0; i < 4; i++){temp[i] = ' '; } 					//reset temp
		do{
			temp[index] = ln[actLoc][charI];
			if(ln[actLoc][charI + 1] == ' ') 							//if the next char is a space
			{				
				index = 4;												//end the temp
				charI++;												//skip to the space
			}
			else{
				index++;
			}
			if(index == 4)												//temp is ended
			{
				reader.pos[posI] = atoi(temp);
				for(int i = 0; i < 4; i++){ temp[i] = ' '; } 			//reset temp
				index = 0;												//restart temp
				posI++;
			}
			charI++;													//go to next char
		}while(charI < 254);
	}
	else{
		printf("ERROR: File doesn't exist!");
		return -1;
	}
	printf("\n");
	if(type < 0) { tableLocs[(type*-1)] = freeSpot; } 					//swapping in a table for prcoess type
	else 																//swapping in a memPage with its tables frame type
	{
		memory[((type*16) + (4*(addr/16)) + 0)] = 1;
		memory[((type*16) + (4*(addr/16)) + 3)] = freeSpot;
	}
	printf("Swapped into page %d from swapFile line: %d\n", freeSpot, whereIsIt);

	for(int i =0; i < 16; i++) { memory[(16*freeSpot) + i] = reader.pos[i]; }
	return freeSpot;
}

/*************************************************** INSTRUCTION FUNCTIONS *******************************************************************/


int map(int process, int addr, int val)
{
	int tFrame, pFrame;
	tFrame = getTable(process);
	if(memory[(tFrame * 16) + (4*(addr/16)) + 1] == 1) 								//if its been created already
	{
		tFrame = getTable(process);
		if(memory[(tFrame * 16) + (4*(addr/16))] == 0) 								//if present bit == 0 (its the first variable in the  addr/16 entry)
		{
			pFrame = getFrame(tFrame, addr, process);								//returns the physical frame the desired memPage is in
			printf("Mapped virtual address %d into physical frame %d!\n", addr, pFrame);
			memory[(tFrame * 16) + (4*(addr/16))] = 1;								//present
		}
		if(memory[(16 * tFrame) + (4*(addr/16)) + 2] == val) 						//check read-write ability
		{
			printf("ERROR: virtual page %d is already mapped with value %d!\n", (addr/16), val);
		}
		else{
			memory[(16 * tFrame) + (4*(addr/16)) + 2] = val;						//update read-write ability
			printf("Updating permissions for virtual page %d\n", (addr/16));
		}
	}
	else 																			//if it hasnt been created yet
	{
		pFrame = getFrame(tFrame, addr, process);									//returns the physical frame the desired memPage is in
		printf("Mapped virtual address %d into physical frame %d!!\n", addr, pFrame);
		memory[(tFrame * 16) + (4*(addr/16))] = 1;									//present
		memory[(tFrame * 16) + (4*(addr/16)) + 1] = 1;								//valid
		memory[(tFrame * 16) + (4*(addr/16)) + 2] = val;							//value
		memory[(tFrame * 16) + (4*(addr/16)) + 3] = pFrame;							//frame
	}
	return 0;
}

/*returns the physical frame the table is located in */
int store(int process, int addr, int val) 
{
	int tFrame = getTable(process);
	int pFrame = getFrame(tFrame, addr, process);									//returns the physical frame the desired memPage is in
	if(memory[(16 * tFrame) + (4*(addr/16)) + 2] == 0) { printf("ERROR: You can't write to this page\n"); return -1; }
	else
	{
		memory[(16*pFrame) + (addr%16)] = val;										//sets the address in the pagesFrame to val;
		printf("Stored value %d at virtual address %d (physical frame: %d)!\n", val, addr, pFrame);
	}
	return 1;
}


int load(int process, int addr, int val)
{
	int tFrame = getTable(process);													//returns the physical frame the table is located in
	int pFrame = getFrame(tFrame, addr, process);									//returns the physical frame the desired memPage is in
	printf("The value %d is stored at virtual address %d (physcial frame: %d)!\n", (int)memory[(16*pFrame) + (addr%16)], addr, pFrame);
	return (int)memory[(16*pFrame) + (addr%16)];
}



/*********************************************************** MAIN ****************************************************************************/

int main(int argc, char **argv)
{
	/* Welcome mat */
	printf("******************************************************\n");
	printf("****** WELCOME TO VIRTUAL MEMORY SIMULATOR 2.0 	******\n");
	printf("******      (Please don't touch anything...)	******\n");
	printf("******************************************************\n\n");
	printf("Input format is : pid,instruction,address,value \n\n");
	printf("                  *************\n");
	printf("                  * Have fun! *\n");
	printf("                  *************\n\n");
	char *userInput[4]; 
 	int pid; 															//pid
	char* instruction; 													//instruction type
	int address; 														// virtual address
	int value;															// value

	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 4; j++){
			ptRegister[i].pages[j].present = 0;
			ptRegister[i].pages[j].valid = 0;
			ptRegister[i].pages[j].value = 0;
			ptRegister[i].pages[j].frame = 100;
		}
	}

	for(int i = 0; i < 4; i++){
		tableLocs[i] = 100;
	}

	memcpy(memory, ptRegister, 16);
	


	for(int i = 0; i < 4; i++){											//initializing
		freeTable.checkPage[i] = 0;
	}

	while(1)
  	{
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
    	//printf("userInput[0]: %s\n", token);

    	/* tokenize rest of inputs */
 		int i = 1;
    	while(i < 4) 
    	{
    		token = strtok(NULL, s);
    		if (!token) { i = 5; } 										// check for null token
    		userInput[i] = token;
    		//printf("userInput[%i]: %s\n", i, token);
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
	}
	return 0;
}
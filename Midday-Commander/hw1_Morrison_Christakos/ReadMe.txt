Andrew Morrison - armorrison@wpi.edu
Peter Christakos - pechristakos@wpi.edu
Project One README - CS 3013
1/24/18

Attached in this zip folder are several files: A makefile which makes each individual ".c" file as well as has capabilities to "make clean" and "make all", the 3 mission commander files, and a test case folder. This folder has test cases copied from the terminal for each of the 3 '.c' files.

Errors:
Due to lack of time and intelligence, some errors occured for the final product of our mc2.c file. In addition the mc0.c, mc1.c and mc2.c files dont have capabilites for reading input files. We decided to use scanf to record inputs and realized to late that it was not the ideal way to do so. mc0 and mc1 work to satisfaction. Since mc2 cant properly output files, we showed our sample pipeline output as well as our tests by copying the terminal. Also, mc2 has issues recording background tasks. 

Test Cases: 
As I mentioned above, the mc0.c, mc1.c, and mc2.c files were not written with capabilities to read from input files due to many issues with scanf. To make up for this, we ran many options in the terminal and copied and pasted the results into an output file. 

How our program works: 

Main:
Our main function starts up by declaring variables and structs which are allocated and will be used throughout the function to perform various tasks and hold various pieces of data. The majority of lines and function calls in the main file are commented and will explain line by line effects. After this, the majority of the main function runs inside of a whille loop. More variables to use per run. We use a getline function to read the inputs. This was done because we used scanf and realized it did not turn out well for piping to an input file, and getline was an easy solution. The main function then goes through many different if statements and then eventually a switch case. The next major operation is adding arguments, which the user is able to do for commands 0, 1, and 2. Inside the statemtnt, the strtok() function is used to convert the arguments "-l -m", for example, into a collection = {"-l", "-m"}. After this function, the fork function is called and its return value is saved to a value. If that value ends up as the pid (meaning) that it is a child process, the parent process gets run. After this, a switch statement based on userInput is run through and smaller tasks are performed per option. 

childProcess:
The child process function simply runs an execvp with the input as well as an array of strtok() arguments.

parentprocess:
This function is used to get statistics like page faults and function elapsed time based on the duration of the wait() function which waits for a process to end. There is also a flag value in the code which will tell us if the process is a background command, and will run a separate function with waitpid() which will tell us when the background process is completed and print data for it. 
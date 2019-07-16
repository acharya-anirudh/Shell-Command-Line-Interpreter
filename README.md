# Shell-Command-Line-Interpreter
Development of my personal shell command line interpreter in C. This was a project assigned by my university. The problem statement is given in the pdf file. I am uploading my solution to the Question 1 with a detailed README.

## References used: 
a. https://www.cs.purdue.edu/homes/grr/SystemsProgrammingBook/Book/Chapter5-WritingYourOwnShell.pdf
b. https://brennan.io/2015/01/16/write-a-shell-in-c/
c. https://www.geeksforgeeks.org/making-linux-shell-c/
d. https://danrl.com/blog/2018/how-to-write-a-tiny-shell-in-c/
e. Linux man pages for dup2, pipe(), execvp etc.
    

## Compilation Instructions:
	Open a terminal in this directory
	Run: "sudo apt-get install libreadline-dev"(without the quotes). This installs the readline required for input and we can link   it    during compilation.
	Run: "gcc shell.c -lreadline"
  
## Execution Instructions:
	 Run: "./a.out" in a terminal in the same directory
  
### Structure of the program
	The basic execution structure is as follows:
	a. Take input command from the user 
	b. The Parser: Parse the command based on spaces and if pipes present then parse it based on pipes. 
	c. The Executer: After parsing the spaces and pipes, the parsed command is sent to the appropriate function to execute it. 

### Functions Used: 

```
a. int main() -> used to allocate memory for arrays and pointers and performs a looping task of calling 
get_input() function to take user input. Sends the user input to the relevant functions for parsing and 
the return value decides is it is a normal command or contains a pipe. Also it handles all the entry and 
exit functionality of the shell.

b. int getInput(char* input) -> Uses readline function for taking input from the user and stores it in a 
global char *. Even though its dangerous to use char *, due to time constraint I could not manage to use 
malloc everywhere though malloc is the way to do it the proper way. Also this function writes the input to 
the command.log file if logging is turned on.

c. int parse(char *input, char** pointer_to_parsedArgs) -> The parse function calls two major functions 
namely "parse_pipe" and "parse_space" to parse the input depending on if pipe is present or not. If pipe 
is present, it directly calls the execute_pipe function else it checks if the command is an internal command 
by calling the function	internal_command_execute(pointer_to_parsedArgs) else it returns to main and 
the function execute(pointer_to_parsedArgs) is invoked to run the external command.

d. void parse_space(char *input, char **pointer_to_parsedArgs) and void parse_pipe(char *input, char ** command_pipe) 
are both parsing functions which parses the input based on spaces or pipes and stores the parsed values in 
the array specified in the second argument.

e. void execute(char ** pointer_to_parsedArgs) -> This function only deals with the commands which doesn't 
include pipes and uses execvp() to execute the command. 

f. void execute_pipe(char **pointer_to_parsedArgs, int command_number, int n) -> This function implements 
pipes using dup2() function and intermediate files. It stores the output of the first command in a file F1.txt 
and the next command reads the input from F1 and stores it in F2.txt. Finally, F2.txt is made F1.txt  for the 
further commands so that only these two files are used for n piped commands.

g. int internal_command_execute(char **pointer_to_parsedArgs) -> The internal command handler executes the 
internal commands specified in the assignment. The commands are stored in a static array and are executed 
from this function.
```
## Completeness of Assignment
```
In my program, the completeness of the assignment is totally dependent on some assumptions:
a. The internal commands will not be used in pipes. 
b. Incomplete pipe commands are not processed and will misbehave. Example: ls | <nothing>
c. The "entry" string to start the terminal is stored in an array of size 20 so anything beyond 
that input size will result in unexpected behaviour.
d. Any input beyond size 120 will not be accepted and program could crash due to errors.
```

## Current Issues
```
a. The changedir function fails at some specific examples. Example: In a normal terminal, suppose 
you are in a directory "user:~$ /home/user/Desktop/" and you issued the command 
"user:~$ cd ~/Documents", then it automatically takes the /home/user by itself and changes the 
directory to "user:~$ /home/user/Documents/". 
In my case doing this, the directory doesn't change until I specify the whole path.

b. After opening output.log file, sometimes the heading of piped commands is appended 
with a garbage value. I am guessing it happends due to the use of char* pointers and not 
allocating the memory using malloc. Still trying to figure out this bug.

c. The headings of output.log commands are in the format [Whole_command] so even when piped 
commands are being used, the heading is specified as [Command1 | Command 2| ...] but the 
required output is [command1] | [command2] ... I have used an algorithm which prints this 
output by detecting each pipe ' | ' and taking the whole string from the start.
Example: "ls -l | grep alpha | more" is broken as "ls -l" then "ls -l | grep alpha" then 
"ls -l | grep alpha | more" thus I can always append the braces as a whole and not individually.

d. Commands doesn't work if you change your directory to /home/ since we are working with 
file creation and printing and /home doesn't allow file creation in its directory. Thus it 
outputs "Open Failed" which is the same case when you try to create a file in /home/.
```

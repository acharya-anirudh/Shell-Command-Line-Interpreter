#include <stdio.h>
#include <string.h>
#include <readline/readline.h> //readline input
#include <readline/history.h> //history readline
#include <unistd.h> //chdir fork dup2
#include <sys/wait.h> //wait(NULL) 
#include <stdlib.h>
#include <time.h> //ctime
#include <fcntl.h> // dup2

int log_flag = 0; // set of logging is being performed
FILE *fp;
FILE *fptr;
char *temp; // main input stored from user
FILE *fptr1,*fptr2, *fptr3;
int failed = 0;
int c_0 = 0; // command 0 fail flag in pipes
char command_log_dir[100]; //stores CWD 
char output_log_dir[100]; //stores CWD

int getInput(char* input)
{
	time_t current_time;
	struct tm * info;
	time(&current_time);
	info = localtime(&current_time);

	temp = readline("\nrts@shell:~");	
	if(strlen(temp) != 0)
	{
		add_history(temp);
		//fptr = fopen("/home/cli-ninj4/Desktop/RTS_ASSIGNMENT/output.log", "a");
		//fprintf(fptr, "%s\n", temp);
		//fclose(fptr);
		if(log_flag == 1)
		{
			fp = fopen(command_log_dir, "a");
			fprintf(fp, "%s %d %d %d %d:%d:%d ", temp, info->tm_mday, info->tm_mon + 1, info->tm_year + 1900, info->tm_hour, info->tm_min, info->tm_sec);
			fclose(fp);
		}
		//printf("cwdi = %s\n", cwdi);
		strcpy(input, temp);
		return 0;
	}
	else return 1;
}

int parse_pipe(char* input, char** command_pipe) 
{ 
	int i; 
	for (i = 0; i < 120; i++) 
	{ 
		command_pipe[i] = strsep(&input, "|"); 
		if (command_pipe[i] == NULL) 
			break; 
	} 

	if (command_pipe[1] == NULL) 
		return 0; // returns zero if no pipe is found. 
	else 
	{ 
		return i; // returns number of pipe commands
	} 
} 

void parse_space(char *input, char ** pointer_to_parsedArgs) 
{
	int i;
	for(i = 0; i < 120; i++)
	{
		pointer_to_parsedArgs[i] = strsep(&input, " ");
		if(pointer_to_parsedArgs[i] == NULL) break;
		if (strlen(pointer_to_parsedArgs[i]) == 0) 
            i--; 

	}
}

void execute_pipe(char ** pointer_to_parsedArgs, int command_number, int n)
{
	pid_t process;
	int fd, fd1;
	int fd_output;
	int v =0;
	process = fork();	

	if(process < 0)
	{
		printf("\nFork failed"); return;
	}

	if(process == 0) // child
	{
		if(command_number == 0) //first command so write output to file 
		{
			
			if((fd = open("F1.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRWXU)) == -1)
			{
				perror("OPEN");
				exit(0);
			}
			close(STDOUT_FILENO); //close standard output so that it writes to the file and not the terminal
			dup2(fd, STDOUT_FILENO); // copy file desc fd to std out
			close(fd);

			//printf("pointer_to_parsedArgs")

			if(execvp(pointer_to_parsedArgs[0], pointer_to_parsedArgs) < 0)
			{
				
				printf("\nCommand not executed");
				exit(2);
			}	
			//exit(4);
		}		
		/*else if(command_number == n) // last command so display the output to the shell and dont save anything to the file
		{
			char * last;

			fd = open("F1.txt", O_RDWR); // open file and read and display
			close(STDIN_FILENO); //input required from file desc and not stdin
			dup2(fd, STDIN_FILENO); // make fd point to stdin
			close(fd);

			if(execvp(pointer_to_parsedArgs[0], pointer_to_parsedArgs) < 0)
			{
				printf("\nCommand not executed");
				exit(0);
			}

		}*/		
		else // any intermediate command for pipe will read and write through intermediate files 
		{
			//printf("HELLO\n");
			fd = open("F1.txt", O_RDWR);
			fd1 = open("F2.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRWXU);
			
			close(STDOUT_FILENO);
			dup2(fd1, STDOUT_FILENO);
			close(fd1);

			close(STDIN_FILENO);
			dup2(fd,STDIN_FILENO);
			close(fd);		



			if(execvp(pointer_to_parsedArgs[0], pointer_to_parsedArgs) < 0)
			{
				printf("\nCommand not executed");
				exit(2);
			}
			
		}
	}

	else // parent
	{
		int status = 0;
		wait(&status);
		
		if(command_number == 0)
		{
			if(status == 512 && log_flag == 1)
			{
				c_0 = 1;
				fp = fopen(command_log_dir, "a");
		        fprintf(fp, "%s\n", "FAILED");
				fclose(fp);
			}

			if(status == 0 && log_flag == 1)
			{
				fp = fopen(command_log_dir, "a");
		        fprintf(fp, "%s\n", "SUCCESS");
				fclose(fp);
			}
		}
			
		else if(command_number !=0)
		{
			if(status == 512 && log_flag == 1 && c_0 != 1)
			{
				fp = fopen(command_log_dir, "a");
		        fprintf(fp, "%s\n", "FAILED");
				fclose(fp);
				return;	
			}		
		}
			
	}

}

int internal_command_execute(char **pointer_to_parsedArgs)
{
	int int_commands = 6, i;
	char* comm[int_commands];
	char cwd[1024];

	time_t current_time;
	struct tm * info;
	time(&current_time);
	info = localtime(&current_time);

	char c;

	comm[0] = "log";
	comm[1] = "unlog";
	comm[2] = "viewcmdlog";
	comm[3] = "viewoutlog";
	comm[4] = "changedir";
	comm[5] = "exit";


	if(strcmp(pointer_to_parsedArgs[0], comm[0]) == 0)
	{
		//comm_success = 1;
		printf("Logging into file named command.log \n");
		log_flag = 1;
		return 1;
	}

	else if(strcmp(pointer_to_parsedArgs[0], comm[1]) == 0)
	{

		//comm_success = 0;
		//printf("in unlog");
		log_flag = 0;
		printf("\nLogged into file named command.log");
		fp = fopen(command_log_dir, "a");
		if(fp == NULL) printf("ERROR");
		fprintf(fp, "%s\n", "SUCCESS");
		fclose(fp);
		return 1;
	}

	else if(strcmp(pointer_to_parsedArgs[0], comm[2]) == 0)
	{
		fp = fopen(command_log_dir, "r");
		if(fp == NULL) 
		{
			printf("\nCan't open file");
			return 1;
		}

		c = fgetc(fp);
		while(c != EOF)
		{
			printf("%c", c);
			c = fgetc(fp);
		}

		fclose(fp);
		return 1;
	}

	else if(strcmp(pointer_to_parsedArgs[0], comm[3]) == 0)
	{
		fp = fopen(output_log_dir, "r");
		if(fp == NULL) 
		{
			printf("\nCan't open file");
			return 1;
		}

		c = fgetc(fp);
		while(c != EOF)
		{
			printf("%c", c);
			c = fgetc(fp);
		}

		fclose(fp);
		return 1;
	}

	else if(strcmp(pointer_to_parsedArgs[0], comm[4]) == 0)
	{
		chdir(pointer_to_parsedArgs[1]);
		getcwd(cwd, sizeof(cwd));
		printf("\nDirectory Changed to: %s", cwd);
		//printf("Writing to file");
		fp = fopen(command_log_dir, "a");
		fprintf(fp, "%s\n", "SUCCESS");
		//printf("\nWritten");
		fclose(fp);
		return 1;
	}

	else if(strcmp(pointer_to_parsedArgs[0], comm[5]) == 0)
	{
		return 2;
	}

	return 0;
}

int parse(char *input, char** pointer_to_parsedArgs)
{
	char c;
	//printf("inpy:%s\n",input );
	char *command_pipe[80]; // stores pipe commands
	//printf("input in parse = %s", input);
	int pipe_exist_count = parse_pipe(input, command_pipe); // returns no of pipe commands if pipe exists
	//printf("pipe exist = %d", pipe_exist_count);
	if(pipe_exist_count)
	{
		fptr2 = fopen("output.log", "a");
		if(fptr2 == NULL)
		{
			printf("\nOpen error");
			exit(0);
		}
		char str[strlen(temp)];
		memcpy(str,temp, strlen(temp));
		char *p = str;
		int in = 0;
		char * t;
		int z = 0;
		//printf("%s", str);
		for(int i = 0; command_pipe[i] != NULL; i++)
		{	
			
			for(; z < strlen(str); z++)
			{
				//printf("%c", *p);
				if(*p == '|')
				{
					if(in)
					{
						char current = *p;
						*p = 0;
						t = str;
						//printf("HERE\n");
						fprintf(fptr2, "[%s]\n", t);
						*p = current;
						in = 0;
						break;
					}
				}

				else 
				{
					in = 1;
				}
				p++;
			}
			if(in )
			{
				//t = str;
				//printf("HERE2\n");
				fprintf(fptr2, "[%s]\n", str);
				
			}

			

			parse_space(command_pipe[i], pointer_to_parsedArgs);
			execute_pipe(pointer_to_parsedArgs, i, pipe_exist_count - 1);

			if(i == 0)
			{
				fptr1 = fopen("F1.txt", "r");
				if(fptr1 == NULL)
				{
					printf("\nOpen failed");
					exit(0);
				}
				c = fgetc(fptr1);
				while(c != EOF)
				{
					fputc(c, fptr2);
					c = fgetc(fptr1);
				}
				fclose(fptr1);	
			}
		    else if(i != 0)
			{
				fptr1 = fopen("F2.txt", "r");
				if(fptr1 == NULL)
				{
					printf("\nOpen failed");
					exit(0);
				}
				c = fgetc(fptr1);
				while(c != EOF)
				{
					fputc(c, fptr2);
					c = fgetc(fptr1);
				}
				fclose(fptr1);
			}
			if(i==pipe_exist_count-1)
			{
				fptr1 = fopen("F2.txt", "r");
				if(fptr1 == NULL)
				{
					printf("\nOpen Failed");
					exit(0);
				}
				c = fgetc(fptr1);
				while(c != EOF)
				{
					printf("%c", c);
					c = fgetc(fptr1);
				}
				fclose(fptr1);
			}
			if(i != 0) // if any intermediate pipe command
			{
				system("mv F2.txt F1.txt");
			}
		}

		fclose(fptr2);
		return 2;		
	}	
	else 
	{
		parse_space(input, pointer_to_parsedArgs);
		//printf("%s %d", input, log_flag);
		if(log_flag == 1)
		{
			fptr1 = fopen(output_log_dir, "a");
			fprintf(fptr1, "\n");
			for(int i = 0; pointer_to_parsedArgs[i] != NULL; i++)
			{
				fprintf(fptr1, "%s", pointer_to_parsedArgs[i]);	
			}
			
			fprintf(fptr1, "\n");
			fclose(fptr1);	
		}
		
	}				
	/*
	printf("Arguments are\n");
	for(int i = 0; pointer_to_parsedArgs[i] != NULL; i++)
	{
		printf("Pointer to parse args: %s ", pointer_to_parsedArgs[i]);
	}
	*/
	int internal_return = internal_command_execute(pointer_to_parsedArgs);
	if(internal_return == 2)
	{
		return 0; //returning once internal command is processed
	
	}

	if(internal_return == 1)
	{
		return 3; //returning once internal command is processed
	}	
	return 1; // normal execute called
}

void execute(char ** pointer_to_parsedArgs)
{
	int fd;
	time_t current_time;
	struct tm * info;
	time(&current_time);
	info = localtime(&current_time);
	pid_t pid = fork();

	if(pid == 0)
	{	

		if((fd = open("F1.txt", O_RDWR | O_TRUNC | O_CREAT, S_IRWXU)) == -1)
		{
			perror("OPEN");
			exit(0);
		}


		close(STDOUT_FILENO); //close standard output so that it writes to the file and not the terminal
		dup2(fd, STDOUT_FILENO); // copy file desc fd to std out
		close(fd);



		if(execvp(pointer_to_parsedArgs[0], pointer_to_parsedArgs) < 0)
		{						
			printf("Execution failed\n");
			exit(2);			
		}	
		
	}
	if(pid == -1) printf("\nFork Failed");

	else 
	{
		int status = 0;
		wait(&status); //wait for child
		//printf("\n%d", status);
		if(status == 0 && log_flag == 1)
		{
			
			fp = fopen(command_log_dir, "a");
	        fprintf(fp, "%s\n", "SUCCESS");
			fclose(fp);
		}

		else if(status == 512 && log_flag == 1)
		{
			fp = fopen(command_log_dir, "a");
	        fprintf(fp, "%s\n", "FAILED");
			fclose(fp);
		}
		return;
	}
}

int main()
{
	char input[120];
	char *pointer_to_parsedArgs[120];	
	char c;
	char a[20];
	int result;
	int exit_true = 0;
	getcwd(command_log_dir,100);
	getcwd(output_log_dir, 100);
	strcat(command_log_dir, "/command.log");
	strcat(output_log_dir, "/output.log");
	while(1)
	{
		Label1:
		fgets(a, 20, stdin);
		a[strcspn(a, "\n")] = '\0';
		//printf("%s\n" ,a);

		if(strcmp(a, "entry") == 0)
		{
			while(1)
			{
				if(getInput(input))
					continue;
				//printf("input = %s\n", input);
				// parse this input

				result = parse(input, pointer_to_parsedArgs);
				//printf("result= %d\n",result );
				if(result == 0)
				{	
					exit_true = 1;
					printf("Command line interpreter stopped...\n");
					goto Label1;
				}
				if(result == 1)
				{					
					execute(pointer_to_parsedArgs);
					if(log_flag == 1)
					{
						fptr2 = fopen("output.log", "a");
						if(fptr2 == NULL)
						{
							printf("\nOpen error");
							exit(0);
						}
						
						fptr1 = fopen("F1.txt", "r");
						if(fptr1 == NULL)
						{
							printf("\nOpen failed");
							exit(0);
						}
						c = fgetc(fptr1);
						while(c != EOF)
						{
							fputc(c, fptr2);
							c = fgetc(fptr1);
						}	

						fclose(fptr1);
						fclose(fptr2);
					}					

					fptr3 = fopen("F1.txt", "r");
					if(fptr3 == NULL)
					{
						printf("\nOpen Failed");
						exit(0);
					}
					c = fgetc(fptr3);
					while(c != EOF)
					{
						printf("%c", c);
						c = fgetc(fptr3);
					}
					fclose(fptr3);
				}				
			}

		}
		else if(strcmp(a, "exit") == 0)
		{
			return 0;
		}
		else 
		{	
			if(exit_true == 1)
			{
				printf("Command line interpreter exited\n");
			}
			else 
				printf("Command line interpreter not started\n");
		}
	}

	return 0;
}

/*bug changedir ~/Downloads


*/
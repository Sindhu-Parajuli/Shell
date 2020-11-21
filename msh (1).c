/*
   Name: Sindhu parajuli
   ID : 1001764819

*/

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n" // We want to split our command lineup into tokens 
						   // so we need to define what delimit our tokens.   
						   // In this case  white space                       
						   // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255 // The maximum command-line size

#define MAX_NUM_ARGUMENTS 10 // Mav shell only supports ten arguments

int main()

{

	int pid_index = 0;

	int history_index = 0;

	char *command_history[15];

	int pid_history[15];

	char *cmd_str = (char *)malloc(MAX_COMMAND_SIZE);

	int i;
	char *nth[2];
	nth[0] = (char *)malloc(255);

	//Allocating memory for the array of command history
	for (i = 0; i < 15; i++)
	{
		command_history[i] = (char *)malloc(MAX_COMMAND_SIZE);
	}
	strcpy(command_history[14], "Not Full");

	while (1)
	{
		// Print out the msh prompt
		printf("msh> ");

		// Read the command from the commandline.  The
		// maximum command that will be read is MAX_COMMAND_SIZE
		// This while command will wait here until the user
		// inputs something since fgets returns NULL when there
		// is no input
		while (!fgets(cmd_str, MAX_COMMAND_SIZE, stdin))
			;
		//cmd_str = strchr(cmd_str, ' ');
		if (cmd_str[0] == '\n')
		{

			continue;
		}

		/* Parse input */
		char *token[MAX_NUM_ARGUMENTS];

		int token_count = 0;
		int value;
		int i = 0;

		// Pointer to point to the token
		// parsed by strsep
		char *argument_ptr;

		char *working_str = strdup(cmd_str);

		// we are going to move the working_str pointer so
		// keep track of its original value so we can deallocate
		// the correct amount at the end
		//Checking if an input starts with '!'

		char *working_root = working_str;
		//Filling up the command_history array with the strings input from the users using strncpy
		if (history_index < 15)
		{
			strncpy(command_history[history_index], cmd_str, 255);
		}

		//Incase of more than 15 history indexes, the history command array is replaced by its preceding command one one by one
		//as we have to only provide the last 15 history commands back in the terminal

		else
		{
			for (i = 0; i < 14; i++)
			{

				strncpy(command_history[i], command_history[i + 1], 255);
			}
			strncpy(command_history[14], cmd_str, 255);
		}

		if (working_str[0] == '!')
		{

			//We copy the string with '!' input to the variable to extract an integer
			strncpy(nth[0], working_str, 255);

			//'atoi' to convert string to an integter to get the value
			value = atoi(&nth[0][1]) - 1;

			if (value < 15)
			{
				if (strcmp(command_history[14], "Not Full") == 0)
				{
					strcpy(working_str, command_history[value]);
					strcpy(command_history[history_index], command_history[value]);
				}

			//checking if value is within the number of commands we have given as input
				else if (value < history_index)
				{
					strcpy(working_str, command_history[value - 1]);
					strcpy(command_history[14], command_history[value - 1]);
				}

				else
				{

					printf("command not found in history \n");
				}
			}
		}
		history_index++;
		if (history_index > 14)
		{
			history_index = 15;
		}
		// Tokenize the input strings with whitespace used as the delimiter
		while (((argument_ptr = strsep(&working_str, WHITESPACE)) != NULL) &&
			   (token_count < MAX_NUM_ARGUMENTS))
		{
			token[token_count] = strndup(argument_ptr, MAX_COMMAND_SIZE);
			if (strlen(token[token_count]) == 0)
			{
				token[token_count] = NULL;
			}
			token_count++;
		}

		//Continue when the user presses 'space' or gives a new line

		//All the 'cd' activities are handled by 'chdir' function onto which token[1] is passed

		if (strcmp("cd", token[0]) == 0)
		{

			int val = chdir(token[1]);
			
		//-1 is returned by the function in cases of no directories found; ie failed cases of cd

			if (val == -1)
			{
				printf("No such directory is found. %s \n", token[1]);
			}
			continue;
			int status;
			wait(&status);
		}

		else if (strcmp("showpids", token[0]) == 0)
		{

			int i = 0;
			//Looping throught the total number of pid as refered by pid_index

			for (i = 0; i < pid_index; i++)
			{
				//Extracting the elements of pid_history array using the index i to get the 15 pids required.

				printf(" %d :  %d \n", i, pid_history[i]);
			}
			continue;
		}

		else if (strcmp("history", token[0]) == 0)
		{

			int i;
			if (strcmp(command_history[14], "Not Full") == 0)
			{
				for (i = 0; i < history_index; i++)
				{
					printf(" %d :  %s \n", i + 1, command_history[i]);
				}
			}
			else
			{
				for (i = 0; i < 15; i++)
				{
					printf(" %d :  %s \n", i + 1, command_history[i]);
				}
			}
			continue;
		}

		//Exiting code with commands 'quit' or 'exit' on the command line, when a user enters quit or exit the shell exits
		else if ((strcmp("quit", token[0]) == 0) || (strcmp("exit", token[0]) == 0))
		{
			return 0;
		}

		else
		{

			//Forking to get further processes for the mav-shell
			pid_t pid = fork();
			if (pid == 0)
			//Entering in the child process as pid == 0 is met ; as the child returns 0
			{
				//execvp function to pass tokens and run it passing its adress
				int ret = execvp(token[0], &token[0]);
				if (ret == -1)
				// -1 is returned when execvp fails and the required command is not found
				{
					printf("%s EXECVP failed : Command not found \n", token[0]);
					exit(0);
				}
			}

			else if (pid == -1)
			{

				printf("Fork failed\n");
			}

			//fork returning postive values, so else block is parent part
			else

			{
				//Filling the pid_history array until 15 pids are collected. The pids are passed after forking.
				if (pid_index < 15)
				{
					pid_history[pid_index++] = pid;
				}

				else
				{

					//Incases of more than 15 pids, new pids replace the older ones after 15, shifting one position as shown in the loop.

					for (i = 0; i < 14; i++)
					{
						pid_history[i] = pid_history[i + 1];
					}

					pid_history[14] = pid;
				}

				//Wait until the child is terminated
				int status;
				waitpid(pid, &status, 0);
				fflush(NULL);
			}

			//Working roots are freed as memory was allocated

			free(working_root);
		}
	}
	return 0;
}
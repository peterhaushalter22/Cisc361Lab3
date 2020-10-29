/*
Created by: Luke Fanizzi and Peter Haushalter
Description: This program is the main program our shell will be running on. It controls parsing the command
line and executing the functions. The arguments on the comand line are put into a char * so we can access
the arguments in each program.
*/

#include <unistd.h>  //included files and libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <glob.h>
#include <signal.h>
#include "sh.h"

pid_t childPid = (int)NULL;
/*
	This function is a callback function that is used catch SIGINT.
	Input:
		int signalNumber - The kill signal number.

*/
void sigHandler(int signalNumber){ 
	printf("received %d\n", signalNumber);
	if (signalNumber == SIGINT){
		printf("\nreceived SIGINT\n");

		if(childPid != NULL){
			kill(childPid, SIGKILL); 
		}
	}
}

int main(int argc, char **argv, char **envp){
	char buffer[MAXLINE]; //declarations
	char *arguments[MAXARGS];
	char *ptr;
	char prompt[16];
	int	status;

	for(int index = 0; index < MAXARGS; index++){
		prompt[index] = '\0'; //clears prompt
	}

	signal(SIGINT, sigHandler);

	printf("%s [%s]>> ", prompt, getcwd(NULL, 0));
	while (fgets(buffer, MAXLINE, stdin) != NULL) {

		//continues if user just enters "\n".
		if (strlen(buffer) == 1 && buffer[strlen(buffer) - 1] == '\n'){
			goto nextprompt;
		}

		if (buffer[strlen(buffer) - 1] == '\n'){
			buffer[strlen(buffer) - 1] = 0; 
		}
 

		int argumentIndex = 0; //declarations
		glob_t globPaths;
  		char **globOutput;
        char *token = strtok(buffer, " "); // tokenizes the command line for parsing

        while (token != NULL && argumentIndex < MAXARGS){	 //This loop is responsible for parsing the command line				   
			if (glob(token, 0, NULL, &globPaths) == 0) { // and storing the arguments in a charcter array
				for (globOutput = globPaths.gl_pathv; *globOutput != NULL; globOutput++){
					arguments[argumentIndex] = *globOutput;
					argumentIndex++; //increments index of new array
				}
			}else{
				arguments[argumentIndex] = token; //stores argument
				argumentIndex++; //increments index
			}

			token = strtok (NULL, " "); 
        }

		arguments[argumentIndex] = (char *) NULL; 

		//continues if "blank" command.
		if (arguments[0] == NULL){
			goto nextprompt;
		}

		if (strcmp(arguments[0], "exit") == 0) {
			printf("You have exited the shell.\n");
			return 1;
		}else if (strcmp(arguments[0], "which") == 0) {
		  		struct pathelement *path, *tmp;
            	char *cmd;

				printf("Executing built-in [which]\n");

				//continues if no agruments are given to to which
		  		if (arguments[1] == NULL) {
		    		printf("which: Too few arguments.\n");
		    		goto nextprompt;
                }
		  		path = get_path();  //gets path
                cmd = which(arguments[1], path);

                if (cmd) {
		    		printf("%s\n", cmd);
                    free(cmd);  //free memory 
                }else{
					printf("%s: Command not found\n", arguments[1]);
				}

		  		while (path) {
		     		tmp = path;
		     		path = path->next;
		     		free(tmp->element);
		     		free(tmp);
                }

		}else if(strcmp(arguments[0], "where") == 0){

			struct pathelement *path, *tmp;
            char **pathFound = NULL;

			printf("Executing built-in [where]\n");

			//continues if no agruments are given to to which
	  		if (arguments[1] == NULL) {	
				printf("where: Too few arguments.\n");
		    	goto nextprompt;
            }

			path = get_path();

            pathFound = where(arguments[1], path);

            if (pathFound){
                
				int index = 0;
				while(pathFound[index] != NULL) {
					printf("%s\n", pathFound[index]);
		 			free(pathFound[index]);
					index++;
            	}

				free(pathFound);  // frees memory

            }else{
				printf("%s: Command not found\n", arguments[1]); // prints if command is not found
			}

			while (path) {
				tmp = path;
		 		path = path->next;
		 		free(tmp->element);
		 		free(tmp); 
            }

		}else if (strcmp(arguments[0], "pwd") == 0) {
			pwd();
	    }else if(strcmp(arguments[0], "pid") == 0){
			printf("%d\n",getpid());
		}else if(strcmp(arguments[0], "kill") == 0){
			killProc(arguments);
		}else if (strcmp(arguments[0], "printenv") == 0) {
			printenv(envp, argumentIndex, arguments);
		}else if (strcmp(arguments[0], "setEnviroment") == 0) {
			setEnvironment(envp, arguments);
		}else if(strcmp(arguments[0], "prompt") == 0){
			if(arguments[1] == NULL){
				printf("Retype a new prompt\n");
			}else if(strcmp(arguments[1], "clear") == 0){
				for(int index = 0; index < MAXARGS; index++){
					prompt[index] = '\0';
				}
			}else{
				strncpy(prompt, arguments[1], MAXARGS);
			}
		}else if (strcmp(arguments[0], "cd") == 0) {
			//I did this because cd function does not like argument[1] being NULL.
			if(arguments[1] == NULL){
				cd("~");
			}else{
				cd(arguments[1]);
			}
		}else if (strcmp(arguments[1], "list") == 0) {
			list(arguments);  
		
		    }else if (strcmp(arguments[1], ">") == 0) {

			}else if (strcmp(arguments[1], ">&") == 0) {

			}else if (strcmp(arguments[1], ">>") == 0) {

			}else if (strcmp(arguments[1], ">>&") == 0) {

			}else if (strcmp(arguments[1], ">>&") == 0) {

			}else{

			struct pathelement *path, *tmp;
			char *cmd;

			childPid = fork();


			if(childPid < 0) {
				perror("fork error");
		  	}else if (childPid == 0) {
				//child

				path = get_path();
				cmd = which(arguments[0], path);
				execve(cmd, arguments, NULL);
				execve(arguments[0], arguments, NULL);
				perror("couldn't execute command");
				exit(127);
			}

		  	/* parent */
			if ((childPid = waitpid(childPid, &status, 0)) < 0){
				perror("waitpid error");
			}

			// while (path) {
			// 	tmp = path;
	     	// 	path = path->next;
	     	// 	free(tmp->element);
		 	// 	free(tmp);
            // }
        }

        nextprompt:
		globfree(&globPaths);
		printf("%s [%s]>> ", prompt, getcwd(NULL, 0));
	}

	exit(0); //exits program
}

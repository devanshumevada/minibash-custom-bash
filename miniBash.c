#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define COMMAND_INPUT_SIZE 200


void execute_single_command(char *cmd) {
    	char *args[10];
        args[0] = strtok(cmd, " ");
        int counter = 0;

        while (args[counter] != NULL){
                counter++;
                args[counter] = strtok(NULL, " ");
        }

    int pid = fork();

    if (pid == 0) {
        if (execvp(args[0],args) < 0) {
            printf("\nCommand not Found\n");
        } 
    } else if (pid > 0){
                int status;
                waitpid(pid, &status, 0);
                /*
                int exitStatus;
                if(WIFEXITED(status)){

                        exitStatus = WIFEXITED(status);

                        //printf("Child %d exited with status %d\n", pid, exitStatus);
                } */

        }
}

void execute_semicolon_commands(char *cmd) {
        int i;
        int number_of_commands = 0;
        char *processed_commands[10];
        char *semicolon_token = strtok(cmd, ";");
        while (semicolon_token != NULL) {
            //printf( " %s", semicolon_token );
            processed_commands[number_of_commands] = semicolon_token;
            number_of_commands++;
            semicolon_token = strtok(NULL, ";");
        }
        //printf("%d", count);
        for (i=0; i<number_of_commands; i++) {
           // printf("\n%s\n",processed_commands[i]);
           execute_single_command(processed_commands[i]);

        }


    

}

void execute_piped_commands(char *cmd) {

    	char *pipe_token = strtok(cmd,"|");
        int count = 0;
        char *piped_command[10];

        while(pipe_token != NULL){
                piped_command[count] = pipe_token;
                count++;
                pipe_token = strtok(NULL, "|");
        }

        int fd[10][2];
        int i;
        for(i=0;i<count;i++){
                char *args[10];
                args[0] = strtok(piped_command[i], " ");
                int counter = 0;

                while (args[counter] != NULL){
                        counter++;
                        args[counter] = strtok(NULL, " ");
                }

              

                if(i!=count-1){
                        if(pipe(fd[i])<0){
                                printf("\nPipe Creation Error\n");
                                return;
                        }
                }

		int pid = fork();
                if(pid == 0){//child1
                        if(i!=count-1 ){
                                dup2(fd[i][1],STDOUT_FILENO);
                                close(fd[i][0]);
                                close(fd[i][1]);
                        }

                        if(i!=0){
                                dup2(fd[i-1][0],STDIN_FILENO);
                                close(fd[i-1][1]);
                                close(fd[i-1][0]);
                        }
                        if (execvp(args[0],args) < 0) {
				printf("\nCommand Error\n");
				exit(1);

			}
                        
                       
                }
                if(i!=0){
                        close(fd[i-1][0]);
                        close(fd[i-1][1]);
                }
                wait(NULL); // parent will wait
        }


   

}

int check_file (int fd) {
	return fd<0;
}

void execute_redirection_commands(char *cmd, int mode) {
	
	char *args[10];
	int counter = 0;
	int fd;

	if (mode == 1) {
		
		args[0] = strtok(cmd, ">");
        	while (args[counter] != NULL){
                	counter++;
                	args[counter] = strtok(NULL, ">");
        	} 

	} else if (mode == 2) {
		args[0] = strtok(cmd, ">>");
		while (args[counter] != NULL){
			counter++;
			args[counter] = strtok(NULL, ">>");
		} 
	}
	
	
	
	
	
       

	char *argv[10];
        argv[0] = strtok(args[0], " ");
        int counter2 = 0;

        while (argv[counter2] != NULL){
                counter2++;
                argv[counter2] = strtok(NULL, " ");
        }

	int j;
	

	char *file_args[10]; // Removing white space from file name
	file_args[0] = strtok(args[counter-1], " ");
	int counter3 = 0;

	while (file_args[counter3] != NULL){
                counter3++;
                file_args[counter3] = strtok(NULL, " ");
        }

	/*
	
	printf("\nargs[0]: %s \n",args[0]);
	printf("\nargs[1]: %s \n",file_args[0]);
	printf("\ncount: %d \n",counter);
	*/

	if (counter == 2) {
		
		
		int pid = fork();
		if (pid == 0) {
			//inside child 

			if (mode == 1) {

				fd = open(file_args[0], O_WRONLY | O_CREAT, 0666);

			} else if (mode == 2) {

				fd = open(file_args[0], O_WRONLY | O_APPEND | O_CREAT, 0666); 
				

			}
			
			
						
			
			

			if (check_file(fd) == 1) {
				printf("\nCannot open: %s\n",args[1]);
				return;
			}

			dup2(fd, STDOUT_FILENO); //redirecting stdout
						
			

			if (execvp(argv[0], argv) < 0) {
				printf("\nCommand Not found\n");
				exit(1);
			}

		}

		wait(NULL); // parent waiting 
		
	} else {
		printf("\nPlease type the command in the format of type example: ls > filename.txt\n");
		return;
	}

	 
   

}

void execute_backgorund_commands(char *cmd) {

	char *args[10];
        args[0] = strtok(cmd, "&");
        int counter = 0;

        while (args[counter] != NULL){
                counter++;
                args[counter] = strtok(NULL, "&");
        }

	char *argv[10];
        argv[0] = strtok(args[0], " ");
        int counter2 = 0;

        while (args[counter2] != NULL){
                counter2++;
                argv[counter2] = strtok(NULL, " ");
        }

	int pid = fork();

	if (pid == 0) {

		if (execvp(argv[0],argv) < 0) {
            		printf("\nCommand not Found\n");
		}
        } 

	wait(NULL); 

	
 

}



void process_command(char *cmd) {

    int is_piping_present = 0;
    int is_redirectin_present = 0;
    int is_redirection_append_present = 0;
    int is_background_present = 0;
    int is_semicolon_present = 0;

    int i = 0;

    for (i=0; i<strlen(cmd); i++) {

        if (cmd[i] == '|') {
            is_piping_present = 1;
        } else if (cmd[i] == ';') {
            is_semicolon_present = 1;
        } else if (cmd[i] == '&') {
            is_background_present = 1;
        } else if (cmd[i] == '>') {

		if (cmd[i+1] == '>') {
			is_redirection_append_present = 1;
			i+=2;
			//printf("Append");

		} else {

			is_redirectin_present = 1;

		}
		

	}

    }



    if (is_piping_present == 1) {
        execute_piped_commands(cmd);
    } else if (is_background_present == 1) {
        execute_backgorund_commands(cmd);
    } else if (is_redirectin_present == 1) {
        execute_redirection_commands(cmd, 1);
    } else if (is_semicolon_present == 1) {
        execute_semicolon_commands(cmd); 
    } else if (is_redirection_append_present == 1) {
	    execute_redirection_commands(cmd, 2);

    } else {
        execute_single_command(cmd); 
    }


   

}

int main(int argc, char **argv) {

    while (1) {
        printf("\nminiBash> ");
        char command_as_input[COMMAND_INPUT_SIZE];
        char *processed_commands[10];
        gets(command_as_input);
        char *semicolon_token = strtok(command_as_input, ";");
        int number_of_commands = 0;
        while (semicolon_token != NULL) {
          
            processed_commands[number_of_commands] = semicolon_token;
            number_of_commands++;
            semicolon_token = strtok(NULL, ";");
        }
        int i;
        for (i=0; i<number_of_commands; i++) {
           
           process_command(processed_commands[i]);

        }
        
    }


    return 0;
}

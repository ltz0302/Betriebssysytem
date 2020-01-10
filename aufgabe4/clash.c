#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "plist.h"


list plist;

void zombies(){
	pid_t zombie;
	int wstatus;
	while((zombie = waitpid(-1,&wstatus,WNOHANG)) > 0){	
		char buf[sysconf(_SC_LINE_MAX)];		
		if(removeElement(&plist, zombie,buf,sizeof(buf)) > 0)
			fprintf(stderr,"Exitstatus: [%s&] = %d\n",buf,WEXITSTATUS(wstatus));
		else
			perror("removeElement");
	}
	return;
}

int printjobs(pid_t pid, const char *cmdLine){
	printf("[%d] %s &\n",pid,cmdLine);
	return 0;
}

int main () {
	char *buf;
	char *buf2;
	char *buf3;
	long int length;
	char *token;
	char *token2;
	pid_t child_pid;
	int wstatus;
	char *path;

	while(1){
		int argc = 1;
		int i = 1;
		int background = 0;

		path = malloc(PATH_MAX);
		buf = malloc(sysconf(_SC_LINE_MAX));
		buf2 = malloc(sysconf(_SC_LINE_MAX));
		buf3 = malloc(sysconf(_SC_LINE_MAX));

		//Prompt
		if(getcwd(path,PATH_MAX) == NULL){
			perror("getcwd");
			exit(EXIT_FAILURE);
		}
		else{	
			zombies();
			fprintf(stderr,"%s: ",path);
		}
		

		//Cmd parser
		fgets(buf,2*sysconf(_SC_LINE_MAX),stdin);
		
		//Ctrl+D
		if(feof(stdin)){
			printf("\n");
			exit(0);
		}

		length = strlen(buf);
		//Empty line
		if(length < 2)	continue;
		if(length > sysconf(_SC_LINE_MAX)){
			printf("Input line too long");
			continue;
		}
		//Remove '\n'
		buf[length-1] = '\0';
	
		length = strlen(buf);
		if(buf[length-1] == '&'){
			background = 1;
			buf[strlen(buf)-1] = '\0';
		}

		strcpy(buf2, buf);
		strcpy(buf3, buf);
		token = strtok(buf," \t");
		while(token!=NULL){
			argc++;
			token = strtok(NULL," \t");
		}
		
		char *argv[argc];

		token2 = strtok(buf2, " \t");
		argv[0] = token2;
		
		while(token2!=NULL){
                        token2 = strtok(NULL, "\t ");
                        argv[i] = token2;
                        i++;
                }
                argv[i-1] = NULL;
		
		if(strcmp(argv[0],"cd") == 0){
			if(chdir(argv[1]) != 0)
				perror("cd");
			continue;
		}
		
		if(strcmp(argv[0],"jobs") == 0){
			walkList(&plist,printjobs);
			continue;
		}

		child_pid = fork();
		if (child_pid == -1) {
			perror("fork");
			exit(EXIT_FAILURE);
			}
		if (child_pid == 0) {
		       	//Code executed by child	
			if(execvp(argv[0],argv) == -1){
				perror("execvp");
				exit(EXIT_FAILURE);
			}
			exit(EXIT_SUCCESS);
		}
	       	else { 
			//Background process
			if(background == 1){
				int insert = insertElement(&plist,child_pid,buf3);
				if (insert == -1){
					fprintf(stderr,"Pid %d exists\n",child_pid);
				}	
				else if(insert == -2){
					perror("insertElement");
				}
				
				continue;
			}
			else{
				//Waiting for the children
				waitpid(child_pid,&wstatus,0);
			}
			fprintf(stderr,"Exitstatus [%s] = %d\n",buf3,WEXITSTATUS(wstatus));		
		}
		free(path);
		free(buf);
		free(buf2);
		free(buf3);
		
	}
	return 0;
}


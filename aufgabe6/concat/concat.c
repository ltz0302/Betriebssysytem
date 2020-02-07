#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#define READ 0
#define WRITE 1



// seq 1 MAX | awk AWK_VORSCHRIFT | grep GREP_VORSCHRIFT > OUTPUT_FILE

int main(int argc, char *argv[]) {
	int pipefd1[2],pipefd2[2];
	pid_t child1_pid,child2_pid,child3_pid;
	int wstatus;
	char *argv_seq[] = {"seq","1",argv[1],NULL};
	char *argv_awk[] = {"awk",argv[2],NULL};
	char *argv_grep[] = {"grep",argv[3],NULL};
	int fd = 0;

	
	if(argc	== 1 || argc > 5){
		fprintf(stderr,"Usage: concat MAX AWK_VORSCHRIFT GREP_VORSCHRIFT OUTPUT_FILE\n");
	}


	if(strcmp(argv[argc-1],"-") != 0){
		if((fd = creat(argv[argc-1],S_IRUSR|S_IWUSR|S_IRGRP)) == -1){
			perror("creat");
			exit(EXIT_FAILURE);
		}
	}


	if(pipe(pipefd1) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}


	if(pipe(pipefd2) == -1){
		perror("pipe");
		exit(EXIT_FAILURE);
	}


	child1_pid = fork();
	if(child1_pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(child1_pid == 0){
		dup2(pipefd1[1],fileno(stdout));
		if(execvp("seq",argv_seq) == -1){
			perror("execvp");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}
	
	close(pipefd1[1]);

	child2_pid = fork();
	if(child2_pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(child2_pid == 0){
		dup2(pipefd1[0],fileno(stdin));
		dup2(pipefd2[1],fileno(stdout));
		if(execvp("awk",argv_awk) == -1){
			perror("execvp");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	}

	close(pipefd1[0]);
	close(pipefd2[1]);

	child3_pid = fork();
	if(child3_pid == -1){
		perror("fork");
		exit(EXIT_FAILURE);
	}
	if(child3_pid == 0){
		dup2(pipefd2[0],fileno(stdin));

		if(fd != 0){
			dup2(fd,fileno(stdout));
		}

		if(execvp("grep",argv_grep) == -1){
			perror("execvp");
			exit(EXIT_FAILURE);
		}
		exit(EXIT_SUCCESS);
	
	}

	close(pipefd2[0]);
	waitpid(child3_pid,&wstatus,0);

        exit(EXIT_SUCCESS);

	return 1;

}



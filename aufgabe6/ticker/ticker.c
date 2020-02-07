#include <stdio.h>
#include <signal.h>
#include <sys/time.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static sem_t sem;
int turn = 0;
struct timeval start;
struct timeval end;
struct timeval res;
struct timeval *time;
struct timeval race_start;
struct timeval race_end;
struct timeval sum;

static void counter(int signum);
static void cancel(int signum);

int main(int argc, char* argv[]){
	int lap;
	struct timeval fastest;
	char *endptr = NULL;
	struct sigaction act1 = {.sa_handler = counter};
	struct sigaction act2 = {.sa_handler = cancel};

	if(argc != 2){
		fprintf(stderr,"Usage: %s <number of laps>\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	if((lap = strtol(argv[1], &endptr,10) )<= 0 || strcmp(endptr,"\0") != 0){
		fprintf(stderr,"invalid number of rounds\n");
		exit(EXIT_FAILURE);
	}

	sem_init(&sem,0,0);

        if(sigaction(SIGINT,&act1,NULL) == -1){
                perror("sigaction");
                exit(EXIT_FAILURE);
        }

	if(sigaction(SIGTERM,&act2,NULL) == -1){
		perror("sigaction");
		exit(EXIT_FAILURE);
	}

	time = malloc(lap*sizeof(struct timeval));	

	pid_t pid = getpid();
	fprintf(stderr,"pid: %d\n",pid);
	fprintf(stderr,"Sinal handler installed\n");
	fprintf(stderr,"ready ...\n");
	
	while(turn <= lap){
		sem_wait(&sem);
		kill(SIGINT,pid);
		kill(SIGTERM,pid);
	}

	fastest = *time;
	for(int i=1;i<lap;i++){
		if(timercmp((time+i),&fastest,<)!=0){
			fastest = *(time+i);
		}
	}


	timersub(&race_end,&race_start,&sum);

	fprintf(stderr,"sum: %2ld:%02ld.%04ld\n", sum.tv_sec/60, sum.tv_sec%60, sum.tv_usec);
	fprintf(stderr,"fastest: %2ld:%02ld.%04ld\n", fastest.tv_sec/60, fastest.tv_sec%60, fastest.tv_usec);
	puts("bye");
	free(time);	
	return 0;
}


static void counter(int signum){
        if(signum == SIGINT){
		if(turn == 0){
			fprintf(stderr,"go!\n");

			if(gettimeofday(&start,NULL) == -1){
				perror("gettimeofday");
				exit(EXIT_FAILURE);
			}

			if(gettimeofday(&race_start,NULL) == -1){
                                perror("gettimeofday");
                                exit(EXIT_FAILURE);
                        }


		}
		if(turn > 0){
			if(gettimeofday(&end,NULL) == -1){
                                perror("gettimeofday");
                                exit(EXIT_FAILURE);
                        }
			
			if(gettimeofday(&race_end,NULL) == -1){
                                perror("gettimeofday");
                                exit(EXIT_FAILURE);
                        }

			timersub(&end,&start,&res);

			fprintf(stderr,"lap %03d %2ld:%02ld.%04ld\n", turn, res.tv_sec/60, res.tv_sec%60, res.tv_usec);
			
			*(time+turn-1) = res;
	
			start.tv_sec = end.tv_sec;
			start.tv_usec = end.tv_usec;
		}
		turn++;
		sem_post(&sem);
                return;
        }
        return;
}

static void cancel(int signum){
	if(signum == SIGTERM){
		fprintf(stderr,"race canceled\n");
		exit(0);
	}
	return;
}

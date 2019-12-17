#include "argumentParser.h"
#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <fnmatch.h>


char* command;
char** argument;
char** option;
int num_argument;
int num_option;
const char *pattern = "-*=*";


int initArgumentParser(int argc, char* argv[]) {
	if(argc==0||argv==NULL){
		errno = EINVAL;
		return -1;
	}
	command = argv[0]; 
	argument = argv+1;

	for(int i=1;i<argc;i++){
		//Compute number of argument
		if(fnmatch(pattern,argv[i],0)!=0){
			num_argument+=1;
		}
		else{	
			option = argv+i;
			num_option = argc-num_argument-1;
			//Check format
			for(int j=0;j<num_option;j++){
				if(fnmatch(pattern,option[j],0)!=0){
					errno = EINVAL;
					return -1;
				}
				
			}
			break;
		}

	}

	return 0;
}

char* getCommand(void) {
	return command;
}

int getNumberOfArguments(void) {
	return num_argument;
}

char* getArgument(int index) {
	if(index>=0&&index<num_argument) return argument[index];
	return NULL;
}

char* getValueForOption(char* keyName) {
	char* value;
	for(int i=0;i<num_option;i++){
		if(strstr(option[i],keyName)!=NULL){
			value = strchr(option[i],'=') + 1;
			return value;
		}	
	}
	return NULL;
}


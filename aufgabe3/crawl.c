#include "argumentParser.h"
#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>

//When flag = 1, use crawl with regex
int flag = 0;

//Check name. If match successful or without -name option, retuen 0
int checkName(const char *pattern, const char *name){
	if(pattern==NULL) return 0;
	else if(fnmatch(pattern,name,0)==0)	return 0;
	else return -1;
}

char checkType(char *type){
	if(type==NULL)	return ' ';
	else if(strcmp(type,"d")==0)	return 'd';
	else if(strcmp(type,"f")==0)	return 'f';
	else{
		printf("-type argument must be d or f\n");	
		return 'n';
	}
}


int checkSizeMode(char sizeMode){
	if(sizeMode=='+') return 1;
	else if(sizeMode=='-') return -1;
	else return 0;
}

//Check size. If match successful or without -size option, retuen 0
int checkSize(off_t st_size, int sizeMode, off_t size){
	if(sizeMode==1){
		if(st_size>=size) return 0;
	}
	else if(sizeMode==-1){
		size = -size;
		if(st_size<=size) return 0;
	}
	else if((sizeMode==0)&&(size!=0)){ 
		if(st_size==size) return 0;
	}
	else return 0;

	return -1;
}


int checkString(const char *file_path, regex_t *line_regex){
	FILE *stream;
	char *line;
	size_t len = 0;
	ssize_t nread;
	int line_number = 1;

	if((stream = fopen(file_path,"r"))==NULL){
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	while ((nread = getline(&line, &len, stream)) != -1) {	
		if(regexec(line_regex,line,0,NULL,0)==0)
			printf("%s:%d:%s",file_path,line_number,line);
		++line_number;
	}
	free(line);
	fclose(stream);
	return 0;
}


static void crawl(char path[], int maxdepth, const char pattern[], char type, int sizeMode, off_t size, regex_t *line_regex){
	DIR *dir = NULL;
	struct dirent *entry;
	struct stat sb;
	char *file_path;
	static int curr_depth = 0;

	if(maxdepth==0) {
                        printf("%s\n",path);
                        return;
                }


	if((dir = opendir(path))!=NULL) {

		if((file_path = calloc(1,100))==NULL)   return; 

		//Print the entered path itself
		if((curr_depth == 0)&&(checkName(pattern,path)==0)&&(type!='f')&&(checkSize(4096,sizeMode,size)==0)&&(flag==0))	printf("%s\n",path);

		curr_depth++;

     		while((entry=readdir(dir))!=NULL){
			sprintf(file_path, "%s/%s", path, entry->d_name);

			if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 )       continue;

			if(lstat(file_path,&sb)==0){

				if((S_ISREG(sb.st_mode))){	
					if((checkName(pattern,entry->d_name)==0)&&(type!='d')&&(checkSize(sb.st_size,sizeMode,size)==0)){  
						if(flag==0)	printf("%s\n",file_path);
						else	checkString(file_path,line_regex);
					}
				}
				
				else if((S_ISDIR(sb.st_mode))){		
					if((checkName(pattern,entry->d_name)==0)&&(type!='f')&&(checkSize(sb.st_size,sizeMode,size)==0)&&(flag==0))  printf("%s\n",file_path);
					if((curr_depth<maxdepth)||(maxdepth==-1))	crawl(file_path,maxdepth,pattern,type,sizeMode,size,line_regex);
					
				}
				else
			       		continue;
			}	
		}

    		free(file_path);
		file_path = NULL;
		closedir(dir);
		curr_depth--;
		
	 }
	//When parameter is not a directory or does not exist
	else{
		char *dirc, *basec, *bname, *dname;

		dirc = strdup(path);
		basec = strdup(path);
		dname = dirname(dirc);
		bname = basename(basec);

		if((dir = opendir(dname))!=NULL) {;
			while((entry=readdir(dir))!=NULL){
				if(strcmp(entry->d_name,bname)==0){
					lstat(path,&sb);
					if((checkName(pattern,entry->d_name)==0)&&(type!='d')&&(checkSize(sb.st_size,sizeMode,size)==0)){
						if(flag==0)     printf("%s\n",path);
						else    checkString(path,line_regex);
						}
					closedir(dir);
					return;
				}
			}
		}
		printf("%s: No such file or directory\n",path);
		free(dirc);
		free(basec);
		closedir(dir);
	} 

	return;
}




int main(int argc, char *argv[]) {
	int arguments;
	//for maxdepth
	char *path;
	char *maxdepth;
	int depth;
	//for name
	const char *pattern;
	//for type
	char *type;
	char n_type;
	//for size
	char *opt_size;
	off_t size;
	int sizeMode;
	//for line
	const char *regex; 
	regex_t preg;
	int errcode;
	char errbuf[128];

	initArgumentParser(argc,argv);
	arguments = getNumberOfArguments();

	if (arguments==0)	printf("Usage: %s path... [-maxdepth=n] [-name=pattern] [-type={d,f}] [-size=[+-]n] [-line=regex]\n",argv[0]);


	maxdepth = getValueForOption("maxdepth");
	pattern = getValueForOption("name");
	type = getValueForOption("type");
	opt_size = getValueForOption("size");
	regex = getValueForOption("line");

	//type
	if((n_type = checkType(type))=='n')     return 0;

	//maxdepth
	if(maxdepth!=NULL){
		depth = strtol(maxdepth,NULL,10);
		if(depth<0){
			printf("-maxdepth argument must be >= 0\n");
			return 0;
		}
	}
	else    depth = -1;

	//size
	if(opt_size!=NULL){
		sizeMode = checkSizeMode(opt_size[0]);
		size = strtol(opt_size,NULL,10);
	}
	else {
		sizeMode = 0;
		size = 0;
	}

	//regex
	if(regex!=NULL){
		flag=1;
		errcode = regcomp(&preg,regex,REG_EXTENDED);
		if(errcode!=0){
			regerror(errcode,&preg, errbuf, sizeof(errbuf));
			fprintf(stderr, "%s: regex '%s' \n", errbuf, regex);
			return 0;
		}
	}
	

	for(int i=0;i<arguments;i++){
                path = getArgument(i);
		crawl(path,depth,pattern,n_type,sizeMode,size,&preg);
        }

	if(flag==1)	regfree(&preg);
	return 0;
}

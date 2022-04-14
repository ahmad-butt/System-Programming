#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

extern int errno;

void listDir(char*);

int main(int argc, char* argv[]) {
	if (argc==1) {
		printf("Directory listing of present directory:\n");
		listDir(".");
	//	printf("Enter exaclty one directory name\n");
	//	exit(1);
	} else if(argc>=2){
		for(int i=1; i<argc; i++) {
			printf("Directory listing of %s:\n", argv[i]);
			listDir(argv[i]);	
			printf("\n");
		}
	}
	return 0;
}

void listDir(char* name) {
	struct dirent* entry;
	DIR* dp = opendir(name);
	if(dp==NULL){
		fprintf(stderr, "Cannot open directory: %s\n", name);
		return;
	}
	errno = 0;
	while((entry = readdir(dp))!=NULL) {
		if(entry==NULL && errno != 0) {
			perror("readdir failed\n");
			exit(errno);
		} else {
			if(entry->d_name[0] != '.')
				printf("%s\n", entry->d_name);
		}
	}
	closedir(dp);
}

#include <stdio.h>
#include <stdlib.h>

void printOnStdout(FILE*);

int main(int argc, char* argv[]){
	if(argc<2){
		printf("more: bad usage\nTry 'more --help' for more information.");
		exit(0);
	}
	int i=0;
	while(argv[i]!=NULL){
		FILE* fd = fopen(argv[i], "r");
		if(fd==NULL){
			perror("Can't open file\n");
			exit(1);
		}
		printOnStdout(fd);
		i++;
	}

	return 0;
}

void printOnStdout(FILE* fd) {
	char ch;
	while((ch=fgetc(fd))!=EOF){
		fputc(ch, stdout);
	}
}

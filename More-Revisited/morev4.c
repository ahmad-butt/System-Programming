#include <stdio.h>
#include <stdlib.h>

#define PAGELEN 29
#define LINELEN 512


void printOnStdout(FILE*);
char getInput(FILE*);

int main(int argc, char* argv[]){
	if(argc==1){
		printOnStdout(stdin);
	}
	int i=0;
	while(argv[++i]!=NULL){
		FILE* fd = fopen(argv[i], "r");
		if(fd==NULL){
			perror("Can't open file\n");
			exit(1);
		}
		printOnStdout(fd);
	}

	return 0;
}

void printOnStdout(FILE* fd) {
	char buff[LINELEN];
	char inp;
	int lineCount = 0;
	FILE* fd_tty = fopen("/dev/tty", "r");
	while(fgets(buff, LINELEN, fd)){
		fputs(buff, stdout);
		lineCount++;
		if(lineCount == PAGELEN){
			inp = getInput(fd_tty);
			if(inp==0){
				printf("\033[1A \033[2K \033[1G");
				return;
			}
			else if(inp==1){
				printf("\033[1A \033[2K \033[1G");
				lineCount-=1;
			}
			else if(inp==2){
				printf("\033[1A \033[2K \033[1G");
				lineCount-=PAGELEN;
			}
			else if(inp==3)
				break;
		}
	}
}

char getInput(FILE* cmdStream){
	printf("\033[7m--More(%)--\033[m");
	int inp = getc(cmdStream);
	if(inp=='q')
		return 0;
	else if(inp == '\n')
		return 1;
	else if(inp == ' ')
		return 2;
	return 3;
}

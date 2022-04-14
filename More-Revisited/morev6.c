#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>

#define PAGELEN 29
#define LINELEN 512
static int totalLines;
static int currLineCount = 29;

void printOnStdout(FILE*);
char getInput(FILE*);
void countTotalLines(FILE*);

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
		countTotalLines(fd);
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
				currLineCount+=1;
			}
			else if(inp==2){
				printf("\033[1A \033[2K \033[1G \033[0J \033[1G");
				lineCount-=PAGELEN;
				currLineCount+=PAGELEN;
			}
			else if(inp==3)
				break;
		}
	}
}

char getInput(FILE* cmdStream){
	int percentage = ((1.0*currLineCount)/totalLines)*100;
	int inp;
	struct termios info;

	//Getting Terminal attributes
	tcgetattr(fileno(cmdStream), &info);

	//Modifying terminal attributes
	struct termios save = info;
	info.c_lflag = info.c_lflag & ~ICANON;

	//Setting terminal attributes
	tcsetattr(fileno(cmdStream), TCSANOW, &info);


	if(currLineCount!=totalLines){
		printf("\033[7m--More(%d%)--\033[m",percentage);
		inp = getc(cmdStream);
		tcsetattr(fileno(cmdStream), TCSANOW, &save);
	}
	if(inp=='q')
		return 0;
	else if(inp == '\n')
		return 1;
	else if(inp == ' ')
		return 2;
	return 3;
}
void countTotalLines(FILE* fd){
	char c;
	while((c=fgetc(fd))!=EOF){
		if(c=='\n')
			totalLines+=1;
	}
	fseek(fd, SEEK_SET, 0);

}

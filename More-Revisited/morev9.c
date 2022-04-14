#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define LINELEN 512

static int PAGELEN;
static int totalLines;
static int currLineCount;
static char filename[LINELEN];

void printOnStdout(FILE*);
char getInput(FILE*);
void countTotalLines(FILE*);
void searchString(FILE*);
void openVim();

int main(int argc, char* argv[]){
	struct winsize ws;
	ioctl(0, TIOCGWINSZ, &ws);
	PAGELEN = ws.ws_row-1;
	currLineCount = PAGELEN;

	if(argc==1){
		printOnStdout(stdin);
	}
	strcpy(filename, argv[1]);
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
				exit(0);
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
			else if(inp==4){
				printf("\033[2K \033[1G");
				printf("/");
				searchString(fd);
				lineCount-=PAGELEN;
				currLineCount+=PAGELEN;
			} else if(inp==5){
				printf("\033[2K \033[1G");
				openVim();
				lineCount-=1;
				currLineCount+=1;
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
	}
	if(inp=='q')
		return 0;
	else if(inp == '\n')
		return 1;
	else if(inp == ' ')
		return 2;
	else if(inp=='/')
		return 4;
	else if(inp=='v')
		return 5;
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
void searchString(FILE* fd) {
	char str[LINELEN];
	char buff[LINELEN];
	int offset;
	fgets(str, LINELEN, stdin);
	while(fgets(buff, LINELEN, fd)) {
		//puts(buff);
		currLineCount++;
		if(strstr(buff, str)!=NULL){
			printf("\033[1A \033[2K \033[1G");
			printf("Skipping....\n");
			fputs(buff, stdout);
			printf("\033[1A \033[2K \033[1G");
			fseek(fd, offset, SEEK_SET);
			return;
		}
		offset = ftell(fd);
		
	}

}
void openVim() {
	int ret = fork();
	if(ret==0){
		char* args[] = {"vim", filename, NULL};
		execvp("/usr/bin/vim", args);
	} 
	wait(NULL);
	return;
}


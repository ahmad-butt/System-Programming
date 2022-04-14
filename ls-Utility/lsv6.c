#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>

#define LINELEN 512

extern int errno;

void listDir(char*);
void displayLongListed(char*,char*);
void uidToUser(int);
void gidToGroup(int);
void getTime(long);
void getFileType(int mode);
void getPermissions(int);

int main(int argc, char* argv[]) {
	if (argc==1) {
		printf("Directory listing of present directory:\n");
		listDir(".");
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
	struct dirent **namelist;
	int n = scandir(name, &namelist, NULL, alphasort);
	for(int i=0; i<n; i++) {
			if(namelist[i]->d_name[0] != '.') {
				//printf("%s\n", entry->d_name);
				displayLongListed(name, namelist[i]->d_name);
				printf("\n");
			}
	}
}
void displayLongListed(char* name, char* fname) {
	char buffer[LINELEN];
	snprintf(buffer, LINELEN, "%s/%s", name, fname);
	struct stat info;
	int rv = lstat(buffer, &info);
	if (rv == -1){
      		perror("stat failed");
    		exit(1);
   	}
   	getFileType(info.st_mode);
   	getPermissions(info.st_mode);
   	printf("%ld ", info.st_nlink);
   	uidToUser(info.st_uid);
   	gidToGroup(info.st_gid);
   	printf("%ld\t", info.st_size);
   	getTime(info.st_mtime);
   	if ((info.st_mode & 0170000) == 0010000) printf("%s\t", fname);
    	else if ((info.st_mode & 0170000) == 0020000) printf("\033[7m%s\033[m\t", fname);
    	else if ((info.st_mode & 0170000) == 0040000) printf("\e[0;34m %s \033[0m\t", fname);
    	else if ((info.st_mode & 0170000) == 0060000) printf("\033[7m%s033[m\t", fname);
    	else if ((info.st_mode & 0170000) == 0120000) printf("\033[35;6m%s \033[0m\t", fname);
    	else if ((info.st_mode & 0170000) == 0140000) printf("%s\t", fname);
    	else if ((info.st_mode & 0170000) == 0100000 && !strcmp(fname,"a.out")) printf("\e[0;32m%s\033[0m\t", fname);
    	else if ((info.st_mode & 0170000) == 0100000 ) printf("%s\t", fname);
}
void uidToUser(int uid){
	struct passwd* pwd = getpwuid(uid);
	printf("%s ",pwd->pw_name);  
}
void gidToGroup(int gid){
	struct group* grp = getgrgid(gid);
	printf("%s ",grp->gr_name);  
}
void getTime(long secs){
	char *ch = ctime(&secs);
	printf("  %-14.12s", ch + 4);
}
void getPermissions(int mode) {
   char str[10];
   strcpy(str, "---------");
//owner  permissions
   if((mode & 0000400) == 0000400) str[0] = 'r';
   if((mode & 0000200) == 0000200) str[1] = 'w';
   if((mode & 0000100) == 0000100) str[2] = 'x';
//group permissions
   if((mode & 0000040) == 0000040) str[3] = 'r';
   if((mode & 0000020) == 0000020) str[4] = 'w';
   if((mode & 0000010) == 0000010) str[5] = 'x';
//others  permissions
   if((mode & 0000004) == 0000004) str[6] = 'r';
   if((mode & 0000002) == 0000002) str[7] = 'w';
   if((mode & 0000001) == 0000001) str[8] = 'x';
//special  permissions
   if((mode & 0004000) == 0004000) str[2] = 's';
   if((mode & 0002000) == 0002000) str[5] = 's';
   if((mode & 0001000) == 0001000) str[8] = 't';
   printf("%s ", str);
}
void getFileType(int mode){
	if ((mode &  0170000) == 0010000) {
		
		printf("p");
	}
   else if ((mode &  0170000) == 0020000) {
		printf("c");
	}
   else if ((mode &  0170000) == 0040000) {
		printf("d");
	}
   else if ((mode &  0170000) == 0060000) {
		printf("b");
	}
   else if ((mode &  0170000) == 0100000) {
		printf("-");
	}
   else if ((mode &  0170000) == 0120000) {
		printf("l");
	}
   else if ((mode &  0170000) == 0140000) {
		printf("s");
	}
}

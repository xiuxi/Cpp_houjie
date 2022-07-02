//https://codeforwin.org/2018/03/c-program-to-list-all-files-in-a-directory-recursively.html
#include "include.h"
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

void listFiles(const char *path);

int main(){
	char cwd[PATH_MAX];
	if (getcwd(cwd, sizeof(cwd)) != NULL) {
		printf("Current working dir: %s\n", cwd);
	} else {
		perror("getcwd() error");
		return 1;
	}
    listFiles(cwd);

	return 0;
}

void listFiles(const char *path)
{
    struct dirent *dp;
    DIR *dir = opendir(path);

    if (!dir) 
        return; 

    while ((dp = readdir(dir)) != NULL)
    {
        printf("%s\n", dp->d_name);
    }
    closedir(dir);
}

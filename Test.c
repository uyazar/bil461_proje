#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h>

int main(void) 
{ 
        int desc=open("favicon.ico",0);
        struct stat stNew;
        fstat(desc,&stNew);
        time_t timeNew=stNew.st_mtime;
        struct stat stTemp;
        printf("burda %ld\n",stNew.st_size);
}



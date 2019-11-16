#include <stdio.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <fcntl.h> 
#include <pthread.h>
 

void* fun(){
    printf("selam\n");
}
int main(void) 
{ 
    pthread_t t1;
    pthread_create(&t1,NULL,fun,NULL);
    sleep(5);
    int t2=pthread_tryjoin_np(t1,NULL);
    printf("%d\n",t2);
}



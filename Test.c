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
    pthread_t* arr=malloc(2*sizeof(pthread_t));
    pthread_create(&arr[0],NULL,fun,NULL);
    pthread_create(&arr[1],NULL,fun,NULL);
    pthread_join(arr[0],NULL);
    pthread_join(arr[1],NULL);
}

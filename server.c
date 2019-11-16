#include "bil461.h"
#include "request.h"
#include <semaphore.h>
#include <string.h>

int i=0;
int* buf;
char* schedule_alg;

sem_t empty;
sem_t full;
sem_t mutex;

void getargs(int *port, int* pool_size,int* buf_size,char* schedule_alg,int argc, char *argv[])
{
    if (argc != 5) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *pool_size=atoi(argv[2]);
    *buf_size=atoi(argv[3]);
    strcpy(schedule_alg,argv[4]);

}

long get_time(char* uri){
   int file_desc = 0; 
    struct stat st;

    file_desc = open(uri,O_RDONLY); 
    if(-1 == file_desc) 
    { 
        printf("\n NULL File descriptor\n"); 
        return -1; 
    } 
 
    errno = 0; 
    if(fstat(file_desc, &st)) 
    { 
        printf("\nfstat error: [%s]\n",strerror(errno)); 
        close(file_desc); 
        return -1; 
    }
 
    close(file_desc); 
    return st.st_mtime;
}

long get_size(char* uri){
   int file_desc = 0; 
    struct stat st; 
    file_desc = open(*uri,O_RDONLY); 
 
    if(-1 == file_desc) 
    { 
        printf("\n NULL File descriptor\n"); 
        return -1; 
    } 
 
    fstat(file_desc, &st);
 
    close(file_desc); 
    return st.st_size;
}

char* get_uri(int fd){

    char* uri=malloc(sizeof(char)*MAXLINE);
    char buf[MAXLINE], method[MAXLINE], version[MAXLINE];
    char filename[MAXLINE], cgiargs[MAXLINE];
    rio_t rio;

    Rio_readinitb(&rio, fd);
    Rio_readlineb(&rio, buf, MAXLINE);

    sscanf(buf, "%s %s %s", method, uri, version);
    return uri;
}

void* enqueue(void* arg){
    int desc=*((int*)arg);
    sem_wait(&empty);
    sem_wait(&mutex);
    buf[i]=desc;
    i++;
    /*
    if(strcmp("FIFO",schedule_alg)==0){
        buf[i]=desc;
        i++;
    }*/
    if(strcmp("RFF",schedule_alg)==0){
        for(int j=0;j<i;j++){
            char* u=get_uri(buf[j]);
            long t=get_time(u+1);
            printf("%s %d %d\n",u,t,buf[j]);
        }
    }
    printf("---------------\n");

    sem_post(&mutex);
    sem_post(&full);

}

void* dequeue(void* arg){
    sem_wait(&full);
    sem_wait(&mutex);
    int conn=buf[i-1];
    buf[i-1]=-1;
    i--;
    printf("consuemer takes %d\n",conn);
    requestHandle(conn);
    Close(conn);
    pthread_t tid=pthread_self();
    printf("tid:%d\n",tid);
    sem_post(&mutex);
    sem_post(&empty);
    pthread_create(&tid,NULL,dequeue,NULL);
}



int main(int argc, char *argv[])
{

    int listenfd, connfd, port, clientlen,pool_size,buf_size;
    schedule_alg=(char*)malloc(5*sizeof(char));
    struct sockaddr_in clientaddr;

    getargs(&port,&pool_size,&buf_size,schedule_alg, argc, argv);

    buf=malloc(buf_size*sizeof(int));
    pthread_t* workers=malloc(pool_size*sizeof(pthread_t));

    sem_init(&empty,0,buf_size);
    sem_init(&full,0,0);
    sem_init(&mutex,0,1);
    /*
    for(int i=0;i<pool_size;i++){
        pthread_create(&workers[i],NULL,dequeue,NULL);
    }
    */

    listenfd = Open_listenfd(port);
    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        enqueue(&connfd);
    }

}










    


 

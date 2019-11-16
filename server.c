#include "bil461.h"
#include "request.h"
#include <semaphore.h>
#include <string.h>


long get_time(char*);
long get_size(char*);
char* get_uri(int);
void right_shift(int*,int,int);
void left_shift(int*,int,int);
//for debugging
void print_buf();
/*buf variables for fifo*/
int buf_size;
int* buf;
char* schedule_alg;

/*buf variables for RFF*/
int count=0;

sem_t empty;
sem_t full;
sem_t mutex;

void shift(int* arr,int left_index,int right_index){
    for(int j=right_index;j>=left_index;j--){
        buf[j+1]=buf[j];
    }
}

void* enqueue(void* arg){

    sem_wait(&empty);
    sem_wait(&mutex);
    
    int desc=*((int*)arg);
    //circular queue
    if(strcmp("FIFO",schedule_alg)==0){
        buf[count]=desc;
        count++;
    }
    else if(strcmp("RFF",schedule_alg)==0){
        //simple insertion wrt the last modification timestamp
        if(count==0){
            buf[0]=desc;
        }
        else{
            char* uri=get_uri(desc);
            long last_modify=get_time(uri+1);
            int k=0;
            while(k<count){
                char* temp_uri=get_uri(buf[k]);
                long temp_lmodify=get_time(temp_uri+1);
                if(last_modify>temp_lmodify)
                    break;
                k++;
            }
            if(k==count){
                buf[k]=desc;
            }
            else{
                shift(buf,k,count-1);
                buf[k]=desc;
            }

        }
        count++;

    }
    else{
        if(count==0){
            buf[0]=desc;
        }
        else{
            char* uri=get_uri(desc);
            long size_of_current=get_size(uri+1);
            int k=0;
            while(k<count){
                char* temp_uri=get_uri(buf[k]);
                long temp_size=get_size(temp_uri+1);
                if(temp_size>size_of_current)
                    break;
                k++;
            }
            if(k==count){
                buf[k]=desc;
            }
            else{
                shift(buf,k,count-1);
                buf[k]=desc;
            }

        }
        count++;     
    }

    sem_post(&mutex);
    sem_post(&full);

}

void* dequeue(void* arg){
    sem_wait(&full);
    sem_wait(&mutex);
    //dequeue
    int conn;
    if (strcmp("FIFO",schedule_alg)==0)
    {
        conn=buf[0];
        //left_shift;
    }
    else{
        conn=buf[0];
        //left shift
        for(int i=0;i<buf_size-1;i++){
            buf[i]=buf[i+1];
        }
        count--;
    }    

    requestHandle(conn);
    Close(conn);
    sem_post(&mutex);
    sem_post(&empty);
    //
    pthread_t tid=pthread_self();
    pthread_create(&tid,NULL,dequeue,NULL);
}

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
    file_desc = open(uri,O_RDONLY); 
 
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

void print_buf(){
    puts("----------buffer content-------------");
    for(int i=0;i<buf_size;i++){
        if(buf[i]!=-1){
            char* temp_uri=get_uri(buf[i]);
            long temp_time=get_time(temp_uri+1);
            long temp_size=get_size(temp_uri+1);
            printf("uri: %s time: %ld size : %ld\n",temp_uri,temp_time,temp_size);
        }

    }
     puts("------------------------------------");
}



int main(int argc, char *argv[])
{

    int listenfd, connfd, port, clientlen,pool_size;
    schedule_alg=(char*)malloc(5*sizeof(char));
    struct sockaddr_in clientaddr;

    getargs(&port,&pool_size,&buf_size,schedule_alg, argc, argv);

    buf=malloc(buf_size*sizeof(int));
    pthread_t* workers=malloc(pool_size*sizeof(pthread_t));

    for(int i=0;i<buf_size;i++)
        buf[i]=-1;

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










    


 

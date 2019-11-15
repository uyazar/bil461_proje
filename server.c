#include "bil461.h"
#include "request.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// bil461: Parse the new arguments too
void getargs(int *port, int argc, char *argv[])
{
    if (argc != 2) {
	fprintf(stderr, "Usage: %s <port>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
}

void* fun(void* i){
    int a=*((int*)i);
    puts("burda");
    printf("%d\n",a);
    sleep(5);
}
int main(int argc, char *argv[])
{

    worker_pool wp;
    initialize_pool(&wp,5);
    int a=5;
    for(int i=1;i<10;i++){
        int p=schedule(fun,&a,&wp);
        //printf("%d\n",p);
    }

    /*
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    getargs(&port, argc, argv);

    // 
    // bil461: Create some threads...
    //

    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
	// 
	// bil461: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work.
	// 
	requestHandle(connfd);

	Close(connfd);
    }*/

}


    


 

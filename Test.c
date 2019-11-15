#include "bil461.h"

void* fun(void* i){
    printf("%d\n",i);
}

int main(){
    worker_pool wp;
    initialize_pool(&wp,5);
}
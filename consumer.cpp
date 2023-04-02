#include <iostream>
#include "shared.hpp"

//Prototypes
void* consume1(void*);
void* consume2(void*);
void* cleanupShm(void*);

//Globals
struct table* sharedtable=nullptr;
const char* SHAREDMEMPATH="/assignment1";
int main(){

    

int fd=shm_open(SHAREDMEMPATH, O_RDWR,0);
sharedtable = (struct table*) mmap(NULL, sizeof(*sharedtable),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

//std::cout<<"No seg fault yet";

pthread_t cons1,cons2,cleanup;

    //Create thread for spot 1 production
    pthread_create(&cons1,NULL,&consume1,NULL);
    //Create thread for spot 2 production
    pthread_create(&cons2,NULL,&consume2,NULL);

    //wait for completion of the threads.
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);
    

    //Cleanup thread init
    pthread_create(&cleanup,NULL,&cleanupShm,NULL);
    pthread_join(cleanup,NULL);

//shm_unlink(SHAREDMEMPATH);
return 0;
}
void* consume1(void* dummyptr){
//Wait for producer to say it can read
while(sem_wait(&sharedtable->val1ProductionDone)!=-1){
    std::cout<<"Consumed val1: "<< sharedtable->val1<<std::endl<<
    "Consumed val2: "<<sharedtable->val2<<std::endl<<"Terminate process?: "<<sharedtable->terminateProcessFlag1;
    if(sharedtable->terminateProcessFlag1){
        //shm_unlink(SHAREDMEMPATH);
        sem_post(&sharedtable->cleanupShm);
        std::cout<<std::endl<<"before exit cons1";
        return dummyptr;
    }
    sem_post(&sharedtable->val1ConsumptionDone);
    
}
return dummyptr;
}
void* consume2(void* dummyptr){
//Wait for producer to say it can read
while(sem_wait(&sharedtable->val2ProductionDone)!=-1){
    std::cout<<"Consumed val1: "<< sharedtable->val1<<std::endl<<
    "Consumed val2: "<<sharedtable->val2<<std::endl
    <<"Terminate process?: "<<sharedtable->terminateProcessFlag2;
    if(sharedtable->terminateProcessFlag2){
        //shm_unlink(SHAREDMEMPATH);
        sem_post(&sharedtable->cleanupShm);
        std::cout<<std::endl<<"before exit cons2";
        return dummyptr;
    }
    sem_post(&sharedtable->val2ConsumptionDone);
    
}
return dummyptr;
}
void* cleanupShm (void* dummy){
    sem_wait(&sharedtable->cleanupShm);
    sem_wait(&sharedtable->cleanupShm);
    shm_unlink(SHAREDMEMPATH);
    std::cout<<"\nCLENAUP DONE\n";
    return dummy;
}
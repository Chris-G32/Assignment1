#include <iostream>
#include "shared.hpp"

int main(){
const char* SHAREDMEMPATH="/assignment1";
int fd=shm_open(SHAREDMEMPATH, O_RDWR,0);
struct table* sharedtable = (struct table*) mmap(NULL, sizeof(*sharedtable),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//Wait for producer to say it can read
while(sem_wait(&sharedtable->productionDone)!=-1){
    std::cout<<"Consumed val1: "<< sharedtable->val1<<std::endl<<
    "Consumed val2: "<<sharedtable->val2<<std::endl;
    std::cout<<"Terminate process?: "<<sharedtable->terminateProcess;
    if(sharedtable->terminateProcess){
        shm_unlink(SHAREDMEMPATH);
        std::cout<<std::endl<<"before exit";
        exit(EXIT_SUCCESS);
    }
    sem_post(&sharedtable->consumptionDone);
    
}
shm_unlink(SHAREDMEMPATH);
exit(EXIT_FAILURE);
}
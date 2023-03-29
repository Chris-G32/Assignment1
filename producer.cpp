// #include <sys/mman.h> //shm_open and shm_unlink
// #include <iostream>
// #include <fcntl.h>
// #include "../Shared/shared.hpp"

// int main(){
//     std::cout<<"Entry";
//     //Initialization
// // const char* SHAREDMEMPATH="/shared-mem-test";
// int fd=shm_open("/shared-mem-test", O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
// struct table *sharedtable=
// (table*)mmap(NULL, sizeof(*sharedtable),PROT_READ | PROT_WRITE,MAP_SHARED, fd, 0);

// if (sharedtable == MAP_FAILED){
//     // errExit("mmap");
// }

// //Init semaphors
// if (sem_init(&sharedtable->productionDone, 1, 0) == -1){
//     // errExit("sem_init-sem1");
// }             
// if (sem_init(&sharedtable->consumptionDone, 1, 1) == -1){
//     // errExit("sem_init-sem2");
// }

// srand(0);//Set seed for consistency
// //Produce 100 items
// for (int i = 0; i < 50; i++) {
//     //Wait for consumption to be done
//     if (sem_wait(&sharedtable->consumptionDone) == -1){
//         // errExit("sem_wait");
//     }
//     sharedtable->val1 = rand() % 201 - 100; // Generate a random number between -100 and 100
//     sharedtable->val2 = rand() % 201 - 100; // Generate a random number between -100 and 100
//      std::cout<<"Produced val1: "<<&sharedtable->val1<<std::endl;
//      std::cout<<"Produced val2: "<<&sharedtable->val2<<std::endl;

//     sem_post(&sharedtable->productionDone);
// }


// return 0;
// }
#include <sys/mman.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>  // for close()
#include <semaphore.h>
#include <errno.h>   // for errno
#include "shared.hpp"

int main() {
    const char* SHAREDMEMPATH = "/assignment1";
    
    int fd = shm_open(SHAREDMEMPATH, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        perror("shm_open");
        return 1;
    }

     if (ftruncate(fd, sizeof(struct table)) == -1) {
         perror("ftruncate");
         return 1;
     }

    struct table* sharedtable = (struct table*) mmap(NULL, sizeof(*sharedtable),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);try{
    if (sharedtable == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

     if (close(fd) == -1) {
         perror("close");
         return 1;
    }

    if (sem_init(&sharedtable->productionDone, 1, 0) == -1) {
        perror("sem_init-sem1");
        return 1;
    }

    if (sem_init(&sharedtable->consumptionDone, 1, 1) == -1) {
        perror("sem_init-sem2");
        return 1;
    }

    srand(0);

    for (int i = 0; i < 50; i++) {
        if (sem_wait(&sharedtable->consumptionDone) == -1) {
            perror("sem_wait");
            return 1;
        }
        if(i==49){
            sharedtable->terminateProcess=true;
        }
        sharedtable->val1 = rand() % 201 - 100;
        sharedtable->val2 = rand() % 201 - 100;
        std::cout << "Produced val1: " << sharedtable->val1 << std::endl;
        std::cout << "Produced val2: " << sharedtable->val2 << std::endl;
    if (sem_post(&sharedtable->productionDone) == -1) {
        perror("sem_post");
        return 1;
    }

}
sharedtable->terminateProcess=true;

}catch(...){
    sharedtable->terminateProcess=true;
    
    std::cout<<std::endl<<"before return 1";
    return 1;
}
std::cout<<std::endl<<"before exit in producer";
exit(0);
}
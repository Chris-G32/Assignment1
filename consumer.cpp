#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h> // for close()
#include <semaphore.h>
#include <time.h>
#include <errno.h> // for errno
#include "shared.hpp"

// Prototypes
void openShm();
void createThreads();
bool initSharedTable(int);
void *consume1(void *);   /*Consumes val1*/
void *consume2(void *);   /*Consumes val2*/
void *cleanupShm(void *); /*Cleans up shared memory and unlinks at end of proc */

// Globals
struct table *sharedtable = nullptr;

int main()
{
    openShm();
    createThreads();

    //End upon all threads finishing
    return 0;
}
/*Opens shared memory and maps table into this processes memory space*/
void openShm(){
    int fd = -1;
    // Try to open shared memory until it is successfully opened
    while (fd == -1)
    {
        fd = shm_open(SHAREDMEMPATH, O_RDWR, 0);
        if (fd == -1)
        {
            std::cout << "Waiting for shm to be open..." << std::endl;
            usleep(250000);
        }
        else
        {
            std::cout << "Shared memory opened" << std::endl;
        }
    }
    // Map shared memory to this process
    if(!initSharedTable(fd)){
        std::cout<<"!!! SHARED TABLE INITIALIZATION FAILED !!!";
    }

}
void createThreads(){
    pthread_t cons1, cons2, cleanup;

    // Create thread for spot 1 consumption
    pthread_create(&cons1, NULL, &consume1, NULL);
    // Create thread for spot 2 consumption
    pthread_create(&cons2, NULL, &consume2, NULL);

    // wait for completion of the threads.
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);

    // Cleanup thread init
    pthread_create(&cleanup, NULL, &cleanupShm, NULL);
    pthread_join(cleanup, NULL);
}
// This function consumes val1 from the shared table
void *consume1(void *dummyptr)
{
    // Wait for producer to say it can read
    while (sem_wait(&sharedtable->val1ProductionDone) != -1)
    {
        // Consume val1 from the shared table
        std::cout << "Consumed val1: " << sharedtable->val1 << std::endl;

        // Check if the process should terminate
        if (sharedtable->terminateProcessFlag1)
        {
            // If so, trigger cleanup and exit thread
            sem_post(&sharedtable->cleanupShm);
            return dummyptr;
        }

        // Signal producer that val1 has been consumed
        sem_post(&sharedtable->val1ConsumptionDone);
    }
    return dummyptr;
}

// This function consumes val2 from the shared table
void *consume2(void *dummyptr)
{
    // Wait for producer to say it can read
    while (sem_wait(&sharedtable->val2ProductionDone) != -1)
    {
        // Consume val2 from the shared table
        std::cout << "Consumed val2: " << sharedtable->val2 << std::endl;

        // Check if the process should terminate
        if (sharedtable->terminateProcessFlag2)
        {
            // If so, trigger cleanup and exit thread
            sem_post(&sharedtable->cleanupShm);
            return dummyptr;
        }

        // Signal producer that val2 has been consumed
        sem_post(&sharedtable->val2ConsumptionDone);
    }
    return dummyptr;
}

// This function cleans up shared memory and unlinks it
void *cleanupShm(void *dummy)
{
    sem_wait(&sharedtable->cleanupShm); // wait for cleanup semaphore
    sem_wait(&sharedtable->cleanupShm); // wait for all threads to finish before continuing
    // Unlink shared memory
    shm_unlink(SHAREDMEMPATH);

    std::cout <<std::endl <<"Consumer: Shared memory unlinked!"<<std::endl<<"Consumer: Terminating..."<<std::endl;

    return dummy;
}

// Initializes shared memory portion and the shared table for this file
bool initSharedMemory()
{
    // Open shared memory
    int fd = shm_open(SHAREDMEMPATH, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR); // Check if successfully opened
    if (fd == -1)
    {
        perror("shm_open");
        return false;
    }

    // Truncate shared memory portion to proper size
    if (ftruncate(fd, sizeof(struct table)) == -1)
    {
        perror("ftruncate");
        return false;
    }

    // Initialize the shared table and return its status
    return initSharedTable(fd);
}

// Initializes the shared table for this file
bool initSharedTable(int fd)
{
    // Map shared space to this process
    sharedtable = (struct table *)mmap(NULL, sizeof(*sharedtable),
                                       PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); // Check if the shared space is mapped successfully
    if (sharedtable == MAP_FAILED)
    {
        perror("mmap");
        return false;
    }

    // Close the file descriptor
    if (close(fd) == -1)
    {
        perror("close");
        return false;
    }

    return true;
}
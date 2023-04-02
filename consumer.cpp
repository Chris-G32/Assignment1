#include <iostream>
#include <sys/mman.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h> // for close()
#include <semaphore.h>
#include <errno.h> // for errno
#include "shared.hpp"

// Prototypes
void *consume1(void *);   /*Consumes val1*/
void *consume2(void *);   /*Consumes val2*/
void *cleanupShm(void *); /*Cleans up shared memory and unlinks at end of proc */

// Globals
struct table *sharedtable = nullptr;

int main()
{
    std::cout<<"ENTRY CONSUMER:"<<std::endl;
    int fd = shm_open(SHAREDMEMPATH, O_RDWR, 0);
    std::cout<<"FD COnsumer: "<<fd;
    sharedtable = (struct table *)mmap(NULL, sizeof(*sharedtable),
                                       PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    
 std::cout<<"No seg fault yet";

    pthread_t cons1, cons2, cleanup;

    // Create thread for spot 1 production
    pthread_create(&cons1, NULL, &consume1, NULL);
    // Create thread for spot 2 production
    pthread_create(&cons2, NULL, &consume2, NULL);

    // wait for completion of the threads.
    pthread_join(cons1, NULL);
    pthread_join(cons2, NULL);

    // Cleanup thread init
    pthread_create(&cleanup, NULL, &cleanupShm, NULL);
    pthread_join(cleanup, NULL);

    // shm_unlink(SHAREDMEMPATH);
    return 0;
}
void *consume1(void *dummyptr)
{
    // Wait for producer to say it can read
    while (sem_wait(&sharedtable->val1ProductionDone) != -1)
    {
        std::cout << "Consumed val1: " << sharedtable->val1 << std::endl;
        if (sharedtable->terminateProcessFlag1)
        {
            // shm_unlink(SHAREDMEMPATH);
            sem_post(&sharedtable->cleanupShm);
            std::cout << std::endl
                      << "before exit cons1";
            return dummyptr;
        }
        sem_post(&sharedtable->val1ConsumptionDone);
    }
    return dummyptr;
}
void *consume2(void *dummyptr)
{
    // Wait for producer to say it can read
    while (sem_wait(&sharedtable->val2ProductionDone) != -1)
    {
        std::cout << "Consumed val2: " << sharedtable->val2 << std::endl;
        if (sharedtable->terminateProcessFlag2)
        {
            // shm_unlink(SHAREDMEMPATH);
            sem_post(&sharedtable->cleanupShm);
            std::cout << std::endl
                      << "before exit cons2";
            return dummyptr;
        }
        sem_post(&sharedtable->val2ConsumptionDone);
    }
    return dummyptr;
}
void *cleanupShm(void *dummy)
{
    sem_wait(&sharedtable->cleanupShm);
    sem_wait(&sharedtable->cleanupShm);
    shm_unlink(SHAREDMEMPATH);
    std::cout << "\nCLENAUP DONE\n";
    return dummy;
}

/// @brief Initializes shared table for the file
/// @return True on success, false on failure
bool initSharedTable(int fd)
{

    // Map shared space to this process
    sharedtable = (struct table *)mmap(NULL, sizeof(*sharedtable),
                                       PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    // Check successful mmap
    if (sharedtable == MAP_FAILED)
    {
        perror("mmap");
        return false;
    }
    // Close file descriptor
    if (close(fd) == -1)
    {
        perror("close");
        return false;
    }

    return true;
}
/// @brief Initializes shared memory portion and the shared table for this file
/// @return True on success, false on failure
bool initSharedMemory()
{
    // Open shared memory
    int fd = shm_open(SHAREDMEMPATH, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    // Check that successful
    if (fd == -1)
    {
        perror("shm_open");
        return false;
    }
    // Truncate shared mem portion to proper size
    if (ftruncate(fd, sizeof(struct table)) == -1)
    {
        perror("ftruncate");
        return false;
    }
    // Init table and return its status
    return initSharedTable(fd);
}
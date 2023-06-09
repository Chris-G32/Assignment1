#include <sys/mman.h>
#include <iostream>
#include <fcntl.h>
#include <unistd.h> // for close()
#include <semaphore.h>
#include <errno.h> // for errno
#include <string.h>
#include "shared.hpp"

// Prototypes
void *produce1(void *);
void *produce2(void *);
bool initSemaphores();
bool initSharedMemory();
bool initSharedTable(int fd);
void createAndJoinThreads();

// Globals
struct table *sharedtable;
const int ITEMSTOPRODUCE=50;
using std::string;
int main()
{
    std::cout<<"Producer: Producer starting..."<<std::endl;
    // Initialization of shared memory elements and semaphores
    if (!initSharedMemory() || !initSemaphores())
    {
        exit(1);
    }
    else
    {
        // Create producer threads
        createAndJoinThreads();
    }
    std::cout << std::endl
              << "Producer: Terminating..." << std::endl;
    exit(0);
}
void *produce1(void *dummyptr)
{
    srand(1432);
    for (int i = 0; i < ITEMSTOPRODUCE; i++)
    {
        if (sem_wait(&sharedtable->val1ConsumptionDone) == -1)
        {
            perror("sem_wait");
            exit(1);
        }
        if (i == ITEMSTOPRODUCE-1)
        {
            sharedtable->terminateProcessFlag1 = true;
        }

        sharedtable->val1 = rand() % 201 - 100;
        //Formatted here to avoid errors where threads seem to get mixed up
        string output="Produced val1: " + std::to_string(sharedtable->val1) + "\n";
        std::cout << output;
        if (sem_post(&sharedtable->val1ProductionDone) == -1)
        {
            perror("sem_post");
            sharedtable->terminateProcessFlag1 = true;
            exit(1);
        }
    }
    return dummyptr;
}
void *produce2(void *dummyptr)
{
    srand(0);
    for (int i = 0; i < ITEMSTOPRODUCE; i++)
    {
        if (sem_wait(&sharedtable->val2ConsumptionDone) == -1)
        {
            perror("sem_wait");
            exit(1);
        }
        if (i == ITEMSTOPRODUCE-1)
        {
            sharedtable->terminateProcessFlag2 = true;
        }

        sharedtable->val2 = rand() % 400 - 100;
        //Formatted here to avoid errors where threads seem to get mixed up
        string output="Produced val2: " + std::to_string(sharedtable->val2) + "\n";
        std::cout << output;
        if (sem_post(&sharedtable->val2ProductionDone) == -1)
        {
            perror("sem_post");
            sharedtable->terminateProcessFlag2 = true;
            exit(1);
        }
    }
    return dummyptr;
}
/// @brief Initializes semaphores in shared memory
/// @return True on success, false on failure
bool initSemaphores()
{
    if (sem_init(&sharedtable->val1ProductionDone, 1, 0) == -1)
    {
        perror("sem_init-sem1");
        return false;
    }

    if (sem_init(&sharedtable->val1ConsumptionDone, 1, 1) == -1)
    {
        perror("sem_init-sem2");
        return false;
    }
    if (sem_init(&sharedtable->val2ProductionDone, 1, 0) == -1)
    {
        perror("sem_init-sem1");
        return false;
    }

    if (sem_init(&sharedtable->val2ConsumptionDone, 1, 1) == -1)
    {
        perror("sem_init-sem2");
        return false;
    }
    if (sem_init(&sharedtable->cleanupShm, 1, 0) == -1)
    {
        perror("sem_init-cleanupshm");
        return false;
    }
    return true;
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

/// @brief Creates the producer threads and runs them
void createAndJoinThreads()
{
    // Create producer threads
    pthread_t prod1, prod2;

    // Create thread for spot 1 production
    pthread_create(&prod1, NULL, &produce1, NULL);
    // Create thread for spot 2 production
    pthread_create(&prod2, NULL, &produce2, NULL);

    // wait for completion of the threads.
    pthread_join(prod1, NULL);
    pthread_join(prod2, NULL);
}

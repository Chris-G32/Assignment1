#ifndef SHARED_HPP_
#define SHARED_HPP_
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>

/*Path for shared memory partition*/
const char *SHAREDMEMPATH = "/assignment1";

/*Struct for a table that contains shared values and semaphores*/
struct table
{
    sem_t val1ProductionDone;  /* Indicates val1 has a new value */
    sem_t val1ConsumptionDone; /* Indicates val1 has been consumed */
    sem_t val2ProductionDone;  /* Indicates val2 has a new value */
    sem_t val2ConsumptionDone; /* Indicates val2 has been consumed */
    /* Used by consumer only to know when to cleanup the shm,
 may not be best practice to toss it in this file honestly */
    sem_t cleanupShm;
    int val1 = -1;                      /*Value one in the table*/
    int val2 = -1;                      /*Value two in the table*/
    bool terminateProcessFlag1 = false; /*Indicates that val1 will not receive any new values and its producer thread is gone*/
    bool terminateProcessFlag2 = false; /*Indicates that val2 will not receive any new values and its producer thread is gone*/
};
#endif
#ifndef SHARED_HPP_
#define SHARED_HPP_
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
// #define errExit(msg)    do { perror(msg); exit(EXIT_FAILURE); \
//                                    } while (0)

           /* Define a structure that will be imposed on the shared
              memory object */

struct table {
    sem_t  productionDone;            /* POSIX unnamed semaphore */
    sem_t  consumptionDone;            /* POSIX unnamed semaphore */
    int val1=-1;
    int val2=-1;
    bool terminateProcess=false;
};
#endif
#ifndef _MIPT_FCP_SERVER_COMMUNICATOR_H
#define _MIPT_FCP_SERVER_COMMUNICATOR_H

#include <sys/epoll.h>
#include <pthread.h>
#include <stdbool.h>

/* Structure of epoll parameters. */
typedef struct {
        int fd;
        struct epoll_event *events;
        int timeout;
} EpollArgs;

/* Structure of shared memory. */
typedef struct {
        int control_fifo_fd_read;
        int communication_fifo_fd_read;
        EpollArgs *epoll_args;
        bool running;
        pthread_mutex_t *mutex;
} SharedMemory;

/* Structure of communication thread function arguments. */
typedef struct {
        int thread_id;
        SharedMemory *shared_memory;
} CommunicationArgs;

/* The communication thread function. */
void *
communicate(void *args_void);

#endif /* _MIPT_FCP_SERVER_COMMUNICATOR_H */

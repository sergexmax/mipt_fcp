#include "../include/communicator.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/lib.h"
#include "../include/lib.h"

/* Follow control process command. */
void
follow(SharedMemory *shared_memory)
{
        EpollArgs *epoll_args;
        char *buf;
        ssize_t read_size;
        char command[COMMAND_MAX_LENGTH];

        epoll_args = shared_memory->epoll_args;

        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                EPRINTF("malloc");

        read_size = read(shared_memory->control_fifo_fd_read, buf, FIFO_ATOMIC_BLOCK_SIZE);
        printf("%lu\n", read_size);
        if (read_size != FIFO_ATOMIC_BLOCK_SIZE)
                EPRINTF("read");

        sscanf(buf, "%s", command);
        if (!strcmp(command, COMMANDS[0])) {
                epoll_ctl(epoll_args->fd, EPOLL_CTL_DEL,
                                shared_memory->control_fifo_fd_read, epoll_args->events);
                epoll_ctl(epoll_args->fd, EPOLL_CTL_DEL,
                                shared_memory->communication_fifo_fd_read, epoll_args->events);
                epoll_args->timeout = 0;
                shared_memory->running = false;
        }

        free(buf);

        return;
}

/* Transfer file to client. */
void
transfer(CommunicationArgs *args)
{
        SharedMemory *shared_memory;
        char *buf;
        ssize_t read_size;

        shared_memory = args->shared_memory;
        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                EPRINTF("malloc");

        read_size = read(shared_memory->communication_fifo_fd_read, buf, FIFO_ATOMIC_BLOCK_SIZE);
        if (read_size != FIFO_ATOMIC_BLOCK_SIZE)
                EPRINTF("read");

        pthread_mutex_unlock(shared_memory->mutex);

        printf("the %dth thread: %s\n", args->thread_id, buf);

        free(buf);
}

void *
communicate(void *args_void)
{
        CommunicationArgs *args;
        SharedMemory *shared_memory;
        EpollArgs *epoll_args;

        /* Prepare parameters. */
        args = (CommunicationArgs *)args_void;
        shared_memory = args->shared_memory;
        epoll_args = shared_memory->epoll_args;

        printf("Hello World! I am the %d thread!\n", args->thread_id);

        while(shared_memory->running) {
                int epoll_nevents; /* Number of epoll events. */

                pthread_mutex_lock(shared_memory->mutex);

                epoll_nevents = epoll_wait(epoll_args->fd, epoll_args->events, EPOLL_NEVENTS_MAX, epoll_args->timeout);
                if (epoll_nevents == 0)
                        pthread_mutex_unlock(shared_memory->mutex);

                /* Check for control command. */
                for (int i = 0; i < epoll_nevents; ++i) {
                        if (epoll_args->events[i].data.fd == shared_memory->control_fifo_fd_read) {
                                follow(shared_memory);
                                if (epoll_nevents == 1)
                                        pthread_mutex_unlock(shared_memory->mutex);
                        }
                }
                /* Check for client request. */
                for (int i = 0; i < epoll_nevents; ++i) {
                        if (epoll_args->events[i].data.fd == shared_memory->communication_fifo_fd_read)
                                transfer(args);
                }
        }

        printf("Goodbye World! I am the %dth thread!\n", args->thread_id);

        return NULL;
}

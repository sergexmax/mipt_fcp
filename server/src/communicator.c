#include "../include/communicator.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/lib.h"
#include "../include/lib.h"

void *
communicate(void *args_void)
{
        CommunicationArgs *args;
        SharedMemory *shared_memory;
        EpollArgs *epoll_args;

        args = (CommunicationArgs *)args_void;
        shared_memory = args->shared_memory;
        epoll_args = shared_memory->epoll_args;

        printf("Hello World! I am the %d thread!\n", args->thread_id);

        while(shared_memory->running) {
                int epoll_nevents; /* A number of epoll events. */

                pthread_mutex_lock(shared_memory->mutex);

                epoll_nevents = epoll_wait(epoll_args->fd, epoll_args->events, EPOLL_NEVENTS_MAX, epoll_args->timeout);

                for (int i = 0; i < epoll_nevents; ++i) {
                        char *buf;
                        ssize_t read_size;

                        buf = malloc(FIFO_ATOMIC_BLOCK_SIZE * sizeof(char));
                        if (buf == NULL)
                                EPRINTF("malloc");

                        read_size = read(epoll_args->events[i].data.fd, buf, FIFO_ATOMIC_BLOCK_SIZE);
                        if (read_size != FIFO_ATOMIC_BLOCK_SIZE)
                                EPRINTF("read");

                        if (epoll_args->events[i].data.fd == shared_memory->control_fifo_fd_read) {
                                char command[COMMAND_MAX_LENGTH];

                                sscanf(buf, "%s", command);
                                if (!strcmp(command, COMMANDS[0])) {
                                        epoll_ctl(epoll_args->fd, EPOLL_CTL_DEL,
                                                  shared_memory->control_fifo_fd_read, epoll_args->events);
                                        epoll_ctl(epoll_args->fd, EPOLL_CTL_DEL,
                                                  shared_memory->communication_fifo_fd_read, epoll_args->events);
                                        epoll_args->timeout = 0;
                                        shared_memory->running = false;
                                }
                        }

                        free(buf);
                }

                pthread_mutex_unlock(shared_memory->mutex);
        }

        printf("Goodbye World! I am the %d thread!\n", args->thread_id);

        return NULL;
}

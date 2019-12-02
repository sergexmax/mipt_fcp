#include "../include/communicator.h"

#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/lib.h"
#include "../include/lib.h"

#define DATA_PATH "/home/serge/data/"

void
futfill_request(CommunicationArgs *args); /* Futfill client request. */
void
copy_file(char *client_fifo_path, char *file_path); /* Copy file to client. */
void
follow_command(SharedMemory *shared_memory); /* Follow control command. */

/* Communication thread function. */
void
communicate(void *args_void)
{
        CommunicationArgs *args;
        SharedMemory *shared_memory;
        EpollArgs *epoll_args;

        /* Prepare variables. */
        args = (CommunicationArgs *)args_void;
        shared_memory = args->shared_memory;
        epoll_args = shared_memory->epoll_args;

        shared_memory->statuses[args->thread_id] = STATUS_IDLE;

        // printf("Hello World! I am the %d thread!\n", args->thread_id);

        while(shared_memory->running) {
                int epoll_nevents; /* Number of epoll events. */

                pthread_mutex_lock(shared_memory->mutex);

                /* Wait for control command or client request. */
                epoll_nevents = epoll_wait(epoll_args->fd, epoll_args->events, EPOLL_NEVENTS_MAX, epoll_args->timeout);

                shared_memory->statuses[args->thread_id] = STATUS_BUSY;

                /* Follow control command. */
                for (int i = 0; i < epoll_nevents; ++i) {
                        if (epoll_args->events[i].data.fd == shared_memory->control_fifo_fd_read) {
                                follow_command(shared_memory);
                                if (epoll_nevents == 1)
                                        pthread_mutex_unlock(shared_memory->mutex);
                        }
                }
                /* Futfill client request. */
                for (int i = 0; i < epoll_nevents; ++i) {
                        if (epoll_args->events[i].data.fd == shared_memory->communication_fifo_fd_read)
                                futfill_request(args);
                }

                /* Check if server was stopped (epoll timeout was set to 0). */
                if (epoll_nevents == 0)
                        pthread_mutex_unlock(shared_memory->mutex);

                shared_memory->statuses[args->thread_id] = STATUS_IDLE;
        }

        shared_memory->statuses[args->thread_id] = STATUS_SHUTTED_DOWN;

        // printf("Goodbye World! I am the %dth thread!\n", args->thread_id);
}

/* Follow control command. */
void
follow_command(SharedMemory *shared_memory)
{
        EpollArgs *epoll_args;
        char *buf;
        char command[COMMAND_MAX_LENGTH];

        /* Prepare variables. */
        epoll_args = shared_memory->epoll_args;
        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                EPRINTF("malloc");

        /* Get control command. */
        if (read(shared_memory->control_fifo_fd_read, buf, FIFO_ATOMIC_BLOCK_SIZE) != FIFO_ATOMIC_BLOCK_SIZE)
                EPRINTF("read");

        /* Follow control command. */
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
}

/* Copy file to client. */
void
copy_file(char *client_fifo_path, char *file_path)
{
        int client_fifo_fd_write;
        int file_fd_read;
        char *buf;
        ssize_t read_size;

        if ((client_fifo_fd_write = open(client_fifo_path, O_WRONLY)) < 0) {
                if (errno == ENOENT) {
                        fprintf(stderr, "open(%s): %s\n", client_fifo_path, strerror(errno));
                        return;
                } else {
                        EPRINTF("open");
                }
        }
        if ((file_fd_read = open(file_path, O_RDONLY | O_NONBLOCK)) < 0) {
                if (errno == ENOENT) {
                        fprintf(stderr, "open(%s): %s\n", file_path, strerror(errno));
                        return;
                } else {
                        EPRINTF("open");
                }
        }
        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                EPRINTF("malloc");
        while ((read_size = read(file_fd_read, buf, FIFO_ATOMIC_BLOCK_SIZE)) != 0) {
                if (read_size < 0)
                        EPRINTF("read");
                if (write(client_fifo_fd_write, buf, read_size) != read_size)
                        EPRINTF("write");
        }

        free(buf);
        close(file_fd_read);
        close(client_fifo_fd_write);
}

/* Futfill client request. */
void
futfill_request(CommunicationArgs *args)
{
        SharedMemory *shared_memory;
        char *buf;
        char pid[PID_MAX_LENGTH];
        char file_name[FILENAME_MAX_LENGTH];
        char client_fifo_path[PATHNAME_MAX_LENGTH];
        char file_path[PATHNAME_MAX_LENGTH];

        /* Prepare variables. */
        shared_memory = args->shared_memory;
        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                EPRINTF("malloc");

        /* Get client request. */
        if (read(shared_memory->communication_fifo_fd_read, buf, FIFO_ATOMIC_BLOCK_SIZE) != FIFO_ATOMIC_BLOCK_SIZE)
                EPRINTF("read");
        sscanf(buf, "%s %s", pid, file_name);
        sprintf(client_fifo_path, "%s%s.fifo", CLIENT_FIFO_PREPATH, pid);
        sprintf(file_path, "%s%s", DATA_PATH, file_name);

        printf("the %dth thread: %s %s\n", args->thread_id, pid, file_name);

        pthread_mutex_unlock(shared_memory->mutex);

        copy_file(client_fifo_path, file_path);

        free(buf);
}

#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/lib.h"
#include "../include/lib.h"
#include "../include/controller.h"
#include "../include/communicator.h"

#define NTHREADS_MAX 100
#define NTHREADS_MIN 1
#define NTHREADS_DEF 10

int
main(int argc, char *argv[], char *envp[])
{
        int msgid;
        MsgBuf msgbuf;
        ControlArgs control_args;
        pthread_t controller;
        int control_fifo_fd_read;
        int communication_fifo_fd_read;
        int epoll_fd;
        struct epoll_event control_epoll_event;
        struct epoll_event communication_epoll_event;
        struct epoll_event epoll_events[EPOLL_NEVENTS_MAX];
        int nthreads = NTHREADS_DEF; /* A number of communication threads. */
        pthread_mutex_t mutex;
        EpollArgs epoll_args;
        SharedMemory shared_memory;
        CommunicationArgs communication_args[NTHREADS_MAX];
        pthread_t communicators[NTHREADS_MAX];

        if ((msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666)) < 0)
                EPRINTF("msgget");

        control_args = (ControlArgs){msgid};
        if (pthread_create(&controller, NULL, control, &control_args))
                EPRINTF("pthread_create");

        if (msgrcv(msgid, &msgbuf, 0, 1, 0) < 0)
                EPRINTF("msgrcv");

        control_fifo_fd_read = open(CONTROL_FIFO_PATH, O_RDONLY | O_NONBLOCK);
        if (control_fifo_fd_read < 0)
                EPRINTF("open");

        if (unlink(COMMUNICATION_FIFO_PATH) && errno != ENOENT)
                EPRINTF("unlink");
        if (mkfifo(COMMUNICATION_FIFO_PATH, 0666))
                EPRINTF("mkfifo");

        communication_fifo_fd_read = open(COMMUNICATION_FIFO_PATH, O_RDONLY | O_NONBLOCK);

        epoll_fd = epoll_create1(0);
        if (epoll_fd < 0)
                EPRINTF("epoll_create1");
        memset(&control_epoll_event, 0, sizeof(struct epoll_event));
        memset(&communication_epoll_event, 0, sizeof(struct epoll_event));
        control_epoll_event.events = EPOLLIN;
        control_epoll_event.data.fd = control_fifo_fd_read;
        communication_epoll_event.events = EPOLLIN;
        communication_epoll_event.data.fd = communication_fifo_fd_read;
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, control_fifo_fd_read, &control_epoll_event))
                EPRINTF("epoll_ctl");
        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, communication_fifo_fd_read, &communication_epoll_event))
                EPRINTF("epoll_ctl");

        pthread_mutex_init(&mutex, NULL);

        epoll_args = (EpollArgs){epoll_fd, epoll_events, -1}; /* The last one is the epoll timeout, */
        shared_memory = (SharedMemory){control_fifo_fd_read, communication_fifo_fd_read, &epoll_args, true, &mutex};
        for (int i = 0; i < nthreads; ++i) {
                communication_args[i] = (CommunicationArgs){i + 1, &shared_memory};
                if (pthread_create(&communicators[i], NULL, communicate, &communication_args[i]))
                        EPRINTF("pthread_create");
        }

        printf("Hello World. I am the Server!\n");

        for (int i = 0; i < nthreads; ++i) {
                if (pthread_join(communicators[i], NULL))
                        EPRINTF("pthread_join");
        }
        if (pthread_mutex_destroy(&mutex))
                EPRINTF("pthread_mutex_destroy");
        if (close(epoll_fd))
                EPRINTF("close");
        if (close(communication_fifo_fd_read))
                EPRINTF("close");
        if (unlink(COMMUNICATION_FIFO_PATH))
                EPRINTF("unlink");
        if (close(control_fifo_fd_read))
                EPRINTF("close");
        if (pthread_join(controller, NULL))
                EPRINTF("pthread_join");
        if (msgctl(msgid, IPC_RMID, NULL))
                EPRINTF("msgctl");

        printf("Goodbye World. I am the Server!\n");

        exit(EXIT_SUCCESS);
}

#include "../include/controller.h"

#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/lib.h"
#include "../include/lib.h"

static const int FORMAT_MAX_LENGTH = COMMAND_MAX_LENGTH + 2; /* Length of decimal record does not exceed
                                                              * number itself. */

void *
control(void *args_void)
{
        ControlArgs *args;
        MsgBuf msgbuf;
        bool scanning;
        char command[COMMAND_MAX_LENGTH];
        char format[FORMAT_MAX_LENGTH];
        int control_fifo_fd_write;

        args = (ControlArgs *)args_void;

        /* Create control fifo. */
        if (unlink(CONTROL_FIFO_PATH) && errno != ENOENT)
                EPRINTF("unlink");
        if (mkfifo(CONTROL_FIFO_PATH, 0666))
                EPRINTF("mkfifo");

        printf("Hello World! I am the Controller!\n");

        /* Send message to main process. */
        msgbuf = (MsgBuf){1};
        if (msgsnd(args->msgid, &msgbuf, 0, IPC_NOWAIT))
                EPRINTF("msgsnd");

        /* Prepare parameters. */
        scanning = true;
        sprintf(format, "%%%ds", COMMAND_MAX_LENGTH - 1);
        control_fifo_fd_write = open(CONTROL_FIFO_PATH, O_WRONLY);
        if (control_fifo_fd_write < 0)
                EPRINTF("open");

        while (scanning) {
                scanf(format, command);
                /* Stop server. */
                if (!strcmp(command, COMMANDS[0])) {
                        char *buf;
                        ssize_t write_size;

                        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                                EPRINTF("malloc");
                        memset(buf, '\0', FIFO_ATOMIC_BLOCK_SIZE);
                        strcpy(buf, COMMANDS[0]);
                        write_size = write(control_fifo_fd_write, buf, FIFO_ATOMIC_BLOCK_SIZE);
                        if (write_size < 0)
                                EPRINTF("write");
                        free(buf);
                        scanning = false;
                }
                /* Print info. */
                else if (!strcmp(command, COMMANDS[1])) {
                        printf("Available commands:\n");
                        for (int i = 0; i < sizeof(COMMANDS) / sizeof(char *); ++i)
                                printf("\t%s\n", COMMANDS[i]);
                /* Print error. */
                } else {
                        printf("Unkown command '%s'.\n", command);
                        printf("Type '%s' to get help.\n", COMMANDS[1]);
                }
        }

        if (close(control_fifo_fd_write))
                EPRINTF("close");
        if (unlink(CONTROL_FIFO_PATH))
                EPRINTF("unlink");

        printf("Goodbye World! I am the Controller!\n");

        return NULL;
}

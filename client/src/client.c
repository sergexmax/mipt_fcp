#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/lib.h"

#define PID_MAX_LENGTH 16
#define FILENAME_MAX_LENGTH 32
#define PATHNAME_MAX_LENGTH 128

static const size_t MESSAGE_MAX_LENGTH = PID_MAX_LENGTH + FILENAME_MAX_LENGTH;

int
main(int argc, char *argv[], char *envp[])
{
        char pid[PID_MAX_LENGTH];
        char message[MESSAGE_MAX_LENGTH];
        char fifo_name[FILENAME_MAX_LENGTH]; /* Fifo name. */
        char fifo_path[PATHNAME_MAX_LENGTH];
        char *buf;
        int communication_fifo_fd_write;
        ssize_t write_size;

        /* Prepare parameters. */
        sprintf(pid, "%d", getpid());
        sprintf(message, "%s", pid);
        sprintf(fifo_name, "%s.fifo", pid);
        sprintf(fifo_path, "%s%s", CLIENT_FIFO_PREPATH, fifo_name);
        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                EPRINTF("malloc");
        memset(buf, '\0', FIFO_ATOMIC_BLOCK_SIZE);
        strcpy(buf, pid);

        /* Create client fifo. */
        if (unlink(fifo_path) && errno != ENOENT)
                EPRINTF("unlink");
        if (mkfifo(fifo_path, 0666))
                EPRINTF("mkfifo");

        /* Send request to server. */
        communication_fifo_fd_write = open(COMMUNICATION_FIFO_PATH, O_WRONLY);
        write_size = write(communication_fifo_fd_write, buf, FIFO_ATOMIC_BLOCK_SIZE);
        if (write_size != FIFO_ATOMIC_BLOCK_SIZE)
                EPRINTF("write");

        if (close(communication_fifo_fd_write))
                EPRINTF("close");
        if (unlink(fifo_path))
                EPRINTF("unlink");
        free(buf);

        printf("Hello World! I am Client!\n");

        exit(EXIT_SUCCESS);
}

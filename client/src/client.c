#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/lib.h"

/* Error codes. */
enum {
        WRONG_USAGE = -2,
};

static const size_t REQUEST_MAX_LENGTH = PID_MAX_LENGTH + 1 + FILENAME_MAX_LENGTH + 1;

void
send_request(char *request); /* Send request to server. */
void
get_file(char *client_fifo_path); /* Get file from server. */

int
main(int argc, char *argv[], char *envp[])
{
        char file_name[FILENAME_MAX_LENGTH];
        char pid[PID_MAX_LENGTH];
        char request[REQUEST_MAX_LENGTH];
        char client_fifo_path[FILENAME_MAX_LENGTH];

        /* Check launch options. */
        if (argc == 1) {
                fprintf(stderr, "Wrong usage.\n");
                fprintf(stderr, "Use: %s <file path>.\n", argv[0]);
                exit(WRONG_USAGE);
        }

        /* Prepare variables. */
        strcpy(file_name, argv[1]);
        sprintf(pid, "%d", getpid());
        sprintf(request, "%s %s", pid, file_name);
        sprintf(client_fifo_path, "%s%s.fifo", CLIENT_FIFO_PREPATH, pid);

        /* Create client fifo. */
        if (unlink(client_fifo_path) && errno != ENOENT)
                EPRINTF("unlink");
        if (mkfifo(client_fifo_path, 0666))
                EPRINTF("mkfifo");

        printf("Hello World! I am Client!\n");

        send_request(request);

        get_file(client_fifo_path);

        /* Exit. */
        if (unlink(client_fifo_path))
                EPRINTF("unlink");

        exit(EXIT_SUCCESS);
}

/* Send request to server. */
void
send_request(char *request)
{
        int communication_fifo_fd_write;
        char *buf;
        ssize_t write_size;

        communication_fifo_fd_write = open(COMMUNICATION_FIFO_PATH, O_WRONLY);
        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                EPRINTF("malloc");
        memset(buf, '\0', FIFO_ATOMIC_BLOCK_SIZE);
        strcpy(buf, request);
        write_size = write(communication_fifo_fd_write, buf, FIFO_ATOMIC_BLOCK_SIZE);
        if (write_size != FIFO_ATOMIC_BLOCK_SIZE)
                EPRINTF("write");

        free(buf);
        if (close(communication_fifo_fd_write))
                EPRINTF("close");
}

/* Get file from server. */
void
get_file(char *client_fifo_path)
{
        int client_fifo_fd_read;
        char *buf;
        ssize_t read_size;

        client_fifo_fd_read = open(client_fifo_path, O_RDONLY);
        if ((buf = malloc(FIFO_ATOMIC_BLOCK_SIZE)) == NULL)
                EPRINTF("malloc");
        while ((read_size = read(client_fifo_fd_read, buf, FIFO_ATOMIC_BLOCK_SIZE)) > 0) {
                if (write(1, buf, read_size) != read_size)
                        EPRINTF("write");
        }
        if (read_size < 0)
                EPRINTF("read");

        free(buf);
        if (close(client_fifo_fd_read))
                EPRINTF("close");
}

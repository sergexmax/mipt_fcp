#ifndef _MIPT_FCP_SERVER_LIB_H
#define _MIPT_FCP_SERVER_LIB_H

#define COMMAND_MAX_LENGTH 16

#define CONTROL_FIFO_PATH "./server/tmp/control.fifo"
#define COMMUNICATION_FIFO_PATH "./tmp/communication.fifo"

#define EPOLL_NEVENTS_MAX 2 /* The max number of epoll events. */

static const char * const COMMANDS[] = {"stop", "info"};

/* The message buffer. */
typedef struct {
        long mtype;
} MsgBuf;

#endif /* _MIPT_FCP_SERVER_LIB_H */

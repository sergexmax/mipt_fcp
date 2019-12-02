#ifndef _MIPT_FCP_SERVER_LIB_H
#define _MIPT_FCP_SERVER_LIB_H

#define COMMAND_MAX_LENGTH 16

#define CONTROL_FIFO_PATH "./server/tmp/control.fifo"

#define EPOLL_NEVENTS_MAX 2 /* Max number of epoll events. */

/* Statuses of threads. */
enum {
    STATUS_NOT_STARTED = 0,
    STATUS_IDLE,
    STATUS_BUSY,
    STATUS_SHUTTED_DOWN
};

static const char * const STATUSES[] = {"Not started up", "Idle", "Busy", "Shutted down"};
static const char * const COMMANDS[] = {"stop", "info", "status"};

#endif /* _MIPT_FCP_SERVER_LIB_H */

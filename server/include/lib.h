#ifndef _MIPT_FCP_SERVER_LIB_H
#define _MIPT_FCP_SERVER_LIB_H

#define COMMAND_MAX_LENGTH 16

#define CONTROL_FIFO_PATH "./server/tmp/control.fifo"

#define EPOLL_NEVENTS_MAX 2 /* Max number of epoll events. */

static const char * const COMMANDS[] = {"stop", "info"};

#endif /* _MIPT_FCP_SERVER_LIB_H */

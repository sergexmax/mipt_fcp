#ifndef _MIPT_FCP_SERVER_CONTROLLER_H
#define _MIPT_FCP_SERVER_CONTROLLER_H

#define CONTROLLER_TO_MAIN_MTYPE 1

/* Message buffer. */
typedef struct {
        long mtype;
} MsgBuf;

/* Structure of control thread function arguments. */
typedef struct {
        int msgid;
        int nthreads;
        int *statuses;
} ControlArgs;

/* Control thread function. */
void
control(void *args_void);

#endif /* _MIPT_FCP_SERVER_CONTROLLER_H */

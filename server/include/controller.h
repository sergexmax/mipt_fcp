#ifndef _MIPT_FCP_SERVER_CONTROLLER_H
#define _MIPT_FCP_SERVER_CONTROLLER_H

/* Structure of control thread function arguments. */
typedef struct {
        int msgid;
} ControlArgs;

/* Control thread function. */
void *
control(void *args_void);

#endif /* _MIPT_FCP_SERVER_CONTROLLER_H */

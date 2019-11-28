#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../include/lib.h"
#include "../include/controller.h"

int
main(int argc, char *argv[], char *envp[])
{
        pthread_t controller;
        ControlArgs control_args;

        printf("Hello World. I am Server!\n");

        /* Launch control process. */
        control_args = (ControlArgs){};
        if (pthread_create(&controller, NULL, control, &control_args)) {
                eprintf("pthread_create");
        }

        if (pthread_join(controller, NULL)) {
                eprintf("pthread_join");
        }

        exit(EXIT_SUCCESS);
}

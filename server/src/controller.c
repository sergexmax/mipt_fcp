#include "../include/controller.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMMAND_MAX_LENGTH 16
static const int FORMAT_MAX_LENGTH = COMMAND_MAX_LENGTH + 2; /* Length of decimal record does not exceed
                                                              * number itself. */

static const char * const COMMANDS[] = {"stop", "info"};

void *
control(void *args_void)
{
        bool scanning;
        char command[COMMAND_MAX_LENGTH];
        char format[FORMAT_MAX_LENGTH];

        printf("Hello World! I am Controller!\n");

        scanning = true;
        sprintf(format, "%%%ds", COMMAND_MAX_LENGTH - 1);
        while (scanning) {
                scanf(format, command);
                if (!strcmp(command, COMMANDS[0])) {
                        scanning = false;
                } else if (!strcmp(command, COMMANDS[1])) {
                        printf("Available commands:\n");
                        for (int i = 0; i < sizeof(COMMANDS) / sizeof(char *); ++i) {
                                printf("\t%s\n", COMMANDS[i]);
                        }
                } else {
                        printf("Unkown command '%s'.\n", command);
                        printf("Type '%s' to get help.\n", COMMANDS[1]);
                }
        }

        return NULL;
}

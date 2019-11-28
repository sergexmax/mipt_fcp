#ifndef _FCP_LIB_H
#define _FCP_LIB_H

/* Colored text. */
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_RESET "\x1b[0m"

/* Error message. */
#define eprintf(command)                                                              \
        fprintf(stderr,                                                               \
                ANSI_COLOR_RED "File: " ANSI_COLOR_RESET "%s\n"                       \
                ANSI_COLOR_RED "Function: " ANSI_COLOR_RESET "%s\n"                   \
                ANSI_COLOR_RED "Line: " ANSI_COLOR_RESET "%d\n"                       \
                ANSI_COLOR_RED "Error message: " ANSI_COLOR_RESET command "(): %s\n", \
                __FILE__, __func__, __LINE__, strerror(errno));                       \
        exit(errno);

#endif /* _FCP_LIB_H */

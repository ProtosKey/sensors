#ifndef SERVER_COMMANDS_H
#define SERVER_COMMANDS_H

/* Handles one command line from a client ("status") and returns the JSON
 * reply (malloc'ed, without trailing newline). Caller frees. NULL on OOM. */
char *command_handle(const char *line);

#endif

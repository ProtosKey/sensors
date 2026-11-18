#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

/*
 * Unix-socket server. Connections stay open: a client may send many
 * commands, one per line ("status\n"), and gets one JSON line per command.
 * Runs until SIGINT/SIGTERM. Returns 0 on clean shutdown, -1 on setup error.
 */
int server_run(const char *socket_path);

#endif

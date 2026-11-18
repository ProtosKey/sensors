#include "server/server.h"
#include "server/commands.h"
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_CLIENTS 16
#define LINE_CAPACITY 256

typedef struct {
  int fd; /* -1 when the slot is free */
  size_t len;
  char buf[LINE_CAPACITY];
} client_t;

static volatile sig_atomic_t stop_requested;

static void on_signal(int sig) {
  (void)sig;
  stop_requested = 1;
}

/* No SA_RESTART: poll() must return EINTR so the loop notices the flag. */
static void signals_install(void) {
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = on_signal;
  sigaction(SIGINT, &sa, NULL);
  sigaction(SIGTERM, &sa, NULL);
}

/* Creates the listening socket, readable only by the current user. */
static int listener_open(const char *path) {
  struct sockaddr_un addr;
  if (strlen(path) >= sizeof(addr.sun_path)) {
    fprintf(stderr, "socket path too long: %s\n", path);
    return -1;
  }

  int fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0);
  if (fd == -1) {
    perror("socket");
    return -1;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, path);
  unlink(path);

  mode_t old_umask = umask(077);
  int bound = bind(fd, (struct sockaddr *)&addr, sizeof(addr));
  umask(old_umask);

  if (bound == -1 || listen(fd, 8) == -1) {
    perror("bind/listen");
    close(fd);
    return -1;
  }
  return fd;
}

static int send_all(int fd, const char *data, size_t len) {
  while (len > 0) {
    ssize_t n = send(fd, data, len, MSG_NOSIGNAL);
    if (n < 0) {
      if (errno == EINTR) {
        continue;
      }
      return -1; /* includes EAGAIN: client is not reading, drop it */
    }
    data += n;
    len -= (size_t)n;
  }
  return 0;
}

/* Answers one command line. Returns -1 if the client must be dropped. */
static int client_answer(client_t *client, char *line) {
  line[strcspn(line, "\r")] = '\0';

  char *reply = command_handle(line);
  if (!reply) {
    return -1;
  }

  int failed = send_all(client->fd, reply, strlen(reply)) != 0 ||
               send_all(client->fd, "\n", 1) != 0;
  free(reply);
  return failed ? -1 : 0;
}

/* Answers every complete line in the buffer. -1 if the client must go. */
static int client_process_lines(client_t *client) {
  char *newline;
  while ((newline = memchr(client->buf, '\n', client->len)) != NULL) {
    *newline = '\0';
    if (client_answer(client, client->buf) != 0) {
      return -1;
    }

    size_t consumed = (size_t)(newline - client->buf) + 1;
    client->len -= consumed;
    memmove(client->buf, client->buf + consumed, client->len);
  }
  return client->len == LINE_CAPACITY ? -1 : 0; /* line too long */
}

/* Reads available bytes. -1 if the client disconnected or misbehaved. */
static int client_read(client_t *client) {
  ssize_t n = read(client->fd, client->buf + client->len,
                   LINE_CAPACITY - client->len);
  if (n == 0) {
    return -1;
  }
  if (n < 0) {
    return (errno == EAGAIN || errno == EINTR) ? 0 : -1;
  }

  client->len += (size_t)n;
  return client_process_lines(client);
}

static void client_close(client_t *client) {
  close(client->fd);
  client->fd = -1;
  client->len = 0;
}

static void client_accept(int listener, client_t *clients) {
  int fd = accept4(listener, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
  if (fd == -1) {
    return;
  }

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].fd == -1) {
      clients[i].fd = fd;
      clients[i].len = 0;
      return;
    }
  }
  close(fd); /* no free slot */
}

/* Fills `fds` with the listener plus active clients; returns the count and
 * maps fds[i] (i >= 1) to its client in `owners`. */
static int poll_set_build(struct pollfd *fds, client_t **owners, int listener,
                          client_t *clients) {
  int count = 0;
  fds[count++] = (struct pollfd){.fd = listener, .events = POLLIN};

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].fd != -1) {
      owners[count] = &clients[i];
      fds[count++] = (struct pollfd){.fd = clients[i].fd, .events = POLLIN};
    }
  }
  return count;
}

static void loop_run(int listener, client_t *clients) {
  struct pollfd fds[MAX_CLIENTS + 1];
  client_t *owners[MAX_CLIENTS + 1];

  while (!stop_requested) {
    int count = poll_set_build(fds, owners, listener, clients);
    if (poll(fds, (nfds_t)count, -1) < 0) {
      continue; /* EINTR: re-check stop_requested */
    }

    for (int i = 1; i < count; i++) {
      if (fds[i].revents & (POLLIN | POLLHUP | POLLERR)) {
        if (client_read(owners[i]) != 0) {
          client_close(owners[i]);
        }
      }
    }
    if (fds[0].revents & POLLIN) {
      client_accept(listener, clients);
    }
  }
}

int server_run(const char *socket_path) {
  signals_install();

  int listener = listener_open(socket_path);
  if (listener == -1) {
    return -1;
  }

  client_t clients[MAX_CLIENTS];
  for (int i = 0; i < MAX_CLIENTS; i++) {
    clients[i].fd = -1;
    clients[i].len = 0;
  }

  printf("daemon listening on %s\n", socket_path);
  fflush(stdout);
  loop_run(listener, clients);

  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (clients[i].fd != -1) {
      client_close(&clients[i]);
    }
  }
  close(listener);
  unlink(socket_path);
  return 0;
}

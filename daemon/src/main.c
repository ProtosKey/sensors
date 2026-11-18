#include "server/server.h"
#include <stdio.h>
#include <stdlib.h>

static int socket_path_get(char *buf, size_t size) {
  const char *runtime_dir = getenv("XDG_RUNTIME_DIR");
  int len = snprintf(buf, size, "%s/asus-hub.sock",
                     (runtime_dir && *runtime_dir) ? runtime_dir : "/tmp");
  return (len < 0 || (size_t)len >= size) ? -1 : 0;
}

int main(void) {
  char path[108];
  if (socket_path_get(path, sizeof(path)) != 0) {
    fprintf(stderr, "socket path is too long\n");
    return EXIT_FAILURE;
  }
  return server_run(path) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

#include "core/file_utils.h"
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define READ_CHUNK 256

#define BAD_READ_LONG_LONG -1
#define SUC_READ_LONG_LONG 0

char *read_text(const char *path) {
  char *buffer = NULL;
  int fd = open(path, O_RDONLY);
  if (fd == -1) {
    goto error_exit;
  }

  size_t capacity = READ_CHUNK;
  buffer = malloc(capacity * sizeof(char));
  if (buffer == NULL) {
    goto error_exit;
  }

  size_t total_bytes = 0;
  ssize_t bytes_read = 0;
  while (1) {
    if (capacity - total_bytes <= 1) {
      capacity *= 2;
      char *new_buffer = realloc(buffer, capacity * sizeof(char));
      if (new_buffer == NULL) {
        goto error_exit;
      }
      buffer = new_buffer;
    }

    bytes_read = read(fd, buffer + total_bytes, capacity - total_bytes - 1);
    if (bytes_read == 0) {
      break;
    }
    if (bytes_read == -1) {
      if (errno == EINTR) {
        continue;
      }
      goto error_exit;
    }
    total_bytes += bytes_read;
  }

  close(fd);
  buffer[total_bytes] = '\0';
  if (total_bytes > 0 && buffer[total_bytes - 1] == '\n') {
    buffer[total_bytes - 1] = '\0';
  }

  return buffer;
error_exit:
  if (fd != -1)
    close(fd);
  free(buffer);
  return NULL;
}

int read_long_long(const char *path, long long *out) {
  char *text = read_text(path);
  if (!text) {
    goto error_exit;
  }

  char *end = NULL;
  long long value = strtoll(text, &end, 10);
  int parsed = (end != text);
  free(text);
  if (!parsed) {
    goto error_exit;
  }

  *out = value;
  return SUC_READ_LONG_LONG;
error_exit:
  return BAD_READ_LONG_LONG;
}

char *link_basename(const char *path) {
  char target[PATH_MAX];
  ssize_t len = readlink(path, target, sizeof(target) - 1);
  if (len <= 0) {
    return NULL;
  }
  target[len] = '\0';

  const char *slash = strrchr(target, '/');
  return strdup(slash ? slash + 1 : target);
}

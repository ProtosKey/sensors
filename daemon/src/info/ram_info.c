#include "info/ram_info.h"
#include "core/file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static long long meminfo_kib(const char *text, const char *key) {
  const char *line = strstr(text, key);
  long long value;

  if (!line || sscanf(line + strlen(key), " %lld", &value) != 1) {
    return -1;
  }
  return value;
}

int ram_info_read(long long *total_mib, long long *used_mib) {
  char *text = read_text("/proc/meminfo");
  if (!text) {
    return -1;
  }

  long long total = meminfo_kib(text, "MemTotal:");
  long long available = meminfo_kib(text, "MemAvailable:");
  free(text);

  if (total < 0 || available < 0) {
    return -1;
  }
  *total_mib = total / 1024;
  *used_mib = (total - available) / 1024;
  return 0;
}

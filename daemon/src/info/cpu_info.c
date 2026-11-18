#include "info/cpu_info.h"
#include "core/file_utils.h"
#include <stdlib.h>
#include <string.h>

char *cpu_info_model(void) {
  char *cpuinfo = read_text("/proc/cpuinfo");
  if (!cpuinfo) {
    return NULL;
  }

  char *model = NULL;
  char *line = strstr(cpuinfo, "model name");
  char *value = line ? strchr(line, ':') : NULL;
  if (value) {
    value++;
    value += strspn(value, " \t");
    value[strcspn(value, "\n")] = '\0';
    model = strdup(value);
  }

  free(cpuinfo);
  return model;
}

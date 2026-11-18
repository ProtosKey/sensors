#include "core/proc.h"
#include <stdio.h>
#include <string.h>

#define LINE_SIZE 256

char *proc_read_line(const char *cmd) {
  FILE *pipe = popen(cmd, "r");
  if (!pipe) {
    return NULL;
  }

  char line[LINE_SIZE];
  if (!fgets(line, sizeof(line), pipe)) {
    line[0] = '\0';
  }
  line[strcspn(line, "\n")] = '\0';

  if (pclose(pipe) == -1) {
    return NULL;
  }
  return strdup(line);
}

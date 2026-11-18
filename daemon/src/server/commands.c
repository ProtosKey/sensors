#include "server/commands.h"
#include "stats/stats.h"
#include <string.h>

#define ERR_COLLECT "{\"error\":\"failed to collect\"}"
#define ERR_UNKNOWN "{\"error\":\"unknown command\"}"

char *command_handle(const char *line) {
  if (strcmp(line, "status") == 0) {
    char *json = stats_collect();
    return json ? json : strdup(ERR_COLLECT);
  }
  return strdup(ERR_UNKNOWN);
}

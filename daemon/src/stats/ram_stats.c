#include "stats/ram_stats.h"
#include "info/ram_info.h"

cJSON *ram_stats(void) {
  cJSON *ram = cJSON_CreateObject();
  long long total, used;

  if (ram_info_read(&total, &used) == 0) {
    cJSON_AddNumberToObject(ram, "used", (double)used);
    cJSON_AddNumberToObject(ram, "total", (double)total);
  }
  return ram;
}

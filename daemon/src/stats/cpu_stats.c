#include "stats/cpu_stats.h"
#include "info/cpu_info.h"
#include "stats/pick.h"
#include <stdlib.h>

cJSON *cpu_stats(const cJSON *devices) {
  cJSON *cpu = cJSON_CreateObject();

  char *model = cpu_info_model();
  cJSON_AddStringToObject(cpu, "model", model ? model : "unknown");
  free(model);

  const cJSON *device;
  cJSON_ArrayForEach(device, devices) {
    if (pick_has_category(device, "cpu")) {
      pick_add_value(cpu, "temp", pick_sensor(device, "temp"));
      break;
    }
  }
  return cpu;
}

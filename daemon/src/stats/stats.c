#include "stats/stats.h"
#include "hwmon/hwmon.h"
#include "info/profile.h"
#include "stats/cpu_stats.h"
#include "stats/fan_stats.h"
#include "stats/gpu_stats.h"
#include "stats/ram_stats.h"
#include <cjson/cJSON.h>
#include <stdlib.h>

static void add_profile(cJSON *root) {
  char *profile = profile_active();
  cJSON_AddStringToObject(root, "profile", profile ? profile : "unknown");
  free(profile);
}

char *stats_collect(void) {
  cJSON *root = cJSON_CreateObject();
  cJSON *devices = hwmon_scan();
  if (!root || !devices) {
    cJSON_Delete(root);
    cJSON_Delete(devices);
    return NULL;
  }

  add_profile(root);
  cJSON_AddItemToObject(root, "cpu", cpu_stats(devices));
  cJSON_AddItemToObject(root, "gpu", gpu_stats(devices));
  cJSON_AddItemToObject(root, "fans", fan_stats(devices));
  cJSON_AddItemToObject(root, "ram", ram_stats());

  char *json = cJSON_PrintUnformatted(root);
  cJSON_Delete(root);
  cJSON_Delete(devices);
  return json;
}

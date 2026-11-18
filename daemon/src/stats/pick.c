#include "stats/pick.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int pick_has_category(const cJSON *device, const char *category) {
  const cJSON *value = cJSON_GetObjectItem(device, "category");
  return cJSON_IsString(value) && strcmp(value->valuestring, category) == 0;
}

const cJSON *pick_sensor(const cJSON *device, const char *type) {
  const cJSON *sensors = cJSON_GetObjectItem(device, "sensors");
  const cJSON *sensor;
  cJSON_ArrayForEach(sensor, sensors) {
    const cJSON *kind = cJSON_GetObjectItem(sensor, "type");
    if (cJSON_IsString(kind) && strcmp(kind->valuestring, type) == 0) {
      return sensor;
    }
  }
  return NULL;
}

void pick_add_value(cJSON *object, const char *key, const cJSON *sensor) {
  const cJSON *value = sensor ? cJSON_GetObjectItem(sensor, "value") : NULL;
  if (cJSON_IsNumber(value)) {
    cJSON_AddNumberToObject(object, key, value->valuedouble);
  }
}

int pick_is_integrated_gpu(const cJSON *device) {
  const cJSON *pci = cJSON_GetObjectItem(device, "device");
  if (!pick_has_category(device, "gpu") || !cJSON_IsString(pci)) {
    return 0;
  }

  char path[256];
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/mem_info_vram_vendor",
           pci->valuestring);
  return access(path, F_OK) != 0;
}

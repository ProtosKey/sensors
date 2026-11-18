#include "stats/fan_stats.h"
#include <string.h>

static int is_generic_label(const char *label) {
  if (strncmp(label, "fan", 3) != 0 || label[3] == '\0') {
    return 0;
  }
  return strspn(label + 3, "0123456789") == strlen(label + 3);
}

static const char *fan_label(const cJSON *sensor) {
  const cJSON *label = cJSON_GetObjectItem(sensor, "label");
  return cJSON_IsString(label) ? label->valuestring : "";
}

static int is_fan(const cJSON *sensor) {
  const cJSON *type = cJSON_GetObjectItem(sensor, "type");
  return cJSON_IsString(type) && strcmp(type->valuestring, "fan") == 0;
}

static int has_named_fan(const cJSON *devices) {
  const cJSON *device, *sensor;
  cJSON_ArrayForEach(device, devices) {
    cJSON_ArrayForEach(sensor, cJSON_GetObjectItem(device, "sensors")) {
      if (is_fan(sensor) && !is_generic_label(fan_label(sensor))) {
        return 1;
      }
    }
  }
  return 0;
}

cJSON *fan_stats(const cJSON *devices) {
  cJSON *fans = cJSON_CreateArray();
  int named_only = has_named_fan(devices);
  const cJSON *device, *sensor;

  cJSON_ArrayForEach(device, devices) {
    cJSON_ArrayForEach(sensor, cJSON_GetObjectItem(device, "sensors")) {
      if (!is_fan(sensor) ||
          (named_only && is_generic_label(fan_label(sensor)))) {
        continue;
      }
      cJSON *fan = cJSON_CreateObject();
      cJSON_AddStringToObject(fan, "label", fan_label(sensor));
      cJSON_AddNumberToObject(
          fan, "rpm", cJSON_GetObjectItem(sensor, "value")->valuedouble);
      cJSON_AddItemToArray(fans, fan);
    }
  }
  return fans;
}

#ifndef STATS_PICK_H
#define STATS_PICK_H

#include <cjson/cJSON.h>

int pick_has_category(const cJSON *device, const char *category);
const cJSON *pick_sensor(const cJSON *device, const char *type);
void pick_add_value(cJSON *object, const char *key, const cJSON *sensor);
int pick_is_integrated_gpu(const cJSON *device);

#endif

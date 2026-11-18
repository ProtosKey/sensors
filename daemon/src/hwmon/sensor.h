#ifndef HWMON_SENSOR_H
#define HWMON_SENSOR_H

#include <cjson/cJSON.h>

cJSON *sensor_collect(const char *hwmon_dir);

#endif

#include "hwmon/hwmon.h"
#include "core/file_utils.h"
#include "hwmon/category.h"
#include "hwmon/sensor.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HWMON_ROOT "/sys/class/hwmon"
#define PATH_SIZE 512

static int count_driver(const cJSON *devices, const char *driver) {
  int count = 0;
  const cJSON *device;
  cJSON_ArrayForEach(device, devices) {
    const cJSON *name = cJSON_GetObjectItem(device, "driver");
    if (cJSON_IsString(name) && strcmp(name->valuestring, driver) == 0) {
      count++;
    }
  }
  return count;
}

static cJSON *device_build(const char *entry, const char *dir,
                           const char *driver, int occurrence, cJSON *sensors) {
  cJSON *device = cJSON_CreateObject();
  if (!device) {
    cJSON_Delete(sensors);
    return NULL;
  }

  char id[128];
  snprintf(id, sizeof(id), "%s-%d", driver, occurrence);
  cJSON_AddStringToObject(device, "id", id);
  cJSON_AddStringToObject(device, "hwmon", entry);
  cJSON_AddStringToObject(device, "driver", driver);
  cJSON_AddStringToObject(device, "category", category_of_driver(driver));

  char link[PATH_SIZE + 16];
  snprintf(link, sizeof(link), "%s/device", dir);
  char *bus_id = link_basename(link);
  if (bus_id) {
    cJSON_AddStringToObject(device, "device", bus_id);
    free(bus_id);
  }

  cJSON_AddItemToObject(device, "sensors", sensors);
  return device;
}

static void device_scan(cJSON *devices, const char *entry) {
  char dir[PATH_SIZE], name_path[PATH_SIZE + 16];
  snprintf(dir, sizeof(dir), "%s/%s", HWMON_ROOT, entry);
  snprintf(name_path, sizeof(name_path), "%s/name", dir);

  char *driver = read_text(name_path);
  cJSON *sensors = sensor_collect(dir);

  if (driver && cJSON_GetArraySize(sensors) > 0) {
    int occurrence = count_driver(devices, driver);
    cJSON *device = device_build(entry, dir, driver, occurrence, sensors);
    if (device) {
      cJSON_AddItemToArray(devices, device);
    }
  } else {
    cJSON_Delete(sensors);
  }
  free(driver);
}

cJSON *hwmon_scan(void) {
  cJSON *devices = cJSON_CreateArray();
  struct dirent **entries = NULL;

  int count = scandir(HWMON_ROOT, &entries, NULL, versionsort);
  for (int i = 0; i < count; i++) {
    if (strncmp(entries[i]->d_name, "hwmon", 5) == 0) {
      device_scan(devices, entries[i]->d_name);
    }
    free(entries[i]);
  }
  free(entries);
  return devices;
}

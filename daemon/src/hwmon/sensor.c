#include "hwmon/sensor.h"
#include "core/file_utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PATH_SIZE 512

typedef struct {
  const char *prefix;
  const char *type;
  const char *unit;
  double divisor;
} sensor_kind_t;

#define COUNT(table) (sizeof(table) / sizeof((table)[0]))

static const sensor_kind_t KINDS[] = {
    {"temp", "temp", "C", 1000.0},        {"fan", "fan", "RPM", 1.0},
    {"in", "voltage", "V", 1000.0},       {"curr", "current", "A", 1000.0},
    {"power", "power", "W", 1000000.0},   {"freq", "freq", "MHz", 1000000.0},
    {"energy", "energy", "J", 1000000.0}, {"humidity", "humidity", "%", 1000.0},
};

typedef struct {
  char prefix[16];
  char index[8];
  char suffix[32];
} sensor_file_t;

static const sensor_kind_t *kind_find(const char *prefix) {
  for (size_t i = 0; i < COUNT(KINDS); i++) {
    if (strcmp(KINDS[i].prefix, prefix) == 0) {
      return &KINDS[i];
    }
  }
  return NULL;
}

static int file_parse(const char *name, sensor_file_t *out) {
  return sscanf(name, "%15[a-z]%7[0-9]_%31s", out->prefix, out->index,
                out->suffix) == 3
             ? 0
             : -1;
}

static void file_path(char *buf, size_t size, const char *dir,
                      const sensor_file_t *file, const char *suffix) {
  snprintf(buf, size, "%s/%s%s_%s", dir, file->prefix, file->index, suffix);
}

static int file_holds_value(const char *dir, const sensor_file_t *file) {
  if (strcmp(file->suffix, "input") == 0) {
    return 1;
  }
  if (strcmp(file->suffix, "average") == 0) {
    char path[PATH_SIZE];
    file_path(path, sizeof(path), dir, file, "input");
    return access(path, F_OK) != 0;
  }
  return 0;
}

static void add_limit(cJSON *sensor, const char *dir, const sensor_file_t *file,
                      const char *limit, double divisor) {
  char path[PATH_SIZE];
  long long raw;

  file_path(path, sizeof(path), dir, file, limit);
  if (read_long_long(path, &raw) == 0) {
    cJSON_AddNumberToObject(sensor, limit, (double)raw / divisor);
  }
}

#define FALLBACK_SIZE 32

static void add_label(cJSON *sensor, const char *dir,
                      const sensor_file_t *file) {
  char path[PATH_SIZE], fallback[FALLBACK_SIZE];

  file_path(path, sizeof(path), dir, file, "label");
  char *label = read_text(path);

  snprintf(fallback, sizeof(fallback), "%s%s", file->prefix, file->index);
  cJSON_AddStringToObject(sensor, "label",
                          (label && *label) ? label : fallback);
  free(label);
}

static cJSON *sensor_build(const char *dir, const sensor_file_t *file,
                           const sensor_kind_t *kind) {
  char path[PATH_SIZE];
  long long raw;

  file_path(path, sizeof(path), dir, file, file->suffix);
  if (read_long_long(path, &raw) != 0) {
    return NULL;
  }

  cJSON *sensor = cJSON_CreateObject();
  if (!sensor) {
    return NULL;
  }

  cJSON_AddStringToObject(sensor, "type", kind->type);
  add_label(sensor, dir, file);
  cJSON_AddNumberToObject(sensor, "value", (double)raw / kind->divisor);
  cJSON_AddStringToObject(sensor, "unit", kind->unit);
  add_limit(sensor, dir, file, "max", kind->divisor);
  add_limit(sensor, dir, file, "crit", kind->divisor);
  return sensor;
}

cJSON *sensor_collect(const char *hwmon_dir) {
  cJSON *sensors = cJSON_CreateArray();
  struct dirent **entries = NULL;

  int count = scandir(hwmon_dir, &entries, NULL, versionsort);
  for (int i = 0; i < count; i++) {
    sensor_file_t file;
    if (file_parse(entries[i]->d_name, &file) == 0) {
      const sensor_kind_t *kind = kind_find(file.prefix);
      if (kind && file_holds_value(hwmon_dir, &file)) {
        cJSON *sensor = sensor_build(hwmon_dir, &file, kind);
        if (sensor) {
          cJSON_AddItemToArray(sensors, sensor);
        }
      }
    }
    free(entries[i]);
  }
  free(entries);
  return sensors;
}

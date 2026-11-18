#include "info/cpu_freq.h"
#include "core/file_utils.h"
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CPU_ROOT "/sys/devices/system/cpu"

typedef struct {
  double sum;
  int count;
} freq_stats_t;

static void stats_add(freq_stats_t *stats, double mhz) {
  stats->sum += mhz;
  stats->count++;
}

static int is_cpu_dir(const char *name) {
  return strncmp(name, "cpu", 3) == 0 && isdigit((unsigned char)name[3]);
}

static void collect_from_sysfs(freq_stats_t *stats) {
  DIR *dir = opendir(CPU_ROOT);
  if (!dir) {
    return;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (!is_cpu_dir(entry->d_name)) {
      continue;
    }

    char path[320];
    long long khz;
    snprintf(path, sizeof(path), CPU_ROOT "/%s/cpufreq/scaling_cur_freq",
             entry->d_name);
    if (read_long_long(path, &khz) == 0) {
      stats_add(stats, (double)khz / 1000.0);
    }
  }
  closedir(dir);
}

static void collect_from_cpuinfo(freq_stats_t *stats) {
  char *text = read_text("/proc/cpuinfo");
  if (!text) {
    return;
  }

  const char *line = text;
  while ((line = strstr(line, "cpu MHz")) != NULL) {
    double mhz;
    const char *colon = strchr(line, ':');
    if (colon && sscanf(colon + 1, "%lf", &mhz) == 1) {
      stats_add(stats, mhz);
    }
    line += strlen("cpu MHz");
  }
  free(text);
}

int cpu_freq_read(double *avg_mhz) {
  freq_stats_t stats = {0, 0};

  collect_from_sysfs(&stats);
  if (stats.count == 0) {
    collect_from_cpuinfo(&stats);
  }
  if (stats.count == 0) {
    return -1;
  }

  *avg_mhz = stats.sum / stats.count;
  return 0;
}

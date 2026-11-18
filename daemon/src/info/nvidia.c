#include "info/nvidia.h"
#include "core/file_utils.h"
#include "core/proc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int pci_id_is_safe(const char *pci_id) {
  return *pci_id &&
         strspn(pci_id, "0123456789abcdefABCDEF:.") == strlen(pci_id);
}

static int gpu_is_active(const char *pci_id) {
  char path[128];
  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/power/runtime_status",
           pci_id);

  char *status = read_text(path);
  int active = !status || strcmp(status, "active") == 0;
  free(status);
  return active;
}

static double field_parse(const char *token) {
  char *end = NULL;
  double value = strtod(token, &end);
  return (end == token) ? -1 : value;
}

static int line_parse(char *line, nvidia_info_t *out) {
  double *fields[] = {&out->temp, &out->power, &out->vram_used,
                      &out->vram_total, &out->freq};
  char *save = NULL;
  char *token = strtok_r(line, ",", &save);

  for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); i++) {
    if (!token) {
      return -1;
    }
    *fields[i] = field_parse(token);
    token = strtok_r(NULL, ",", &save);
  }
  return 0;
}

void nvidia_read(const char *pci_id, nvidia_info_t *out) {
  out->active = 1;
  out->temp = out->power = out->vram_used = out->vram_total = out->freq = -1;

  if (!pci_id_is_safe(pci_id)) {
    return;
  }
  if (!gpu_is_active(pci_id)) {
    out->active = 0;
    return;
  }

  char cmd[256];
  snprintf(cmd, sizeof(cmd),
           "nvidia-smi -i %s --query-gpu=temperature.gpu,power.draw,"
           "memory.used,memory.total,clocks.gr "
           "--format=csv,noheader,nounits 2>/dev/null",
           pci_id);

  char *line = proc_read_line(cmd);
  if (line && line_parse(line, out) != 0) {
    out->temp = out->power = out->vram_used = out->vram_total = out->freq = -1;
  }
  free(line);
}

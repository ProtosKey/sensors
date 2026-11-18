#include "stats/gpu_stats.h"
#include "core/file_utils.h"
#include "info/gpu_info.h"
#include "info/nvidia.h"
#include "stats/pick.h"
#include <stdio.h>
#include <unistd.h>

#define MIB 1048576.0

static int read_memory_mib(const char *pci_id, const char *kind,
                           const char *field, double *out) {
  char path[256];
  long long bytes;

  snprintf(path, sizeof(path), "/sys/bus/pci/devices/%s/mem_info_%s_%s", pci_id,
           kind, field);
  if (read_long_long(path, &bytes) != 0) {
    return -1;
  }
  *out = (double)bytes / MIB;
  return 0;
}

static void add_memory(cJSON *gpu, const cJSON *device, const char *kind) {
  const cJSON *pci = cJSON_GetObjectItem(device, "device");
  double used, total;

  if (!cJSON_IsString(pci) ||
      read_memory_mib(pci->valuestring, kind, "used", &used) != 0 ||
      read_memory_mib(pci->valuestring, kind, "total", &total) != 0) {
    return;
  }

  cJSON *memory = cJSON_AddObjectToObject(gpu, kind);
  cJSON_AddNumberToObject(memory, "used", used);
  cJSON_AddNumberToObject(memory, "total", total);
}

static void add_name(cJSON *gpu, const cJSON *device) {
  const cJSON *pci = cJSON_GetObjectItem(device, "device");
  const cJSON *driver = cJSON_GetObjectItem(device, "driver");

  const char *name =
      cJSON_IsString(pci) ? gpu_info_name(pci->valuestring) : NULL;
  if (!name) {
    name = cJSON_IsString(driver) ? driver->valuestring : "GPU";
  }
  cJSON_AddStringToObject(gpu, "name", name);
}

static cJSON *gpu_build(const cJSON *device) {
  cJSON *gpu = cJSON_CreateObject();
  const cJSON *driver = cJSON_GetObjectItem(device, "driver");

  add_name(gpu, device);
  cJSON_AddStringToObject(gpu, "driver",
                          cJSON_IsString(driver) ? driver->valuestring : "");
  pick_add_value(gpu, "temp", pick_sensor(device, "temp"));
  pick_add_value(gpu, "freq", pick_sensor(device, "freq"));
  if (!pick_is_integrated_gpu(device)) {
    pick_add_value(gpu, "power", pick_sensor(device, "power"));
    add_memory(gpu, device, "vram");
  }
  add_memory(gpu, device, "gtt");
  return gpu;
}

static void add_known(cJSON *object, const char *key, double value) {
  if (value >= 0) {
    cJSON_AddNumberToObject(object, key, value);
  }
}

static cJSON *nvidia_build(const char *pci_id) {
  nvidia_info_t info;
  nvidia_read(pci_id, &info);

  cJSON *gpu = cJSON_CreateObject();
  const char *name = gpu_info_name(pci_id);
  cJSON_AddStringToObject(gpu, "name", name ? name : "NVIDIA GPU");
  cJSON_AddStringToObject(gpu, "driver", "nvidia");
  cJSON_AddBoolToObject(gpu, "active", info.active);
  add_known(gpu, "temp", info.temp);
  add_known(gpu, "power", info.power);
  add_known(gpu, "freq", info.freq);

  if (info.vram_used >= 0 && info.vram_total >= 0) {
    cJSON *vram = cJSON_AddObjectToObject(gpu, "vram");
    cJSON_AddNumberToObject(vram, "used", info.vram_used);
    cJSON_AddNumberToObject(vram, "total", info.vram_total);
  }
  return gpu;
}

cJSON *gpu_stats(const cJSON *devices) {
  cJSON *gpus = cJSON_CreateArray();

  char pci_ids[4][NVIDIA_PCI_SIZE];
  int nvidia_count = nvidia_list(pci_ids, 4);
  for (int i = 0; i < nvidia_count; i++) {
    cJSON_AddItemToArray(gpus, nvidia_build(pci_ids[i]));
  }

  const cJSON *device;
  cJSON_ArrayForEach(device, devices) {
    if (pick_has_category(device, "gpu")) {
      cJSON_AddItemToArray(gpus, gpu_build(device));
    }
  }
  return gpus;
}

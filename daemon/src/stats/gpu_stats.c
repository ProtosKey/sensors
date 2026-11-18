#include "stats/gpu_stats.h"
#include "core/file_utils.h"
#include "info/drm.h"
#include "info/gpu_info.h"
#include "info/nvidia.h"
#include "stats/pick.h"
#include <stdio.h>
#include <string.h>
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

static void add_known(cJSON *object, const char *key, double value) {
  if (value >= 0) {
    cJSON_AddNumberToObject(object, key, value);
  }
}

/* A GPU without a hwmon device (e.g. Intel integrated graphics): the DRM
 * subsystem still knows it, so it is shown with its name and, where the
 * driver exposes it, the current clock. */
static cJSON *bare_build(const drm_gpu_t *card) {
  cJSON *gpu = cJSON_CreateObject();
  const char *name = gpu_info_name(card->pci);

  cJSON_AddStringToObject(gpu, "name", name ? name : card->driver);
  cJSON_AddStringToObject(gpu, "driver", card->driver);
  add_known(gpu, "freq", drm_freq_mhz(card));
  return gpu;
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

/* hwmon device of the GPU at this PCI address, or NULL. */
static const cJSON *find_hwmon_gpu(const cJSON *devices, const char *pci) {
  const cJSON *device;
  cJSON_ArrayForEach(device, devices) {
    const cJSON *bus_id = cJSON_GetObjectItem(device, "device");
    if (pick_has_category(device, "gpu") && cJSON_IsString(bus_id) &&
        strcmp(bus_id->valuestring, pci) == 0) {
      return device;
    }
  }
  return NULL;
}

#define MAX_GPUS 8

/* Discrete NVIDIA GPUs first (their data comes from nvidia-smi), then all
 * other GPUs in card order. */
cJSON *gpu_stats(const cJSON *devices) {
  cJSON *gpus = cJSON_CreateArray();

  drm_gpu_t cards[MAX_GPUS];
  int count = drm_list(cards, MAX_GPUS);

  for (int i = 0; i < count; i++) {
    if (cards[i].vendor == DRM_VENDOR_NVIDIA) {
      cJSON_AddItemToArray(gpus, nvidia_build(cards[i].pci));
    }
  }
  for (int i = 0; i < count; i++) {
    if (cards[i].vendor == DRM_VENDOR_NVIDIA) {
      continue;
    }
    const cJSON *device = find_hwmon_gpu(devices, cards[i].pci);
    cJSON_AddItemToArray(gpus,
                         device ? gpu_build(device) : bare_build(&cards[i]));
  }
  return gpus;
}

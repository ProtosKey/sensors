#include "info/drm.h"
#include "core/file_utils.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DRM_ROOT "/sys/class/drm"

/* "card0" (not "card0-HDMI-A-1", "renderD128" and friends). */
static int is_card_name(const char *name) {
  return strncmp(name, "card", 4) == 0 && strchr(name, '-') == NULL;
}

static long long read_vendor(const char *card) {
  char path[320];
  snprintf(path, sizeof(path), DRM_ROOT "/%s/device/vendor", card);

  char *text = read_text(path);
  if (!text) {
    return -1;
  }
  long long vendor = strtoll(text, NULL, 16);
  free(text);
  return vendor;
}

/* Last component of a symlink under the card's device directory
 * ("device" -> PCI address, "device/driver" -> driver name). */
static void read_link(const char *card, const char *name, char *out,
                      size_t size) {
  char path[320];
  snprintf(path, sizeof(path), DRM_ROOT "/%s/%s", card, name);

  char *target = link_basename(path);
  snprintf(out, size, "%s", target ? target : "");
  free(target);
}

static int gpu_fill(drm_gpu_t *gpu, const char *card) {
  gpu->vendor = read_vendor(card);
  snprintf(gpu->card, sizeof(gpu->card), "%s", card);
  read_link(card, "device", gpu->pci, sizeof(gpu->pci));
  read_link(card, "device/driver", gpu->driver, sizeof(gpu->driver));
  return gpu->pci[0] ? 0 : -1;
}

int drm_list(drm_gpu_t *gpus, int max) {
  struct dirent **entries = NULL;
  int total = scandir(DRM_ROOT, &entries, NULL, versionsort);

  int count = 0;
  for (int i = 0; i < total; i++) {
    if (count < max && is_card_name(entries[i]->d_name) &&
        gpu_fill(&gpus[count], entries[i]->d_name) == 0) {
      count++;
    }
    free(entries[i]);
  }
  free(entries);
  return count;
}

/* Files with the GPU clock, in order of preference: the actual clock first,
 * then the requested one; the "gt/gt0" names are used by newer kernels. */
static const char *const FREQ_FILES[] = {
    "gt_act_freq_mhz",
    "gt/gt0/rps_act_freq_mhz",
    "gt_cur_freq_mhz",
    "gt/gt0/rps_cur_freq_mhz",
};

static double freq_from_dir(const char *card_dir) {
  for (size_t i = 0; i < sizeof(FREQ_FILES) / sizeof(FREQ_FILES[0]); i++) {
    char path[400];
    long long mhz;

    snprintf(path, sizeof(path), "%s/%s", card_dir, FREQ_FILES[i]);
    if (read_long_long(path, &mhz) == 0) {
      return (double)mhz;
    }
  }
  return -1;
}

double drm_freq_mhz(const drm_gpu_t *gpu) {
  char card_dir[320];
  snprintf(card_dir, sizeof(card_dir), DRM_ROOT "/%s", gpu->card);
  return freq_from_dir(card_dir);
}

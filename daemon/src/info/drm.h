#ifndef INFO_DRM_H
#define INFO_DRM_H

#define DRM_ID_SIZE 32
#define DRM_VENDOR_NVIDIA 0x10de

typedef struct {
  char card[DRM_ID_SIZE];
  char pci[DRM_ID_SIZE];
  char driver[DRM_ID_SIZE];
  long long vendor;
} drm_gpu_t;

int drm_list(drm_gpu_t *gpus, int max);
double drm_freq_mhz(const drm_gpu_t *gpu);

#endif

#ifndef INFO_NVIDIA_H
#define INFO_NVIDIA_H

typedef struct {
  int active;
  double temp;
  double power;
  double vram_used;
  double vram_total;
  double freq;
} nvidia_info_t;

void nvidia_read(const char *pci_id, nvidia_info_t *out);

#endif

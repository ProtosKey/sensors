#ifndef INFO_NVIDIA_H
#define INFO_NVIDIA_H

#define NVIDIA_PCI_SIZE 32

typedef struct {
  int active;
  double temp;
  double power;
  double vram_used;
  double vram_total;
  double freq;
} nvidia_info_t;

int nvidia_list(char pci_ids[][NVIDIA_PCI_SIZE], int max);
void nvidia_read(const char *pci_id, nvidia_info_t *out);

#endif

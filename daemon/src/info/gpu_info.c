#include "info/gpu_info.h"
#include "info/cpu_info.h"
#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CACHE_SIZE 8
#define PCI_SIZE 32
#define NAME_SIZE 128

typedef struct {
  char pci[PCI_SIZE];
  char name[NAME_SIZE];
} name_entry;

static struct udev *udev_context;
static name_entry cache[CACHE_SIZE];
static int cache_len;

static void model_clean(const char *model, char *out, size_t size) {
  const char *open = strchr(model, '[');
  const char *close = open ? strrchr(model, ']') : NULL;

  if (open && close && close > open + 1) {
    snprintf(out, size, "%.*s", (int)(close - open - 1), open + 1);
  } else {
    snprintf(out, size, "%s", model);
  }
}

static void name_refine(char *name, size_t size) {
  if (!strstr(name, " / ")) {
    return;
  }

  char *cpu = cpu_info_model();
  if (!cpu) {
    return;
  }

  char copy[NAME_SIZE], prefix[NAME_SIZE] = "", match[NAME_SIZE] = "";
  snprintf(copy, sizeof(copy), "%s", name);

  int matches = 0;
  int first = 1;
  char *piece = copy;
  while (piece) {
    char *next = strstr(piece, " / ");
    if (next) {
      *next = '\0';
      next += 3;
    }

    char *space = strrchr(piece, ' ');
    char *variant = space ? space + 1 : piece;
    if (first) {
      snprintf(prefix, sizeof(prefix), "%.*s", (int)(variant - piece), piece);
      first = 0;
    }
    if (*variant && strstr(cpu, variant)) {
      matches++;
      snprintf(match, sizeof(match), "%s", variant);
    }
    piece = next;
  }

  if (matches == 1) {
    snprintf(name, size, "%s%s", prefix, match);
  }
  free(cpu);
}

static int name_lookup(const char *pci_id, char *out, size_t size) {
  if (!udev_context) {
    udev_context = udev_new();
    if (!udev_context) {
      return -1;
    }
  }

  char syspath[96];
  snprintf(syspath, sizeof(syspath), "/sys/bus/pci/devices/%s", pci_id);

  struct udev_device *device =
      udev_device_new_from_syspath(udev_context, syspath);
  if (!device) {
    return -1;
  }

  const char *model =
      udev_device_get_property_value(device, "ID_MODEL_FROM_DATABASE");
  if (model) {
    model_clean(model, out, size);
    name_refine(out, size);
  }

  udev_device_unref(device);
  return model ? 0 : -1;
}

const char *gpu_info_name(const char *pci_id) {
  for (int i = 0; i < cache_len; i++) {
    if (strcmp(cache[i].pci, pci_id) == 0) {
      return cache[i].name[0] ? cache[i].name : NULL;
    }
  }
  if (cache_len == CACHE_SIZE) {
    return NULL;
  }

  name_entry *entry = &cache[cache_len++];
  snprintf(entry->pci, sizeof(entry->pci), "%s", pci_id);
  if (name_lookup(pci_id, entry->name, sizeof(entry->name)) != 0) {
    entry->name[0] = '\0';
  }
  return entry->name[0] ? entry->name : NULL;
}

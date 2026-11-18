#include "info/profile.h"
#include "core/proc.h"
#include <stdlib.h>
#include <string.h>

char *profile_active(void) {
  char *profile =
      proc_read_line("asusctl profile get | grep Active | awk '{print $3}'");
  if (profile && *profile == '\0') {
    free(profile);
    return NULL;
  }
  return profile;
}

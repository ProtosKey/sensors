#include "info/profile.h"
#include "core/file_utils.h"
#include <ctype.h>
#include <stdlib.h>

#define PLATFORM_PROFILE "/sys/firmware/acpi/platform_profile"

/* Active platform profile from ACPI ("quiet", "balanced", "performance", ...),
 * with the first letter capitalized ("Quiet"). NULL if unavailable. */
char *profile_active(void) {
  char *profile = read_text(PLATFORM_PROFILE);
  if (!profile) {
    return NULL;
  }
  if (*profile == '\0') {
    free(profile);
    return NULL;
  }

  profile[0] = (char)toupper((unsigned char)profile[0]);
  return profile;
}

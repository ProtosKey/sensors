#include "hwmon/category.h"
#include <string.h>

typedef struct {
  const char *driver;
  const char *category;
} driver_rule;

#define COUNT(table) (sizeof(table) / sizeof((table)[0]))

static const driver_rule EXACT_RULES[] = {
    {"k10temp", "cpu"},
    {"coretemp", "cpu"},
    {"zenpower", "cpu"},
    {"zenergy", "cpu"},
    {"cpu_thermal", "cpu"},
    {"soc_thermal", "cpu"},
    {"amdgpu", "gpu"},
    {"radeon", "gpu"},
    {"nouveau", "gpu"},
    {"i915", "gpu"},
    {"xe", "gpu"},
    {"nvme", "storage"},
    {"drivetemp", "storage"},
    {"jc42", "memory"},
    {"spd5118", "memory"},
    {"asus", "board"},
    {"asus_wmi_sensors", "board"},
    {"asus_ec_sensors", "board"},
    {"asus_custom_fan_curve", "board"},
    {"acpi_fan", "board"},
    {"thinkpad", "board"},
    {"dell_smm", "board"},
    {"hp_wmi_sensors", "board"},
    {"applesmc", "board"},
    {"intel_pch_thermal", "board"},
    {"acpitz", "virtual"},
};

static const driver_rule PREFIX_RULES[] = {
    {"nct", "board"},   {"it8", "board"},       {"f718", "board"},
    {"w836", "board"},  {"BAT", "power"},       {"AC", "power"},
    {"ucsi_", "power"}, {"iwlwifi", "network"}, {"ath", "network"},
    {"rtw", "network"}, {"mt79", "network"},
};

const char *category_of_driver(const char *driver) {
  for (size_t i = 0; i < COUNT(EXACT_RULES); i++) {
    if (strcmp(driver, EXACT_RULES[i].driver) == 0) {
      return EXACT_RULES[i].category;
    }
  }

  for (size_t i = 0; i < COUNT(PREFIX_RULES); i++) {
    if (strncmp(driver, PREFIX_RULES[i].driver,
                strlen(PREFIX_RULES[i].driver)) == 0) {
      return PREFIX_RULES[i].category;
    }
  }
  return "other";
}

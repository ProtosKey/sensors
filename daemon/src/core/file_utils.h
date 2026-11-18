#ifndef CORE_FILE_UTILS_H
#define CORE_FILE_UTILS_H

char *read_text(const char *path);
int read_long_long(const char *path, long long *out);
char *link_basename(const char *path);

#endif

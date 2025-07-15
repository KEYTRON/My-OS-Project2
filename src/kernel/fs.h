#ifndef FS_H
#define FS_H

void fs_init();
int fs_create(const char *name, const char *data);
const char* fs_read(const char *name);
int fs_get(const char *name, const char **data, unsigned int *size);
void fs_list();

#endif

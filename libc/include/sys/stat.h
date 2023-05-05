#ifndef _SYS_STAT_H
#define _SYS_STAT_H

typedef unsigned int _dev_t;
typedef unsigned short _ino_t;
typedef unsigned short _mode_t;
typedef long _off_t;

#define dev_t _dev_t
#define ino_t _ino_t
#define mode_t _mode_t
#define off_t _off_t

int mkdir(const char *path, mode_t mode);
int remove(const char *pathname);
int rename(const char *oldpath, const char *newpath);

#endif

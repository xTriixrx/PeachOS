#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include "pparser.h"

typedef unsigned int FILE_MODE;
typedef unsigned int FILE_SEEK_MODE;
typedef unsigned int FILE_STAT_FLAGS;

struct disk;
struct file_stat;

typedef int (*FS_CLOSE_FUNCTION) (void* private);
typedef int (*FS_STAT_FUNCTION) (struct disk* disk, void* private, struct file_stat* stat);
typedef int (*FS_SEEK_FUNCTION) (void* private, uint32_t offset, FILE_SEEK_MODE seek_mode);
typedef void* (*FS_OPEN_FUNCTION) (struct disk* disk, struct path_part* path, FILE_MODE mode);
typedef int (*FS_READ_FUNCTION) (struct disk* disk, void* private, uint32_t size, uint32_t nmemb, char* out);
typedef int (*FS_RESOLVE_FUNCTION) (struct disk* disk);

enum
{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

enum
{
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
};

enum
{
    FILE_STAT_READ_ONLY = 0b00000001
};

struct filesystem
{
    // Filesystem should return zero from resolve if the provided disk is using its filesystem
    FS_OPEN_FUNCTION open;
    FS_RESOLVE_FUNCTION resolve;
    FS_READ_FUNCTION read;
    FS_SEEK_FUNCTION seek;
    FS_STAT_FUNCTION stat;
    FS_CLOSE_FUNCTION close;
    char name[20];
};

struct file_descriptor
{
    // The descriptor index
    int index;
    struct filesystem* filesystem;

    // Private data for internal file descriptor
    void* private;
    
    // The disk that the file descriptor should be used on
    struct disk* disk;
};

struct file_stat
{
    FILE_STAT_FLAGS flags;
    uint32_t filesize;
};

void fs_init();
int fclose(int);
int fstat(int, struct file_stat*);
int fseek(int, int, FILE_SEEK_MODE);
int fopen(const char*, const char*);
int fread(void*, uint32_t, uint32_t, int);
struct filesystem* fs_resolve(struct disk*);
void fs_insert_filesystem(struct filesystem*);

#endif
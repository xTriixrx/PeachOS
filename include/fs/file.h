#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include "pparser.h"

typedef unsigned int FILE_MODE;
typedef unsigned int FILE_SEEK_MODE;

struct disk;
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

struct filesystem
{
    // Filesystem should return zero from resolve if the provided disk is using its filesystem
    FS_OPEN_FUNCTION open;
    FS_RESOLVE_FUNCTION resolve;
    FS_READ_FUNCTION read;
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

void fs_init();
int fopen(const char*, const char*);
int fread(void*, uint32_t, uint32_t, int);
struct filesystem* fs_resolve(struct disk*);
void fs_insert_filesystem(struct filesystem*);

#endif
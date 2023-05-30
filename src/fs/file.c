#include "lib.h"
#include "disk.h"
#include "file.h"
#include "fat16.h"
#include "kheap.h"
#include "status.h"
#include "config.h"
#include "memory.h"
#include "string.h"
#include "pparser.h"

struct filesystem* filesystems[MAX_FILESYSTEMS];
struct file_descriptor* file_descriptors[MAX_FILE_DESCRIPTORS];

static struct filesystem** fs_get_free_filesystem()
{
    int i = 0;

    for (i = 0; i < MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] == 0)
        {
            return &filesystems[i];
        }
    }

    return 0;
}

static int file_new_descriptor(struct file_descriptor** desc_out)
{
    int res = -ENOMEM;

    for (int i = 0; i < MAX_FILE_DESCRIPTORS; i++)
    {
        if (file_descriptors[i] == 0)
        {
            struct file_descriptor* desc = kzalloc(sizeof(struct file_descriptor));

            // Descriptors start at 1
            desc->index = i + 1;
            file_descriptors[i] = desc;
            *desc_out = desc;
            res = 0;
            break;
        }
    }

    return res;
}

static struct file_descriptor* file_get_descriptor(int fd)
{
    if (fd <= 0 || fd >= MAX_FILE_DESCRIPTORS)
    {
        return 0;
    }

    // Descriptors start at 1
    return file_descriptors[fd - 1];
}

static void fs_static_load()
{
    fs_insert_filesystem(fat16_init());   
}

void fs_insert_filesystem(struct filesystem* filesystem)
{
    struct filesystem** fs;
    
    // Panic here
    if (filesystem == 0)
    {
        print("File system is empty"); while(1) {}   
    }

    fs = fs_get_free_filesystem();
    
    // Panic here
    if (!fs)
    {
        print("Problem with inserting filesystem"); while(1) {}
    }

    *fs = filesystem;
}

void fs_load()
{
    memset(filesystems, 0, sizeof(filesystems));
    fs_static_load();
}

void fs_init()
{
    memset(file_descriptors, 0, sizeof(file_descriptors));
    fs_load();
}

struct filesystem* fs_resolve(struct disk* disk)
{
    struct filesystem* fs = 0;

    for (int i = 0; i < MAX_FILESYSTEMS; i++)
    {
        if (filesystems[i] != 0 && filesystems[i]->resolve(disk) == 0)
        {
            fs = filesystems[i];
            break;
        }
    }

    return fs;
}

FILE_MODE file_get_mode_by_string(const char* str)
{
    FILE_MODE mode = FILE_MODE_INVALID;

    if (strncmp(str, "r", 1) == 0)
    {
        mode = FILE_MODE_READ;
    }
    else if (strncmp(str, "w", 1) == 0)
    {
        mode = FILE_MODE_WRITE;
    }
    else if (strncmp(str, "a", 1) == 0)
    {
        mode = FILE_MODE_APPEND;
    }

    return mode;
}

int fopen(const char* path, const char* mode_str)
{
    int res = 0;
    FILE_MODE mode = file_get_mode_by_string(mode_str);
    struct path_root* root_path = pathparser_parse(path, NULL);

    if (!root_path)
    {
        return 0;
    }

    // Just a root path such as 0:/
    if (!root_path->first)
    {
        return 0;
    }

    struct disk* disk = disk_get(root_path->drive_no);

    // Ensure the disk we are reading from exists
    if (!disk)
    {
        return 0;
    }

    // Ensure a file
    if (!disk->filesystem)
    {
        return 0;
    }

    if (mode == FILE_MODE_INVALID)
    {
        return 0;
    }

    void* descriptor_private_data = disk->filesystem->open(disk, root_path->first, mode);

    if (ISERR(descriptor_private_data))
    {
        return 0;
    }

    struct file_descriptor* desc = 0;
    res = file_new_descriptor(&desc);

    desc->filesystem = disk->filesystem;
    desc->private = descriptor_private_data;
    desc->disk = disk;
    res = desc->index;

    // fopen shouldn't return negative values 
    if (res < 0)
    {
        return 0;
    }

    return res;
}
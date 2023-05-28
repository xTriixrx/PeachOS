#ifndef DISKSTREAMER_H
#define DISKSTREAMER_H

#include "disk.h"

struct disk_stream
{
    int pos;
    struct disk* disk;  
};

struct disk_stream* diskstreamer_new(int);
void diskstreamer_close(struct disk_stream*);
int diskstreamer_seek(struct disk_stream*, int);
int diskstreamer_read(struct disk_stream*, void*, int);

#endif
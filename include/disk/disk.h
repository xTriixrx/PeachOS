#ifndef DISK_H
#define DISK_H

typedef unsigned int DISK_TYPE;

// Represents a real physical hard disk
#define DISK_TYPE_REAL 0

struct disk
{
    DISK_TYPE type;
    int sector_size;

};

struct disk* disk_get(int);
void disk_search_and_init();
int disk_read_block(struct disk*, unsigned int, int, void*);

#endif
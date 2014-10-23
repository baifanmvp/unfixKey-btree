#ifndef _VIEWSTREAM_H
#define _VIEWSTREAM_H
#define _GUN_SOURCE
#define __USE_FILE_OFFSET64
#define __USE_LARGEFILE64
#define _LARGEFILE64_SOURCE
#define _XOPEN_SOURCE 500


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "eggDef.h"
typedef struct viewStream
{
    fd_t fd;
    size64_t fileSz;
    
}viewStream_t;

viewStream_t* viewStream_new(char* path);

int viewStream_delete(viewStream_t* pViewStream);

offset64_t viewStream_write(viewStream_t* pViewStream, void* data, size32_t nSize);


int viewStream_read(viewStream_t* pViewStream, void* data, size32_t nSize, offset64_t nOffset);
int viewStream_update(viewStream_t* pViewStream, void* data, size32_t nSize, offset64_t nOffset);
#endif

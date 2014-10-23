#include "ViewStream.h"

viewStream_t* viewStream_new(char* path)
{
    
    viewStream_t* p_view = malloc(sizeof(viewStream_t));
    p_view->fileSz = 0;
    if( (p_view->fd = open(path, O_RDWR | O_LARGEFILE|O_CREAT, 0666)) == -1)
    {
        perror("viewStream_new");
        exit(-1);
    }
    struct stat st_stat_buff = {0};  
    if(stat(path, &st_stat_buff) < 0)
    {
        perror("viewStream_new");
        exit(-1);    
    }
    else
    {
        p_view->fileSz = st_stat_buff.st_size;
    }
    if(!p_view->fileSz)
    {
        char headinfo[32]={0};
        if(write(p_view->fd, headinfo, 32) == -1)
        {
            perror("viewStream_write");
            exit(-1);
        }

    }

    p_view->fileSz = 32;
   
    return p_view;
}

int viewStream_delete(viewStream_t* pViewStream)
{
    close(pViewStream->fd);
    free(pViewStream);
    return EGG_TRUE;
}


offset64_t viewStream_write(viewStream_t* pViewStream, void* data, size32_t nSize)
{
    if(!pViewStream || !data)
    {
        printf("viewStream_write : !pViewStream || !data \n");
        exit(-1);
    }
    
    offset64_t n_off_ret = pViewStream->fileSz;

    if(pwrite(pViewStream->fd, data, nSize, pViewStream->fileSz) == -1)
    {
        perror("viewStream_write");
        exit(-1);
    }
    pViewStream->fileSz += nSize;
    return n_off_ret;
}



int viewStream_read(viewStream_t* pViewStream, void* data, size32_t nSize, offset64_t nOffset)
{
    if(!pViewStream || !data )
    {
        printf("viewStream_read : !pViewStream || !data \n");
        exit(-1);
    }

    if(nOffset >= pViewStream->fileSz || nOffset+nSize > pViewStream->fileSz)
    {
        printf("viewStream_read : read data position  error \n");
        exit(-1);
        
    }
    if(pread(pViewStream->fd, data, nSize, nOffset) == -1)
    {
        perror("viewStream_read");
        exit(-1);
    }
    return EGG_TRUE;
}

int viewStream_update(viewStream_t* pViewStream, void* data, size32_t nSize, offset64_t nOffset)
{
    if(!pViewStream || !data )
    {
        printf("viewStream_update : !pViewStream || !data \n");
        exit(-1);
    }

    if(nOffset >= pViewStream->fileSz || nOffset + nSize > pViewStream->fileSz)
    {
        printf("viewStream_update : update data position  error \n");
        exit(-1);
        
    }
    if(pwrite(pViewStream->fd, data, nSize, nOffset) == -1)
    {
        perror("viewStream_update");
        exit(-1);
    }
    return EGG_TRUE;
}

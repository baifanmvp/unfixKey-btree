#ifndef _EGG_DEF_H
#define _EGG_DEF_H

typedef unsigned char size8_t;
typedef unsigned short size16_t;
typedef unsigned int  size32_t;
typedef unsigned long long  size64_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int  uint32_t;
typedef unsigned long long  uint64_t;

typedef unsigned char offset8_t;
typedef unsigned short offset16_t;
typedef unsigned int  offset32_t;
typedef unsigned long long  offset64_t;


typedef int fd_t;
typedef unsigned long long  pageno_t;
typedef void eggIndexRd_t;
typedef unsigned short  type_t;

#define EGG_TRUE  1
#define EGG_FALSE 0
#define EGG_NULL 0
#define _LARGEFILE64_SOURCE

#endif

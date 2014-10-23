#ifndef _EGG_INDEX_H
#define _EGG_INDEX_H
#include <stdio.h>
#include <stdlib.h>
#include "eggDef.h"
#include "ViewStream.h"

typedef struct eggIndexBoughRd
{
    pageno_t lchild;  //right child Nd
    pageno_t host;
    uint16_t kSz;
}eggIndexBoughRd_t;


typedef struct eggIndexLeafRd
{
    uint16_t kSz;
    uint32_t vSz;
}eggIndexLeafRd_t;


//common Nd struct
typedef struct eggIndexNd
{
    type_t ty;
    uint32_t useSz;    
}eggIndexNd_t;


typedef struct eggIndexBoughNd
{
    type_t ty;
    uint32_t useSz;
    pageno_t rchild; // the left child Nd of first record

}eggIndexBoughNd_t;


typedef struct eggIndexLeafNd
{
    type_t ty;
    uint32_t useSz;
    pageno_t next; // next leaf
    
}eggIndexLeafNd_t;

typedef struct eggIndexInf
{
    pageno_t root;
    pageno_t leaf;
    type_t rdtype;
    size32_t rdmaxSz;
}eggIndexInf_t;

typedef struct eggIndex
{
     viewStream_t* hViewStream;
    eggIndexInf_t info;
}eggIndex_t;


typedef struct eggIndexNdList
{
    pageno_t pageno;
    eggIndexNd_t* nd;
    struct eggIndexNdList* next;
}eggIndexNdList_t;
#define EGG_INVALID_PAGENO (-1)

#define EGG_IDXNDSZ (1000)
#define EGG_IDXRDMAXSZ (EGG_IDXNDSZ/5)
#define EGG_IDX_BOUGH_ND 1
#define EGG_IDX_LEAF_ND  2

#define EGG_IDXBOUGHRD_SZ(rd) (((eggIndexBoughRd_t*)(rd))->kSz + sizeof(eggIndexBoughRd_t) )
#define EGG_IDXLEAFRD_SZ(rd) (((eggIndexLeafRd_t*)(rd))->kSz + ((eggIndexLeafRd_t*)(rd))->vSz + sizeof(eggIndexLeafRd_t) )

#define EGG_IDXND_TAILPOS(nd) ((char*)(nd) + nd->useSz )


#define EGG_IDXBOUGHND_IS_FULL(nd, rd)                              \
    ((EGG_IDXNDSZ - (nd)->useSz) < EGG_IDXBOUGHRD_SZ(rd) ? 1 :0)

#define EGG_IDXLEAFND_IS_FULL(nd, rd)                           \
    ((EGG_IDXNDSZ - (nd)->useSz) < EGG_IDXLEAFRD_SZ(rd) ? 1 :0)

typedef int (*INDEXRDCMP)(void*, void*); //return
                                 // 0  : =
                                 // 1  : > 
                                 //-1  : < 


#define EGG_IDXBOUGHND_DATASZ (EGG_IDXNDSZ - sizeof(eggIndexboughNd_t) )
#define EGG_IDXLEAFND_DATASZ (EGG_IDXNDSZ - sizeof(eggIndexLeafNd_t) )

int rdcmp_bough_str(eggIndexBoughRd_t* pSrcRd, eggIndexBoughRd_t* pDestRd);

int rdcmp_leaf_str(eggIndexLeafRd_t* pSrcRd, eggIndexLeafRd_t* pDestRd);

eggIndexNdList_t* eggIndexNdList_push(eggIndexNdList_t* pNdList,pageno_t pageno, void* nd, eggIndexBoughRd_t* fatherRd);


int eggIndexNdList_free(eggIndexNdList_t* pNdList);

eggIndexBoughRd_t* eggIndexBoughRd_new(char* key, uint16_t kSz);

eggIndexBoughRd_t* eggIndexRd_leaf_to_bough(eggIndexLeafRd_t* hLeafRd);


int eggIndexBoughRd_delete(eggIndexBoughRd_t* pRd);

eggIndexLeafRd_t* eggIndexLeafRd_new(char* key, uint16_t kSz, char* val, uint32_t vSz);

int eggIndexLeafRd_delete(eggIndexLeafRd_t* pRd);

eggIndexBoughNd_t* eggIndexBoughNd_init(void* addr);

int eggIndexBoughNd_destroy(eggIndexBoughNd_t* pBoughNd);

int eggIndexBoughNd_find(eggIndexBoughNd_t* pNd, eggIndexBoughRd_t* pRd, char** pos, INDEXRDCMP fn);

int eggIndexBoughNd_insert(eggIndexBoughNd_t* pNd, eggIndexBoughRd_t* pRd, INDEXRDCMP fn);

int eggIndexBoughNd_insert_withPos(eggIndexBoughNd_t* pNd, eggIndexBoughRd_t* pRd, void* pos);

eggIndexBoughRd_t* eggIndexBoughNd_split(eggIndexBoughNd_t* pOrgNd, eggIndexBoughNd_t* pNewNd, eggIndexBoughRd_t* pRd, INDEXRDCMP fn);

eggIndexBoughRd_t* eggIndexBoughNd_split_withPos(eggIndexBoughNd_t* pOrgNd, eggIndexBoughNd_t* pNewNd, eggIndexBoughRd_t* pRd, void* pInsertPos);

eggIndexLeafNd_t* eggIndexLeafNd_init(void* addr);

int eggIndexLeafNd_destroy(eggIndexLeafNd_t* pLeafNd);


int eggIndexLeafNd_find(eggIndexLeafNd_t* pNd, eggIndexLeafRd_t* pRd, char** pos, INDEXRDCMP fn);

int eggIndexLeafNd_insert(eggIndexLeafNd_t* pNd, eggIndexLeafRd_t* pRd, INDEXRDCMP fn);

int eggIndexLeafNd_insert_withPos(eggIndexLeafNd_t* pNd, eggIndexLeafRd_t* pRd, void* pos);

eggIndexLeafRd_t* eggIndexLeafNd_split_withPos(eggIndexLeafNd_t* pOrgNd, eggIndexLeafNd_t* pNewNd, eggIndexLeafRd_t* pRd, void* pInsertPos);

eggIndexLeafRd_t* eggIndexLeafNd_split(eggIndexLeafNd_t* pOrgNd, eggIndexLeafNd_t* pNewNd, eggIndexLeafRd_t* pRd, INDEXRDCMP fn);

int eggIndexNd_destroy(eggIndexNd_t* pNd);

eggIndex_t* eggIndex_new(viewStream_t* hViewStream, eggIndexInf_t* pInfo);

int eggIndex_delete(eggIndex_t* lp_index);

eggIndexLeafRd_t* eggIndex_find(eggIndex_t* hIndex, char* key, uint16_t kSz);

int eggIndex_add(eggIndex_t* hIndexView, char* key, uint16_t kSz, char* val, uint32_t vSz);

int eggIndex_leafNd_result(eggIndex_t* hIndex);

eggIndexRd_t* eggIndex_split(eggIndex_t* hIndex, eggIndexNdList_t* pNdList,   eggIndexRd_t* pInsertRd, pageno_t* p_new_pageno);
#endif

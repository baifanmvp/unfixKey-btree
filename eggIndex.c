#include "eggIndex.h"

int rdcmp_bough_str(eggIndexBoughRd_t* pSrcRd, eggIndexBoughRd_t* pDestRd)
{
    void* p_src_key = pSrcRd + 1;
    void* p_dest_key = pDestRd + 1;
    size16_t n_src_sz = pSrcRd->kSz;
    size16_t n_dest_sz = pDestRd->kSz;
    size16_t n_cmp_sz = n_src_sz > n_dest_sz ? n_dest_sz : n_src_sz;
    int n_cmp_ret = 0;
    if ((n_cmp_ret = memcmp(p_src_key, p_dest_key, (size_t)n_cmp_sz)) == 0)
    {
        n_cmp_ret = n_src_sz - n_dest_sz;
    }
    return n_cmp_ret;
}

int rdcmp_leaf_str(eggIndexLeafRd_t* pSrcRd, eggIndexLeafRd_t* pDestRd)
{
    void* p_src_key = pSrcRd + 1;
    void* p_dest_key = pDestRd + 1;
    size16_t n_src_sz = pSrcRd->kSz;
    size16_t n_dest_sz = pDestRd->kSz;
    size16_t n_cmp_sz = n_src_sz > n_dest_sz ? n_dest_sz : n_src_sz;
    int n_cmp_ret = 0;
    if ((n_cmp_ret = memcmp(p_src_key, p_dest_key, (size_t)n_cmp_sz)) == 0)
    {
        n_cmp_ret = n_src_sz - n_dest_sz;
    }
    return n_cmp_ret;
}


eggIndexNdList_t* eggIndexNdList_push(eggIndexNdList_t* pNdList,pageno_t pageno, void* nd, eggIndexBoughRd_t* fatherRd)
{
    if(pNdList)
    {
        eggIndexNdList_t*  pNewNdList = (eggIndexNdList_t*)malloc(sizeof(eggIndexNdList_t));
        pNewNdList->pageno = pageno;
        pNewNdList->nd = nd;
        pNewNdList->next = pNdList;
        return pNewNdList;
    }
    else
    {
        pNdList = (eggIndexNdList_t*)malloc(sizeof(eggIndexNdList_t));
        pNdList->pageno = pageno;
        pNdList->nd = nd;
        pNdList->next = EGG_NULL;
        return pNdList;
    }
}


int eggIndexNdList_free(eggIndexNdList_t* pNdList)
{
    while(pNdList)
    {
        free(pNdList->nd);
        eggIndexNdList_t* pTmpNdList =  pNdList;
        pNdList = pNdList->next;
        free(pTmpNdList);
    }
    return EGG_TRUE;
}

eggIndexBoughRd_t* eggIndexBoughRd_new(char* key, uint16_t kSz)
{
    eggIndexBoughRd_t* p_rd = (eggIndexBoughRd_t*) calloc(1, sizeof(eggIndexBoughRd_t) + kSz);
    p_rd->kSz = kSz;
    memcpy(p_rd + 1, key, kSz);
    return p_rd;
}

eggIndexBoughRd_t* eggIndexRd_leaf_to_bough(eggIndexLeafRd_t* hLeafRd)
{
    eggIndexBoughRd_t* p_bough_rd = eggIndexBoughRd_new((char*)(hLeafRd + 1), hLeafRd->kSz);
    eggIndexLeafRd_delete(hLeafRd);
    return p_bough_rd;
}


int eggIndexBoughRd_delete(eggIndexBoughRd_t* pRd)
{
    if(pRd)
    {
        free(pRd);
    }
    return EGG_TRUE;
}

eggIndexLeafRd_t* eggIndexLeafRd_new(char* key, uint16_t kSz, char* val, uint32_t vSz)
{
    eggIndexLeafRd_t* p_rd = (eggIndexLeafRd_t*) calloc(1, sizeof(eggIndexLeafRd_t) + kSz + vSz);
    p_rd->kSz = kSz;
    p_rd->vSz = vSz;
    memcpy(p_rd + 1, key, kSz);
    memcpy((char*)(p_rd + 1) + kSz, val, vSz);
    return p_rd;
}

int eggIndexLeafRd_delete(eggIndexLeafRd_t* pRd)
{
    if(pRd)
    {
        free(pRd);
    }
    return EGG_TRUE;
}



eggIndexBoughNd_t* eggIndexBoughNd_init(void* addr)
{
    eggIndexBoughNd_t* p_nd = (eggIndexBoughNd_t*)addr;
    p_nd->ty = EGG_IDX_BOUGH_ND;
    p_nd->useSz = sizeof(eggIndexBoughNd_t);
    return p_nd;
}

int eggIndexBoughNd_find(eggIndexBoughNd_t* pNd, eggIndexBoughRd_t* pRd, char** pos, INDEXRDCMP fn)
{
    if(!pNd || !pRd)
    {
        return 0;
    }
    eggIndexBoughRd_t* p_iter = (eggIndexBoughRd_t*)(pNd + 1);
    int n_find_ret = 0;
    void* p_rd_tail = (char*)pNd + pNd->useSz;
    while(p_iter != p_rd_tail)
    {
        int n_cmp_ret = fn(p_iter, pRd);
        if (n_cmp_ret == 1)
        {
            p_iter = (eggIndexBoughRd_t*)((char*)p_iter + EGG_IDXBOUGHRD_SZ(p_iter));
        }
        else if (n_cmp_ret == -1)
        {
            break;
        }
        else
        {
            n_find_ret = 1;
            break;
        }
    }
    
    *pos = (char*)p_iter;
    return n_find_ret;
}


int eggIndexBoughNd_insert(eggIndexBoughNd_t* pNd, eggIndexBoughRd_t* pRd, INDEXRDCMP fn)
{
    if(!pNd || !pRd || EGG_IDXBOUGHND_IS_FULL(pNd, pRd))
    {
        return EGG_FALSE;
    }

    eggIndexBoughRd_t* p_insert_pos = EGG_NULL;
    int n_find_ret = eggIndexBoughNd_find(pNd, pRd, (char**)&p_insert_pos, fn);
    if(!n_find_ret)
    {
        void* p_rd_tail = (char*)pNd + pNd->useSz;
        if(p_insert_pos != p_rd_tail)
        {
            size_t n_move_size = p_rd_tail - (void*)p_insert_pos;
            memmove((char*)p_insert_pos + EGG_IDXBOUGHRD_SZ(pRd), p_insert_pos, n_move_size);
        }
        memcpy(p_insert_pos, pRd, EGG_IDXBOUGHRD_SZ(pRd));

        pNd->useSz += EGG_IDXBOUGHRD_SZ(pRd);
        return EGG_TRUE;
        
    }
    else
    {
        return EGG_FALSE;
    }
}


int eggIndexBoughNd_insert_withPos(eggIndexBoughNd_t* pNd, eggIndexBoughRd_t* pRd, void* pos)
{
    if(!pNd || !pRd || EGG_IDXBOUGHND_IS_FULL(pNd, pRd) || !pos)
    {
        return EGG_FALSE;
    }

    eggIndexBoughRd_t* p_insert_pos = pos;
    void* p_rd_tail = (char*)pNd + pNd->useSz;
    if(p_insert_pos != p_rd_tail)
    {
        size_t n_move_size = p_rd_tail - (void*)p_insert_pos;
        memmove((char*)p_insert_pos + EGG_IDXBOUGHRD_SZ(pRd), p_insert_pos, n_move_size);
    }
    memcpy(p_insert_pos, pRd, EGG_IDXBOUGHRD_SZ(pRd));

    pNd->useSz += EGG_IDXBOUGHRD_SZ(pRd);
    return EGG_TRUE;
    
}


eggIndexBoughRd_t* eggIndexBoughNd_split(eggIndexBoughNd_t* pOrgNd, eggIndexBoughNd_t* pNewNd, eggIndexBoughRd_t* pRd, INDEXRDCMP fn)
{
     uint32_t n_old_sz = pOrgNd->useSz - sizeof(eggIndexBoughNd_t) ;
    uint32_t n_tmp_sz = n_old_sz + EGG_IDXBOUGHRD_SZ(pRd);
    char* lp_tmp_buf = (char*)malloc(n_tmp_sz);
    memcpy(lp_tmp_buf, pOrgNd + 1, n_old_sz);  
    eggIndexBoughRd_t* p_iter = (eggIndexBoughRd_t*)lp_tmp_buf;
    
    while(1)
    {
        
        if((unsigned  long)p_iter == (size_t)(lp_tmp_buf + n_old_sz ) )
        {
            memcpy(p_iter, pRd, EGG_IDXBOUGHRD_SZ(pRd));
            break;
        }
        int n_cmp_ret = fn(p_iter, pRd);
        if (n_cmp_ret == 1)
        {
           
            p_iter = (eggIndexBoughRd_t*)((char*)p_iter + EGG_IDXBOUGHRD_SZ(p_iter));
        }
        else if (n_cmp_ret == -1)
        {
            size_t n_move_size = (size_t)(lp_tmp_buf + n_old_sz) - (size_t)p_iter;
            memmove((char*)p_iter + EGG_IDXBOUGHRD_SZ(pRd), p_iter, n_move_size);
            memcpy(p_iter, pRd, EGG_IDXBOUGHRD_SZ(pRd));
            break;
        }
        else
        {
            //find same key !!
            free(lp_tmp_buf);
            return EGG_NULL;
        }

    }

    char* lp_mid_pos = lp_tmp_buf + n_tmp_sz/2;
    eggIndexBoughRd_t* lp_mid_rd = EGG_NULL;
    p_iter = (eggIndexBoughRd_t*)lp_tmp_buf;

    while((size_t)p_iter != (size_t)(lp_tmp_buf + n_tmp_sz))
    {
        if((size_t)p_iter <= (size_t)lp_mid_pos && (size_t)lp_mid_pos <= (size_t)((char*)p_iter + EGG_IDXBOUGHRD_SZ(p_iter)))
        {
            lp_mid_rd = p_iter;
            break;
        }
        p_iter = (eggIndexBoughRd_t*)((char*)p_iter + EGG_IDXBOUGHRD_SZ(p_iter));
    }

    pOrgNd->useSz = (size_t)lp_mid_rd - (size_t)lp_tmp_buf + sizeof(eggIndexBoughNd_t);
    memcpy(pOrgNd + 1, lp_tmp_buf, pOrgNd->useSz - sizeof(eggIndexBoughNd_t));
    pNewNd->useSz =  (size_t)lp_tmp_buf + n_tmp_sz -  (size_t)((char*)lp_mid_rd + EGG_IDXBOUGHRD_SZ(lp_mid_rd)) +sizeof(eggIndexBoughNd_t);
    memcpy(pNewNd + 1, ((char*)lp_mid_rd + EGG_IDXBOUGHRD_SZ(lp_mid_rd)), pNewNd->useSz - sizeof(eggIndexBoughNd_t));
    
    pNewNd->rchild = pOrgNd->rchild;
    pOrgNd->rchild = lp_mid_rd->lchild;

    eggIndexBoughRd_t* lp_ret_rd = (eggIndexBoughRd_t*)malloc(EGG_IDXBOUGHRD_SZ(lp_mid_rd));
    memcpy(lp_ret_rd, lp_mid_rd, EGG_IDXBOUGHRD_SZ(lp_mid_rd));
    free(lp_tmp_buf);
    return lp_ret_rd;

}





eggIndexBoughRd_t* eggIndexBoughNd_split_withPos(eggIndexBoughNd_t* pOrgNd, eggIndexBoughNd_t* pNewNd, eggIndexBoughRd_t* pRd, void* pInsertPos)
{
     uint32_t n_old_sz = pOrgNd->useSz - sizeof(eggIndexBoughNd_t) ;
     uint32_t n_tmp_sz = n_old_sz + EGG_IDXBOUGHRD_SZ(pRd);
     char* lp_tmp_buf = (char*)malloc(n_tmp_sz);

     uint32_t n_front_half_sz = (uint32_t)((size_t)pInsertPos -  (size_t)pOrgNd) - sizeof(eggIndexBoughNd_t);
    
    memcpy(lp_tmp_buf, pOrgNd + 1, n_front_half_sz);
    memcpy(lp_tmp_buf + n_front_half_sz, pRd, EGG_IDXBOUGHRD_SZ(pRd));
    memcpy(lp_tmp_buf + n_front_half_sz + EGG_IDXBOUGHRD_SZ(pRd), pInsertPos, pOrgNd->useSz - n_front_half_sz - sizeof(eggIndexBoughNd_t));
    
    char* lp_mid_pos = lp_tmp_buf + n_tmp_sz/2;
    eggIndexBoughRd_t* lp_mid_rd = EGG_NULL;
    eggIndexBoughRd_t* p_iter = (eggIndexBoughRd_t*)lp_tmp_buf;

    while((size_t)p_iter != ((size_t)lp_tmp_buf + n_tmp_sz))
    {
        if((size_t)p_iter <= (size_t)lp_mid_pos && (size_t)lp_mid_pos <= (size_t)((char*)p_iter + EGG_IDXBOUGHRD_SZ(p_iter)))
        {
            lp_mid_rd = p_iter;
            break;
        }
        p_iter = (eggIndexBoughRd_t*)((char*)p_iter + EGG_IDXBOUGHRD_SZ(p_iter));
    }
    
    pOrgNd->useSz = (size_t)lp_mid_rd - (size_t)lp_tmp_buf + sizeof(eggIndexBoughNd_t);
    memcpy(pOrgNd + 1, lp_tmp_buf, pOrgNd->useSz - sizeof(eggIndexBoughNd_t));
    pNewNd->useSz = (size_t)lp_tmp_buf + n_tmp_sz - (size_t)((char*)lp_mid_rd + EGG_IDXBOUGHRD_SZ(lp_mid_rd))  + sizeof(eggIndexBoughNd_t);
    memcpy(pNewNd + 1, ((char*)lp_mid_rd + EGG_IDXBOUGHRD_SZ(lp_mid_rd)), pNewNd->useSz - sizeof(eggIndexBoughNd_t));

    
    pNewNd->rchild = pOrgNd->rchild;
    pOrgNd->rchild = lp_mid_rd->lchild;

    
    eggIndexBoughRd_t* lp_ret_rd = (eggIndexBoughRd_t*)malloc(EGG_IDXBOUGHRD_SZ(lp_mid_rd));
    memcpy(lp_ret_rd, lp_mid_rd, EGG_IDXBOUGHRD_SZ(lp_mid_rd));
    free(lp_tmp_buf);
    return lp_ret_rd;
}





eggIndexLeafNd_t* eggIndexLeafNd_init(void* addr)
{
    eggIndexLeafNd_t* p_nd = (eggIndexLeafNd_t*)addr;
    p_nd->ty = EGG_IDX_LEAF_ND;
    p_nd->useSz = sizeof(eggIndexLeafNd_t);
    p_nd->next = 0;
    return p_nd;
}


int eggIndexLeafNd_find(eggIndexLeafNd_t* pNd, eggIndexLeafRd_t* pRd, char** pos, INDEXRDCMP fn)
{
    if(!pNd || !pRd)
    {
        return 0;
    }
    eggIndexLeafRd_t* p_iter = (eggIndexLeafRd_t*)(pNd + 1);
    int n_find_ret = 0;
    void* p_rd_tail = (char*)pNd + pNd->useSz;
    while(p_iter != p_rd_tail)
    {
        int n_cmp_ret = fn(p_iter, pRd);
        if (n_cmp_ret == 1)
        {
            p_iter = (eggIndexLeafRd_t*)((char*)p_iter + EGG_IDXLEAFRD_SZ(p_iter));
        }
        else if (n_cmp_ret == -1)
        {
            break;
        }
        else
        {
            n_find_ret = 1;
            break;
        }
    }
    
    *pos = (void*)p_iter;
    return n_find_ret;
}


int eggIndexLeafNd_insert(eggIndexLeafNd_t* pNd, eggIndexLeafRd_t* pRd, INDEXRDCMP fn)
{
    if(!pNd || !pRd || EGG_IDXLEAFND_IS_FULL(pNd, pRd))
    {
        return EGG_FALSE;
    }

    eggIndexLeafRd_t* p_insert_pos = EGG_NULL;
    int n_find_ret = eggIndexLeafNd_find(pNd, pRd, (char**)&p_insert_pos, fn);
    if(!n_find_ret)
    {
        void* p_rd_tail = (char*)pNd + pNd->useSz;
        if(p_insert_pos != p_rd_tail)
        {
            size_t n_move_size = (size_t)p_rd_tail - (size_t)p_insert_pos;
            memmove((char*)p_insert_pos + EGG_IDXLEAFRD_SZ(pRd), p_insert_pos, n_move_size);
        }
        memcpy(p_insert_pos, pRd, EGG_IDXLEAFRD_SZ(pRd));

        pNd->useSz += EGG_IDXLEAFRD_SZ(pRd);
        return EGG_TRUE;
        
    }
    else
    {
        return EGG_FALSE;
    }
}



int eggIndexLeafNd_insert_withPos(eggIndexLeafNd_t* pNd, eggIndexLeafRd_t* pRd, void* pos)
{
    if(!pNd || !pRd || EGG_IDXLEAFND_IS_FULL(pNd, pRd) || !pos)
    {
        return EGG_FALSE;
    }

    eggIndexLeafRd_t* p_insert_pos = pos;
    void* p_rd_tail = (char*)pNd + pNd->useSz;
    if(p_insert_pos != p_rd_tail)
    {
        size_t n_move_size = (size_t)p_rd_tail - (size_t)p_insert_pos;
        memmove((char*)p_insert_pos + EGG_IDXLEAFRD_SZ(pRd), p_insert_pos, n_move_size);
    }
    memcpy(p_insert_pos, pRd, EGG_IDXLEAFRD_SZ(pRd));

    pNd->useSz += EGG_IDXLEAFRD_SZ(pRd);
    return EGG_TRUE;
}


eggIndexLeafRd_t* eggIndexLeafNd_split_withPos(eggIndexLeafNd_t* pOrgNd, eggIndexLeafNd_t* pNewNd, eggIndexLeafRd_t* pRd, void* pInsertPos)
{
     uint32_t n_old_sz = pOrgNd->useSz -sizeof(eggIndexLeafNd_t);
    uint32_t n_tmp_sz = n_old_sz + EGG_IDXLEAFRD_SZ(pRd);
    char* lp_tmp_buf = (char*)malloc(n_tmp_sz);
    
    uint32_t n_front_half_sz = (uint32_t)((size_t)pInsertPos - (size_t)pOrgNd) - sizeof(eggIndexLeafNd_t);
    
    memcpy(lp_tmp_buf, pOrgNd + 1, n_front_half_sz);
    memcpy(lp_tmp_buf + n_front_half_sz, pRd, EGG_IDXLEAFRD_SZ(pRd));
    memcpy(lp_tmp_buf + n_front_half_sz + EGG_IDXLEAFRD_SZ(pRd), pInsertPos, pOrgNd->useSz - n_front_half_sz - sizeof(eggIndexLeafNd_t));

    

    char* lp_mid_pos = lp_tmp_buf + n_tmp_sz/2;
    eggIndexLeafRd_t* lp_mid_rd = EGG_NULL;
    eggIndexLeafRd_t* p_iter = (eggIndexLeafRd_t*)lp_tmp_buf;

    while((size_t)p_iter != (size_t)(lp_tmp_buf + pOrgNd->useSz))
    {
        if((size_t)p_iter <= (size_t)lp_mid_pos && (size_t)lp_mid_pos <= (size_t)((char*)p_iter + EGG_IDXLEAFRD_SZ(p_iter)))
        {
            lp_mid_rd = p_iter;
            break;
        }
        p_iter = (eggIndexLeafRd_t*)((char*)p_iter + EGG_IDXLEAFRD_SZ(p_iter));
    }    
    if(((size_t)lp_mid_pos - (size_t)lp_mid_rd) < ((size_t)lp_mid_rd + EGG_IDXLEAFRD_SZ(lp_mid_rd) - (size_t)lp_mid_pos))
    {
        //copy to new node
         pOrgNd->useSz = (size_t)lp_mid_rd - (size_t)lp_tmp_buf + sizeof(eggIndexLeafNd_t);
        memcpy(pOrgNd + 1, lp_tmp_buf, pOrgNd->useSz - sizeof(eggIndexLeafNd_t));
        pNewNd->useSz = (size_t)lp_tmp_buf + n_tmp_sz - (size_t)lp_mid_rd + sizeof(eggIndexLeafNd_t);
        memcpy(pNewNd + 1, lp_mid_rd, pNewNd->useSz - sizeof(eggIndexLeafNd_t));
    }
    else
    {

        //cpoy to old node
        pOrgNd->useSz = (size_t)lp_mid_rd + EGG_IDXLEAFRD_SZ(lp_mid_rd) - (size_t)lp_tmp_buf + sizeof(eggIndexLeafNd_t);
        memcpy(pOrgNd + 1, lp_tmp_buf, pOrgNd->useSz - sizeof(eggIndexLeafNd_t));
        pNewNd->useSz = (size_t)lp_tmp_buf + n_tmp_sz - (size_t)lp_mid_rd - EGG_IDXLEAFRD_SZ(lp_mid_rd) + sizeof(eggIndexLeafNd_t);
        lp_mid_rd = (eggIndexLeafRd_t*)((char*)lp_mid_rd + EGG_IDXLEAFRD_SZ(lp_mid_rd));
        memcpy(pNewNd + 1, lp_mid_rd, pNewNd->useSz - sizeof(eggIndexLeafNd_t));

    }

    eggIndexLeafRd_t* lp_ret_rd = (eggIndexLeafRd_t*)malloc(EGG_IDXLEAFRD_SZ(lp_mid_rd));
    memcpy(lp_ret_rd, lp_mid_rd, EGG_IDXLEAFRD_SZ(lp_mid_rd));
    free(lp_tmp_buf);
    return lp_ret_rd;

}



eggIndexLeafRd_t* eggIndexLeafNd_split(eggIndexLeafNd_t* pOrgNd, eggIndexLeafNd_t* pNewNd, eggIndexLeafRd_t* pRd, INDEXRDCMP fn)
{
     uint32_t n_old_sz = pOrgNd->useSz - sizeof(eggIndexLeafNd_t) ;
    uint32_t n_tmp_sz = n_old_sz + EGG_IDXLEAFRD_SZ(pRd);
    char* lp_tmp_buf = (char*)malloc(n_tmp_sz);
    memcpy(lp_tmp_buf, pOrgNd + 1, n_old_sz);  
    eggIndexLeafRd_t* p_iter = (eggIndexLeafRd_t*)lp_tmp_buf;
    
    while(1)
    {
        
        if((size_t)p_iter == ((size_t)lp_tmp_buf + pOrgNd->useSz))
        {
            memcpy(p_iter, pRd, EGG_IDXLEAFRD_SZ(pRd));
            break;
        }
        int n_cmp_ret = fn(p_iter, pRd);
        if (n_cmp_ret == 1)
        {
           
            p_iter = (eggIndexLeafRd_t*)((char*)p_iter + EGG_IDXLEAFRD_SZ(p_iter));
        }
        else if (n_cmp_ret == -1)
        {
            size_t n_move_size = (size_t)lp_tmp_buf +  (size_t)n_old_sz - (size_t)p_iter;
            memmove((char*)p_iter + EGG_IDXLEAFRD_SZ(pRd), p_iter, n_move_size);
            memcpy(p_iter, pRd, EGG_IDXLEAFRD_SZ(pRd));
            break;
        }
        else
        {
            //find same key !!
            free(lp_tmp_buf);
            return EGG_NULL;
        }

    }

    char* lp_mid_pos = lp_tmp_buf + n_tmp_sz/2;
    eggIndexLeafRd_t* lp_mid_rd = p_iter;
    p_iter = (eggIndexLeafRd_t*)lp_tmp_buf;

    while((size_t)p_iter != ((size_t)lp_tmp_buf + n_old_sz))
    {
        if((size_t)p_iter <= (size_t)lp_mid_pos && (size_t)lp_mid_pos <= ((size_t)p_iter + EGG_IDXLEAFRD_SZ(p_iter)))
        {
            lp_mid_rd = p_iter;
            break;
        }
        p_iter = (eggIndexLeafRd_t*)((char*)p_iter + EGG_IDXLEAFRD_SZ(p_iter));
    }    
    if(((size_t)lp_mid_pos - (size_t)lp_mid_rd) < ((size_t)lp_mid_rd + EGG_IDXLEAFRD_SZ(lp_mid_rd) - (size_t)lp_mid_pos))
    {
        //copy to new node
         pOrgNd->useSz = (size_t)lp_mid_rd - (size_t)lp_tmp_buf + sizeof(eggIndexLeafNd_t);
        memcpy(pOrgNd + 1, lp_tmp_buf, pOrgNd->useSz - sizeof(eggIndexLeafNd_t));
        pNewNd->useSz = (size_t)lp_tmp_buf + n_tmp_sz - (size_t)lp_mid_rd + sizeof(eggIndexLeafNd_t);
        memcpy(pNewNd + 1, lp_mid_rd, pNewNd->useSz - sizeof(eggIndexLeafNd_t));
    }
    else
    {

        //cpoy old node
         pOrgNd->useSz = (size_t)lp_mid_rd + EGG_IDXLEAFRD_SZ(lp_mid_rd) - (size_t)lp_tmp_buf + sizeof(eggIndexLeafNd_t);
        memcpy(pOrgNd + 1, lp_tmp_buf, pOrgNd->useSz - sizeof(eggIndexLeafNd_t));
         pNewNd->useSz = (size_t)lp_tmp_buf + n_tmp_sz - (size_t)lp_mid_rd - EGG_IDXLEAFRD_SZ(lp_mid_rd) + sizeof(eggIndexLeafNd_t);
         lp_mid_rd = (eggIndexLeafRd_t*)((char*)lp_mid_rd + EGG_IDXLEAFRD_SZ(lp_mid_rd));
        memcpy(pNewNd + 1, lp_mid_rd, pNewNd->useSz - sizeof(eggIndexLeafNd_t));

    }

    eggIndexLeafRd_t* lp_ret_rd = (eggIndexLeafRd_t*)malloc(EGG_IDXLEAFRD_SZ(lp_mid_rd));
    memcpy(lp_ret_rd, lp_mid_rd, EGG_IDXLEAFRD_SZ(lp_mid_rd));
    free(lp_tmp_buf);
    return lp_ret_rd;

}



eggIndex_t* eggIndex_new(HVIEWSTREAM hViewStream, eggIndexInf_t* pInfo)
{
    if(!hViewStream)
    {
        return EGG_NULL; 
    }

    eggIndex_t* lp_index = (eggIndex_t*)malloc( sizeof(eggIndex_t) );
    lp_index->hViewStream = hViewStream;
    
    memcpy(&lp_index->info, pInfo, sizeof(eggIndexInf_t));

    return lp_index;
}

int eggIndex_delete(eggIndex_t* lp_index)
{
    if(!lp_index)
    {
        return EGG_FALSE; 
    }
    
    ViewStream_delete(lp_index->hViewStream);
    free(lp_index);
    
    return EGG_TRUE;
}

eggIndexLeafRd_t* eggIndex_find(eggIndex_t* hIndex, char* key, uint16_t kSz)
{
    if(!hIndex || !key)
    {
        return EGG_FALSE;
    }

    if(hIndex->info.root == EGG_INVALID_PAGENO)
    {
        return EGG_FALSE;
    }
    HVIEWSTREAM hViewStream   = hIndex->hViewStream;
    uint32_t n_node_sz = EGG_IDXNDSZ;
    INDEXRDCMP rdCmp;
    eggIndexLeafRd_t* lp_ret_rd = EGG_NULL;

    eggIndexNd_t* p_com_nd = malloc(n_node_sz);

    pageno_t n_iter_no = hIndex->info.root;

    ViewStream_read(hViewStream, p_com_nd, n_node_sz, n_iter_no);
    

    if(p_com_nd->ty == EGG_IDX_BOUGH_ND)
    {
        eggIndexBoughRd_t* lp_iter_rd = EGG_NULL;
        eggIndexBoughRd_t* lp_bough_rd = eggIndexBoughRd_new(key, kSz);
        rdCmp = rdcmp_bough_str;
                
        while(1)
        {
            if(eggIndexBoughNd_find((eggIndexBoughNd_t*)(p_com_nd), lp_bough_rd, (char**)&lp_iter_rd, rdCmp) == 1)
            {
                //find key
                n_iter_no = lp_iter_rd->host;
            }
            else
            {
                //no find key
                n_iter_no = (size_t)EGG_IDXND_TAILPOS(p_com_nd) == (size_t)lp_iter_rd ?
                    ((eggIndexBoughNd_t*)(p_com_nd))->rchild :lp_iter_rd->lchild;   
            }

            ViewStream_read(hViewStream, p_com_nd, n_node_sz, n_iter_no);
            
            if (p_com_nd->ty == EGG_IDX_LEAF_ND)
                break;
        }
        eggIndexBoughRd_delete(lp_bough_rd);
    }

    //search leaf
    eggIndexLeafRd_t* lp_iter_rd = EGG_NULL;
    eggIndexLeafRd_t* lp_leaf_rd = eggIndexLeafRd_new(key, kSz, EGG_NULL, EGG_NULL);
    rdCmp = rdcmp_leaf_str;

    if(eggIndexLeafNd_find((eggIndexLeafNd_t*)(p_com_nd), lp_leaf_rd, (char**)&lp_iter_rd, rdCmp) == 1)
    {
        //find key
        lp_ret_rd = (eggIndexLeafRd_t*)malloc(EGG_IDXLEAFRD_SZ(lp_iter_rd));
        memcpy(lp_ret_rd, lp_iter_rd, EGG_IDXLEAFRD_SZ(lp_iter_rd));
    }
    
    eggIndexLeafRd_delete(lp_leaf_rd);
    free(p_com_nd);

    return lp_ret_rd;
    
}




int eggIndex_add(eggIndex_t* hIndex, char* key, uint16_t kSz, char* val, uint32_t vSz)
{
    if(!hIndex || !key || !val)
    {
        return EGG_FALSE;
    }

    if(hIndex->info.root == EGG_INVALID_PAGENO)
    {
        return EGG_FALSE;
    }
    HVIEWSTREAM hViewStream  = hIndex->hViewStream;
    uint32_t n_node_sz = EGG_IDXNDSZ;
    INDEXRDCMP rdCmp;

    eggIndexNd_t* p_com_nd = malloc(n_node_sz);
    pageno_t n_iter_no = hIndex->info.root;
    ViewStream_read(hViewStream, p_com_nd, n_node_sz, n_iter_no);

    eggIndexNdList_t* p_nd_head = eggIndexNdList_push(EGG_NULL, n_iter_no, p_com_nd, EGG_NULL);
    

    if(p_com_nd->ty == EGG_IDX_BOUGH_ND)
    {
        eggIndexBoughRd_t* lp_iter_rd = EGG_NULL;
        eggIndexBoughRd_t* lp_bough_rd = eggIndexBoughRd_new(key, kSz);
        rdCmp = rdcmp_bough_str;
                
        while(1)
        {
            if(eggIndexBoughNd_find((eggIndexBoughNd_t*)(p_com_nd), lp_bough_rd, (char**)&lp_iter_rd, rdCmp) == 1)
            {
                //find same key, return !
                eggIndexBoughRd_delete(lp_bough_rd);
                eggIndexNdList_free(p_nd_head);
                return EGG_FALSE;
            }
                //no find key
            n_iter_no = (size_t)EGG_IDXND_TAILPOS(p_com_nd) == (size_t)lp_iter_rd ?
                ((eggIndexBoughNd_t*)(p_com_nd))->rchild :lp_iter_rd->lchild;   

            p_com_nd = malloc(n_node_sz);
            ViewStream_read(hViewStream, p_com_nd, n_node_sz, n_iter_no);
            p_nd_head = eggIndexNdList_push(p_nd_head, n_iter_no, p_com_nd, lp_iter_rd);
            
            if (p_com_nd->ty == EGG_IDX_LEAF_ND)
                break;
        }
        eggIndexBoughRd_delete(lp_bough_rd); 
    }

    //search leaf
    eggIndexRd_t* lp_insert_rd = (eggIndexRd_t*)eggIndexLeafRd_new(key, kSz, val, vSz);
    eggIndexRd_t* lp_insert_pos = EGG_NULL;
    

    rdCmp = rdcmp_leaf_str;

    if(eggIndexLeafNd_find((eggIndexLeafNd_t*)(p_com_nd), lp_insert_rd, (char**)&lp_insert_pos, rdCmp) == 1)
    {
        //find same key, return !
        eggIndexLeafRd_delete(lp_insert_rd);
        eggIndexNdList_free(p_nd_head);
        return EGG_FALSE;
    }

    eggIndexNdList_t* p_nd_iter = p_nd_head;
    pageno_t n_new_pageno  = 0;
    do
    {
         lp_insert_rd = eggIndex_split(hIndex, p_nd_iter,  lp_insert_rd, &n_new_pageno);
        if(lp_insert_rd == EGG_NULL)
            break;
        p_nd_iter = p_nd_iter->next;
    }while(1);

    return EGG_TRUE;
}

eggIndexRd_t* eggIndex_split(eggIndex_t* hIndex, eggIndexNdList_t* pNdList,   eggIndexRd_t* pInsertRd, pageno_t* p_new_pageno)
{
    if(!hIndex || !pNdList || !pInsertRd )
    {
        return EGG_NULL;
    }

    HVIEWSTREAM hViewStream = hIndex->hViewStream;
    uint32_t n_node_sz = EGG_IDXNDSZ;
    INDEXRDCMP rdCmp;
    eggIndexRd_t* lp_ret_rd = EGG_NULL;
    
    if(!pNdList)
    {
         eggIndexBoughNd_t* p_root_nd = (eggIndexBoughNd_t*)eggIndexBoughNd_init(malloc(n_node_sz));

         p_root_nd->useSz += EGG_IDXBOUGHRD_SZ(pInsertRd);
         memcpy(p_root_nd + 1, pInsertRd, EGG_IDXBOUGHRD_SZ(pInsertRd));
         p_root_nd->rchild = *p_new_pageno;

         hIndex->info.root =  ViewStream_write(hViewStream, p_root_nd, n_node_sz);
              
         return EGG_NULL;
    }
        
    if(pNdList->nd->ty == EGG_IDX_LEAF_ND)
    {
        
        rdCmp = rdcmp_leaf_str;
        eggIndexLeafNd_t* p_old_nd = (eggIndexLeafNd_t*)pNdList->nd;
        pageno_t n_old_pageno = pNdList->pageno;

        eggIndexLeafRd_t* p_insert_rd = pInsertRd;
        if(EGG_IDXLEAFND_IS_FULL(p_old_nd, p_insert_rd))
        {
//create new nd
             eggIndexLeafNd_t* p_new_nd = eggIndexLeafNd_init(malloc(n_node_sz));
             *p_new_pageno =  ViewStream_write(hViewStream, p_new_nd, n_node_sz);

//split old nd             
             eggIndexBoughRd_t* p_father_insert_rd = eggIndexRd_leaf_to_bough(eggIndexLeafNd_split(p_old_nd, p_new_nd,
             p_insert_rd,  rdCmp));

//update father insert Rd 
             p_father_insert_rd->lchild = n_old_pageno;
             p_father_insert_rd->host = *p_new_pageno;

//update leaf list pageno
             p_new_nd->next = p_old_nd->next;
             p_old_nd->next = *p_new_pageno;

//update new nd and old nd to file
             ViewStream_update(hViewStream, p_old_nd, n_node_sz, n_old_pageno);
             ViewStream_update(hViewStream, p_new_nd, n_node_sz, * p_new_pageno);

             lp_ret_rd = p_father_insert_rd;
        }
        else
        {
             eggIndexLeafNd_insert(p_old_nd,  p_insert_rd, rdCmp);
             
             lp_ret_rd = EGG_NULL;
        }
    }
    else
    {
        // the same with leaf node
        rdCmp = rdcmp_bough_str;

        eggIndexBoughNd_t* p_old_nd = (eggIndexBoughNd_t*)pNdList->nd;
        pageno_t n_old_pageno = pNdList->pageno;

        eggIndexBoughRd_t* p_insert_rd = pInsertRd;
        if(EGG_IDXBOUGHND_IS_FULL(p_old_nd, p_insert_rd))
        {
//create new nd
             eggIndexBoughNd_t* p_new_nd = eggIndexBoughNd_init(malloc(n_node_sz));
             *p_new_pageno =  ViewStream_write(hViewStream, p_new_nd, n_node_sz);

//split old nd             
             eggIndexBoughRd_t* p_insert_pos = EGG_NULL;
             eggIndexBoughNd_find(p_old_nd, p_insert_rd, (char**)&p_insert_pos, rdCmp);
             if((size_t)EGG_IDXND_TAILPOS(p_old_nd) == (size_t)p_insert_pos)
             {
                  p_old_nd->rchild = * p_new_pageno;
             }
             else
             {
                  p_insert_pos->lchild = * p_new_pageno;
             }

             eggIndexBoughRd_t* p_father_insert_rd = eggIndexBoughNd_split_withPos(p_old_nd, p_new_nd, p_insert_rd,  p_insert_pos);

//update father insert Rd 
             p_father_insert_rd->lchild = n_old_pageno;
             


//update new nd and old nd to file
             ViewStream_update(hViewStream, p_old_nd, n_node_sz, n_old_pageno);
             ViewStream_update(hViewStream, p_new_nd, n_node_sz, * p_new_pageno);

             lp_ret_rd = p_father_insert_rd;
        }
        else
        {
             eggIndexBoughRd_t* p_insert_pos = EGG_NULL;
             eggIndexBoughNd_find(p_old_nd, p_insert_rd, (char**)&p_insert_pos, rdCmp);
             if((size_t)EGG_IDXND_TAILPOS(p_old_nd) == (size_t)p_insert_pos)
             {
                  p_old_nd->rchild = * p_new_pageno;
             }
             else
             {
                  p_insert_pos->lchild = * p_new_pageno;
             }

             eggIndexBoughNd_insert_withPos(p_old_nd,  p_insert_rd, p_insert_pos);
             
             lp_ret_rd = EGG_NULL;
        }
    

    }    
}
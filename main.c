
#include <stdio.h>
#include <stdlib.h>
#include <eggIndex.h>
int main(int argc, char** argv)
{

    unlink("./btree");
    HEGGFILE hEggFile = EggFile_open("./btree");
    HVIEWSTREAM hViewStream = ViewStream_new(hEggFile);
    eggIndexInf_t info = {0};
    eggIndex_t* p_index = eggIndex_new(hViewStream, &info);
    offset64_t off = 0;
    char num[12];
    int i =0;
    int cnt = atoi(argv[1]);
    while(i < cnt)
    {
        int randnum = rand()%1000000;
        memset(num, 0, 10);
        sprintf(num, "%d", randnum);
        //  printf("num : [%s]\n", num);
        eggIndex_add(p_index, num, 10, &off, sizeof(off));
        i++;
    }

    //  eggIndex_leafNd_result(p_index);
    printf("root : %lld  leaf : %lld \n", p_index->info.root, p_index->info.leaf);

    printf("\n===============================\n");
    i=0;
    while(i < cnt)
    {
        int randnum = 100000-i;//rand()%10000;
        memset(num, 0, 10);
        sprintf(num, "%d", randnum);
//        printf("num : [%s]\n", num);
        eggIndex_add(p_index, num, 10, &off, sizeof(off));
        i++;
    }

    
    eggIndex_delete(p_index);

     return 0;
}


#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <eggIndex.h>

int main(int argc, char** argv)
{

    unlink("./btree");

    viewStream_t* hViewStream = viewStream_new("./btree");
    eggIndexInf_t info = {0};
    info.root = EGG_INVALID_PAGENO;
    eggIndex_t* p_index = eggIndex_new(hViewStream, &info);
    offset64_t off = 0;
    int i =0;
    FILE* fp_file = fopen("kfc.txt", "r+");

    

    char *buf = EGG_NULL;
    size_t fileSize = 0;
    int cnt = 0;
    while (getline(&buf, &fileSize, fp_file) != -1)
    {
        char* blank = strchr(buf, ' ');
        if(blank)
            *blank='\0';
        eggIndex_add(p_index, buf, strlen(buf)+1, (char*)&off, sizeof(off));
        i++;
        cnt++;
    }
    printf("cnt : %d\n", cnt);
    eggIndex_leafNd_result(p_index);
    eggIndex_delete(p_index);
    fclose(fp_file);

     return 0;
}

/*
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
//    FILE* fp_file = fopen("kfc.txt", "r+");

    
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

    
    eggIndex_delete(p_index);

     return 0;
}
*/

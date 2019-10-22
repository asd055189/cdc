#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include "rabin.h"

// 1MiB buffer
uint8_t buf[1024*1024]; //1024*1024byte
size_t bytes;


int main(void) {
    struct rabin_t *hash;
    hash = rabin_init();
    FILE *pFile;

    pFile = fopen( "ChunkedSize.txt","w" );

    if( NULL == pFile ){

        printf( "open failure" );

        return 1;

    }
    unsigned int chunks = 0;
   // int i=1;
    while (!feof(stdin)) {
        //copy the file to buffer, len is data size(Byte)
        size_t len = fread(buf, 1, sizeof(buf), stdin);
        uint8_t *ptr = &buf[0];
        //printf("%uint8_t **",&ptr);
        bytes += len;
       // printf("interation %d\n",i);
        while (1) {

            int remaining = rabin_next_chunk(hash, ptr, len);

            if (remaining < 0) {
                break;
            }
            //printf("%p address\n" ,ptr);
            len -= remaining;
            ptr += remaining;
           // printf("%p address \n%x value \n%d remaining\n",ptr,*ptr,remaining);
            //printf("chunksize %d Byte \n",last_chunk.length);
            fprintf(pFile, "%d\n",last_chunk.length);

            chunks++;
        }
       // i++;
    }

    if (rabin_finalize(hash) != NULL) {
        chunks++;
         //printf("chunksize %d Byte \n", last_chunk.length);
         fprintf(pFile, "%d\n",last_chunk.length);
    }

    unsigned int avg = 0;
    if (chunks > 0)
        avg = bytes / chunks; 
    printf("%d chunks, average chunk size %d byte", chunks, avg);
     fclose(pFile);
    return 0;
}

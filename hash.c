#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <openssl/sha.h>
unsigned char buf[8*1024*1024];//unsigned char = 1byte


int main(void){
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    fp = fopen("Small_file_ChunkedSize.txt", "r");
    int count=1;
    while ((read = getline(&line, &len, fp)) != -1) {
    	
	    int chunksize = atoi(line);
	    printf("%d  %d ",count,chunksize );
	        size_t len = fread(buf, 1,chunksize, stdin);
	 		unsigned char hash[20];
	 		SHA1(buf,chunksize, hash);
	 		for (int i=0;i<20;i++)
	 			printf("%x",hash[i]);
	 		printf("\n");
	    count ++;
	}
    if (line)
       free(line);
	fclose(fp);
}
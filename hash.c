#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <openssl/sha.h>
#include <stdlib.h>
unsigned char buf2[8*1024*1024];//unsigned char = 1byte
#define HASH_TABLE_INDEX 65536
//gcc -o hash hash.c -L/usr/local/opt/openssl/lib -lcrypto -I/usr/local/opt/openssl/include/
typedef struct node Node;
struct node{
	unsigned char fingerprint[20]; //8*20=160
	Node *next;
	int size;
	int linked_count;
};

int main(){
	FILE *pFile;
    pFile = fopen( "log.txt","w" );
	Node *hash_table[HASH_TABLE_INDEX]={};
	FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    double filesize=0;
    double dedupsize=0;
    fp = fopen("Small_file_ChunkedSize.txt", "r");
    int count=1;
    while ((read = getline(&line, &len, fp)) != -1) {
	    int chunksize = atoi(line);
	    filesize+=chunksize;
	    fprintf(pFile,"  chunksize = %d ",chunksize );
	        size_t len = fread(buf2, 1,chunksize, stdin);
	 		unsigned char hash[20]; //8 (bit/index) (8*20=160 bit)
	 		SHA1(buf2,chunksize, hash);
	 		for (int i=0;i<20;i++)
	 			fprintf(pFile,"%02x",hash[i]);  //8bit have two digits in hex
	 		int index=0;
	 		int num;
	 		num = hash[18];
	 		index+=(hash[18]/16)*16*16*16+(hash[18]%16)*16*16;
	 		num = hash[19];
	 		index+=(hash[19]/16)*16+(hash[19]%16);
	 		Node *tmp=(Node*)malloc(sizeof(Node));
	 		tmp->size=chunksize;
	 		tmp->next=NULL;
	 		tmp->linked_count=1;
	 		for (int i=0;i<20;i++)
	 			tmp->fingerprint[i]=hash[i];
	 		fprintf(pFile,"  %d",index);
	 		if (hash_table[index]==NULL){
	 			fprintf(pFile,"is empty! ");
	 			hash_table[index]=tmp;
	 		}
	 		else{
	 			Node* ptr=hash_table[index];
	 			ptr->linked_count++;
	 			fprintf(pFile,"this inex has %d entry",ptr->linked_count );
	 			while(ptr->next!=NULL)
	 				ptr=ptr->next;
	 			ptr->next=tmp;
	 		}
	 		fprintf(pFile,"\n");
	}
	if (line)
       free(line);
	fclose(fp);
	fprintf(pFile,"******hash_table built******\n");
 		for (int i=0;i<HASH_TABLE_INDEX;i++){
 			if (hash_table[i]!=0)
	 			if (hash_table[i]->linked_count>1){
	 				fprintf(pFile,"\n%d   %d\n",hash_table[i]->linked_count,i );
	 				unsigned char dup[hash_table[i]->linked_count][20];
	 				int dup_chunksize[hash_table[i]->linked_count];
	 				Node* ptr=hash_table[i];
	 				int pos=0;
	 				while (ptr->next!=NULL){
	 					for (int j=0;j<20;j++){
	 						//fprintf(pFile,"%02x",ptr->fingerprint[j]);
 							dup[pos][j]=ptr->fingerprint[j];
	 					}
 						dup_chunksize[pos]=ptr->size;
 						ptr=ptr->next;
 						pos++;
 					}
 					for (int j=0;j<20;j++){
 							dup[pos][j]=ptr->fingerprint[j];
 							//fprintf(pFile,"%02x",ptr->fingerprint[j]);
 					}
 					dup_chunksize[pos]=ptr->size;
 					int dedupcount=0;
 					for (int a=0;a<hash_table[i]->linked_count;a++){
 						for (int b=a+1;b<hash_table[i]->linked_count;b++){	
 							bool tag=true;
 							for (int c=0;c<20;c++)
 								if (dup[a][c]!=dup[b][c])
 									tag=false;
 							if(tag){
 								fprintf(pFile, "find deduplicate! chunksize = %d\n",dup_chunksize[b]);
 								dedupsize+=dup_chunksize[b];
 								dup_chunksize[b]=0;
 							}
 						}
 					}
	 			}
 		}
	fprintf(pFile,"dedup size/file size = %f/%f (bytes)\n",dedupsize,filesize );
	fprintf(pFile, "dedup rate is %.10f \n",(1.0-dedupsize/filesize));
	fclose(pFile);
}
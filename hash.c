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
    fp = fopen("ChunkedSize.txt", "r");
    int count=1;
    while ((read = getline(&line, &len, fp)) != -1) {
	    int chunksize = atoi(line);
	    filesize+=chunksize;
	   // fprintf(pFile,"  chunksize = %d ",chunksize );
	        size_t len = fread(buf2, 1,chunksize, stdin);
	        if (len != chunksize) {fputs ("\n==========\nReading error, the input file don't match the ChunkedSize\n==========\n",stderr); exit (3);}
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
	 			//fprintf(pFile,"is empty! ");
	 			hash_table[index]=tmp;
	 		}
	 		else{
	 			Node* ptr=hash_table[index];
	 			ptr->linked_count++;
	 			//fprintf(pFile,"this index has %d entry",ptr->linked_count );
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
	 				//fprintf(pFile,"\n%d   %d\n",hash_table[i]->linked_count,i );
	 				Node* ptr=hash_table[i];
	 				while (ptr->next!=NULL){
	 					Node* fptr2=ptr;
						Node* ptr2=ptr->next;
	 					while(ptr2->next!=NULL){
		 					bool tag=true;
		 					for (int j=0;j<20;j++){
		 						if (ptr->fingerprint[j]!=ptr2->fingerprint[j])
		 							tag=false;
		 					}
	 						if(tag){
	 							fprintf(pFile,"dedup! %d entry pos %d\n",ptr2->size,i);
	 							dedupsize+=ptr2->size;
	 							fptr2->next=ptr2->next;
	 							//delete ptr2;
	 						}
	 						ptr2=ptr2->next;
	 						if(!tag)
	 							fptr2=fptr2->next;
 						}
 						bool tag=true;
 						for (int j=0;j<20;j++){
	 						if (ptr->fingerprint[j]!=ptr2->fingerprint[j])
	 							tag=false;
		 				}
		 				if(tag){
		 					fprintf(pFile,"dedup! %d entry pos %d\n",ptr2->size,i);
	 							dedupsize+=ptr2->size;
	 							//fptr2->next=NULL;
	 							//delete ptr2;
	 					}
	 					ptr=ptr->next;
 					}
	 			}
 		}
	fprintf(pFile,"dedup size/file size = %f/%f (bytes)\n",dedupsize,filesize );
	fprintf(pFile, "dedup rate is %.10f \n",dedupsize/filesize);
	fclose(pFile);
}

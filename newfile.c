#include "stdio.h"
#include "stdlib.h"
#include "math.h"

/*
   The following is rather crude demonstration code to read 
   uncompressed and compressed TGA files of 16, 24, or 32 bit 
   TGA. 
   Hope it is helpful.
*/

typedef struct {
   short width;
   short height;
} HEADER;

typedef struct {
   unsigned char r,g,b,a;
} PIXEL;

int main(int argc,char **argv)
{
   int n=0,i,j;
   int bytes2read,skipover = 0;
   unsigned char p[5];
   FILE *fptr;
   HEADER header;
   PIXEL *pixels;

   if (argc < 2) {
      fprintf(stderr,"Usage: %s tgafile\n",argv[0]);
      exit(-1);
   }

   /* Open the file */
   if ((fptr = fopen(argv[1],"r")) == NULL) {
      fprintf(stderr,"File open failed\n");
      exit(-1);
   }

   for(int i = 0; i < 12; i++) {
	fgetc(fptr);	 
   }
   fread(&header.width,1,2,fptr);
   fprintf(stderr,"Width:             %d\n",header.width);

   fread(&header.height,1,2,fptr);
   fprintf(stderr,"Height:            %d\n",header.height);

   fclose(fptr);
}



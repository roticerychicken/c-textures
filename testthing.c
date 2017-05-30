#include <stdio.h>
#include <string.h>

int main()
{
   FILE *fp;
   char c[] = "this is tutorialspoint";
   char buffer[100];

   /* Open file for both reading and writing */
   fp = fopen("file.txt", "w+");

   /* Write data to the file */
   fwrite(c, strlen(c) + 1, 1, fp);

   /* Seek to the beginning of the file */
   fseek(fp, SEEK_SET, 0);

   /* Read and display data */
   fread(buffer, 2, 5, fp);
   printf("%s\n", buffer);
   fclose(fp);
   
   return(0);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

int main()
{
    char ch, file_name[25];
    FILE *fp;
    fp = fopen("/dev/Character-driver-device", "w"); // write mode

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fputs("0123456789\n", fp);
    fclose(fp);

    fp = fopen("/dev/Character-driver-device", "r"); // read mode

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; SEEK_END-i >= 0; i++){
        fseek(fp,i,SEEK_END);
        printf("%c", fgetc(fp));
    }


    fclose(fp);

    return 0;
}
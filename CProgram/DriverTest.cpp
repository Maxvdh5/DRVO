#include <stdio.h>
#include <stdlib.h>

int main()
{
    char ch, file_name[25];
    FILE *fp;
    fp = fopen("/tmp/test.txt", "w"); // write mode

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    fputs("writing some text to file\n", fp);
    fclose(fp);

    fp = fopen("/tmp/test.txt", "r"); // read mode

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        exit(EXIT_FAILURE);
    }

    while((ch = fgetc(fp)) != EOF)
        printf("%c", ch);

    fclose(fp);

    return 0;
}
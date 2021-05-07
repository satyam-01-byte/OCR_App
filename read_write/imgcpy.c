#include <stdio.h>
#include <stdlib.h>

void ignoreComments(FILE *fi)
{
    if (fgetc(fi) == '#')
        while (fgetc(fi) != '\n')
            ;
    else
        fseek(fi, -1, SEEK_CUR);
}

int main(int argc, char *argv[])
{
    FILE *fi, *fo;
    fi = fopen(argv[1], "r");
    fo = fopen(argv[2], "w");
    if (fi == NULL || fo == NULL)
    {
        printf("Unable to open files.\n");
        exit(1);
    }

    char magic[3];
    int height, width, maxGrayValue;
    fgets(magic, 3, fi); //scan upto 2 characters or EOF or \n
    fscanf(fi, "\n");
    ignoreComments(fi);

    fscanf(fi, "%d %d\n", &height, &width);
    ignoreComments(fi);

    if (magic == "P2")
        fscanf(fi, "%d\n", &maxGrayValue);
    ignoreComments(fi);

    int **image = (int **)malloc(height * sizeof(int *));
    for (int i = 0; i < height; i++)
        image[i] = (int *)malloc(width * sizeof(int));

    //initialise with -1 for when input file data is incomplete
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
        {
            image[i][j] = -1;
        }

    int h, w;
    h = w = 0;
    for (int i = 0; i < height * width; i++)
    {
        if (feof(fi))
            break;
        if (w >= width)
            h++, w = 0;
        if (fgetc(fi) == '\n')
            fseek(fi, 0, SEEK_CUR);
        else
        {
            fseek(fi, -1, SEEK_CUR);
            fscanf(fi, "%d ", &image[h][w++]);
        }
    }

    fprintf(fo, "%s\n", magic);
    fprintf(fo, "%d %d\n", height, width);

    if (magic == "P2")
        fprintf(fi, "%d\n", maxGrayValue);

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] != -1)
                fprintf(fo, "%d ", image[i][j]);
        }
        fprintf(fo, "\n");
    }
    fclose(fi);
    fclose(fo);
}

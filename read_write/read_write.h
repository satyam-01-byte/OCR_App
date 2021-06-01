#include <stdlib.h>
#include <stdio.h>

void ignoreComments(FILE *fi)
{
    if (fgetc(fi) == '#')
        while (fgetc(fi) != '\n')
            ;
    else
        fseek(fi, -1, SEEK_CUR);
}

void checkFileStatus(FILE *fi, FILE *fo)
{
    if (fi == NULL)
        printf("Unable to open 1st file!\n");
    if (fo == NULL)
        printf("Unable to open 2nd file!\n");
    if (fi == NULL || fo == NULL)
    {
        printf("Please, check your files.\n");
        exit(1);
    }
}

char *readMagicNumber(FILE *fi)
{
    char *magic = (char *)malloc(3);
    fgets(magic, 3, fi); //scan upto 2 characters or EOF or \n
    fscanf(fi, "\n");
    ignoreComments(fi);
    return magic;
}

int readWidth(FILE *fi)
{
    int width;
    fscanf(fi, "%d ", &width);
    return width;
}

int readHeight(FILE *fi)
{
    int height;
    fscanf(fi, "%d\n", &height);
    ignoreComments(fi);
    return height;
}

int readMaxGrayValue(FILE *fi, char *magic)
{
    int maxGrayValue = -1;
    if (magic == "P2")
        fscanf(fi, "%d\n", &maxGrayValue);
    ignoreComments(fi);
    return maxGrayValue;
}

int **initialiseImageArray(int height, int width)
{
    int **image = (int **)malloc(height * sizeof(int *));
    for (int i = 0; i < height; i++)
        image[i] = (int *)malloc(width * sizeof(int));

    //initialise with -1 for when input file data is incomplete
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            image[i][j] = -1;

    return image;
}

void readImageContent(FILE *fi, int **image, int height, int width)
{
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
}

void writeDimensions(FILE *fo, char *magic, int width, int height)
{
    fprintf(fo, "%s\n", magic);
    fprintf(fo, "%d %d\n", width, height);
}

void writeMaxGrayValue(FILE *fo, char *magic, int maxGrayValue)
{
    if (magic == "P2" && maxGrayValue != -1)
        fprintf(fo, "%d\n", maxGrayValue);
}

void writeImageContent(FILE *fo, int **image, int height, int width)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] != -1)
                fprintf(fo, "%d ", image[i][j]);
        }
        fprintf(fo, "\n");
    }
}
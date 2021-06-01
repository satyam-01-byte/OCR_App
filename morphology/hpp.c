#include "../read_write/read_write.h"

int main(int argc, char *argv[])
{
    FILE *fi, *fo;
    fi = fopen(argv[1], "r");
    fo = fopen(argv[2], "w");
    checkFileStatus(fi, fo);

    char *magic = readMagicNumber(fi);
    int width = readWidth(fi);
    int height = readHeight(fi);
    int maxGrayValue = readMaxGrayValue(fi, magic);
    int **image = initialiseImageArray(height, width);
    readImageContent(fi, image, height, width);

    int cnt;
    for (int i = 0; i < height; i++)
    {
        cnt = 0;
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] == 1)
            {
                cnt++;
            }
        }
        for (int j = 0; j < width; j++)
        {
            if (j < cnt)
            {
                image[i][j] = 1;
            }
            else
            {
                image[i][j] = 0;
            }
        }
    }

    writeDimensions(fo, magic, width, height);
    writeMaxGrayValue(fo, magic, maxGrayValue);
    writeImageContent(fo, image, height, width);

    fclose(fi);
    fclose(fo);
}
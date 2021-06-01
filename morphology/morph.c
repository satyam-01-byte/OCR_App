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

    /*structuring element: 1x13 is good here*/
    int bh, bw;
    printf("Enter dimensions\n");
    printf("Enter height: ");
    scanf("%d", &bh);
    printf("Enter width: ");
    scanf("%d", &bw);
    int B[bh][bw];
    printf("B (structuring element) %dX%d\n", bh, bw);
    for (int i = 0; i < bh; i++)
    {
        for (int j = 0; j < bw; j++)
        {
            B[i][j] = 1; //1;
            printf("%d ", B[i][j]);
        }
        printf("\n");
    }

    int flag;
    /*erosion*/
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            flag = 0;
            if (i + bh < height && j + bw < width /*&& i > 0 && j > ((bw / 2) - 1) && (i + (bh / 2)) < height && (j + (bw / 2)) < width)*/)
            {
                for (int k = 0; k < bh; k++)
                {
                    for (int l = 0; l < bw; l++)
                    {
                        if (image[i + k][j + l] != B[k][l])
                        {
                            flag = 1;
                            break;
                        }
                    }
                }
                if (flag == 0)
                {
                    int k, l;
                    for (k = 0; k < bh; k++)
                    {
                        for (l = 0; l < bw; l++)
                        {
                            image[i + k][j + l] = 1; //0;
                        }
                    }
                }
                //image[i + bh / 2][j + bw / 2] = 0;
                else
                    image[i][j] = 0;
                //image[i + bh / 2][j + bw / 2] = 1; //1;
            }
            else
                image[i][j] = 0; //0;
        }
    }

    writeDimensions(fo, magic, width, height);
    writeMaxGrayValue(fo, magic, maxGrayValue);
    writeImageContent(fo, image, height, width);

    fclose(fi);
    fclose(fo);
}
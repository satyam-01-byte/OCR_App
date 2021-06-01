#include "../read_write/read_write.h"
#define PIXEL_RANGE 256

void findPixelHistogram(int pixelHistogram[], int height, int width, int *image[])
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            pixelHistogram[image[i][j]]++;
        }
    }
}

void findPixelBetweenVariance(float betweenVarianceArray[], int pixelHistogram[], int totalPixels)
{
    for (int i = 0; i < PIXEL_RANGE; i++)
    {
        float W1 = 0, W2 = 0, u1 = 0, u2 = 0, betweenVariance;
        for (int j = 0; j < PIXEL_RANGE; j++)
        {
            if (j <= i)
            {
                W1 += pixelHistogram[j];
                u1 += j * pixelHistogram[j];
            }
            else
            {
                W2 += pixelHistogram[j];
                u2 += j * pixelHistogram[j];
            }
        }
        u1 /= W1, u2 /= W2, W1 /= totalPixels, W2 /= totalPixels;
        betweenVariance = W1 * W2 * (u1 - u2) * (u1 - u2);
        betweenVarianceArray[i] = betweenVariance;
    }
}

int MaximumBetweenVariancePixel(float betweenVarianceArray[])
{
    int thresholdPixel = 127;
    float maxBetweenVariance = betweenVarianceArray[127];
    for (int i = 0; i < PIXEL_RANGE; i++)
    {
        if (betweenVarianceArray[i] > maxBetweenVariance)
        {
            thresholdPixel = i;
            maxBetweenVariance = betweenVarianceArray[i];
        }
    }
    return thresholdPixel;
}

void convertToBW(int **image, int height, int width, int thresholdPixel)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] <= thresholdPixel)
                image[i][j] = 0;
            else
                image[i][j] = 255;
        }
    }
}

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

    int pixelHistogram[PIXEL_RANGE] = {0};
    findPixelHistogram(pixelHistogram, height, width, image);

    int totalPixels = height * width;
    float betweenVarianceArray[PIXEL_RANGE];
    findPixelBetweenVariance(betweenVarianceArray, pixelHistogram, totalPixels);

    int thresholdPixel = MaximumBetweenVariancePixel(betweenVarianceArray);
    convertToBW(image, height, width, thresholdPixel);

    writeDimensions(fo, magic, width, height);
    writeMaxGrayValue(fo, magic, maxGrayValue);
    writeImageContent(fo, image, height, width);

    fclose(fi);
    fclose(fo);
}
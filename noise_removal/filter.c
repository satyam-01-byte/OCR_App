#include "../read_write/read_write.h"

int comp(const void *elem1, const void *elem2)
{
    int f = *((int *)elem1);
    int s = *((int *)elem2);
    if (f > s)
        return 1;
    if (f < s)
        return -1;
    return 0;
}

int chooseFilter()
{
    int filterChoice = 0;
    while (filterChoice != 1 && filterChoice != 2)
    {
        printf("Press 1 for mean filter, 2 for median filter: ");
        scanf("%d", &filterChoice);
    }
    return filterChoice;
}

int chooseWindow()
{
    int windowSize = 2;
    while (windowSize % 2 == 0)
    {
        printf("Enter window size (3 for 3x3, 5 for 5x5 and so on (ODD number only)): ");
        scanf("%d", &windowSize);
    }
    return windowSize;
}

int **createPixelReplicatedArray(int newHeight, int newWidth)
{
    int **pixelReplicatedImage = (int **)malloc(newHeight * sizeof(int *));
    for (int i = 0; i < newHeight; i++)
        pixelReplicatedImage[i] = (int *)malloc(newWidth * sizeof(int));
    return pixelReplicatedImage;
}

void copyImageArrayData(int **pixelReplicatedImage, int **image, int height, int width, int halfWindow)
{
    int hIndex, wIndex;
    hIndex = wIndex = halfWindow;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            pixelReplicatedImage[hIndex][wIndex++] = image[i][j];
        }
        hIndex++;
        wIndex = halfWindow;
    }
}

void fillExtraColumnsPixelArray(int **pixelReplicatedImage, int height, int width, int halfWindow)
{
    for (int i = 0; i < halfWindow; i++)
    {
        for (int j = halfWindow; j < ((halfWindow) + height); j++)
        {
            pixelReplicatedImage[j][i] = pixelReplicatedImage[j][halfWindow];
            pixelReplicatedImage[j][i + width + halfWindow] = pixelReplicatedImage[j][halfWindow + width - 1];
        }
    }
}

void fillExtraRowsPixelArray(int **pixelReplicatedImage, int height, int newWidth, int halfWindow)
{
    for (int i = 0; i < halfWindow; i++)
    {
        for (int j = 0; j < newWidth; j++)
        {
            pixelReplicatedImage[i][j] = pixelReplicatedImage[halfWindow][j];
            pixelReplicatedImage[i + height + halfWindow][j] = pixelReplicatedImage[halfWindow + height - 1][j];
        }
    }
}

void meanFilter(int **pixelReplicatedImage, int **image, int height, int width, int windowSize)
{
    int x = 0, y = 0, halfWindow = windowSize / 2;
    for (int i = halfWindow; i < halfWindow + height; i++)
    {
        for (int j = halfWindow; j < halfWindow + width; j++)
        {
            int sum = 0;
            for (int k = 0; k < windowSize; k++)
            {
                for (int l = 0; l < windowSize; l++)
                {
                    sum += pixelReplicatedImage[i - halfWindow + k][j - halfWindow + l];
                }
            }
            if (y >= width)
                x++, y = 0;
            if (pixelReplicatedImage[i][j] == 0 || pixelReplicatedImage[i][j] == 255)
                image[x][y++] = sum / (windowSize * windowSize);
            else
                image[x][y++] = pixelReplicatedImage[i][j];
        }
    }
}

void medianFilter(int **pixelReplicatedImage, int **image, int height, int width, int windowSize)
{
    int x = 0, y = 0, halfWindow = windowSize / 2;
    for (int i = halfWindow; i < halfWindow + height; i++)
    {
        for (int j = halfWindow; j < halfWindow + width; j++)
        {
            int tempArray[windowSize * windowSize], p = 0;
            for (int k = 0; k < windowSize; k++)
            {
                for (int l = 0; l < windowSize; l++)
                {
                    tempArray[p++] = pixelReplicatedImage[i - halfWindow + k][j - halfWindow + l];
                }
            }
            if (y >= width)
                x++, y = 0;
            if (pixelReplicatedImage[i][j] == 0 || pixelReplicatedImage[i][j] == 255)
            {
                qsort(tempArray, windowSize * windowSize, sizeof(*tempArray), comp);
                image[x][y++] = tempArray[(windowSize * windowSize) / 2];
            }
            else
                image[x][y++] = pixelReplicatedImage[i][j];
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
    int height = readHeight(fi);
    int width = readWidth(fi);
    int maxGrayValue = readMaxGrayValue(fi, magic);
    int **image = initialiseImageArray(height, width);
    readImageContent(fi, image, height, width);

    int filterChoice = chooseFilter();
    int windowSize = chooseWindow();

    int halfWindow = windowSize / 2;

    /*
    eg: image array: 3x4, then pixelreplicatedarray = 5x6 for window size 3, 7x8 for window size 5, etc, let's take window size 3
    image array: i
       0 1 2 3 
    0 |i|i|i|i|
    1 |i|i|i|i|
    2 |i|i|i|i|

    pixel replicated array: for window size 3 (5x6)
       0 1 2 3 4 5
    0 |_|_|_|_|_|_|
    1 |_|i|i|i|i|_|
    2 |_|i|i|i|i|_| 
    3 |_|i|i|i|i|_|
    4 |_|_|_|_|_|_|
    */

    //pixel replicated array: array with additional rows and columsn according to window size for ease in applying filter
    int newHeight = height + windowSize - 1;
    int newWidth = width + windowSize - 1;
    int **pixelReplicatedImage = createPixelReplicatedArray(newHeight, newWidth);

    //copy values of image array in pixelreplicatedarray
    copyImageArrayData(pixelReplicatedImage, image, height, width, halfWindow);

    //fill additional columns on both left and right sides in pixelreplicatedarray
    fillExtraColumnsPixelArray(pixelReplicatedImage, height, width, halfWindow);

    //fill additional rows on both top and bottom in pixelreplicatedarray
    fillExtraRowsPixelArray(pixelReplicatedImage, height, newWidth, halfWindow);

    //mean filter
    if (filterChoice == 1)
        meanFilter(pixelReplicatedImage, image, height, width, windowSize);
    //median filter
    else
        medianFilter(pixelReplicatedImage, image, height, width, windowSize);

    writeDimensions(fo, magic, height, width);
    writeMaxGrayValue(fo, magic, maxGrayValue);
    writeImageContent(fo, image, height, width);

    fclose(fi);
    fclose(fo);
}
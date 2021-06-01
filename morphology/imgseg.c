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

    /*getting horizontal projection profile*/
    int cnt;
    int **imghpp = (int **)malloc(height * sizeof(int *));
    for (int i = 0; i < height; i++)
        imghpp[i] = (int *)malloc(width * sizeof(int));

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
                imghpp[i][j] = 1;
            }
            else
            {
                imghpp[i][j] = 0;
            }
        }
    }

    /*finding indices where 'shirorekha' is present, or the longest hpp for each line*/
    int linedata[height], count, maxlen = 0, lineht;
    for (int i = 0; i < height; i++)
        linedata[i] = 0;

    for (int i = 1; i < height; i++)
    {
        if (imghpp[i][0] == 0 && imghpp[i - 1][0] == 1)
        {
            //reset
            linedata[lineht] = 1;
            maxlen = 0;
        }
        else if (imghpp[i][0] == 1)
        {
            count = 0;
            for (int j = 0; j < width; j++)
            {
                if (imghpp[i][j] == 1)
                    count++;
                else
                    break;
            }
            if (count > maxlen)
            {
                maxlen = count;
                lineht = i;
            }
        }
    }

    /*finding no. of lines*/
    int linecnt = 1;
    for (int i = 0; i < height; i++)
    {
        if (linedata[i] == 1)
            linecnt++;
    }

    /*finding average word height from horizontal profile projection*/
    int wordheights[linecnt];
    for (int j = 0; j < linecnt; j++)
    {
        wordheights[j] = 0;
    }

    int j = 0;
    for (int i = 0; i < height; i++)
    {
        if (linedata[i] == 1)
        {
            while (imghpp[i][0] == 1)
            {
                wordheights[j] = wordheights[j] + 1;
                i = i + 1;
            }
        }
    }
    int avgwordheight = 0;
    for (int i = 0; i < linecnt; i++)
    {
        avgwordheight += wordheights[i];
    }
    avgwordheight = avgwordheight / linecnt;
    printf("Avg. word height: %d\n", avgwordheight);

    /*making a horizontal cut above shirorekha (topline)*/
    /*and, horizontal cut avg word height distance below shirorekha (baseline)*/
    for (int i = 0; i < height; i++)
    {
        if (linedata[i] == 1)
        {
            for (int j = 0; j < width; j++)
            {
                if (image[i - 2][j])
                    image[i - 2][j] = 0;
                if (image[i + avgwordheight - 4][j])
                {
                    image[i + avgwordheight - 4][j] = 0;
                }
            }
        }
    }

    /*vertical cut in shirorekha if no pixel found vertically below it for avg word height distance*/
    int found;
    for (int i = 0; i < height; i++)
    {
        if (linedata[i] == 1)
        {
            for (int j = 0; j < width; j++)
            {
                found = 0;
                if (image[i][j] == 1)
                {
                    for (int k = 5; k < avgwordheight - 4; k++)
                    {
                        if (image[i + k][j] == 1)
                            found = 1;
                    }
                    if (found == 0)
                    {
                        for (int c = -1; c <= 2; c++)
                            image[i + c][j] = 0;
                    }
                }
            }
        }
    }

    writeDimensions(fo, magic, width, height);
    writeMaxGrayValue(fo, magic, maxGrayValue);
    writeImageContent(fo, image, height, width);

    fclose(fi);
    fclose(fo);
}
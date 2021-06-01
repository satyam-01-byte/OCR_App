#include "../read_write/read_write.h"

struct Node
{
    int data;
    struct Node *next;
    struct Node *btm;
} *head = NULL;

void dependencyList(int x, int y)
{
    struct Node *t = (struct Node *)malloc(sizeof(struct Node));
    struct Node *b = (struct Node *)malloc(sizeof(struct Node));
    t->data = x;
    b->data = y;
    t->next = head;
    t->btm = b;
    b->next = NULL;
    b->btm = NULL;
    head = t;
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

    int **imgcpy = initialiseImageArray(height, width);

    struct Node *end = (struct Node *)malloc(sizeof(struct Node));
    end->data = 0;
    end->next = NULL;
    end->btm = NULL;
    head = end;

    printf("width: %d, height: %d\n", width, height);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (image[i][j] == 255)
                imgcpy[i][j] = 0;
            else
                imgcpy[i][j] = 1;
        }
    }

    printf("I cannot figure out why height and width have been switched. :(\n");

    printf("\nOriginal image:\n");
    for (int i = 0; i < height; i++)
    {
        printf("\n");
        for (int j = 0; j < width; j++)
        {
            printf("%d ", image[i][j]);
        }
    }

    printf("\n\nImage converted into 0's for background and 1's for foreground\n");
    for (int i = 0; i < height; i++)
    {
        printf("\n");
        for (int j = 0; j < width; j++)
        {
            printf("%d ", imgcpy[i][j]);
        }
    }

    /*1st raster scan*/
    int c = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (imgcpy[i][j] == 1)
            {
                /*if above and left of pixel both don't exist*/
                if (!imgcpy[i - 1][j] && !imgcpy[i][j - 1])
                    imgcpy[i][j] = ++c;
                /*if left exists and above doesn't*/
                else if (!imgcpy[i][j - 1] && imgcpy[i - 1][j])
                {
                    if (imgcpy[i - 1][j] == 0)
                        imgcpy[i][j] = ++c;
                    else
                        imgcpy[i][j] = imgcpy[i - 1][j];
                }
                /*if above exists and left doesn't*/
                else if (!imgcpy[i - 1][j] && imgcpy[i][j - 1])
                {
                    if (imgcpy[i][j - 1] == 0)
                        imgcpy[i][j] = ++c;
                    else
                        imgcpy[i][j] = imgcpy[i][j - 1];
                }
                /*if both above and left exist*/
                else
                {
                    if (imgcpy[i - 1][j] == 0 && imgcpy[i][j - 1] == 0)
                        imgcpy[i][j] = ++c;
                    else if (imgcpy[i - 1][j] == imgcpy[i][j - 1])
                        imgcpy[i][j] = imgcpy[i - 1][j];
                    else
                    {
                        if (imgcpy[i - 1][j] < imgcpy[i][j - 1])
                        {
                            imgcpy[i][j] = imgcpy[i - 1][j];
                            //LL imgcpy[i][j-1] -> imgcpy[i-1][j]
                            dependencyList(imgcpy[i][j - 1], imgcpy[i - 1][j]);
                        }
                        else
                        {
                            imgcpy[i][j] = imgcpy[i][j - 1];
                            //LL imgcpy[i-1][j] -> imgcpy[i][j-1]
                            dependencyList(imgcpy[i - 1][j], imgcpy[i][j - 1]);
                        }
                    }
                }
            }
        }
    }

    printf("\n\nAfter 1st raster scan:\n");
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j % width == 0)
                printf("\n%d ", imgcpy[i][j]);
            else
                printf("%d ", imgcpy[i][j]);
        }
    }

    printf("\n\nDependency list:\n");
    struct Node *head2 = head;
    while (head2->data != 0)
    {
        printf("\n%d -> %d", head2->data, head2->btm->data);
        head2 = head2->next;
    }

    while (head->data != 0)
    {
        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                if (imgcpy[i][j] == head->data)
                    imgcpy[i][j] = head->btm->data;
            }
        }
        head = head->next;
    }

    printf("\n\nAfter 2nd raster scan:\n");
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j % width == 0)
                printf("\n%d ", imgcpy[i][j]);
            else
                printf("%d ", imgcpy[i][j]);
        }
    }
    printf("\n");

    writeDimensions(fo, magic, width, height);
    writeMaxGrayValue(fo, magic, maxGrayValue);
    writeImageContent(fo, image, height, width);

    fclose(fi);
    fclose(fo);
}
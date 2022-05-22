#include <string.h>
#include <stdio.h>

int countDigits(char *string)
{

    int count = 0;
    for (size_t i = 0; i < strlen(string); i++)
    {
        if (*(string + i) <= '9' && *(string + i) >= '0')
            count++;
    }
    return count;
}
int digCmp(char *string1, char *string2)
{
    int count1 = countDigits(string1);
    int count2 = countDigits(string2);

    if (count1 > count2)
        return 1;
    if (count2 > count1)
        return 2;
    if (count2 == count1)
        return 0;

    return -1;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        return -1;
    }
    return digCmp(argv[1], argv[2]);
}
#include <string.h>
int myStrLenCmp(char* string1,char* string2){
	int size1=strlen(string1);
	int size2=strlen(string2);
    if (size1>size2)
    {
        return 1;
    }
	if(size1<size2){
		return 2;
	}
	if(size1==size2)
	return 0;
    
    return -1;
}
int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		return -1;
	}
	return myStrLenCmp(argv[1], argv[2]);
}
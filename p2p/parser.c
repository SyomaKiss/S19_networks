#include <stdio.h>
#include <stdlib.h>
int MAX_WORD_LENGTH = 20;

int amount_of_words(char* filename)
{
    FILE * fp = fopen(filename, "r");
	if (fp == NULL) return 1;
	char c;
	int count = 0;
	while((c = fgetc(fp)) != EOF)
	{
		if(c == ' ' || c == '\n')
		{
			++count;
		}
	}
    fclose(fp);
    return (count + 1);
}

char** convert_text_to_array(char* filename)
{
    int size = amount_of_words(filename);
    FILE * fp = fopen(filename, "r");
    char **data = (char **)calloc(size, sizeof(char *)); 
    for (int i=0; i<size; i++) 
         data[i] = (char *)calloc(MAX_WORD_LENGTH, sizeof(int));
	if (fp == NULL) return (char **)1;
	char c;
	int count = 0;
    int i = -1;
	while((c = fgetc(fp)) != EOF)
	{
		if(c == ' ' || c == '\n')
		{
            data[count][++i] = '\0';
            count++;
            i = -1;
		}
		else
		{
            data[count][++i] = c;
		}
	}
	fclose(fp);
    return data;
}

int main(int argc, char **argv)
{
    int size = amount_of_words("file.txt");
    printf("%d", size);
    char **data = convert_text_to_array("file.txt");
    for (int i = 0; i < size; i++){
        printf("\n%s", data[i]);
    }
    printf("\n");
	return 0;
}

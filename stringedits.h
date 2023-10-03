
int my_putchar(char c)
{
    return write(1, &c, 1);
}

int my_puts(char *s)
{
    int i = 0;

    while (s[i])
    {
        my_putchar(s[i]);
        i++;
    }
    return i;
}
void freeStringArray(char **stringArray)
{
    if (stringArray == NULL)
    {
        return;
    }

    int count = 0;
    while (stringArray[count] != NULL)
    {
        count++;
    }

    for (int i = 0; i < count; i++)
    {
        if (stringArray[i] != NULL)
        {
            free(stringArray[i]);
            stringArray[i] = NULL;
        }
    }

    free(stringArray);
    stringArray = NULL;
}
char **reAlloc(char **myArray)
{
    int count = 0;
    while (myArray[count] != NULL)
    {
        count++;
    }

    char **newArray = malloc(sizeof(char *) * (count + 1));

    for (int i = 0; myArray[i] != NULL; i++)
    {
        newArray[i] = malloc(sizeof(char) * (strlen(myArray[i]) + 1));
        strcpy(newArray[i], myArray[i]);
    }

    newArray[count] = NULL;
    freeStringArray(myArray);

    return newArray;
}

char **mySplit(char *input, char delimiter)
{
    int index = 0;
    char **stringArray = malloc(sizeof(char *) * 100); // allocate space for 10 pointers to char
    int wordCount = 0;
    int len = strlen(input);

    for (int i = 0; i < len; i++)
    {
        if (index == 0)
        {
            stringArray[wordCount] = malloc(sizeof(char) * len); // allocate space for a string of length 'len'
        }

        if (input[i] != delimiter)
        {
            stringArray[wordCount][index] = input[i];
            index++;
        }
        else
        {
            stringArray[wordCount][index] = '\0';
            wordCount++;
            index = 0;
        }
    }

    stringArray[wordCount][index] = '\0';
    wordCount++;
    stringArray[wordCount] = NULL;

    char **res = reAlloc(stringArray);

    return res;
}

char *getFirstWord(char *input)
{

    int i = 0;
    char *command = (char *)malloc(sizeof(char) * 20);

    while (input[i] != ' ' || input[i] != '\0')
    {
        command[i] = input[i];
        i++;
    }
    command[i] = '\0';

    return command;
}
char *removeBackN(char *line)
{

    for (int i = 0; line[i]; i++)
    {
        if (line[i] == '\n')
        {
            line[i] = '\0';
        }
    }
    return line;
}
char *removeSpace(char *input)
{
    int i = strlen(input);

    if (input[i - 2] == ' ')
    {
        input[i - 2] = '\0';
    }
    return input;
}

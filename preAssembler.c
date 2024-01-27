#include "genericH.h"

typedef struct
{
    char name[MAX_LABEL_SIZE]; /* array of defines*/
    int value;                 /*define's value*/
    int defined;               /*the flag indicate if the define is already exists*/
} Define;

Define defines[MAX_DEFINES] = {0};

int preAssembel(char *fileName)
{
    /*var decleration section*/
    char fileNameAs[MAX_LABEL_SIZE];
    char fileNameAm[MAX_LABEL_SIZE];
    FILE *asFile;
    FILE *amFile;
    char line[MAX_LINE_SIZE] = {0};
    int lineCounter;
    int isDefineLine = 0;
    int defineCounter = 0;

    /*------------------------*/
    lineCounter = 1;

    /* Open the src file*/
    strcpy(fileNameAs, fileName);
    strcat(fileNameAs, ".as");
    asFile = fopen(fileNameAs, "r");
    if (asFile == NULL)
    {
        printf("Failed to open the file \"%s\".\n", fileName);
        return TRUE;
    }
    /* Open the dst file*/
    strcpy(fileNameAm, fileName);
    strcat(fileNameAm, ".am");
    amFile = fopen(fileNameAm, "w");
    if (amFile == NULL)
    {
        printf("Failed to create new AM file \"%s\".\n", fileName);
        return TRUE;
    }

    while (fgets(line, sizeof(line), asFile))
    {
        if (removeCommentsAndSpaces(line)) /*if comments return 1, or replace line with single space only*/
        {
            continue;
        }
        printf("i know who am i ! %d\n", defineCounter);
        isDefineLine = replaceDefines(line, lineCounter, defineCounter);
        printf("who am i ?! %d\n", defineCounter);
        if (isDefineLine == 1)
        { /*if define return 1 its an error, if returns 0 = skip define line*/
            return TRUE;
        }
        else if (isDefineLine == 0)
        {
            memset(line, 0, sizeof(line));
            continue;
        }
        fprintf(amFile, "%s", line);
        memset(line, 0, sizeof(line));
        lineCounter++;
    }
    // if (processMacro(amFile))
    // {
    //     return;
    // }

    fclose(asFile);
    fclose(amFile);
}

// int processMacro(FILE *inputfile)
// // int processMacro(char *input[], int *lineCount)
// {
//     int i;
//     int j;
//     char *macroName = NULL;
//     char *macroContents[MAX_LINES];
//     int macroLineCount = 0;
//     for (i = 0; i < *lineCount; i++)
//     {
//         if (strncmp(input[i], "mcr", 3) == 0)
//         {
//             macroName = strtok(input[i] + 4, " ");
//             i++; /*Skip the mcr line*/
//             while (strncmp(input[i], "endmcr", 6) != 0)
//             {
//                 macroContents[macroLineCount++] = input[i++];
//             }
//         }
//         else if (macroName != NULL && strncmp(input[i], macroName, strlen(macroName)) == 0)
//         {
//             /*Replace macro invocation with macroContents*/
//             for (j = 0; j < macroLineCount; j++)
//             {
//                 input[*lineCount + j] = macroContents[j];
//             }
//             *lineCount += macroLineCount;
//             macroName = NULL; /*Reset macroName after processing*/
//         }
//     }
// }

int getDefineIndex(char *name, int defineCounter)
{
    int i;
    for (i = 0; i < defineCounter; i++)
    {
        if (strcmp(defines[i].name, name) == 0 && defines[i].defined)
        {
            return i;
        }
    }
    return -1;
}
int replaceDefines(char *text, int lineNum, int defineCounter)
{
    int length = strlen(text);
    char name[MAX_LABEL_SIZE];
    char textNum[MAX_LINE_SIZE];
    int i = 0;
    int p = 0;
    int j = 0;
    int defineIndex = 0;
    char tempName[MAX_LABEL_SIZE];
    char tempNum[MAX_LINE_SIZE];
    int hasSpace = FALSE;

    while (text[i] != '\0')
    {
        if (i == 0 && length > 8 && strncmp(&text[i], ".define", 7) == 0 && text[i + 7] == ' ')
        { /*legit define*/
            i += 8;
            while (text[i] != '\0' && text[i] != ' ' && text[i] != '=')
            {
                if (!isalpha(text[i]) && text[i] > 96)
                {
                    printf("ERROR: In line %d - Define '%s' has no valid define label.\n", lineNum, text);
                    return TRUE;
                }
                name[j] = text[i];
                i++;
                j++;
            }
            if (text[i] == '\0')
            {
                printf("ERROR: In line %d - Define '%s' has no valid define label.\n", lineNum, text);
                return TRUE;
            }
            if (text[i] == ' ')
            {
                i++;
            }
            if (text[i] == '=')
            {
                i++;
                if (text[i] == ' ')
                {
                    i++;
                }
                while (text[i] != '\0' && text[i] != '\n')
                {
                    textNum[p] = text[i];
                    p++;
                    i++;
                }
                textNum[p] = '\0';
                if (savedWord(textNum))
                {
                    printf("ERROR: In line %d - Define '%s' cannot be a saved word.\n", lineNum, text);
                    return TRUE;
                }
                if (!isOnlyDigit(textNum))
                {
                    printf("ERROR: In line %d - Define '%s' value is not a number.\n", lineNum, text);
                    return TRUE;
                }
                if (getDefineIndex(name, defineCounter) != NA)
                {
                    printf("ERROR: In line %d - Define '%s' already exists.\n", lineNum, text);
                    return TRUE;
                }
                else
                {
                    strcpy(defines[defineCounter].name, name);
                    defines[defineCounter].value = atoi(textNum);
                    defines[defineCounter].defined = TRUE;
                    printf("here! %d\n", defineCounter);
                    defineCounter = defineCounter + 1;
                    printf("here! %d\n", defineCounter);
                    return FALSE;
                }
            }
            else
            {
                printf("ERROR: In line %d - Define decleration '%s' does not contain number.\n", lineNum, text);
                return TRUE;
            }
        }
        else
        {
            while (text[i] != '\0')
            {
                if (text[i] == ' ' ||
                    text[i] == '#')
                {
                    hasSpace = TRUE;
                }
                while (isalpha(text[i]))
                {
                    tempName[j] = text[i];
                    i++;
                    j++;
                }
                if (text[i] == ' ' ||
                    (text[i] == ']' && text[i - j - 1] == '['))
                {
                    hasSpace = TRUE;
                }
                tempName[j] = '\0';
                defineIndex = getDefineIndex(tempName, defineCounter);
                if (defineIndex != NA && hasSpace)
                {
                    sprintf(tempNum, "%d", defines[defineIndex].value);
                    strncat(tempNum, &text[i], strlen(&text[i] - 1));
                    memmove(&text[i - strlen(tempName)], &tempNum, strlen(tempNum) + 1);
                }
                j = 0;
                i++;
                hasSpace = FALSE;
            }
        }
        i++;
    }
    return 2;
}
int removeCommentsAndSpaces(char *line)
{
    char newLine[MAX_LINE_SIZE] = {'\0'};
    if (line[0] == ';')
    {
        return TRUE;
    }
    clearDuplicateBlankChars(line, newLine);
    memset(line, 0, MAX_LINE_SIZE);
    strcpy(line, newLine);
    return FALSE;
}
int savedWord(char *textNum)
{
    if (strcmp(textNum, "mcr") == 0 ||
        strcmp(textNum, "endmcr") == 0 ||
        strcmp(textNum, "mov") == 0 ||
        strcmp(textNum, "cmp") == 0 ||
        strcmp(textNum, "add") == 0 ||
        strcmp(textNum, "sub") == 0 ||
        strcmp(textNum, "lea") == 0 ||
        strcmp(textNum, "clr") == 0 ||
        strcmp(textNum, "not") == 0 ||
        strcmp(textNum, "inc") == 0 ||
        strcmp(textNum, "dec") == 0 ||
        strcmp(textNum, "jmp") == 0 ||
        strcmp(textNum, "bne") == 0 ||
        strcmp(textNum, "red") == 0 ||
        strcmp(textNum, "prn") == 0 ||
        strcmp(textNum, "jsr") == 0 ||
        strcmp(textNum, "rts") == 0 ||
        strcmp(textNum, "hlt") == 0 ||
        strcmp(textNum, "r0") == 0 ||
        strcmp(textNum, "r1") == 0 ||
        strcmp(textNum, "r2") == 0 ||
        strcmp(textNum, "r3") == 0 ||
        strcmp(textNum, "r4") == 0 ||
        strcmp(textNum, "r5") == 0 ||
        strcmp(textNum, "r6") == 0 ||
        strcmp(textNum, "r7") == 0 ||
        strcmp(textNum, "data") == 0 ||
        strcmp(textNum, "string") == 0 ||
        strcmp(textNum, "entry") == 0 ||
        strcmp(textNum, "extern") == 0)
    {
        return TRUE;
    }
    return FALSE;
}
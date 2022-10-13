/* Assembler code fragment */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <map>
#include <string>
#include <bitset>

using namespace std;

#define MAXLINELENGTH 1000

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int binOpcode(char *);

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    map<string, int> symboricAddress; // key = label, value = address

    if (argc != 3)
    {
        printf("error: usage: %s <assembly-code-file> <machine-code-file>\n",
               argv[0]);
        exit(1);
    }

    inFileString = argv[1];
    outFileString = argv[2];

    inFilePtr = fopen(inFileString, "r");
    if (inFilePtr == NULL)
    {
        printf("error in opening %s\n", inFileString);
        exit(1);
    }
    outFilePtr = fopen(outFileString, "w");
    if (outFilePtr == NULL)
    {
        printf("error in opening %s\n", outFileString);
        exit(1);
    }

    /* read instructions from file */
    for (int i = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); i++)
    {
        /* show address and instruction */
        printf("%d. %s %s %s %s %s\n", i, label, opcode, arg0, arg1, arg2);

        /* have label? store label and symboric address */
        if (strcmp(label, ""))
            symboricAddress.insert({label, i});
    }

    /* check symboric address */
    for (auto itr = symboricAddress.begin(); itr != symboricAddress.end(); ++itr)
    {
        cout << itr->first << " " << itr->second << endl;
    }

    /* this is how to rewind the file ptr so that you start reading from the
        beginning of the file */
    rewind(inFilePtr);

    for (int i = 0; readAndParse(inFilePtr, label, opcode, arg0, arg1, arg2); i++)
    {
        /* show address and instruction */
        printf("%d. %s %s %s %s %s\n", i, label, opcode, arg0, arg1, arg2);

        cout << bitset<3>(binOpcode(opcode)) << endl;
    }

    /* after doing a readAndParse, you may want to do the following to test the
        opcode */
    if (!strcmp(opcode, "add"))
    {
        /* do whatever you need to do for opcode "add" */
    }

    return (0);
}

/*
 * Read and parse a line of the assembly-language file.  Fields are returned
 * in label, opcode, arg0, arg1, arg2 (these strings must have memory already
 * allocated to them).
 *
 * Return values:
 *     0 if reached end of file
 *     1 if all went well
 *
 * exit(1) if line is too long.
 */
int readAndParse(FILE *inFilePtr, char *label, char *opcode, char *arg0,
                 char *arg1, char *arg2)
{
    char line[MAXLINELENGTH];
    char *ptr = line;

    /* delete prior values */
    label[0] = opcode[0] = arg0[0] = arg1[0] = arg2[0] = '\0';

    /* read the line from the assembly-language file */
    if (fgets(line, MAXLINELENGTH, inFilePtr) == NULL)
    {
        /* reached end of file */
        return (0);
    }

    /* check for line too long (by looking for a \n) */
    if (strchr(line, '\n') == NULL)
    {
        /* line too long */
        printf("error: line too long\n");
        exit(1);
    }

    /* is there a label? */
    ptr = line;
    if (sscanf(ptr, "%[^\t\n ]", label))
    {
        /* successfully read label; advance pointer over the label */
        ptr += strlen(label);
    }

    /*
     * Parse the rest of the line.  Would be nice to have real regular
     * expressions, but scanf will suffice.
     */
    sscanf(ptr, "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
           opcode, arg0, arg1, arg2);
    return (1);
}

int isNumber(char *string)
{
    /* return 1 if string is a number */
    int i;
    return ((sscanf(string, "%d", &i)) == 1);
}

/* convert string opcode to integer opcode */
int binOpcode(char *opcode)
{
    if (!strcmp(opcode, "add"))
    {
        return 0b000;
    }

    if (!strcmp(opcode, "nand"))
    {
        return 0b001;
    }

    if (!strcmp(opcode, "lw"))
    {
        return 0b010;
    }

    if (!strcmp(opcode, "sw"))
    {
        return 0b011;
    }

    if (!strcmp(opcode, "beq"))
    {
        return 0b100;
    }

    if (!strcmp(opcode, "jalr"))
    {
        return 0b101;
    }

    if (!strcmp(opcode, "halt"))
    {
        return 0b110;
    }

    if (!strcmp(opcode, "noop"))
    {
        return 0b111;
    }

    return -1;
}
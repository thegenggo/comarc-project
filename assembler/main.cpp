#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <map>
#include <bitset>
#include <iostream>

using namespace std;

#define MAXLINELENGTH 1000
#define MAXADDRESS 65536

class Instruction
{
private:
    int address;
    string label;
    string instruction;
    string field[3];

public:
    Instruction(int address, string label, string instruction, string field0, string field1, string field2)
    {
        this->address = address;
        this->label = label;
        this->instruction = instruction;
        this->field[0] = field0;
        this->field[1] = field1;
        this->field[2] = field2;
    }

    int getAddress()
    {
        return address;
    }

    string getInstruction()
    {
        return instruction;
    }

    string getField(int n)
    {
        return field[n];
    }
};

int readAndParse(FILE *, char *, char *, char *, char *, char *);
int isNumber(char *);
int convertOpcodeToInterger(char *);
int generateMachineCode(Instruction *, Instruction **, map<string, int>);
int calculateOffsetField(string, Instruction *, Instruction **, map<string, int>);
int calculateField(int, Instruction *, Instruction **, map<string, int>);

int main(int argc, char *argv[])
{
    char *inFileString, *outFileString;
    FILE *inFilePtr, *outFilePtr;
    char label[MAXLINELENGTH], opcode[MAXLINELENGTH], arg0[MAXLINELENGTH],
        arg1[MAXLINELENGTH], arg2[MAXLINELENGTH];

    map<string, int> symboricAddress;      // key = label, value = address
    Instruction *instructions[MAXADDRESS]; // all program instructions

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
        // printf("%d. %s %s %s %s %s\n", i, label, opcode, arg0, arg1, arg2);

        /* have label? store label and symboric address */
        if (strcmp(label, ""))
        {
            /*  true if new element inserted
                false if key already exist */
            if (!symboricAddress.insert({label, i}).second)
            {
                printf("error: label already exist");
                exit(1);
            }
        }

        /* store instructions for use later */
        instructions[i] = new Instruction(i, label, opcode, arg0, arg1, arg2);
    }

    /* check symboric address */
    for (auto itr = symboricAddress.begin(); itr != symboricAddress.end(); ++itr)
    {
        // cout << itr->first << " " << itr->second << endl;
    }

    for (auto instruction : instructions)
    {
        if (!instruction)
            break; // end of instructions
        int machineCode = generateMachineCode(instruction, instructions, symboricAddress);
        fprintf(outFilePtr, "%d\n", machineCode);
    }

    fclose(inFilePtr);
    fclose(outFilePtr);

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

int isNumber(const char *string)
{
    /* return 1 if string is a number */
    int i;
    return ((sscanf(string, "%d", &i)) == 1);
}

/*
 * Convert string opcode to integer opcode
 *
 * @parameter opcode -> opcode in type string
 * @return opcode in type integer (binary) or -1 if can't define opcode
 */
int convertOpcodeToInterger(const char *opcode)
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

    if (!strcmp(opcode, ".fill"))
    {
        return 0b000;
    }

    return -1;
}

/*
 * Generate machine code from given instruction.
 *
 * @parameter intruction -> instruction that is in processing
 * @parameter intructions -> all instructions in program
 * @parameter symboricAddress -> mapping between label and address
 * @return machine code from instruction
 * @exit(1) if opcode is undefined
 */
int generateMachineCode(Instruction *instruction, Instruction **instructions, map<string, int> symboricAddress)
{
    const char *opcode = instruction->getInstruction().c_str();
    int binOpcode = convertOpcodeToInterger(opcode);
    if (binOpcode == -1)
    {
        printf("error: can not define opcode");
        exit(1);
    }

    int result = 0;
    result = result | binOpcode << 22; // bits 24-22 = opcode

    /* R-type instructions (add, nand) */
    if (!strcmp(opcode, "add") || !strcmp(opcode, "nand"))
    {
        int regA = calculateField(0, instruction, instructions, symboricAddress);
        int regB = calculateField(1, instruction, instructions, symboricAddress);
        int destReg = calculateField(2, instruction, instructions, symboricAddress);

        // cout << regA << " " << regB << " " << destReg << endl;

        result = result | regA << 19; // bits 21-19 = regA (rs)
        result = result | regB << 16; // bits 18-16 = regB (rt)
        result = result | destReg;    // bits 2-0 = destReg (rd)
    }

    /* I-type instructions (lw, sw, beq) */
    if (!strcmp(opcode, "lw") || !strcmp(opcode, "sw") || !strcmp(opcode, "beq"))
    {
        int regA = calculateField(0, instruction, instructions, symboricAddress);
        int regB = calculateField(1, instruction, instructions, symboricAddress);
        int offsetField = calculateField(2, instruction, instructions, symboricAddress);

        // cout << "offsetField: " << bitset<16>(offsetField) << "(" << offsetField << ")" << endl;

        if (-32768 > offsetField || offsetField > 32767)
        {
            printf("error: offsetField is greater than 16 bits");
            exit(1);
        }

        result = result | regA << 19;                         // bits 21-19 = regA (rs)
        result = result | regB << 16;                         // bits 18-16 = regB (rt)
        result = result | (offsetField & 0b1111111111111111); // bits 15-0 = offsetField (2's complement)
    }

    /* J-Type instructions (jalr) */
    if (!strcmp(opcode, "jalr"))
    {
        int regA = calculateField(0, instruction, instructions, symboricAddress);
        int regB = calculateField(1, instruction, instructions, symboricAddress);

        result = result | regA << 19; // bits 21-19 = regA (rs)
        result = result | regB << 16; // bits 18-16 = regB (rd)
    }

    /* O-type instructions (halt, noop) */
    if (!strcmp(opcode, "halt") || !strcmp(opcode, "noop"))
    {
        return result;
    }

    /* fill instruction */
    if (!strcmp(opcode, ".fill"))
    {
        result = calculateField(0, instruction, instructions, symboricAddress);
    }

    return result;
}

/*
 * Calculate value of the field.
 *
 * @parameter n -> field position need to calculate
 * @parameter instruction -> instruction that is in processing
 * @parameter intructions -> all instructions in program
 * @parameter symboricAddress -> mapping between label and address
 * @return value of the field in type integer (binary)
 */
int calculateField(int n, Instruction *instruction, Instruction **instructions, map<string, int> symboricAddress)
{
    string arg[3];
    arg[0] = instruction->getField(0), arg[1] = instruction->getField(1), arg[2] = instruction->getField(2);

    return isNumber(arg[n].c_str()) ? stoi(arg[n]) : calculateOffsetField(arg[n], instruction, instructions, symboricAddress);
}

/*
 * Calculate offsetfield from label.
 *
 * @parameter label -> label name that related to other instruction
 * @parameter instruction -> instruction that is in processing
 * @parameter intructions -> all instructions in program
 * @parameter symboricAddress -> mapping between label and address
 * @return value of the field in type integer (binary)
 */
int calculateOffsetField(string label, Instruction *instruction, Instruction **instructions, map<string, int> symboricAddress)
{
    int address = symboricAddress[label];
    if (!address)
    {
        printf("error: undefine label");
        exit(1);
    }

    Instruction *labeledInstruction = *(instructions + address);

    /* beq instruction address calculate */
    if (!instruction->getInstruction().compare("beq"))
    {
        return labeledInstruction->getAddress() - instruction->getAddress() - 1;
    }

    return labeledInstruction->getAddress();
}
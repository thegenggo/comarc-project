#include <stdio.h>
#include <string.h>
#include <bitset>
#include <iostream>
using namespace std;

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8       /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct
{
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);

int main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    int insCount = 0;
    stateType state;
    FILE *filePtr;

    if (argc != 2)
    {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        return (1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL)
    {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        return (1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
         state.numMemory++)
    {
        if (sscanf(line, "%d", state.mem + state.numMemory) != 1)
        {
            printf("error in reading address %d\n", state.numMemory);
            return (1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }

    for (int i = 0; i < NUMREGS; i++)
    {
        state.reg[i] = 0;
    }
    while(true)
    {
        int opcode = (state.mem[state.pc] >> 22) & 0b111;
        int regA = (state.mem[state.pc] >> 19) & 0b111;
        int regB = (state.mem[state.pc] >> 16) & 0b111;
        insCount++;

        printState(&state);
        // Add
        if (opcode == 0b000)
        {
            int destReg = state.mem[state.pc] & 0b111;
            state.reg[destReg] = state.reg[regA] + state.reg[regB];
        }
        // nand
        if (opcode == 0b001)
        {
            int destReg = state.mem[state.pc] & 0b111;
            state.reg[destReg] = ~(state.reg[regA] & state.reg[regB]);
        }
        // lw
        if (opcode == 0b010)
        {
            int offsetField = state.mem[state.pc] & 0xffff;
            if (((state.mem[state.pc] >> 15) & 0b1) == 1)
            {
                offsetField = offsetField | 0xffff0000;
            }
            state.reg[regB] = state.mem[state.reg[regA] + offsetField];
        }
        // sw
        if (opcode == 0b011)
        {
            int offsetField = state.mem[state.pc] & 0xffff;
            if ((state.mem[state.pc] >> 15) & 0b1 == 1)
            {
                offsetField = offsetField | 0xffff0000;
            }
            state.mem[state.reg[regA] + offsetField] = state.reg[regB];
        }
        // beq
        if (opcode == 0b100)
        {
            int offsetField = state.mem[state.pc] & 0xffff;
            if ((state.mem[state.pc] >> 15) & 0b1 == 1)
            {
                offsetField = offsetField | 0xffff0000;
            }
            if (state.reg[regA] == state.reg[regB])
            {
                state.pc = state.pc + offsetField;
            }
        } 
        // jalr
        if (opcode == 0b101)
        {
            state.reg[regB] = state.pc + 1;
            state.pc = state.reg[regA];
            continue;
        }
        //halt
        if (opcode == 0b110)
        {
            state.pc++;
            break;
        }
        //noop
        if (opcode == 0b111)
        {

        }

        state.pc++;
    }
    cout << "machine halted" << endl;
    cout << "total of " << insCount << " instructions executed" << endl;
    cout << "final state of machine:"  << endl;
    printState(&state);


    return (0);
}

void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++)
    {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++)
    {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}
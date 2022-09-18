#include <iostream>
#include <fstream>
#include <string>

#define MAXLINELENGTH 1000

using namespace std;

int readAndParse(string, string &, string &, string &, string &, string &);

int main(int argc, char *argv[])
{
    string myText;
    ofstream outputFile("output.txt");
    ifstream inputFile("input.txt");
    while (getline(inputFile, myText))
    {
        cout << myText << endl;
        string label, opcode, arg0, arg1, arg2;
        readAndParse(myText, label, opcode, arg0, arg1, arg2);
    }
    return 0;
}

int readAndParse(string instruction, string &label, string &opcode, string &arg0,
                 string &arg1, string &arg2)
{
    char labelChar[MAXLINELENGTH], opcodeChar[MAXLINELENGTH], arg0Char[MAXLINELENGTH],
        arg1Char[MAXLINELENGTH], arg2Char[MAXLINELENGTH];

    if (sscanf(instruction.c_str(), "%[^\t\n ]", labelChar))
    {
        label = labelChar;
    }

    sscanf(instruction.c_str() + label.length(), "%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]%*[\t\n ]%[^\t\n ]",
        opcodeChar, arg0Char, arg1Char, arg2Char);

    opcode = opcodeChar;
    arg0 = arg0Char;
    arg1 = arg1Char;
    arg2 = arg2Char;
    
    return 0;
}
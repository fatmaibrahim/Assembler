#include <iostream>
#include <regex>
#include <fstream>
#include <vector>


using namespace std;
vector<string> symbolicNames;
vector<string> jumpOperands;
string startLabel = "";
vector<int> repeatedSymbolicnames;

string capitalizeWord(string word) {
    string capitalLetters = "";
    for (int index = 0; index < word.length(); index++) {
        capitalLetters += toupper(word[index]);
    }
    return capitalLetters;
}

bool checkRepeatedSymbolicnames(string symbolicname) {
    for (int index = 0; index < symbolicNames.size(); index++) {
        if (symbolicname == symbolicNames[index]) {
            return true;
        }
    }
    return false;
}

void findSymbolicNames(vector<string> lineParts, int lineIndex) {
    string label = lineParts[0];
    string opCode = lineParts[1];
    string operand = lineParts[2];

    if (opCode == "BYTE" || opCode == "WORD" || opCode == "RESB" || opCode == "RESW") {
        if (checkRepeatedSymbolicnames(label)) {
            repeatedSymbolicnames.push_back(lineIndex);
            //cout << "Error repeated symbolic name : " << label << endl;
        } else {
            symbolicNames.push_back(label);
            cout << "symbole name : " << label << endl;
        }
    }
    if (opCode == "J" || opCode == "JEQ" || opCode == "JLT" || opCode == "JGT" || opCode == "JSUB") {
        jumpOperands.push_back(operand);
        cout << "jump operand : " << operand << endl;
    }

}

bool checkLabelCorrectness(string opCode, string label, int index) {
    regex startOfOperand;
    bool check = true;
    bool flag = false;
    if (opCode == "RMO" || opCode == "LDCH" || opCode == "STCH" || opCode == "ADD" || opCode == "SUB" ||
        opCode == "MUL" || opCode == "DIV" || opCode == "ADDR" || opCode == "SUBR" || opCode == "MULR" ||
        opCode == "DIVR" || opCode == "COMP" || opCode == "COMPR" || opCode == "J" || opCode == "JEQ" ||
        opCode == "JLT" || opCode == "JGT" || opCode == "TIX" || opCode == "TIXR" || opCode == "JSUB" ||
        opCode == "RSUB" || opCode == "TD" || opCode == "RD" || opCode == "WD" ||
        regex_match(opCode, startOfOperand.assign("(ST)([AXLBSTF])")) ||
        regex_match(opCode, startOfOperand.assign("(LD)([AXLBSTF])"))) {

        for (int index = 0; index < symbolicNames.size(); index++) {
            if (label == symbolicNames[index]) {
                flag = true;
                break;
            }
        }
        for (int index = 0; index < jumpOperands.size(); index++) {
            if (label == jumpOperands[index]) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            if (label != "") {
                check = false;
            }
        }

    } else if (opCode == "BYTE" || opCode == "RESB" || opCode == "RESW" || opCode == "WORD") {

    } else if (opCode == "START") {
        startLabel = label;
    } else if (opCode == "END" || opCode == "ORG" || opCode == "BASE" || opCode == "NOBASE" || opCode == "LTORG") {
        if (label != "") {
            check = false;
        }
    } else {
        cout << "     ***Error in opCode :" << opCode << "in line :" << index << " isn't defined in the language"
             << endl;
    }
    return check;
}

bool checkOperandCorrectness(string opCode, string operand) {
    bool check = true;
    regex startOfOperand;
    if (opCode == "RMO" || opCode == "ADDR" || opCode == "SUBR" || opCode == "MULR" ||
        opCode == "DIVR" || opCode == "COMPR") {
        //take two registers


        if (!regex_match(operand, startOfOperand.assign("([AXLBSTF])(/,)([AXLBSTF])"))) {
            check = false;
        }

    } else if (regex_match(opCode, startOfOperand.assign("(ST)([AXLBSTF])")) ||
               regex_match(opCode, startOfOperand.assign("(LD)([AXLBSTF])")) || opCode == "ADD" ||
               opCode == "SUB" || opCode == "MUL" || opCode == "DIV" || opCode == "COMP" || opCode == "TIX" ||
               opCode == "TD" || opCode == "RD" || opCode == "WD") {
        bool flag = false;
        for (int index = 0; index < symbolicNames.size(); index++) {
            if (operand == symbolicNames[index]) {
                flag = true;
                break;
            }
        }
        if (!flag) {
            regex startOfOperand;
            if (operand.length() < 5) {
                if (!regex_match(operand, startOfOperand.assign("(#)/d*")) ||
                    !regex_match(operand, startOfOperand.assign("(@)/d*"))) {

                    check = false;
                }
            }
        }
    } else if (opCode == "LDCH" || opCode == "STCH") {

        bool flag = false;
        string subOperand = "";

        if (operand[operand.length() - 2] == ',' && operand[operand.length() - 1] == 'X') {
            subOperand += operand.substr(0, operand.length() - 2);
        } else {
            subOperand += operand;
        }
        for (int index = 0; index < symbolicNames.size(); index++) {
            if (subOperand == symbolicNames[index]) {
                flag = true;
                break;
            }
        }

        if (!flag) {
            check = false;
        }
    } else if (opCode == "J" || opCode == "JEQ" ||
               opCode == "JLT" || opCode == "JGT" || opCode == "JSUB") {
        if (operand[0] == ' ') {
            check = false;
        }
    } else if (opCode == "TIXR") {
        if (!regex_match(operand, startOfOperand.assign("([AXLBSTF])"))) {
            check = false;
        }
    } else if (opCode == "RSUB" || opCode == "NOBASE" || opCode == "LTORG") {
        if (operand != "                  ") {
            check = false;
        }
    } else if (opCode == "BYTE") {
        regex startOfOpCode;
        if (!regex_match(operand, startOfOpCode.assign("(C')[A-Z]/'")) ||
            !regex_match(operand, startOfOpCode.assign("(X')[A-Z1-9]/'"))) {
            check = false;
        }
    } else if (opCode == "WORD") {
        regex startOfOpCode;
        if (operand.length() <= 4) {
            if (!regex_match(operand, startOfOpCode.assign("/d*"))) {
                if (operand != "                  ") {
                    check = false;
                }
            }
        } else {
            bool flag = false;
            for (int index = 0; index < symbolicNames.size(); index++) {
                if (operand == symbolicNames[index]) {
                    flag = true;
                }
            }
            if (!flag) {
                check = false;
            }
        }

    } else if (opCode == "RESW" || opCode == "RESB") {
        regex startOfOpCode;
        if (operand.length() <= 4) {
            if (!regex_match(operand, startOfOpCode.assign("/d*"))) {
                check = false;
            }
        }
    } else if (opCode == "START") {
        regex startOfOpCode;
        if (operand.length() <= 4) {
            if (!regex_match(operand, startOfOpCode.assign("/d*"))) {
                if (operand != "                  ") {
                    check = false;
                }
            }
        }
    } else if (opCode == "END") {
        if (operand != startLabel) {
            if (operand != "") {
                check = false;
            }
        }
    } else if (opCode == "ORG") {
        bool flag = false;
        for (int index = 0; index < symbolicNames.size(); index++) {
            if (operand == symbolicNames[index]) {
                flag = true;
            }
        }
        if (!flag) {
            check = false;
        }

    } else if (opCode == "EQU" || opCode == "BASE") {
        bool flag = false;
        for (int index = 0; index < symbolicNames.size(); index++) {
            if (operand == symbolicNames[index]) {
                flag = true;
            }
        }
        if (!flag) {
            regex startOfOpCode;
            if (operand.length() <= 4) {
                if (!regex_match(operand, startOfOpCode.assign("/d*"))) {
                    check = false;
                }
            }
        }
    }
}

string parse(vector<string> lineParts, int index) {
    string label = lineParts[0];
    string opCode = lineParts[1];
    string operand = lineParts[2];

    if (!checkLabelCorrectness(opCode, label, index)) {
        return "Error in label";
    }
    if (checkOperandCorrectness(opCode, operand)) {
        return "Error in operand ";
    }
    return "correct";

}

bool spacesCheck(string line) {
    bool isCorrect = false;
    regex str;


    if (line[line.length() - 1] == ' ') {//check for the spaces at the end of the line
        isCorrect = false;
    }
//check the start
    if ((line[0] >= 'a' && line[0] <= 'z') || (line[0] >= 'A' && line[0] <= 'Z')) {
        isCorrect = true;
    } else if (line.substr(0, 7) == "       ") {
        isCorrect = true;
    } else {
        isCorrect = false;
        //error in line format
    }
//check the opCode
    if (line[9] >= 'A' && line[9] <= 'Z') {
        isCorrect = true;
    } else {
        isCorrect = false;
    }
    //check the operand
    if (line.length() >= 17) {
        if ((line[17] >= 'A' && line[17] <= 'Z') || line[17] == '#' || (line[17] >= '0' && line[17] <= '9')) {
            isCorrect = true;
        } else {
            isCorrect = false;
        }
    }
//check the comments
    if (line.length() >= 35) {
        if (line[35] == '.') {
            isCorrect = true;
        } else {
            isCorrect = false;
        }
    }

    return isCorrect;
}

vector<string> spliter(string line) {
    vector<string> lineParts;

    if (spacesCheck(line)) {
        string opCode = "";
        string label = "";
        string operand = "";

        for (int index = 0; index < line.length(); index++) {
            if (index < 8 && line[index] != ' ') {
                label += line[index];
            } else if (index > 8 && index < 15 && line[index] != ' ') {
                opCode += line[index];
            } else if (index > 16 && index < 35 && line[index] != ' ') {
                operand += line[index];
            }
        }
        lineParts.push_back(capitalizeWord(label));
        lineParts.push_back(capitalizeWord(opCode));
        lineParts.push_back(capitalizeWord(operand));
    }
    return lineParts;
}


bool commentsCheck(string line) {
    bool check = false;
    if (line[0] == '.') {
        check = true;
    }
    return check;
}

void readFromFileAndPrintTheFile(string fileName) {
    string line = "";
    ifstream file(fileName);
    vector<string> temp;
    int lineIndex = 0;
    string check;
    bool startExistance = true;
    bool endExistance = false;
    //now file is opened
    if (file.is_open()) {
        //check the file is opened or not
        while (getline(file, line)) {//print line by line
            lineIndex++;

            if (!commentsCheck(line)) {
                temp = spliter(line);
                if (temp.empty()) {
                    cout << line << endl;
                    cout << "      ***Error in indentation" << endl;

                } else {
                    check = parse(temp, lineIndex);
                    if (startExistance) {
                        startExistance = false;
                        if (startLabel == "") {
                            cout << "      ***Error in opCode START" << endl;

                        }
                    }
                    if (temp[1] == "END") {
                        endExistance = true;
                    }
                    if (check == "correct") {
                        bool flag = true;
                        for (int index = 0; index < repeatedSymbolicnames.size(); index++) {
                            if (lineIndex == repeatedSymbolicnames[index]) {
                                flag = false;
                                cout << line << endl;
                                cout << "     ***Error repeated symbolic name " << endl;
                            }
                        }
                        if (flag) {
                            cout << line << endl;
                        }
                        flag = true;
                    } else if (check == "Error in operand ") {
                        cout << line << endl;
                        cout <<\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\ "     ***Error in operand " << endl;

                    } else if (check == "Error in label") {
                        cout << line << endl;
                        cout << "     ***Error in label" << endl;
                    }
                }
            } else {
                cout << line << endl;
            }
        }
        if (!endExistance) {
            cout << "Error in opCode END" << endl;
        }
    } else {
        cout << "file doesn't open" << endl;
    }
    file.close();
}

void readFromFileAndExtractSymboleTable(string fileName) {
    string line = "";
    ifstream file(fileName);
    //now file is opened
    int lineIndex = 0;
    if (file.is_open()) {
        //check the file is opened or not
        while (getline(file, line)) {//read line by line
            lineIndex++;
            if (!commentsCheck(line)) {
                vector<string> temp;
                temp = (spliter(line));
                if (!temp.empty()) {
                    findSymbolicNames(temp, lineIndex);
                }
            }
        }
    } else {
        cout << "file doesn't open" << endl;
    }
    file.close();

    readFromFileAndPrintTheFile(fileName);
}


int main() {
    string fileName;
    // cout << "Enter the file name : ";
    //cin >> fileName;
    readFromFileAndExtractSymboleTable("test.txt");
    return 0;
}

#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <sstream>
#include <tuple>
#include <cstdlib>
#include <cstdio>
#include <cstdint>

using namespace std;

struct mc{
    int opcode, fun3, fun7, rd, rs1, rs2;
};

enum INSTRUCTION_TYPE {
    R_TYPE, I_TYPE, S_TYPE, SB_TYPE, U_TYPE, UJ_TYPE, IL_TYPE
};

unordered_map<string, INSTRUCTION_TYPE> formats = {
    {"add", R_TYPE},
    {"addw", R_TYPE},
    {"and", R_TYPE},
    {"or", R_TYPE},
    {"sll", R_TYPE},
    {"slt", R_TYPE},
    {"sra", R_TYPE}, 
    {"srl", R_TYPE},
    {"sub", R_TYPE},
    {"subw", R_TYPE}, 
    {"xor", R_TYPE},
    {"mul", R_TYPE},
    {"mulw", R_TYPE},
    {"div", R_TYPE},
    {"divw", R_TYPE},
    {"rem", R_TYPE},
    {"remw", R_TYPE},
    {"addi", I_TYPE}, 
    {"addiw", I_TYPE},
    {"andi", I_TYPE}, 
    {"ori", I_TYPE}, 
    {"lb", IL_TYPE}, 
    {"ld", IL_TYPE}, 
    {"lh", IL_TYPE}, 
    {"lw", IL_TYPE}, 
    {"jalr", I_TYPE},
    {"sb", S_TYPE},
    {"sh", S_TYPE},
    {"sw", S_TYPE},
    {"sd", S_TYPE},
    {"beq", SB_TYPE},
    {"bne", SB_TYPE},
    {"bge", SB_TYPE},
    {"blt", SB_TYPE},
    {"auipc", U_TYPE},
    {"lui", U_TYPE},
    {"jal", UJ_TYPE}
};

unordered_map<string, string> registers = {
    {"x0", "00000"}, {"x1", "00001"}, {"x2", "00010"}, {"x3", "00011"},
    {"x4", "00100"}, {"x5", "00101"}, {"x6", "00110"}, {"x7", "00111"},
    {"x8", "01000"}, {"x9", "01001"}, {"x10", "01010"}, {"x11", "01011"},
    {"x12", "01100"}, {"x13", "01101"}, {"x14", "01110"}, {"x15", "01111"},
    {"x16", "10000"}, {"x17", "10001"}, {"x18", "10010"}, {"x19", "10011"},
    {"x20", "10100"}, {"x21", "10101"}, {"x22", "10110"}, {"x23", "10111"},
    {"x24", "11000"}, {"x25", "11001"}, {"x26", "11010"}, {"x27", "11011"},
    {"x28", "11100"}, {"x29", "11101"}, {"x30", "11110"}, {"x31", "11111"},
    {"sp", "00010"}, {"zero", "00000"}, {"ra", "00001"}, {"gp", "00011"},
    {"tp", "00100"}, {"t0", "00101"}, {"t1", "00110"}, {"t2", "00111"},
    {"s0", "01000"}, {"s1", "01001"}, {"a0", "01010"}, {"a1", "01011"}, {"a2", "01100"},
    {"a3", "01101"}, {"a4", "01110"}, {"a5", "01111"}, {"a6", "10000"}, {"a7", "10001"},
    {"s2", "10010"}, {"s3", "10011"}, {"s4", "10100"}, {"s5", "10101"}, {"s6", "10110"},
    {"s7", "10111"}, {"s8", "11000"}, {"s9", "11001"}, {"s10", "11010"}, {"s11", "11011"},
    {"t3", "11100"}, {"t4", "11101"}, {"t5", "11110"}, {"t6", "11111"}
};

unordered_map<string, tuple<string, string, string>> r_format = {
    {"add", {"0110011", "000", "0000000"}}, {"and", {"0110011", "111", "0000000"}},
    {"or",  {"0110011", "110", "0000000"}}, {"sll", {"0110011", "001", "0000000"}},
    {"slt", {"0110011", "010", "0000000"}}, {"sra", {"0110011", "101", "0100000"}},
    {"srl", {"0110011", "101", "0000000"}}, {"sub", {"0110011", "000", "0100000"}},
    {"xor", {"0110011", "100", "0000000"}}, {"mul", {"0110011", "000", "0000001"}},
    {"div", {"0110011", "100", "0000001"}}, {"rem", {"0110011", "110", "0000001"}}, {"addw", {"0111011", "000", "0000000"}},
    {"subw", {"0111011", "000", "0100000"}},
    {"mulw", {"111011", "000", "0010000"}},
    {"divw", {"111011", "100", "0010000"}},
    {"remw", {"111011", "110", "0010000"}}
};

unordered_map<string, tuple<string, string>> i_format = {
    {"addi", {"0010011", "000"}}, {"andi", {"0010011", "111"}},
    {"ori",  {"0010011", "110"}}, {"lb",   {"0000011", "000"}},
    {"ld",   {"0000011", "011"}}, {"lh",   {"0000011", "001"}},
    {"lw",   {"0000011", "010"}}, {"jalr", {"1100111", "000"}},
    {"addiw", {"0011011", "000"}}
};

unordered_map<string, pair<string, string>> s_format = {
    {"sb", {"0100011", "000"}}, {"sh", {"0100011", "001"}},
    {"sw", {"0100011", "010"}}, {"sd", {"0100011", "011"}}
};

unordered_map<string, pair<string, string>> sb_format = {
    {"beq", {"1100011", "000"}}, {"bne", {"1100011", "001"}},
    {"blt", {"1100011", "100"}}, {"bge", {"1100011", "101"}},
}; 


unordered_map<string, string> u_format = {
    {"auipc", "0010111"}, {"lui", "0110111"}
};

unordered_map<string, string> uj_format = {
    {"jal", "1101111"}
};

unordered_map<string, int> dataSize = {
    {".byte", 1}, {".half", 2} , {".word", 4}, {".dword",8}, {".asciiz",1}
};

unordered_map<string, int> labelTable;

const  int  dataStart = 0x10000000;
const int stackStart =  0x7FFFFFDC;

int intToBinaryChar(char *buffer,const unsigned int size, const int n){
    int arr[size];
    bool isNeg = (n<0);
    int x = (isNeg) ? ((-n) - 1) : n;
    int temp;
    for (int i = 0; i < size; i++){
        temp = x%2;
        if (isNeg){
            if (temp == 0) arr[size-i-1] = 1;
            else arr[size-i-1] = 0;
        }
        else {
            arr[size-i-1] = temp;
        }
        x=x/2;
    }
    for (int i = 0; i < size; i++){
        buffer[i] = (arr[i] == 0) ? '0' : '1';
    }
    buffer[size] = '\0';
    return (isNeg) ? ((-n) - 1) : n;
}

string ltrim(string s) {
    size_t i = 0;
    while (i < s.length() && (isspace(static_cast<unsigned char>(s[i])) || static_cast<unsigned char>(s[i] == '\r'))) i++;
    s.erase(0, i);
    return s;
}

string rtrim(string s) {
    size_t i = s.length();
    while (i > 0 && (isspace(static_cast<unsigned char>(s[i])) || static_cast<unsigned char>(s[i] == '\r'))) i--;
    s.erase(i);
    return s;
}

string trim(string s) {
    return ltrim(rtrim(s));
}

auto loadFile(string fileName){
    vector<string> r;
    ifstream asmFile;
    asmFile.open(fileName);
    if (!asmFile.is_open()){
        cout << "error reading from file. are you sure the file exist?" << '\n';
        exit(-9);
    }
    else {
        string t;
        while(getline(asmFile, t, '\n')){
            t = trim(t);
            if (!t.empty()) r.push_back(t);
        }
    }
    asmFile.close();
    return r;
}

string replaceChar(const char r,  const char t, string s){  // r gets repolaced by t
    bool operate = true;
    char l = '\0';
    for(int  i =  0; i < s.length(); i++){
        if (s[i] == '\'' || s[i] == '\"'){
            if (operate){
                l = s[i];
                operate = !(operate);
            }
            else if (s[i] == l){
                l = '\0';
                operate = !(operate);
            }
        }
        if (s[i] ==  r && operate) s[i] =  t;
    }
    return s;
}

auto splitAt(const char c, string s) {
    vector<string> r;
    bool inQuotes = false;
    char quoteChar = '\0';
    string current;

    for (size_t i = 0; i < s.size(); i++) {
        char ch = s[i];

        // Handle quote toggling
        if (ch == '\'' || ch == '"') {
            if (!inQuotes) {
                inQuotes = true;
                quoteChar = ch;
            } else if (ch == quoteChar) {
                inQuotes = false;
                quoteChar = '\0';
            }
            current += ch;
        }
        // Split only if outside quotes
        else if (!inQuotes && ch == c) {
            string t = trim(current);
            if (!t.empty()) r.push_back(t);
            current.clear();
        }
        else {
            current += ch;
        }
    }

    // push last token
    if (!current.empty()) {
        string t = trim(current);
        if (!t.empty()) r.push_back(t);
    }

    return r;
}


void storeLable (vector<string>& v){
    int n = v.size();
    for(int i=0;i<n; i++){
        int j= v[i].find(':');
        if (j==string::npos) continue;
        string l=trim(v[i].substr(0,j));
        v[i].erase(0, j+1);
        v[i]= trim(v[i]);
       if(v[i].empty()) {
            v.erase(v.begin()+i);
            i--;
            n--;
        }
       labelTable.insert({l,4 * (i+1)});
    }
}

int convertToDecimal(const string &num) {
    if (num.empty()) return 0;
    else if (num.find("0x") == 0) return stoi(num.substr(2), nullptr, 16);
    else if (num.find("0b") == 0) return stoi(num.substr(2), nullptr, 2);
    else return stoi(num);
}

string binToHex(string machineInstruction, bool padRight = false) {

    size_t len = machineInstruction.length();
    size_t rem = len % 4;
    if (rem != 0) {
        size_t pad = 4 - rem;
        if (padRight)
            machineInstruction.append(pad, '0');
        else
            machineInstruction.insert(0, pad, '0');
    }

    ostringstream machineInstructionHex;
    char mcHex[9] = {};
    mcHex[8] = '\0';

    for (size_t i = 0; i < machineInstruction.length(); i += 4) {
        string sTemp = machineInstruction.substr(i, 4);
        uint32_t iTemp = stoi(sTemp, nullptr, 2);
        sprintf(mcHex, "%x", iTemp);
        machineInstructionHex << mcHex;
    }
    return machineInstructionHex.str();
}


string handle_r(const vector<string>& i){
    string opcode, rd, rs1, rs2, funct3, funct7;
    tie(opcode, funct3, funct7) = r_format[i[0]];
    rd = registers[i[1]];
    rs1 = registers[i[2]];
    rs2 = registers[i[3]];
    string machineInstruction;
    machineInstruction = funct7 + rs2 + rs1 + funct3 + rd + opcode;
    ostringstream r;
    string machineInstructionHexStr = binToHex(machineInstruction);
    r << "0x" << machineInstructionHexStr << " , " << i[0] << " " << i[1] << ',' << i[2] << ',' << i[3] << " # " << opcode << "-" << funct3 << "-" << funct7 << "-" << rd << "-" << rs1 << "-" << rs2 << "-" << "NULL" << endl;
    return r.str();
}

string handle_i(const vector<string>& i){
    string opcode, rd, rs1, imm, funct3;
    tie(opcode, funct3) = i_format[i[0]];
    rd = registers[i[1]];
    rs1 = registers[i[2]];
    imm = i[3];
    int iImm = (imm.substr(0, 2) == "0x") ? convertToDecimal(imm) : stoi(imm);
    char buffer[33];
    intToBinaryChar(buffer, 32, iImm);
    imm = buffer;
    while (imm.length() < 12) imm = "0" + imm;
    if (imm.length() > 12) imm.erase(0, imm.length() - 12);
    string machineInstruction = imm + rs1 + funct3 + rd + opcode;
    ostringstream r;
    string machineInstructionHexStr = binToHex(machineInstruction);
    r << "0x" << machineInstructionHexStr << " , " << i[0] << " " << i[1] << ',' << i[2] << ',' << i[3] << " # " << opcode << "-" << funct3 << "-" << "NULL" << "-" << rd << "-" << rs1 << "-" << "NULL" << "-" << imm << endl;
    return r.str();
}

string handle_il(const vector<string>& j){
    vector<string> i;
    i.push_back(j[0]);
    i.push_back(j[1]);
    auto t = splitAt('(', j[2]);
    auto t2 = splitAt(')', t[1]);
    t.pop_back();
    for (auto& t3:t2) t.push_back(t3);
    for (auto& t3:t) i.push_back(t3);
    string opcode, rd, rs1, imm, funct3;
    tie(opcode, funct3) = i_format[i[0]];
    rd = registers[i[1]];
    rs1 = registers[i[3]];
    imm = i[2];
    int iImm = (imm.substr(0, 2) == "0x") ? convertToDecimal(imm) : stoi(imm);
    char buffer[33];
    intToBinaryChar(buffer, 32, iImm);
    imm = buffer;
    while (imm.length() < 12) imm = "0" + imm;
    if (imm.length() > 12) imm.erase(0, imm.length() - 12);
    string machineInstruction = imm + rs1 + funct3 + rd + opcode;
    ostringstream r;
    string machineInstructionHexStr = binToHex(machineInstruction);
    r << "0x" << machineInstructionHexStr << " , " << j[0] << " " << j[1] << ',' << j[2] << " # " << opcode << "-" << funct3 << "-" << "NULL" << "-" << rd << "-" << rs1 << "-" << "NULL" << "-" << imm << endl;
    return r.str();
}

string handle_s(const vector<string>& j){
    vector<string> i;
    i.push_back(j[0]);
    i.push_back(j[1]);
    auto t = splitAt('(', j[2]);
    auto t2 = splitAt(')', t[1]);
    t.pop_back();
    for (auto& t3:t2) t.push_back(t3);
    for (auto& t3:t) i.push_back(t3);
    string opcode, rs1, rs2, imm, funct3;
    tie(opcode, funct3) = s_format[i[0]];
    rs1 = registers[i[3]];
    rs2 = registers[i[1]];
    imm = i[2];
    int iImm = (imm.substr(0, 2) == "0x") ? convertToDecimal(imm) : stoi(imm);
    char buffer[33];
    intToBinaryChar(buffer, 32, iImm);
    imm = buffer;
    while (imm.length() < 12) imm = "0" + imm;
    if (imm.length() > 12) imm.erase(0, imm.length() - 12);
    string machineInstruction = imm.substr(0, 7) + rs2 + rs1 + funct3 + imm.substr(7, 5) + opcode;
    ostringstream r;
    string machineInstructionHexStr = binToHex(machineInstruction);
    r << "0x" << machineInstructionHexStr << " , " << j[0] << " " << j[1] << ',' << j[2] << " # " << opcode << "-" << funct3 << "-" << "NULL" << "-" << "NULL" << "-" << rs1 << "-" << rs2 << "-" << imm << endl;
    return r.str();
}

string handle_sb(const vector<string>& i){
    string opcode, rs1, rs2, imm, funct3;
    tie(opcode, funct3) = sb_format[i[0]];
    rs1 = registers[i[1]];
    rs1 = registers[i[2]];
    imm = i[3];
    int iImm = (imm.substr(0, 2) == "0x") ? convertToDecimal(imm) : stoi(imm);
    char buffer[33];
    intToBinaryChar(buffer, 32, iImm);
    imm = buffer;
    while (imm.length() < 13) imm = "0" + imm;
    if (imm.length() > 13) imm.erase(0, imm.length() - 13);
    string machineInstruction = string(1, imm[0]) + imm.substr(2, 6) + rs2 + rs1 + funct3 + imm.substr(8, 4) + string(1, imm[1]) + opcode;
    ostringstream r;
    string machineInstructionHexStr = binToHex(machineInstruction);
    r << "0x" << machineInstructionHexStr << " , " << i[0] << " " << i[1] << ',' << i[2] << ',' << i[3] << " # " << opcode << "-" << funct3 << "-" << "NULL" << "-" << "NULL" << "-" << rs1 << "-" << rs2 << "-" << imm << endl;
    return r.str();
}

string handle_u(const vector<string>& i){
    string opcode, rd, imm;
    opcode = u_format[i[0]];
    rd = registers[i[1]];
    imm = i[2];
    int iImm = (imm.substr(0, 2) == "0x") ? convertToDecimal(imm) : stoi(imm);
    char buffer[33];
    intToBinaryChar(buffer, 32, iImm);
    imm = buffer;
    while (imm.length() < 20) imm = "0" + imm;
    if (imm.length() > 20) imm.erase(0, imm.length() - 20);
    string machineInstruction = imm + rd + opcode;
    ostringstream r;
    string machineInstructionHexStr = binToHex(machineInstruction);
    r << "0x" << machineInstructionHexStr << " , " << i[0] << " " << i[1] << ',' << i[2] << " # " << opcode << "-" << "NULL" << "-" << "NULL" << "-" << rd << "-" << "NULL" << "-" << "NULL" << "-" << imm << endl;
    return r.str();
}

string handle_uj(const vector<string>& i){
    string opcode, rd, imm;
    opcode = uj_format[i[0]];
    rd = registers[i[1]];
    imm = i[2];
    int iImm = (imm.substr(0, 2) == "0x") ? convertToDecimal(imm) : stoi(imm);
    char buffer[33];
    intToBinaryChar(buffer, 32, iImm);
    imm = buffer;
    while (imm.length() < 21) imm = "0" + imm;
    if (imm.length() > 21) imm.erase(0, imm.length() - 21);
    string machineInstruction = string(1, imm[0]) + imm.substr(10, 10) + string(1, imm[9]) + imm.substr(1,8) + rd + opcode;
    ostringstream r;

    string machineInstructionHexStr = binToHex(machineInstruction);

    r << "0x" << machineInstructionHexStr << " , " << i[0] << " " << i[1] << ',' << i[2] << " # " << opcode << "-" << "NULL" << "-" << "NULL" << "-" << rd << "-" << "NULL" << "-" << "NULL" << "-" << imm << endl;
    return r.str();
}

string secondPass(vector<vector<string>> v){
    stringstream outp;
    int off = 0;
    char buffer[33];
    for (auto& i:v){
        auto instruction_type = formats.at(i[0]);
        string mc;
    
        switch(instruction_type){
            case (R_TYPE) :  mc=handle_r(i); break;
            case (I_TYPE) :  mc=handle_i(i); break;
            case (IL_TYPE) :  mc=handle_il(i); break;
            case (S_TYPE) :  mc=handle_s(i); break;
            case (SB_TYPE) : ; mc=handle_sb(i); break;
            case (U_TYPE) :  mc=handle_u(i); break;
            case (UJ_TYPE) : ; mc=handle_uj(i); break;
        };
        sprintf(buffer, "%x", off);
        outp << "0x" << buffer << " " << mc;
        off += 4;
    }
    outp << endl;
    return outp.str();
}

vector<vector<string>> dataPart(vector<string>&v){
    int dataline=0;
    while(dataline < v.size() && v[dataline].find(".data")==string::npos){
        dataline++;
    }
    v.erase(v.begin()+dataline);
    int dstart = dataline;
    vector<vector<string>> data;
    while (dataline < v.size() && (v[dataline].find(".text") == string::npos)){
        string line = v[dataline];
        v[dataline]= replaceChar(',',' ', v[dataline]);
        v[dataline]= replaceChar(':',' ', v[dataline]);
        auto parts = splitAt(' ', line);
        if (!parts.empty()) data.push_back(parts);
        dataline++;
    }
    if (dataline < v.size())v.erase(v.begin() + dstart,v.begin()+dataline);
    else v.erase(v.begin() + dstart, v.end());
    dataline = 0;
    while(dataline < v.size() && v[dataline].find(".text")==string::npos){
        dataline++;
    }
    if (dataline < v.size())v.erase(v.begin()+dataline);
    return data;
}

string intToHex(int num) {
    std::stringstream ss;
    ss << hex;
    ss << num;
    return ss.str();
}

string  intToString(int  num){
    stringstream  ss;
    ss << num;
    return  ss.str();
}

string dataReader(vector<vector<string>>data){
    string dataSeg;
    int address= dataStart;
    int sizeStart = 1;
    for (vector<string> v:data){
        sizeStart = 1;
        if (v.size() < 2 || trim(v[0]).empty()) continue;
        if (v[0].find(':') != string::npos){
            v[0].erase(v[0].begin()+v[0].find(':'));
            labelTable.insert({v[0],address});
        }
        else {
            sizeStart--;
        }
        int size = dataSize.at(v[sizeStart]);
        int n = v.size();
        bool isNum = (v[sizeStart].find(".asciiz") == string::npos);
        if (isNum){
            for(int i=sizeStart+1;i<n; i++){
                int val = convertToDecimal(v[i]);
                dataSeg = dataSeg+("0x"+intToHex(address)+" 0x"+intToHex(val)+"\n");
                address = address+size;
            }
        }
        else{ //null pointer at end
            for (auto c:v[sizeStart+1]){
                if (c == '\"') continue;
                int val = static_cast<unsigned int>(c);
                dataSeg = dataSeg+("0x"+intToHex(address)+" 0x"+intToHex(val)+"\n");
                address = address+size;
            }

            dataSeg = dataSeg+("0x"+intToHex(address)+" 0x"+intToHex(0)+"\n");
            address= address+1;
        }
    }
    return dataSeg;
}

void storeOutput(string output, string fileName){
    ofstream outfile;
    outfile.open(fileName);
    if (outfile.is_open()){
        outfile.write(output.c_str(), output.size());
    }
    else {
        cout << "error opening output file" << endl;
        exit(-11);
    }
    outfile.close();
}

int main(){
    string inputFileName = "input.asm";
    cout << "trying to read input.asm" << endl;
    //cout << "Enter the file name: ";
    //getline(cin, inputFileName);
    auto v = loadFile(inputFileName);
    cout << "read input.asm" << endl;
    cout << "processing." << flush;
    //handle .data and remove it form v and insert it 
    vector <vector<string>> data = dataPart(v);
    cout << '.' << flush;
    string dataSeg = dataReader(data); //data seg to be added at end of result
    cout << '.' << flush;
    //lable with semicolen removed
    storeLable(v);
    cout << '.' << flush;
    vector<vector<string>> instrc;
    for(string s : v){
        s = replaceChar(',', ' ',s);
        instrc.push_back(splitAt(' ',s));
    }
    cout << '.' << flush;

    //replaicing lable in instructions with value in lableTable
    int offset = 0;
    for(vector<string>&x:instrc){
        INSTRUCTION_TYPE t= formats[x[0]];
        if (t=R_TYPE){
            offset += 4;
            continue;
        }
        string  *y=&x.back();
        auto temp = labelTable.find(*y);
        if (temp==labelTable.end()){
            offset += 4;
            continue;
        }
        string replace = to_string((temp->second) - offset), lable=temp->first;
        int pos= (*y).find(lable);
        (*y).replace(pos, lable.length(), replace);
        offset += 4;
    }
    cout << '.' << flush;
    string textSeg = secondPass(instrc);
    cout << '.' << endl;
    string finalOut = textSeg + "\n\n<!============END OF CODE. DATA SEGMENT STARTS NOW============!>\n\n" + dataSeg;

    cout << "writing to file" << endl;
    string outputFileName = "output.mc";
    storeOutput(finalOut, outputFileName);
    cout << "finished!" << endl;
    cout << endl;
    
    return 0;
}
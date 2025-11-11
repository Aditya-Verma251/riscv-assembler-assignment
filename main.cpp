#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cctype>
#include <sstream>

using namespace std;

struct mc{
    int opcode, fun3, fun7, rd, rs1, rs2;
};

enum INSTRUCTION_TYPE {
    R_TYPE, I_TYPE, S_TYPE, SB_TYPE, U_TYPE, UJ_TYPE
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
    {"lb", I_TYPE}, 
    {"ld", I_TYPE}, 
    {"lh", I_TYPE}, 
    {"lw", I_TYPE}, 
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

unordered_map<string, int> labelTable;

const  int  dataStart = 0x10000000;

string ltrim(string s) {
    size_t i = 0;
    while (i < s.length() && isspace(static_cast<unsigned char>(s[i]))) i++;
    s.erase(0, i);
    return s;
}

string rtrim(string s) {
    size_t i = s.length();
    while (i > 0 && isspace(static_cast<unsigned char>(s[i]))) i--;
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
    for   (int  i =  0; i < s.length(); i++){
        if (s[i] ==  r) s[i] =  t;
    }
    return s;
}

auto splitAt(const  char  c, string  s){
    vector<string>  r;
    int  n;
    while ((n  =  s.find(c))  != string::npos){
        string t  = trim(s.substr(0, n));
        if (!t.empty()) r.push_back(t);
        s.erase(0, n);
    }
    return r;
}

void storeLable (vector<string>v){
    int n = v.size();
    for(int i=0;i<n; i++){
        int j= v[i].find(':');
        if (j==string::npos) continue;
        string l=v[i].substr(0,i-1);
        v[i].erase(0, j+1);
        v[i]= trim(v[i]);
       if(v[i].empty()) {
            v.erase(v.begin()+i);
            i--;
            n--;
        }
       labelTable.insert({l,i+1});
    }
}

int convertToDecimal(const string &num) {
    if (num.empty()) return 0;
    else if (num.find("0x") == 0) return stoi(num.substr(2), nullptr, 16);
    else if (num.find("0b") == 0) return stoi(num.substr(2), nullptr, 2);
    else return 0;
    return stoi(num);
}

string handle_r(vector<string>& i){}

string handle_i(vector<string>& i){}

string handle_s(vector<string>& i){}

string handle_sb(vector<string>& i){}

string handle_u(vector<string>& i){}

string handle_uj(vector<string>& i){}

int secondPass(vector<vector<string>> v, string outputFileName){
    stringstream outp;
    int off = 0;
    for (auto& i:v){
        auto instruction_type = formats[i[0]];
        string mc;
        switch(instruction_type){
            case (R_TYPE) : mc=handle_r(i); break;
            case (I_TYPE) : mc=handle_i(i); break;
            case (S_TYPE) : mc=handle_s(i); break;
            case (SB_TYPE) : mc=handle_sb(i); break;
            case (U_TYPE) : mc=handle_u(i); break;
            case (UJ_TYPE) : mc=handle_uj(i); break;
        };
        off += 4;
    }
}

int mymain(){
    return 0;
void handleData(vector<string>v){

}

int main(){
    string a;
    getline(cin, a);
    auto v = loadFile(a);
    for(string s:v){
        cout << s << '\n';
    }
    cout << endl;
    //handle .data and remove it form v and insert it 

    //lable with semicolen removed
    storeLable(v);
    
    vector<vector<string>> instrc;

    mymain();
    for(string s : v){
        instrc.push_back(splitAt(' ',s));
    }
    
    return 0;
}
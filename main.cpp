#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cctype>

using namespace std;

struct mc{
    int opcode, fun3, fun7, rd, rs1, rs2;
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
        string t  = s.substr(0, n);
        r.push_back(t);
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

int main(){
    string a;
    getline(cin, a);
    auto v = loadFile(a);
    for(string s:v){
        cout << s << '\n';
    }
    cout << endl;
    //handle .data and remove it form v

    //lable with semicolen removed
    storeLable(v);
    
    vector<vector<string>> instrc;
    



    return 0;
}
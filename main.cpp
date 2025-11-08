#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <cctype>

using namespace std;

unordered_map<string, int> labelTable;

string ltrim(string s) {
    size_t i = 0;
    while (i < s.length() && isspace((unsigned char)s[i])) i++;
    s.erase(0, i);
    return s;
}

string rtrim(string s) {
    size_t i = s.length();
    while (i > 0 && isspace((unsigned char)s[i - 1])) i--;
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
    return r;
}

int main(){
    string a;
    getline(cin, a);
    auto v = loadFile(a);
    for(string s:v){
        cout << s << '\n';
    }
    cout << endl;
    return 0;
}
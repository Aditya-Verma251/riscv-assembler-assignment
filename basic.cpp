#include <iostream>
#include <bits/stdc++.h>
using namespace std;

string intToHex(int num) {
    std::stringstream ss;
    ss << hex;
    ss << num;
    return ss.str();
}

int main() {
    int num=16;
    string s=  intToHex(num);
    cout<<s;
}   
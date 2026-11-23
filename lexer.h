#pragma once
#include <vector>
#include <string>
#include <cctype>
using namespace std;

vector<string> getTokens(string s) {
    vector<string> tokens;
    for (int i = 0; i < (int)s.size(); ) {
        if (isspace(s.at(i))) { i++; continue; }
        if (s.at(i) == '/' && i + 1 < (int)s.size() && s.at(i + 1) == '/') {
            while (i < (int)s.size() && s.at(i) != '\n') i++;
            continue;
        }
        if (isalpha(s.at(i)) || s.at(i) == '_') {
            string word;
            while (i < (int)s.size() && (isalnum(s.at(i)) || s.at(i) == '_')) word += s.at(i++);
            tokens.push_back(word);
        } else if (isdigit(s.at(i))) {
            string num;
            while (i < (int)s.size() && isdigit(s.at(i))) num += s.at(i++);
            tokens.push_back(num);
        } else {
            tokens.push_back(string(1, s.at(i++)));
        }
    }
    return tokens;
}

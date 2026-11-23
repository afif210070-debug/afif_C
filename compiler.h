#pragma once
#include <sstream>
#include <map>
#include <set>
#include <stdexcept>
#include "lexer.h"

string toStr(int n) { ostringstream s; s << n; return s.str(); }
int toInt(string s) { int n = 0; istringstream(s) >> n; return n; }

class MiniCompiler {
    vector<string> tok;
    int pos = 0, tmp = 0;
    set<string> sym;
    vector<vector<string>> ir, rawIR, bc;

    string peek() { return pos < (int)tok.size() ? tok.at(pos) : ""; }
    void match(string s) { if (peek() != s) throw runtime_error("Syntax Error: Expected '" + s + "'"); pos++; }
    bool isNum(string s) {
        if (s.empty()) return false;
        for (int i = (s.at(0) == '-' && s.size() > 1) ? 1 : 0; i < (int)s.size(); i++)
            if (!isdigit(s.at(i))) return false;
        return true;
    }

    string factor() {
        string t = peek();
        if (isNum(t)) { pos++; return t; }
        if (t == "(") { match("("); string v = expr(); match(")"); return v; }
        if (!t.empty() && isalpha(t.at(0))) {
            pos++; if (!sym.count(t)) throw runtime_error("Semantic Error: Undeclared '" + t + "'");
            return t;
        }
        throw runtime_error("Syntax Error near '" + t + "'");
    }

    string binOp(string ops, string (MiniCompiler::*sub)()) {
        string l = (this->*sub)();
        while (!peek().empty() && ops.find(peek()) != string::npos) {
            string op = peek(); pos++;
            string r = (this->*sub)(), t = "t" + toStr(tmp++);
            if (op == "/" && r == "0") throw runtime_error("Semantic Error: Division by zero");
            ir.push_back({t, l, op, r}); l = t;
        }
        return l;
    }
    string term() { return binOp("*/", &MiniCompiler::factor); }
    string expr() { return binOp("+-", &MiniCompiler::term); }

    void stmt() {
        string t = peek();
        if (t == "int") {
            match("int"); string id = peek(); pos++;
            if (!sym.insert(id).second) throw runtime_error("Semantic Error: Redeclared '" + id + "'");
            string val = (peek() == "=") ? (match("="), expr()) : "0";
            match(";"); ir.push_back({id, val, "", ""});
        } else if (t == "printf") {
            match("printf"); match("("); ir.push_back({"PRINT", expr(), "", ""}); match(")"); match(";");
        } else if (t == "return") { match("return"); expr(); match(";"); }
        else if (!t.empty() && isalpha(t.at(0))) {
            string id = peek(); pos++;
            if (!sym.count(id)) throw runtime_error("Semantic Error: Undeclared '" + id + "'");
            match("="); ir.push_back({id, expr(), "", ""}); match(";");
        } else throw runtime_error("Syntax Error near '" + t + "'");
    }

    string dump(vector<vector<string>>& v) {
        string s;
        for (auto& x : v) s += (x.at(0) == "PRINT" ? "PRINT " + x.at(1) : x.at(0) + " = " + x.at(1) + " " + x.at(2) + " " + x.at(3)) + "\n";
        return s;
    }

public:
    string compileAndRun(string code) {
        tok = getTokens(code);
        if (peek() == "int" && pos + 1 < (int)tok.size() && tok.at(pos + 1) == "main") {
            match("int"); match("main"); match("("); match(")"); match("{");
            while (peek() != "}" && !peek().empty()) stmt();
            match("}");
        } else while (!peek().empty()) stmt();

        rawIR = ir; map<string, string> cv;
        for (auto& x : ir) {
            if (cv.count(x.at(1))) x.at(1) = cv[x.at(1)];
            if (cv.count(x.at(3))) x.at(3) = cv[x.at(3)];
            if (!x.at(2).empty() && isNum(x.at(1)) && isNum(x.at(3))) {
                int a = toInt(x.at(1)), b = toInt(x.at(3));
                x.at(1) = toStr(x.at(2) == "+" ? a + b : x.at(2) == "-" ? a - b : x.at(2) == "*" ? a * b : a / b);
                x.at(2) = x.at(3) = "";
            }
            if (x.at(2).empty() && isNum(x.at(1)) && x.at(0).at(0) == 't') cv[x.at(0)] = x.at(1);
        }

        for (auto& x : ir) {
            bc.push_back({isNum(x.at(1)) ? "PUSH" : "LOAD", x.at(1)});
            if (x.at(0) == "PRINT") bc.push_back({"PRINT", ""});
            else if (x.at(2).empty()) bc.push_back({"STORE", x.at(0)});
            else {
                bc.push_back({isNum(x.at(3)) ? "PUSH" : "LOAD", x.at(3)});
                bc.push_back({x.at(2) == "+" ? "ADD" : x.at(2) == "-" ? "SUB" : x.at(2) == "*" ? "MUL" : "DIV", ""});
                bc.push_back({"STORE", x.at(0)});
            }
        }

        vector<int> st; map<string, int> mem; stringstream out;
        for (auto& b : bc) {
            if (b.at(0) == "PUSH") st.push_back(toInt(b.at(1)));
            else if (b.at(0) == "LOAD") st.push_back(mem[b.at(1)]);
            else if (b.at(0) == "STORE") { mem[b.at(1)] = st.back(); st.pop_back(); }
            else if (b.at(0) == "PRINT") { out << st.back() << "\n"; st.pop_back(); }
            else {
                int r = st.back(); st.pop_back(); int l = st.back(); st.pop_back();
                st.push_back(b.at(0) == "ADD" ? l + r : b.at(0) == "SUB" ? l - r : b.at(0) == "MUL" ? l * r : l / r);
            }
        }

        string tokS, bcS;
        for (auto& t : tok) tokS += "[" + t + "] ";
        for (auto& b : bc) bcS += b.at(0) + " " + b.at(1) + "\n";
        return "OK|||" + out.str() + "|||" + tokS + "|||" + dump(rawIR) + "|||" + dump(ir) + "|||" + bcS;
    }
};

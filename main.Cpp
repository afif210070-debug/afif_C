#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <winsock2.h>
#include "compiler.h"
#pragma comment(lib, "ws2_32.lib")

using namespace std;

string toStr(size_t num) {
    ostringstream ss;
    ss << num;
    return ss.str();
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET srv = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    bind(srv, (sockaddr*)&addr, sizeof(addr));
    listen(srv, 5);
    system("start msedge --app=http://localhost:8080");
    cout << "afif_C Compiler Ready at: http://localhost:8080\n";

    while (true) {
        SOCKET c = accept(srv, NULL, NULL);
        string buf(8192, '\0');
        int n = recv(c, &buf.at(0), 8191, 0);
        if (n <= 0) { closesocket(c); continue; }

        string req = buf.substr(0, n), res, body;
        if (req.find("GET / ") == 0) {
            ifstream f("index.html");
            string line;
            while (getline(f, line)) body += line + "\n";
            res = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + toStr(body.size()) + "\r\n\r\n" + body;
        } else if (req.find("POST /run") == 0) {
            size_t pos = req.find("\r\n\r\n");
            string code = (pos != string::npos) ? req.substr(pos + 4) : "";
            try {
                MiniCompiler compiler;
                body = compiler.compileAndRun(code);
            } catch (exception& e) {
                body = string("ERR|||") + e.what();
            }
            res = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: " + toStr(body.size()) + "\r\n\r\n" + body;
        }
        send(c, res.c_str(), res.size(), 0);
        closesocket(c);
    }
}

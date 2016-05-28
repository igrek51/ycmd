#include "input.h"
#include "system.h"

bool input_cmd() {
    char ch;
    string cmd = "";
    while (true) {
        cout << "> ";
        cmd = "";
        while (true) {
            ch = getch();
            if (ch == 27) { //escape
                cmd = "exit";
                break;
            }
            if (ch == 8) { //backspace
                if (cmd.length() > 0) {
                    cmd = cmd.substr(0, cmd.length() - 1);
                    continue;
                }
            }
            if (ch == '\r' || ch == '\n') { //enter
                cout << endl;
                break;
            }
            if (ch == -32) {
                getch();
                continue;
            }
            cout << ch;
            cmd += ch;
        }
        if (cmd == "exit") break;
        system2(cmd);
        //cout<<endl;
    }
    return true;
}

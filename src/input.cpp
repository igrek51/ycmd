#include "input.h"
#include "system.h"

#include <conio.h>

bool input_cmd() {
    char ch, ch2;
    string cmd = "";
    while (true) {
        cout << "> ";
        cmd = "";
        do {
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
                ch2 = getch();
                continue;
            }
            cout << ch;
            cmd += ch;
        } while (ch != '\r' && ch != '\n');
        if (cmd == "exit") break;
        system2(cmd);
        //cout<<endl;
    }
    return true;
}

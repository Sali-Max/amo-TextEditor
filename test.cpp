#include <iostream>
#include <ncurses.h>
#include <string>
#include <fstream>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace std;

// تابعی برای چاپ خطوط جدید در `pad`
void printBuffer(int start, int end, vector<string> &lines, WINDOW* pad) {
    for (int i = start; i < end; i++) {
        mvwprintw(pad, i, 0, "%s", lines[i].c_str());
    }
}

// تابعی برای خواندن خطوط جدید از فایل
void read_and_save_to_vector(int screen_y, vector<string> &lines, ifstream &file, WINDOW* pad, int &lineNumber) {
    string buffer;
    int oldLineNumber = lineNumber;
    
    // خواندن `screen_y` خط جدید
    for (int i = 0; i < screen_y && getline(file, buffer); i++) {
        lines.push_back(buffer);
        lineNumber++;
    }

    // به‌روز کردن `pad`
    printBuffer(oldLineNumber, lineNumber, lines, pad);
}

void showText_and_movement(ifstream &file, int screen_y, int screen_x) {
    initscr();
    noecho();
    keypad(stdscr, true);

    int x = 0, y = 0;
    int key = 0;
    int pad_index = 0;
    int lineNumber = 0;

    vector<string> lines;
    WINDOW* pad = newpad(screen_y * 10, screen_x); // `pad` را بزرگ‌تر از صفحه در نظر بگیرید

    read_and_save_to_vector(screen_y, lines, file, pad, lineNumber);
    
    prefresh(pad, pad_index, 0, 0, 0, screen_y - 1, screen_x - 1);

    while (true) {
        key = getch();
        
        if (key == 258) { // ↓
            if (y < lineNumber - 1) {
                y++;
                if (y >= pad_index + screen_y - 1) {
                    pad_index++;
                    prefresh(pad, pad_index, 0, 0, 0, screen_y - 1, screen_x - 1);
                }
            } else if (getline(file, lines.emplace_back())) { // خواندن خط جدید
                lineNumber++;
                mvwprintw(pad, lineNumber - 1, 0, "%s", lines.back().c_str());
                prefresh(pad, pad_index, 0, 0, 0, screen_y - 1, screen_x - 1);
            }
        } 
        else if (key == 259) { // ↑
            if (y > 0) {
                y--;
                if (y < pad_index) {
                    pad_index--;
                    prefresh(pad, pad_index, 0, 0, 0, screen_y - 1, screen_x - 1);
                }
            }
        } 
        else if (key == 'q') { // خروج
            break;
        }

        move(y - pad_index, x);
    }

    delwin(pad);
    endwin();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    ifstream file(argv[1]);
    if (!file.is_open()) {
        printf("Error: Cannot open %s\n", argv[1]);
        return 1;
    }

    struct winsize scr;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &scr);
    
    showText_and_movement(file, scr.ws_row - 1, scr.ws_col);
    return 0;
}

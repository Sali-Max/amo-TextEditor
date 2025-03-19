#include <ncurses.h>
#include <fstream>
#include <string>
using namespace std;

void showText(ifstream &file) {
    initscr();
    noecho();
    keypad(stdscr, true);

    string line;
    int y = 0, key;
    WINDOW* pad = newpad(1000, 80);  // پد با 1000 خط و عرض 80

    while (getline(file, line))  
        mvwprintw(pad, y++, 0, "%s", line.c_str());

    int pad_index = 0;
    while ((key = getch()) != 'q') {  // خروج با 'q'
        if (key == KEY_DOWN && pad_index < y - 1) pad_index++;
        if (key == KEY_UP && pad_index > 0) pad_index--;

        clear();
        mvprintw(0, 0, "Scrolling: %d/%d", pad_index, y - 1);  // نمایش وضعیت اسکرول
        prefresh(pad, pad_index, 0, 1, 0, 20, 80);
    }

    endwin();
}

int main(int argc, char* argv[]) {
    ifstream file(argv[1]);
    if (!file) return 1;
    showText(file);
    return 0;
}

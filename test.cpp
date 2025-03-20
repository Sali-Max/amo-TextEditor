#include <ncurses.h>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

vector<string> lines;  // Store file lines
int cursor_x = 0, cursor_y = 0;

void loadFile(const char* filename) {
    ifstream file(filename);
    string line;
    while (getline(file, line))
        lines.push_back(line);
    if (lines.empty()) lines.push_back(""); // Ensure at least one line exists
}

void saveFile(const char* filename) {
    ofstream file(filename);
    for (const auto& line : lines)
        file << line << endl;
}

void printText() {
    clear();
    for (size_t i = 0; i < lines.size(); i++)
        mvprintw(i, 0, "%s", lines[i].c_str());
    move(cursor_y, cursor_x);
    refresh();
}

void editText(int key) {
    if (key == KEY_BACKSPACE || key == 127) {  // Backspace
        if (cursor_x > 0) {
            lines[cursor_y].erase(cursor_x - 1, 1);
            cursor_x--;
        }
    } else if (key == 10) {  // Enter key
        lines.insert(lines.begin() + cursor_y + 1, lines[cursor_y].substr(cursor_x));
        lines[cursor_y] = lines[cursor_y].substr(0, cursor_x);
        cursor_y++;
        cursor_x = 0;
    } else if (key >= 32 && key <= 126) {  // Printable characters
        lines[cursor_y].insert(cursor_x, 1, key);
        cursor_x++;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    loadFile(argv[1]);
    initscr(); noecho(); keypad(stdscr, true);

    int key;
    while ((key = getch()) != 27) {  // ESC to exit
        if (key == KEY_UP && cursor_y > 0) cursor_y--;
        if (key == KEY_DOWN && cursor_y < lines.size() - 1) cursor_y++;
        if (key == KEY_LEFT && cursor_x > 0) cursor_x--;
        if (key == KEY_RIGHT && cursor_x < lines[cursor_y].size()) cursor_x++;
        editText(key);
        printText();
    }

    saveFile(argv[1]);
    endwin();
    return 0;
}

/*
 * amo TextEditor
 * 
 * Copyright (C) 2025 Sali
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License only.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */


#include<iostream>
#include<ncurses.h>
#include<string>
#include<cstring> //strcmp
#include<fstream> 
#include <sstream>
#include<unistd.h>
#include<vector>
#include<sys/ioctl.h>
#include <algorithm>
using namespace std;

#define APP_NAME "amo Editor";
#define APP_VERSION "0.90";


bool save(const vector<string> &lines, const string &filename) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        return false;
    }

    const size_t BUFFER_SIZE = 8192; // 
    string buffer;
    buffer.reserve(BUFFER_SIZE);

    for (const auto &line : lines) {
        buffer.append(line).append("\n");

        // اگر بافر پر شد، بنویسیم و آن را پاک کنیم
        if (buffer.size() >= BUFFER_SIZE) {
            file.write(buffer.c_str(), buffer.size());
            buffer.clear();
        }
    }

    // نوشتن باقی‌مانده داده‌ها در فایل
    if (!buffer.empty()) {
        file.write(buffer.c_str(), buffer.size());
    }

    return file.good();
}

void refresh_line(WINDOW* pad, const long int &what_is_number_line, const vector<string> &lines)
{
    // int old_y, old_x;
    // getyx(pad, old_y, old_x);
    wmove(pad, what_is_number_line, 0); //go to line
    wclrtoeol(pad); //delete line
    mvwprintw(pad, what_is_number_line, 0, "%s", lines[what_is_number_line].c_str()); // print new line
    // wmove(pad, old_y, old_x);
}

void keyboard_Handel(vector<string> &lines, long int &what_is_number_line, int &cursor_x, const int key, vector<int> &lineNumber, int &cursor_y, WINDOW* pad, int &pad_index, const int &max_y)
{
    if(key >= 32 && key <= 126) // Print Printable key
    {
        lines[what_is_number_line].insert(cursor_x, 1, key);    // insert key to line
        refresh_line(pad, what_is_number_line, lines);  // update screen

        lineNumber[what_is_number_line] = lines[what_is_number_line].length(); // Update line size
        cursor_x++;
    }
    else if(key == KEY_BACKSPACE)
    {
        if(cursor_x > 0)    //delete one char in line
        {
            lines[what_is_number_line].erase(cursor_x-1, 1);
            refresh_line(pad, what_is_number_line, lines);  // update screen
            lineNumber[what_is_number_line] = lines[what_is_number_line].length(); // Update line size
            cursor_x--;
        }
        else    // delete line
        {   
            if(what_is_number_line != 0)    // not delete zero line
            {
                int old_lineNumber = lineNumber[what_is_number_line-1];
                //////////////////////
                lines[what_is_number_line-1].append(lines[what_is_number_line]);    //add text to up
                lineNumber[what_is_number_line-1] = lines[what_is_number_line-1].length(); ///generate new up line, lineNumber
                /////////////////////////////////////////// delete line 
                lines.erase(lines.begin()+what_is_number_line);
                lineNumber.erase(lineNumber.begin()+what_is_number_line);
                //// update screen
                for(int i=what_is_number_line-1; i<lines.size(); i++)   //refresh range
                {
                    refresh_line(pad, i, lines); 
                }
                {   //fix Endline refresh, (endline Not exist)
                    wmove(pad, lines.size(), 0); // رفتن به ابتدای خط مورد نظر
                    wclrtoeol(pad);             // پاک کردن از مکان فعلی تا انتهای خط
                }
                /////////////////////////// cursor
                what_is_number_line--;
                cursor_x = old_lineNumber;
                if(0 < cursor_y) cursor_y--;
                else pad_index--;
            }
        }
    }
    else if(key == KEY_DC)
    {
        if(cursor_x+1 < lineNumber[what_is_number_line])
        {
            lines[what_is_number_line].erase(cursor_x+1, 1);    // delete char
            refresh_line(pad, what_is_number_line, lines);  //refresh screen
            lineNumber[what_is_number_line] = lines[what_is_number_line].length(); //update lineNumber
        }
    }
    else if(key == '\n')
    {
        string send_to_next_line = lines[what_is_number_line].substr(cursor_x);
        if(send_to_next_line != "") //if selected item
        {
            ////////////////////////////////////////////////////////////// create new line
            string selected = lines[what_is_number_line].substr(cursor_x);  //get nextline text
            lines.insert(lines.begin()+what_is_number_line+1, selected);
            /////////////////////////////////// active line
            lines[what_is_number_line] = lines[what_is_number_line].erase(cursor_x, lines[what_is_number_line].length()); //update active line
            lineNumber[what_is_number_line] = lines[what_is_number_line].length(); //update active lineNumber
            //////////////////////////////////
            lineNumber.insert(lineNumber.begin()+what_is_number_line+1, selected.length()); //update new line, LineNumber
            //////////////////////////////////////////////// screen run(pad_index, cursor_y), go to next line
            if(max_y > cursor_y) cursor_y++;
            else pad_index++;
            /////////////////////// 
            what_is_number_line++;
            cursor_x = 0;
            //////////////////////////////////////////////// Refresh Display
            int lines_size = lines.size();
            for(int i=what_is_number_line-1; i<lines_size; i++)
            {
                refresh_line(pad, i, lines);
            }
        }
        else    //if not selected item to next line
        {
            lines.insert(lines.begin()+what_is_number_line+1, "");    // create line
            lineNumber.insert(lineNumber.begin()+what_is_number_line+1, 0);   //set lineNumber
            /////////////////////////////////// screen run(pad_index, cursor_y), go to next line
            if(max_y > cursor_y) cursor_y++;
            else pad_index++;
            what_is_number_line++;
            //////////////
            cursor_x = 0;
            /////////////////////////////////////////////////// refresh Display
            int lines_size = lines.size();
            for(int i=what_is_number_line-1; i<lines_size; i++)
            {
                refresh_line(pad, i, lines);
            }
        }
    }
    else if(key == 17)
    {
        endwin();
        exit(0);
    }
}

void switch_line_cursor_x_fix(vector<int> &lineNumber, int &cursor_x, long int &what_is_number_line)
{
    if(lineNumber[what_is_number_line] <= cursor_x)  // recheck cursor_x, (limit)
    {
        cursor_x = lineNumber[what_is_number_line];
    }
}

void showText_and_movement(ifstream &file, const string &filename, int max_x, int max_y)
{
    initscr();
    noecho();
    keypad(stdscr, true);
    // curs_set(0);

    //Variable
    vector<string> lines;
    vector<int> lineNumber;
    //

    
    
    {   //loading
        mvprintw(max_y,max_x/2,"Loading");
        refresh();
    }
    
    WINDOW* pad = newpad(1000, 30);
    string buffer; //temp variable
    int linenumber = 0;
    while(getline(file, buffer))    //get lines in file, save lines to lines, print line in pad
    {
        lines.push_back(buffer);
        lineNumber.push_back(buffer.length());
        mvwprintw(pad, linenumber, 0, "%s", buffer.c_str());
        linenumber++;
    }
    if(lineNumber.size() == 0)  //empty file fix
    {
        lines.push_back("");
        lineNumber.push_back(0);
    }

    file.close();   //safe close file
    
    {   //remove Loading
        move(max_y, max_x / 2);
        clrtoeol();
        refresh();
    }

    
    refresh();  //refresh Screen
    
    int key = 0;
    int pad_index = 0;
    int cursor_y = 0;
    int cursor_x = 0;
    long int what_is_number_line = 0;
    while (true)
    {   
        /////////////////////// Update Scr size (for Dynamic Screen Size)
        struct winsize scr;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &scr);  // Get Terminal Detail
        int max_y = scr.ws_row-1;
        int max_x = scr.ws_col;
        ///////////////////////

        /////////////////////////////   Input Handler
        if(key == KEY_DOWN)
        {
            if(what_is_number_line < lines.size()-1)  //set maximum down
            {
                if(cursor_y == max_y) 
                {
                    pad_index++; 
                    what_is_number_line++;

                    // recheck cursor_x, (limit)
                    switch_line_cursor_x_fix(lineNumber, cursor_x, what_is_number_line);
                }
                else
                {
                    cursor_y++;
                    what_is_number_line++;
                    // recheck cursor_x, (limit)
                    switch_line_cursor_x_fix(lineNumber, cursor_x, what_is_number_line);
                }
            }
        }
        else if(key == KEY_UP)
        {
            if(cursor_y > 0)
            {
                cursor_y--;
                what_is_number_line--;
                switch_line_cursor_x_fix(lineNumber, cursor_x, what_is_number_line);
            } 
            else 
            {
                if(pad_index > 0)   // only positive pad_index
                {
                    pad_index--;
                    what_is_number_line--;
                    switch_line_cursor_x_fix(lineNumber, cursor_x, what_is_number_line);
                }
            }
        }
        else if(key == KEY_LEFT)
        {
            if(cursor_x > 0)
            {
                cursor_x--;
            }
            else    // go to up line, (if exist)
            {
                if(what_is_number_line > 0)
                {
                    if(cursor_y > 0) cursor_y--;
                    else pad_index--;
                    cursor_x = lineNumber[what_is_number_line-1];
                    what_is_number_line--;
                }
            }
        }
        else if(key == KEY_RIGHT)
        {
            if(lineNumber[what_is_number_line] > cursor_x)
            {      
                cursor_x++;
            }
            else //if right unvilable >> line to next(if next is avilable)
            {
                if(what_is_number_line+1 < lines.size())    //if next line is avilable
                {   
                    if(cursor_y < max_y) cursor_y++;
                    else pad_index++;
                    what_is_number_line++;
                    cursor_x = 0;
                }
            }
        }
        else if(key == KEY_PPAGE) // page Up    
        {
            if(pad_index > 10)
            {
                pad_index-=10;
                what_is_number_line-=10;
                switch_line_cursor_x_fix(lineNumber, cursor_x, what_is_number_line);
            }
            else
            {
                pad_index=0;
                cursor_y=0;
                what_is_number_line = 0;
                switch_line_cursor_x_fix(lineNumber, cursor_x, what_is_number_line);
            }

        }
        else if(key == KEY_NPAGE)   //page down
        {
            if(what_is_number_line+(max_y+cursor_y) < lines.size())   //if thisLine+10 not biger of all lines
            {
                pad_index+=10;
                what_is_number_line+=10;
                
                switch_line_cursor_x_fix(lineNumber, cursor_x, what_is_number_line);
            }
            else    //if end Pageup btn click
            {
                while (true)    // cursor_y to end screen
                {
                    if(cursor_y < max_y && what_is_number_line < lines.size()-1)
                    {
                        cursor_y++;
                        what_is_number_line++;
                    }
                    else break;
                }

                while(true) //pad_index to end
                {
                    if(what_is_number_line < lines.size()-1)    // cursor now is end, this (if) checked pad_index is ended!?
                    {
                        what_is_number_line++;
                        pad_index++;
                    }
                    else break;
                }
            }
            
        }
        else if(key == KEY_HOME)
        {
            cursor_x = 0;
        }
        else if(key == KEY_END)
        {
            cursor_x = lineNumber[what_is_number_line];
        }
        ////////////////////////////// edit file
        else if(key == 19) //save file
        {
            if(save(lines, filename))
            {
                { //debug
                    endwin();
                    cout << "Success :)" << "\n";
                    sleep(1);
                    // initscr();
                }
            }
            else
            {
                { //debug
                    endwin();
                    cout << "ReadOnly File :(" << "\n";
                    sleep(2);
                    // initscr();
                }
            }
        }
        else
        {
            keyboard_Handel(lines, what_is_number_line, cursor_x, key, lineNumber, cursor_y, pad, pad_index, max_y);
        }
        /////////////////////////////
        mvprintw(cursor_y, cursor_x, "");
        prefresh(pad, pad_index, 0, 0, 0, max_y, 50);   //refresh pad
        key = getch();
    }    
    
    endwin();

}

int main(int number, char* args[])
{
   
    if(number > 1) //no args coredump fix
    {
        if(strcmp(args[1], "-v")== 0 or strcmp(args[1], "--version") == 0)   //show version
        {
            cout << "Version: " << APP_VERSION;
            printf("\n");
            return 0;
        }
        if(strcmp(args[1], "-h") == 0 or strcmp(args[1], "--help") == 0)    // help
    {
        cout << R"( 
        Amo - Lightweight Terminal Text Editor

        Usage:  
        amo [file]      Open a file for editing  
        amo -h          Show this help message  
        amo -v          Show version information  

        Keyboard Shortcuts:  
        Arrow Keys      Move cursor  
        Enter          Insert new line  
        Backspace      Delete character  
        Ctrl+S         Save file  
        Ctrl+Q         Quit editor  

        Example:  
        amo notes.txt   # Open 'notes.txt' for editing  

        Fast. Simple. Efficient.
         )" << endl; 
        return 0;    
    }
    }

    ifstream file = ifstream(args[1]);

    // if(!file.is_open())  // readonly mod
    // {
    //     printf("amo: cannot open %s: No such file or directory", args[1]);
    //     return 0;
    // }

    struct winsize scr;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &scr);  // Get Terminal Detail
    int max_y = scr.ws_row-1;
    int max_x = scr.ws_col;

    showText_and_movement(file, args[1], max_x, max_y); //send file and filename
    return 0;
}

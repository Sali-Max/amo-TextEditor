#include<iostream>
#include<ncurses.h>
#include<string>
#include<fstream>
#include<unistd.h>
#include<vector>
#include<sys/ioctl.h>
using namespace std;



void showText_and_movement(ifstream &file)
{
    initscr();
    noecho();
    keypad(stdscr, true);
    // curs_set(0);

    //Variable
    vector<string> lines;
    vector<int> lineNumber;
    //

    WINDOW* pad = newpad(1000, 10);
    string buffer; //temp variable
    int linenumber = 0;
    while(getline(file, buffer))
    {
        lines.push_back(buffer);
        lineNumber.push_back(buffer.length());

        mvwprintw(pad, linenumber, 0, "%s", buffer.c_str());
        linenumber++;
    }
    
    refresh();  //refresh Screen
    
    int key = 0;
    int cursor_y, cursor_x,pad_index = 0;
    long int what_is_number_line = 0;
    while (true)
    {   
        /////////////////////// Update Scr size (for Dynamic Screen Size)
        struct winsize scr;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &scr);  // Get Terminal Detail
        int max_y = scr.ws_row-1;
        int max_x = scr.ws_col;
        ///////////////////////
        
        if(key == KEY_DOWN)
        {
            if(what_is_number_line < lines.size())
            {
                if(cursor_y == max_y) 
                {
                    pad_index++; 
                    what_is_number_line++;
                }
                else
                {
                    cursor_y++;
                    what_is_number_line++;
                }
            }
        }
        else if(key == KEY_UP)
        {
            if(cursor_y > 0)
            {
                cursor_y--;
                what_is_number_line--;
            } 
            else 
            {
                if(pad_index > 0)   // only positive pad_index
                {
                    pad_index--;
                    what_is_number_line--;
                }
            }
        }
        else if(key == KEY_LEFT)
        {
            if(cursor_x > 0)
            {
                cursor_x--;
            }
        }
        else if(key == KEY_RIGHT)
        {
            if(lineNumber[what_is_number_line] > cursor_x)
            {
                cursor_x++;
            }
        }
        mvprintw(cursor_y, cursor_x, "");
        prefresh(pad, pad_index, 0, 0, 0, max_y, 50);   //refresh pad
        key = getch();
    }
    
    

    
    
    endwin();


}

int main(int number, char* args[])
{
    ifstream file = ifstream(args[1]);

    if(!file.is_open())
    {
        printf("aim: cannot open %s: No such file or directory", args[1]);
        return 0;
    }
    showText_and_movement(file);
    return 0;
}
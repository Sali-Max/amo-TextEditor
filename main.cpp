#include<iostream>
#include<ncurses.h>
#include<string>
#include<fstream>
#include<unistd.h>
#include<vector>
#include<sys/ioctl.h>

using namespace std;





void showText_and_movement(ifstream &file,int screen_x, int &screen_y)
{
    initscr();
    noecho();
    keypad(stdscr, true);
    

    //////////////////////////////// Variable
    int x,y = 0;    // cursor position
    int key = 0;
    vector<int> linesize; // all lines size
    vector<string> lines;   //all line text
    string buffer;  //temp line
    ////////////////////////////////

    /////////////////////////////////////// Read File And Print
    while (getline(file, buffer)) //read file
    {
        lines.push_back(buffer);
        linesize.push_back(buffer.length());
    }

    int all_line_number = lines.size();
    WINDOW* pad = newpad (all_line_number, 10); //dynamic pad size
    for(size_t i=0; i<all_line_number; i++) //print all text(line by line)
    {
        mvwprintw(pad, i, 0, "%s", lines[i].c_str());
    }
    ///////////////////////////////////////

    int pad_index = 0;
    int max_y;
    int max_x;
    while (true)
    {        
        /////////////////////// Update Scr size (for Dynamic Screen Size)
        struct winsize scr;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &scr);  // Get Terminal Detail
        max_y = scr.ws_row-1;
        max_x = scr.ws_col;
        ///////////////////////


        refresh();  //refresh Cursor
        prefresh(pad, pad_index, 0, 0, 0, max_y, 80); //refresh text file
        key = getch();
        clear();


        //////////////////////// Generate Cursor Position
        if(key == 258) // down
        {
            if(y >= max_y)  //if cursor is end
            {
                pad_index++;
                /*
                    if cursor out screen > set cursor to max screen
                    
                    (fix) change size Screen > Cursor out screen
                */
                if(y != max_y) y = max_y;
            }
            else  // Cursor movement
            {
                y++;
                if(linesize[y] < x) //if cursor position(x) is not avilable on y line   //debug
                {
                    x = linesize[y];    //update x position to max
                }
            }

        }
        else if(key == 259) //up
        {
            if(0 >= y)   //max y display
            {
                pad_index--;
            }
            else if(y > 0)
            {
                y--;
                if(linesize[y] < x) x = linesize[y];//if cursor position(x) is not avilable on y line    
            }
            
        }
        else if(key == 260) //left
        {
            if(x>0)
            {
                x--;
            }
        }
        else if(key == 261) //right
        {
            if(linesize[y] > x)
            {
                x++;
            }
        }
        ////////////////////////
        
        mvprintw(y, x, "");    //print Cursor
    }

    endwin();
}

int main(int number, char* args[])
{

    // 
    struct winsize scr;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &scr);  // Get Terminal Detail
    int screen_y = scr.ws_row-1;
    int screen_x = scr.ws_col;
    //

    // printf("you say: %s", args[1]); debug
    ifstream file = ifstream(args[1]);

    if(!file.is_open())
    {
        printf("aim: cannot open %s: No such file or directory", args[1]);
        return 0;
    }

    showText_and_movement(file ,screen_x ,screen_y);

    return 0;
}
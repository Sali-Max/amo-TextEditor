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
    

    //////////////////////////////// Variable
    int x,y = 0;    // save cursor position
    int key = 0;
    int lineNumber=0;
    vector<int> linesize; //save all lines size
    string buffer;  //temp
    string final;
    ////////////////////////////////

    
    /////////////////////////////////////   print text
    WINDOW* pad = newpad(1000, 10);
    
    while (getline(file, buffer))
    {
        linesize.push_back(1);
        linesize[lineNumber] = buffer.length(); // save line size to vector
        mvwprintw(pad, lineNumber, 0, buffer.c_str());
        lineNumber++;   
    }
    /////////////////////////////

    int pad_index = 0;
    int max_y;
    int max_x;
    while (true)
    {        
        /////////////////////// update scr size
        struct winsize scr;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &scr);  // Get Terminal Detail
        max_y = scr.ws_row-1;
        max_x = scr.ws_col;
        ///////////////////////

        refresh();
        prefresh(pad, pad_index, 0, 0, 0, max_y, 80);
        key = getch();
        clear();


        //////////////////////// generate Cursor Position
        if(key == 258) // down
        {
            if(y >= max_y)
            {
                pad_index++;

                /*
                    if cursor out screen > set cursor to max screen
                    
                    (fix) change size Screen > Cursor out screen
                */
                if(y != max_y) y = max_y;
            }
            else if(y < lineNumber-1)    //debug
            {
                y++;
                if(linesize[y] < x) //if cursor position(x) is not avilable on y line
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


    // struct winsize scr;  //debug
    // ioctl(STDOUT_FILENO, TIOCGWINSZ, &scr);  // Get Terminal Detail
    // int max_y = scr.ws_row;
    // int max_x = scr.ws_col;
    // cout << "E: " << max_y << endl;
    // sleep(5);

    // printf("you say: %s", args[1]); debug
    ifstream file = ifstream(args[1]);

    if(!file.is_open())
    {
        printf("aim: cannot open %s: No such file or directory", args[1]);
        return 0;
    }

    showText_and_movement(file);

    return 0;
}
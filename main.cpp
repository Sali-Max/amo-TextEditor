#include<iostream>
#include<ncurses.h>
#include<string>
#include<fstream>
#include<unistd.h>
#include<vector>
using namespace std;






void showText_and_movement(ifstream &file)
{
    initscr();
    noecho();
    keypad(stdscr, true);
    

    //////////////////////////////// Variable
    int x,y = 0;    // save cursor position
    int key;
    int lineNumber=0;
    vector<int> linesize; //save all lines size
    string buffer;  //temp
    string final = "";
    ////////////////////////////////
    int pad_index = 0;
    while (getline(file, buffer)) // get Text(line by line)
    {

        linesize.push_back(1);  // add site vector
        linesize[lineNumber] = buffer.length(); // save line size to vector
        final += buffer;    // add tmp tp final text
        final += "\n";
        lineNumber++;

        // mvwprintw(pad, lineNumber, 0, "%s", buffer.c_str());
    }
    
    

    
    while (true)
    {
        clear();

        printw("%s", final.c_str());
        
        /////////////////////// generate Cursor Position
        if(key == 258) // down
        {
            if(y < lineNumber-1)
            {
                y++;
                pad_index++;
            }
            if(linesize[y] < x) //if cursor position(x) is not avilable on y line
            {
                x = linesize[y];    //update x position to max
            }
        }
        else if(key == 259) //up
        {
            if(y > 0)
            {
                y--;
            }

            if(linesize[y] < x) //if cursor position(x) is not avilable on y line
            {
                x = linesize[y];    //update x position to max
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
        key=getch();
        refresh();
    }

    endwin();
}

int main(int number, char* args[])
{
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
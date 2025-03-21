#include<iostream>
#include<ncurses.h>
#include<string>
#include<fstream>
#include<unistd.h>
#include<vector>
#include<sys/ioctl.h>
using namespace std;

#define APP_NAME "amo Editor";
#define APP_VERSION "0.80";


bool save(const vector<string> &lines, const string &filename)
{
    ofstream file(filename);
    if(!file.is_open()) //check 
    {
        return false;
    }

    int allLineNumber = lines.size();
    for(size_t i=0; i < allLineNumber; i++)   //print all(-1) lines and go to next line
    {
        file << lines[i] << endl;
    }

    file.close();
    return true;
}

void refresh_line(WINDOW* pad, const long int &what_is_number_line, const vector<string> &lines)
{
    wmove(pad, what_is_number_line, 0); //go to line
    wclrtoeol(pad); //delete line
    mvwprintw(pad, what_is_number_line, 0, "%s", lines[what_is_number_line].c_str()); // print new line
}

void edit(vector<string> &lines, long int &what_is_number_line, int &cursor_x, const int key, vector<int> &lineNumber, int &cursor_y, WINDOW* pad, int &pad_index, const int &max_y)
{
    if(key >= 32 && key <= 126) // Print Printable key
    {
        lines[what_is_number_line].insert(cursor_x, 1, key);    // insert key to line
        refresh_line(pad, what_is_number_line, lines);  // update screen

        lineNumber[what_is_number_line] = lines[what_is_number_line].length(); // Update line size
        cursor_x++;
    }
    else if(key == KEY_BACKSPACE)    //Delete char
    {
        if(cursor_x > 0)
        {
            lines[what_is_number_line].erase(cursor_x-1, 1);
            refresh_line(pad, what_is_number_line, lines);  // update screen
            lineNumber[what_is_number_line] = lines[what_is_number_line].length(); // Update line size
            cursor_x--;
        }
    }
    else if(key == KEY_DC)  //delete key
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
        if(send_to_next_line != "")
        {
            //////////////////////////////////////////////////////////////////////////////////
            lineNumber.insert(lineNumber.begin()+what_is_number_line+1, send_to_next_line.length()); //set linenumber new line
            lines.insert(lines.begin()+what_is_number_line+1, send_to_next_line); //create new line and set value
            lines[what_is_number_line].erase(cursor_x, lineNumber[what_is_number_line]); //delete value in old line
            lineNumber.insert(lineNumber.begin()+what_is_number_line, lines[what_is_number_line].length()); //set linenumber after remove 
            ////////////////////////////////////////////////
            if(max_y > cursor_y) cursor_y++;
            else pad_index++;
            /////////////////////// 
            what_is_number_line++;
            cursor_x = 0;
            //////////////////////////////////////////////// Refresh Display
            refresh_line(pad, what_is_number_line-1, lines);
            for(int i=what_is_number_line; i<lines.size(); i++)
            {
                refresh_line(pad, i, lines);
            }


        }
        else    //if not selected item to next line
        {
            what_is_number_line++;  // first go to new line
            ////////////////////////////////////////////////////////// create new line
            lines.insert(lines.begin() + what_is_number_line, "");  //create empty line
            lineNumber.insert(lineNumber.begin() + what_is_number_line, 0); // set lineNnumber 
            /////////////////////////////////// go to next line
            if(max_y > cursor_y) cursor_y++;
            else pad_index++;
            //////////////
            cursor_x = 0;
            /////////////////////////////////////////////////// refresh Display
            refresh_line(pad, what_is_number_line-1, lines);
            for(int i=what_is_number_line; i<lines.size(); i++)
            {
                refresh_line(pad, i, lines);
            }
        }
    }
}

void switch_line_cursor_x_fix(vector<int> &lineNumber, int &cursor_x, long int &what_is_number_line)
{
    if(lineNumber[what_is_number_line] <= cursor_x)  // recheck cursor_x, (limit)
    {
        cursor_x = lineNumber[what_is_number_line];
    }
}

void showText_and_movement(ifstream &file, const string &filename)
{
    initscr();
    noecho();
    keypad(stdscr, true);
    // curs_set(0);

    //Variable
    vector<string> lines;
    vector<int> lineNumber;
    //

    WINDOW* pad = newpad(1000, 30);
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
        }
        else if(key == KEY_RIGHT)
        {
            if(lineNumber[what_is_number_line] > cursor_x)
            {      
                cursor_x++;
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
            edit(lines, what_is_number_line, cursor_x, key, lineNumber, cursor_y, pad, pad_index, max_y);
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
    ifstream file = ifstream(args[1]);

    if(!file.is_open())
    {
        printf("amo: cannot open %s: No such file or directory", args[1]);
        return 0;
    }
    showText_and_movement(file, args[1]); //send file and filename
    return 0;
}
/* https://adventofcode.com/2021/day/2 Part 2 */

#include <istream>
#include <iostream>
#include <fstream>

#define LF 10

int main(int argc, char *argv[])
{
    std::string cmd;
    int amount;
    int position=0,depth=0,aim=0;
    std::fstream stream;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (stream.peek() != EOF)
        {
            stream >> cmd >> amount;
            
            if (cmd.compare("forward") == 0)
            {
                position += amount;
                depth += amount*aim;
            }
            if (cmd.compare("down") == 0)
                aim += amount;
            if (cmd.compare("up") == 0)
                aim -= amount;
            // std::cout << cmd << amount << aim << '\n';
 
            // consume the LF, so the stream will show EOF at last line             
            if (stream.peek() == 10) stream.get();
        }
        std::cout << "result: " << position*depth << '\n';
        stream.close();
    }
}
/* https://adventofcode.com/2021/day/3 Part 1 */

#include <istream>
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>


uint8_t countBits(std::vector<std::string> lines,int column)
{
    uint16_t zeroCount=0;
    uint16_t onesCount=0;
    for (auto line : lines)
    {
        char cell;
        cell = line[column];
        if (cell == '0') zeroCount ++;
        if (cell == '1') onesCount ++;
    }
    return (onesCount >= zeroCount)?1:0;
}

int main(int argc, char *argv[])
{
    std::string line;
    std::vector<std::string> lines;
    int lineCount=0;
    int columns=0;
    std::fstream stream;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            line.clear();
            std::getline(stream,line);
            lines.push_back(line);
            lineCount++;
        }
        stream.close();
        columns = lines[0].size();
        std::cout << "Size of grid:" << columns << ':' << lineCount << '\n';

        int epsilon = 0;
        int gamma = 0;
        for (auto i=0;i<columns;i++)
        {
            epsilon <<=1;
            gamma   <<=1;
            int cb = countBits(lines,i);
            epsilon |= (cb&1);
            gamma   |= ((cb&1)^1);
        }

        std::cout << "result: " << (epsilon * gamma) << '\n';
    }          
}

/* https://adventofcode.com/2021/day/6 - Part 1 */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

#define CHAR_SPACE 32

using std::vector;
using vNumbers = vector<int>;


// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vNumbers& obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
    return os;
}

bool isOnlyWhiteSpace(std::string _str)
{
    for (const auto& c : _str)
    {
        if (c > CHAR_SPACE )
            return false;
    }
    return true;
}

// tokenize input string and answer a vector of integers
vector<int> &tokenizeNumbers(const std::string &input)
{
    vNumbers *tokens = new vNumbers();
    int next=0,last = 0;
    int inputSize = input.size();
    while (next<inputSize)
    {
        while (isdigit(input[next]) && next<inputSize) next++;
        tokens->push_back(stoi(input.substr(last,next-last),nullptr,10));
        while (!isdigit(input[next]) && next<inputSize) next++;
        last = next;
    }
    return *tokens;    
}

int main(int argc, char *argv[])
{
    std::string numberString;
    std::fstream stream;
    vNumbers lfishList;

    int max=0;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,numberString);
            if (!isOnlyWhiteSpace(numberString))
            {
                lfishList = tokenizeNumbers(numberString);
            }
        }
        stream.close();

        int maxDay = 80;
        std::cout << "Initial state: " << lfishList << '\n';
        for (auto day=1;day<=maxDay;day++)
        {
            int i=0,end=lfishList.size();
            while (i < end)
            {
                if (lfishList[i] == 0)
                {
                    lfishList.push_back(8);
                    lfishList[i] = 6;
                }
                else
                {
                    lfishList[i] -= 1;
                }
                i++;
            }
            // std::cout << "State at day " << day << ':' << lfishList << '\n';
        }

        std::cout << "Result: "<< lfishList.size() << '\n';
    }
}
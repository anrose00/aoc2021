/* https://adventofcode.com/2021/day/8 - Part 1 */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <map>

#define CHAR_SPACE 32
#define CHAR_LF 10
#define CHAR_CR 13

#define LF '\n'

using std::vector;
using vStrings = vector<std::string>;


// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vStrings& obj) { 
    for(const auto& str: obj) std::cout << str << ' ';
    return os;
}

bool isWhiteSpace(char c)
{
    return c == CHAR_SPACE || c == CHAR_LF || c == CHAR_CR;
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

// tokenize input string and answer a vector of alpha strings
vector<std::string> tokenizeWords(const std::string &input)
{
    vStrings tokens = vStrings();
    int next = 0, last = 0;
    int inputSize = input.size();
    std::string token;
    // skip leading whitespace
    while (isWhiteSpace(input[next]) && next<inputSize) next++;
    last = next;
    while (next<inputSize)
    {
        while (isalpha(input[next]) && next<inputSize) next++;
        token = input.substr(last,next-last);
        tokens.push_back(token);
        while (isWhiteSpace(input[next]) && next<inputSize) next++;
        last = next;
    }
    return tokens;    
}

void appendToVector(vStrings &dest, vStrings const &other)
{
    for (auto s : other)
        dest.push_back(s);
}

int translate(std::map<int,int> &map, int code)
{
    if (map.count(code) > 0)
        return map.at(code);
    return 0;
}

int main(int argc, char *argv[])
{
    std::string line;
    std::string substring;
    std::fstream stream;
    auto codes = std::vector<std::string>();
    std::map<int,int> translateMap;
    int indexBar;
    
    int max=0;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,line);
            // take string right of |
            indexBar = line.find_last_of('|');
            if (indexBar == std::string::npos)
            {
                std::cout << "error in input!\n";
                exit(1);
            }
            indexBar++;
            substring = line.substr(indexBar,line.size()-indexBar);
            {
                appendToVector(codes,tokenizeWords(substring));
            }
        }
        stream.close();

        translateMap = std::map<int,int>();
        translateMap.insert(std::make_pair(2,1));   // two segments => 1
        translateMap.insert(std::make_pair(3,7));   // three segments => 7
        translateMap.insert(std::make_pair(4,4));   // four segments => 4
        translateMap.insert(std::make_pair(7,8));   // seven segments => 8

        int sum = 0;
        for (auto code : codes)      
        {
            sum += (translateMap.count(code.length()) > 0)?1:0;
        }

        std::cout << "Result: "<< sum << '\n';
    }
}
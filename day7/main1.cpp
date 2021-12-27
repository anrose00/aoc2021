/* https://adventofcode.com/2021/day/7 - Part 1 */

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

// debug helper to print pair<int,int> using <<
std::ostream & operator<<(std::ostream &os, const std::pair<int,int>& obj) { 
    os << '[' << obj.first << ',' << obj.second << ']';
    return os;
}

// tokenize input string and answer a vector of integers
vector<int> tokenizeNumbers(const std::string &input)
{
    vNumbers tokens = vNumbers();
    int next=0,last = 0;
    int inputSize = input.size();
    while (next<inputSize)
    {
        int num;
        while (isdigit(input[next]) && next<inputSize) next++;
        num = stoi(input.substr(last,next-last),nullptr,10);
        tokens.push_back(num);
        while (!isdigit(input[next]) && next<inputSize) next++;
        last = next;
    }
    return tokens;    
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

int getMax(int start, const vNumbers _vn)
{
    int res=start;
    for (auto e : _vn)
        res = std::max(res,e);
    return res;
}

std::pair<int,int> getMinIndex(int start, const vNumbers _vn)
{
    int res=start;
    int index;
    int end =_vn.size();
    for (auto i=0;i<end;i++)
    {
        if (_vn[i] < res)
        {
            res = _vn[i];
            index = i;
        }
    }
    return std::make_pair(res,index);
}

int getFuel(const vNumbers _vn, int ref)
{
    int fuel=0;
    for (auto i : _vn)
        fuel += std::abs(ref - i);
    return fuel;
}

int main(int argc, char *argv[])
{
    std::string numberString;
    std::fstream stream;
    vNumbers distances;
    vNumbers fuel;
    
    int max=0;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,numberString);
            if (!isOnlyWhiteSpace(numberString))
            {
                distances = tokenizeNumbers(numberString);
            }
        }
        stream.close();
        
        max = getMax(0,distances);

        for (auto i=0;i<=max;i++)
        {
            fuel.push_back(getFuel(distances,i));
        }

        std::cout << "Result: "<< getMinIndex(max*distances.size(),fuel) << '\n';
    }
}
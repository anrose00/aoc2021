/* https://adventofcode.com/2021/day/6 - Part 2 */
/* This one is nice. The first part directs you to 
   solution, that is not feasible for part 2.
   Because all fish in a certain state switch their
   state immediatly, you can simply manage all states
   in a single list of size 9. Just need to use 64 bit ints
   for counting */

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
using vNumbers = vector<long long>;


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
vector<long long> &tokenizeNumbers(const std::string &input)
{
    vNumbers *tokens = new vNumbers();
    int next=0,last = 0;
    int inputSize = input.size();
    while (next<inputSize)
    {
        int num;
        while (isdigit(input[next]) && next<inputSize) next++;
        num = stoi(input.substr(last,next-last),nullptr,10);
        tokens->push_back((uint8_t)num);
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
    vNumbers fishStates = {0,0,0,0,0,0,0,0,0};
    

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
        for (auto num : lfishList)
            fishStates[num]++;

        int maxDay = 256;
        std::cout << "Initial state: " << lfishList << '\n';
        for (auto day=0;day<=maxDay;day++)
        {
            // std::cout << fishStates << '\n';
            long long tmp = fishStates[0];
            for (auto i=0;i<8;i++)
                fishStates[i] = fishStates[i+1];
            fishStates[8] = tmp;
            fishStates[6] += tmp;
        }
        long long count=0;
        for (auto i=0;i<8;i++)
            count += fishStates[i];

        std::cout << "Result: "<< count << '\n';
    }
}
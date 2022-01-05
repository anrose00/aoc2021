/* https://adventofcode.com/2021/day/14 - Part 2 */

#include <istream>
#include <iostream>
#include <sstream>
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

using std::vector;
using std::string;
using std::map;

std::ostream & operator<<(std::ostream &os, const map<uint16_t,char> &_map) { 
    for(const auto& pair: _map) 
        std::cout << (char)(pair.first & 0xFF) << (char)(pair.first >> 8) << " -> " << pair.second << '\n';
    return os;
}

std::ostream & operator<<(std::ostream &os, const map<uint16_t,uint64_t> &_map) { 
    for(const auto& pair: _map) 
        std::cout << (char)(pair.first & 0xFF) << (char)(pair.first >> 8) << " -> " << pair.second << '\n';
    return os;
}

std::ostream & operator<<(std::ostream &os, const map<char,uint64_t> &_map) { 
    for(const auto& pair: _map) 
        std::cout << (char)(pair.first) << " -> " << pair.second << '\n';
    return os;
}

bool isOnlyWhiteSpace(string _str)
{
    for (const auto& c : _str)
    {
        if (c > CHAR_SPACE )
            return false;
    }
    return true;
}

bool isWhiteSpace(char c)
{
    return c == CHAR_SPACE || c == CHAR_LF || c == CHAR_CR;
}

std::string trimWhiteSpace(std::string _input)
{
    int indexEnd=_input.size()-1;
    while (indexEnd >= 0 && isWhiteSpace(_input[indexEnd])) indexEnd--;
    int indexStart=0;
    while (indexStart >= 0 && isWhiteSpace(_input[indexStart])) indexEnd++;
    return _input.substr(indexStart,indexEnd+1);
}

void sortVector(std::vector<std::pair<char,uint64_t>> &v) {
    std::pair<char,uint64_t> tmp;
    for (auto i=0;i<v.size()-1;i++)
        for (auto j=i+1;j<v.size();j++)
            if (v[i].second < v[j].second)
                {
                    tmp = v[i];
                    v[i] = v[j];
                    v[j] = tmp;
                }
}

uint64_t calcDiff(std::map<char,uint64_t> _map)
{
    auto freqVec = std::vector<std::pair<char,uint64_t>>();
    for (auto &p : _map)
        freqVec.push_back(p);
    sortVector(freqVec);
    std::cout << "Most common char:" << (freqVec.front().first) << ' ' << freqVec.front().second << '\n';
    std::cout << "Least common char:" << (freqVec.back().first) << ' ' << freqVec.back().second <<'\n';
    return freqVec.front().second-freqVec.back().second;
}

uint64_t sum(std::map<char,uint64_t> _map)
{
    uint64_t sum=0;
    for (auto &p : _map)
        sum += p.second;
    return sum;
}

int main(int argc, char *argv[])
{
    string line;
    std::fstream stream;
    string  polymereTmpl;
    string  output=string();
    auto insertRules=std::map<uint16_t,char>();
    auto inputMap = std::map<uint16_t,uint64_t>();
    auto outputMap = std::map<uint16_t,uint64_t>();
    auto countMap = std::map<char,uint64_t>();
    string substringLeft,substringRight;
    int indexSep,lineNum = 0;
    uint16_t keyInt;

    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,line);
            if (!isOnlyWhiteSpace(line) && line.size()> 0 )
            {
                if (lineNum == 0)
                    polymereTmpl = trimWhiteSpace(line);
                else
                    {
                    // take string left of -> as key
                    indexSep = line.find("->");
                    if (indexSep == string::npos)
                    {
                        std::cout << "error in input!\n";
                        exit(1);
                    }
                    substringLeft = line.substr(0,indexSep);
                    substringLeft = trimWhiteSpace(substringLeft);
                    indexSep++;
                    // take string right of -> as value
                    substringRight = line.substr(indexSep+2,line.length()-indexSep);
                    substringRight = trimWhiteSpace(substringRight);
                    keyInt = (substringLeft[1] << 8) | substringLeft[0];
                    insertRules.insert(std::make_pair(keyInt,substringRight[0]));
                    }
                lineNum++;
            }
        }
        stream.close();
        std::cout << polymereTmpl << '\n';
        std::cout << insertRules;
        int c1,c2,transInt,count,iterations=0;
        std::stringstream inputStream=std::stringstream(polymereTmpl);
        // build a map that contains pairs of two chars (16bit) and how often this pair appears
        while (!inputStream.eof())
        {
            c1 = inputStream.get();
            c2 = inputStream.peek();
            if (c2 != EOF)
            {
                keyInt = c1 | (c2 << 8);
                if (inputMap.count(keyInt) > 0)
                    inputMap.at(keyInt) += 1;
                else
                    inputMap.insert(std::make_pair(keyInt,1));
            } 
            if (countMap.count(c1) > 0)
                countMap.at(c1) += 1;
            else
                countMap.insert(std::make_pair(c1,1));
        }

        std::cout << "Iteration start:" << inputMap << countMap << '\n';
        while (iterations < 40 )
        {
            for (auto &p : inputMap)
            {
                // get a pair from input map and transform
                keyInt = p.first;
                c1 = keyInt & 0xFF;
                c2 = keyInt >> 8;
                transInt = insertRules.at(keyInt);
                // now build two new pairs and put them in output map
                // pair #1
                keyInt = c1 | (transInt << 8);
                count = outputMap.count(keyInt);
                if (count > 0)
                    outputMap.at(keyInt) += p.second;
                else
                    outputMap.insert(std::make_pair(keyInt,p.second));

                // pair #2
                keyInt = transInt | (c2 << 8);
                count = outputMap.count(keyInt);
                if (count > 0)
                    outputMap.at(keyInt) += p.second;
                else
                    outputMap.insert(std::make_pair(keyInt,p.second));
                // count the frequency of the different chars
                if (countMap.count(transInt) > 0)
                    countMap.at(transInt) += p.second;
                else
                    countMap.insert(std::make_pair(transInt,p.second));
            }
            // swap input / output map and clear output afterwards
            outputMap.swap(inputMap);
            outputMap.clear();
            std::cout << "Round "<< iterations << '\n' << inputMap << sum(countMap) <<'\n';
            iterations++;
        }

        std::cout << "Result: "<< calcDiff(countMap) << '\n';
    }
}
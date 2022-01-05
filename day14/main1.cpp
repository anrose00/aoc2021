/* https://adventofcode.com/2021/day/14 - Part 1 */

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

std::ostream & operator<<(std::ostream &os, const map<string,string> &_map) { 
    for(const auto& pair: _map) 
        std::cout << pair.first << " -> " << pair.second << '\n';
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

void sortVector(std::vector<std::pair<char,int>> &v) {
    std::pair<char,int> tmp;
    for (auto i=0;i<v.size()-1;i++)
        for (auto j=i+1;j<v.size();j++)
            if (v[i].second < v[j].second)
                {
                    tmp = v[i];
                    v[i] = v[j];
                    v[j] = tmp;
                }
}

int calcDiff(string _input)
{
    auto freqMap = std::map<char,int>();
    for (auto &c : _input)
    {
        if (freqMap.count(c) == 0)
            freqMap.insert(std::pair(c,1));
        else
            freqMap.at(c) += 1;
    }
    auto freqVec = std::vector<std::pair<char,int>>();
    for (auto &p : freqMap)
        freqVec.push_back(p);
    sortVector(freqVec);
    std::cout << "Least common char:" << (int)(freqVec.back().first) << '\n';
    return freqVec.front().second-freqVec.back().second;
}

int main(int argc, char *argv[])
{
    string line;
    std::fstream stream;
    string  polymereTmpl;
    string  output=string();
    auto insertRules=std::map<string,string>();
    string substringLeft,substringRight;
    int indexSep,lineNum = 0;

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
                    // take string right of -> as key
                    substringRight = line.substr(indexSep+2,line.length()-indexSep);
                    substringRight = trimWhiteSpace(substringRight);
                    insertRules.insert(std::make_pair(substringLeft,substringRight));
                    }
                lineNum++;
            }
        }
        stream.close();
        std::cout << polymereTmpl << '\n';
        std::cout << insertRules;

        std::stringstream inputStream(polymereTmpl);
        std::stringstream outputStream(output);
        int iterations=0;
        char key_c[3];
        while (iterations < 10 )
        {
            while (!inputStream.eof())
            {
                key_c[0] = inputStream.get();
                key_c[1] = inputStream.peek();
                if (key_c[1] != EOF)
                {
                    string key = string(key_c);
                    if (insertRules.count(key))
                    {
                        outputStream.put(key[0]);
                        outputStream << (insertRules.at(key));
                    }
                    else
                    {
                        outputStream.put(key_c[0]);
                    }
                }
                else
                {
                    outputStream.put(key_c[0]);
                }
            }
            string s = outputStream.str();

            if (outputStream.tellp() < 100)
                std::cout << s << '\n';
            else
                std::cout << outputStream.tellp() << '\n';
            inputStream.str(s);
            inputStream.seekp(0);
            inputStream.clear();
            outputStream.seekp(0);
            iterations++;
        }

        std::cout << "Result: "<< calcDiff(outputStream.str()) << '\n';
    }
}
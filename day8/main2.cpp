/* https://adventofcode.com/2021/day/8 - Part 2 */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <map>
#include <list>

#define CHAR_SPACE 32
#define CHAR_LF 10
#define CHAR_CR 13

#define LF '\n'

#define TP 1    // TOP
#define TL 2    // TOP LEFT
#define TR 4    // TOP RIGHT
#define MD 8    // MIDDLE
#define BL 16   // BOTTOM LEFT
#define BR 32   // BOTTOM RIGHT
#define BT 64   // BOTTOM

#define SE_A 1
#define SE_B 2
#define SE_C 4
#define SE_D 8
#define SE_E 16
#define SE_F 32
#define SE_G 64
#define SE_ALL 127

using std::vector;
using vStrings = vector<std::string>;

// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vStrings& obj) { 
    for(const auto& str: obj) std::cout << str << ' ';
    return os;
}
std::ostream & operator<<(std::ostream &os, const std::vector<int>& obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
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

int countBits(int num)  // use IA 486 opcode
{
    return __builtin_popcount(num);
}

std::list<int> candidatesOfLen(std::vector<int> &input , int len)
{
    auto result = std::list<int>();
    for (auto i : input)
        if (countBits(i) == len)
        {
            result.push_back(i);
        }
    return result;
}

int asSegInt(std::string input)
{
    int result = 0;
    for (auto c : input)
    {
        switch (c)
        {
            case 'a' : 
                result |= SE_A;
                break;
            case 'b' :
                result |= SE_B;
                break;
            case 'c' :
                result |= SE_C;
                break;
            case 'd' :
                result |= SE_D;
                break;
            case 'e' :
                result |= SE_E;
                break;
            case 'f' :
                result |= SE_F;
                break;
            case 'g' :
                result |= SE_G;
                break;
        }
    }
    return result;
}

std::vector<int> encoded(std::vector<std::string> input)
{
    auto result = std::vector<int>();
    for (auto string : input)
        result.push_back(asSegInt(string));
    return result;
}

/* strategy:
    - we encode each char into a bit - so we have numbers with 1 ... 7 bits set
    - this way we can easily detect common bit (bit and) or removing the same bits (bit xor)
    
    - take the codes for 7 and 1 => determines code for TP segment and candidates for TR/BR
    - take the codes for 4 and 1 => determines candidates for MD / TL 
    - build the difference between 8 and (0,6,9) - then the difference with candidates for TR/MD
      the one that has no common bits with must be the one for BL
    - then we can determine the remaining bits
*/

std::map<int,int> buildWiringMap(std::vector<std::string> inputS)
{
    auto wmap = std::map<int,int>();
    auto input = encoded(inputS);
    std::list<int> cndd,cndd2;
    int code,code2,tmp;
    std::ios_base::fmtflags f( std::cout.flags() );
    std::cout << std::hex << std::uppercase;

    std::cout << "Wiring map input: " <<  inputS << LF;
    std::cout << "Wiring map input (E): " <<   input << LF;

    // "1"
    cndd = candidatesOfLen(input,2);   // candidates for TR, BR  #('a' 'b')  => list size = 1
    code = cndd.front();
    wmap.insert(std::make_pair(TR,code));
    wmap.insert(std::make_pair(BR,code));
    // "7"
    cndd = candidatesOfLen(input,3);   // candidates for TR,BR,TP => list size = 1
    code = cndd.front();
    code ^= wmap.at(TR); // remove candidates for TR/BR => TP determined
    wmap.insert(std::make_pair(TP,code));
    std::cout << "TP:" << wmap.at(TP) << LF;
    // "4"
    cndd = candidatesOfLen(input,4);   // candidates for TL,MD,TR,BR => list size = 1
    code = cndd.front();
    code ^= wmap.at(TR); // remove TR
    wmap.insert(std::make_pair(TL,code));   // two candidates for TL
    wmap.insert(std::make_pair(MD,code));   // two candidates for MD
    // "8"
    // since this is an invariant, we can use the define
    // building the diff between 8 and 6,9,0 is exactly one segment
    // building the diff with stuff we know already, lets you determine TR/MD/BL
    cndd2 = candidatesOfLen(input,6);   // encodings for 6,9,0 
    for (auto tmp : cndd2)
    {
        int mask = SE_ALL - tmp;
        if ((wmap.at(TR) & mask) > 0) // determines TR
        {
            wmap.at(TR) &= mask;
        } else if ((wmap.at(MD) & mask) > 0)  // determines MD
        {
            wmap.at(MD) &= mask;
        } else {
            // the one having nothing in common, must be BL
            wmap.insert(std::make_pair(BL,mask));
        }
    }
    // now its easy to determine BR
    wmap.at(BR) ^= wmap.at(TR);
    // now we should know TP, TR, MD, BR
    std::cout << "TR:" << wmap.at(TR) << LF;
    std::cout << "MD:" << wmap.at(MD) << LF;
    std::cout << "BR:" << wmap.at(BR) << LF;
    std::cout << "BL:" << wmap.at(BL) << LF;
    // because MD is known, we can now determine TL
    wmap.at(TL) ^= wmap.at(MD);
    std::cout << "TL:" << wmap.at(TL) << LF;
    // now we know 6 segments - the seventh can be deducted 
    code = SE_ALL;
    for (auto pair : wmap)
        code ^=pair.second;
    wmap.insert(std::make_pair(BT,code));
    std::cout << "BT:" << wmap.at(BT) << LF;
    std::cout.flags( f );
    return wmap;
}

int translate(std::map<int,int> &wmap, std::vector<std::string> inputS)
{
    static auto segmentNrMap = std::map<int,int>();
    segmentNrMap.insert(std::make_pair(TR+BR,1));
    segmentNrMap.insert(std::make_pair(TP+TR+MD+BL+BT,2));
    segmentNrMap.insert(std::make_pair(TP+TR+MD+BR+BT,3));
    segmentNrMap.insert(std::make_pair(TL+TR+MD+BR,4));
    segmentNrMap.insert(std::make_pair(TP+TL+MD+BR+BT,5));
    segmentNrMap.insert(std::make_pair(TP+TL+MD+BR+BL+BT,6));
    segmentNrMap.insert(std::make_pair(TP+TR+BR,7));
    segmentNrMap.insert(std::make_pair(TP+TL+TR+MD+BR+BL+BT,8));
    segmentNrMap.insert(std::make_pair(TP+TL+TR+MD+BR+BT,9));
    segmentNrMap.insert(std::make_pair(TP+TL+TR+BR+BL+BT,0));

    auto input = encoded(inputS);

    std::cout << "translate map input: " <<  inputS << LF;
    std::cout << "translate map input (E): " <<   input << LF;

    int result = 0;

    for (auto code : input)
    {
        int segments=0;
        for (auto pair : wmap)
        {
            if ((pair.second & code) > 0)
                segments |= pair.first;
        }
        result *= 10;
        result += segmentNrMap.at(segments);
    }
    return result;
}


int main(int argc, char *argv[])
{
    std::string line;
    std::string substringLeft,substringRight;
    std::fstream stream;
    auto codes = std::vector<int>();
    std::map<int,int> wiringMap;

    int indexBar,code;
    
    int max=0;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,line);
            if (line.size() == 0)
                break;
            // take string left of | for building the wiring map
            indexBar = line.find_last_of('|');
            if (indexBar == std::string::npos)
            {
                std::cout << "error in input!\n";
                exit(1);
            }
            substringLeft = line.substr(0,indexBar);
            indexBar++;
            // take string right of | for decoding
            substringRight = line.substr(indexBar,line.length()-indexBar);
            {
                wiringMap = buildWiringMap(tokenizeWords(substringLeft));
                code = translate(wiringMap,tokenizeWords(substringRight));
                codes.push_back(code);
            }
        }
        stream.close();

        int sum = 0;
        for (auto num : codes)      
        {
            sum += num;
        }

        std::cout << "Result: "<< sum << '\n';
    }
}
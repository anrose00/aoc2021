/* https://adventofcode.com/2021/day/10 - Part 1 */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <stack>

#define CHAR_SPACE 32

#define CL_BR1 ')'
#define CL_BR2 ']'
#define CL_BR3 '}'
#define CL_BR4 '>'

#define OP_BRL "([{<"
#define CL_BRL ")]}>"

#define SC_BR1 3
#define SC_BR2 57
#define SC_BR3 1197
#define SC_BR4 25137

using std::vector;
using vNumbers = vector<int>;

// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vNumbers& obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
    return os;
}

int scoreOf(char c)
{
    int result=0;
    switch (c) {
        case CL_BR1:
            result = SC_BR1;
            break;
        case CL_BR2:
            result = SC_BR2;
            break;
        case CL_BR3:
            result = SC_BR3;
            break;
        case CL_BR4:
            result = SC_BR4;
            break;
    }
    return result;
}

int checkSyntax(std::string &line,std::stack<char> &stack)
{
    static auto op_brl = std::string(OP_BRL);
    static auto cl_brl = std::string(CL_BRL);
    for (auto c : line)
    {
        if (c < CHAR_SPACE) 
        {
            // end of line - stack should be empty - ignoring this error for now
            return 0;
        }
        int idx_op = op_brl.find_first_of(c);
        int idx_cl = cl_brl.find_first_of(c);

        // if stack is empty and a closing token appears => error
        if (stack.empty() && (idx_cl >= 0))
            return scoreOf(c);
        else {
            // an open token will always cause a push 
            if (idx_op >= 0)
                stack.push(c);
            else {
                // a closing token is encountered
                // find the matching open token and check if it matches stack top
                char match = op_brl[idx_cl];
                if (stack.top() == match)
                    // there was a match => pop
                    stack.pop();
                else
                    // no match => error
                    return scoreOf(c);
            }
        }
    }
    return 0;
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

int main(int argc, char *argv[])
{
    std::string line;
    std::fstream stream;
    int errorScore = 0;

    if (argc == 2)
    {

        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,line);
            if (!isOnlyWhiteSpace(line))
            {
                auto stack = std::stack<char>();
                int lineScore = checkSyntax(line,stack);
                std::cout << line << '\n';
                std::cout << "score: " << lineScore << '\n';
                errorScore += lineScore;
            }
        }
        stream.close();

        std::cout << "Result: "<< errorScore << '\n';
    }
}
/* https://adventofcode.com/2021/day/10 - Part 2 */

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


#define OP_BR1 '('
#define OP_BR2 '['
#define OP_BR3 '{'
#define OP_BR4 '<'

#define CL_BR1 ')'
#define CL_BR2 ']'
#define CL_BR3 '}'
#define CL_BR4 '>'

#define OP_BRL "([{<"
#define CL_BRL ")]}>"

#define SC_BR1 1
#define SC_BR2 2
#define SC_BR3 3
#define SC_BR4 4

using std::vector;
using vNumbers = vector<int>;

// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vector<int64_t>& obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
    return os;
}

void sortVector(std::vector<int64_t> &v) {
    int64_t tmp;
    for (auto i=0;i<v.size()-1;i++)
        for (auto j=i+1;j<v.size();j++)
            if (v[i] < v[j])
                {
                    tmp = v[i];
                    v[i] = v[j];
                    v[j] = tmp;
                }
}

int scoreOf(char c)
{
    int result=0;
    switch (c) {
        case OP_BR1:
            result = SC_BR1;
            break;
        case OP_BR2:
            result = SC_BR2;
            break;
        case OP_BR3:
            result = SC_BR3;
            break;
        case OP_BR4:
            result = SC_BR4;
            break;
    }
    return result;
}

int64_t checkSyntax(std::string &line,std::stack<char> &stack)
{
    static auto op_brl = std::string(OP_BRL);
    static auto cl_brl = std::string(CL_BRL);
    for (auto c : line)
    {
        if (c < CHAR_SPACE) 
        {
            break;
        }
        int idx_op = op_brl.find_first_of(c);
        int idx_cl = cl_brl.find_first_of(c);

        // if stack is empty and a closing token appears => error
        if (stack.empty() && (idx_cl >= 0))
            return -1;
        else {
            // an open token will always cause a push 
            if (idx_op >= 0)
                stack.push(c);
            else {
                // a closing token is encountered
                // find the matching open token and check, if it matches stack top
                char match = op_brl[idx_cl];
                if (stack.top() == match)
                    // there was a match => pop
                    stack.pop();
                else
                    // no match => error
                    return -1;
            }
        }
    }
    // end of line - stack contains the not closed brackets
    int64_t score=0;
    while (!stack.empty())
    {
        score *=5;
        score += scoreOf(stack.top());
        stack.pop();
    }
    return score;
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
    auto scores = vector<int64_t>();
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
                int64_t lineScore = checkSyntax(line,stack);
                if (lineScore > 0)
                {
                    std::cout << line << '\n';
                    std::cout << "score: " << lineScore << '\n';
                    scores.push_back(lineScore);
                }
            }
        }
        stream.close();
        std::cout << "Scores: " << scores << '\n';
        sortVector(scores);
        int middle = scores.size() / 2;

        std::cout << "Result: "<< scores[middle] << '\n';
    }
}
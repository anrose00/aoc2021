/* https://adventofcode.com/2021/day/4 - Part 2 */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <signal.h>

using std::vector;
using vNumbers = vector<int>;

#define MARKER_BIT 0x8000
#define CHAR_SPACE 32

// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vNumbers& obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
    return os;
}

class BingoMatrix {
    private:
        vector<vNumbers> matrix;
        int lastTag;

    public:
        BingoMatrix() {
            matrix = std::vector<vNumbers>();
        }
        void addRow(vNumbers _vn) {
            matrix.push_back(_vn);
        }

        void tag(int num)
        {
            lastTag = num;
            // we mark the slot by setting a certain bit
            // this is legal, because input numbers are below 255
            for(auto& row: matrix)
            {
                vNumbers::iterator it=row.begin();
                auto end=row.end();
                while (it != end)
                {
                    if (*it == num)
                        *it |= MARKER_BIT;
                    it++;
                }
            }
        }
        bool isBingo()
        {
            // check rows
            bool complete;
            for(auto& row: matrix)
            {
                complete = true;
                for (auto& slot : row)
                    if ((slot < MARKER_BIT))
                    {
                        complete = false;
                        break;
                    }
                if (complete)
                    return true;
            }
            // check columns
            if (!matrix.size()) return false;  // prevent seg fault if empty
            int columnSize = matrix[0].size();
            int rowSize = matrix.size();
            for (int x=0;x<columnSize;x++)
            {
                complete = true;
                for (int y=0;y<rowSize;y++)
                {
                    if (matrix[y][x] < MARKER_BIT)
                    {
                        complete = false;
                        break;
                    }
                }
                if (complete)
                    return true;
            }
            return complete;
        }
        int calcAocResult()
        {
            int sum=0;
            for(const auto& row: matrix) 
                for (const auto slot : row)
                    if (slot < MARKER_BIT)
                        sum += slot;
            return sum*lastTag;
        }
        // print the playfield using << operator
        friend std::ostream & operator<<(std::ostream &os, const BingoMatrix& _bm) { 
            std::ios_base::fmtflags f( os.flags() );
            
            for(const auto& row: _bm.matrix) 
            {
                for (const auto slot : row)
                {
                    os << std::setw(1);
                    if (slot < MARKER_BIT)
                    {
                        os << ' '; 
                        os << std::setw(2);
                        os <<  slot;
                    }
                    else
                    {
                        os << '*'; 
                        os << std::setw(2);
                        os << (slot&(MARKER_BIT-1));
                    }
                }
                os << '\n';
            }
            os.flags( f );

            return os;
        }
};

bool isOnlyWhiteSpace(std::string _str)
{
    for (const auto& c : _str)
    {
        if (c > CHAR_SPACE )
            return false;
    }
    return true;
}
// tokenize input string using delim and answer a vector of integers
vector<int> &tokenize(const std::string &input, char delim)
{
    vNumbers *tokens = new vNumbers();
    int next,last = 0;
    while ((next = input.find(delim,last)) != std::string::npos)
    {
        tokens->push_back(stoi(input.substr(last,next-last),nullptr,10));
        last = next + 1;
    }
    return *tokens;    
}
// tokenize input string by using count chars
vector<int> &tokenizeCount(const std::string &input, int count)
{
    vNumbers *tokens = new vNumbers();
    int next=count,last = 0;
    while (last < input.size())
    {
        tokens->push_back(stoi(input.substr(last,next-last),nullptr,10));
        last = next;
        next = last + count;
    }
    return *tokens;    
}

int main(int argc, char *argv[])
{
    std::string numberString;
    auto listPlayFields = vector<BingoMatrix*>();

    BingoMatrix* curPlayField=nullptr;

    std::fstream stream;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        // read first line with drawn numbers
        std::getline(stream,numberString);
        vNumbers random = tokenize(numberString,',');

        std::cout << random << '\n';

        // read the list of bingo playfields
        while (!stream.eof())
        {
            numberString.clear();
            std::getline(stream,numberString);
            if (isOnlyWhiteSpace(numberString))
            {
                if (curPlayField) listPlayFields.push_back(curPlayField);
                curPlayField = new BingoMatrix();
            }
            else
            {
                curPlayField->addRow(tokenizeCount(numberString,3));
            }
        }
        listPlayFields.push_back(curPlayField);
        stream.close();

        auto it = random.begin();
        auto end = random.end();
        while (it != end)
        {
            // pull a number from the list and tag the number on each playfield
            std::cout << "### " << *it << " ###\n";
            for (auto il=listPlayFields.begin();il!=listPlayFields.end();il++)
            {
                (*il)->tag(*it);
                std::cout << '\n'<< **il ;
            }
            // check each playfield if it has reached bingo status - remove those from list
            auto il = listPlayFields.begin();
            while (il != listPlayFields.end())
            {
                if (((*il)->isBingo()) && (listPlayFields.size() > 1))
                {
                    delete *il;
                    il = listPlayFields.erase(il);
                }
                else
                    il++;
            }
            // if only one playfield is left and is has bingo status, we are done
            if (listPlayFields.size() == 1 && listPlayFields.front()->isBingo())
            {
                std::cout << "Result: " << listPlayFields.front()->calcAocResult() << '\n';
                delete listPlayFields.front();
                listPlayFields.clear();
                break;
            }
            else
                // draw next number and repeat
                it++;
        }
    }
}
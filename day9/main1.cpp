/* https://adventofcode.com/2021/day/9 - Part 1 */

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

class Grid {
    private:
        vector<vector<uint8_t>> grid;
    public:
        Grid() {
            grid = vector<vector<uint8_t>>();
        }

        void addLine(std::string line) {
            auto internal = vector<uint8_t>();
            for (auto c : line)
                if (c >= '0')
                    internal.push_back(c-'0');
            grid.push_back(internal);
        }
        void printLine(std::ostream &os, int y) const {
            if (y >= 0 && y < grid.size())
            {
                auto line = grid[y];
                for (auto c : line)
                    os << (char)(c + '0');
            }
        }
        int sizeY() const {
            return grid.size();
        }
        int sizeX() const {
            if (grid.size() == 0)
                return 0;
            return grid[0].size();
        }
        uint8_t element(int x, int y) {
            return grid[y][x];
        }
        bool hasOnlyHigherNeighbours(int x, int y) {
            auto center = element(x,y);
            // element above of x,y
            if ( y > 0 ) 
                if ( element(x,y-1) <= center ) return false;
            // element left of x,y
            if ( x > 0 )
                if ( element(x-1,y) <= center ) return false;
            // element below of x,y
            if ( y < (sizeY()-1))
                if ( element(x,y+1) <= center ) return false;
            // element right of x,y
            if ( x < (sizeX()-1)) 
                if ( element(x+1,y) <= center ) return false;
            return true;
        }
        std::vector<uint8_t> collectLowSpots()
        {
            auto result = std::vector<uint8_t>();
            for (auto y = 0; y < sizeY(); y++)
                for (auto x = 0; x < sizeX(); x++)
                    if (hasOnlyHigherNeighbours(x,y))
                        result.push_back(element(x,y));
            return result;
        }
        int riskLevels() {
            auto lowSpots = collectLowSpots();
            int sum=0;
            for (auto i : lowSpots) sum += (i+1);
            return sum;
        }
        friend std::ostream & operator<<(std::ostream &os, const Grid&  g) { 
            for (auto y=0; y < g.sizeY(); y++)
            {
                g.printLine(os,y);
                os << '\n';
            }
            return os;
        }        
};

int main(int argc, char *argv[])
{
    std::string numberString;
    std::fstream stream;
    Grid grid = Grid();

    int max=0;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,numberString);
            if (!isOnlyWhiteSpace(numberString))
            {
                grid.addLine(numberString);
            }
        }
        stream.close();
        std::cout << grid << '\n';
        std::cout << "Result: "<< grid.riskLevels() << '\n';
    }
}
/* https://adventofcode.com/2021/day/11 - Part 1 */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

#define CHAR_SPACE 32

#define FLASH_LVL 10

using std::vector;
using vNumbers = vector<int>;

// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vNumbers &obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
    return os;
}

std::ostream & operator<<(std::ostream &os, std::vector<std::pair<int,int>> &obj) { 
    for(const std::pair<int,int> &p: obj) 
        std::cout << '[' << p.first << ',' << p.second << ']';
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
                    if (c < FLASH_LVL)
                        os << (char)(c + '0');
                    else
                        os << '*';
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
        int numElements()
        {
            return sizeX()*sizeY();
        }
        uint8_t element(int x, int y) {
            return grid[y][x];
        }
        bool isValidPosition(int x, int y)
        {
            return (x >=0 && y >= 0 && x < sizeX() && y < sizeY());
        }
        bool hasMaxEnergyAt(int x, int y)
        {
            return element(x,y) >= FLASH_LVL;
        }
        uint8_t elementInc(int x, int y, int v) 
        {
            grid[y][x] += v;
            return grid[y][x];
        }
        bool isInFlashList(std::vector<std::pair<int,int>> &list,int x, int y)
        {            
            for (auto e : list)
                if ((e.first == x) && (e.second == y)) 
                    return true;
            return false;
        }
        void increaseEnergyLevel()
        {
            for (auto y = 0; y < sizeY(); y++)
                for (auto x = 0; x < sizeX(); x++)
                    elementInc(x,y,1);
        }
        void distributeEnergy() 
        {
            auto flashList1 = std::vector<std::pair<int,int>>();
            for (auto y = 0; y < sizeY(); y++)
                for (auto x = 0; x < sizeX(); x++)
                {
                    auto center = element(x,y);
                    if (center >= FLASH_LVL)
                        flashList1.push_back(std::make_pair(x,y));
                }
            auto flashList2 = std::vector<std::pair<int,int>>();
            while (!flashList1.empty())
            {
                
                // std::cout << "FL1: " << flashList1 << '\n';
                for (std::pair<int,int> p : flashList1)
                    distributeEnergyAt(p.first,p.second,flashList1, flashList2);
                // std::cout << "FL2: " << flashList2 << '\n';
                flashList1.clear();
                flashList1.swap(flashList2);
            }
        }
        void distributeEnergyAt(int x, int y, std::vector<std::pair<int,int>> &list1, std::vector<std::pair<int,int>> &list2) 
        {
            for (auto dx=-1;dx<=1;dx++)
                for (auto dy=-1;dy<=1;dy++)
                {
                    auto nx = x + dx;
                    auto ny = y + dy;
                    if (isValidPosition(nx,ny) && (dx != 0 || dy != 0) && !hasMaxEnergyAt(nx,ny))
                    {
                        if (elementInc(nx,ny,1) >= FLASH_LVL && !isInFlashList(list1,nx,ny))
                            list2.push_back(std::make_pair(nx,ny));;
                    }
                }
        }
        int countFlashesReset()
        {
            int result=0;
            for (auto y = 0; y < sizeY(); y++)
                for (auto x = 0; x < sizeX(); x++)
                    if (grid[y][x] >= FLASH_LVL)
                    {
                        result++;
                        grid[y][x] = 0;
                    }
            return result;
        }
        int calcFlashes()
        {
            int result=0;
            increaseEnergyLevel();  // add 1 energy to each element
            // std::cout << "after inc energy\n" << *this << '\n';
            distributeEnergy();     // if element has >9 energy distribute to neighbors
            // std::cout << "after distr energy\n" << *this << '\n';
            result += countFlashesReset();  // count the flashes this step and reset
            return result;
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
        int flashCount=0;
        for (auto step=0;step<100;step++)
        {
            int flashThisStep = grid.calcFlashes();
            std::cout << "Step: " << step << " - " << flashThisStep << '\n';
            flashCount += flashThisStep;
        }
        
        std::cout << "Result: "<< flashCount << ' ' << '\n';
    }
}
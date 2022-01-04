/* https://adventofcode.com/2021/day/13 - Part 2 */

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
// read an integer value in input string at index pos
int getNumAt(const std::string &input, int pos)
{
    int inputSize = input.size();
    int next=pos,last = pos;
    int num;
    while (isdigit(input[next]) && next<inputSize) next++;
    num = stoi(input.substr(last,next-last),nullptr,10);
    return num;
}

int getMax(int start, const vNumbers _vn)
{
    int res=start;
    for (auto e : _vn)
        res = std::max(res,e);
    return res;
}

struct Point {
    int x;
    int y;

    Point(int _x,int _y) {
        x = _x;
        y = _y;
    }
};

struct Fold {
    int x;
    int y;

    Fold(int _x,int _y) {
        x = _x;
        y = _y;
    }
    bool isYFold() const {
        return x < 0;
    }
    bool isXFold() const {
        return y < 0;
    }
};
// friend function to print fold object easily
std::ostream & operator<<(std::ostream &os, const Fold& _f) { 
    if (_f.isYFold())
        os << "Fold(y)=" << _f.y;
    else
        os << "Fold(x)=" << _f.x;
    return os;
}
// friend functino to print a vector of fold objects easily
std::ostream & operator<<(std::ostream &os, const vector<Fold>& obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
    return os;
}

class Grid {
    private:
        vector<vector<uint8_t>> grid;
        int dim_x;
        int dim_y;
    public:
        Grid(int _dimx, int _dimy) {
            dim_x = _dimx;
            dim_y = _dimy;
            for (auto i=0;i<=dim_y;i++)
            {
                vector<uint8_t> row = vector<uint8_t>();
                row.insert(row.begin(),dim_x+1,0);
                grid.push_back(row);
            }
        }
        void setMarker(Point &_p)
        {
            grid[_p.y][_p.x] = 1;
        }
        int numVisibleDots()
        {
            int count=0;
            for (auto y=0;y<=dim_y;y++)
                for (auto x=0;x<=dim_x;x++)
                    count += grid[y][x];
            return count;
        }
        // fold the grid at y offset 
         void foldY(int _y)
        {
            int y1,y2,x;
            for (y1=_y+1,y2=_y-1;y1<=dim_y;y1++,y2--)
                for (x=0;x<=dim_x;x++)
                {
                    grid[y2][x] |= grid[y1][x];
                }
            dim_y = _y - 1;
        }
        // fold the grid at x offset
        void foldX(int _x)
        {
            int x1,x2,y;
            for (x1=_x+1,x2=_x-1;x1<=dim_x;x1++,x2--)
                for (y=0;y<=dim_y;y++)
                {
                    grid[y][x2] |= grid[y][x1];
                }
            dim_x = _x - 1;
        }
        friend std::ostream & operator<<(std::ostream &os, const Grid& g) { 
            for (auto y=0;y<=g.dim_y;y++)
            {
                for (auto x=0;x<=g.dim_x;x++)
                    os << (g.grid[y][x]==0?'.':'#');
                os << '\n';
            }
            return os;
        }        
};

int main(int argc, char *argv[])
{
    std::string line;
    std::fstream stream;
    auto listPoints = vector<Point>();
    auto foldOperations = vector<Fold>();

    int max_x=0;
    int max_y=0;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            vNumbers tl;
            std::getline(stream,line);
            if (!isOnlyWhiteSpace(line) && line.size()> 0 )
            {
                // parsing the lines with points and determine the dimensions
                if (isdigit(line[0]))
                {
                    tl = tokenizeNumbers(line);
                    listPoints.push_back(Point(tl[0],tl[1]));
                    max_x = std::max(max_x,tl[0]);
                    max_y = std::max(max_y,tl[1]);
                }
                else
                {
                    // parsing the lines with fold operations
                    int num;
                    std::string s = std::string("x=");
                    int pos = line.find(s);
                    if (pos != std::string::npos)
                    {
                        num = getNumAt(line,pos+2);
                        foldOperations.push_back(Fold(num,-1));
                    }
                    else
                    {
                        std::string s = std::string("y=");
                        int pos = line.find(s);
                        if (pos != std::string::npos)
                        {
                            num = getNumAt(line,pos+2);
                            foldOperations.push_back(Fold(-1,num));
                        }
                        else
                        {
                            std::cout << "Error parsing folds!";
                            exit(1);
                        }
                    }
                }
            }
        }
        stream.close();
        // walk the list of points and set markers
        Grid g = Grid(max_x,max_y);
        for (auto p : listPoints )
        {
            g.setMarker(p);
        }
        std::cout << g << '\n';
        std::cout << foldOperations << '\n';
        // perform all fold operations and print the grid
        for (auto f : foldOperations)
        {
            if (f.isYFold())
                g.foldY(f.y);
            else
                g.foldX(f.x);
        }
        std::cout << g << '\n';
        std::cout << "Result: "<< g.numVisibleDots() << '\n';
    }
}
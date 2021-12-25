/* https://adventofcode.com/2021/day/5 - Part 2 */

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

struct GeLine {
    int x0;
    int y0;
    int x1;
    int y1;

    GeLine(int _x0,int _y0,int _x1,int _y1) {
        x0 = _x0;
        y0 = _y0;
        x1 = _x1;
        y1 = _y1;
    }
};

// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vNumbers& obj) { 
    for(const auto& num: obj) std::cout << num << ' ';
    return os;
}

std::ostream & operator<<(std::ostream &os, const GeLine& _gl) { 
    os << '(' << _gl.x0 << ',' << _gl.y0 << ')';
    os << " -> ";
    os << '(' << _gl.x1 << ',' << _gl.y1 << ')';
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
vector<int> &tokenizeNumbers(const std::string &input)
{
    vNumbers *tokens = new vNumbers();
    int next=0,last = 0;
    int inputSize = input.size();
    while (next<inputSize)
    {
        while (isdigit(input[next]) && next<inputSize) next++;
        tokens->push_back(stoi(input.substr(last,next-last),nullptr,10));
        while (!isdigit(input[next]) && next<inputSize) next++;
        last = next;
    }
    return *tokens;    
}

int getMax(int start, const vNumbers _vn)
{
    int res=start;
    for (auto e : _vn)
        res = std::max(res,e);
    return res;
}

class Grid {
    private:
        vector<vector<uint8_t>> grid;
        int dim;
    public:
        Grid(int _dim) {
            dim = _dim;
            for (auto i=0;i<=_dim;i++)
            {
                vector<uint8_t> row = vector<uint8_t>();
                row.insert(row.begin(),_dim+1,0);
                grid.push_back(row);
            }
        }
        void makeLine(const GeLine _gl)
        {
            if (_gl.x0 == _gl.x1)
            {
                int y0,y1;
                if (_gl.y1 < _gl.y0)
                {
                    y0 = _gl.y1;
                    y1 = _gl.y0;
                }
                else
                {
                    y0 = _gl.y0;
                    y1 = _gl.y1;
                }
                int x = _gl.x0;
                for (auto y=y0;y<=y1;y++)
                    grid[y][x] += 1;
            }
            else if (_gl.y0 == _gl.y1)
            {
                int x0,x1;
                if (_gl.x1 < _gl.x0)
                {
                    x0 = _gl.x1;
                    x1 = _gl.x0;
                }
                else
                {
                    x0 = _gl.x0;
                    x1 = _gl.x1;
                }
                int y = _gl.y0;
                for (auto x=x0;x<=x1;x++)
                    grid[y][x] += 1;
            }
            else {
                /* The special cases above could also be adapted to a pattern 
                   like this. Perhaps use generics? */
                int dx = _gl.x0 - _gl.x1;
                int dy = _gl.y0 - _gl.y1;
                if (abs(dx / dy) == 1)
                {
                    dx = (dx > 0)?-1:1;
                    dy = (dy > 0)?-1:1;
                    int x = _gl.x0;
                    int y = _gl.y0;
                    int end = _gl.x1+dx;
                    while (x != end)
                    {
                        grid[y][x] += 1;
                        x += dx;
                        y += dy;
                    }
                }
            }
        }
        int countHotSpots()
        {
            int count=0;
            for (auto y=0;y<=dim;y++)
                for (auto x=0;x<=dim;x++)
                    if (grid[y][x] > 1) count++;
            return count;
        }
        friend std::ostream & operator<<(std::ostream &os, const Grid& g) { 
            for (auto y=0;y<=g.dim;y++)
            {
                for (auto x=0;x<=g.dim;x++)
                    os << (g.grid[y][x]==0?'.':char('0'+g.grid[y][x]));
                os << '\n';
            }
            return os;
        }        
};

int main(int argc, char *argv[])
{
    std::string numberString;
    std::fstream stream;

    vector<GeLine> listPoints;
    int max=0;
    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            vNumbers tl;
            std::getline(stream,numberString);
            if (!isOnlyWhiteSpace(numberString))
            {
                tl = tokenizeNumbers(numberString);
                listPoints.push_back(GeLine(tl[0],tl[1],tl[2],tl[3]));
                max = getMax(max,tl);
            }
        }
        stream.close();
        char c;
        Grid g = Grid(max);
        for (auto gl : listPoints )
        {
            g.makeLine(gl);
            // std::cout << "###" << gl << "###" << '\n';
            // std::cout << g << '\n';
            // std::cin >> c;
        }
        std::cout << "Result: "<< g.countHotSpots() << '\n';
    }
}
/* https://adventofcode.com/2021/day/15 - Part 2 (unfinished) */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <climits>
#include <list>

#define CHAR_SPACE 32

using std::vector;
using std::list;
using vNumbers = vector<int>;


// debug helper to print vector using <<
std::ostream & operator<<(std::ostream &os, const vNumbers &obj) { 
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

struct Coord {
    int x;
    int y;

    Coord(int _x, int _y) {
        x = _x;
        y = _y;
    }
    bool operator == (const Coord & _c) {
        return x == _c.x && y == _c.y;
    }
};

std::ostream & operator<<(std::ostream &os, list<Coord> &list) { 
    for( auto c : list) 
        std::cout << '[' << c.x << ',' << c.y << ']';
    return os;
}

bool contains(list<Coord> & listOfElements, const Coord & element)
{
    // Find the iterator if element in list
    auto it = listOfElements.begin();
    while (it != listOfElements.end())
    {
        if (*it == element)
            break;
        it++;
    }
    //return if iterator points to end or not. If it points to end then it means element
    // does not exists in list
    return it != listOfElements.end();
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
        void copyGridTo(int dx, int dy, int lx, int ly, int add)
        {
            for (auto y = 0; y < ly; y++)
                for (auto x = 0; x < lx; x++)
                {
                    int v;
                    v = ((grid[y][x] - 1 + add) % 9) + 1;
                    grid[y+dy][x+dx] = v;
                }
        }
        void enlargeBy(int f)
        {
            int cur_x=sizeX(),cur_y=sizeY();
            int new_x=cur_x*f,new_y=cur_y*f;
            for (auto y=0;y<cur_y;y++)
                grid[y].resize(new_x,0);
            for (auto y=cur_y;y<new_y;y++)
                grid.push_back(vector<uint8_t>(new_x,0));

            for (auto fy=0;fy<f;fy++)
                for (auto fx=0;fx<f;fx++)
                {
                    if (fx != 0 || fy != 0)
                        copyGridTo(cur_x*fx,cur_y*fy,cur_x,cur_y,fx+fy);
                }
        }

        Coord findShortestEdge(vector<vector<int>> const &dist, list<Coord> const &pqueue)
        {
            int min = INT_MAX;
            Coord min_c = Coord(-1,-1);
            for (auto c : pqueue)
            {
                int d = dist[c.x][c.y];
                if (d < min)
                {
                    min = d;
                    min_c = c;
                }
            }
            return min_c;
        }

        int dijkstra()
        {
            auto dist = vector<vector<int>>();
            auto pqueue = list<Coord>();

            for (auto y=0; y < sizeY(); y++)
            {
                dist.push_back(vector<int>(sizeX(),INT_MAX));
                for (auto x=0; x < sizeX(); x++)
                {
                    pqueue.push_back(Coord(x,y));
                }
            }
            dist[0][0] = 0;

            while (!pqueue.empty())
            {
                Coord min_c = findShortestEdge(dist,pqueue);

                if ((min_c.x == sizeX()-1) && (min_c.y == sizeY()-1))
                    return dist[min_c.x][min_c.y];

                if ((min_c.x < sizeX()-1) && contains(pqueue,Coord(min_c.x+1,min_c.y)) )
                {
                    int distRight = dist[min_c.x][min_c.y] + element(min_c.x+1,min_c.y);

                    if (distRight < dist[min_c.x+1][min_c.y] )
                        dist[min_c.x+1][min_c.y] = distRight;
                }
                if ((min_c.y < sizeY()-1) && contains(pqueue,Coord(min_c.x,min_c.y+1)) )
                {
                    int distRight = dist[min_c.x][min_c.y] + element(min_c.x,min_c.y+1);

                    if (distRight < dist[min_c.x][min_c.y+1] )
                        dist[min_c.x][min_c.y+1] = distRight;
                }
                pqueue.remove(min_c);
                std::cout << pqueue.size() << '\n';
            }
            return INT_MAX;
        }
        void printLine(std::ostream &os, int y) const {
            if (y >= 0 && y < grid.size())
            {
                for (auto c : grid[y])
                    os << (char)(c + '0');
            }
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
        grid.enlargeBy(5);
        std::cout << grid << '\n';
       
        std::cout << "Result: "<<  grid.dijkstra() << '\n';
    }
}
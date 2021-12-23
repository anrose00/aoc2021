/* https://adventofcode.com/2021/day/1 Part 1 */

/* Read a sequence of numbers from stdin
   count the numbers that are bigger than 
   their predecessor
*/

#include <istream>
#include <iostream>

int main(int argc, char *argv[])
{
    bool firstTime=true;
    int current,previous,count=0;
    while (!std::cin.eof())
    {
        if (!firstTime)
        {
            previous = current;
            firstTime = false;
        }
        std::cin >> current;
        if (!firstTime && (current > previous))
            count++;
    }
    std::cout << count << '\n';
}

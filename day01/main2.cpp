/* https://adventofcode.com/2021/day/1 Part 2 */

/* Walk a list of numbers. Build a sum of three elements for a
   sliding window of 3. Answer the number, where the sum of the 
   previous window is greater than the current.
*/

#include <iostream>
#include <list>

#define WIND_SIZE 3

int main(int argc, char *argv[])
{
    bool firstTime=true;
    int readVal,current=-1,previous=-1,count=0;
    std::list<int> queue;
    while (!std::cin.eof())
    {
        std::cin >> readVal;
        queue.push_front(readVal);
        if (queue.size() == WIND_SIZE)
        {
            int sum=0;
            for (auto it=queue.begin();it != queue.end(); it++ )
                sum += *it;
            queue.pop_back();
            if (!firstTime)
                previous = current;
            current = sum;
            if (!firstTime && (current > previous))
                count++;
            firstTime = false;
        }
    }
    std::cout << "result: " << count << '\n';
}

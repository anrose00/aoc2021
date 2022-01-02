/* https://adventofcode.com/2021/day/12 - Part 1 */

#include <istream>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <list>

#define CHAR_SPACE 32
#define CHAR_LF 10
#define CHAR_CR 13

bool isOnlyWhiteSpace(std::string _str)
{
    for (const auto& c : _str)
    {
        if (c > CHAR_SPACE )
            return false;
    }
    return true;
}

bool isWhiteSpace(char c)
{
    return c == CHAR_SPACE || c == CHAR_LF || c == CHAR_CR;
}

std::string trimWhiteSpace(std::string _input)
{
    int index=_input.size()-1;
    while (index >= 0 && isWhiteSpace(_input[index])) index--;
    return _input.substr(0,index+1);
}

class Node {
    private:
        std::list<Node*> v_edges;
        std::string     v_name;
    public:
        Node(std::string _name) {
            v_edges = std::list<Node*>();
            v_name = _name;
        }
        ~Node()
        {
            v_edges.clear();
        }
        void connectTo(Node * _node)
        {
            v_edges.push_back(_node);
        }
        bool mayVisitOnce()
        {
            return islower(v_name[0]);
        }
        bool mayVisitMultiple()
        {
            return isupper(v_name[0]);
        }
        std::list<Node*> const &edges(){
            return v_edges;
        }
        std::string &name()
        {
            return v_name;
        }
};

std::ostream & operator<<(std::ostream &os, const  std::list<Node*> &list) { 
    for(const auto node: list) 
        std::cout << node->name() << ' ';
    return os;
}

class Graph {
    private:
        std::list<Node*> nodes;
        Node *start;
        Node *end;
        int pathCount;

    public:
        Graph() {
            nodes = std::list<Node*>();
        }
        ~Graph() {
            for (auto n : nodes)
                delete n;
            nodes.clear();
        }

        Node *findNodeNamed(std::string _name)
        {
            for (auto n : nodes)
                if (n->name().compare(_name) == 0)
                    return n;
            return nullptr;
        }

        void buildNodesFromString(std::string _line)
        {
            int index;
            std::string left,right;
            Node *leftNode, *rightNode;
            index = _line.find_first_of('-');
            left = _line.substr(0,index);
            right = _line.substr(index+1,_line.size()-index);
            right = trimWhiteSpace(right);
            // try to find an existing node with that name
            leftNode = findNodeNamed(left);
            // if not found, create a new one
            if (!leftNode) 
            {
                leftNode = new Node(left);
                addNode(leftNode);
            }
            // same for right part
            rightNode = findNodeNamed(right);
            if (!rightNode)
            {
                rightNode = new Node(right);
                addNode(rightNode);
            }
            if (left.compare("start") == 0)  start = leftNode;
            if (right.compare("start") == 0)  start = rightNode;
            if (left.compare("end") == 0)  end = leftNode;
            if (right.compare("end") == 0)  end = rightNode;

            connectNodes(leftNode,rightNode);
        }
        void addNode(Node * _node)
        {
            nodes.push_back(_node);
        }
        void connectNodes(Node * _node1, Node * _node2)
        {
            _node1->connectTo(_node2);
            _node2->connectTo(_node1);
        }
        bool isIncluded(Node *node, std::list<Node*> &list )
        {
            for (auto n : list)
                if (node == n) return true;
            return false;
        }
        bool mayVisit(Node *node, std::list<Node*> &visited)
        {
            return (!isIncluded(node,visited)) || (node->mayVisitMultiple());
        }
        void traverse(Node *next, std::list<Node*> &visited)
        {
            visited.push_back(next);
            if (next == end)
            {
                std::cout << "Found path: " << visited << '\n';
                pathCount ++;
            }
            else
            {
                for (auto edge : next->edges())
                {
                    if (mayVisit(edge,visited))
                    {
                        traverse(edge,visited);
                    }
                }
            }
            visited.pop_back();
            return;
        }
        int findPaths()
        {
            auto visited = std::list<Node*>();
            pathCount = 0;
            traverse(start,visited);
            return pathCount;
        }
};

int main(int argc, char *argv[])
{
    std::string line;
    std::fstream stream;
    Graph graph = Graph();

    if (argc == 2)
    {
        stream.open(argv[1],std::ios::in);
        while (!stream.eof())
        {
            std::getline(stream,line);
            if (!isOnlyWhiteSpace(line))
            {
                graph.buildNodesFromString(line);
            }
        }
        stream.close();

        int distinctPaths = graph.findPaths();
        std::cout << "Result: "<< distinctPaths << ' ' << '\n';
    }
}
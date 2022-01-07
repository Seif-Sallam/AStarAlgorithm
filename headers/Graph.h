#pragma once
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>


// Inspired By The One Lone Coder video: https://www.youtube.com/watch?v=icZj67PTFhc&ab_channel=javidx9
// A grid based Graph Class

struct Node {
    bool obstacle = false;          // is it an obstacle node or not
    bool visited = false;           // is it visited or not 
    float globalGoal;               // distance to the goal
    float localGoal;                // Local distance
    int x;                          // Node position
    int y;
    std::vector<Node*> neighbours;  // connected nodes
    Node* parent = nullptr;                   // parent node
};

class Graph
{
public:
    Graph(bool diagonalMoves = false);
    void Solve_AStar();

    int GetHeight();
    int GetWidth();

    Node* GetANode(int x, int y);
    
    Node* nodeStart;
    Node* nodeEnd;
private:
    int m_iHeight;
    int m_iWidth;
    Node* m_nodes;
};


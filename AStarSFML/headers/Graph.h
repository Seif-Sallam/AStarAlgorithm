#pragma once
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>

// Inspired By The One Lone Coder video: https://www.youtube.com/watch?v=icZj67PTFhc&ab_channel=javidx9
// A grid based Graph Class

struct Node
{
    bool obstacle = false; // is it an obstacle node or not
    bool visited = false;  // is it visited or not
    float globalGoal;      // distance to the goal
    float localGoal;       // Local distance
    int x;                 // Node position
    int y;
    std::vector<Node *> neighbours; // connected nodes
    Node *parent = nullptr;         // parent node
};

class Graph
{
public:
    Graph(bool diagonalMoves = false, int width = 20, int height = 20);
    void Solve_AStar();
    inline void SetWidthAndHeight(int width, int height)
    {
        m_iWidth = width;
        m_iHeight = height;
        PrepareGraph(diagonalMoves);
    }
    int GetHeight() const { return m_iHeight; }
    int GetWidth() const { return m_iWidth; };
    void PrepareGraph(bool diagonalMoves);

    Node *GetANode(int x, int y);

    Node *nodeStart;
    Node *nodeEnd;

    ~Graph();
private:
    int m_iHeight;
    int m_iWidth;
    bool diagonalMoves;
    Node *m_nodes;
};

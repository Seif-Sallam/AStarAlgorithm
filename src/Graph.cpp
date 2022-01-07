#include "Graph.h"

Graph::Graph(bool diagonalMoves)
{
    //Those are felxible but for the sake of this demonstration
    m_iWidth = 16;
    m_iHeight = 16;

    // Allocating the memory for the nodes array
    m_nodes = new Node[m_iHeight * m_iWidth];

    // Since it is a grid we can go through the grid using a 2 dimensional loop.
    for (int x = 0; x < m_iWidth; x++)
        for (int y = 0; y < m_iHeight; y++)
        {
            // Setting the default values for all the nodes 
            int index = y * m_iWidth + x;           // The index of each node
            
            m_nodes[index].obstacle = false;           // Setting it to be regular nodes
            m_nodes[index].visited = false;        // Setting it to be unvisited
            m_nodes[index].parent = nullptr;       // No parents so far
            m_nodes[index].x = x;                  // The position in 2D space
            m_nodes[index].y = y;                  

            int upIndex = (y - 1) * m_iWidth + x;   // The indexes of all the neighbour nodes
            int downIndex = (y + 1) * m_iWidth + x; 
            int leftIndex = y * m_iWidth + (x - 1); 
            int rightIndex = y * m_iWidth + (x + 1);

            // Setting the neighbour nodes
            if (y > 0)
                m_nodes[index].neighbours.push_back(&m_nodes[upIndex]);
            if(y < m_iHeight - 1)
                m_nodes[index].neighbours.push_back(&m_nodes[downIndex]);
            if (x > 0)
                m_nodes[index].neighbours.push_back(&m_nodes[leftIndex]);
            if (x < m_iWidth - 1)
                m_nodes[index].neighbours.push_back(&m_nodes[rightIndex]);

            if (diagonalMoves) {
                int upRight = (y - 1) * m_iWidth + (x + 1);
                int upLeft = (y - 1) * m_iWidth + (x - 1);
                int downLeft = (y + 1) * m_iWidth + (x - 1);
                int downRight = (y + 1) * m_iWidth + (x + 1);
                
                if (x > 0 && y > 0 && y < m_iHeight - 1 && x < m_iWidth - 1)
                {
                    m_nodes[index].neighbours.push_back(&m_nodes[upRight]);
                    m_nodes[index].neighbours.push_back(&m_nodes[downRight]);
                    m_nodes[index].neighbours.push_back(&m_nodes[upLeft]);
                    m_nodes[index].neighbours.push_back(&m_nodes[downLeft]);
                }

            }
        }

    // Setting the starting node and the end node to something in the middle of the screen
    nodeStart = &m_nodes[(m_iHeight / 2) * m_iWidth + 1];
    nodeEnd = &m_nodes[(m_iHeight / 2) * m_iWidth + (m_iWidth - 2)];

}

void Graph::Solve_AStar()
{
    // We should make sure that all the values are default values
    for(int x = 0; x < m_iWidth; x++)
        for (int y = 0; y < m_iHeight; y++)
        {
            int index = y * m_iWidth + x;
            m_nodes[index].visited = false;
            m_nodes[index].parent = nullptr;
            m_nodes[index].globalGoal = INFINITY;
            m_nodes[index].localGoal = INFINITY;
        }

    // A lamda that will calculate the distance between the nodes

    auto distance = [](const Node* a, const Node* b)
    {
        return sqrtf((a->x - b->x) * (a->x - b->x) + (a->y - b->y) * (a->y - b->y));
    };
   
    // A lamda that will calculate the heuristic distance between the node and the end node
    auto heuristic = [distance](Node* a, Node* b)
    {
        return distance(a, b);
    };

    
    // Updating the values for the starting node

    Node* currentNode = nodeStart;              // The pointer that will go through all the graph
    nodeStart->localGoal = 0.f;
    nodeStart->globalGoal = heuristic(nodeStart, nodeEnd);

    // Making a list of the not yet tested nodes
    std::list<Node*> notTestedNodes;
    notTestedNodes.push_back(nodeStart);

    // Here starts everyhting to update
    while (!notTestedNodes.empty() && currentNode != nodeEnd)
    {
        // Sorting the nodes to the nearest possible answer
        notTestedNodes.sort([](const Node* lhs, const Node* rhs) {return lhs->globalGoal < rhs->globalGoal; });

        // Popping the node from the list if it is visited
        while (!notTestedNodes.empty() && notTestedNodes.front()->visited)
            notTestedNodes.pop_front();
        
        // Rechecking if it became empty or not
        if (notTestedNodes.empty())
            break;
        
        currentNode = notTestedNodes.front();
        currentNode->visited = true;
        
        // For every neighbour you update each one
        for (auto& n : currentNode->neighbours)
        {
            // Adding whatever is valid to the list
            if (!n->visited && !n->obstacle)
                notTestedNodes.push_back(n);

            // Seeing the possible LOWER Local goal
            float goal = currentNode->localGoal + distance(currentNode, n);

            // Updating the node's info
            if (goal < n->localGoal)
            {
                n->parent = currentNode;
                n->localGoal = goal;
                
                n->globalGoal = n->localGoal + heuristic(n, nodeEnd);
            }
        } // end of the for loop
    } // end of the while loop
} // End of the Solve_AStar method

int Graph::GetHeight()
{
    return m_iHeight;
}

int Graph::GetWidth()
{
    return m_iWidth;
}

Node* Graph::GetANode(int x, int y)
{
    // Making sure it is within the boundries before returning it 
    if (x >= 0 && x <= m_iWidth - 1 && y >= 0 && y <= m_iHeight)
        return &m_nodes[y * m_iWidth + x];
    return nullptr;
}

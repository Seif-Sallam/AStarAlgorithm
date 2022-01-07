#pragma once
#include "SFML/Graphics.hpp"
#include "Graph.h"
#include <iostream>

class Demonstration
{
public:
	Demonstration(uint32_t width, uint32_t  height);
	void Run();
	~Demonstration();
private:
	void EventHandler();
	void Update();
	void Draw();

private:
	sf::RenderWindow* m_window;
	sf::Event* m_event;
	Graph* m_graph;
	
	sf::View m_view;
	sf::Vector2f m_fOffset;
	sf::Vector2f m_fTileSize;
	sf::Vector2f m_fCellSize;

	int gHeight;
	int gWidth;

	sf::RectangleShape* m_nodesShapes;
	sf::VertexArray m_connections;
	sf::Text details[6];
	sf::Font m_font;

};

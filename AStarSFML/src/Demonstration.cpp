#include "Demonstration.h"

Demonstration::Demonstration(uint32_t sWidth, uint32_t sHeight)
{
	m_window = new sf::RenderWindow(sf::VideoMode(sWidth, sHeight), "A Star Algorithm Demo", sf::Style::Titlebar);
	m_event = new sf::Event;
	ImGui::SFML::Init(*m_window);
	bool diagonal = true;
	m_graph = new Graph(diagonal);
	m_fCellSize = {32.0f, 32.0f};
	m_fTileSize = {20.0f, 20.0f};
	m_fOffset = {16.0f, 16.0f};
	m_nodesShapes = nullptr;
	PrepareConnections();

	m_view.setSize(sf::Vector2f(m_window->getSize()));
	m_view.setCenter(sf::Vector2f(m_view.getSize().x / 2.0f, m_view.getSize().y / 2.0f));
}

void Demonstration::Run()
{
	while (m_window->isOpen())
	{
		EventHandler();
		Update();
		ImGuiLayer();
		Draw();
	}
}

void Demonstration::EventHandler()
{
	while (m_window->pollEvent(*m_event))
	{
		ImGui::SFML::ProcessEvent(*m_event);
		if (m_event->type == sf::Event::Closed)
			m_window->close();
		if (m_event->type == sf::Event::KeyPressed && m_event->key.code == sf::Keyboard::Escape)
			m_window->close();
		if (m_event->type == sf::Event::Resized)
			m_view.setSize(sf::Vector2f(m_window->getSize()));
		if (m_event->type == sf::Event::MouseButtonPressed && m_event->mouseButton.button == sf::Mouse::Left && !isInBoundsOfImGui())
		{
			sf::Vector2i mousePos = sf::Mouse::getPosition(*m_window) - sf::Vector2i(m_fOffset);
			int x = mousePos.x / 32;
			int y = mousePos.y / 32;
			if (x >= 0 && x < gWidth)
				if (y >= 0 && y < gHeight)
				{
					int index = y * gWidth + x;
					if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
					{
						if (m_graph->GetANode(x, y) != m_graph->nodeEnd)
							m_graph->nodeStart = m_graph->GetANode(x, y);
					}
					else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
					{
						if (m_graph->GetANode(x, y) != m_graph->nodeStart)
							m_graph->nodeEnd = m_graph->GetANode(x, y);
					}
					else
					{
						if (m_graph->GetANode(x, y) != m_graph->nodeEnd && m_graph->GetANode(x, y) != m_graph->nodeStart)
						{
							auto node = m_graph->GetANode(x, y);
							if (node)
								node->obstacle = !node->obstacle;
						}
					}
					m_graph->Solve_AStar();
				}
		}
	}
}

void Demonstration::ImGuiLayer()
{
	ImGui::Begin("Simulation Settings");

	ImGui::PushTextWrapPos(ImGui::GetWindowWidth());
	ImGui::TextUnformatted("The green square represents the starting node, and the red square represents the end node."
						   "To make the node inaccessable just click on it. To change the start node you just press Left CTRL + click on the node. To change the end node press Left SHIFT and click on the node"
						   "Yellow nodes are the path nodes");
	ImGui::Separator();
	static bool isDiagonalEnabled = true;
	static bool lastValue = true;
	ImGui::Checkbox("Diagonal edges", &isDiagonalEnabled);
	if (lastValue != isDiagonalEnabled)
	{
		m_graph->PrepareGraph(isDiagonalEnabled);
		PrepareConnections();
		lastValue = isDiagonalEnabled;
	}
	static int wh[2] = {16, 16};
	ImGui::SliderInt2("Width and Height", wh, 10, 20);
	if (ImGui::Button("Change"))
	{
		m_graph->SetWidthAndHeight(wh[0], wh[1]);
		PrepareConnections();
	}

	ImGui::End();
}

void Demonstration::Update()
{
	sf::Time dt = m_clock.restart();
	ImGui::SFML::Update(*m_window, dt);
	for (int x = 0; x < gWidth; x++)
	{
		for (int y = 0; y < gHeight; y++)
		{
			int index = y * gWidth + x;
			auto currentNode = m_graph->GetANode(x, y);
			if (m_graph->nodeStart != currentNode && m_graph->nodeEnd != currentNode)
			{
				if (currentNode->obstacle)
					m_nodesShapes[index].setFillColor(sf::Color(128, 128, 128));
				else if (currentNode->visited)
					m_nodesShapes[index].setFillColor(sf::Color::Blue);
				else if (!currentNode->visited)
					m_nodesShapes[index].setFillColor(sf::Color(0, 0, 255, 64));
			}
			else if (m_graph->nodeStart == currentNode)
			{
				m_nodesShapes[index].setFillColor(sf::Color::Green);
			}
			else if (m_graph->nodeEnd == currentNode)
			{
				m_nodesShapes[index].setFillColor(sf::Color::Red);
			}
		}
	}

	if (m_graph->nodeEnd != nullptr)
	{
		Node *p = m_graph->nodeEnd;

		while (p->parent != nullptr)
		{
			int index = p->y * gWidth + p->x;

			if (p != m_graph->nodeEnd)
				m_nodesShapes[index].setFillColor(sf::Color::Yellow);
			p = p->parent;
		}
	}
}

void Demonstration::Draw()
{
	m_window->setView(m_view);
	m_window->clear(sf::Color());

	m_window->draw(m_connections);

	for (int x = 0; x < gWidth; x++)
		for (int y = 0; y < gHeight; y++)
		{
			m_window->draw(m_nodesShapes[y * gWidth + x]);
		}

	ImGui::SFML::Render(*m_window);
	m_window->display();
}

Demonstration::~Demonstration()
{
	ImGui::SFML::Shutdown();
	delete m_window;
	delete m_event;
	delete m_graph;
	if (m_nodesShapes)
		delete[] m_nodesShapes;
}

void Demonstration::PrepareConnections()
{
	gWidth = m_graph->GetWidth();
	gHeight = m_graph->GetHeight();
	if (m_nodesShapes != nullptr)
	{
		delete[] m_nodesShapes;
	}
	m_nodesShapes = new sf::RectangleShape[gWidth * gHeight];
	m_connections.clear();
	for (int x = 0; x < gWidth; ++x)
		for (int y = 0; y < gHeight; ++y)
		{
			int index = y * gWidth + x;
			m_nodesShapes[index].setSize(m_fTileSize);
			m_nodesShapes[index].setPosition(sf::Vector2f(x * m_fCellSize.x + m_fOffset.x, y * m_fCellSize.y + m_fOffset.y));
		}

	m_connections.setPrimitiveType(sf::Lines);
	for (int x = 0; x < gWidth; x++)
		for (int y = 0; y < gHeight; y++)
		{
			auto n = m_graph->GetANode(x, y);
			if (n)
			{
				for (auto c : n->neighbours)
				{
					sf::Vector2f center1 = {n->x * m_fCellSize.x + m_fOffset.x + m_fCellSize.x / 4.0f,
											n->y * m_fCellSize.y + m_fOffset.y + m_fCellSize.y / 4.0f};
					sf::Vector2f center2 = {c->x * m_fCellSize.x + m_fOffset.x + m_fCellSize.x / 4.f,
											c->y * m_fCellSize.y + m_fOffset.y + m_fCellSize.y / 4.f};

					sf::Vertex v1;
					v1.position = center1;
					v1.color = sf::Color(0, 0, 255, 64);
					sf::Vertex v2;
					v2.position = center2;
					v2.color = sf::Color(0, 0, 255, 64);

					m_connections.append(v1);
					m_connections.append(v2);
				}
			}
		}
}

bool Demonstration::isInBoundsOfImGui()
{
	return ImGui::GetIO().WantCaptureMouse;
}
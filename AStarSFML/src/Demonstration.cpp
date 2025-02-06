#include "Demonstration.h"

Demonstration::Demonstration(uint32_t sWidth, uint32_t sHeight)
{
	m_window = new sf::RenderWindow(sf::VideoMode(sWidth, sHeight), "A Star Algorithm Demo", sf::Style::Titlebar);
	m_event = new sf::Event;
	ImGui::SFML::Init(*m_window);
	bool diagonal = true;
	mGuiGraphSize[0] = 20;
	mGuiGraphSize[1] = 20;
	m_graph = new Graph(diagonal, mGuiGraphSize[0], mGuiGraphSize[1]);
	m_fCellSize = {32.0f, 32.0f};
	m_fTileSize = {20.0f, 20.0f};
	m_fOffset = {16.0f, 16.0f};
	m_nodesShapes = nullptr;
	PrepareConnections();

	m_view.setSize(sf::Vector2f(m_window->getSize()));
	m_view.setCenter(sf::Vector2f(m_view.getSize().x / 2.0f, m_view.getSize().y / 2.0f));

	// Bounding Box stuff
	m_boundingBox.setSize(sf::Vector2f(m_fCellSize.x * m_graph->GetWidth() - 2, m_fCellSize.y * m_graph->GetHeight()));
	m_boundingBox.setFillColor(sf::Color::Transparent);
	m_boundingBox.setOutlineColor(sf::Color::White);
	m_boundingBox.setOutlineThickness(1.0f);
	m_boundingBox.setPosition({m_fOffset.x - 3, m_fOffset.y - 3});


	ImGuiIO& io = ImGui::GetIO();

	io.Fonts->AddFontFromFileTTF(RSC_DIR"KnowingHow.ttf", 24.0f); // Change 24.0f to the desired size
	ImGui::SFML::UpdateFontTexture();

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
			if (x >= 0 && x < m_graph->GetWidth())
				if (y >= 0 && y < m_graph->GetHeight())
				{
					int index = y * m_graph->GetWidth() + x;
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
	ImGui::Begin("Simulation Settings",  nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

	ImGui::SetWindowSize(ImVec2(float((m_window->getSize().x - (m_fCellSize.x * m_graph->GetWidth())) - m_fOffset.x - 1), float(m_window->getSize().y)));
	ImGui::SetWindowPos(ImVec2((m_fCellSize.x) * m_graph->GetWidth()+ m_fOffset.x, 0));
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]); // Assuming you loaded a second font

	ImGui::PushTextWrapPos(ImGui::GetWindowWidth());
	ImGui::TextUnformatted("The green square represents the starting node, and the red square represents the end node."
						   "\n* To make the node inaccessible just click on it.\n* To change the start node you just press Left CTRL + click on the node. To change the end node press Left SHIFT and click on the node"
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
	ImGui::SliderInt2("Width and Height", mGuiGraphSize, 10, 20);
	if (ImGui::Button("Change"))
	{
		m_graph->SetWidthAndHeight(mGuiGraphSize[0], mGuiGraphSize[1]);
		m_boundingBox.setSize(sf::Vector2f(m_fCellSize.x * m_graph->GetWidth() - 2, m_fCellSize.y * m_graph->GetHeight()));
		PrepareConnections();
	}

	ImGui::PopFont();
	ImGui::End();
}

void Demonstration::Update()
{
	sf::Time dt = m_clock.restart();
	ImGui::SFML::Update(*m_window, dt);
	for (int x = 0; x < m_graph->GetWidth(); x++)
	{
		for (int y = 0; y < m_graph->GetHeight(); y++)
		{
			int index = y * m_graph->GetWidth() + x;
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
			int index = p->y * m_graph->GetWidth() + p->x;

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

	for (int x = 0; x < m_graph->GetWidth(); x++)
		for (int y = 0; y < m_graph->GetHeight(); y++)
		{
			m_window->draw(m_nodesShapes[y * m_graph->GetWidth() + x]);
		}

	m_window->draw(m_boundingBox);
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
	if (m_nodesShapes != nullptr)
	{
		delete[] m_nodesShapes;
	}
	m_nodesShapes = new sf::RectangleShape[m_graph->GetWidth() * m_graph->GetHeight()];
	m_connections.clear();
	for (int x = 0; x < m_graph->GetWidth(); ++x)
		for (int y = 0; y < m_graph->GetHeight(); ++y)
		{
			int index = y * m_graph->GetWidth() + x;
			m_nodesShapes[index].setSize(m_fTileSize);
			m_nodesShapes[index].setPosition(sf::Vector2f(x * m_fCellSize.x + m_fOffset.x, y * m_fCellSize.y + m_fOffset.y));
		}

	m_connections.setPrimitiveType(sf::Lines);
	for (int x = 0; x < m_graph->GetWidth(); x++)
		for (int y = 0; y < m_graph->GetHeight(); y++)
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
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <queue>
#include <limits>

using namespace std;

const int WIDTH = 800;
const int HEIGHT = 600;
const int NODE_RADIUS = 20;
const int EDGE_THICKNESS = 3;
const int ANIMATION_DELAY_MS = 100;

const int INF = numeric_limits<int>::max();

struct Edge {
    int to;
    int weight;
    Edge(int t, int w) : to(t), weight(w) {}
};

struct Node {
    int id;
    int dist;
    bool visited;
    sf::CircleShape shape;
    Node* prev;
    Node(int i, int d, int x, int y) : id(i), dist(d), visited(false), prev(nullptr) {
        shape.setRadius(NODE_RADIUS);
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::White);
        shape.setOutlineThickness(2);
        shape.setOutlineColor(sf::Color::Black);
    }
};

bool operator<(const Node& n1, const Node& n2) {
    return n1.dist > n2.dist;
}

vector<int> dijkstra(const vector<vector<Edge>>& graph, int source, vector<Node>& nodes) {
    int n = graph.size();
    vector<int> dist(n, INF);
    priority_queue<Node> pq;

    dist[source] = 0;
    pq.push(nodes[source]);

    while (!pq.empty()) {
        Node node = pq.top();
        pq.pop();

        if (node.visited) continue;
        node.visited = true;

        for (const Edge& edge : graph[node.id]) {
            if (dist[edge.to] > dist[node.id] + edge.weight) {
                dist[edge.to] = dist[node.id] + edge.weight;
                nodes[edge.to].prev = &nodes[node.id]; // Update previous node for path reconstruction
                pq.push(nodes[edge.to]);
            }
        }

        // Visualization
        sf::sleep(sf::milliseconds(ANIMATION_DELAY_MS));
        window.clear(sf::Color::Black);
        for (int i = 0; i < nodes.size(); ++i) {
            if (nodes[i].visited) {
                nodes[i].shape.setFillColor(sf::Color::Green);
            }
            window.draw(nodes[i].shape);
        }
        window.display();
    }

    return dist;
}

void drawPath(sf::RenderWindow& window, Node& start, Node& end) {
    Node* current = &end;
    while (current->prev != nullptr) {
        sf::Vertex line[] = {
            sf::Vertex(start.shape.getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS), sf::Color::Red),
            sf::Vertex(current->shape.getPosition() + sf::Vector2f(NODE_RADIUS, NODE_RADIUS), sf::Color::Red)
        };
        window.draw(line, 2, sf::Lines);
        current = current->prev;
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Dijkstra's Algorithm Visualization");

    vector<vector<Edge>> graph = {
        { Edge(1, 10), Edge(2, 5) },
        { Edge(2, 2), Edge(3, 1) },
        { Edge(1, 3), Edge(3, 9), Edge(4, 2) },
        { Edge(4, 4) },
        {}
    };

    vector<Node> nodes = {
        Node(0, INF, 100, 100),
        Node(1, INF, 200, 200),
        Node(2, INF, 300, 300),
        Node(3, INF, 400, 400),
        Node(4, INF, 500, 500)
    };

    int startNode = -1;
    int endNode = -1;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    for (int i = 0; i < nodes.size(); ++i) {
                        if (nodes[i].shape.getGlobalBounds().contains(mousePos)) {
                            if (startNode == -1) {
                                startNode = i;
                                nodes[startNode].shape.setFillColor(sf::Color::Green);
                            } else if (endNode == -1 && i != startNode) {
                                endNode = i;
                                nodes[endNode].shape.setFillColor(sf::Color::Red);
                                vector<int> distances = dijkstra(graph, startNode, nodes);
                                drawPath(window, nodes[startNode], nodes[endNode]);
                                startNode = -1;
                                endNode = -1;
                                for (int i = 0; i < nodes.size(); ++i) {
                                    nodes[i].dist = INF;
                                    nodes[i].visited = false;
                                    nodes[i].prev = nullptr;
                                    nodes[i].shape.setFillColor(sf::Color::White);
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::Black);

        for (int i = 0; i < nodes.size(); ++i) {
            window.draw(nodes[i].shape);
        }

        window.display();
    }

    return 0;
}

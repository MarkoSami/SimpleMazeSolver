#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <queue>
#include <array>
#include <cmath>
#include<thread>
#include<map>
#include<unordered_map>

#include "utils.h"

#define PI 3.14159265358979323846


struct graphEdge {
    sf::RectangleShape* graphicalEdge;
    int otherVertex;
    graphEdge(int otherVertex, sf::RectangleShape* ge) {
        this->graphicalEdge = ge;
        this->otherVertex = otherVertex;
    }
};

struct graphicalVertex
{
    int id;
    sf::CircleShape gVertex;
    graphicalVertex(int id , sf::CircleShape c) {
        this->id = id;
        this->gVertex = c;
    }
};


// vertices vector
std::vector<graphicalVertex> vertices;

// lines vector
std::vector<sf::RectangleShape*> edges;

// current selected vertices
std::queue<graphicalVertex> lineQueue;

// graph 
std::unordered_map<int,std::list<graphEdge>> graph;

int vertexCounter = 0;

std::unordered_map<int, bool> visited;



sf::RectangleShape* createNewEdge(double start_x, double start_y, double end_x, double end_y) {
    double distance = sqrt(pow((start_x - end_x), 2) + pow(start_y - end_y, 2));
    sf::RectangleShape* edge = new sf::RectangleShape(sf::Vector2f(distance, 3.f));
    edge->setFillColor(sf::Color::Black);
    edge->setPosition(start_x, start_y);

    edge->rotate(atan2(end_y - start_y, end_x - start_x) * 180 / PI);

    return edge;
}

bool DFS(int source,int  target) {
    if (source == target) {
        return true;
    }
    visited[source] = true;

    bool path = false;
    for (auto child : graph[source]) {
        child.graphicalEdge->setFillColor(sf::Color::Green);
        if (!visited[child.otherVertex]) {
            path = DFS(child.otherVertex, target);
        }
        if (path) {
            child.graphicalEdge->setFillColor(sf::Color::Red);
            return true;
        }

    }
    return path;


}


int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Maze Solver");
    window.setFramerateLimit(30);

    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("maze.png")) {
        // Handle error if the image fails to load
        return EXIT_FAILURE;
    }

    sf::Sprite backgroundSprite(backgroundTexture);

    backgroundSprite.setScale(
        static_cast<float>(window.getSize().x) / backgroundSprite.getLocalBounds().width,
        static_cast<float>(window.getSize().y) / backgroundSprite.getLocalBounds().height
    );



    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }




        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            if (!vertices.empty()) {
                DFS(vertices[0].id, vertices[vertices.size() - 1].id);
                std::cout << "DFS STARTED..!\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            for (auto edge : edges) {
                edge->setFillColor(sf::Color::Black);
                visited.clear();
            }
        }


        // Checking if the left mouse button is clicked
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            // Adding a new vertex
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
            {
                sf::CircleShape vertex;
                vertex.setFillColor(sf::Color::Green);
                vertex.setRadius(20.f);
                int x = sf::Mouse::getPosition(window).x;
                int y = sf::Mouse::getPosition(window).y;
                vertex.setPosition(x , y ); vertex.setOrigin(vertex.getRadius(), vertex.getRadius());


                graphicalVertex gVertex(vertexCounter,vertex);
                vertices.push_back(gVertex);
                graph[vertexCounter];
                vertexCounter++;

                std::this_thread::sleep_for(std::chrono::milliseconds(50));


            }
            else {
                for (auto vertex = vertices.begin(); vertex != vertices.end();)
                {
                    if (vertex->gVertex.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
                    {
                        if (lineQueue.size() >= 2) {
                            lineQueue.pop();
                        }
                        graphicalVertex gVertex(vertex->id,vertex->gVertex);
                        lineQueue.push(gVertex);
                        std::cout << vertex->gVertex.getPosition().x << " " << vertex->gVertex.getPosition().y << "\n";
                        break;
                    }
                    else
                    {
                        ++vertex; // Move to the next element
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));

            }
            
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
        {
            if (lineQueue.size() == 2)
            {
                // Create a new line representing the new edge between two selected vertices
                int start_x = lineQueue.front().gVertex.getPosition().x, start_y = lineQueue.front().gVertex.getPosition().y;
                int fromId = lineQueue.front().id;
                lineQueue.pop();

                int end_x = lineQueue.front().gVertex.getPosition().x, end_y = lineQueue.front().gVertex.getPosition().y;
                int toId = lineQueue.front().id;
                lineQueue.pop();

                sf::RectangleShape* edge = utils::createNewEdge(start_x, start_y, end_x, end_y);
                graph[fromId].push_back(graphEdge(toId, edge));
                graph[toId].push_back(graphEdge(fromId, edge));
                edges.push_back(edge);


                std::cout << "New edge created!\n";
            }
        }

        if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::V))
            {
                for (auto vertex = vertices.begin(); vertex != vertices.end();)
                {
                    if (vertex->gVertex.getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
                    {
                        vertex = vertices.erase(vertex); 
                        break;
                    }
                    else
                    {
                        ++vertex; // Move to the next element
                    }
                }
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            {
                for (auto edge = edges.begin(); edge != edges.end();)
                {
                    if ((*edge)->getGlobalBounds().contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y))
                    {
                        edge = edges.erase(edge); // Erase and update the iterator
                        break;
                    }
                    else
                    {
                        ++edge; // Move to the next element
                    }
                }

            }
        }

        // CLEAR
        window.clear(sf::Color::White);

        // DRAW

        window.draw(backgroundSprite);

        // Drawing vertices
        for (auto vertex : vertices)
        {
            window.draw(vertex.gVertex);
        }

        // Drawing lines
        for (auto& edge : edges)
        {
            window.draw((*edge));
        }



        // DISPLAY
        window.display();
    }

    return 0;
}

#include "Common.hpp"
#include "Host.hpp"
#include <enet/enet.h>
#include <SFML/Graphics.hpp>
#include <iostream>

struct Server
{
        Server();
        void update(const sf::Time);

        Host host;
        sf::Vector2f position;
        sf::Vector2f velocity;
        bool running;
};

int main()
{
        Server server;

        if (enet_initialize() != 0)
        {
                std::cerr << "Could not initialize enet." << std::endl;
                return EXIT_FAILURE;
        }

        if (!server.host.create("localhost", PORT_SV, 5))
        {
                std::cerr << "Could not create the host." << std::endl;
                return EXIT_FAILURE;
        }

        sf::Clock clock;
        sf::Time accumulator;

        while (server.running)
        {
                accumulator += clock.restart();

                while (accumulator >= TIME_PER_TICK)
                {
                        accumulator -= TIME_PER_TICK;

                        server.update(TIME_PER_TICK);
                }

        }

        enet_deinitialize();

        return EXIT_SUCCESS;
}

Server::Server()
        : position(0.0f, 0.0f)
        , velocity(0.0f, 0.0f)
        , running(true)
{
}

void Server::update(const sf::Time elapsed)
{
        Event event;
        while (host.pollEvent(event))
        {
                if (event.type == Event::Type::Connect)
                {
                        std::cout << "Connection[id=" << event.peer.id;
                        std::cout << "]" << std::endl;
                }
                else if (event.type == Event::Type::Disconnect)
                {
                        std::cout << "Disconnection[id=";
                        std::cout << event.peer.id << "]" << std::endl;
                }
                else if (event.type == Event::Type::Receive)
                {
                        Uint8 input;
                        event.packet >> input;

                        sf::Vector2f acceleration;
                        if (input & 0x1) // Right
                        {
                                acceleration.x += ACCELERATION;
                        }
                        if (input & 0x2) // Left
                        {
                                acceleration.x -= ACCELERATION;
                        }
                        if (input & 0x4) // Up
                        {
                                acceleration.y -= ACCELERATION;
                        }
                        if (input & 0x8) // Down
                        {
                                acceleration.y += ACCELERATION;
                        }

                        velocity += acceleration * elapsed.asSeconds();
                        float frictionRatio = 1 / (1 + FRICTION * elapsed.asSeconds());
                        velocity *= frictionRatio;

                        position += velocity * elapsed.asSeconds();
                }
        }

        Packet packet;
        packet << position.x << position.y;
        host.broadcast(packet);
}

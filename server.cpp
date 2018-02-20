#include "Common.hpp"
#include "Host.hpp"
#include <enet/enet.h>
#include <SFML/Graphics.hpp>
#include <iostream>

struct Server
{
        Server() : playerX(0.0f), playerY(0.0f), running(true) {}

        Host host;
        float playerX;
        float playerY;
        bool running;
};

void update(const sf::Time, Server&);

int main()
{
        Server server;
        server.running = true;

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

                        update(TIME_PER_TICK, server);
                }

        }

        enet_deinitialize();

        return EXIT_SUCCESS;
}

void update(const sf::Time, Server& server)
{
        Event event;
        while (server.host.pollEvent(event))
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

                        if (input & 0x1) // Right
                        {
                                server.playerX += 1;
                        }
                        if (input & 0x2) // Left
                        {
                                server.playerX -= 1;
                        }
                        if (input & 0x4) // Up
                        {
                                server.playerY += 1;
                        }
                        if (input & 0x8) // Down
                        {
                                server.playerY -= 1;
                        }
                }
        }

        Packet packet;
        packet << server.playerX << server.playerY;
        server.host.broadcast(packet);
}

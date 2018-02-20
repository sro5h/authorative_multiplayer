#include "Common.hpp"
#include "Host.hpp"
#include <enet/enet.h>
#include <SFML/Graphics.hpp>
#include <iostream>

struct Client
{
        Client() : playerX(0.0f), playerY(0.0f), running(true) {}

        Host host;
        Peer server;
        float playerX;
        float playerY;
        bool running;

        sf::RenderWindow window;
};

void update(const sf::Time, Client&);
void draw(Client&);
Uint8 processInput();

int main()
{
        Client client;
        client.window.create(sf::VideoMode(400, 400), "App");

        if (enet_initialize() != 0)
        {
                std::cerr << "Could not initialize enet." << std::endl;
                return EXIT_FAILURE;
        }

        if (!client.host.create("localhost", PORT_CL, 1))
        {
                std::cerr << "Could not create the host." << std::endl;
                return EXIT_FAILURE;
        }

        if (!client.host.connect("localhost", PORT_SV))
        {
                std::cerr << "Failed to attempt a connection." << std::endl;
                return EXIT_FAILURE;
        }

        sf::Clock clock;
        sf::Time accumulator;

        while (client.running)
        {
                accumulator += clock.restart();

                while (accumulator >= TIME_PER_TICK)
                {
                        accumulator -= TIME_PER_TICK;

                        update(TIME_PER_TICK, client);
                }

                draw(client);
        }

        enet_deinitialize();

        return EXIT_SUCCESS;
}

void update(const sf::Time, Client& client)
{
        sf::Event sfEvent;
        while (client.window.pollEvent(sfEvent))
        {
                if (sfEvent.type == sf::Event::Closed)
                {
                        client.host.disconnectAll();
                        client.window.close();
                        client.running = false;
                }
        }

        if (client.server)
        {
                Uint8 input = processInput();
                Packet packet;
                packet << input;
                client.host.send(client.server, packet);
        }

        Event event;
        while (client.host.pollEvent(event))
        {
                if (event.type == Event::Type::Connect)
                {
                        std::cout << "Connection[id=" << event.peer.id;
                        std::cout << "]" << std::endl;
                        client.server = event.peer;
                }
                else if (event.type == Event::Type::Disconnect)
                {
                        std::cout << "Disconnection[id=";
                        std::cout << event.peer.id << "]" << std::endl;
                }
                else if (event.type == Event::Type::Receive)
                {
                        event.packet >> client.playerX;
                        event.packet >> client.playerY;

                        std::cout << "Pos = " << client.playerX << ", ";
                        std::cout << client.playerY << std::endl;
                }
        }
}

void draw(Client& client)
{
        client.window.clear();

        sf::CircleShape shape(20.0f);
        shape.setPosition(client.playerX, client.playerY);
        client.window.draw(shape);

        client.window.display();
}

Uint8 processInput()
{
        Uint8 input = 0x0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                input |= 0x1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                input |= 0x2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                input |= 0x4;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                input |= 0x8;

        return input;
}

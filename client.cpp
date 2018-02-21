#include "Common.hpp"
#include "Host.hpp"
#include <enet/enet.h>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include <cassert>

struct PlayerState
{
        PlayerState();

        sf::Vector2f position;
};

struct Client
{
        Client();
        void update(const sf::Time);
        void draw();
        void processInput();

        Host host;
        Peer server;
        bool running;

        std::map<Peer, PlayerState> players;

        sf::RenderWindow window;
};

int main()
{
        Client client;

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

                        client.update(TIME_PER_TICK);
                }

                client.draw();
        }

        enet_deinitialize();

        return EXIT_SUCCESS;
}

PlayerState::PlayerState()
        : position(0.0f, 0.0f)
{
}

Client::Client()
        : running(true)
        , window(sf::VideoMode(400, 400), "App")
{
}

void Client::update(const sf::Time)
{
        sf::Event sfEvent;
        while (window.pollEvent(sfEvent))
        {
                if (sfEvent.type == sf::Event::Closed)
                {
                        host.disconnectAll();
                        window.close();
                        running = false;
                }
        }

        if (server)
        {
                processInput();
        }

        Event event;
        while (host.pollEvent(event))
        {
                if (event.type == Event::Type::Connect)
                {
                        std::cout << "Connection[id=" << event.peer.id;
                        std::cout << "]" << std::endl;

                        server = event.peer;
                        players.insert({ server, PlayerState() });
                }
                else if (event.type == Event::Type::Disconnect)
                {
                        std::cout << "Disconnection[id=";
                        std::cout << event.peer.id << "]" << std::endl;

                        std::size_t rc;
                        rc = players.erase(server);
                        assert(rc == 1);
                }
                else if (event.type == Event::Type::Receive)
                {
                        Uint16 id;
                        while (event.packet >> id)
                        {
                                if (id == server.id)
                                {
                                        PlayerState& state = players.at(server);
                                        event.packet >> state.position.x;
                                        event.packet >> state.position.y;
                                }
                        }
                }
        }
}

void Client::draw()
{
        window.clear();

        if (server)
        {
                PlayerState& state = players.at(server);
                sf::CircleShape shape(20.0f);
                shape.setPosition(state.position);
                window.draw(shape);
        }

        window.display();
}

void Client::processInput()
{
        Packet packet;
        Uint8 input = 0x0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                input |= 0x1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                input |= 0x2;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                input |= 0x4;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                input |= 0x8;

        packet << input;
        host.send(server, packet);
}

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
        sf::Vector2f velocity;
};

struct PlayerInput
{
        PlayerInput();

        bool right, left, up, down;
};

struct Server
{
        Server();
        void update(const sf::Time);
        void updatePlayers(const sf::Time);

        Host host;
        bool running;

        std::map<Peer, PlayerState> players;
        std::map<Peer, PlayerInput> inputs;
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

PlayerState::PlayerState()
        : position(0.0f, 0.0f)
        , velocity(0.0f, 0.0f)
{
}

PlayerInput::PlayerInput()
        : right(false)
        , left(false)
        , up(false)
        , down(false)
{
}

Server::Server()
        : running(true)
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

                        players.insert({ event.peer, PlayerState() });
                        inputs.insert({ event.peer, PlayerInput() });
                }
                else if (event.type == Event::Type::Disconnect)
                {
                        std::cout << "Disconnection[id=";
                        std::cout << event.peer.id << "]" << std::endl;

                        std::size_t rc;
                        rc = inputs.erase(event.peer);
                        assert(rc == 1);
                        rc = players.erase(event.peer);
                        assert(rc == 1);
                }
                else if (event.type == Event::Type::Receive)
                {
                        Uint8 input;
                        event.packet >> input;

                        PlayerInput& playerInput = inputs.at(event.peer);
                        playerInput.right = input & 0x1;
                        playerInput.left = input & 0x2;
                        playerInput.up = input & 0x4;
                        playerInput.down = input & 0x8;
                }
        }

        updatePlayers(elapsed);

        if (!players.empty())
        {
                Packet packet;

                for (auto& pair: players)
                {
                        packet << pair.first.id;
                        packet << pair.second.position.x;
                        packet << pair.second.position.y;
                }

                host.broadcast(packet);
        }
}

void Server::updatePlayers(const sf::Time elapsed)
{
        for (auto& pair: inputs)
        {
                sf::Vector2f acceleration;
                if (pair.second.right)
                        acceleration.x += ACCELERATION;
                if (pair.second.left)
                        acceleration.x -= ACCELERATION;
                if (pair.second.up)
                        acceleration.y -= ACCELERATION;
                if (pair.second.down)
                        acceleration.y += ACCELERATION;

                PlayerState& state = players.at(pair.first);
                state.velocity += acceleration * elapsed.asSeconds();
                float friction = 1 / (1 + FRICTION * elapsed.asSeconds());
                state.velocity *= friction;
                state.position += state.velocity * elapsed.asSeconds();
        }
}

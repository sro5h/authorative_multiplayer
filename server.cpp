#include "GameServer.hpp"
#include <SFML/System/Clock.hpp>
#include <enet/enet.h>
#include <iostream>

int main()
{
        GameServer server;

        if (enet_initialize() != 0)
        {
                std::cerr << "Could not initialize enet." << std::endl;
                return EXIT_FAILURE;
        }

        if (!server.create())
        {
                enet_deinitialize();
                return EXIT_FAILURE;
        }

        sf::Clock clock;
        sf::Time accumulator;

        while (server.isRunning())
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

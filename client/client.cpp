#include "../Common.hpp"
#define ENET_IMPLEMENTATION
#include "../enet.h"
#include "GameClient.hpp"
#include <SFML/System/Clock.hpp>
#include <iostream>

int main(int argc, char** argv) {
        sf::Uint16 port = Constants::PortCl;
        GameClient client;

        if (enet_initialize() != 0) {
                std::cout << "[client] Could not initialize enet." << std::endl;
                return EXIT_FAILURE;
        }

        if (argc == 2) {
                port = std::atoi(argv[1]);
        }

        if (!client.create(port)) {
                enet_deinitialize();
                return EXIT_FAILURE;
        }

        if (!client.connect("localhost", Constants::PortSv)) {
                enet_deinitialize();
                return EXIT_FAILURE;
        }

        std::cout << "[client] Host created on port " << port << std::endl;

        sf::Clock clock;
        sf::Time delta;
        sf::Time accumulator;

        while (client.isRunning()) {
                delta = clock.restart();
                accumulator += delta;

                while (accumulator >= Constants::TimePerTick) {
                        accumulator -= Constants::TimePerTick;
                        client.update(Constants::TimePerTick);
                }

                client.render(delta);
        }

        enet_deinitialize();

        return EXIT_SUCCESS;
}

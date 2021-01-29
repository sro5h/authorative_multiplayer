#include "GameServer.hpp"
#include "../common/Common.hpp"
#include "../common/NetworkInitialiser.hpp"
#include <SFML/System/Clock.hpp>
#include <iostream>

int main(int argc, char** argv) {
        sf::Uint16 port = Constants::PortSv;
        NetworkInitialiser initialiser;
        GameServer server;

        if (!initialiser.initialise()) {
                std::cout << "[server] Could not initialize enet." << std::endl;
                return EXIT_FAILURE;
        }

        if (argc == 2) {
                port = std::atoi(argv[1]);
        }

        if (!server.create(port)) {
                return EXIT_FAILURE;
        }

        std::cout << "[server] Host created on port " << port << std::endl;

        sf::Clock clock;
        sf::Time delta;
        sf::Time accumulator;

        while (server.isRunning()) {
                delta = clock.restart();
                accumulator += delta;

                while (accumulator >= Constants::TimePerTick) {
                        accumulator -= Constants::TimePerTick;
                        server.update(Constants::TimePerTick);
                }
        }

        return EXIT_SUCCESS;
}

#include "Common.hpp"
#include "Host.hpp"
#include <enet/enet.h>
#include <iostream>

int main()
{
        Host host;
        bool running = true;

        if (enet_initialize() != 0)
        {
                std::cerr << "Could not initialize enet." << std::endl;
                return EXIT_FAILURE;
        }

        if (!host.create("localhost", PORT_CL, 1))
        {
                std::cerr << "Could not create the host." << std::endl;
                return EXIT_FAILURE;
        }

        if (!host.connect("localhost", PORT_SV))
        {
                std::cerr << "Failed to attempt a connection." << std::endl;
                return EXIT_FAILURE;
        }

        while (running)
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
                                std::string msg;
                                event.packet >> msg;

                                std::cout << "Received[id=" << event.peer.id;
                                std::cout << "] " << msg << std::endl;
                        }
                }
        }

        enet_deinitialize();

        return EXIT_SUCCESS;
}

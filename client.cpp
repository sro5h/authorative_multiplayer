#include "Host.hpp"
#include <enet/enet.h>
#include <iostream>

int main()
{
        if (enet_initialize() != 0)
        {
                std::cerr << "Could not initialize enet." << std::endl;
                return EXIT_FAILURE;
        }

        std::cout << "All good" << std::endl;

        enet_deinitialize();

        return EXIT_SUCCESS;
}

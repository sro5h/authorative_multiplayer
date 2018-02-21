#include "Common.hpp"
#include "Event.hpp"
#include <iostream>

void logEvent(const Event& event)
{
        if (event.type == Event::Type::Connect)
        {
                std::cout << "Connect";
                std::cout << " from peer" << event.peer.id << std::endl;
        }
        else if (event.type == Event::Type::Disconnect)
        {
                std::cout << "Disconnect";
                std::cout << " from peer" << event.peer.id << std::endl;
        }
}

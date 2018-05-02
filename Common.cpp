#include "Common.hpp"
#include "Event.hpp"
#include <iostream>

PlayerState::PlayerState()
        : position(0.0f, 0.0f)
        , velocity(0.0f, 0.0f)
{
}

PlayerInput::PlayerInput()
        : id(0)
        , right(false)
        , left(false)
        , up(false)
        , down(false)
{
}

Packet& operator<<(Packet& p, const ClientMessage& msg)
{
        return p << static_cast<Uint8>(msg);
}

Packet& operator>>(Packet& p, ClientMessage& msg)
{
        Uint8 data;
        if (p >> data)
                msg = static_cast<ClientMessage>(data);

        return p;
}

Packet& operator<<(Packet& p, const ServerMessage& msg)
{
        return p << static_cast<Uint8>(msg);
}

Packet& operator>>(Packet& p, ServerMessage& msg)
{
        Uint8 data;
        if (p >> data)
                msg = static_cast<ServerMessage>(data);

        return p;
}

Packet& operator<<(Packet& p, const sf::Vector2f& vec)
{
        return p << vec.x << vec.y;
}

Packet& operator>>(Packet& p, sf::Vector2f& vec)
{
        sf::Vector2f data;
        if (p >> data.x >> data.y)
                vec = data;

        return p;
}

void logEvent(const Event& event)
{
        if (event.type == Event::Type::Connect)
        {
                std::cout << "Connect";
                std::cout << " from peer with id " << event.peer.connectId;
                std::cout << std::endl;
        }
        else if (event.type == Event::Type::Disconnect)
        {
                std::cout << "Disconnect";
                std::cout << " from peer with id " << event.peer.connectId;
                std::cout << std::endl;
        }
}

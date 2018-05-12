#include "Common.hpp"
#include "Event.hpp"
#include <iostream>

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

void applyPlayerInput(sf::Time delta, const PlayerInput& input,
                PlayerState& state)
{
        sf::Vector2f accel;

        if (input.right)
                accel.x += ACCELERATION;

        if (input.left)
                accel.x -= ACCELERATION;

        if (input.up)
                accel.y -= ACCELERATION;

        if (input.down)
                accel.y += ACCELERATION;

        state.velocity += accel * delta.asSeconds();
}

void updatePlayerState(sf::Time delta, PlayerState& state)
{
        float friction = 1 / (1 + FRICTION * delta.asSeconds());
        state.velocity *= friction;
        state.position += state.velocity * delta.asSeconds();
}

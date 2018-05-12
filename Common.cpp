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

#include <limits>
static_assert(std::numeric_limits<float>::is_iec559, "Requires IEEE 754");


#include <algorithm>
#include <cmath>
bool equals(float a, float b, float epsilon)
{
        float max = std::max({ 1.0f, a, b });
        return std::abs(a - b) <= max * epsilon;
}

bool equals(const sf::Vector2f& a, const sf::Vector2f& b)
{
        return equals(a.x, b.x) && equals(a.y, b.y);
}

bool equals(const PlayerState& a, const PlayerState& b)
{
        return equals(a.position, b.position) && equals(a.velocity, b.velocity);
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

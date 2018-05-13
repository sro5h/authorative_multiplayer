#include "Common.hpp"
#include "Event.hpp"
#include <iostream>

PlayerState::PlayerState()
        : position(0.0f, 0.0f)
        , velocity(0.0f, 0.0f)
{
}

PlayerInput::PlayerInput()
        : left(false)
        , right(false)
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

Packet& operator<<(Packet& p, const sf::Vector2f& v)
{
        return p << v.x << v.y;
}

Packet& operator>>(Packet& p, sf::Vector2f& v)
{
        float x, y;
        if (p >> x >> y)
        {
                v.x = x;
                v.y = y;
        }

        return p;
}

Packet& operator<<(Packet& p, const PlayerInput& input)
{
        Uint8 data = 0;

        if (input.left)
                data |= 0x1;

        if (input.right)
                data |= 0x2;

        if (input.up)
                data |= 0x4;

        if (input.down)
                data |= 0x8;

        return p << data;
}

Packet& operator>>(Packet& p, PlayerInput& input)
{
        Uint8 data = 0;
        if (p >> data)
        {
                input.left  = data & 0x1;
                input.right = data & 0x2;
                input.up    = data & 0x4;
                input.down  = data & 0x8;
        }

        return p;
}

#include <limits>
static_assert(std::numeric_limits<float>::is_iec559, "Requires IEEE 754");


#include <algorithm>
#include <cmath>
bool equals(float a, float b, float epsilon)
{
        if (a == b) return true;

        float diff = std::abs(a - b);
        float absSum = std::abs(a) + std::abs(b);

        return (diff / std::min(absSum, std::numeric_limits<float>::max()))
               < epsilon;
}

bool equals(const sf::Vector2f& a, const sf::Vector2f& b)
{
        return equals(a.x, b.x) && equals(a.y, b.y);
}

bool equals(const PlayerState& a, const PlayerState& b)
{
        return equals(a.position, b.position) && equals(a.velocity, b.velocity);
}

#include <random>
float frandom(float lower, float upper)
{
        static std::random_device rd;
        static std::default_random_engine e(rd());

        std::uniform_real_distribution<float> dist(lower, upper);
        return dist(e);
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

        if (input.left)
                accel.x -= ACCELERATION;

        if (input.right)
                accel.x += ACCELERATION;

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

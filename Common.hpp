#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include "Types.hpp"
#include "Packet.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

constexpr Uint16 PORT_CL = 0;
constexpr Uint16 PORT_SV = 42424;

const sf::Time TIME_PER_TICK = sf::microseconds(50000);

constexpr float ACCELERATION = 200.0f;
constexpr float FRICTION = 5.0f;

enum class ClientMessage : Uint8
{
        Input,
};

enum class ServerMessage : Uint8
{
        State,
};

struct PlayerState
{
        PlayerState();

        sf::Vector2f position;
        sf::Vector2f velocity;
};

struct PlayerInput
{
        PlayerInput();

        bool right, left, up, down;
};

Packet& operator<<(Packet&, const ClientMessage&);
Packet& operator>>(Packet&, ClientMessage&);
Packet& operator<<(Packet&, const ServerMessage&);
Packet& operator>>(Packet&, ServerMessage&);

void logEvent(const Event& event);

void applyPlayerInput(sf::Time delta, const PlayerInput& input,
                PlayerState& state);
void updatePlayerState(sf::Time delta, PlayerState& state);

#endif // COMMON_HPP_INCLUDED

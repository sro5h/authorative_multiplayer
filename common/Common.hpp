#ifndef AM_COMMON_HPP
#define AM_COMMON_HPP

#include "MsgpackAdaptors.hpp"
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <msgpack.hpp>

namespace Constants {

constexpr sf::Uint16 PortCl = 0;
constexpr sf::Uint16 PortSv = 42424;

const sf::Time TimePerTick = sf::microseconds(50000);

constexpr float Acceleration = 800.0f;
constexpr float Friction = 10.0f;

}

struct State {
        explicit State() : position{}, velocity{} { }

        sf::Vector2f position;
        sf::Vector2f velocity;

        MSGPACK_DEFINE(position, velocity);
};

struct Input {
        explicit Input() : left{}, right{}, up{}, down{} { }

        bool left, right, up, down;

        MSGPACK_DEFINE(left, right, up, down);
};

void applyInput(sf::Time delta, Input const& input, State& state);
void updateState(sf::Time delta, State& state);

#endif

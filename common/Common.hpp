#ifndef AM_COMMON_HPP
#define AM_COMMON_HPP

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

namespace Constants {

constexpr sf::Uint16 PortCl = 0;
constexpr sf::Uint16 PortSv = 42424;

const sf::Time TimePerTick = sf::microseconds(50000);

}

struct State {
        explicit State() = default;

        sf::Vector2f position;
        sf::Vector2f velocity;
};

struct Input {
        explicit Input() = default;

        bool left, right, up, down;
};

#endif

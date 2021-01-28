#ifndef AM_COMMON_HPP
#define AM_COMMON_HPP

#include <SFML/System/Time.hpp>

namespace Constants {

constexpr sf::Uint16 PortCl = 0;
constexpr sf::Uint16 PortSv = 42424;

const sf::Time TimePerTick = sf::microseconds(50000);

}

#endif

#ifndef COMMON_HPP_INCLUDED
#define COMMON_HPP_INCLUDED

#include "Types.hpp"
#include <SFML/System/Time.hpp>

constexpr Uint16 PORT_CL = 53535;
constexpr Uint16 PORT_SV = 42424;

const sf::Time TIME_PER_TICK = sf::microseconds(100000);

#endif // COMMON_HPP_INCLUDED

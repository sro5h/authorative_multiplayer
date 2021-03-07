#ifndef AM_COMPONENTS_HPP
#define AM_COMPONENTS_HPP

#include "Common.hpp"

struct Transform {
        explicit Transform();

        sf::Vector2f position;
        sf::Vector2f velocity;
};

// History component that stores input history and more.
// !TODO: Maybe rename.
struct History {
        explicit History();

        sf::Uint32 lastInputTick;
        // !TODO: Maybe use fixed size buffer;
        std::vector<Input> inputs;
};

auto applyInput(sf::Time delta, Input const& input) -> sf::Vector2f;
void updateTransform(sf::Time delta, Transform& transform);

#endif

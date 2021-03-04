#include "Common.hpp"

void applyInput(sf::Time delta, Input const& input, State& state) {
        sf::Vector2f acceleration;

        if (input.left)
                acceleration.x -= Constants::Acceleration;
        if (input.right)
                acceleration.x += Constants::Acceleration;
        if (input.up)
                acceleration.y -= Constants::Acceleration;
        if (input.down)
                acceleration.y += Constants::Acceleration;

        state.velocity += acceleration * delta.asSeconds();
}

void updateState(sf::Time delta, State& state) {
        float friction = 1 / (1 + Constants::Friction * delta.asSeconds());
        state.velocity *= friction;
        state.position += state.velocity * delta.asSeconds();
}

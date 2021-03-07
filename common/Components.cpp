#include "Components.hpp"

Transform::Transform()
        : position{}
        , velocity{} {
}

History::History()
        : lastInputTick{}
        , inputs{} {
}

auto applyInput(sf::Time delta, Input const& input) -> sf::Vector2f {
        sf::Vector2f acceleration;

        if (input.left)
                acceleration.x -= Constants::Acceleration;
        if (input.right)
                acceleration.x += Constants::Acceleration;
        if (input.up)
                acceleration.y -= Constants::Acceleration;
        if (input.down)
                acceleration.y += Constants::Acceleration;

        return acceleration;
}

void updateTransform(sf::Time delta, Transform& transform) {
        float friction = 1 / (1 + Constants::Friction * delta.asSeconds());
        transform.velocity *= friction;
        transform.position += transform.velocity * delta.asSeconds();
}

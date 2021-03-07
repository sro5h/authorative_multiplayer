#include "RenderSystem.hpp"
#include "../common/Components.hpp"
#include <SFML/Graphics/CircleShape.hpp>

void RenderSystem::update(sf::Time delta, entt::registry& registry, sf::RenderTarget& target) {
        auto view = registry.view<const Transform>();
        for (auto [entity, transform] : view.each()) {
                auto shape = sf::CircleShape{20.0f};
                shape.setPosition(transform.position);
                shape.setFillColor(sf::Color{64, 64, 157});

                target.draw(shape);
        }
}

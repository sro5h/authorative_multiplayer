#include "PhysicsSystem.hpp"
#include "../common/Components.hpp"

void PhysicsSystem::update(sf::Time delta, entt::registry& registry) {
        auto view = registry.view<Transform>();
        for (auto [entity, transform] : view.each()) {
                updateTransform(delta, transform);
        }
}

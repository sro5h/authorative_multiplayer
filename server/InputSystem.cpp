#include "InputSystem.hpp"
#include "../common/Components.hpp"

void InputSystem::update(sf::Time delta, entt::registry& registry) {
        auto view = registry.view<Transform, History>();
        for (auto [entity, transform, history] : view.each()) {
                if (!history.inputs.empty()) {
                        auto acceleration = applyInput(delta, history.inputs.back());
                        transform.velocity += acceleration * delta.asSeconds();
                }
        }
}

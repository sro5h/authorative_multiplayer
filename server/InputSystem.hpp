#ifndef AM_INPUT_SYSTEM_HPP
#define AM_INPUT_SYSTEM_HPP

#include "../common/Common.hpp"
#include <entt/entity/registry.hpp>

class InputSystem {
public:
        explicit InputSystem() = default;

        void update(sf::Time delta, entt::registry& registry);
};

#endif

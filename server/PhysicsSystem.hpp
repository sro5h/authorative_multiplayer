#ifndef AM_PHYSICS_SYSTEM_HPP
#define AM_PHYSICS_SYSTEM_HPP

#include "../common/Common.hpp"
#include <entt/entt.hpp>

class PhysicsSystem {
public:
        explicit PhysicsSystem() = default;

        void update(sf::Time delta, entt::registry& registry);
};

#endif

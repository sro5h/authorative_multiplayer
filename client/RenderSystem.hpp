#ifndef AM_RENDER_SYSTEM_HPP
#define AM_RENDER_SYSTEM_HPP

#include "../common/Common.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <entt/entt.hpp>

class RenderSystem {
public:
        explicit RenderSystem() = default;

        void update(sf::Time delta, entt::registry& registry, sf::RenderTarget& target);
};

#endif

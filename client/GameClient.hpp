#ifndef AM_GAME_CLIENT_HPP
#define AM_GAME_CLIENT_HPP

#include "RenderSystem.hpp"
#include "../common/Common.hpp"
#include "../common/Messages.hpp"
#include <SFML/Graphics/RenderWindow.hpp>
#include <entt/entity/registry.hpp>
#include <msgpack.hpp>
#include <string>

struct _ENetHost;
struct _ENetPeer;
struct _ENetPacket;

class GameClient {
public:
        explicit GameClient();

        void update(sf::Time delta);
        void render(sf::Time delta);

        bool create(sf::Uint16 port);
        bool connect(std::string const& address, sf::Uint16 port);
        void disconnect();

        bool isRunning() const;

private:
        void onConnect(_ENetPeer& peer);
        void onDisconnect(_ENetPeer& peer);
        void onDisconnectTimeout(_ENetPeer& peer);
        void onReceive(_ENetPeer& peer, _ENetPacket& packet);

        void onReceiveState(_ENetPeer& peer, ServerHeader const& header, msgpack::unpacker& unpacker);

        void processInput(sf::Time delta);

        void nextTick();
        sf::Uint32 getTick() const;

private:
        sf::RenderWindow m_window;

        _ENetHost* m_host;
        bool m_running;
        sf::Uint32 m_tickCounter;

        _ENetPeer* m_peer;
        bool m_connected;

        entt::registry m_registry;
        entt::entity m_entity;

        // For debug purposes
        entt::entity m_serverEntity;

        RenderSystem m_renderSystem;
};

#endif

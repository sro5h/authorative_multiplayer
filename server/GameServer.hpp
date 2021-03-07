#ifndef AM_GAME_SERVER_HPP
#define AM_GAME_SERVER_HPP

#include "InputSystem.hpp"
#include "PhysicsSystem.hpp"
#include "../common/Common.hpp"
#include "../common/Messages.hpp"
#include <entt/entt.hpp>
#include <msgpack.hpp>
#include <map>
#include <vector>

struct _ENetHost;
struct _ENetPeer;
struct _ENetPacket;

class GameServer {
public:
        struct Client {
                explicit Client(_ENetPeer* peer = nullptr, entt::entity entity = entt::null);

                _ENetPeer* peer;
                entt::entity entity;
        };

        explicit GameServer();

        void update(sf::Time delta);

        bool create(sf::Uint16 port);
        bool isRunning() const;

private:
        void onConnect(_ENetPeer& peer);
        void onDisconnect(_ENetPeer& peer);
        void onDisconnectTimeout(_ENetPeer& peer);
        void onReceive(_ENetPeer& peer, _ENetPacket& packet);

        void onReceiveInput(_ENetPeer& peer, ClientHeader const& header, msgpack::unpacker& unpacker);

        void updateClients(sf::Time delta);
        void broadcastState();

        void nextTick();
        sf::Uint32 getTick() const;

private:
        _ENetHost* m_host;
        bool m_running;
        sf::Uint32 m_tickCounter;

        std::map<sf::Uint32, Client> m_clients;
        entt::registry m_registry;
        InputSystem m_inputSystem;
        PhysicsSystem m_physicsSystem;
};

#endif

#ifndef AM_GAME_SERVER_HPP
#define AM_GAME_SERVER_HPP

#include "../Common.hpp"
#include <map>

struct _ENetHost;
struct _ENetPeer;
struct _ENetPacket;

class GameServer {
public:
        struct Client {
                explicit Client(_ENetPeer* peer = nullptr);

                _ENetPeer* peer;
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

        void nextTick();
        sf::Uint32 getTick() const;

private:
        _ENetHost* m_host;
        bool m_running;
        sf::Uint32 m_tickCounter;

        std::map<sf::Uint32, Client> m_clients;
};

#endif

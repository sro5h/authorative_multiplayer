#ifndef GAME_CLIENT_HPP_INCLUDED
#define GAME_CLIENT_HPP_INCLUDED

#include "Common.hpp"
#include "Host.hpp"
#include <SFML/Graphics.hpp>
#include <map>

class GameClient
{
public:
        explicit GameClient();

        void update(sf::Time time);
        void draw();

        bool create(Uint16 port);
        bool connect(const std::string& address, Uint16 port);
        bool isRunning();

private:
        void processInput();

        void onConnect(Peer& peer);
        void onDisconnect(Peer& peer);
        void onReceive(Peer& peer, Packet& packet);

        void onReceiveState(Peer& peer, Packet& packet);

private:
        sf::RenderWindow mWindow;

        Host mHost;
        bool mRunning;

        Uint32 mPlayerId;
        Peer mPeer;
        PlayerState mPlayerState;

        std::map<Uint32, PlayerState> mOtherPlayers;
};

#endif // GAME_CLIENT_HPP_INCLUDED

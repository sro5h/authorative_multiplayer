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

        bool create();
        bool connect(const std::string& address, Uint16 port);
        bool isRunning();

private:
        void processInput();

        void onConnect(Peer& peer);
        void onDisconnect(Peer& peer);
        void onReceive(Peer& peer, Packet& packet);

private:
        const Uint16 mPort;

        sf::RenderWindow mWindow;

        Host mHost;
        Peer mPeer;
        bool mRunning;

        std::map<Uint16, PlayerState> mPlayers;
};

#endif // GAME_CLIENT_HPP_INCLUDED

#ifndef GAME_CLIENT_HPP_INCLUDED
#define GAME_CLIENT_HPP_INCLUDED

#include "Common.hpp"
#include "Host.hpp"
#include <SFML/Graphics.hpp>
#include <map>

class GameClient
{
public:
        struct Player
        {
                explicit Player() = default;

                sf::Vector2f pos;
                sf::Vector2f vel;
        };

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
        Peer mPeer;
        bool mRunning;

        std::map<Uint32, Player> mPlayers;
};

#endif // GAME_CLIENT_HPP_INCLUDED

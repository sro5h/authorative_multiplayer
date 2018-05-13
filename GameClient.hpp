#ifndef GAME_CLIENT_HPP_INCLUDED
#define GAME_CLIENT_HPP_INCLUDED

#include "Common.hpp"
#include "Host.hpp"
#include <SFML/Graphics.hpp>
#include <map>
#include <deque>

class GameClient
{
public:
        // this state * input = nextState
        struct Prediction
        {
                Uint32 tick;
                PlayerState state;
                PlayerInput input;
        };

        explicit GameClient();

        void update(sf::Time time);
        void draw();

        bool create(Uint16 port);
        bool connect(const std::string& address, Uint16 port);
        bool isRunning();

private:
        void onConnect(Peer& peer);
        void onDisconnect(Peer& peer);
        void onReceive(Peer& peer, Packet& packet);

        void onReceiveState(Peer& peer, Packet& packet);

        void processInput(sf::Time delta);
        void nextTick();
        Uint32 getTick();
private:
        sf::RenderWindow mWindow;

        Host mHost;
        bool mRunning;
        Uint32 mTickCounter;

        Uint32 mPlayerId;
        Peer mPeer;
        PlayerState mPlayerState;
        std::deque<Prediction> mPredictions;

        std::map<Uint32, PlayerState> mOtherPlayers;

        // For debug purposes
        PlayerState mServerState;
};

#endif // GAME_CLIENT_HPP_INCLUDED

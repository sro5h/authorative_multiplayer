#include "GameServer.hpp"
#include <iostream>
#include <cassert>

GameServer::Player::Player(Peer peer)
        : peer(peer)
{
}

GameServer::GameServer()
        : mPort(PORT_SV)
        , mRunning(true)
{
}

void GameServer::update(sf::Time delta)
{
        Event event;
        while (mHost.pollEvent(event))
        {
                logEvent(event);

                if (event.type == Event::Type::Connect)
                {
                        onConnect(event.peer);
                }
                else if (event.type == Event::Type::Disconnect)
                {
                        onDisconnect(event.peer);
                }
                else if (event.type == Event::Type::Receive)
                {
                        onReceive(event.peer, event.packet);
                }
        }

        updatePlayers(delta);
        broadcastState();
}

void GameServer::onConnect(Peer& peer)
{
        assert(mPlayers.find(peer.connectId) == mPlayers.end());

        mPlayers.insert({ peer.connectId, Player(peer) });
}

void GameServer::onDisconnect(Peer& peer)
{
        assert(mPlayers.find(peer.connectId) != mPlayers.end());

        mPlayers.erase(peer.connectId);
}

void GameServer::onReceive(Peer& peer, Packet& packet)
{
        assert(mPlayers.find(peer.connectId) != mPlayers.end());
        assert(packet.getDataSize() > 0);

        ClientMessage msgType;
        packet >> msgType;

        switch (msgType)
        {
        case ClientMessage::Input:
                onReceiveInput(peer, packet);
                break;

        default:
                std::cerr << "Unknown message type ";
                std::cerr << static_cast<Uint8>(msgType) << std::endl;
                break;
        }
}

void GameServer::onReceiveInput(Peer& peer, Packet& packet)
{
        Uint8 data;
        packet >> data;

        PlayerInput& input = mPlayers[peer.connectId].lastInput;
        input.right = data & 0x1;
        input.left  = data & 0x2;
        input.up    = data & 0x4;
        input.down  = data & 0x8;
}

void GameServer::updatePlayers(sf::Time delta)
{
        for (auto& item: mPlayers)
        {
                const PlayerInput& input = item.second.lastInput;
                sf::Vector2f accel;

                if (input.right)
                        accel.x += ACCELERATION;

                if (input.left)
                        accel.x -= ACCELERATION;

                if (input.up)
                        accel.y -= ACCELERATION;

                if (input.down)
                        accel.y += ACCELERATION;

                PlayerState& state = item.second.state;
                state.velocity += accel * delta.asSeconds();
                float friction = 1 / (1 + FRICTION * delta.asSeconds());
                state.velocity *= friction;
                state.position += state.velocity * delta.asSeconds();
        }
}

void GameServer::broadcastState()
{
        for (const auto& item: mPlayers)
        {
                Packet packet;
                packet << ServerMessage::State;

                packet << item.first;
                packet << item.second.state.position.x;
                packet << item.second.state.position.y;

                for (const auto& other: mPlayers)
                {
                        if (other.first == item.first) continue;

                        packet << other.first;
                        packet << other.second.state.position.x;
                        packet << other.second.state.position.y;
                }

                mHost.send(item.second.peer, packet);
        }
}

bool GameServer::create()
{
        if (!mHost.create("localhost", mPort, 5))
        {
                std::cerr << "Could not create the host" << std::endl;
                return false;
        }

        return true;
}

bool GameServer::isRunning()
{
        return mRunning;
}

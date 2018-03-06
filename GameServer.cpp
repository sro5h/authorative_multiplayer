#include "GameServer.hpp"
#include <iostream>
#include <cassert>

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

        if (!mPlayers.empty())
        {
                Packet packet;
                packet << ServerMessage::State;

                for (const auto& pair: mPlayers)
                {
                        packet << pair.first;
                        packet << pair.second.position.x;
                        packet << pair.second.position.y;
                }

                mHost.broadcast(packet);
        }
}

void GameServer::onConnect(Peer& peer)
{
        assert(mPlayers.find(peer.connectId) == mPlayers.end());
        assert(mPlayerInputs.find(peer.connectId) == mPlayerInputs.end());

        mPlayers.insert({ peer.connectId, PlayerState() });
        mPlayerInputs.insert({ peer.connectId, PlayerInput() });
}

void GameServer::onDisconnect(Peer& peer)
{
        assert(mPlayers.find(peer.connectId) != mPlayers.end());
        assert(mPlayerInputs.find(peer.connectId) != mPlayerInputs.end());

        mPlayerInputs.erase(peer.connectId);
        mPlayers.erase(peer.connectId);
}

void GameServer::onReceive(Peer& peer, Packet& packet)
{
        assert(mPlayerInputs.find(peer.connectId) != mPlayerInputs.end());
        assert(packet.getDataSize() > 0);

        ClientMessage msgType;
        packet >> msgType;

        switch (msgType)
        {
                case ClientMessage::Input:
                {
                        onReceiveInput(peer, packet);
                }
                break;

                default:
                {
                        std::cerr << "Unknown message type ";
                        std::cerr << static_cast<Uint8>(msgType) << std::endl;
                }
                break;
        }
}

void GameServer::onReceiveInput(Peer& peer, Packet& packet)
{
        Uint8 data;
        packet >> data;

        PlayerInput& input = mPlayerInputs[peer.connectId];
        input.right = data & 0x1;
        input.left  = data & 0x2;
        input.up    = data & 0x4;
        input.down  = data & 0x8;
}

void GameServer::updatePlayers(sf::Time delta)
{
        for (auto& pair: mPlayerInputs)
        {
                sf::Vector2f accel;
                if (pair.second.right)
                        accel.x += ACCELERATION;

                if (pair.second.left)
                        accel.x -= ACCELERATION;

                if (pair.second.up)
                        accel.y -= ACCELERATION;

                if (pair.second.down)
                        accel.y += ACCELERATION;

                PlayerState& state = mPlayers[pair.first];
                state.velocity += accel * delta.asSeconds();
                float friction = 1 / (1 + FRICTION * delta.asSeconds());
                state.velocity *= friction;
                state.position += state.velocity * delta.asSeconds();
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

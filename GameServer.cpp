#include "GameServer.hpp"
#include <iostream>
#include <cassert>

GameServer::Player::Player(Peer peer)
        : peer(peer)
        , lastInputTick(0)
{
}

GameServer::GameServer()
        : mPort(PORT_SV)
        , mHost(5, 5)
        , mRunning(true)
        , mTickCounter(0)
{
}

void GameServer::update(sf::Time delta)
{
        nextTick();

        mHost.update(getTick());

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
        assert(packet.getSize() > 0);

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
        Uint32 tick;
        Uint8 data;
        packet >> tick >> data;

        PlayerInput input;
        input.left  = data & 0x1;
        input.right = data & 0x2;
        input.up    = data & 0x4;
        input.down  = data & 0x8;

        assert(tick > mPlayers[peer.connectId].lastInputTick);

        mPlayers[peer.connectId].inputs.push_back(input);
        mPlayers[peer.connectId].lastInputTick = tick;
}

void GameServer::updatePlayers(sf::Time delta)
{
        for (auto& item: mPlayers)
        {
                PlayerState& state = item.second.state;

                if (!item.second.inputs.empty())
                {
                        const PlayerInput& input = item.second.inputs.back();

                        applyPlayerInput(delta, input, state);
                        item.second.inputs.clear();
                }

                updatePlayerState(delta, state);
        }
}

void GameServer::broadcastState()
{
        for (const auto& item: mPlayers)
        {
                Packet packet;
                packet << ServerMessage::State;

                packet << item.first;
                packet << item.second.lastInputTick;
                packet << item.second.state.position;
                packet << item.second.state.velocity;

                for (const auto& other: mPlayers)
                {
                        if (other.first == item.first) continue;

                        packet << other.first;
                        packet << other.second.state.position;
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

void GameServer::nextTick()
{
        ++mTickCounter;
}

Uint32 GameServer::getTick()
{
        return mTickCounter;
}

#include "GameClient.hpp"
#include <iostream>
#include <cassert>

GameClient::GameClient()
        : mWindow(sf::VideoMode(400, 400), "App")
        , mRunning(true)
{
}

void GameClient::update(sf::Time)
{
        sf::Event sfEvent;
        while (mWindow.pollEvent(sfEvent))
        {
                if (sfEvent.type == sf::Event::Closed)
                {
                        mHost.disconnectAll();
                        mWindow.close();
                        mRunning = false;
                }
        }

        if (mPeer)
        {
                processInput();
        }

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
}

void GameClient::onConnect(Peer& peer)
{
        assert(mPlayers.find(peer.connectId) == mPlayers.end());

        mPeer = peer;
        mPlayers.insert({ mPeer.connectId, PlayerState() });
}

void GameClient::onDisconnect(Peer& peer)
{
        assert(mPeer == peer);
        assert(mPlayers.find(peer.connectId) != mPlayers.end());

        mPlayers.erase(mPeer.connectId);
}

void GameClient::onReceive(Peer& peer, Packet& packet)
{
        assert(mPeer == peer);
        assert(packet.getDataSize() > 0);

        ServerMessage msgType;
        packet >> msgType;

        switch (msgType)
        {
        case ServerMessage::State:
                onReceiveState(peer, packet);
                break;

        default:
                std::cerr << "Unknown message type ";
                std::cerr << static_cast<Uint8>(msgType) << std::endl;
                break;
        }
}

void GameClient::onReceiveState(Peer&, Packet& packet)
{
        Uint32 connectId;
        float x, y;

        while (packet >> connectId >> x >> y)
        {
                if (mPlayers.find(connectId) == mPlayers.end())
                {
                        std::cerr << "Received state of unknown player ";
                        std::cerr << connectId << std::endl;
                        continue;
                }

                PlayerState& state = mPlayers[connectId];
                state.position.x = x;
                state.position.y = y;
        }
}

void GameClient::processInput()
{
        Packet packet;
        Uint8 input = 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && mWindow.hasFocus())
                input |= 0x1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && mWindow.hasFocus())
                input |= 0x2;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && mWindow.hasFocus())
                input |= 0x4;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && mWindow.hasFocus())
                input |= 0x8;

        packet << ClientMessage::Input;
        packet << input;
        mHost.send(mPeer, packet);
}

void GameClient::draw()
{
        mWindow.clear();

        for (const auto& pair: mPlayers)
        {
                sf::CircleShape shape(20.0f);
                shape.setPosition(pair.second.position);

                if (pair.first == mPeer.connectId)
                        shape.setFillColor(sf::Color::Green);

                mWindow.draw(shape);
        }

        mWindow.display();
}

bool GameClient::create(Uint16 port)
{
        if (!mHost.create("localhost", port, 5))
        {
                std::cerr << "Could not create the host" << std::endl;
                return false;
        }

        return true;
}

bool GameClient::connect(const std::string& address, Uint16 port)
{
        if (!mHost.connect(address, port))
        {
                std::cerr << "Failed to attempt a connection" << std::endl;
                return false;
        }

        return true;
}

bool GameClient::isRunning()
{
        return mRunning;
}

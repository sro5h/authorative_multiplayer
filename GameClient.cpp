#include "GameClient.hpp"
#include <iostream>
#include <cassert>

GameClient::GameClient()
        : mPort(PORT_CL)
        , mWindow(sf::VideoMode(400, 400), "App")
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
        assert(mPlayers.find(peer.outgoingId) == mPlayers.end());

        mPeer = peer;
        mPlayers.insert({ mPeer.outgoingId, PlayerState() });
}

void GameClient::onDisconnect(Peer& peer)
{
        assert(mPlayers.find(peer.outgoingId) != mPlayers.end());

        mPlayers.erase(mPeer.outgoingId);
}

void GameClient::onReceive(Peer&, Packet& packet)
{
        Uint16 peerId;
        while (packet >> peerId)
        {
                assert(mPlayers.find(peerId) != mPlayers.end());

                PlayerState& state = mPlayers.at(peerId);

                packet >> state.position.x;
                packet >> state.position.y;
        }
}

void GameClient::processInput()
{
        Packet packet;
        Uint8 input = 0;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                input |= 0x1;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                input |= 0x2;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                input |= 0x4;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                input |= 0x8;

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

                if (pair.first == mPeer.outgoingId)
                        shape.setFillColor(sf::Color::Green);

                mWindow.draw(shape);
        }

        mWindow.display();
}

bool GameClient::create()
{
        if (!mHost.create("localhost", mPort, 5))
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

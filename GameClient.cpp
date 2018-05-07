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
        mPlayers.insert({ mPeer.connectId, Player() });
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
        StateMessage message;

        while (packet >> message)
        {
                if (mPlayers.find(message.id) == mPlayers.end())
                {
                        std::cerr << "Received state of unknown player ";
                        std::cerr << message.id << std::endl;
                        continue;
                }

                Player& player = mPlayers[message.id];
                player.pos = message.pos;
        }
}

void GameClient::processInput()
{
        Packet packet;
        InputMessage message;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && mWindow.hasFocus())
                message.right = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && mWindow.hasFocus())
                message.left = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && mWindow.hasFocus())
                message.up = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && mWindow.hasFocus())
                message.down = true;

        packet << ClientMessage::Input;
        packet << message;
        mHost.send(mPeer, packet);
}

void GameClient::draw()
{
        mWindow.clear();

        for (const auto& pair: mPlayers)
        {
                sf::CircleShape shape(20.0f);
                shape.setPosition(pair.second.pos);

                if (pair.first == mPeer.connectId)
                        shape.setFillColor(sf::Color(64, 64, 157));
                else
                        shape.setFillColor(sf::Color(42, 42, 42));

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

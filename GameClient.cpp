#include "GameClient.hpp"
#include <iostream>
#include <cassert>

GameClient::GameClient()
        : mWindow(sf::VideoMode(400, 400), "App")
        , mRunning(true)
        , mPlayerId(0)
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
        assert(mPlayerId == 0);

        mPlayerId = peer.connectId;
        mPeer = peer;
        mPlayerState = PlayerState();
}

void GameClient::onDisconnect(Peer& peer)
{
        assert(mPeer == peer);
        assert(mPlayerId == peer.connectId);

        mPlayerId = 0;
        mPeer = Peer();
        mPlayerState = PlayerState();
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
        packet >> connectId >> x >> y;

        assert(mPlayerId == connectId);

        mPlayerState.position.x = x;
        mPlayerState.position.y = y;

        while (packet >> connectId >> x >> y)
        {
                if (mOtherPlayers.find(connectId) == mOtherPlayers.end())
                {
                        std::cerr << "Received state of unknown player ";
                        std::cerr << connectId << std::endl;
                        continue;
                }

                PlayerState& state = mOtherPlayers[connectId];
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

        for (const auto& item: mOtherPlayers)
        {
                sf::CircleShape shape(20.0f);
                shape.setPosition(item.second.position);
                shape.setFillColor(sf::Color(42, 42, 42));

                mWindow.draw(shape);
        }

        if (mPlayerId != 0)
        {
                sf::CircleShape shape(20.0f);
                shape.setPosition(mPlayerState.position);
                shape.setFillColor(sf::Color(64, 64, 157));

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

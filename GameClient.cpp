#include "GameClient.hpp"
#include <iostream>
#include <cassert>

GameClient::GameClient()
        : mWindow(sf::VideoMode(400, 400), "App")
        , mRunning(true)
        , mTickCounter(0)
        , mPlayerId(0)
{
}

void GameClient::update(sf::Time delta)
{
        nextTick();

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

        if (mPeer)
        {
                processInput(delta);
        }
}

void GameClient::onConnect(Peer& peer)
{
        assert(mPlayerId == 0);

        mPlayerId = peer.connectId;
        mPeer = peer;
        mPlayerState = PlayerState();
        mServerState = PlayerState();
}

void GameClient::onDisconnect(Peer& peer)
{
        assert(mPeer == peer);
        assert(mPlayerId == peer.connectId);

        mPlayerId = 0;
        mPeer = Peer();
        mPlayerState = PlayerState();
        mServerState = PlayerState();
}

void GameClient::onReceive(Peer& peer, Packet& packet)
{
        assert(mPeer == peer);
        assert(packet.getSize() > 0);

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
        Uint32 lastTick;
        sf::Vector2f pos, vel;
        PlayerState receivedState;
        packet >> connectId >> lastTick >> pos >> vel;

        assert(mPlayerId == connectId);

        receivedState.position = pos;
        receivedState.velocity = vel;

        mServerState = receivedState;

        if (mPredictions.empty())
        {
                // No predicted state, just apply received state
                mPlayerState = receivedState;
        }
        else
        {
                // Goto relevant prediction
                while (!mPredictions.empty() && mPredictions.front().tick < lastTick)
                        mPredictions.pop_front();

                assert(!mPredictions.empty());

                // Check if predicited state is correct
                if (!equals(mPredictions.front().state, receivedState))
                {
                        mPlayerState = receivedState;

                        // Correct prediction, apply all inputs, starting from
                        // mPredictions.front().input
                        for (const auto& prediction: mPredictions)
                        {
                                applyPlayerInput(TIME_PER_TICK, prediction.input, mPlayerState);
                                updatePlayerState(TIME_PER_TICK, mPlayerState);
                        }
                }
                else
                {
                        std::cout << "Prediction was correct :)" << std::endl;
                }

                // Prediction was handled
                mPredictions.pop_front();
        }

        while (packet >> connectId >> pos)
        {
                if (mOtherPlayers.find(connectId) == mOtherPlayers.end())
                {
                        std::cerr << "Received state of unknown player ";
                        std::cerr << connectId << std::endl;
                        continue;
                }

                PlayerState& state = mOtherPlayers[connectId];
                state.position = pos;
        }
}

void GameClient::processInput(sf::Time delta)
{
        PlayerInput input;
        Uint32 tick = getTick();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && mWindow.hasFocus())
                input.left = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && mWindow.hasFocus())
                input.right = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && mWindow.hasFocus())
                input.up = true;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && mWindow.hasFocus())
                input.down = true;

        // Store old state and input
        mPredictions.push_back({ tick, mPlayerState, input });

        applyPlayerInput(delta, input, mPlayerState);
        updatePlayerState(delta, mPlayerState);

        // Send input
        Packet packet;
        Uint8 data = 0;

        if (input.left)
                data |= 0x1;

        if (input.right)
                data |= 0x2;

        if (input.up)
                data |= 0x4;

        if (input.down)
                data |= 0x8;

        packet << ClientMessage::Input;
        packet << getTick();
        packet << data;
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

                sf::CircleShape serverState(20.0f);
                serverState.setPosition(mServerState.position);
                serverState.setFillColor(sf::Color::Transparent);
                serverState.setOutlineColor(sf::Color(124, 124, 217));
                serverState.setOutlineThickness(1.0f);

                mWindow.draw(shape);
                mWindow.draw(serverState);
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

void GameClient::nextTick()
{
        mTickCounter++;
}

Uint32 GameClient::getTick()
{
        return mTickCounter;
}

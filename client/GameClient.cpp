#include "GameClient.hpp"
#include "../common/Messages.hpp"
#include "../enet.h"
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <iostream>
#include <cassert>

GameClient::GameClient()
        : m_window{sf::VideoMode(400, 400), "Client"}
        , m_host{nullptr}
        , m_running{true}
        , m_tickCounter{0}
        , m_peer{nullptr} {
}

void GameClient::update(sf::Time delta) {
        nextTick();

        sf::Event windowEvent;
        while (m_window.pollEvent(windowEvent)) {
                switch (windowEvent.type) {
                case sf::Event::Closed:
                        disconnect();
                        m_window.close();
                        m_running = false;
                        break;
                }
        }

        ENetEvent event;
        while (enet_host_service(m_host, &event, 0) > 0) {
                switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:
                        onConnect(*event.peer);
                        break;

                case ENET_EVENT_TYPE_DISCONNECT:
                        onDisconnect(*event.peer);
                        break;

                case ENET_EVENT_TYPE_DISCONNECT_TIMEOUT:
                        onDisconnectTimeout(*event.peer);
                        break;

                case ENET_EVENT_TYPE_RECEIVE:
                        onReceive(*event.peer, *event.packet);
                        enet_packet_destroy(event.packet);
                        break;
                }
        }

        if (m_peer) {
                processInput(delta);
        }
}

void GameClient::render(sf::Time delta) {
        m_window.clear();

        if (m_peer) {
                sf::CircleShape shape(20.0f);
                shape.setPosition(m_localState.position);
                shape.setFillColor(sf::Color(64, 64, 157));

                m_window.draw(shape);
        }

        m_window.display();
}

void GameClient::onConnect(ENetPeer& peer) {
        m_peer = &peer;
        m_localState = State{};
        m_serverState = State{};
        std::cout << "[client] onConnect " << peer.connectID << std::endl;
}

void GameClient::onDisconnect(ENetPeer& peer) {
        assert(m_peer == &peer);

        m_peer = nullptr;
        m_localState = State{};
        m_serverState = State{};
        std::cout << "[client] onDisconnect " << peer.connectID << std::endl;
}

void GameClient::onDisconnectTimeout(ENetPeer& peer) {
        std::cout << "[client] onDisconnectTimeout" << std::endl;
        onDisconnect(peer);
}

void GameClient::onReceive(ENetPeer& peer, ENetPacket& packet) {
        assert(m_peer == &peer);
        assert(packet.dataLength > 0);
        std::cout << "[client] onReceive" << std::endl;
}

void GameClient::processInput(sf::Time delta) {
        Input input;
        if (m_window.hasFocus()) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                        input.left = true;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                        input.right = true;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                        input.up = true;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                        input.down = true;
                }
        }

        // Send input message
        msgpack::sbuffer buffer;
        msgpack::pack(buffer, ClientHeader{
                        ClientHeader::MessageType::Input,
                        getTick()
        });
        msgpack::pack(buffer, InputMessage(input));

        ENetPacket* packet = enet_packet_create(
                        buffer.data(),
                        buffer.size(),
                        ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT
        );
        enet_peer_send(m_peer, 0, packet);

}

bool GameClient::create(sf::Uint16 port) {
        assert(m_host == nullptr);

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = port;
        m_host = enet_host_create(&address, 2, 2, 0, 0);

        return m_host != nullptr;
}

bool GameClient::connect(std::string const& address, sf::Uint16 port) {
        assert(m_host);

        ENetAddress enetAddress;
        if (address.empty()) {
                enetAddress.host = ENET_HOST_ANY;
        } else {
                if (enet_address_set_host(&enetAddress, address.c_str()) != 0) {
                        return false;
                }
        }

        enetAddress.port = port;
        m_peer = enet_host_connect(m_host, &enetAddress, 2, 0);

        return m_peer != nullptr;
}

void GameClient::disconnect() {
        assert(m_host);
        assert(m_peer);
        enet_peer_disconnect(m_peer, 0);
}

bool GameClient::isRunning() const {
        return m_running;
}

void GameClient::nextTick() {
        ++m_tickCounter;
}

sf::Uint32 GameClient::getTick() const {
        return m_tickCounter;
}

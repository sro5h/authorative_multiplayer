#include "GameClient.hpp"
#include "../enet.h"
#include <iostream>
#include <cassert>

GameClient::GameClient()
        : m_host{nullptr}
        , m_running{true}
        , m_tickCounter{0}
        , m_playerId{0}
        , m_peer{nullptr} {
}

void GameClient::update(sf::Time delta) {
        nextTick();

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
}

void GameClient::render(sf::Time delta) {
}

void GameClient::onConnect(ENetPeer& peer) {
        std::cout << "[client] onConnect" << std::endl;
}

void GameClient::onDisconnect(ENetPeer& peer) {
        std::cout << "[client] onDisconnect" << std::endl;
}

void GameClient::onDisconnectTimeout(ENetPeer& peer) {
        std::cout << "[client] onDisconnectTimeout" << std::endl;
        onDisconnect(peer);
}

void GameClient::onReceive(ENetPeer& peer, ENetPacket& packet) {
        std::cout << "[client] onReceive" << std::endl;
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

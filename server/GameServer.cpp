#include "GameServer.hpp"
#include "../enet.h"
#include <iostream>
#include <cassert>

GameServer::Client::Client(ENetPeer* peer)
        : peer{peer} {
}

GameServer::GameServer()
        : m_host{nullptr}
        , m_running{true}
        , m_tickCounter{0}
        , m_clients{} {
}

void GameServer::update(sf::Time delta) {
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

void GameServer::onConnect(ENetPeer& peer) {
        assert(m_clients.find(peer.connectID) == m_clients.end());

        m_clients.insert({ peer.connectID, Client(&peer) });
        std::cout << "[server] onConnect" << std::endl;
}

void GameServer::onDisconnect(ENetPeer& peer) {
        assert(m_clients.find(peer.connectID) != m_clients.end());

        m_clients.erase(peer.connectID);
        std::cout << "[server] onDisconnect" << std::endl;
}

void GameServer::onDisconnectTimeout(ENetPeer& peer) {
        std::cout << "[server] onDisconnectTimeout" << std::endl;
        onDisconnect(peer);
}

void GameServer::onReceive(ENetPeer& peer, ENetPacket& packet) {
        assert(m_clients.find(peer.connectID) != m_clients.end());
        assert(packet.dataLength > 0);
        std::cout << "[server] onReceive" << std::endl;
}

bool GameServer::create(sf::Uint16 port) {
        assert(m_host == nullptr);

        ENetAddress address;
        address.host = ENET_HOST_ANY;
        address.port = port;
        m_host = enet_host_create(&address, 10, 2, 0, 0);

        return m_host != nullptr;
}

bool GameServer::isRunning() const {
        return m_running;
}

void GameServer::nextTick() {
        ++m_tickCounter;
}

sf::Uint32 GameServer::getTick() const {
        return m_tickCounter;
}

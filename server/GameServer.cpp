#include "GameServer.hpp"
#include "../common/Messages.hpp"
#include "../enet.h"
#include <iostream>
#include <cassert>

GameServer::Client::Client(ENetPeer* peer)
        : peer{peer}
        , state{}
        , lastInputTick{0}
        , inputs{} {
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

        updateClients(delta);
        broadcastState();
}

void GameServer::onConnect(ENetPeer& peer) {
        assert(m_clients.find(peer.connectID) == m_clients.end());

        m_clients.insert({ peer.connectID, Client(&peer) });
        std::cout << "[server] onConnect " << peer.connectID << std::endl;
}

void GameServer::onDisconnect(ENetPeer& peer) {
        assert(m_clients.find(peer.connectID) != m_clients.end());

        m_clients.erase(peer.connectID);
        std::cout << "[server] onDisconnect " << peer.connectID << std::endl;
}

void GameServer::onDisconnectTimeout(ENetPeer& peer) {
        std::cout << "[server] onDisconnectTimeout" << std::endl;
        onDisconnect(peer);
}

void GameServer::onReceive(ENetPeer& peer, ENetPacket& packet) {
        assert(m_clients.find(peer.connectID) != m_clients.end());
        assert(packet.dataLength > 0);

        // !TODO: Eliminate copying of packet data
        msgpack::unpacker unpacker;
        unpacker.reserve_buffer(packet.dataLength);
        std::memcpy(unpacker.buffer(), packet.data, packet.dataLength);
        unpacker.buffer_consumed(packet.dataLength);

        msgpack::object_handle handle;
        unpacker.next(handle);
        msgpack::object object = handle.get();
        ClientHeader header = object.as<ClientHeader>();

        switch (header.messageType) {
        case ClientHeader::MessageType::Input:
                onReceiveInput(peer, header, unpacker);
                break;
        }
}

void GameServer::onReceiveInput(ENetPeer& peer, ClientHeader const& header, msgpack::unpacker& unpacker) {
        Client& client = m_clients[peer.connectID];
        msgpack::object_handle handle;
        unpacker.next(handle);
        InputMessage message = handle.get().as<InputMessage>();

        if (header.tick > client.lastInputTick) {
                client.inputs.push_back(message.input);
                client.lastInputTick = header.tick;
        }
}

void GameServer::updateClients(sf::Time delta) {
        for (auto& item: m_clients) {
                Client& client = item.second;

                if (!client.inputs.empty()) {
                        applyInput(delta, client.inputs.back(), client.state);
                        client.inputs.clear();
                }

                updateState(delta, client.state);
        }
}

void GameServer::broadcastState() {
        for (auto const& item: m_clients) {
                Client const& client = item.second;

                msgpack::sbuffer buffer;
                msgpack::pack(buffer, ServerHeader{
                                ServerHeader::MessageType::State,
                                getTick()
                });
                msgpack::pack(buffer, StateMessage(client.state));

                ENetPacket* packet = enet_packet_create(
                                buffer.data(),
                                buffer.size(),
                                ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT
                );
                enet_peer_send(client.peer, 0, packet);
        }
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

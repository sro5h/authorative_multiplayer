#include "Common.hpp"
#include "Event.hpp"
#include <iostream>

StateMessage::StateMessage()
        : pos(0.0f, 0.0f)
{
}

InputMessage::InputMessage()
        : right(false)
        , left(false)
        , up(false)
        , down(false)
{
}

Packet& operator<<(Packet& p, const ClientMessage& msg)
{
        return p << static_cast<Uint8>(msg);
}

Packet& operator>>(Packet& p, ClientMessage& msg)
{
        Uint8 data;
        if (p >> data)
                msg = static_cast<ClientMessage>(data);

        return p;
}

Packet& operator<<(Packet& p, const ServerMessage& msg)
{
        return p << static_cast<Uint8>(msg);
}

Packet& operator>>(Packet& p, ServerMessage& msg)
{
        Uint8 data;
        if (p >> data)
                msg = static_cast<ServerMessage>(data);

        return p;
}

Packet& operator<<(Packet& p, const StateMessage& msg)
{
        return p << msg.id << msg.pos.x << msg.pos.y;
}

Packet& operator>>(Packet& p, StateMessage& msg)
{
        Uint32 id;
        float x, y;
        if (p >> id >> x >> y)
        {
                msg.id = id;
                msg.pos.x = x;
                msg.pos.y = y;
        }

        return p;
}

Packet& operator<<(Packet& p, const InputMessage& msg)
{
        Uint8 data = 0;
        if (msg.right)
                data |= 0x1;

        if (msg.left)
                data |= 0x2;

        if (msg.up)
                data |= 0x4;

        if (msg.down)
                data |= 0x8;

        return p << data;
}

Packet& operator>>(Packet& p, InputMessage& msg)
{
        Uint8 data;
        if (p >> data)
        {
                msg.right = data & 0x1;
                msg.left  = data & 0x2;
                msg.up    = data & 0x4;
                msg.down  = data & 0x8;
        }

        return p;
}

void logEvent(const Event& event)
{
        if (event.type == Event::Type::Connect)
        {
                std::cout << "Connect";
                std::cout << " from peer with id " << event.peer.connectId;
                std::cout << std::endl;
        }
        else if (event.type == Event::Type::Disconnect)
        {
                std::cout << "Disconnect";
                std::cout << " from peer with id " << event.peer.connectId;
                std::cout << std::endl;
        }
}

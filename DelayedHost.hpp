#ifndef DELAYED_HOST_HPP_INCLUDED
#define DELAYED_HOST_HPP_INCLUDED

#include "Host.hpp"
#include <queue>

struct QueuedPacket
{
        QueuedPacket(const Peer& peer, const Packet& packet, Uint32 tick);

        Peer peer;
        Packet packet;
        Uint32 tick;
};

struct QueuedEvent
{
        QueuedEvent(const Event& event, Uint32 tick);

        Event event;
        Uint32 tick;
};

class DelayedHost : public Host
{
public:
        explicit DelayedHost(Uint32 delayOutgoing, Uint32 delayIncoming);

        void update(Uint32 tick);
        bool pollEvent(Event& event) override;
        bool send(const Peer& peer, const Packet& packet) override;

private:
        const Uint32 mDelayOutgoing;
        const Uint32 mDelayIncoming;
        Uint32 mCurrentTick;

        std::queue<QueuedPacket> mOutgoingQueue;
        std::queue<QueuedEvent> mIncomingQueue;
};

#endif // DELAYED_HOST_HPP_INCLUDED

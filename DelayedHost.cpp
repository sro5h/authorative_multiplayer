#include "DelayedHost.hpp"
#include <cassert>

QueuedPacket::QueuedPacket(const Peer& peer, const Packet& packet, Uint32 tick)
        : peer(peer)
        , packet(packet)
        , tick(tick)
{
}

QueuedEvent::QueuedEvent(const Event& event, Uint32 tick)
        : event(event)
        , tick(tick)
{
}

DelayedHost::DelayedHost(Uint32 delayOutgoing, Uint32 delayIncoming)
        : mDelayOutgoing(delayOutgoing)
        , mDelayIncoming(delayIncoming)
        , mCurrentTick(0)
{
}

void DelayedHost::update(Uint32 tick)
{
        mCurrentTick = tick;

        while (!mOutgoingQueue.empty()
               && ((mOutgoingQueue.front().tick + mDelayOutgoing)
                   <= mCurrentTick))
        {
                QueuedPacket& item = mOutgoingQueue.front();
                Host::send(item.peer, item.packet);
                mOutgoingQueue.pop();
        }

        Event event;
        while (Host::pollEvent(event))
        {
                mIncomingQueue.push(QueuedEvent(event, mCurrentTick));
        }
}

bool DelayedHost::pollEvent(Event& event)
{
        if (!mIncomingQueue.empty()
            && ((mIncomingQueue.front().tick + mDelayIncoming)
                <= mCurrentTick))
        {
                event = mIncomingQueue.front().event;
                mIncomingQueue.pop();

                return true;
        }

        return false;
}

bool DelayedHost::send(const Peer& peer, const Packet& packet)
{
        assert(peer);
        assert(packet);

        mOutgoingQueue.push(QueuedPacket(peer, packet, mCurrentTick));
        return true;
}

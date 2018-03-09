#include "DelayedHost.hpp"
#include <iostream>
#include <cassert>

QueuedPacket::QueuedPacket(const Peer& peer, const Packet& packet,
                           const sf::Time time)
        : peer(peer)
        , packet(packet)
        , time(time)
{
}

QueuedEvent::QueuedEvent(const Event& event, const sf::Time time)
        : event(event)
        , time(time)
{
}

DelayedHost::DelayedHost(sf::Time delayOutgoing, sf::Time delayIncoming)
        : mDelayOutgoing(delayOutgoing)
        , mDelayIncoming(delayIncoming)
{
}

bool DelayedHost::pollEvent(Event& event)
{
        sf::Time elapsed;

        // Queue packets that should be sent
        elapsed = mClock.getElapsedTime();
        while (!mOutgoingQueue.empty()
               && (elapsed - mOutgoingQueue.front().time) >= mDelayOutgoing)
        {
                QueuedPacket& item = mOutgoingQueue.front();
                assert(item.peer);
                Host::send(item.peer, item.packet);
                mOutgoingQueue.pop();
        }

        Event tmpEvent;
        elapsed = mClock.getElapsedTime();
        while (Host::pollEvent(tmpEvent))
        {
                mIncomingQueue.push({ tmpEvent, elapsed });
        }

        if (!mIncomingQueue.empty()
             && (elapsed - mIncomingQueue.front().time) >= mDelayIncoming)
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
        mOutgoingQueue.push({ peer, packet, mClock.getElapsedTime() });
        return true;
}

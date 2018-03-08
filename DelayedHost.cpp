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

DelayedHost::DelayedHost(sf::Time delay)
        : mDelay(delay)
{
}

bool DelayedHost::pollEvent(Event& event)
{
        // Queue packets that should be sent
        sf::Time elapsed = mClock.getElapsedTime();
        while (!mOutgoingQueue.empty()
                        && elapsed - mOutgoingQueue.front().time >= mDelay)
        {
                QueuedPacket& item = mOutgoingQueue.front();
                assert(item.peer);
                Host::send(item.peer, item.packet);
                mOutgoingQueue.pop();
        }

        return Host::pollEvent(event);
}

bool DelayedHost::send(const Peer& peer, const Packet& packet)
{
        assert(peer);
        mOutgoingQueue.push({ peer, packet, mClock.getElapsedTime() });
        return true;
}

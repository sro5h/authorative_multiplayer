#ifndef DELAYED_HOST_HPP_INCLUDED
#define DELAYED_HOST_HPP_INCLUDED

#include "Host.hpp"
#include <SFML/System/Clock.hpp>
#include <queue>

struct QueuedPacket
{
        QueuedPacket(const Peer& peer, const Packet& packet,
                     const sf::Time time);

        const Peer peer;
        const Packet packet;
        const sf::Time time;
};

struct QueuedEvent
{
        QueuedEvent(const Event& event, const sf::Time time);

        const Event event;
        const sf::Time time;
};

class DelayedHost : public Host
{
public:
        explicit DelayedHost(sf::Time delayOutgoing, sf::Time delayIncoming);

        bool pollEvent(Event& event);
        bool send(const Peer& peer, const Packet& packet);

private:
        const sf::Time mDelayOutgoing;
        const sf::Time mDelayIncoming;
        sf::Clock mClock;

        std::queue<QueuedPacket> mOutgoingQueue;
        std::queue<QueuedEvent> mIncomingQueue;
};

#endif // DELAYED_HOST_HPP_INCLUDED

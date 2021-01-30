#ifndef AM_MESSAGES_HPP
#define AM_MESSAGES_HPP

#include "Common.hpp"
#include <msgpack.hpp>

struct ClientHeader {
public:
        enum class MessageType {
                Input,
        };

        explicit ClientHeader() = default;
        explicit ClientHeader(MessageType type, sf::Uint32 tick)
                : messageType{type}, tick{tick} { }

        MessageType messageType;
        sf::Uint32 tick;

        MSGPACK_DEFINE(messageType, tick);
};

MSGPACK_ADD_ENUM(ClientHeader::MessageType);

struct InputMessage {
public:
        explicit InputMessage() = default;
        explicit InputMessage(Input const& input)
                : left{input.left}, right{input.right}, up{input.up}, down{input.down} { }

        bool left, right, up, down;

        MSGPACK_DEFINE(left, right, up, down);
};

#endif

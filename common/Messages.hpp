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
                : input{input} { }

        Input input;

        MSGPACK_DEFINE(input);
};

struct ServerHeader {
public:
        enum class MessageType {
                State,
        };

        explicit ServerHeader() = default;
        explicit ServerHeader(MessageType type, sf::Uint32 tick)
                : messageType{type}, tick{tick} { }

        MessageType messageType;
        sf::Uint32 tick;

        MSGPACK_DEFINE(messageType, tick);
};

MSGPACK_ADD_ENUM(ServerHeader::MessageType);

struct StateMessage {
public:
        explicit StateMessage() = default;
        explicit StateMessage(State const& state)
                : state{state} { }

        State state;

        MSGPACK_DEFINE(state);
};

#endif

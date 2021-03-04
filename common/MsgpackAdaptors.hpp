#ifndef AM_MSGPACK_ADAPTORS_HPP
#define AM_MSGPACK_ADAPTORS_HPP

#include <msgpack.hpp>
#include <SFML/System/Vector2.hpp>

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template <>
struct convert<sf::Vector2f> {
        msgpack::object const& operator()(msgpack::object const& o, sf::Vector2f& vector) const {
                if (o.type != msgpack::type::ARRAY) throw msgpack::type_error();
                if (o.via.array.size != 2) throw msgpack::type_error();
                o.via.array.ptr[0] >> vector.x;
                o.via.array.ptr[1] >> vector.y;
                return o;
        }
};

template <>
struct pack<sf::Vector2f> {
        template <typename Stream>
        msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, sf::Vector2f const& vector) const {
                o.pack_array(2);
                o.pack(vector.x);
                o.pack(vector.y);
                return o;
        }
};

}
}
}

#endif

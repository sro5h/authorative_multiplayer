#ifndef AM_NETWORK_INITIALISER_HPP
#define AM_NETWORK_INITIALISER_HPP

class NetworkInitialiser final {
public:
        explicit NetworkInitialiser() = default;
        ~NetworkInitialiser();

        bool initialise() const;
};

#endif

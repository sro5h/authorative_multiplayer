#include "NetworkInitialiser.hpp"
#define ENET_IMPLEMENTATION
#include <enet/include/enet.h>

NetworkInitialiser::~NetworkInitialiser() {
        enet_deinitialize();
}

bool NetworkInitialiser::initialise() const {
        return enet_initialize() == 0;
}

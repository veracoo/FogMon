#ifndef IADAPTIVEFOLLOWER_CONNECTIONS_HPP_
#define IADAPTIVEFOLLOWER_CONNECTIONS_HPP_

#include "connections.hpp"

class IAdaptiveFollower;

class IAdaptiveFollowerConnections : virtual public IConnections {
public:
    virtual void initialize(IAdaptiveFollower* parent) = 0;
};

#endif
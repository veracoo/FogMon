#ifndef ADAPTIVE_FACTORY_HPP_
#define ADAPTIVE_FACTORY_HPP_

#include "factory.hpp"
#include "adaptive_follower_connections.hpp"
#include "adaptive_storage_monitoring.hpp"

class AdaptiveFactory : virtual public Factory {
public:
    virtual AdaptiveFollowerConnections* newConnections(int nThread);

    virtual IAdaptiveStorageMonitoring* newStorage(std::string path);
};

#endif
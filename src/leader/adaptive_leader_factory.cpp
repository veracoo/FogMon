#include "adaptive_leader_factory.hpp"
#include <iostream>

AdaptiveLeaderConnections* AdaptiveLeaderFactory::newConnections(int nThread){
    return new AdaptiveLeaderConnections(nThread);
}

IAdaptiveLeaderStorageMonitoring* AdaptiveLeaderFactory::newStorage(std::string path, Message::node node){
    IAdaptiveLeaderStorageMonitoring* ret = new AdaptiveLeaderStorageMonitoring(node);
    ret->open(path);
    return ret;
}
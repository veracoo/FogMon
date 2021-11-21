#ifndef I_ADAPTIVE_LEADER_HPP_
#define I_ADAPTIVE_LEADER_HPP_

#include "iadaptiveleader_connections.hpp"
#include "ileader.hpp"
#include "iadaptivefollower.hpp"
#include "iadaptiveleader_storage_monitoring.hpp"
#include "adaptive_leader_controller.hpp"

class IAdaptiveLeader : virtual public ILeader, virtual public IAdaptiveFollower {
public:
    virtual IAdaptiveLeaderConnections* getConnections() = 0;
    virtual IAdaptiveLeaderStorageMonitoring* getStorage() = 0;
    virtual AdaptiveLeaderController* getAdaptiveController() = 0;

    virtual bool getReceivedCpuLogs() = 0;
};

#endif
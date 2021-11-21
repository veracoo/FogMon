#ifndef I_ADAPTIVEFOLLOWER_HPP_
#define I_ADAPTIVEFOLLOWER_HPP_

#include "iagent.hpp"
#include "iadaptive_storage_monitoring.hpp"
#include "iadaptivefollower_connections.hpp"
#include "adaptive_controller.hpp"

class IAdaptiveFollower : virtual public IAgent {
public:
    virtual IAdaptiveFollowerConnections* getConnections() = 0;
    virtual IAdaptiveStorageMonitoring* getStorage() = 0;
    virtual AdaptiveController* getAdaptiveController() = 0;

    virtual vector<Metric> getMetrics() = 0;
    virtual bool changeServer(vector<Message::node> nodes) = 0;

    virtual void disableMetrics(vector<Metric> metrics) = 0;
    virtual void enableMetrics(vector<Metric> metrics) = 0;
};

#endif

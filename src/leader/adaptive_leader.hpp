#ifndef ADAPTIVE_LEADER_HPP
#define ADAPTIVE_LEADER_HPP

#include "leader.hpp"
#include "adaptive_follower.hpp"
#include "adaptive_leader_connections.hpp"
#include "adaptive_leader_factory.hpp"
#include "adaptive_leader_controller.hpp"

class AdaptiveLeader : virtual public IAdaptiveLeader, public Leader, public AdaptiveFollower {

public:
    static AdaptiveLeader* myobj;

    AdaptiveLeader();
    AdaptiveLeader(Message::node node, int nThreads);
    ~AdaptiveLeader();

    void initialize(AdaptiveLeaderFactory* factory = NULL);

    void start(std::vector<Message::node> mNodes);
    void stop();

    IAdaptiveLeaderConnections* getConnections();
    IAdaptiveLeaderStorageMonitoring* getStorage();

    AdaptiveLeaderController* getAdaptiveController();

    bool getReceivedCpuLogs();

protected:
    AdaptiveLeaderController *adaptive_controller;

    void timerFun() override;

    AdaptiveLeaderFactory tFactory;
    AdaptiveLeaderFactory *factory;

    AdaptiveLeaderConnections *connections;
    IAdaptiveLeaderStorageMonitoring *storage;

    bool received_cpu_logs;
};

#endif
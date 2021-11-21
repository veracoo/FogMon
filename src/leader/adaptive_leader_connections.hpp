#ifndef ADAPTIVE_LEADER_CONNECTIONS_HPP_
#define ADAPTIVE_LEADER_CONNECTIONS_HPP_

#include "leader_connections.hpp"
#include "adaptive_follower_connections.hpp"
#include "iadaptiveleader_connections.hpp"
#include "iadaptiveleader.hpp"
#include <fstream>

class AdaptiveLeaderConnections : public LeaderConnections, public AdaptiveFollowerConnections, virtual public IAdaptiveLeaderConnections {

protected:
    void handler(int fd, Message &m);
    void call_super_handler(int fd, Message &m) override;

    IAdaptiveLeader* parent;

    ofstream f;

public:
    AdaptiveLeaderConnections(int nThread);
    ~AdaptiveLeaderConnections();

    void initialize(IAdaptiveLeader* parent);

    bool sendRequestReport(Message::node ip) override;

    bool sendMReport(Message::node ip, vector<AdaptiveReport::adaptive_report_result> report);

    bool sendChangeServer();
    bool sendChangeTimeReport(Message::node ip, int newTimeReport);

    bool sendRemoveLeader(Message::leader_update update);

    bool sendDisableMetrics(Message::node ip, vector<Metric> metrics);
    bool sendEnableMetrics(Message::node ip, vector<Metric> metrics);
};

#endif
#ifndef IADAPTIVELEADER_CONNECTIONS_HPP_
#define IADAPTIVELEADER_CONNECTIONS_HPP_

#include "leader_connections.hpp"
#include "adaptive_follower_connections.hpp"

class IAdaptiveLeader;

class IAdaptiveLeaderConnections : virtual public ILeaderConnections, virtual public IAdaptiveFollowerConnections  {
public:
    virtual void initialize(IAdaptiveLeader* parent) = 0;

    virtual bool sendMReport(Message::node ip, vector<AdaptiveReport::adaptive_report_result> report) = 0;

    virtual bool sendChangeTimeReport(Message::node ip, int newTimeReport) = 0;
    virtual bool sendChangeServer() = 0;

    virtual bool sendRemoveLeader(Message::leader_update update) = 0;

    virtual bool sendDisableMetrics(Message::node ip, vector<Metric> metrics) = 0;
    virtual bool sendEnableMetrics(Message::node ip, vector<Metric> metrics) = 0;
};

#endif
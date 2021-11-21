#ifndef ADAPTIVE_FOLLOWER_HPP_
#define ADAPTIVE_FOLLOWER_HPP_

#include "follower.hpp"                // parent
#include "adaptive_factory.hpp"  
#include "adaptive_controller.hpp"
#include "common.hpp"
#include "adaptive_follower_connections.hpp"
#include "iadaptivefollower.hpp"
#include "metrics_generator.hpp"

#include <iostream>
#include <fstream>


class AdaptiveFollower : virtual public IAdaptiveFollower, virtual public Follower {

public:
    static AdaptiveFollower* myobj;

    // metrics
    static map<Metric, bool> metrics;

    // leader adequacy
    static bool leaderAdequacy;

    AdaptiveFollower();
    AdaptiveFollower(Message::node node, int nThreads);
    ~AdaptiveFollower();

    
    virtual void initialize(AdaptiveFactory* factory = NULL);

    virtual void start(vector<Message::node> mNodes);
    virtual void stop();

    IAdaptiveFollowerConnections* getConnections();
    IAdaptiveStorageMonitoring* getStorage();
    AdaptiveController* getAdaptiveController();

    vector<Metric> getMetrics();
    void setMetrics(vector<Metric> metrics);
    
    void addMetric(Metric metric);
    void removeMetric(Metric metric);

    virtual bool changeServer(vector<Message::node> mNodes);

    virtual void disableMetrics(vector<Metric> metrics);
    virtual void enableMetrics(vector<Metric> metrics);

    ofstream f;

protected:

    AdaptiveController* adaptive_controller;

    AdaptiveFactory tFactory;
    AdaptiveFactory * factory;

    AdaptiveFollowerConnections *connections;
    IAdaptiveStorageMonitoring *storage;

    MetricsGenerator *metricsGenerator;

    void getBattery();
    void getHardware() override;

    void timer() override;
    void TestTimer() override;

    void startBandwidthTest();
    void stopBandwidthTest();

    static bool test_ready;

    bool metrics_generator;
    bool cpu_logs;
};

#endif
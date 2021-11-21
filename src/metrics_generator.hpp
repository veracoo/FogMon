#ifndef METRICS_GENERATOR_HPP_
#define METRICS_GENERATOR_HPP_

#include "common.hpp"
#include "sleeper.hpp"

#include <vector>
#include <map>
#include <thread>

#include <fstream>

#include "iadaptivefollower.hpp"

using namespace std;

class MetricsGenerator {

public:
    enum Trend {trSTABLE, trUNSTABLE, trTOO_LOW, trOK, trOK_TOO_LOW_OK, trSPIKE, trRAND, trSTUN, last};

    MetricsGenerator();
    ~MetricsGenerator();

    void start();
    void stop();

    void initialize(IAdaptiveFollower *node);

    
    static map<Metric, Trend> trends;
    static map<Metric, float> currentVal;
    static map<Metric, map<Trend, vector<float>>> series;

    ofstream f;

private:
    IAdaptiveFollower* node;

    bool running;
    thread *metricsThreads;

    void metricsRoutine(Metric metric, Trend trend);

    Sleeper sleeper;

    bool generated_cpu_logs;
};

#endif

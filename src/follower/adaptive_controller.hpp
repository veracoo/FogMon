#ifndef ADAPTIVE_CONTROLLER_HPP_
#define ADAPTIVE_CONTROLLER_HPP_

#include "sleeper.hpp"
#include "common.hpp"
#include "rule.hpp"

#include <map>
#include <vector>
#include <math.h>

#include <thread>               // std::thread
#include <mutex>                // std::mutex, std::unique_lock
#include <condition_variable>   // std::condition_variable

class IAdaptiveFollower;

class AdaptiveController {
    
friend class AdaptiveFollower;

public:

    map<Metric, vector<State>> getStates();
    Rule* rule;

    AdaptiveController();
    ~AdaptiveController();

    void initialize(IAdaptiveFollower* node);

    virtual void start();
    virtual void stop();

    void toStringSeries();
    void toStringStates();

protected:
    IAdaptiveFollower* node;

    int history = 5;

    map<Metric, vector<float>> series;
    map<Metric, vector<State>> states;

    //threads
    //std::thread statesThread;

    //static bool ready;
    //std::mutex mtx;
    //std::condition_variable cv;

    //bool running;

    void statesTimer();

    void addState(Metric metric, State state);
    void saveStates();

    //delta max = 0.01
    // tol = 1
    void stable(float delta_max = 0.1, float tol = 0.8);
    void increasing(float tol = 0.8);
    void decreasing(float tol = 0.8);
    void alarms(float tol=0.8, float too_high=1, float too_low=0.3, float alarming_high=1, float alarming_low=0.4);

};
#endif
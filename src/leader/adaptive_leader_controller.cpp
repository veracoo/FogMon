#include "adaptive_leader_controller.hpp"
#include "iadaptiveleader.hpp"
#include <iostream>

using namespace std;

AdaptiveLeaderController::AdaptiveLeaderController() : AdaptiveController() { }

AdaptiveLeaderController::~AdaptiveLeaderController() {
    try{
        delete this->leader_rule;
        this->leader_rule = NULL;
    }catch(...) {}
}

void AdaptiveLeaderController::initialize(IAdaptiveLeader* node) {
    AdaptiveController::initialize(node);

    this->node = node;

    this->leader_rule = new Rule();
    vector<string> paths;
    //paths.push_back("clips/leader_rules.clp");
    this->rule->initialize(paths);
}

void AdaptiveLeaderController::start() {
    AdaptiveController::start();

    this->running = true;
    this->leaderRulesThread = thread(&AdaptiveLeaderController::leaderRulesTimer, this);
}

void AdaptiveLeaderController::stop() {
    AdaptiveController::stop();

    this->running = false;
    if(this->leaderRulesThread.joinable()){
        this->leaderRulesThread.join();
    }
}

void AdaptiveLeaderController::leaderRulesTimer() {
    while(this->running){
        auto t_start = std::chrono::high_resolution_clock::now();

        this->leader_rule->run();       // trigger to CLIPS rules engine

        auto t_end = std::chrono::high_resolution_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::duration<float>>(t_end-t_start).count();
        
        int sleeptime = 30-elapsed_time;

        if (sleeptime > 0)
            sleeper.sleepFor(chrono::seconds(sleeptime));
    }
}
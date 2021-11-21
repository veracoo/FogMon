#ifndef ADAPTIVE_LEADER_CONTROLLER_HPP_
#define ADAPTIVE_LEADER_CONTROLLER_HPP_

#include "adaptive_controller.hpp"
#include "sleeper.hpp"

class IAdaptiveLeader;

class AdaptiveLeaderController : public AdaptiveController {

public:
    Rule* leader_rule;

    AdaptiveLeaderController();
    ~AdaptiveLeaderController();

    void initialize(IAdaptiveLeader* node);
    void start() override;
    void stop() override;

protected:
    IAdaptiveLeader* node;

    bool running;

    //threads
    std::thread leaderRulesThread;

    Sleeper sleeper;

    void leaderRulesTimer();
};
#endif
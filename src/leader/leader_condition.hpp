#ifndef LEADER_CONDITION_HPP_
#define LEADER_CONDITION_HPP_

#include "common.hpp"
#include "clips.h"
#include <vector>
#include <tuple>
#include "string.h"

using namespace std;

class AdaptiveLeader;

class LeaderCondition {
public:
    LeaderCondition();
    ~LeaderCondition();

    static void IsBatteryTooLow(Environment *env, UDFContext *udfc, UDFValue *out);
    static void IsFollowerBatteryTooLow(Environment * env, UDFContext *udfc, UDFValue *out);
};

#endif
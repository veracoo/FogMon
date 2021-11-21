#ifndef LEADER_ACTION_HPP_
#define LEADER_ACTION_HPP_

#include "clips.h"

class LeaderAction {
public:

    LeaderAction();
    ~LeaderAction();

    static void ChangeTimeReportLeader(Environment *env, UDFContext *udfc, UDFValue *out);
    static void SendChangeServer(Environment *env, UDFContext *udfc, UDFValue *out);

    static void SendDisableMetrics(Environment *env, UDFContext *udfc, UDFValue *out);
    static void SendEnableMetrics(Environment *env, UDFContext *udfc, UDFValue *out);
};

#endif
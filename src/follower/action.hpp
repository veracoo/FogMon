#ifndef ACTION_HPP_
#define ACTION_HPP_

#include "clips.h"

class Action {
public:

    Action();
    ~Action();

    static void ChangeTimeReport(Environment *env, UDFContext *udfc, UDFValue *out);
    static void ChangeTimeTests(Environment *env, UDFContext *udfc, UDFValue *out);
    static void ChangeTimeLatency(Environment *env, UDFContext *udfc, UDFValue *out);
    
    static void DisableMetrics(Environment *env, UDFContext *udfc, UDFValue *out);
    static void EnableMetrics(Environment *env, UDFContext *udfc, UDFValue *out);

    static void SetLeaderAdequacy(Environment *env, UDFContext *udfc, UDFValue *out);
};

#endif
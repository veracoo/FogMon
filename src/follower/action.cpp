#include "action.hpp"
#include "adaptive_follower.hpp"
#include "node.hpp"
#include "common.hpp"
#include <iostream>

Action::Action() {}
Action::~Action() {}

void Action::ChangeTimeReport(Environment *env, UDFContext *udfc, UDFValue *out){
    UDFValue t;
    UDFFirstArgument(udfc, INTEGER_BIT, &t);
    int time = t.integerValue->contents;

    AdaptiveFollower* node = AdaptiveFollower::myobj;

    node->node->timeReport = time;

    cout << "Time report changed: " << node->node->timeReport << endl;
}

void Action::ChangeTimeTests(Environment *env, UDFContext *udfc, UDFValue *out){
    UDFValue t;
    UDFFirstArgument(udfc, INTEGER_BIT, &t);
    int time = t.integerValue->contents;

    AdaptiveFollower* node = AdaptiveFollower::myobj;

    node->node->timeTests = time;

    cout << "Time tests changed: " << node->node->timeTests << endl;
}

void Action::ChangeTimeLatency(Environment *env, UDFContext *udfc, UDFValue *out){
    UDFValue t;
    UDFFirstArgument(udfc, INTEGER_BIT, &t);
    int time = t.integerValue->contents;

    AdaptiveFollower* node = AdaptiveFollower::myobj;

    node->node->timeLatency = time;

    cout << "Time latency changed: " << node->node->timeLatency << endl;
}

void Action::EnableMetrics(Environment *env, UDFContext *udfc, UDFValue *out){
    UDFValue _metrics; 
    UDFFirstArgument(udfc, MULTIFIELD_BIT, &_metrics);

    string s;
    vector<Metric> metrics;
    for(int i=0; i<_metrics.multifieldValue->length; i++){
        s = (_metrics.multifieldValue->contents[i]).lexemeValue->contents;
        metrics.push_back(lMetrics.at(s));
    }

    AdaptiveFollower* node = AdaptiveFollower::myobj;
    node->enableMetrics(metrics);
}

void Action::DisableMetrics(Environment *env, UDFContext *udfc, UDFValue *out){
    UDFValue _metrics; 
    UDFFirstArgument(udfc, MULTIFIELD_BIT, &_metrics);

    string s;
    vector<Metric> metrics;
    for(int i=0; i<_metrics.multifieldValue->length; i++){
        s = (_metrics.multifieldValue->contents[i]).lexemeValue->contents;
        metrics.push_back(lMetrics.at(s));
    }

    AdaptiveFollower* node = AdaptiveFollower::myobj;
    node->disableMetrics(metrics);
}

void Action::SetLeaderAdequacy(Environment *env, UDFContext *udfc, UDFValue *out){
    UDFValue v;
    UDFFirstArgument(udfc, INTEGER_BIT, &v);
    int value = v.integerValue->contents;

    if(value == 0){
        AdaptiveFollower::leaderAdequacy = false;
    }else{
        AdaptiveFollower::leaderAdequacy = true;
    }
    cout << "Leader Adequacy: " << AdaptiveFollower::leaderAdequacy << endl;
}

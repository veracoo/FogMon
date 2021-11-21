#include "clips_function.hpp"
#include "adaptive_follower.hpp"
#include "adaptive_leader.hpp"
#include <cmath>
#include <iostream>

using namespace std;

ClipsFunction::ClipsFunction() {}
ClipsFunction::~ClipsFunction() {}

void ClipsFunction::GetTimeReportMean(Environment *env, UDFContext *udfc, UDFValue *out) {
    
    UDFValue _c; UDFValue _n; UDFValue _w; UDFValue _mintr; UDFValue _maxtr;  

    UDFNthArgument(udfc, 1, INTEGER_BIT, &_c);
    UDFNthArgument(udfc, 2, INTEGER_BIT, &_n);
    UDFNthArgument(udfc, 3, INTEGER_BIT, &_w);
    UDFNthArgument(udfc, 4, INTEGER_BIT, &_mintr);
    UDFNthArgument(udfc, 5, INTEGER_BIT, &_maxtr);

    float c = (float)_c.integerValue->contents;
    float n = (float)_n.integerValue->contents;
    float w = (float)_w.integerValue->contents;
    float mintr = (float)_mintr.integerValue->contents;
    float maxtr = (float)_maxtr.integerValue->contents;

    int tr = (int)ceil(((maxtr-mintr)*(c/(n*w))) + mintr);

    out->integerValue = CreateInteger(env,tr);
}

void ClipsFunction::GetTimeReport(Environment *env, UDFContext *udfc, UDFValue *out) {
    
    UDFValue _s; UDFValue _t; UDFValue _tr;

    UDFNthArgument(udfc, 1, STRING_BIT, &_s);
    UDFNthArgument(udfc, 2, INTEGER_BIT, &_t);
    UDFNthArgument(udfc, 3, INTEGER_BIT, &_tr);

    const char* state = _s.lexemeValue->contents;
    int x = _t.integerValue->contents;
    int tr = _tr.integerValue->contents;

    float a=0;
    float b=0;
    float min_x=0;
    float max_x=0;

    int res = 0;


    /*
    (a, b)          -> time report (interval of possible values)
    (min_x, max_x)  -> number of previous samples with the same state (interval of possible values)
    x               -> number of previous samples with the same state
    */

    if(state == State2String.at(UNSTABLE)){

        if(x <= 5 && tr > 4){
            b=-4;
            min_x=1;
            max_x=5;
            a= - ( floor(- ( (b - (-(tr-1))) * ( (3 - min_x) / (max_x - min_x) ) + (-(tr-1)) )) );
            res = floor(- ( (b - a) * ( (x - min_x) / (max_x - min_x) ) + a ));
        }else{ // if metric is unstable since more than 5 samples or time report is = 4
            res = 4;    // set to minumum
        }

    } else if(state == State2String.at(STABLE)){

        if(x <= 10 && tr < 60) {
            b=60;
            min_x=1;
            max_x=10;
            a= floor(( (b - (tr+1)) * ( (2 - min_x) / (max_x - min_x) ) + (tr+1) ));;
            res = floor(( (b - a) * ( (x - min_x) / (max_x - min_x) ) + a ));
        }else{ // if metric is stable since more than 10 samples or time report is = 60
            res = 60;   // set to maximum
        }
    }

    out->integerValue = CreateInteger(env,res);
}

void ClipsFunction::GetNumActiveMetrics(Environment *env, UDFContext *udfc, UDFValue *out){
    int count = 0;

    for(auto &m : AdaptiveFollower::metrics)
        if(m.second == true)
            count += 1;
    
    out->integerValue = CreateInteger(env,count);
}

void ClipsFunction::LoadFactsLeader(Environment *env, UDFContext *udfc, UDFValue *out){

    AdaptiveLeader* node = AdaptiveLeader::myobj;

    string s;

    // follower's active metrics
    vector<tuple<string, Metric>> metrics = node->getStorage()->getMMetrics();
    for(auto &m : metrics){
        s = "(node_metric_enabled (node " + get<0>(m) + ") (metric " + Metric2String.at(get<1>(m)) + "))";
        AssertString(env, s.c_str());
    }

    // follower's metrics states
    vector<tuple<string, Metric, State>> states = node->getStorage()->getMStates();
    for(auto &m : states){
        s = "(node_metric_state (node " + get<0>(m) +  ") (metric " + Metric2String.at(get<1>(m)) + ") (state " + State2String.at(get<2>(m)) + "))";
        AssertString(env, s.c_str());
    }
}

void ClipsFunction::LoadFactsFollower(Environment *env, UDFContext *udfc, UDFValue *out){

    AdaptiveFollower* node = AdaptiveFollower::myobj;

    string s;

    // parameters
    int time_report = node->node->timeReport;
    int time_tests = node->node->timeTests;
    int time_latency = node->node->timeLatency;
    s = "(my_parameters (time_report " + to_string(time_report) + ") (time_tests " + to_string(time_tests) + ") (time_latency " + to_string(time_latency) + "))";
    AssertString(env, s.c_str());

    // active metrics
    vector<Metric> metrics = node->getMetrics();
    for(auto &m : metrics){
        s = "(my_metric_enabled (metric " + Metric2String.at(m) + "))";
        AssertString(env, s.c_str());
    }

    // metrics states
    vector<tuple<string, int, int>> states = node->getStorage()->getCurrentStates();
    for(auto &m : states){
        s = "(my_metric_state (metric " + Metric2String.at(static_cast<Metric>(get<1>(m))) +  ") (state " + State2String.at(static_cast<State>(get<2>(m))) + "))";
        AssertString(env, s.c_str());
    }
}
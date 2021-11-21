#include "condition.hpp"
#include "adaptive_follower.hpp"
#include <iostream>

Condition::Condition() {}

Condition::~Condition() {}


void Condition::MetricInStateFor(Environment *env, UDFContext *udfc, UDFValue *out) {

    // get arguments
    UDFValue m; UDFValue s;
    UDFNthArgument(udfc, 1, STRING_BIT, &m);
    UDFNthArgument(udfc, 2, STRING_BIT, &s);

    const char* metric = m.lexemeValue->contents;
    const char* state = s.lexemeValue->contents;

    int res = MetricInStateFor(lMetrics.at(metric), lStates.at(state));
    out->integerValue = CreateInteger(env, res);
}

void Condition::IsMetricEnabled(Environment *env, UDFContext *udfc, UDFValue *out) {

    //get arguments
    UDFValue m;
    UDFFirstArgument(udfc, STRING_BIT, &m);

    const char* metric = m.lexemeValue->contents;

    Metric met = lMetrics.at(metric);
    bool enabled = AdaptiveFollower::metrics.at(met);

    out->lexemeValue = CreateBoolean(env, enabled);
}

void Condition::NumMetricInStateFor(Environment *env, UDFContext *udfc, UDFValue *out) {

    //get arguments
    UDFValue s; UDFValue t;
    UDFNthArgument(udfc, 1, STRING_BIT, &s);
    UDFNthArgument(udfc, 2, INTEGER_BIT, &t);

    const char* state = s.lexemeValue->contents;
    int time = t.integerValue->contents;

    int res = NumMetricInStateFor(lStates.at(state), time);
    out->integerValue = CreateInteger(env, res);
}

vector<tuple<string, int, int>> Condition::getData(){
    IAdaptiveStorageMonitoring* storage = new AdaptiveStorageMonitoring();
    storage->open("monitoring.db");
    vector<tuple<string, int, int>> data = storage->getStates();
    storage->close();

    return data;
}

int Condition::MetricInStateFor(Metric metric, State state){
    vector<tuple<string, int, int>> data = getData();

    int count = 0;
    bool found = false;
    string lasttime;

    try{
        lasttime = get<0>(data.at(0));
    }catch (const std::out_of_range& e) { }

    
    for(int i=0; i<data.size(); i++){

        string currtime = get<0>(data[i]);
        
        if(lasttime != currtime){
            lasttime = currtime;
            if(found==false){
                break;
            }else{
                found = false;
            }
        }else{
            if(found == true){
                continue;
            }
        }

        if(get<1>(data[i]) == metric &&
           get<2>(data[i]) == state){
                count += 1;
                found = true;
        }     
    }

    return count;
}

int Condition::NumMetricInStateFor(State state, int time) {
    int count = 0;
    for(auto &m : AdaptiveFollower::metrics){
        if(m.second){
            if(MetricInStateFor(m.first, state) >= time){
                count += 1;
            }
        }
    }
    return count;
}
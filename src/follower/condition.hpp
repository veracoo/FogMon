#ifndef CONDITION_HPP_
#define CONDITION_HPP_

#include "common.hpp"
#include "clips.h"
#include <map>
#include <vector>
#include <tuple>
#include <string>

using namespace std;

class AdaptiveFollower;

class Condition {
public:
    Condition();
    ~Condition();

    //* da quanti istanti consecutivi la metrica M si trova nello stato S *//
    static void MetricInStateFor(Environment *env, UDFContext *udfc, UDFValue *out);
    static void IsMetricEnabled(Environment *env, UDFContext *udfc, UDFValue *out);

    static void NumMetricInStateFor(Environment *env, UDFContext *udfc, UDFValue *out);

    static int MetricInStateFor(Metric metric, State state);
    static int NumMetricInStateFor(State state, int time);
    static vector<tuple<string, int, int>> getData();
};

#endif
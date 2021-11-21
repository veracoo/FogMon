#include "iadaptiveleader_storage_monitoring.hpp"

using namespace std;

int IAdaptiveLeaderStorageMonitoring::getMStatesCallback(void *vec, int argc, char **argv, char **azColName) {
    vector<tuple<string, Metric, State>> *v = (vector<tuple<string, Metric, State>>*)vec;
    tuple<string, Metric, State> state;

    state = make_tuple(string(argv[0]), static_cast<Metric>(stoi(argv[1])), static_cast<State>(stoi(argv[2])));
  
    v->push_back(state);

   return 0;
}

int IAdaptiveLeaderStorageMonitoring::getMMetricsCallback(void *vec, int argc, char **argv, char **azColName) {
    vector<tuple<string, Metric>> *v = (vector<tuple<string, Metric>>*)vec;
    tuple<string, Metric> metric;

    metric = make_tuple(string(argv[0]), static_cast<Metric>(stoi(argv[1])));

    v->push_back(metric);

    return 0;
}


int IAdaptiveLeaderStorageMonitoring::getNodeCallback(void *N, int argc, char **argv, char **azColName) {
    Message::node *n = (Message::node*)N;
    for(int i=0; i<argc; i++) {
        if(strcmp("id", azColName[i])==0) {
            n->id = argv[i];
        }else if(strcmp("ip", azColName[i])==0) {
            n->ip = argv[i];
        }else if(strcmp("port", azColName[i])==0) {
            n->port = argv[i];
        }
    }
    return 0;
}


#include "iadaptive_storage_monitoring.hpp"
#include "string.h"
#include <vector>
#include <tuple>

using namespace std;

int IAdaptiveStorageMonitoring::getBatteryCallback(void *R, int argc, char **argv, char **azColName) {
    AdaptiveReport::battery_result *r = (AdaptiveReport::battery_result*)R;
    for(int i=0; i<argc; i++) {
        if(strcmp("mean_battery", azColName[i])==0) {
            r->mean_battery = stof(argv[i]);
        }else if(strcmp("var_battery", azColName[i])==0) {
            r->var_battery = stof(argv[i]);
        }else if(strcmp("lasttime", azColName[i])==0) {
            r->lasttime = stoll(argv[i]);
        }
    }
    return 0;
}

int IAdaptiveStorageMonitoring::getStatesCallback(void *vec, int argc, char **argv, char **azColName){
    vector<tuple<string, int, int>> *v = (vector<tuple<string, int, int>>*)vec;
    tuple<string, int, int> state;

    state = make_tuple(string(argv[0]), stoi(argv[1]), stoi(argv[2]));
  
    v->push_back(state);

   return 0;
}
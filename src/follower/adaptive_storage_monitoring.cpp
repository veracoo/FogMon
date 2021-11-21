#include "adaptive_storage_monitoring.hpp"
#include "string.h"
#include <vector>
#include <tuple>
#include <sstream>

using namespace std;

AdaptiveStorageMonitoring::AdaptiveStorageMonitoring() : Storage() { }
AdaptiveStorageMonitoring::~AdaptiveStorageMonitoring() { }

void AdaptiveStorageMonitoring::createTables(){
    Storage::createTables();

    char *zErrMsg = 0;
    
    vector<string> query = {"CREATE TABLE IF NOT EXISTS Other_metrics (time TIMESTAMP PRIMARY KEY, battery REAL)",
                            "CREATE TABLE IF NOT EXISTS States (timestamp TIMESTAMP, metric INTEGER, state INTEGER)"};
    
    for(string str : query) {
        int err = sqlite3_exec(this->db, str.c_str(), 0, 0, &zErrMsg);
        isError(err, zErrMsg, "createTablesAdaptive");
    }
}

AdaptiveReport::battery_result AdaptiveStorageMonitoring::getBattery() {
    char *zErrMsg = 0;
    char buf[1024];
    std::sprintf(buf, "SELECT avg(battery) AS mean_battery, variance(battery) AS var_battery, strftime('%%s','now') as lasttime FROM Other_metrics");

    AdaptiveReport::battery_result r;
    memset(&r,0,sizeof(AdaptiveReport::battery_result));

    int err = sqlite3_exec(this->db, buf, IAdaptiveStorageMonitoring::getBatteryCallback, &r, &zErrMsg);
    isError(err, zErrMsg, "getBattery");

    return r;
}

vector<float> AdaptiveStorageMonitoring::getLastValues(Metric metric, int limit){

    char *zErrMsg = 0;
    char buf[1024];
    stringstream query;

    if(metric == Metric::FREE_CPU){
        query << "SELECT free_cpu FROM Hardware ORDER BY time DESC LIMIT " << limit;

    }else if(metric == Metric::FREE_MEMORY){
        query << "SELECT CASE WHEN memory = 0 THEN 0 ELSE free_memory/memory END FROM Hardware ORDER BY time DESC LIMIT " << limit;

    }else if(metric == Metric::FREE_DISK){
        query << "SELECT CASE WHEN disk = 0 THEN 0 ELSE free_disk/disk END FROM Hardware ORDER BY time DESC LIMIT " << limit; 

    }else if(metric == Metric::BATTERY){
        query << "SELECT battery FROM Other_metrics ORDER BY time DESC LIMIT " << limit;
    }

    std::sprintf(buf, query.str().c_str());

    vector<float> r;
    int err = sqlite3_exec(this->db, buf, IStorage::VectorFloatCallback, &r, &zErrMsg);
    isError(err, zErrMsg, "getLastValues");

    return r;
}

vector<tuple<string, int, int>> AdaptiveStorageMonitoring::getStates(){
    char *zErrMsg = 0;
    char buf[1024];

    stringstream query;
    query << "SELECT * FROM States ORDER BY timestamp DESC";
    std::sprintf(buf, query.str().c_str());

    vector<tuple<string, int, int>> vect;

    int err = sqlite3_exec(this->db, buf, IAdaptiveStorageMonitoring::getStatesCallback, &vect, &zErrMsg);

    isError(err, zErrMsg, "getStates");

    return vect;
}


void AdaptiveStorageMonitoring::saveStates(vector<State> states, Metric metric){
    char *zErrMsg = 0;
    char buf[1024];

    int err;
    stringstream query;

    for(int i=0; i<states.size(); i++){
        query.str("");
        query << "INSERT INTO States (timestamp, metric, state) VALUES (DATETIME('now'), " << metric << ", " << states[i] << ")";
        std::sprintf(buf, query.str().c_str());
        
        err = sqlite3_exec(this->db, buf, 0, 0, &zErrMsg);
        isError(err, zErrMsg, "saveStates1");
    }

    query.str("");
    query << "SELECT COUNT (DISTINCT timestamp) FROM States";
    std::sprintf(buf, query.str().c_str());

    int ntimes;
    err = sqlite3_exec(this->db, buf, IStorage::getIntCallback, &ntimes, &zErrMsg);
    isError(err, zErrMsg, "saveStates2");

    int window = 10;
    if(ntimes > window){
        query.str("");
        query << "DELETE FROM States WHERE timestamp = (SELECT MIN(timestamp) FROM States)";
        std::sprintf(buf, query.str().c_str());

        err = sqlite3_exec(this->db, buf, 0, 0, &zErrMsg);
        isError(err, zErrMsg, "saveStates3");
    }
}

void AdaptiveStorageMonitoring::saveBattery(AdaptiveReport::battery_result battery, int window) {
    char *zErrMsg = 0;
    char buf[1024];
    std::sprintf(buf,"INSERT INTO Other_metrics (time, battery) VALUES (DATETIME('now'), %f)", battery.mean_battery);

    int err = sqlite3_exec(this->db, buf, 0, 0, &zErrMsg);
    isError(err, zErrMsg, "saveBattery1");

    std::sprintf(buf,"DELETE FROM Other_metrics WHERE time <= (SELECT time FROM Other_metrics ORDER BY time DESC LIMIT 1 OFFSET %d)", window);

    err = sqlite3_exec(this->db, buf, 0, 0, &zErrMsg);
    isError(err, zErrMsg, "saveBattery2");
}

std::vector<std::tuple<string, int, int>> AdaptiveStorageMonitoring::getCurrentStates() {
    vector<tuple<string, int, int>> states = this->getStates();

    vector<tuple<string, int, int>> res;

    if(states.empty()){
        return res;
    }

    string currtime = get<0>(states.at(0));
    for(auto &s: states){
        if(get<0>(s) == currtime){
            res.push_back(s);
        }else{
            break;
        }
    }
    
    return res;
}
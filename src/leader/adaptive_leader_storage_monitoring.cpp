#include "adaptive_leader_storage_monitoring.hpp"
#include <string.h>
#include <vector>
#include <sstream>
#include <iostream>

using namespace std;

AdaptiveLeaderStorageMonitoring::AdaptiveLeaderStorageMonitoring(Message::node node) : LeaderStorage(node), AdaptiveStorageMonitoring(), Storage() { }
AdaptiveLeaderStorageMonitoring::~AdaptiveLeaderStorageMonitoring() { }


void AdaptiveLeaderStorageMonitoring::createTables(){
    AdaptiveStorageMonitoring::createTables();

    char *zErrMsg = 0;
    
    vector<string> query = {"CREATE TABLE IF NOT EXISTS MMNodes (id STRING PRIMARY KEY, ip STRING NOT NULL, port STRING NOT NULL, UNIQUE(ip, port))",
                            "CREATE TABLE IF NOT EXISTS MNodes (id STRING PRIMARY KEY, ip STRING NOT NULL, port STRING NOT NULL, cores INTEGER, mean_free_cpu REAL, var_free_cpu REAL, memory INTEGER, mean_free_memory FLOAT, var_free_memory FLOAT, disk INTEGER, mean_free_disk FLOAT, var_free_disk FLOAT, mean_battery FLOAT, var_battery FLOAT, lasttime TIMESTAMP, monitoredBy STRING REFERENCES MMNodes(id) NOT NULL, UNIQUE(ip, port))",
                            "CREATE TABLE IF NOT EXISTS MLinks (idA STRING REFERENCES MNodes(id) NOT NULL, idB STRING REFERENCES MNodes(id) NOT NULL, meanL FLOAT, varianceL FLOAT, lasttimeL TIMESTAMP, meanB FLOAT, varianceB FLOAT, lasttimeB TIMESTAMP, PRIMARY KEY(idA,idB))",
                            "CREATE TABLE IF NOT EXISTS MIots (id STRING PRIMARY KEY, desc STRING, ms INTEGER, idNode STRING REFERENCES MNodes(id) NOT NULL)",
                            "CREATE TABLE IF NOT EXISTS MStates (id STRING, metric INTEGER, state INTEGER)",
                            "CREATE TABLE IF NOT EXISTS MMetrics (id STRING, metric INTEGER)",
                            "DELETE FROM MMNodes",
                            string("INSERT OR IGNORE INTO MMNodes (id, ip, port) VALUES (\"")+ this->nodeM.id+ string("\", \"::1\", \""+ this->nodeM.port +"\")")};
    
    for(string str : query) {
        int err = sqlite3_exec(this->db, str.c_str(), 0, 0, &zErrMsg);
        isError(err, zErrMsg, "createTablesAdaptiveLeader");
    }
}

vector<AdaptiveReport::adaptive_report_result> AdaptiveLeaderStorageMonitoring::getAdaptiveReport(bool complete) {
    //sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
    vector<Message::node> nodes = this->getAllNodes();
    vector<AdaptiveReport::adaptive_report_result> res;

    for(auto node : nodes) {
        res.push_back(this->getAdaptiveReport(node, complete));
    }
    //sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
    return res;
}

AdaptiveReport::adaptive_report_result AdaptiveLeaderStorageMonitoring::getAdaptiveReport(Message::node node, bool complete) {
    Report::report_result r = LeaderStorage::getReport(node, complete);

    AdaptiveReport::adaptive_report_result ar;
    ar.source = r.source;
    ar.hardware = r.hardware;
    ar.latency = r.latency;
    ar.bandwidth = r.bandwidth;
    ar.leader = r.leader;
    ar.battery = this->getBattery(ar.source);

    return ar;
}


std::string AdaptiveLeaderStorageMonitoring::addNode(Message::node node, AdaptiveReport::hardware_result hardware, AdaptiveReport::battery_result battery, Message::node *monitored) {

    int64_t lasttime;
    if(hardware.lasttime != 0){
        lasttime = hardware.lasttime;
    }else if(battery.lasttime != 0){
        lasttime = battery.lasttime;
    }else{
        return "";
    }

    char *zErrMsg = 0;
    char buf[1024];
    vector<long long> res;
    std::sprintf(buf,"SELECT strftime('%%s',lasttime) FROM MNodes WHERE (strftime('%%s',lasttime)-%" PRId64" > 0) AND (id = \"%s\") ", lasttime, node.id.c_str());
    int err = sqlite3_exec(this->db, buf, IStorage::VectorIntCallback, &res, &zErrMsg);
    isError(err, zErrMsg, "addNodeAdaptiveLeader0");

    if(!res.size()) {
        stringstream query;
        query <<    "INSERT OR REPLACE INTO MNodes"
                    " (id, ip, port, cores, mean_free_cpu, var_free_cpu, memory, mean_free_memory, var_free_memory, disk, mean_free_disk, var_free_disk, mean_battery, var_battery, lasttime, monitoredBy)"
                    " VALUES (\""<< node.id <<"\", \""<< node.ip <<"\", \""<< node.port <<"\", "<<
                        hardware.cores <<", "<< hardware.mean_free_cpu <<", "<< hardware.var_free_cpu <<", "<<
                        hardware.memory <<", "<< hardware.mean_free_memory <<", "<< hardware.var_free_memory <<", "<<
                        hardware.disk <<", "<< hardware.mean_free_disk <<", "<< hardware.var_free_disk <<", "<< battery.mean_battery <<", "<< battery.var_battery <<", DATETIME("<< lasttime <<",\"unixepoch\"),";
        if(!monitored) {
            query << " \"" << this->nodeM.id <<"\")";
        }else {
            // block report about this leader from other nodes
            if (monitored->id == this->nodeM.id){
                return "";
            }
            query << " \""<< monitored->id <<"\")";
            //printf("report: %s --> %s lasttime: %ld\n",monitored->id.c_str(), node.ip.c_str(),hardware.lasttime);
        }
        int err = sqlite3_exec(this->db, query.str().c_str(), 0, 0, &zErrMsg);
        isError(err, zErrMsg, "addNodeAdaptiveLeader1");
        return node.id;
    }
    return "";
}

void AdaptiveLeaderStorageMonitoring::addReport(AdaptiveReport::adaptive_report_result result, Message::node *monitored) {
    if( this->addNode(result.source, result.hardware, result.battery, monitored) != "") {
        this->addReportLatency(result.source, result.latency);        
        this->addReportBandwidth(result.source, result.bandwidth);
        this->addReportIot(result.source, result.iot);
    }
}

void AdaptiveLeaderStorageMonitoring::addReport(std::vector<AdaptiveReport::adaptive_report_result> results, Message::node node) {                    
    for(auto &result : results) {
        if(result.source.ip == string("::1"))
            result.source.ip = node.ip;
        for(auto &test : result.latency) {
            if(test.target.ip == string("::1"))
                test.target.ip = node.ip;
        }
        for(auto &test : result.bandwidth) {
            if(test.target.ip == string("::1"))
                test.target.ip = node.ip;
        }
        Message::node leader(result.leader,"","");
        this->addNode(result.source, result.hardware, result.battery, &leader);
    }

    for(auto result : results) {
        Message::node leader(result.leader,"","");
        this->addReport(result, &leader);
    }
}


AdaptiveReport::battery_result AdaptiveLeaderStorageMonitoring::getBattery(Message::node node) {
    char *zErrMsg = 0;
    char buf[1024];
    std::sprintf(buf,"SELECT *, (CASE WHEN \"%s\" = N.monitoredBy THEN strftime('%%s','now') ELSE strftime('%%s',lasttime) END) as lasttime FROM MNodes as N WHERE id = \"%s\" GROUP BY ip", this->nodeM.id.c_str(),node.id.c_str());

    AdaptiveReport::battery_result r(-1,0,0);

    int err = sqlite3_exec(this->db, buf, IAdaptiveStorageMonitoring::getBatteryCallback, &r, &zErrMsg);
    isError(err, zErrMsg, "getBatteryLeader");

    return r;
}

void AdaptiveLeaderStorageMonitoring::addReportStates(Message::node node, map<Metric, vector<State>> states) {
    if(states.empty()){
        return;
    }

    char *zErrMsg = 0;
    stringstream query;

    query << "DELETE FROM MStates WHERE id = \"" << node.id << "\"";

    int err = sqlite3_exec(this->db, query.str().c_str(), 0, 0, &zErrMsg);
    isError(err, zErrMsg, "addReportStates1");

    query.str("");
    for(auto &m : states){
        for(auto &s : m.second){
            query << "INSERT INTO MStates (id, metric, state) VALUES (\"" << node.id << "\", " << m.first << ", " << s << ")";
            
            err = sqlite3_exec(this->db, query.str().c_str(), 0, 0, &zErrMsg);
            isError(err, zErrMsg, "addReportStates2");

            query.str("");
        }
    }
}

void AdaptiveLeaderStorageMonitoring::addReportMetrics(Message::node node, std::vector<Metric> metrics) {
    if(metrics.empty()){
        return;
    }

    char *zErrMsg = 0;
    stringstream query;

    query << "DELETE FROM MMetrics WHERE id = \"" << node.id << "\"";

    int err = sqlite3_exec(this->db, query.str().c_str(), 0, 0, &zErrMsg);
    isError(err, zErrMsg, "addReportMetrics1");

    query.str("");
    for(auto &m : metrics){
        query << "INSERT INTO MMetrics (id, metric) VALUES (\"" << node.id << "\", " << m << ")";

        err = sqlite3_exec(this->db, query.str().c_str(), 0, 0, &zErrMsg);
        isError(err, zErrMsg, "addReportMetrics2");

        query.str("");
    }
}

/*
std::vector<Message::node> AdaptiveLeaderStorageMonitoring::getFollowerIdBatteryTooLow() {
    cout << "getFollowerIdBatteryTooLow()" << endl;
    char *zErrMsg = 0;
    stringstream query;

    query << "SELECT S.id, N.ip, N.port FROM MStates AS S INNER JOIN MNodes AS N ON S.id = N.id WHERE S.metric = " << static_cast<int>(BATTERY) << " AND S.state = " << static_cast<int>(TOO_LOW); 

    vector<Message::node> nodes;

    int err = sqlite3_exec(this->db, query.str().c_str(), VectorNodeCallback, &nodes, &zErrMsg);
    isError(err, zErrMsg, "getFollowerIdBatteryTooLow");

    return nodes;
}
*/

vector<tuple<string, Metric, State>> AdaptiveLeaderStorageMonitoring::getMStates() {
    char *zErrMsg = 0;
    stringstream query;

    query << "SELECT * FROM MStates";

    vector<tuple<string, Metric, State>> data;

    int err = sqlite3_exec(this->db, query.str().c_str(), getMStatesCallback, &data, &zErrMsg);
    isError(err, zErrMsg, "getMStates()");

    return data;
}

vector<tuple<string, Metric>> AdaptiveLeaderStorageMonitoring::getMMetrics() {
    char *zErrMsg = 0;
    stringstream query;

    query << "SELECT * FROM MMetrics";

    vector<tuple<string, Metric>> data;

    int err = sqlite3_exec(this->db, query.str().c_str(), getMMetricsCallback, &data, &zErrMsg);
    isError(err, zErrMsg, "getMMetrics()");

    return data;
}

Message::node AdaptiveLeaderStorageMonitoring::getMNode(std::string id) {
    char *zErrMsg = 0;
    stringstream query;

    query << "SELECT id, ip, port FROM MNodes WHERE id = \"" << id << "\"";

    Message::node node;

    int err = sqlite3_exec(this->db, query.str().c_str(), getNodeCallback, &node, &zErrMsg);
    isError(err, zErrMsg, "getMNode()");

    return node;
}

void AdaptiveLeaderStorageMonitoring::removeOldNodesMStates(vector<Message::node> nodes) {
    char *zErrMsg = 0;
    stringstream query;

    for(auto &n : nodes){
        query << "DELETE FROM MStates WHERE id = \"" << n.id << "\"";

        int err = sqlite3_exec(this->db, query.str().c_str(), 0, 0, &zErrMsg);
        isError(err, zErrMsg, "removeOldNodesMStates()");

        query.str("");
    }
}

void AdaptiveLeaderStorageMonitoring::removeOldNodesMMetrics(vector<Message::node> nodes) {
    char *zErrMsg = 0;
    stringstream query;

    for(auto &n : nodes){
        query << "DELETE FROM MMetrics WHERE id = \"" << n.id << "\"";

        int err = sqlite3_exec(this->db, query.str().c_str(), 0, 0, &zErrMsg);
        isError(err, zErrMsg, "removeOldNodesMMetrics()");

        query.str("");
    }
}
#include "leader_action.hpp"
#include "adaptive_leader.hpp"
#include <unistd.h>
#include <iostream>
#include "string.h"
#include <vector>

using namespace std;

void LeaderAction::ChangeTimeReportLeader(Environment *env, UDFContext *udfc, UDFValue *out) {
    
    UDFValue _time; UDFValue _ip; 
    UDFNthArgument(udfc, 1, INTEGER_BIT, &_time);
    UDFNthArgument(udfc, 2, STRING_BIT, &_ip);

    int time = _time.integerValue->contents;
    string ip = _ip.lexemeValue->contents;

    AdaptiveLeader* node = AdaptiveLeader::myobj;

    Message::node follower;
    for (auto &f : node->getStorage()->getMLRHardware(100, 0)){
        if(f.ip == ip)
            follower = f;
    }

    if (follower.id == "" || follower.ip == "" || follower.port == "")
        return;
        
    node->getConnections()->sendChangeTimeReport(follower, time);
}

void LeaderAction::SendChangeServer(Environment *env, UDFContext *udfc, UDFValue *out) {

    AdaptiveLeader* node = AdaptiveLeader::myobj;

    //node->getConnections()->sendChangeServer();

    vector<Message::node> mnodes = node->getStorage()->getMNodes();

    vector<Message::node> nodes;
    for(int i=0; i<mnodes.size(); i++){
        if(mnodes[i].id != node->getMyNode().id)
            nodes.push_back(mnodes[i]);
    }

    Message::leader_update update;
    update.selected = nodes;

    cout << "LeaderAction::sendChangeServer() " << endl;
    for(auto &n : update.selected){
        cout << n.ip << endl;
    }

    node->getConnections()->sendChangeRoles(update);
    node->getConnections()->sendRemoveLeader(update);
}

void LeaderAction::SendDisableMetrics(Environment *env, UDFContext *udfc, UDFValue *out){
    //get arguments
    UDFValue _id; UDFValue _metrics; 
    UDFNthArgument(udfc, 1, SYMBOL_BIT, &_id);
    UDFNthArgument(udfc, 2, MULTIFIELD_BIT, &_metrics);

    string id = _id.lexemeValue->contents;

    string s;
    vector<Metric> metrics;

    for(int i=0; i<_metrics.multifieldValue->length; i++){
        s = (_metrics.multifieldValue->contents[i]).lexemeValue->contents;
        metrics.push_back(lMetrics.at(s));
    }

    AdaptiveLeader* node = AdaptiveLeader::myobj;
    Message::node follower = node->getStorage()->getMNode(id);

    node->getConnections()->sendDisableMetrics(follower, metrics);
}

void LeaderAction::SendEnableMetrics(Environment *env, UDFContext *udfc, UDFValue *out){
    //get arguments
    UDFValue _id; UDFValue _metrics; 
    UDFNthArgument(udfc, 1, SYMBOL_BIT, &_id);
    UDFNthArgument(udfc, 2, MULTIFIELD_BIT, &_metrics);

    string id = _id.lexemeValue->contents;

    string s;
    vector<Metric> metrics;

    for(int i=0; i<_metrics.multifieldValue->length; i++){
        s = (_metrics.multifieldValue->contents[i]).lexemeValue->contents;
        metrics.push_back(lMetrics.at(s));
    }

    AdaptiveLeader* node = AdaptiveLeader::myobj;
    Message::node follower = node->getStorage()->getMNode(id);

    node->getConnections()->sendEnableMetrics(follower, metrics);
}
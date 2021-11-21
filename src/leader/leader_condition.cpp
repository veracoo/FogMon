#include "leader_condition.hpp"
#include "message.hpp"
#include "adaptive_leader.hpp"
#include <iostream>

using namespace std;

void LeaderCondition::IsBatteryTooLow(Environment *env, UDFContext *udfc, UDFValue *out) {
    UDFValue _ip;
    UDFFirstArgument(udfc, STRING_BIT, &_ip);

    const char* ip = _ip.lexemeValue->contents;

    bool ret = true; // !! ritorna sempre true !! //

    out->lexemeValue = CreateBoolean(env, ret);
}

void LeaderCondition::IsFollowerBatteryTooLow(Environment * env, UDFContext *udfc, UDFValue *out) {
    // leader object
    /*
    AdaptiveLeader* leader = AdaptiveLeader::myobj;

    vector<Message::node> nodes = leader->getStorage()->getFollowerIdBatteryTooLow();
    string s = "";
    for(int i=0; i<nodes.size(); i++){
        s += nodes[i];
        if(i < nodes.size()-1){
            s += " ";
        }
    }

    out->multifieldValue = StringToMultifield(env, s);
    */
}
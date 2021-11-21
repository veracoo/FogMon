#include "adaptive_uiconnection.hpp"

#include <iostream>
#include <string>
#include <curl/curl.h>
#include <unistd.h>
#include <cstdio>
#include <time.h>

using namespace std;

AdaptiveUIConnection::AdaptiveUIConnection(Message::node myNode, std::string ip, int session) : UIConnection(myNode, ip, session) { }

AdaptiveUIConnection::~AdaptiveUIConnection() { }

bool AdaptiveUIConnection::sendTopology(std::vector<AdaptiveReport::adaptive_report_result> report) {
    bool res = false;
    if(this->ip != "") {
        Message m;
        m.setSender(myNode);
        m.setType((Message::Type)0);
        m.setCommand((Message::Command)0);
        m.setArgument((Message::Argument)this->session);

        AdaptiveReport r;
        r.setReports(report);

        m.setData(r);
        m.buildString();
        res = sendToInterface(this->ip,m.getString());
        if (!res) {
            sleep(1);
            res = sendToInterface(this->ip,m.getString());
        }
    }
    return res;
}
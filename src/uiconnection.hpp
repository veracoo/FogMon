#ifndef UICONNECTION_HPP_
#define UICONNECTION_HPP_

#include <string>
#include <vector>
#include "message.hpp"
#include "report.hpp"

class UIConnection {

public:
    UIConnection(Message::node myNode, std::string ip = "", int session = 0);
    ~UIConnection();

    bool sendTopology(std::vector<Report::report_result> report);
    bool sendChangeRole(Message::leader_update update);
protected:
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
    bool sendToInterface(std::string ip,std::string str);

    std::string ip;
    Message::node myNode;
    int session;
};

#endif
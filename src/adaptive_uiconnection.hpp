#ifndef ADAPTIVE_UICONNECTION_HPP_
#define ADAPTIVE_UICONNECTION_HPP_

#include "uiconnection.hpp"
#include <string>
#include <vector>
#include "message.hpp"
#include "adaptive_report.hpp"

class AdaptiveUIConnection : public UIConnection {

public:
    AdaptiveUIConnection(Message::node myNode, std::string ip = "", int session = 0);
    ~AdaptiveUIConnection();

    bool sendTopology(std::vector<AdaptiveReport::adaptive_report_result> report);
};

#endif
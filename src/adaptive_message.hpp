#ifndef ADAPTIVE_MESSAGE_HPP_
#define ADAPTIVE_MESSAGE_HPP_

#include "message.hpp"

class AdaptiveReport;

class AdaptiveMessage : public Message {
public:
    AdaptiveMessage();
    ~AdaptiveMessage();

    /**
     * set the data as a report
     * @param report
    */
    void setData(AdaptiveReport& report);
};

#endif
#include "adaptive_message.hpp"
#include "adaptive_report.hpp"

AdaptiveMessage::AdaptiveMessage() : Message() { }
AdaptiveMessage::~AdaptiveMessage() { }

void AdaptiveMessage::setData(AdaptiveReport& report) {
    this->data = *(report.getJson());
}
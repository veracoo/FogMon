#ifndef ADAPTIVE_REPORT_HPP_
#define ADAPTIVE_REPORT_HPP_

#include "report.hpp"
#include "common.hpp"
#include <iostream>
#include <map>

class AdaptiveReport : public Report {
public:
    AdaptiveReport();
    ~AdaptiveReport();

    /**
     * hold a battery test
    */
    typedef struct battery_result{
        /**
         * battery
        */
        float mean_battery = 0;
        float var_battery = 0;

        int64_t lasttime = 0;

        battery_result() {}

        battery_result(float battery, int64_t lasttime) {
            this->mean_battery = battery;
            this->lasttime = lasttime;
        }
        battery_result(float mean_battery, float var_battery, int64_t lasttime) {
            this->mean_battery = mean_battery;
            this->var_battery = var_battery;
            this->lasttime = lasttime;
        }
        
    }battery_result;

    /**
     * the structure of a complete report
    */
    typedef struct adaptive_report_result : report_result {
       
        battery_result battery;

        std::map<Metric, std::vector<State>> states;
        std::vector<Metric> metrics;
        
        adaptive_report_result() : report_result() {}
        adaptive_report_result(Message::node Source, hardware_result Hardware, battery_result Battery, std::vector<test_result> Latency, std::vector<test_result> Bandwidth, std::vector<IoT> Iot, std::string _leader, std::map<Metric, std::vector<State>> States, std::vector<Metric> Metrics)
        : report_result(Source, Hardware, Latency, Bandwidth, Iot, _leader), battery(Battery), states(States), metrics(Metrics) {}

    }adaptive_report_result;

    /**
     * set the battery test
     * @param battery the battery test
    */
    void setBattery(battery_result battery);

    /**
     * get the battery test
     * @param battery the variable that in case of success is changed
     * @return true in case of success and set the variable
    */
    bool getBattery(battery_result& battery);

    void setStates(std::map<Metric, std::vector<State>> states);
    bool getStates(std::map<Metric, std::vector<State>>& states);

    void setMetrics(std::vector<Metric> metrics);
    bool getMetrics(std::vector<Metric>& metrics);

    /**
     * set the report given a report structure
     * @param report
    */
    void setReport(adaptive_report_result report);

    /**
     * set this class to comunicate multiple report not just one
     * @param reports the vector of reports
    */
    void setReports(std::vector<adaptive_report_result> reports);
    
    /**
     * get the adaptive report
     * @param reports the variable that in case of success is changed
     * @return true in case of success and set the variable
    */
    bool getReport(adaptive_report_result &report);
    
    /**
     * get the adaptive reports
     * @param reports the variable that in case of success is changed
     * @return true in case of success and set the variable
    */
    bool getReports(std::vector<adaptive_report_result> &reports);

protected:
    void getStatesHelper();
};

#endif
#ifndef IADAPTIVE_STORAGE_MONITORING_HPP_
#define IADAPTIVE_STORAGE_MONITORING_HPP_

#include "storage.hpp"
#include "adaptive_report.hpp"

class IAdaptiveStorageMonitoring : virtual public IStorage {
public:
    /**
     * get the state of the battery saved
     * @return 
    */
    virtual AdaptiveReport::battery_result getBattery() = 0;

    /**
     * get recent values of a metric
     * @param metric kind of metric
     * @param limit history
    */
    virtual std::vector<float> getLastValues(Metric metric, int limit) = 0;

     /**
     * get the state values of a metric
     * @return
    */
    virtual std::vector<std::tuple<std::string, int, int>> getStates() = 0;

     /**
     * save states of a metric
     * @param states states of metric
     * @param metric metric
    */
    virtual void saveStates(std::vector<State> states, Metric metric) = 0;

    /**
     * save a battery test
     * @param battery the result of the test
    */
    virtual void saveBattery(AdaptiveReport::battery_result battery, int window) = 0;

    virtual std::vector<std::tuple<std::string, int, int>> getCurrentStates() = 0;

protected:
    static int getBatteryCallback(void *R, int argc, char **argv, char **azColName);
    static int getStatesCallback(void *vec, int argc, char **argv, char **azColName);
};

#endif
#ifndef ADAPTIVE_STORAGE_MONITORING_HPP_
#define ADAPTIVE_STORAGE_MONITORING_HPP_

#include "storage.hpp"
#include "iadaptive_storage_monitoring.hpp"
#include "adaptive_report.hpp"

class AdaptiveStorageMonitoring : virtual public Storage, virtual public IAdaptiveStorageMonitoring {
protected:
    virtual void createTables() override;
public:
    AdaptiveStorageMonitoring();
    ~AdaptiveStorageMonitoring();

    AdaptiveReport::battery_result getBattery();

    std::vector<float> getLastValues(Metric metric, int limit);

    std::vector<std::tuple<std::string, int, int>> getStates();
    void saveStates(std::vector<State> states, Metric metric);

    void saveBattery(AdaptiveReport::battery_result battery, int window);

    std::vector<std::tuple<std::string, int, int>> getCurrentStates();
};

#endif
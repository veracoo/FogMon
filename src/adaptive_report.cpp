#include "adaptive_report.hpp"
#include <iostream>

using namespace rapidjson;
using namespace std;

AdaptiveReport::AdaptiveReport() : Report() {
}

AdaptiveReport::~AdaptiveReport() { }

void AdaptiveReport::setBattery(battery_result battery) {
    Value obj(kObjectType);
    doc.RemoveMember("battery");
    
    Value mean_battery(battery.mean_battery);
    Value var_battery(battery.var_battery);

    Document::AllocatorType& allocator = doc.GetAllocator();

    obj.AddMember("mean_battery", mean_battery, allocator);
    obj.AddMember("var_battery", var_battery, allocator);
    obj.AddMember("lasttime", battery.lasttime, allocator);

    doc.AddMember("battery", obj, allocator);
}

void AdaptiveReport::setStates(map<Metric, vector<State>> states) {
    if(states.empty())
        return;

    Value obj(kObjectType);
    doc.RemoveMember("states");

    Document::AllocatorType& allocator = doc.GetAllocator();

    for(auto &s : states){
        Value a(kArrayType);

        for(int i=0; i<s.second.size(); i++){
            a.PushBack(s.second[i], allocator);
        }

        switch(s.first){
            case FREE_CPU    : obj.AddMember("free_cpu", a, allocator); break;
            case FREE_MEMORY : obj.AddMember("free_memory", a, allocator); break;
            case FREE_DISK   : obj.AddMember("free_disk", a, allocator); break;
            case BATTERY     : obj.AddMember("battery", a, allocator); break;
        }
    }
    doc.AddMember("states", obj, allocator);
}

void AdaptiveReport::setMetrics(vector<Metric> metrics){
    Value arr(kArrayType);
    doc.RemoveMember("metrics");

    Document::AllocatorType& allocator = doc.GetAllocator();

    for(auto m : metrics) {
        arr.PushBack(m, allocator);
    }
    doc.AddMember("metrics", arr, allocator);
}

bool AdaptiveReport::getStates(map<Metric, vector<State>>& states) {
    if(!this->doc.HasMember("states") || !this->doc["states"].IsObject())
        return false;
    
    Value &val = doc["states"];

    vector<State> vc;
    if(val.HasMember("free_cpu")){
        for(auto &v : val["free_cpu"].GetArray())
            vc.push_back(static_cast<State>(v.GetInt()));
        states[FREE_CPU] = vc;
    }

    vc.clear();

    if(val.HasMember("free_memory")){
        for(auto &v : val["free_memory"].GetArray())
            vc.push_back(static_cast<State>(v.GetInt()));
        states[FREE_MEMORY] = vc;
    }

    vc.clear();

    if(val.HasMember("free_disk")){
        for(auto &v : val["free_disk"].GetArray())
            vc.push_back(static_cast<State>(v.GetInt()));
        states[FREE_DISK] = vc;
    }

    vc.clear();

    if(val.HasMember("battery")){
        for(auto &v : val["battery"].GetArray())
            vc.push_back(static_cast<State>(v.GetInt()));
        states[BATTERY] = vc;
    }

    return true;
}

bool AdaptiveReport::getMetrics(vector<Metric>& metrics){
    if( !this->doc.HasMember("metrics") || !this->doc["metrics"].IsArray())
        return false;

    Value &val = doc["metrics"];

    vector<Metric> vc;

    for(auto &v : val.GetArray()){
        vc.push_back(static_cast<Metric>(v.GetInt()));
    }
    metrics = vc;

    return true;
}


void AdaptiveReport::setReport(adaptive_report_result report) {
    this->setBattery(report.battery);
    this->setStates(report.states);
    Report::setReport(report);
}

void AdaptiveReport::setReports(std::vector<adaptive_report_result> reports) {

    Value arr(kArrayType);

    doc.RemoveMember("reports");

    Document::AllocatorType& allocator = doc.GetAllocator();

    for(auto test : reports) {

        Value hw(kObjectType);
        Value lt(kArrayType);
        Value bw(kArrayType);
        Value th(kArrayType);
        Value bt(kObjectType);
        Value st(kObjectType);

        {
            AdaptiveReport::hardware_result &hardware = test.hardware;
            
            Value cores(hardware.cores);
            Value mean_free_cpu(hardware.mean_free_cpu);
            Value var_free_cpu(hardware.var_free_cpu);
            Value memory(hardware.memory);
            Value mean_free_memory(hardware.mean_free_memory);
            Value var_free_memory(hardware.var_free_memory);
            Value disk(hardware.disk);
            Value mean_free_disk(hardware.mean_free_disk);
            Value var_free_disk(hardware.var_free_disk);

            hw.AddMember("cores", cores, allocator);
            hw.AddMember("mean_free_cpu", mean_free_cpu, allocator);
            hw.AddMember("var_free_cpu", var_free_cpu, allocator);
            hw.AddMember("memory", memory, allocator);
            hw.AddMember("mean_free_memory", mean_free_memory, allocator);
            hw.AddMember("var_free_memory", var_free_memory, allocator);
            hw.AddMember("disk", disk, allocator);
            hw.AddMember("mean_free_disk", mean_free_disk, allocator);
            hw.AddMember("var_free_disk", var_free_disk, allocator);
            hw.AddMember("lasttime", hardware.lasttime, allocator);
        }
        
        for(auto testLt : test.latency) {
            Value mean(testLt.mean);
            Value variance(testLt.variance);
            Value lasttime(testLt.lasttime);
            Value obj(kObjectType);
            obj.AddMember("target",testLt.target.getJson(allocator), allocator);
            obj.AddMember("mean",mean, allocator);
            obj.AddMember("variance",variance, allocator);
            obj.AddMember("lasttime",lasttime, allocator);

            lt.PushBack(obj, allocator);
        }

        for(auto testBw : test.bandwidth) {
            Value mean(testBw.mean);
            Value variance(testBw.variance);
            Value lasttime(testBw.lasttime);
            Value obj(kObjectType);
            obj.AddMember("target",testBw.target.getJson(allocator), allocator);
            obj.AddMember("mean",mean, allocator);
            obj.AddMember("variance",variance, allocator);
            obj.AddMember("lasttime",lasttime, allocator);

            bw.PushBack(obj, allocator);
        }

        for(auto iot : test.iot) {
            Value id(iot.id.c_str(), allocator);
            Value desc(iot.desc.c_str(), allocator);
            Value latency(iot.latency);
            Value obj(kObjectType);
            obj.AddMember("id",id, allocator);
            obj.AddMember("desc",desc, allocator);
            obj.AddMember("latency",latency, allocator);

            th.PushBack(obj, allocator);
        }

        {
            AdaptiveReport::battery_result &battery = test.battery;
    
            Value mean_battery(battery.mean_battery);
            Value var_battery(battery.var_battery);

            bt.AddMember("mean_battery", mean_battery, allocator);
            bt.AddMember("var_battery", var_battery, allocator);
            bt.AddMember("lasttime", battery.lasttime, allocator);
        }

        {
            map<Metric, vector<State>> &states = test.states;

            Value a(kArrayType);
            for(auto &s : states){
                for(int i=0; i<s.second.size(); i++)
                    a.PushBack(s.second[i], allocator);
                
                switch(s.first){
                    case FREE_CPU    : st.AddMember("free_cpu", a, allocator); break;
                    case FREE_MEMORY : st.AddMember("free_memory", a, allocator); break;
                    case FREE_DISK   : st.AddMember("free_disk", a, allocator); break;
                    case BATTERY     : st.AddMember("battery", a, allocator); break;
                }
                a.Clear();
            }
        }

        Value obj(kObjectType);

        Value leader(test.leader.c_str(), allocator);

        obj.AddMember("source",test.source.getJson(allocator), allocator);
        obj.AddMember("hardware",hw, allocator);
        obj.AddMember("latency",lt, allocator);
        obj.AddMember("bandwidth",bw, allocator);
        obj.AddMember("iot",th, allocator);
        obj.AddMember("battery",bt, allocator);
        obj.AddMember("states",st,allocator);
        obj.AddMember("leader",leader, allocator);

        arr.PushBack(obj, allocator);
    }
    
    doc.AddMember("reports", arr, doc.GetAllocator());
}

bool AdaptiveReport::getBattery(battery_result& battery) {
    if( !this->doc.HasMember("battery") || !this->doc["battery"].IsObject())
        return false;

    Value &val = doc["battery"];
    
    if( !val.HasMember("mean_battery") || !val["mean_battery"].IsFloat() ||
        !val.HasMember("var_battery") || !val["var_battery"].IsFloat() ||
        !val.HasMember("lasttime") || !val["lasttime"].IsInt64())
        return false;

    battery.mean_battery = val["mean_battery"].GetFloat();
    battery.var_battery = val["var_battery"].GetFloat();
    battery.lasttime = val["lasttime"].GetInt64();

    return true;
}

bool AdaptiveReport::getReport(adaptive_report_result &report) {
    if( !this->getBattery(report.battery))
        return false;

    if( !this->getStates(report.states))
        return false;

    Report::getReport(report);
}

bool AdaptiveReport::getReports(std::vector<adaptive_report_result> &reports) {

    if( !this->doc.HasMember("reports") || !this->doc["reports"].IsArray()){
        return false;
    }
    
    reports.clear();

    for (auto& v : this->doc["reports"].GetArray()) {
        if( !v.IsObject() ||
            !v.HasMember("hardware") || !v["hardware"].IsObject() ||
            !v.HasMember("latency") || !v["latency"].IsArray() ||
            !v.HasMember("bandwidth") || !v["bandwidth"].IsArray() ||
            !v.HasMember("iot") || !v["iot"].IsArray() ||
            !v.HasMember("battery") || !v["battery"].IsObject() ||
            !v.HasMember("source") || !v["source"].IsObject() ||
            !v.HasMember("leader") || !v["leader"].IsString())
                return false;

        adaptive_report_result result;
        memset(&result.hardware,0,sizeof(AdaptiveReport::hardware_result));
        memset(&result.battery,0,sizeof(AdaptiveReport::battery_result));

        if(v.HasMember("states"))
            memset(&result.states,0,sizeof(map<Metric, vector<State>>));

        result.source.setJson(v["source"]);

        Value &val = v["hardware"];

        if( !val.HasMember("cores") || !val["cores"].IsInt() ||
            !val.HasMember("mean_free_cpu") || !val["mean_free_cpu"].IsFloat() ||
            !val.HasMember("var_free_cpu") || !val["var_free_cpu"].IsFloat() ||
            !val.HasMember("memory") || !val["memory"].IsInt64() ||
            !val.HasMember("mean_free_memory") || !val["mean_free_memory"].IsFloat() ||
            !val.HasMember("var_free_memory") || !val["var_free_memory"].IsFloat() ||
            !val.HasMember("disk") || !val["disk"].IsInt64() ||
            !val.HasMember("mean_free_disk") || !val["mean_free_disk"].IsFloat() ||
            !val.HasMember("var_free_disk") || !val["var_free_disk"].IsFloat() ||
            !val.HasMember("lasttime") || !val["lasttime"].IsInt64())
                return false;
        {
            result.hardware.cores = val["cores"].GetInt();
            result.hardware.mean_free_cpu = val["mean_free_cpu"].GetFloat();
            result.hardware.var_free_cpu = val["var_free_cpu"].GetFloat();
            result.hardware.memory = val["memory"].GetInt64();
            result.hardware.mean_free_memory = val["mean_free_memory"].GetFloat();
            result.hardware.var_free_memory = val["var_free_memory"].GetFloat();
            result.hardware.disk = val["disk"].GetInt64();
            result.hardware.mean_free_disk = val["mean_free_disk"].GetFloat();
            result.hardware.var_free_disk = val["var_free_disk"].GetFloat();
            result.hardware.lasttime = val["lasttime"].GetInt64();
        }

        for (auto& v : v["latency"].GetArray()) {
            if( !v.IsObject() ||
                !v.HasMember("target") || !v["target"].IsObject() ||
                !v.HasMember("mean") || !v["mean"].IsFloat() ||
                !v.HasMember("variance") || !v["variance"].IsFloat() ||
                !v.HasMember("lasttime") || !v["lasttime"].IsInt64())
                    return false;

            test_result test;
            test.target.setJson(v["target"]);
            test.mean = v["mean"].GetFloat();
            test.variance = v["variance"].GetFloat();
            test.lasttime = v["lasttime"].GetInt64();

            result.latency.push_back(test);
        }

        for (auto& v : v["bandwidth"].GetArray()) {
            if( !v.IsObject() ||
                !v.HasMember("target") || !v["target"].IsObject() ||
                !v.HasMember("mean") || !v["mean"].IsFloat() ||
                !v.HasMember("variance") || !v["variance"].IsFloat() ||
                !v.HasMember("lasttime") || !v["lasttime"].IsInt64())
                    return false;
    
            test_result test;
            test.target.setJson(v["target"]);
            test.mean = v["mean"].GetFloat();
            test.variance = v["variance"].GetFloat();
            test.lasttime = v["lasttime"].GetInt64();

            result.bandwidth.push_back(test);
        }

        for (auto& v : v["iot"].GetArray()) {
            if( !v.IsObject() ||
                !v.HasMember("id") || !v["id"].IsString() ||
                !v.HasMember("desc") || !v["desc"].IsString() ||
                !v.HasMember("latency") || !v["latency"].IsInt())
                    return false;
            
            IoT iot;
            iot.id = string(v["id"].GetString());
            iot.desc = string(v["latency"].GetString());
            iot.latency = v["latency"].GetInt();

            result.iot.push_back(iot);
        }

        val = v["battery"]; 

        if( !val.HasMember("mean_battery") || !val["mean_battery"].IsFloat() ||
            !val.HasMember("var_battery") || !val["var_battery"].IsFloat() ||
            !val.HasMember("lasttime") || !val["lasttime"].IsInt64())
                return false;

        {
            result.battery.mean_battery = val["mean_battery"].GetFloat();
            result.battery.var_battery = val["var_battery"].GetFloat();
            result.battery.lasttime = val["lasttime"].GetInt64();
        }

        val = v["states"];

        {
            vector<State> vc;

            if(val.HasMember("free_cpu")){
                for(auto &i : val["free_cpu"].GetArray())
                    vc.push_back(static_cast<State>(i.GetInt()));
                result.states[FREE_CPU] = vc;
            }
            vc.clear();

            if(val.HasMember("free_memory")){
                for(auto &i : val["free_memory"].GetArray())
                    vc.push_back(static_cast<State>(i.GetInt()));
                result.states[FREE_MEMORY] = vc;
            }
            vc.clear();

            if(val.HasMember("free_disk")){
                for(auto &i : val["free_disk"].GetArray())
                    vc.push_back(static_cast<State>(i.GetInt()));
                result.states[FREE_DISK] = vc;
            }
            vc.clear();

            if(val.HasMember("battery")){
                for(auto &i : val["battery"].GetArray())
                    vc.push_back(static_cast<State>(i.GetInt()));
                result.states[BATTERY] = vc;
            }
        }

        result.leader = v["leader"].GetString();
        reports.push_back(result);
    }

    return true;
}
#include "adaptive_controller.hpp"
#include "iadaptivefollower.hpp"
#include <iostream>

AdaptiveController::AdaptiveController(){
    //this->running = false;
}

AdaptiveController::~AdaptiveController() {
    try{
        delete this->rule;
        this->rule = NULL;
    }catch(...) {}
}

void AdaptiveController::initialize(IAdaptiveFollower* node) {
    this->node = node;

    this->rule = new Rule();
    //vector<string> paths;


    //paths.push_back("clips/rules-exp2.clp");
    //this->rule->initialize(paths);
}

void AdaptiveController::start() {

    vector<string> paths;

    bool ex1_dm = false; bool ex1_ctr = false;

    for(auto &op : this->node->node->options){
        if(op == "clips-exp1-dm")
            ex1_dm = true;

        if(op == "clips-exp1-ctr"){
            cout << "here" << endl;
            ex1_ctr = true;
        }
        
        if(op == "clips-exp2"){
            paths.push_back("clips/rules-exp2.clp");
        }
    }

    if(ex1_dm && ex1_ctr){
        paths.push_back("clips/rules-exp1-dm-ctr.clp");
    }else if(ex1_dm){
        paths.push_back("clips/rules-exp1-dm.clp");
    }else if(ex1_ctr){
        paths.push_back("clips/rules-exp1-ctr.clp");
    }

    this->rule->initialize(paths);

    //this->running = true;
    //this->statesThread = thread(&AdaptiveController::statesTimer, this);
}

void AdaptiveController::stop() {
    /*
    this->running = false;
    if(this->statesThread.joinable()){
        this->statesThread.join();
    }
    */
}

map<Metric, vector<State>> AdaptiveController::getStates(){
    return this->states;
}

void AdaptiveController::addState(Metric metric, State state){
    this->states[metric].push_back(state);
}

void AdaptiveController::saveStates(){
    for(auto &s : this->states){
        this->node->getStorage()->saveStates(s.second, s.first);
    }
}

void AdaptiveController::statesTimer(){

    this->series.clear();
    this->states.clear();

    vector<Metric> met = this->node->getMetrics(); 

    for(auto const &m : met) {

        vector<float> data;
        data = this->node->getStorage()->getLastValues(m, this->history);

        int i=0;
        bool stop = false;
        vector<float> res;

        while(!stop && i<data.size()){
            if(data[i] != 0.0){
                res.push_back(data[i]);
            }else{
                stop = true;
            }
            i++;
        }
           
        if(!res.empty()){
            this->series[m] = res;
        }
    }

    this->stable();
    this->increasing();
    this->decreasing();
    this->alarms();

    this->saveStates();
        
    this->rule->run();         // trigger to CLIPS rules engine

    this->toStringSeries();
    this->toStringStates();
}

void AdaptiveController::stable(float delta_max, float tol){
    
    for(auto s : this->series){
        
        int k = s.second.size()-1;

        if(k == 0)     // only one sample
            continue;

        State first_sample = State::NONE;
        if(abs(s.second[0] - s.second[1] < delta_max)){
            first_sample = State::STABLE;
        }else{
            first_sample = State::UNSTABLE;
        }
        
        int stables = 0;

        for(int i=0; i<k; i++){
            float current_sample = s.second[i];
            float prev_sample = s.second[i+1];

            if(abs(current_sample - prev_sample) < delta_max){
                stables += 1;
            }
        }

        if(first_sample == State::STABLE && stables >= ceil(tol*k)){
            this->states[s.first].push_back(State::STABLE);
        }else{
            this->states[s.first].push_back(State::UNSTABLE);
        }
    }
}


void AdaptiveController::increasing(float tol) {
    for(auto s : this->series){

        int k = s.second.size() - 1;

        if(k == 0)     // only one sample
            continue;

        //State first_sample = State::NONE;
        //if(s.second[0] > s.second[1]){
        //    first_sample = State::INCREASING;
        //}
        

        int increasing = 0;
        for (int i=0; i<k; i++){
            float current_sample = s.second[i];
            float prev_sample = s.second[i+1];

            if(current_sample > prev_sample){
                increasing += 1;
            }
        }
        
       
        // first_sample == State::INCREASING &&
        if(increasing >= ceil(tol*k) && s.second[0]>s.second[k]){
            this->states[s.first].push_back(State::INCREASING);
        }
    }
}


void AdaptiveController::decreasing(float tol){
    for(auto s : this->series){
        
        int k = s.second.size() - 1;

        if(k == 0)     // only one sample
            continue;

        
        //State first_sample = State::NONE;
        //if(s.second[k] < s.second[k-1]){
        //    first_sample = State::DECREASING;
        //}
        

        int decreasing = 0;
        for (int i=0; i<k; i++){
            float current_sample = s.second[i];
            float prev_sample = s.second[i+1];

            if(current_sample < prev_sample){
                decreasing += 1;
            }
        }
        // first_sample == State::DECREASING &&
        if(decreasing >= ceil(tol*k) && s.second[0]<s.second[k]){
            this->states[s.first].push_back(State::DECREASING);
        }
    }
}


void AdaptiveController::alarms(float tol, float too_high, float too_low, float alarming_high, float alarming_low){
    for(auto s : this->series){

        int k = s.second.size() - 1;

        if(k == 0)     // only one sample
            continue;

        State first_sample;
        if(s.second[0] > too_high){
            first_sample = State::TOO_HIGH;
        }else if(s.second[0] < too_low){
            first_sample = State::TOO_LOW;
        }else if(s.second[0] > alarming_high){
            first_sample = State::ALARMING_HIGH;
        }else if(s.second[0] < alarming_low){
            first_sample = State::ALARMING_LOW;
        }else{
            first_sample = State::OK;
        }

        int too_high_c = 0, too_low_c = 0, alarming_high_c = 0, alarming_low_c = 0, ok_c = 0;

        for(int i=0; i<k; i++){
            float sample = s.second[i];

            if(sample > too_high){
                too_high_c += 1;
            }else if(sample < too_low){
                too_low_c +=1;
            }else if(sample > alarming_high){
                alarming_high_c += 1;
            }else if(sample < alarming_low){
                alarming_low_c += 1;
            }else{
                ok_c += 1;
            }
        }

        if(first_sample == State::TOO_HIGH && too_high_c >= ceil(tol*k)){
            this->states[s.first].push_back(State::TOO_HIGH);
        }else if(first_sample == State::TOO_LOW && too_low_c >= ceil(tol*k)){
            this->states[s.first].push_back(State::TOO_LOW);
        }else if(first_sample == State::ALARMING_HIGH && alarming_high_c >= ceil(tol*k)){
            this->states[s.first].push_back(State::ALARMING_HIGH);
        }else if(first_sample == State::ALARMING_LOW && alarming_low_c >= ceil(tol*k)){
            this->states[s.first].push_back(State::ALARMING_LOW);
        }else{
            this->states[s.first].push_back(State::OK);
        }
    }
}


void AdaptiveController::toStringSeries(){
    cout << "************ Series ************" << endl;

    for(auto s : this->series){
        switch(s.first){
            case Metric::FREE_CPU:          cout << "Free CPU:"; break;
            case Metric::FREE_MEMORY:       cout << "Free Memory:"; break;
            case Metric::FREE_DISK:         cout << "Free Disk:"; break;
            case Metric::BATTERY:           cout << "Battery:"; break;
        }

        for(int i=0; i<s.second.size(); i++){
            cout << s.second[i];
            if(i!=s.second.size()-1){
                cout << ",";
            }
        }
        cout << endl;
    }
    cout << "********************************" << endl; 
}

void AdaptiveController::toStringStates(){
    cout << "************ Current state ************" << endl;

    for(auto s : this->states){
        switch(s.first){
            case Metric::FREE_CPU:          cout << "Free CPU:"; break;
            case Metric::FREE_MEMORY:       cout << "Free Memory:"; break;
            case Metric::FREE_DISK:         cout << "Free Disk:"; break;
            case Metric::BATTERY:           cout << "Battery:"; break;

        }

        for(int i=0; i<s.second.size(); i++){
            switch(s.second[i]){
                case State::STABLE:         cout << " Stable"; break;
                case State::UNSTABLE:       cout << " Unstable"; break;
                case State::INCREASING:     cout << " Increasing"; break;
                case State::DECREASING:     cout << " Decreasing"; break;
                case State::TOO_HIGH:       cout << " Too_High"; break;
                case State::TOO_LOW:        cout << " Too_Low"; break;
                case State::ALARMING_HIGH:  cout << " Alarming_High"; break;
                case State::ALARMING_LOW:   cout << " Alarming_Low"; break;
                case State::OK:             cout << " Ok"; break;
            }
            if(i!=s.second.size()-1){
                cout << ",";
            }
        }
        cout << endl;
    }
    cout << "**************************************" << endl; 
}

#include "rule.hpp"
#include <vector>
#include <string>
#include <iostream>


Rule::Rule(){
    this->env = CreateEnvironment();
}

Rule::~Rule(){
    DestroyEnvironment(this->env);
}

void Rule::initialize(vector<string> paths){
    for(auto &path : paths){
        Load(this->env, path.c_str());
    }
}

void Rule::run(){
    
    Watch(this->env,FACTS);
    Watch(this->env,RULES);
    Watch(this->env,ACTIVATIONS);

    Reset(this->env);
    Run(this->env, -1);
}

Environment* Rule::getEnv(){
    return this->env;
}
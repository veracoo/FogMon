#include "adaptive_follower_connections.hpp"
#include <iostream>

#include <sys/socket.h>
#include <unistd.h>
#include "adaptive_report.hpp"
#include "adaptive_message.hpp"


AdaptiveFollowerConnections::AdaptiveFollowerConnections(int nThread) : FollowerConnections(nThread) {
}

AdaptiveFollowerConnections::~AdaptiveFollowerConnections() {
}

void AdaptiveFollowerConnections::initialize(IAdaptiveFollower* parent) {
    Connections::initialize(parent);
    this->parent = parent;
}


std::optional<std::pair<int64_t,Message::node>> AdaptiveFollowerConnections::sendUpdate(Message::node ipS, std::pair<int64_t,Message::node> update) {
    cout << "sendUpdate0" << endl;
    int Socket = openConnection(ipS.ip, ipS.port);
    cout << "sendUpdate0.5" << endl;
    if(Socket < 0) {
        return nullopt;
    }

    fflush(stdout);
    char buffer[10];

    //build update message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeNOTIFY);
    m.setCommand(Message::Command::commUPDATE);
    m.setArgument(Message::Argument::argREPORT);

    AdaptiveReport r;

    int64_t now = this->parent->getStorage()->getTime();
    int64_t time = update.first;

    bool skip = false;
    for(auto &m : this->parent->getMetrics()){
        if(m == FREE_CPU || m == FREE_MEMORY || m == FREE_DISK){
            if(!skip){
                r.setHardware(this->parent->getStorage()->getHardware());
                skip = true;
            }
        }else if(m == BATTERY){
            r.setBattery(this->parent->getStorage()->getBattery());
        }else if(m == LATENCY){
            if(ipS == update.second) {          // se l'ultima update è stata fatta a questo ip
                r.setLatency(this->parent->getStorage()->getLatency(this->parent->node->sensitivity,time));
        
            } else { //send all data
                r.setLatency(this->parent->getStorage()->getLatency(0));
            }
        }else if(m == BANDWIDTH){
            if(ipS == update.second) {          // se l'ultima update è stata fatta a questo ip
                r.setBandwidth(this->parent->getStorage()->getBandwidth(this->parent->node->sensitivity,time));
        
            } else { //send all data
                r.setBandwidth(this->parent->getStorage()->getBandwidth(0));
            }
        }else if(m == CONNECTED_IOTS){
            r.setIot(this->parent->getStorage()->getIots());
        }
    }

    r.setMetrics(this->parent->getMetrics());
    r.setStates(this->parent->getAdaptiveController()->getStates());

    m.setData(r);

    std::optional<std::pair<int64_t,Message::node>> result = nullopt;
    cout << "sendUpdate1" << endl;
    //send update message
    if(this->sendMessage(Socket, m)) {
        cout << "sendUpdate2" << endl;
        Message res;
        if(this->getMessage(Socket, res)) {
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commUPDATE &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                
                result = std::make_pair(now, ipS); // aggiornamento del dato membro 'update' con nodo e tempo dell'ultimo update
                this->parent->getStorage()->saveState(time,this->parent->node->sensitivity);        // ??? non viene svolto
            }
        }
    }
    cout << "sendUpdate3" << endl;
    close(Socket);
    return result;
}


void AdaptiveFollowerConnections::handler(int fd, Message &m){
    string strIp = this->getSource(fd,m);

    bool handled = false;

    if(m.getType() == Message::Type::typeREQUEST){
        if(m.getCommand() == Message::Command::commGET){
            if(m.getArgument() == Message::Argument::argREPORT){
                handled = true;

                //build report
                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commGET);
                res.setArgument(Message::Argument::argPOSITIVE);

                AdaptiveReport r;

                bool skip = false;
                for(auto &m : this->parent->getMetrics()){
                    if(m == FREE_CPU || m == FREE_MEMORY || m == FREE_DISK){
                        if(!skip){
                            r.setHardware(this->parent->getStorage()->getHardware());
                            skip = true;
                        }
                    }else if(m == BATTERY){
                        r.setBattery(this->parent->getStorage()->getBattery());
                    }
                }
                

                r.setLatency(this->parent->getStorage()->getLatency(this->parent->node->sensitivity));
                r.setBandwidth(this->parent->getStorage()->getBandwidth(this->parent->node->sensitivity));
                r.setIot(this->parent->getStorage()->getIots());

                res.setData(r);

                //send report
                this->sendMessage(fd, res);
            }
        }else if(m.getCommand() == Message::Command::commDISABLE){
            if(m.getArgument() == Message::Argument::argMETRICS){
                handled = true;

                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commDISABLE);
                res.setArgument(Message::Argument::argPOSITIVE);

                vector<int> data;
                m.getData(data);

                vector<Metric> metrics;
                for(auto &d : data){
                    metrics.push_back(static_cast<Metric>(d));
                }

                this->parent->disableMetrics(metrics);

                this->sendMessage(fd, res);
            }
        }else if(m.getCommand() == Message::Command::commENABLE){
            if(m.getArgument() == Message::Argument::argMETRICS){
                handled = true;

                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commENABLE);
                res.setArgument(Message::Argument::argPOSITIVE);

                vector<int> data;
                m.getData(data);

                vector<Metric> metrics;
                for(auto &d : data){
                    metrics.push_back(static_cast<Metric>(d));
                }

                this->parent->enableMetrics(metrics);

                this->sendMessage(fd, res);
            }
        }
    } else if(m.getType() == Message::Type::typePREQUEST){
        if(m.getCommand() == Message::Command::commSET) {
            if(m.getArgument() == Message::Argument::argPARAM_TIME_REPORT){

                int time_report_new;
                m.getData(time_report_new);
                cout << "Time Report update: " <<  time_report_new << endl;

                Message res; 
                    res.setType(Message::Type::typePRESPONSE);
                    res.setCommand(Message::Command::commSET);

                if (!m.getData(time_report_new)) {
                    res.setArgument(Message::Argument::argNEGATIVE);
                }else {
                    res.setArgument(Message::Argument::argPOSITIVE);
                    this->parent->node->timeReport = time_report_new;
                }
                this->sendMessage(fd, res);
            }
        }
    }else if(m.getType() == Message::Type::typeNOTIFY){
        if(m.getCommand() == Message::Command::commSELECT_NEW_SERVER){
            if(m.getArgument() == Message::Argument::argMNODES){
                handled = true;

                vector<Message::node> res;
                m.getData(res);
                
                if(!this->parent->changeServer(res)){
                    cout << "Server not changed" << endl;
                } else{
                    cout << "Server changed" << endl;
                }
            }
        }
    }

    if(!handled)
        this->call_super_handler(fd, m);
}

bool AdaptiveFollowerConnections::sendHello(Message::node ipS) {
    cout << "Trying server " << ipS.id << " " << ipS.ip << ":" << ipS.port <<endl;
    int Socket = openConnection(ipS.ip, ipS.port);
    if(Socket < 0) {
        return false;
    }

    fflush(stdout);
    char buffer[10];

    //build hello message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeNOTIFY);
    m.setCommand(Message::Command::commHELLO);
    AdaptiveReport r;
    

    bool skip = false;
    for(auto &m : this->parent->getMetrics()){
        if(m == FREE_CPU || m == FREE_MEMORY || m == FREE_DISK){
            if(!skip){
                r.setHardware(this->parent->getStorage()->getHardware());
                skip = true;
            }
        }else if(m == BATTERY){
            r.setBattery(this->parent->getStorage()->getBattery());
        }
    }
    
    m.setData(r);
    bool result = false;

    //send hello message
    if(this->sendMessage(Socket, m)) {
        Message res;
        if(this->getMessage(Socket, res)) {
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commHELLO &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                Message::node node;
                vector<Message::node> vec;
                if(res.getData(node, vec)) {
                    cout << "My id: " << node.id << " " << node.ip << ":" << node.port << endl;
                    this->parent->getStorage()->setFilter(ipS.ip);
                    this->parent->getStorage()->refreshNodes(vec);
                    result = true;
                    cout << "Server: " << ipS.id << " " << ipS.ip << ":" << ipS.port << endl;
                }
            }
        }
    }
    
    return result;
}



void AdaptiveFollowerConnections::call_super_handler(int fd, Message &m) {
    FollowerConnections::handler(fd, m);
}
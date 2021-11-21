#include "adaptive_leader_connections.hpp"

#include <iostream>
#include <unistd.h>

AdaptiveLeaderConnections::AdaptiveLeaderConnections(int nThread) : LeaderConnections(nThread),  AdaptiveFollowerConnections(nThread), FollowerConnections(nThread) { }

AdaptiveLeaderConnections::~AdaptiveLeaderConnections() { }

void AdaptiveLeaderConnections::initialize(IAdaptiveLeader* parent) {
    LeaderConnections::initialize(parent);
    AdaptiveFollowerConnections::parent = parent;
    this->parent = parent;
}

void AdaptiveLeaderConnections::handler(int fd, Message &m){
    
    string strIp = this->getSource(fd,m);

    bool handled = false;

    if(m.getType() == Message::Type::typeMREQUEST) {
        if(m.getCommand() == Message::Command::commSET) {
            if(m.getArgument() == Message::Argument::argREPORT) {

                handled = true;
                AdaptiveReport r;

                // Too long reponding
                Message res;
                res.setType(Message::Type::typeMRESPONSE);
                res.setCommand(Message::Command::commSET);
                res.setArgument(Message::Argument::argPOSITIVE);

                sendMessage(fd, res);

                // Do this in another thread
                if(m.getData(r)) {
                    vector<AdaptiveReport::adaptive_report_result> results;
                    if(r.getReports(results)) {
                        this->parent->getStorage()->addReport(results, m.getSender());
                    }
                }
            } else if(m.getArgument() == Message::Argument::argROLES) {
                Message::leader_update update;
                //contains the list of new leaders
                m.getData(update);

                cout << "AdaptiveLeaderConnections::handler()" << endl;
                for(auto &u : update.selected){
                    cout << u.ip << endl;
                }

                cout << "Selected Leaders: "<< update.selected.size() << endl;
                for(auto &node : update.selected) {
                    if(node.ip == "::1" || node.ip == "127.0.0.1") {
                        node.ip = strIp;
                    }
                    cout << node.ip << endl;
                }
                this->parent->changeRole(update.selected);

                Message res;
                res.setType(Message::Type::typeMRESPONSE);
                res.setCommand(Message::Command::commSET);
                res.setArgument(Message::Argument::argPOSITIVE);

                sendMessage(fd, res);
            }
        }
    } else if(m.getType() == Message::Type::typeNOTIFY){
        if(m.getCommand() == Message::Command::commHELLO){
            handled = true;

            //get report
            AdaptiveReport r;
            if(m.getData(r)) {
                Report::hardware_result hardware;
                r.getHardware(hardware);

                AdaptiveReport::battery_result battery;
                r.getBattery(battery);

                Message::node sender = m.getSender();
                //sender.id = "";
                //if(sender.ip == "::1" && sender.port == this->parent->getMyNode().port) {
                //    sender.id = this->parent->getMyNode().id;
                //}

                //set new node online                
                sender.id = this->parent->getStorage()->addNode(sender, hardware, battery);
                
                vector<Message::node> vec = this->parent->getStorage()->getNodes();

                //get nodelist
                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commHELLO);
                res.setArgument(Message::Argument::argPOSITIVE);

                res.setData(sender, vec);
                
                sendMessage(fd, res);

                //inform all the other nodes about it
                //
                Message broadcast;
                broadcast.setType(Message::Type::typeNOTIFY);
                broadcast.setCommand(Message::Command::commUPDATE);
                broadcast.setArgument(Message::Argument::argNODES);
                vector<Message::node> v;
                v.push_back(sender);
                vector<Message::node> v2;
                broadcast.setData(v ,v2);

                this->notifyAll(broadcast);
            }

        }else if(m.getCommand() == Message::Command::commUPDATE){
            if(m.getArgument() == Message::Argument::argREPORT){
                handled = true;
                //get the report
                //the report should be only a part of it
                AdaptiveReport r;

                if(m.getData(r)) {
                    Message res;
                    res.setType(Message::Type::typeRESPONSE);
                    res.setCommand(Message::Command::commUPDATE);
                    res.setArgument(Message::Argument::argPOSITIVE);
                    
                    sendMessage(fd, res);

                    AdaptiveReport::hardware_result hardware;
                    vector<AdaptiveReport::test_result> latency;
                    vector<AdaptiveReport::test_result> bandwidth;
                    vector<AdaptiveReport::IoT> iot;
                    AdaptiveReport::battery_result battery;
                    map<Metric, vector<State>> states;
                    vector<Metric> metrics;

                    bool hw = r.getHardware(hardware);
                    bool bt = r.getBattery(battery);


                    if(hw || bt) {

                        auto now = std::chrono::system_clock::now();
                        auto UTC = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
                        
                        //auto in_time_t = std::chrono::system_clock::to_time_t(now);
                        //std::stringstream datetime;
                        //datetime << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");


                        if(this->parent->getReceivedCpuLogs() &&
                           m.getSender().id != this->parent->getMyNode().id){
                            this->f.open("monitoring_logs/CPU_leader_adp.csv", ios_base::out | ios_base::app);

                            if(this->f.is_open()){
                                this->f << hardware.mean_free_cpu << " " << std::to_string(UTC) << "\n"; 
                                this->f.close();
                            } else{
                                cout << "Unable to open file." << endl;
                            }
                        }
                        
                        this->parent->getStorage()->addNode(m.getSender(), hardware, battery);
                    }
                    if(r.getLatency(latency)) {
                        this->parent->getStorage()->addReportLatency(m.getSender(), latency);
                    }
                    if(r.getBandwidth(bandwidth)) {
                        this->parent->getStorage()->addReportBandwidth(m.getSender(), bandwidth);
                    }
                    if(r.getIot(iot)) {
                        this->parent->getStorage()->addReportIot(m.getSender(), iot);
                    }
                    if(r.getStates(states)){
                        this->parent->getStorage()->addReportStates(m.getSender(), states);
                    }
                    if(r.getMetrics(metrics)){
                        this->parent->getStorage()->addReportMetrics(m.getSender(), metrics);
                    }
                }else {
                    Message res;
                    res.setType(Message::Type::typeRESPONSE);
                    res.setCommand(Message::Command::commUPDATE);
                    res.setArgument(Message::Argument::argNEGATIVE);
                    
                    sendMessage(fd, res);
                }
            }
        }
    }

    if(!handled)
        LeaderConnections::handler(fd, m);
    
    if(!handled)
        AdaptiveFollowerConnections::handler(fd, m);
}


void AdaptiveLeaderConnections::call_super_handler(int fd, Message &m) { }


bool AdaptiveLeaderConnections::sendRequestReport(Message::node ip){
    int Socket = openConnection(ip.ip,ip.port);
    
    if(Socket < 0) {
        return false;
    }

    fflush(stdout);
    char buffer[10];

    //build message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeREQUEST);
    m.setCommand(Message::Command::commGET);
    m.setArgument(Message::Argument::argREPORT);

    bool ret = false;

    //send message
    if(this->sendMessage(Socket, m)) {
        Message res;
        if(this->getMessage(Socket, res)) {
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commGET &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                //get report and save it
                AdaptiveReport r;
                if(m.getData(r)) {
                    Report::hardware_result hardware;
                    vector<Report::test_result> latency;
                    vector<Report::test_result> bandwidth;
                    vector<Report::IoT> iot;
                    AdaptiveReport::battery_result battery;
                    
                    bool hw = r.getHardware(hardware);
                    bool bt = r.getBattery(battery);

                    if(hw || bt) {
                        this->parent->getStorage()->addNode(ip, hardware, battery);
                    }
                    if(r.getLatency(latency)) {
                        this->parent->getStorage()->addReportLatency(ip, latency);
                    }
                    if(r.getBandwidth(bandwidth)) {
                        this->parent->getStorage()->addReportBandwidth(ip, bandwidth);
                    }
                    if(r.getIot(iot)) {
                        this->parent->getStorage()->addReportIot(ip, iot);
                    }
                    ret = true;
                }
            }
        }
    }
    close(Socket);
    return ret;
}

bool AdaptiveLeaderConnections::sendMReport(Message::node ip, vector<AdaptiveReport::adaptive_report_result> report) {
    int Socket = this->openConnection(ip.ip, ip.port);
    if(Socket < 0) {
        return false;
    }
    fflush(stdout);
    char buffer[10];

    //build message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeMREQUEST);
    m.setCommand(Message::Command::commSET);
    m.setArgument(Message::Argument::argREPORT);

    AdaptiveReport r;
    r.setReports(report);

    m.setData(r);
    bool ret = false;

    //send message
    if(this->sendMessage(Socket, m)) {
        Message res;
        if(this->getMessage(Socket, res)) {
            if( res.getType()==Message::Type::typeMRESPONSE &&
                res.getCommand() == Message::Command::commSET &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                ret = true;
            }
        }
    }
    close(Socket);
    return ret;
}

bool AdaptiveLeaderConnections::sendChangeServer(){
    vector<Message::node> mnodes = this->parent->getStorage()->getMNodes();

    cout << "MNodes:" << endl;
    for(auto &n : mnodes){
        cout << n.ip << endl;
    }

    vector<Message::node> nodes;
    for(int i=0; i<mnodes.size(); i++){
        if(mnodes[i].ip != this->parent->getMyNode().ip)
            nodes.push_back(mnodes[i]);
    }

    cout << "Nodes:" << endl;
    for(auto &n : nodes){
        cout << n.ip << endl;
    }

    Message broadcast;
    broadcast.setSender(this->parent->getMyNode());
    broadcast.setType(Message::Type::typeNOTIFY);
    broadcast.setCommand(Message::Command::commSELECT_NEW_SERVER);
    broadcast.setArgument(Message::Argument::argMNODES);
    broadcast.setData(nodes);

    cout << "Sending changeServer()" << endl;

    return this->notifyAll(broadcast);
}


bool AdaptiveLeaderConnections::sendChangeTimeReport(Message::node ip, int newTimeReport){
    int Socket = openConnection(ip.ip, ip.port);

    if (Socket < 0){
        return false;
    }

    fflush(stdout);
    char buffer[10];

    // build message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typePREQUEST);
    m.setCommand(Message::Command::commSET);
    m.setArgument(Message::Argument::argPARAM_TIME_REPORT);

    m.setData(newTimeReport);

    bool ret = false;

    // send message
    if (this->sendMessage(Socket, m)){
        ret = true;
        
        Message res;
        if (this->getMessage(Socket, res)){
            if( res.getType()==Message::Type::typePRESPONSE &&
                res.getCommand() == Message::Command::commSET &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                    ret = true;
                }
        }
    }

    close(Socket);
    return ret;
}

 bool AdaptiveLeaderConnections::sendRemoveLeader(Message::leader_update update) {
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeMREQUEST);
    m.setCommand(Message::Command::commSET);
    m.setArgument(Message::Argument::argROLES);

    m.setData(update);

    return this->notifyAllM(m);
 }

bool AdaptiveLeaderConnections::sendDisableMetrics(Message::node ip, vector<Metric> metrics) {
    int Socket = openConnection(ip.ip, ip.port);

    if (Socket < 0){
        return false;
    }

    fflush(stdout);
    char buffer[10];

    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeREQUEST);
    m.setCommand(Message::Command::commDISABLE);
    m.setArgument(Message::Argument::argMETRICS);

    vector<int> vc;
    for(auto &m : metrics){
        vc.push_back(static_cast<int>(m));
    }

    m.setData(vc);

    bool ret = false;

    // send message
    if (this->sendMessage(Socket, m)){
        
        Message res;
        if (this->getMessage(Socket, res)){
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commDISABLE &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                    ret = true;
                }
        }
    }

    close(Socket);
    return ret;
}

bool AdaptiveLeaderConnections::sendEnableMetrics(Message::node ip, vector<Metric> metrics) {
    int Socket = openConnection(ip.ip, ip.port);

    if (Socket < 0){
        return false;
    }

    fflush(stdout);
    char buffer[10];

    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeREQUEST);
    m.setCommand(Message::Command::commENABLE);
    m.setArgument(Message::Argument::argMETRICS);

    vector<int> vc;
    for(auto &m : metrics){
        vc.push_back(static_cast<int>(m));
    }

    m.setData(vc);

    bool ret = false;

    // send message
    if (this->sendMessage(Socket, m)){
        
        Message res;
        if (this->getMessage(Socket, res)){
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commENABLE &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                    ret = true;
                }
        }
    }

    close(Socket);
    return ret;
}
#include "follower_connections.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/un.h>
#include <string>
#include <netdb.h>
#include <iostream>

using namespace std;

FollowerConnections::FollowerConnections(int nThread) : Connections(nThread) {
}

FollowerConnections::~FollowerConnections() {
}

void FollowerConnections::initialize(IAgent *parent) {
    Connections::initialize(parent);
    this->parent = parent;
}

void FollowerConnections::handler(int fd, Message &m) {
    string strIp = this->getSource(fd,m);

    if(m.getType() == Message::Type::typeREQUEST) {
        if(m.getArgument() == Message::Argument::argIPERF) {
            if(m.getCommand() == Message::Command::commSTART) {
                
                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commSTART);
                int port = this->parent->getIperfPort();
                if(port > 0) {
                    res.setArgument(Message::Argument::argPOSITIVE);
                    res.setData(port);
                }else {
                    res.setArgument(Message::Argument::argNEGATIVE);
                }
                //send response
                if(this->sendMessage(fd, res)) {
                    
                }
            }
        }else if(m.getArgument() == Message::Argument::argESTIMATE) {
            if(m.getCommand() == Message::Command::commSTART) {
                
                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commSTART);
                int port = this->parent->getEstimatePort();
                string a = string(" ");
                string port_ = to_string(port);
                Message::node val(a,strIp,port_);
                if(port > 0) {
                    res.setArgument(Message::Argument::argPOSITIVE);
                    res.setData(val);
                }else {
                    res.setArgument(Message::Argument::argNEGATIVE);
                }
                //send response
                if(this->sendMessage(fd, res)) {
                    
                }
            }
        }else if(m.getArgument() == Message::Argument::argNODES) {
            if(m.getCommand() == Message::Command::commGET) {
                //build array of nodes
                vector<Message::node> nodes = this->parent->getStorage()->getNodes();

                //send nodes
                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commGET);
                res.setArgument(Message::Argument::argPOSITIVE);

                res.setData(nodes);
            
                if(this->sendMessage(fd, res)) {
                    
                }
            }else if(m.getCommand() == Message::Command::commSET) {
                Message res; //--------------------------------
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commSET);

                //refresh all the nodes with the array of nodes
                vector<Message::node> ips;
                if(!m.getData(ips)) {
                    res.setArgument(Message::Argument::argNEGATIVE);
                }else {
                    res.setArgument(Message::Argument::argPOSITIVE);
                }
                //ips now contains the ip of the nodes
                this->parent->getStorage()->refreshNodes(ips);

                //send report
                if(this->sendMessage(fd, res)) {
                    
                }
            }
        }else if(m.getArgument() == Message::Argument::argMNODES) {
            if(m.getCommand() == Message::Command::commGET) {
                //build array of nodes
                vector<Message::node> nodes = this->parent->node->getMNodes();
                //send nodes
                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commMNODELIST);
                res.setArgument(Message::Argument::argPOSITIVE);

                res.setData(nodes);
            
                sendMessage(fd, res);
            }
        }else if(m.getArgument() == Message::Argument::argREPORT) {
            if(m.getCommand() == Message::Command::commGET) {
                //build report
                Message res;
                res.setType(Message::Type::typeRESPONSE);
                res.setCommand(Message::Command::commGET);
                res.setArgument(Message::Argument::argPOSITIVE);
                Report r;
                
                r.setHardware(this->parent->getStorage()->getHardware());
                r.setLatency(this->parent->getStorage()->getLatency(this->parent->node->sensitivity));
                r.setBandwidth(this->parent->getStorage()->getBandwidth(this->parent->node->sensitivity));
                r.setIot(this->parent->getStorage()->getIots());
                res.setData(r);

                //send report
                if(this->sendMessage(fd, res)) {
                    
                }
            }
        }else if(m.getArgument() == Message::Argument::argROLES) {
            if(m.getCommand() == Message::Command::commSET) {
                Message::leader_update update;
                //contains the list of new leaders
                m.getData(update);

                cout << "FollowerConnections::handler()" << endl;
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
            }
        }
    }else if(m.getType() == Message::Type::typeNOTIFY) {
        if(m.getCommand() == Message::Command::commUPDATE) {
            if(m.getArgument() == Message::Argument::argNODES) {
                //data contains 2 array: new and deleted nodes
                vector<Message::node> ipsNew;
                vector<Message::node> ipsRem;
                if(!m.getData(ipsNew,ipsRem))
                    return;
                //update the nodes
                this->parent->getStorage()->updateNodes(ipsNew,ipsRem);
            }else if(m.getArgument() == Message::Argument::argREPORT) {
                cout << "I'm not a leader anymore!?" << endl;
                fflush(stdout);
                cout << "I'm not a leader anymore!? (closed)" << endl;
            }
        }
    }
}

vector<Message::node> FollowerConnections::requestNodes(Message::node ipS) {
    int Socket = openConnection(ipS.ip, ipS.port);
    
    if(Socket < 0) {
        return vector<Message::node>();
    }

    //build request message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeREQUEST);
    m.setCommand(Message::Command::commGET);
    m.setArgument(Message::Argument::argNODES);

    vector<Message::node> nodes;
    bool result = false;
    //send request message
    if(this->sendMessage(Socket, m)) {
        Message res;
        if(this->getMessage(Socket, res)) {
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commNODELIST &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                if(res.getData(nodes)) {
                    result = true;
                }
            }
        }
    }
    close(Socket);
    if(result)
        return nodes;
    return vector<Message::node>();
}

vector<Message::node> FollowerConnections::requestMNodes(Message::node ipS) {
    int Socket = openConnection(ipS.ip, ipS.port);
    
    if(Socket < 0) {
        return vector<Message::node>();
    }

    fflush(stdout);

    //build request message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeREQUEST);
    m.setCommand(Message::Command::commGET);
    m.setArgument(Message::Argument::argMNODES);

    vector<Message::node> nodes;
    bool result = false;
    //send request message
    if(this->sendMessage(Socket, m)) {
        Message res;
        if(this->getMessage(Socket, res)) {
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commMNODELIST &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                if(res.getData(nodes)) {
                    result = true;
                }
            }
        }
    }
    close(Socket);
    if(result) {
        for(auto &node : nodes) {
            if(node.ip == "::1") {
                node.ip = ipS.ip;
            }
        }
        return nodes;
    }
    return vector<Message::node>();
}

bool FollowerConnections::sendHello(Message::node ipS) {
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
    Report r;
    
    r.setHardware(this->parent->getStorage()->getHardware());

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


optional<pair<int64_t,Message::node>> FollowerConnections::sendUpdate(Message::node ipS, pair<int64_t,Message::node> update) {
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

    Report r;
    r.setHardware(this->parent->getStorage()->getHardware());
    r.setIot(this->parent->getStorage()->getIots());

    int64_t now = this->parent->getStorage()->getTime();
    int64_t time = update.first;
    if(ipS == update.second) {          // se l'ultima update è stata fatta a questo ip
        r.setLatency(this->parent->getStorage()->getLatency(this->parent->node->sensitivity,time));
        r.setBandwidth(this->parent->getStorage()->getBandwidth(this->parent->node->sensitivity,time));
        
    } else { //send all data
        r.setLatency(this->parent->getStorage()->getLatency(0));
        r.setBandwidth(this->parent->getStorage()->getBandwidth(0));
    }
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

int FollowerConnections::sendStartIperfTest(Message::node ip) {
    int Socket = openConnection(ip.ip, ip.port);
    
    if(Socket < 0) {
        return -1;
    }

    fflush(stdout);
    char buffer[10];

    //build start iperf message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeREQUEST);
    m.setCommand(Message::Command::commSTART);
    m.setArgument(Message::Argument::argIPERF);

    int port = -1;

    //send start iperf message
    if(this->sendMessage(Socket, m)) {
        Message res;
        if(this->getMessage(Socket, res)) {
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commSTART &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                
                res.getData(port);
            }
        }
    }
    close(Socket);
    return port;
}

int FollowerConnections::sendStartEstimateTest(Message::node ip, std::string &myIp) {
    int Socket = openConnection(ip.ip, ip.port);
    
    if(Socket < 0) {
        return -1;
    }

    fflush(stdout);
    char buffer[10];

    //build start estimate message
    Message m;
    m.setSender(this->parent->getMyNode());
    m.setType(Message::Type::typeREQUEST);
    m.setCommand(Message::Command::commSTART);
    m.setArgument(Message::Argument::argESTIMATE);

    int port = -1;

    //send start estimate message
    if(this->sendMessage(Socket, m)) {
        Message res;
        if(this->getMessage(Socket, res)) {
            if( res.getType()==Message::Type::typeRESPONSE &&
                res.getCommand() == Message::Command::commSTART &&
                res.getArgument() == Message::Argument::argPOSITIVE) {
                Message::node val;
                res.getData(val);
                port = stol(val.port);
                myIp = val.ip;
            }
        }
    }
    close(Socket);
    return port;
}
#ifndef NODE_HPP_
#define NODE_HPP_

#include <string>
#include <thread>
#include <vector>
#include "message.hpp"

class IAgent;

class Node {
public:

    Node(std::string port, bool isLeader, int threads, bool adp);
    ~Node();
    //start listener for incoming ping and directions
    void start();
    //stop everything
    void stop();

    bool setParam(std::string name, int value);
    
    bool setParam(std::string name, std::string value);

    // metrics generator options
    bool setParam(std::string name, std::vector<std::string> value);

    //promote to leader if follower
    virtual void promote(std::vector<Message::node> nodes);

    //demote to follower if leader
    void demote(std::vector<Message::node> nodes);

    void setMNodes(std::vector<Message::node> nodes);
    std::vector<Message::node> getMNodes();

    bool isFollower();
    bool isAdaptive();

    //configs
    int timeReport;
    int timeTests;
    int timeLatency;
    int timeBandwidth;
    int maxPerLatency;
    int maxPerBandwidth;
    int leaderCheck;

    int latencyWindow;
    int bandwidthWindow;
    int hardwareWindow;
    int sensitivity;
    int leaderFormula;

    int timesilent;
    int timePropagation;

    std::string interfaceIp;
    int session;

    std::vector<std::string> mg_options;
    std::vector<std::string> options;
    std::vector<std::string> m_en_dis_options;

protected:
    IAgent * agent;
    bool isLeader;

    bool adp;   // adaptivity support

    //list of known leaders for future reconnections
    std::vector<Message::node> mNodes;

    //thread to promote/demote
    std::thread restartThread;

    std::string id;

    int threads;
    std::string port;

    void restart(std::vector<Message::node> nodes);

    void create();

    static std::string genId();
};

#endif
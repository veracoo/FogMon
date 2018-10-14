#ifndef CONNECTIONS_HPP_
#define CONNECTIONS_HPP_

#include "inode.hpp"
#include "iconnections.hpp"
#include "queue.hpp"
#include "message.hpp"

#include <thread>

class Connections : public IConnections {
private:
    void handler(int fd, Message &m);

public:
    Connections(INode *parent, int nThread);
    ~Connections();

    //ip:port
    bool sendHello(std::string ipS);
    bool sendReport(std::string ipS);
};

#endif
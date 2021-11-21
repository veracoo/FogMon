#ifndef ADAPTIVE_FOLLOWER_CONNECTIONS_HPP_
#define ADAPTIVE_FOLLOWER_CONNECTIONS_HPP_

#include "follower_connections.hpp"
#include "iadaptivefollower_connections.hpp"
#include "iadaptivefollower.hpp"

class AdaptiveFollowerConnections : virtual public FollowerConnections, virtual public IAdaptiveFollowerConnections {
protected:
    void handler(int fd, Message &m);

    IAdaptiveFollower* parent;

    virtual void call_super_handler(int fd, Message &m);

public:
    AdaptiveFollowerConnections(int nThread);
    ~AdaptiveFollowerConnections();

    void initialize(IAdaptiveFollower* parent);
    std::optional<std::pair<int64_t,Message::node>> sendUpdate(Message::node ipS, std::pair<int64_t,Message::node> update) override;

    bool sendHello(Message::node ipS) override;
};

#endif
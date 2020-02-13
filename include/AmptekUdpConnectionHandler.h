#ifndef AmptekUdpConnectionHandler_h
#define AmptekUdpConnectionHandler_h

#include "AmptekConnectionHandler.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <semaphore.h>

#define MAX_UDP_PACKET_SIZE 520


class AmptekUdpConnectionHandler : public AmptekConnectionHandler{
public:
    AmptekUdpConnectionHandler(std::string hostname, int port, double timeout);
    virtual ~AmptekUdpConnectionHandler();
    virtual Packet sendAndReceive( const Packet& request);
    virtual void ClearCommunicationBuffer();
private:
    int sockfd;
    unsigned int addrlength;
    struct sockaddr_in server, from;
    struct hostent *host;
    sem_t mu;
};

#endif
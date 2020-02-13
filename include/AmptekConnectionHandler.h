#ifndef AmptekConnectionHandler_h
#define AmptekConnectionHandler_h

#include "packet.h"
class AmptekConnectionHandler{

public:
    virtual ~AmptekConnectionHandler(){};
    virtual Packet sendAndReceive( const Packet& request) = 0;
    virtual void ClearCommunicationBuffer() = 0;
};


#endif 
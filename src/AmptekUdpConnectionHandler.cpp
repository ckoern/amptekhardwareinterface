#include "AmptekUdpConnectionHandler.h"
#include <cstring>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <arpa/inet.h>
AmptekUdpConnectionHandler::AmptekUdpConnectionHandler(std::string hostname, int port, double timeout) 
    : AmptekConnectionHandler()
{
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        stringstream ss;
        ss << "Error opening socket: " << strerror(errno);
        AmptekException e = AmptekException(ss.str());
        throw e;
    }

    server.sin_family = AF_INET;
    host = gethostbyname(hostname.c_str());
    if (host==NULL)
    {
        stringstream ss;
        ss << "Error opening socket: host " << hostname << "does not exist";
        AmptekException e = AmptekException(ss.str());
        throw e;
    }

    memcpy((char *)host->h_addr,
        (char *)&server.sin_addr,
         host->h_length);
    server.sin_addr.s_addr = inet_addr( hostname.c_str() );
    server.sin_port = htons(port);
    addrlength=sizeof(struct sockaddr_in);

    //setting up a timeout for receiving
    if (timeout < 0)
        throw AmptekException("Error opening socket: timeout value mas be greater than 0");
    int seconds = floor(timeout);
    int useconds = floor((timeout - seconds) * 1e6);
    struct timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = useconds;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(struct timeval));
    //std::cout << "bind?: " << bind(sockfd,  (struct sockaddr *)&server, addrlength) << std::endl;
    sem_init(&mu, 0, 1);
}


AmptekUdpConnectionHandler::~AmptekUdpConnectionHandler(){
    close(sockfd);
}


Packet AmptekUdpConnectionHandler::sendAndReceive( const Packet& request){
    sem_wait(&mu);
    int n=sendto(sockfd, &(request.at(0)), request.size(), 0, (const struct sockaddr *)&server, addrlength);
    //int n=send(sockfd, &(request.at(0)), request.size(), 0);
    if (n < 0){
        sem_post(&mu);
        throw AmptekException("Failed sending  the request via UDP: " + std::string(strerror(errno)));
    }
    //std::cout << "Sent " << n << " bytes: " << request.toString() << std::endl;
    Packet p;
    byte udp_buffer[MAX_UDP_PACKET_SIZE];
    memset(udp_buffer,0,MAX_UDP_PACKET_SIZE);

    n = recvfrom(sockfd, &udp_buffer, MAX_UDP_PACKET_SIZE, 0, (struct sockaddr *)&from, &addrlength);
    //n = recv(sockfd, &udp_buffer, MAX_UDP_PACKET_SIZE, 0);
    if (n < MIN_PACKET_LEN){
        sem_post(&mu);
        throw AmptekException("Failed receiving the response via UDP. Only received " + std::to_string(n) + " bytes: " + strerror(errno));
    }
    int datalength = mergeBytes( udp_buffer[LEN_MSB], udp_buffer[LEN_LSB] );
    int packet_size = MIN_PACKET_LEN + datalength;
    //std::cout << "total response packet data size: " <<std::dec <<  datalength << std::endl;
    if (packet_size == n){
        try{
            p.resize( packet_size);
            p.fromByteArray(udp_buffer);
        }
        catch(...){
            std::cerr << p.toString() << std::endl;
            sem_post(&mu);
            throw;
        }
    }
    else{
        byte packet_buffer[packet_size];
        int offset = n;
        //std::cout << "offset at " << offset  << std::endl;
        memcpy(packet_buffer,udp_buffer, n);
        while(offset < packet_size){
            //std::cout << "Ask for " << std::min(packet_size - offset,  MAX_UDP_PACKET_SIZE) << " bytes" << std::endl;
            n = recvfrom(sockfd, &udp_buffer, std::min(packet_size - offset,  MAX_UDP_PACKET_SIZE), 0, (struct sockaddr *)&from, &addrlength);
            //n = recv(sockfd, &udp_buffer, std::min(packet_size - offset,  MAX_UDP_PACKET_SIZE), 0);
            if (n <= 0){
                sem_post(&mu);
                std::cerr << "Failed reading multi-udp packet!" << std::endl;
                std::cerr << "Data until crash:" << std::endl;
                for (int j = 0; j < offset; ++j){
                    std::cerr << j << "\t" << std::hex << "0x"<< std::setfill('0') << std::setw(2) << (short)packet_buffer[j] << std::dec <<  std::endl;
                }
                
                throw AmptekException("Failed receiving the response via UDP");
            }
            memcpy(packet_buffer + offset,udp_buffer, n);
            offset += n;
            // std::cout << "read " << n << " bytes in step" << std::endl;
            // std::cout << "offset at " << offset  << std::endl;
        }
        
        try{
            p.resize( packet_size);
            p.fromByteArray(packet_buffer);
        }
        catch(...){
            std::cerr << p.toString() << std::endl;
            sem_post(&mu);
            throw;
        }
        
    }
    sem_post(&mu);
    return p;

}



void AmptekUdpConnectionHandler::ClearCommunicationBuffer(){
    sem_wait(&mu);
    int n = 1;
    byte udp_buffer[MAX_UDP_PACKET_SIZE];
    while(n >0){
        n = recvfrom(sockfd, &udp_buffer,  MAX_UDP_PACKET_SIZE, 0, (struct sockaddr *)&from, &addrlength);

    }
    sem_post(&mu);
}
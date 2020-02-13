#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <string>
#include <sstream>
#include "types.h"

#define MIN_PACKET_LEN 8

using namespace std;

enum Offset 
{
    SYNC1,
    SYNC2,
    PID1,
    PID2,
    LEN_MSB,
    LEN_LSB,
    DATA
};

enum PID1_TYPE{
    PX5_RESPONSE_STATUS = 0x80,
    PX5_RESPONSE_SPECTRUM = 0x81,
    PX5_RESPONSE_CONFIG = 0x82,
    PX5_RESPONSE_COMMTEST = 0x8F,
    PX5_ACK = 0xFF,
    PX5_REQUEST_CONFIG =0x20
};
enum STATUS_PACKET_TYPES{
    PX5_RESPONSE_STATUS_INFO = 0x01,
};
enum SPECTRUM_PACKET_TYPES{
    
    PX5_RESPONSE_SPECTRUM256         = 0x01,
    PX5_RESPONSE_SPECTRUM256_STATUS  = 0x02,
    PX5_RESPONSE_SPECTRUM512         = 0x03,
    PX5_RESPONSE_SPECTRUM512_STATUS  = 0x04,
    PX5_RESPONSE_SPECTRUM1024        = 0x05,
    PX5_RESPONSE_SPECTRUM1024_STATUS = 0x06,
    PX5_RESPONSE_SPECTRUM2048        = 0x07,
    PX5_RESPONSE_SPECTRUM2048_STATUS = 0x08,
    PX5_RESPONSE_SPECTRUM4096        = 0x09,
    PX5_RESPONSE_SPECTRUM4096_STATUS = 0x0A,
    PX5_RESPONSE_SPECTRUM8192        = 0x0B,
    PX5_RESPONSE_SPECTRUM8192_STATUS = 0x0C,
};
enum CONFIG_PACKET_TYPES{
    PX5_RESPONSE_CONFIG_TEXT = 0x07,
    PX5_REQUEST_SET_CONFIG = 0x02,
    PX5_REQUEST_GET_CONFIG = 0x03,
};
enum COMMTEST_PACKET_TYPES{
    PX5_RESPONSE_COMMETEST_ECHO = 0x7F,

};
enum ACK_PACKET_TYPES{
    PX5_ACK_OK = 0x00,
    PX5_ACK_SYNC_ERR = 0x01,
    PX5_ACK_PID_ERR = 0x02,
    PX5_ACK_LEN_ERR = 0x03,
    PX5_ACK_CHECKSUM_ERR = 0x04,
    PX5_ACK_BADPARAM_ERR = 0x05,
    PX5_ACK_BADHEX_ERR = 0x06,
    PX5_ACK_BADCMD_ERR = 0x07,
    PX5_ACK_FPGA_ERR = 0x08,
    PX5_ACK_CP2201_ERR = 0x09,
    PX5_ACK_SCOPE_ERR = 0x0A,
    PX5_ACK_PC5_ERR = 0x0B,
    PX5_ACK_OK_SHARING = 0x0C,
    PX5_ACK_BUSY_ERR = 0x0D,
    PX5_ACK_I2C_ERR = 0x0E,
    PX5_ACK_OK_FPGAADDR = 0x0F,
    PX5_ACK_VERSION_ERR = 0x10,
    PX5_ACK_CALIB_ERR = 0x11
};

class AmptekException : public std::runtime_error
{
public:
    AmptekException(std::string what) : runtime_error(what){}
    ~AmptekException(){}
};


class Packet: public vector<byte>
{
    friend class vector;

public:

    Packet();
    Packet(unsigned short initSize);
    Packet(byte pid1, byte pid2, byte* data, word16 datasize);
    word16 dataLength;
    void initSync();
    void setPid1(byte p1);
    void setPid2(byte p2);
    word16 calcLen() const;
    word16 calcChecksum() const;
    void calcAndFillChecksum();
    void validateChecksum();
    void setData(byte* data, word16 size);
    string toString() const;
    void fromByteArray(byte* response);

    bool isType( word16 );
    bool isType( std::vector<word16> );
    bool isType( byte, byte );



public:
    static const Packet PX5_REQUEST_SPECTRUM;
    static const Packet PX5_REQUEST_SPECTRUM_AND_STATUS;
    static const Packet PX5_REQUEST_STATUS;
    static const Packet PX5_REQUEST_ENABLE;
    static const Packet PX5_REQUEST_DISABLE;
    static const Packet PX5_REQUEST_CLEAR_SPECTRUM;
    static const Packet PX5_REQUEST_SET_CONFIG;
    static const Packet PX5_REQUEST_GET_CONFIG;
    static const Packet PX5_REQUEST_KEEP_ALIVE_NO_SHARING;

    static const Packet gernerateSetConfigurationRequest(std::string text_configuration);
    static const Packet gernerateGetConfigurationRequest(std::string text_configuration);

};

#endif

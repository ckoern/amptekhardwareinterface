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

// Packet impementation and hex values are based on the DP5 Programmers Guide, Version B1

// Packet ID first byte (packet group)
enum PID1_TYPE{
    DP5_P1_STATUS_REQUEST       = 0x01,
    DP5_P1_SPECTRUM_REQUEST     = 0x02,
    DP5_P1_DATA_REQUEST         = 0x03,
    DP5_P1_SCA_REQUEST          = 0x04,
    DP5_P1_TEXTCONFIG_REQUEST   = 0x20,
    DP5_P1_COMMAND_REQUEST      = 0xF0,
    DP5_P1_COMMTEST_REQUEST     = 0xF1,

    DP5_P1_STATUS_RESPONSE      = 0x80,
    DP5_P1_SPECTRUM_RESPONSE    = 0x81,
    DP5_P1_DATA_RESPONSE        = 0x82,
    DP5_P1_COMMTEST_RESPONSE    = 0x8F,

    DP5_P1_ACK                  = 0xFF,
};



// Packet ID second byte (one enum per group)

enum PID2_SUBTYPE_STATUS_REQUEST{
    DP5_P2_STATUS_REQUEST_INFO = 0x01,
};

enum PID2_SUBTYPE_SPECTRUM_REQUEST{
    DP5_P2_SPECTRUM_REQUEST_SPECTRUM              = 0x01,
    DP5_P2_SPECTRUM_REQUEST_SPECTRUM_CLEAR        = 0x02,
    DP5_P2_SPECTRUM_REQUEST_SPECTRUM_STATUS       = 0x03,
    DP5_P2_SPECTRUM_REQUEST_SPECTRUM_STATUS_CLEAR = 0x04,
    DP5_P2_SPECTRUM_REQUEST_BUFFER                = 0x05,
    DP5_P2_SPECTRUM_REQUEST_BUFFER_CLEAR          = 0x06,
    DP5_P2_SPECTRUM_REQUEST_GET_BUFFER            = 0x07,
};
enum PID2_SUBTYPE_DATA_REQUEST{
    DP5_P2_DATA_REQUEST_LISTDATA = 0x09,
};
enum PID2_SUBTYPE_COMMAND_REQUEST{
    DP5_P2_COMMAND_REQUEST_CLEAR_SPECTRUM         = 0x01,
    DP5_P2_COMMAND_REQUEST_ENABLE                 = 0x02,
    DP5_P2_COMMAND_REQUEST_DISABLE                = 0x03,
    DP5_P2_COMMAND_REQUEST_CLEAR_GPCOUNTER        = 0x10,
    DP5_P2_COMMAND_REQUEST_CLEAR_TIMER            = 0x16,
    DP5_P2_COMMAND_REQUEST_RESTART_BUFFER         = 0x1E,
    DP5_P2_COMMAND_REQUEST_CANCEL_BUFFER          = 0x1F,
    DP5_P2_COMMAND_REQUEST_KEEP_ALIVE_NO_SHARE    = 0x21,
};
enum PID2_SUBTYPE_COMMTEST_REQUEST{
    DP5_P2_COMMTEST_REQUEST_ACK                   = 0xF1,
    DP5_P2_COMMTEST_REQUEST_STREAM                = 0x7E,
    DP5_P2_COMMTEST_REQUEST_ECHO                  = 0x7F,
};
enum PID2_SUBTYPE_STATUS_RESPONSE{
    DP5_P2_STATUS_RESPONSE_INFO = 0x01,
};
enum PID2_SUBTYPE_SPECTRUM_RESPONSE{
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM256          = 0x01,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM256_STATUS   = 0x02,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM512          = 0x03,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM512_STATUS   = 0x04,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM1024         = 0x05,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM1024_STATUS  = 0x06,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM2048         = 0x07,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM2048_STATUS  = 0x08,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM4096         = 0x09,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM4096_STATUS  = 0x0A,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM8192         = 0x0B,
    DP5_P2_SPECTRUM_RESPONSE_SPECTRUM8192_STATUS  = 0x0C,
};

enum PID2_SUBTYPE_TEXTCONFIG_REQUEST{
    DP5_P2_TEXTCONFIG_REQUEST_SET = 0x02,
    DP5_P2_TEXTCONFIG_REQUEST_GET = 0x03,
};

enum PID2_SUBTYPE_DATA_RESPONSE{
    DP5_P2_DATA_RESPONSE_CONFIG_READBACK = 0x07,
    DP5_P2_DATA_RESPONSE_LISTDATA        = 0x0A,
    DP5_P2_DATA_RESPONSE_LISTDATA_FULL   = 0x0B,
};

enum PID2_SUBTYPE_COMMTEST_RESPONSE{
    DP5_P2_COMMTEST_RESPONSE_ECHO = 0x7F,
};


enum PID2_SUBTYPE_ACK{
    DP5_P2_ACK_OK                = 0x00,
    DP5_P2_ACK_SYNC_ERR          = 0x01,
    DP5_P2_ACK_PID_ERR           = 0x02,
    DP5_P2_ACK_LEN_ERR           = 0x03,
    DP5_P2_ACK_CHECKSUM_ERR      = 0x04,
    DP5_P2_ACK_BADPARAM_ERR      = 0x05,
    DP5_P2_ACK_BADHEX_ERR        = 0x06,
    DP5_P2_ACK_BADCMD_ERR        = 0x07,
    DP5_P2_ACK_FPGA_ERR          = 0x08,
    DP5_P2_ACK_CP2201_ERR        = 0x09,
    DP5_P2_ACK_SCOPE_ERR         = 0x0A,
    DP5_P2_ACK_PC5_ERR           = 0x0B,
    DP5_P2_ACK_OK_SHARING        = 0x0C,
    DP5_P2_ACK_BUSY_ERR          = 0x0D,
    DP5_P2_ACK_I2C_ERR           = 0x0E,
    DP5_P2_ACK_OK_FPGAADDR       = 0x0F,
    DP5_P2_ACK_VERSION_ERR       = 0x10,
    DP5_P2_ACK_CALIB_ERR         = 0x11
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
    static const Packet DP5_PKT_REQUEST_SPECTRUM;
    static const Packet DP5_PKT_REQUEST_SPECTRUM_AND_STATUS;
    static const Packet DP5_PKT_REQUEST_STATUS;
    static const Packet DP5_PKT_REQUEST_ENABLE;
    static const Packet DP5_PKT_REQUEST_DISABLE;
    static const Packet DP5_PKT_REQUEST_CLEAR_SPECTRUM;
    static const Packet DP5_PKT_REQUEST_KEEP_ALIVE_NO_SHARING;
    static const Packet DP5_PKT_REQUEST_RESTART_SEQ_BUFFERING;
    static const Packet DP5_PKT_REQUEST_CANCEL_SEQ_BUFFERING;
    static const Packet DP5_PKT_REQUEST_LIST_DATA;
    static const Packet DP5_PKT_REQUEST_CLEAR_LIST_TIMER;
    static const Packet DP5_PKT_REQUEST_STOP_STREAM_COMMTEST;

    static const Packet gernerateSetConfigurationRequest(std::string text_configuration);
    static const Packet gernerateGetConfigurationRequest(std::string text_configuration);

    static const Packet generateBufferRequest(uint16_t buffer_index);
    static const Packet generateBufferAndClearRequest(uint16_t buffer_index);
    static const Packet generateGetBufferRequest(uint16_t buffer_index);

    static const Packet generateCommtestStreamingRequest(uint16_t min_channel,uint16_t max_channel, 
                                                          uint16_t increment, uint16_t period);

};

#endif

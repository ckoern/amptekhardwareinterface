#include "packet.h"
#include <iostream>
#include <iomanip>
#include <algorithm>


const Packet Packet::DP5_PKT_REQUEST_STATUS                = Packet( DP5_P1_STATUS_REQUEST      , DP5_P2_STATUS_REQUEST_INFO                , nullptr, 0);

const Packet Packet::DP5_PKT_REQUEST_SPECTRUM              = Packet( DP5_P1_SPECTRUM_REQUEST    , DP5_P2_SPECTRUM_REQUEST_SPECTRUM          , nullptr, 0);
const Packet Packet::DP5_PKT_REQUEST_SPECTRUM_AND_STATUS   = Packet( DP5_P1_SPECTRUM_REQUEST    , DP5_P2_SPECTRUM_REQUEST_SPECTRUM_STATUS   , nullptr, 0);

const Packet Packet::DP5_PKT_REQUEST_LIST_DATA             = Packet( DP5_P1_DATA_REQUEST        , DP5_P2_DATA_REQUEST_LISTDATA              , nullptr, 0);

const Packet Packet::DP5_PKT_REQUEST_CLEAR_SPECTRUM        = Packet( DP5_P1_COMMAND_REQUEST     , DP5_P2_COMMAND_REQUEST_CLEAR_SPECTRUM     , nullptr, 0);
const Packet Packet::DP5_PKT_REQUEST_ENABLE                = Packet( DP5_P1_COMMAND_REQUEST     , DP5_P2_COMMAND_REQUEST_ENABLE             , nullptr, 0);
const Packet Packet::DP5_PKT_REQUEST_DISABLE               = Packet( DP5_P1_COMMAND_REQUEST     , DP5_P2_COMMAND_REQUEST_DISABLE            , nullptr, 0);
const Packet Packet::DP5_PKT_REQUEST_KEEP_ALIVE_NO_SHARING = Packet( DP5_P1_COMMAND_REQUEST     , DP5_P2_COMMAND_REQUEST_KEEP_ALIVE_NO_SHARE, nullptr, 0);

const Packet Packet::DP5_PKT_REQUEST_RESTART_SEQ_BUFFERING = Packet( DP5_P1_COMMAND_REQUEST     , DP5_P2_COMMAND_REQUEST_RESTART_BUFFER     , nullptr, 0);
const Packet Packet::DP5_PKT_REQUEST_CANCEL_SEQ_BUFFERING  = Packet( DP5_P1_COMMAND_REQUEST     , DP5_P2_COMMAND_REQUEST_CANCEL_BUFFER      , nullptr, 0);
const Packet Packet::DP5_PKT_REQUEST_CLEAR_LIST_TIMER      = Packet( DP5_P1_COMMAND_REQUEST     , DP5_P2_COMMAND_REQUEST_CLEAR_TIMER        , nullptr, 0);


const Packet Packet::DP5_PKT_REQUEST_STOP_STREAM_COMMTEST  = Packet( DP5_P1_COMMTEST_REQUEST    , DP5_P2_COMMTEST_REQUEST_STREAM            , nullptr, 0 );

const Packet Packet::gernerateSetConfigurationRequest(std::string text_configuration){
    word16 len = text_configuration.size();
    byte data[len];
    charToByte(text_configuration.c_str(), data, len);
    return Packet( DP5_P1_TEXTCONFIG_REQUEST, DP5_P2_TEXTCONFIG_REQUEST_SET, data, len );
}
const Packet Packet::gernerateGetConfigurationRequest(std::string text_configuration){
    word16 len = text_configuration.size();
    byte data[len];
    charToByte(text_configuration.c_str(), data, len);
    return Packet( DP5_P1_TEXTCONFIG_REQUEST, DP5_P2_TEXTCONFIG_REQUEST_GET, data, len );
}

const Packet Packet::generateBufferRequest(uint16_t buffer_index){
    byte data[2];
    data[0] = buffer_index>>8;
    data[1] = buffer_index;
    return Packet( DP5_P1_SPECTRUM_REQUEST, DP5_P2_SPECTRUM_REQUEST_BUFFER, data, 2 );
}
const Packet Packet::generateBufferAndClearRequest(uint16_t buffer_index){
    byte data[2];
    data[0] = buffer_index>>8;
    data[1] = buffer_index;
    return Packet( DP5_P1_SPECTRUM_REQUEST, DP5_P2_SPECTRUM_REQUEST_BUFFER_CLEAR, data, 2 );
}
const Packet Packet::generateGetBufferRequest(uint16_t buffer_index){
    byte data[2];
    data[0] = buffer_index>>8;
    data[1] = buffer_index;
    return Packet( DP5_P1_SPECTRUM_REQUEST, DP5_P2_SPECTRUM_REQUEST_GET_BUFFER, data, 2 );
}
const Packet Packet::generateCommtestStreamingRequest(uint16_t min_channel,uint16_t max_channel, 
                                                          uint16_t increment, uint16_t period)
{
    byte data[8];
    data[0] = min_channel >> 8;
    data[1] = min_channel & 0xFF;
    data[2] = max_channel >> 8;
    data[3] = max_channel & 0xFF;
    data[4] = increment >> 8;
    data[5] = increment & 0xFF;
    data[6] = period >> 8;
    data[7] = period & 0xFF;
    return Packet( DP5_P1_COMMTEST_REQUEST    , DP5_P2_COMMTEST_REQUEST_STREAM , data, 8 );

}


Packet::Packet():vector<byte>(), dataLength(0)
{ }

Packet::Packet(unsigned short initSize):vector<byte>(initSize,0), dataLength(0)
{ }

Packet::Packet(byte pid1, byte pid2, byte* data, word16 datasize):vector<byte>(MIN_PACKET_LEN,0){
    initSync();
    setPid1(pid1);
    setPid2(pid2);
    setData(data, datasize);
    calcAndFillChecksum();
}

void Packet::initSync()
{
    at(SYNC1) = 0XF5;
    at(SYNC2) = 0XFA;
}

void Packet::setPid1(byte p1)
{
    at(PID1) = p1;
}

void Packet::setPid2(byte p2)
{
    at(PID2) = p2;
}

word16 Packet::calcLen() const
{
    word16 len = size() - 8;
    return len;
}

word16 Packet::calcChecksum() const
{
    word16 sum = 0;
    for (int i = 0; i < dataLength + DATA; ++i)
    {
//         printf("Packet::calcChecksum: calculating sum, adding data = %x\n", *it);
        sum += at(i);
    }

//     printf("Packet::calcChecksum(): sum = %x\n", sum);
    word16 checksum = twosComplement(sum);
//     printf("Packet::calcChecksum(): checksum: %x\n", checksum);
    return checksum;
}

void Packet::calcAndFillChecksum()
{
//     printf("dataLength: %d\n", dataLength);
    word16 checksum = calcChecksum();
//     printf("Step 1\n");
    at(DATA+dataLength)   = msbyte(checksum);
//     printf("Step 2\n");
    at(DATA+dataLength+1) = lsbyte(checksum);
//     printf("Step 3\n");
}

void Packet::validateChecksum()
{
    word16 calculatedChecksum = calcChecksum();
    word16 currentChecksum = mergeBytes(at(size()-2),at(size()-1));
    if (calculatedChecksum != currentChecksum)
    {
        AmptekException e = AmptekException("Checksum mismatch. Package discarted.");
        throw e;
    }
}

void Packet::setData(byte* data, word16 len)
{
    dataLength = len;
    if (len > 0){
        insert(begin()+DATA, data, data+len);
        at(LEN_MSB) = msbyte(len);
        at(LEN_LSB) = lsbyte(len);
    }
}

void Packet::fromByteArray(byte* response)
{
    dataLength  = mergeBytes(response[LEN_MSB], response[LEN_LSB]);
    resize( dataLength+ MIN_PACKET_LEN);
    at(SYNC1)   = response[SYNC1];
    at(SYNC2)   = response[SYNC2];
    at(PID1)    = response[PID1];
    at(PID2)    = response[PID2];
    at(LEN_MSB) = response[LEN_MSB];
    at(LEN_LSB) = response[LEN_LSB];
   
    std::copy( response+DATA, response+DATA+dataLength, begin()+DATA );
    //insert(begin()+DATA, response+DATA, response+DATA+dataLength);
    at(DATA+dataLength) = response[DATA+dataLength];
    at(DATA+dataLength+1) = response[DATA+dataLength+1];
    validateChecksum();
}

bool Packet::isType( word16 allowed_type){
    return mergeBytes(at(PID1),at(PID2)) == allowed_type;
}
bool Packet::isType( std::vector<word16> allowed_types){
    word16 this_type = mergeBytes(at(PID1),at(PID2));
    for (auto allowed_type : allowed_types){
        if(this_type == allowed_type){
            return true;
        }
    }
    return false;

}
bool Packet::isType( byte pid1, byte  pid2){
    return isType( mergeBytes(pid1,pid2) );
}

string Packet::toString() const
{
    stringstream ss;
    ss << "SYNC1 = " << hex << (short)at(SYNC1) << dec;
    ss << " SYNC2 = " << (short)at(SYNC2) << endl;
    ss << "PID1 = " << (short)at(PID1);
    ss << " PID2 = " << (short)at(PID2) << endl;
    ss << "LEN_MSB = " << (short)at(LEN_MSB);
    ss << " LEN_LSB = " << (short)at(LEN_LSB) << endl;
    ss << "CHECKSUM_MSB = " << (short)at(size()-2);
    ss << " CHECKSUM_LSB = " << (short)at(size()-1) << endl;
    ss << " DATA LENGTH = " << dataLength << endl;
    for (int i = 0; i < dataLength; i++)
        ss << "DATA["  << i << hex << "] = " << (short)at(DATA+i)<< dec << endl;
    
    return ss.str();
}



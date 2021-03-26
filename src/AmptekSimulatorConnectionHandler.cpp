

#include "AmptekSimulatorConnectionHandler.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <iomanip>
Packet AmptekSimulatorConnectionHandler::sendAndReceive( const Packet& request){

    byte pid1 = request.at(PID1);
    byte pid2 = request.at(PID2);

    Packet p(MIN_PACKET_LEN);
    p.initSync();
    if (pid1 == Packet::DP5_PKT_REQUEST_SPECTRUM.at(PID1) 
     && pid2 == Packet::DP5_PKT_REQUEST_SPECTRUM.at(PID2))
    {
        p.setPid1( DP5_P1_SPECTRUM_RESPONSE );
        switch (speclen){
            case 256:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM256 );
                break;
            case 512:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM512 );
                break;
            case 1024:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM1024 );
                break;
            case 2048:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM2048 );
                break;
            case 4096:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM4096 );
                break;
            case 8192:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM8192 );
                break;
        }
        
        word16 len = 3*speclen;
        byte arr[len];
        createSpectrumData(arr);
        p.resize( MIN_PACKET_LEN + len );
        p.setData(arr, len);
    }
    else if (pid1 == Packet::DP5_PKT_REQUEST_SPECTRUM_AND_STATUS.at(PID1) 
     && (pid2 == DP5_P2_SPECTRUM_REQUEST_SPECTRUM_STATUS || pid2 ==  DP5_P2_SPECTRUM_REQUEST_GET_BUFFER) )
    {
        p.setPid1( DP5_P1_SPECTRUM_RESPONSE );
        switch (speclen){
            case 256:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM256_STATUS );
                break;
            case 512:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM512_STATUS );
                break;
            case 1024:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM1024_STATUS );
                break;
            case 2048:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM2048_STATUS );
                break;
            case 4096:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM4096_STATUS );
                break;
            case 8192:
                p.setPid2( DP5_P2_SPECTRUM_RESPONSE_SPECTRUM8192_STATUS );
                break;
        }
        
        word16 len = 3*speclen + STATUS_SIZE;
        byte arr[len];
        createSpectrumData(arr);
        createStatusData(arr + 3*speclen);
        p.resize( MIN_PACKET_LEN + len );
        p.setData(arr, len);
    }
    else if (pid1 == Packet::DP5_PKT_REQUEST_STATUS.at(PID1) 
     && pid2 == Packet::DP5_PKT_REQUEST_STATUS.at(PID2))
    {
        p.setPid1( DP5_P1_STATUS_RESPONSE );
        p.setPid2( DP5_P2_STATUS_RESPONSE_INFO );
        word16 len =  STATUS_SIZE;
        byte arr[len];
        createStatusData(arr);
        p.resize( MIN_PACKET_LEN + len );
        p.setData(arr, len);
    }
    else if (pid1 == DP5_P1_TEXTCONFIG_REQUEST
     && pid2 == DP5_P2_TEXTCONFIG_REQUEST_GET)
    {
        p.setPid1( DP5_P1_DATA_RESPONSE );
        p.setPid2( DP5_P2_DATA_RESPONSE_CONFIG_READBACK );

        char configs[ request.dataLength + 1 ];
        byteToChar( (byte*)&(request.at(DATA)), configs, request.dataLength );
        string configstring = buildConfigResponse(configs);
        word16 len =  configstring.size();
        byte arr[len];
        charToByte(configstring.c_str(), arr, len);
        p.resize( MIN_PACKET_LEN + len );
        p.setData(arr, len);
    }
    else if( (pid1 == Packet::DP5_PKT_REQUEST_ENABLE.at(PID1) 
              && pid2 == Packet::DP5_PKT_REQUEST_ENABLE.at(PID2)) )
    {
        enable();
        return Packet(DP5_P1_ACK, DP5_P2_ACK_OK, nullptr, 0);
    }
    else if ( (pid1 == Packet::DP5_PKT_REQUEST_DISABLE.at(PID1) 
              && pid2 == Packet::DP5_PKT_REQUEST_DISABLE.at(PID2)) )
    {
        disable();
        return Packet(DP5_P1_ACK, DP5_P2_ACK_OK, nullptr, 0);
    }
    else if (pid1 == Packet::DP5_PKT_REQUEST_CLEAR_SPECTRUM.at(PID1) 
              && pid2 == Packet::DP5_PKT_REQUEST_CLEAR_SPECTRUM.at(PID2))
    {
        clear();
        return Packet(DP5_P1_ACK, DP5_P2_ACK_OK, nullptr, 0);
    }
    else if (pid1 == Packet::DP5_PKT_REQUEST_KEEP_ALIVE_NO_SHARING.at(PID1) 
              && pid2 == Packet::DP5_PKT_REQUEST_KEEP_ALIVE_NO_SHARING.at(PID2))
    {
        return Packet(DP5_P1_ACK, DP5_P2_ACK_OK, nullptr, 0);
    }
    else if(pid1 == DP5_P1_TEXTCONFIG_REQUEST
              && pid2 == DP5_P2_TEXTCONFIG_REQUEST_SET)
    {
        char configs[ request.dataLength + 1 ];
        byteToChar( (byte*)&(request.at(DATA)), configs, request.dataLength );
        readConfig(configs);
        return Packet(DP5_P1_ACK, DP5_P2_ACK_OK, nullptr, 0);
    }
    else{
        return Packet(DP5_P1_ACK, DP5_P2_ACK_OK, nullptr, 0);
    }
    p.calcAndFillChecksum();
    return p;

}

AmptekSimulatorConnectionHandler::AmptekSimulatorConnectionHandler(){
    spectrum = new unsigned int[speclen];
    clear();
    initConfigs();
    spectrum_thread = new std::thread( 
        [&]{
            auto start_time = std::chrono::system_clock::now();
            while( is_running ){
                if (flag_enable){
                    is_enabled = true;
                    start_time = std::chrono::system_clock::now();
                    flag_enable = false;
                }
                if (flag_disable){
                    is_enabled = false;
                    flag_disable = false;
                }
                if (is_enabled){
                    spectrum[ rand() % speclen ]++;
                    total_counts += 1;
                    auto now = std::chrono::system_clock::now();
                    acc_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time).count() / 1000.;
                    if (acquisition_time > 0){
                        
                        if (acc_time >= acquisition_time){
                            is_enabled = false;
                        }
                    }
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(2));

                
            }
        }
     );
     
}

AmptekSimulatorConnectionHandler::~AmptekSimulatorConnectionHandler(){
    is_running = false;
    spectrum_thread->join();
    delete[] spectrum;
}

void AmptekSimulatorConnectionHandler::enable(){
    flag_enable = true;
}

void AmptekSimulatorConnectionHandler::disable(){
    flag_disable = true;
}
void AmptekSimulatorConnectionHandler::clear(){
    total_counts = 0;
    acc_time = 0;
    for (int i = 0; i < speclen; ++i){
        spectrum[i] = 0;
    }
}

void AmptekSimulatorConnectionHandler::createSpectrumData(byte* buffer){
    for (int i = 0; i < speclen; ++i){
        buffer[3*i]     = spectrum[i]       & 0xff;
        buffer[3*i + 1] = spectrum[i] >> 8  & 0xff;
        buffer[3*i + 2] = spectrum[i] >> 16 & 0xff;
    }
}

void AmptekSimulatorConnectionHandler::createStatusData(byte* buffer){
    for (int i = 0; i < STATUS_SIZE; ++i){
        buffer[i] = 0;
    }

    //set fast count
    buffer[0] = total_counts       & 0xff;  //lsb
    buffer[1] = total_counts >> 8  & 0xff;
    buffer[2] = total_counts >> 16 & 0xff;
    buffer[3] = total_counts >> 24 & 0xff;  //msb

    //set slow count
    buffer[4] = total_counts       & 0xff;  //lsb
    buffer[5] = total_counts >> 8  & 0xff;
    buffer[6] = total_counts >> 16 & 0xff;
    buffer[7] = total_counts >> 24 & 0xff;  //msb

    //acc time
    int nticks_0p1 = 10*acc_time;
    int nticks_0p001 = 1000*( acc_time - nticks_0p1*0.1 );
    buffer[12] = nticks_0p001;
    buffer[13] = nticks_0p1       &  0xff;
    buffer[14] = nticks_0p1 >> 8  &  0xff;
    buffer[15] = nticks_0p1 >> 16 &  0xff;

    //real time
    nticks_0p001 = 1000* acc_time;
    buffer[20] = nticks_0p1       &  0xff;
    buffer[21] = nticks_0p1 >> 8  &  0xff;
    buffer[22] = nticks_0p1 >> 16 &  0xff;
    buffer[23] = nticks_0p1 >> 24 &  0xff;

    // is pret reached
    if (acquisition_time > 0 && (acc_time - acquisition_time) < 0.0001 && !is_enabled){
        buffer[35] |=  (1 << 7);
    }
    if (is_enabled){
        buffer[35] |=  (1 << 5);
    }
}

void AmptekSimulatorConnectionHandler::readConfig(char* configs){
    std::string config_name, config_value, configline;
    std::stringstream configstream(configs);
    while(std::getline(configstream, configline, ';')){
        try{
            std::stringstream linestream(configline);
            std::getline(linestream, config_name,'=');
            linestream >> config_value;
            if (config_name == "MCAC"){
                speclen = std::stoi(config_value);
                delete[] spectrum;
                spectrum = new unsigned int[speclen];
                for(int i = 0; i < speclen; ++i){
                    spectrum[i] = 0;
                }
            }else if (config_name == "PRET"){
                if (config_value == "OFF"){
                    acquisition_time = -1;    
                }else{
                    acquisition_time = std::stod( config_value );
                }
            }
            text_configs[config_name] = config_value;

        }catch(...){
            std::cerr << "Failed reading config " << configline << std::endl;
        }
    }
}

std::string  AmptekSimulatorConnectionHandler::buildConfigResponse(char* config_names){
    std::string config_name;
    std::stringstream configname_stream(config_names);
    std::stringstream configresponse_stream;
    while(std::getline(configname_stream, config_name, ';')){
        try{
            configresponse_stream << config_name << "=" << text_configs[config_name] << ";";
        }
        catch(...){
            std::cerr << "Failed reading config " << config_name << std::endl;
        }
    }
    return configresponse_stream.str();
}



void AmptekSimulatorConnectionHandler::initConfigs(){
    text_configs["AINP"] = "NEG";
    text_configs["AU34"] = "1";
    text_configs["AUO1"] = "ICR";
    text_configs["AUO2"] = "ICR";
    text_configs["BLRD"] = "0";
    text_configs["BLRM"] = "OFF";
    text_configs["BLRU"] = "0";
    text_configs["CON1"] = "DAC";
    text_configs["CON2"] = "AUXOUT2";
    text_configs["CLCK"] = "AUTO";
    text_configs["CLKL"] = "100";
    text_configs["CUSP"] = "0";
    text_configs["DACF"] = "0";
    text_configs["DACO"] = "OFF";
    text_configs["GAIA"] = "0";
    text_configs["GAIF"] = "1";
    text_configs["GAIN"] = "1";
    text_configs["GATE"] = "OFF";
    text_configs["GPED"] = "FALLING";
    text_configs["GPGA"] = "ON";
    text_configs["GPIN"] = "AUX1";
    text_configs["GPMC"] = "ON";
    text_configs["GPME"] = "ON";
    text_configs["HVSE"] = "OFF";
    text_configs["INOF"] = "DEF";
    text_configs["INOG"] = "LOW";
    text_configs["MCAC"] = std::to_string(speclen);
    text_configs["MCAE"] = "OFF";
    text_configs["MCAS"] = "NORM";
    text_configs["MCSL"] = "0";
    text_configs["MCSH"] = "8191";
    text_configs["MCST"] = "1";
    text_configs["PAPS"] = "OFF";
    text_configs["PAPZ"] = "OFF";
    text_configs["PDMD"] = "NORM";
    text_configs["PRCL"] = "0";
    text_configs["PRCH"] = "8191";
    text_configs["PREC"] = "OFF";
    text_configs["PREL"] = "OFF";
    text_configs["PRER"] = "OFF";
    text_configs["PRET"] = "OFF";
    text_configs["PURE"] = "OFF";
    text_configs["RESL"] = "OFF";
    text_configs["RTDD"] = "0";
    text_configs["RTDE"] = "OFF";
    text_configs["RTDS"] = "0";
    text_configs["RTDT"] = "0";
    text_configs["RTDW"] = "0";
    text_configs["SCAH"] = "0";
    text_configs["SCAI"] = "1";
    text_configs["SCAL"] = "0";
    text_configs["SCAO"] = "OFF";
    text_configs["SCAW"] = "100";
    text_configs["SCOE"] = "RISING";
    text_configs["SCOG"] = "1";
    text_configs["SCOT"] = "87";
    text_configs["SOFF"] = "OFF";
    text_configs["SYNC"] = "INT";
    text_configs["TECS"] = "OFF";
    text_configs["TFLA"] = "0";
    text_configs["THFA"] = "0";
    text_configs["THSL"] = "0";
    text_configs["TLLD"] = "OFF";
    text_configs["TPEA"] = "0.4";
    text_configs["TPFA"] = "400";
    text_configs["TPMO"] = "OFF";
    text_configs["VOLU"] = "OFF";
}

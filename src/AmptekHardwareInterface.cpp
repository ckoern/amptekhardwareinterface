#include "AmptekHardwareInterface.h"
#include "packet.h"
#include <iostream>
#include <iomanip>
#include <cstring>
#include "AmptekUdpConnectionHandler.h"
#include "AmptekUsbConnectionHandler.h"
#include "AmptekSimulatorConnectionHandler.h"

AmptekHardwareInterface::AmptekHardwareInterface(){

}

AmptekHardwareInterface::~AmptekHardwareInterface(){
    std::cout << "in AmptekHardwareInterface::~AmptekHardwareInterface()" << std::endl;
    if (connection_handler != nullptr){
        std::cout << "delete connection_handler" << std::endl;
        delete connection_handler;
    }
}


/**
 * @brief Connect via UDP to the detector
 * 
 * @param hostname host of the detector, can be IP address
 * @param port port on which the amptek is listening. Should be 10001
 * @param timeout connection timeout in seconds
 */
void AmptekHardwareInterface::connectUDP(std::string hostname, int port, double timeout){

    if (connection_handler != nullptr){
        throw AmptekException("Amptek is already connected");
    }
    else{
        connection_handler = new AmptekUdpConnectionHandler(hostname, port, timeout);
        current_state = ON;
    }
}

/**
 * @brief Connect via USB to the detector
 * 
 * @param serialNb the serial number of the detector to connect with
 */
void AmptekHardwareInterface::connectUSB(int serialNb){
if (connection_handler != nullptr){
        throw AmptekException("Amptek is already connected");
    }
    else{
        connection_handler = new AmptekUsbConnectionHandler(serialNb);
        current_state = ON;
    }
}

/**
 * @brief Connect to a simulator interface
 * This allows debugging of interface logic without an available hardware to connect to
 * 
 */
void AmptekHardwareInterface::connectSimulator(){

    if (connection_handler != nullptr){
        throw AmptekException("Amptek is already connected");
    }
    else{
        connection_handler = new AmptekSimulatorConnectionHandler();
        current_state = ON;
    }
}


/**
 * @brief Checks if the stored last spectrum is younger than the given maximum age
 * 
 * @param max_age_ms maximum allowed age in milliseconds
 * @return true if spectrum is young enough
 * @return false if it is too old
 */
bool AmptekHardwareInterface::spectrumAgeOk( double max_age_ms ) const {
    if (max_age_ms < 0){
        return false;
    }
    return last_spectrum.AgeMillis() < max_age_ms;
}


/**
 * @brief  Checks if the stored last status is younger than the given maximum age
 * 
 * @param max_age_ms maximum allowed age in milliseconds
 * @return true true if status is young enough
 * @return false false if it is too old
 */
bool AmptekHardwareInterface::statusAgeOk( double max_age_ms ) const {
    if (max_age_ms < 0){
        return false;
    }
    return last_status.AgeMillis() < max_age_ms;
}


/**
 * @brief Enables the acquisition on the detector
 * 
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::Enable(){
    try{
        expectAcknowledge( connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_ENABLE ) );
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}


/**
 * @brief Disable the acquisition on the detector
 * 
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::Disable(){
    try{
        expectAcknowledge( connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_DISABLE ) );
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}

/**
 * @brief Send a test packet to the detector
 * 
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::Ping(){
    try{
        expectAcknowledge( connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_KEEP_ALIVE_NO_SHARING ) );
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}

/**
 * @brief Clear the current spectrum buffer on the detector
 * 
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::ClearSpectrum(){
    try{
        expectAcknowledge( connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_CLEAR_SPECTRUM ) );
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}


/**
 * @brief Set the accumulation time for a measurement. The detector will stop automatically when the duration is reached
 * 
 * @note The Accumulation timer is stopped while the detector is gated, in transfer mode, reset lockout
 * or any other mode that temporarily blockes the MCA. Therefore, accumulation time is larger than the real measurement duration
 * 
 * @see SetPresetAccumulationTime, SetPresetRealTime, SetPresetCounts
 * @param t accumulation time in seconds. Minimum is 0.1. If below zero, the accumulation time is not part of the stop condition during acquisition
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::SetPresetAccumulationTime(double t){
    std::stringstream cmd_stream;
    cmd_stream << "PRET=";
    if (t>0){
        cmd_stream <<  std::fixed << std::setprecision(1) << t;
    }
    else{
        cmd_stream << "OFF";
    }
    return SetTextConfiguration( {cmd_stream.str()} );
}


/**
 * @brief Set the real time for a measurement. The detector will stop automatically when the duration is reached
 * 
 * @see SetPresetAccumulationTime, SetPresetRealTime, SetPresetCounts
 * @param t real time in seconds. If below zero, the real time is not part of the stop condition during acquisition
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::SetPresetRealTime(double t){
    std::stringstream cmd_stream;
    cmd_stream << "PRER=";
    if (t>0){
        cmd_stream <<  std::fixed << std::setprecision(1) << t;
    }
    else{
        cmd_stream << "OFF";
    }    
    return SetTextConfiguration( {cmd_stream.str()} );
}


/**
 * @brief Set the count limit for a measurement. The detector will stop automatically when the slow count is reached
 * 
 * @see SetPresetAccumulationTime, SetPresetRealTime, SetPresetCounts
 * @param c maximum slow counts. If below zero, the slow count is not part of the stop condition during acquisition
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::SetPresetCounts(int c){
    std::stringstream cmd_stream;
    cmd_stream << "PREC=";
    if (c>0){
        cmd_stream << c;
    }
    else{
        cmd_stream << "OFF";
    }
    return SetTextConfiguration( {cmd_stream.str()} );
}

/**
 * @brief Send text configuration parameters to the detector. See DP5 Programmers Guide for available commands
 * 
 * @param commands vector of strings in the format "CMD=VAL", each entry in the vector being one config
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::SetTextConfiguration(std::vector<std::string> commands){
    // std::cout << "Configuration is\n";
    // for (auto cmd :commands){
    //     std::cout << "\t" << cmd << "\n";
    // }
    // std::cout << std::endl;
    try{

        stringstream cmdstream;
        string cmd;
        int streamsize = 0;
        
        for (int i = 0; i < commands.size(); ++i){
                cmd = commands[i];

            // if max packet size (512) would be exceeded when adding this command, send the previous commands and clear the input stringstream
            if(  streamsize + cmd.size() > 511){
                //std::cout << "Send " << cmdstream.str() << std::endl;
                expectAcknowledge( connection_handler->sendAndReceive( Packet::gernerateSetConfigurationRequest( cmdstream.str() ) ) );
                cmdstream = stringstream();
            }
            
            //append the current command to the stringstream and add seperator
            cmdstream << cmd << ";";
            streamsize += cmd.size() + 1;

            // if this is the last command in the loop, send the stringstream even if max size is not reached
            if(  i == commands.size()-1 ){
                //std::cout << "Send " << cmdstream.str() << std::endl;
                expectAcknowledge( connection_handler->sendAndReceive( Packet::gernerateSetConfigurationRequest( cmdstream.str() ) ) );
            }
        }
    }
    catch( AmptekException& e){
        std::cerr << "Failed sending Text Config: " << e.what() <<  "\n";
        std::cerr << "Configuration was\n";
        for (auto cmd :commands){
            std::cerr << "\t" << cmd << "\n";
        }
        std::cerr << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}


/**
 * @brief Configuration read back from the detector
 * 
 * @param commands  each element being one configuration parameter CMD
 * @return std::vector<std::string> vector of strings in the format "CMD=VAL"
 */
std::vector<std::string> AmptekHardwareInterface::GetTextConfiguration(std::vector<std::string> commands){
    
        stringstream cmdstream;
        string cmd;
        int streamsize = 0;
        char configbuffer[MAX_UDP_PACKET_SIZE];

        std::vector<std::string> configs;
        std::string config_segment;

        int max_commands_per_request = 32; //command config return strings are below 16 chars (except few), meaning that 32 command will always be below 512 byte
        for (int i = 0; i < commands.size(); ++i){
            cmd = commands[i];
            cmdstream << cmd << ";";
            if (  ( i > 0 && i%max_commands_per_request ==0) //if max is reached
                 || i == commands.size() - 1)                // or last config
            {
                Packet settingResponse = connection_handler->sendAndReceive( Packet::gernerateGetConfigurationRequest( cmdstream.str() ) );
                
                if (settingResponse.at(PID1) != DP5_P1_DATA_RESPONSE
                || settingResponse.at(PID2) != DP5_P2_DATA_RESPONSE_CONFIG_READBACK)
                {
                    throw AmptekException("Failed in AmptekHardwareInterface::GetTextConfiguration: \
                                                Response Packet did not match expected PIDs. \n\
                                                Raw Packet Data:\n" + settingResponse.toString() );
                }
                
                memcpy(configbuffer, &(settingResponse.at(DATA)), settingResponse.dataLength);
                configbuffer[ settingResponse.dataLength ] = NULL;
                std::stringstream configstream(configbuffer);
                while(std::getline(configstream, config_segment, ';')){
                    configs.push_back(config_segment);
                }
            }
        }
        return configs;
}


/**
 * @brief Update the detector status if too old
 * 
 * @param max_age_ms maximum age of status in milliseconds. If too old, the status will be read from the detector
 * @return AmptekStatus& reference to the current status object
 */
AmptekStatus& AmptekHardwareInterface::updateStatus(double max_age_ms){
    if ( !statusAgeOk(max_age_ms)){
        Packet statusResponse = connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_STATUS );
        if (statusResponse.at(PID1) != DP5_P1_STATUS_RESPONSE
          || statusResponse.at(PID2) != DP5_P2_STATUS_RESPONSE_INFO)
        {
              throw AmptekException("Response Packet did not match expected PIDs: " + statusResponse.toString() );
              return last_status;
        }
        last_status = AmptekStatus(&(statusResponse.at(DATA)));
    }
    return last_status;
}


/**
 * @brief Update the spectrum data if too old
 * 
 * @param max_age_ms maximum age of spectrum in milliseconds. If too old, the spectrum will be read from the detector
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::updateSpectrum(double max_age_ms){
    if ( !spectrumAgeOk(max_age_ms)){
        Packet spectrumResponse = connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_SPECTRUM_AND_STATUS );
        //std::cout << spectrumResponse.size() << std::endl;
        if (spectrumResponse.at(PID1) != DP5_P1_SPECTRUM_RESPONSE )
        {
              std::cerr<< "Response Packet is not of type Spectrum: " << spectrumResponse.toString() << std::endl;
              return false;
        }
        bool with_status = true;
        int spectrum_length;
        switch( spectrumResponse.at(PID2) ){
            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM256:
                with_status = false;        // not break! use the respose with status setting the length
            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM256_STATUS:
                spectrum_length=256;
                break;

            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM512:
                with_status = false;        // not break! use the respose with status setting the length
            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM512_STATUS:
                spectrum_length=512;
                break;

            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM1024:
                with_status = false;        // not break! use the respose with status setting the length
            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM1024_STATUS:
                spectrum_length=1024;
                break;

            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM2048:
                with_status = false;        // not break! use the respose with status setting the length
            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM2048_STATUS:
                spectrum_length=2048;
                break;

            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM4096:
                with_status = false;        // not break! use the respose with status setting the length
            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM4096_STATUS:
                spectrum_length=4096;
                break;

            
            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM8192:
                with_status = false;        // not break! use the respose with status setting the length
            case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM8192_STATUS:
                spectrum_length=8192;
                break;
            default:
                throw AmptekException("Unknown PID2. Cannot read Spectrum Response");
        }
        
        int spectrum_bytesize = 3*spectrum_length;
        try{
            last_spectrum = AmptekSpectrum( &(spectrumResponse.at(DATA) ), spectrum_length );

        }
        catch(std::runtime_error& e){
            throw AmptekException(std::string("Failed in AmptekHardwareInterface::updateSpectrum \
                                   during spectrum update: ") + e.what());
        }

        try{
            if (with_status){
                last_status = AmptekStatus(&(spectrumResponse.at(DATA + spectrum_bytesize)));
            }
        }
        catch(std::runtime_error& e){
            throw AmptekException(std::string("Failed in AmptekHardwareInterface::updateSpectrum \
                                   during status update: ") + e.what());
        }
        
    }
    return true;
}


/**
 * @brief Enables the list mode and streams the records into the targetfile 
 * 
 * The targetfile will be filled with the packet content of list data response messages
 * The sync and checksum bytes are stripped from the response, but PIDs and LEN bytes will be written as well.
 * This allows extending the output with other packet types later on if needed, 
 * as well as having a clean way to find FIFO overflow packets during analysis
 * 
 * This function will invoke a continous loop in a worker thread and will result in 
 * high load on the USB interface and possibly CPU
 * 
 * @param targetfile the path of the file to stream the responses
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::EnableListMode(std::string targetfile){

    streamfile.open( targetfile, ios::binary );
    //f.write( static_cast <char*> ( &(pts.count) ), sizeof( unsigned ) );
    listmode_flag = true;
    ResetListModeTimer();
    list_reader_thread = new std::thread([&](){
        while(listmode_flag){
            try{
                 Packet listResponse = connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_LIST_DATA );
                //std::cout << spectrumResponse.size() << std::endl;
                if (listResponse.at(PID1) != DP5_P1_DATA_RESPONSE )
                {
                    throw AmptekException("Response Packet is not of type DATA_RESPONSE: " + listResponse.toString());
                }
                if( listResponse.at(PID2) != DP5_P2_DATA_RESPONSE_LISTDATA && listResponse.at(PID2) != DP5_P2_DATA_RESPONSE_LISTDATA_FULL ){
                    throw AmptekException("Response Packet is not of subtype LISTDATA: " + listResponse.toString());
                }
                //write the packet without the sync and checksum to file : [PID1,PID2,LEN_MSB;LEN_LSB,DATA_0,....,DATA_N]
                if (listResponse.dataLength > 0){
                    streamfile.write( reinterpret_cast<char*>( &listResponse[PID1] ), listResponse.dataLength + 4 );
                }
                if (listResponse.dataLength < 1024){ // only sleep if not too many entries
                    std::this_thread::sleep_for(std::chrono::microseconds(20));
                }
            }
            catch(AmptekException& e){
                std::cerr << e.what() << std::endl;
            }
            
        }
    });
    return true;
}





/**
 * @brief Disable the list mode streaming.
 * 
 * This will close the target file and delete the worker thread
 * 
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::DisableListMode(){
    listmode_flag = false;
    if (list_reader_thread != nullptr ){
        list_reader_thread->join();
        streamfile.close();
        delete list_reader_thread;
        list_reader_thread = nullptr;
        return true;
    }
    return false;
}


/**
 * @brief Reset the frame and counter registers of the list mode timer
 * 
 * This will create a 0 time record in the data stream. 
 * 
 * @note The list mode timer is free running! This does not stop the timer, only resets it!
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::ResetListModeTimer(){
    try{
        expectAcknowledge(connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_CLEAR_LIST_TIMER) );
    }
    catch(AmptekException& e){
    
            std::cerr<< "Failed clearing list-mode timer: " << e.what() << std::endl;
            return false;
    }
    return true;
}


/**
 * @brief Starts the hardware sequential buffering mode. 
 * 
 * THe sequential buffering mode ends, when all buffers are full or the buffering is canceled using stopBuffering.
 * The hardware channel for buffer triggers is AUX IN 2
 * 
 * @note Hardware signals have to be provided to buffer a spectrum
 * @note On PX5, the Connector input has to be configured to the right AUX input
 * @note While in buffering mode, spectrum requests will be blocked with an ACK BUSY response packet
 * 
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::startHardwareBuffering(){
    try{
        expectAcknowledge(connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_RESTART_SEQ_BUFFERING) );
    }
    catch(AmptekException& e){
    
            std::cerr<< "Failed clearing list-mode timer: " << e.what() << std::endl;
            return false;
    }
    return true;
}


/**
 * @brief Cancels the hardware sequential buffering mode. 
 * 
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::stopHardwareBuffering(){
    try{
        expectAcknowledge(connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_CANCEL_SEQ_BUFFERING) );
    }
    catch(AmptekException& e){
    
            std::cerr<< "Failed clearing list-mode timer: " << e.what() << std::endl;
            return false;
    }
    return true;
}



bool AmptekHardwareInterface::BufferAndClearSpectrum(uint16_t index){
    try{
        expectAcknowledge(connection_handler->sendAndReceive( Packet::generateBufferAndClearRequest(index)  ) );
    }
    catch(AmptekException& e){
    
            std::cerr<< "Failed sending buffer request: " << e.what() << std::endl;
            return false;
    }
    return true;
}


bool AmptekHardwareInterface::BufferSpectrum(uint16_t index){
    try{
        expectAcknowledge(connection_handler->sendAndReceive( Packet::generateBufferRequest(index)  ) );
    }
    catch(AmptekException& e){
    
            std::cerr<< "Failed sending buffer request: " << e.what() << std::endl;
            return false;
    }
    return true;
}

/**
 * @brief Get Status and spectrum stored in an hardware buffer on the detector
 * 
 * @note This will always return data. It as to be assured externally, that the buffer is really filled
 * @param id index of the buffer to read from
 * @return std::pair<AmptekSpectrum, AmptekStatus> spectrum and status stored in the buffer slot
 */
std::pair<AmptekSpectrum, AmptekStatus>  AmptekHardwareInterface::GetBufferedSpectrum(size_t id){
    Packet spectrumResponse = connection_handler->sendAndReceive( Packet::generateGetBufferRequest(id) );
    
    //std::cout << spectrumResponse.size() << std::endl;
    if (spectrumResponse.at(PID1) != DP5_P1_SPECTRUM_RESPONSE )
    {
            throw AmptekException( "Response Packet is not of type Spectrum: " + spectrumResponse.toString() );
            
    }
    int spectrum_length;
    switch( spectrumResponse.at(PID2) ){
        case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM256_STATUS:
            spectrum_length=256;
            break;

        case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM512_STATUS:
            spectrum_length=512;
            break;

        case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM1024_STATUS:
            spectrum_length=1024;
            break;

        case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM2048_STATUS:
            spectrum_length=2048;
            break;

        case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM4096_STATUS:
            spectrum_length=4096;
            break;

        
        case DP5_P2_SPECTRUM_RESPONSE_SPECTRUM8192_STATUS:
            spectrum_length=8192;
            break;
        
        default:
            throw AmptekException("Invalid PID2. No Status Attached: " + spectrumResponse.toString() );
    }
    
    int spectrum_bytesize = 3*spectrum_length;
    AmptekSpectrum buffered_spectrum( &(spectrumResponse.at(DATA) ), spectrum_length );
    AmptekStatus buffered_status(&(spectrumResponse.at(DATA + spectrum_bytesize)));
    //std:cout << buffered_status.SlowCount() << std::endl;
    return  std::pair<AmptekSpectrum, AmptekStatus>( buffered_spectrum, buffered_status );
}



/**
 * @brief This starts the streaming commtest allowing debugging without a signal source 
 * 
 * This will generate predictable counts in the digital backend of the detector, which can be used for MCA, MCS and List Mode.
 * It will generate signals starting at amplitude min_channel, incrementing by increment until max_channel is reached. Then the generated 
 * amplitude falls down to min_channel again.
 * 
 * @param min_channel lower amplitude limit for generated events
 * @param max_channel upper amplitude limit for generated events
 * @param increment amplitude increment between two generated events
 * @param rate event rate in evt/sec
 * @return true on success 
 * @return false on failure
 */
bool AmptekHardwareInterface::StartCommtestStreaming(uint16_t min_channel,uint16_t max_channel, 
                                    uint16_t increment, uint32_t rate)
{
    // convert from rate (cts/sec) to number of fpga clock cycles between two events
    uint32_t period = std::max(8., last_status.FpgaClock()*1e6/rate );
    std::cout << "Pulse Period: " << period << std::endl;
    try{
        Packet commtest_packet = Packet::generateCommtestStreamingRequest(min_channel, max_channel, increment, period);
        expectAcknowledge(connection_handler->sendAndReceive( commtest_packet) );
    }
    catch(AmptekException& e){
    
            std::cerr<< "Failed starting comm test: " << e.what() << std::endl;
            return false;
    }
    return true;
}


/**
 * @brief Disables the commtest streaming mode 
 * 
 * @return true on success
 * @return false on failure
 */
bool AmptekHardwareInterface::StopCommtestStreaming(){
    try{
        expectAcknowledge(connection_handler->sendAndReceive( Packet::DP5_PKT_REQUEST_STOP_STREAM_COMMTEST) );
    }
    catch(AmptekException& e){
    
            std::cerr<< "Failed stopping comm test: " << e.what() << std::endl;
            return false;
    }
    return true;
}








std::vector<unsigned int> AmptekHardwareInterface::GetSpectrum(double max_age_ms){
    updateSpectrum(max_age_ms);
	return last_spectrum.bins;
}


// int AmptekHardwareInterface::GetSpectrum(unsigned int* spectrum, double max_age_ms) {
//     updateSpectrum(max_age_ms);
//     spectrum = last_spectrum;
//     return spectrum_length;
// }





/**
 * @brief Throws AmptekException if packet is not of type Acknowledge OK.
 * 
 * This function can be wrapped around sendAndReceive calls if the request package ony expects an acknowledge response
 * 
 * 
 * @param packet 
 */
void AmptekHardwareInterface::expectAcknowledge(Packet packet){
    if (packet.at(PID1) != DP5_P1_ACK){
        throw AmptekException("Response Package was not an Acknowledge: " + packet.toString());
    }else{
        if (   packet.at(PID2) == DP5_P2_ACK_OK
            || packet.at(PID2) == DP5_P2_ACK_OK_SHARING
            || packet.at(PID2) == DP5_P2_ACK_OK_FPGAADDR){
            return;
        }else{
            std::string error_msg;
            char databuffer[packet.dataLength];
            switch (packet.at(PID2) ){

                case DP5_P2_ACK_SYNC_ERR:
                    error_msg = "Sync bytes in Request Packet were not correct, \
                                                        and therefore, the Request Packet was rejected.";
                    break;
                case DP5_P2_ACK_PID_ERR:
                    error_msg =  "PID1 & PID2 combination is not recognized as a valid \
                                                     Request Packet, and therefore, the Request Packet was rejected.";
                    break;

                case DP5_P2_ACK_LEN_ERR:
                    error_msg = "LEN field of the Request Packet was not consistent with \
                                                     Request Packet type defined by the PID1 & PID2 combination. \
                                                     It is not recognized as a valid Request Packet, \
                                                     and therefore, the Request Packet was rejected.";
                    break;

                case DP5_P2_ACK_CHECKSUM_ERR:
                    error_msg = "Checksum of the Request Packet was incorrect, \
                                                     and therefore, the Request Packet was rejected.";
                    break;


                case DP5_P2_ACK_BADPARAM_ERR:
                    error_msg = "Bad parameter.";
                    break;


                case DP5_P2_ACK_BADHEX_ERR:
                    error_msg = "Microcontroller or FPGA upload packets error: \
                                                     hex record contained in the data field \
                                                     of the Request Packet had a checksum or other structural error.";
                    break;



                case DP5_P2_ACK_BADCMD_ERR:
                    
                    byteToChar(&(packet.at(DATA)), databuffer, packet.dataLength);
                    error_msg ="Unrecognized command: " + std::string(databuffer);
                    break;



                case DP5_P2_ACK_FPGA_ERR:
                    error_msg =  "FPGA initialization failed.";
                    break;



                case DP5_P2_ACK_CP2201_ERR:
                    error_msg = "Set Ethernet Settings Request Packet was received, \
                                                     but an Ethernet controller was not detected on the DP5.";
                    break;



                case DP5_P2_ACK_SCOPE_ERR:
                    error_msg = "Send Scope Data Request Packet was received, \
                                                     but the digital oscilloscope hasn’t triggered, \
                                                     so no data is available. [The digital oscilloscope must be armed, \
                                                     and then a trigger must occur for data to be available.]";
                    break;


                case DP5_P2_ACK_PC5_ERR:
                    error_msg = "ASCII command errors – the data field will contain the \
                                                     ASCII command and parameter which caused the error. “Bad Parameter” \
                                                     means that the parameter isn’t recognized \
                                                     or exceeds the range of the command. “Unrecognized Command” \
                                                     means that the 4-character command isn’t recognized. \
                                                     “PC5 Not Present” is returned if a PC5 is not mated to the DP5, \
                                                     and a command requiring a PC5 is sent. (i.e. “HVSE”, Set High \
                                                     Voltage.) [A ‘Bad Parameter’ ACK packet may also be returned \
                                                     for a malformed I2C Request Packet, in which case LEN=0.] \
                                                     If an incomplete or garbled command is returned in the data field, \
                                                     it may mean that the ASCII Configuration Packet has structural \
                                                     issues. (Disallowed whitespace, missing semicolon, etc.)";
                    break;
                

                case DP5_P2_ACK_BUSY_ERR:
                    error_msg = "Busy, another interface in use.";
                    break;

                case DP5_P2_ACK_I2C_ERR:
                    error_msg =  "‘I2C Transfer’ Request Packet, but no I2C ACK \
                                                     was detected from an I2C Slave.";
                    break;


                case DP5_P2_ACK_VERSION_ERR:
                    error_msg ="Request Packet has been recognized as valid by the firmware, \
                                                     but it is not supported by the installed FPGA version. \
                                                     Update the FPGA to the latest FP version.";
                    break;
                    

                case DP5_P2_ACK_CALIB_ERR:
                    error_msg = "Calibration Error";
                    break;
                default:
                    error_msg = "Unknown wrong PID2 in ACK Response Package.";

            }
            throw AmptekException( error_msg );
        }
    }
}
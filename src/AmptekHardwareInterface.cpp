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


void AmptekHardwareInterface::connectUDP(std::string hostname, int port, double timeout){

    if (connection_handler != nullptr){
        throw AmptekException("Amptek is already connected");
    }
    else{
        connection_handler = new AmptekUdpConnectionHandler(hostname, port, timeout);
        current_state = ON;
    }
}

void AmptekHardwareInterface::connectSimulator(){

    if (connection_handler != nullptr){
        throw AmptekException("Amptek is already connected");
    }
    else{
        connection_handler = new AmptekSimulatorConnectionHandler();
        current_state = ON;
    }
}

bool AmptekHardwareInterface::spectrumAgeOk( double max_age_ms ) const {
    if (max_age_ms < 0){
        return false;
    }
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - last_spectrum_update_time).count() < max_age_ms;
}

bool AmptekHardwareInterface::statusAgeOk( double max_age_ms ) const {
    if (max_age_ms < 0){
        return false;
    }
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - last_status_update_time).count() < max_age_ms;
}


bool AmptekHardwareInterface::Enable(){
    try{
        expectAcknowledge( connection_handler->sendAndReceive( Packet::PX5_REQUEST_ENABLE ) );
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}



bool AmptekHardwareInterface::Disable(){
    try{
        expectAcknowledge( connection_handler->sendAndReceive( Packet::PX5_REQUEST_DISABLE ) );
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}
bool AmptekHardwareInterface::Ping(){
    try{
        expectAcknowledge( connection_handler->sendAndReceive( Packet::PX5_REQUEST_KEEP_ALIVE_NO_SHARING ) );
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}

bool AmptekHardwareInterface::ClearSpectrum(){
    try{
        expectAcknowledge( connection_handler->sendAndReceive( Packet::PX5_REQUEST_CLEAR_SPECTRUM ) );
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}

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


bool AmptekHardwareInterface::SetTextConfiguration(std::vector<std::string> commands){
    try{

        stringstream cmdstream;
        string cmd;
        int streamsize = 0;
        
        for (int i = 0; i < commands.size(); ++i){
                cmd = commands[i];

            // if max packet size (512) would be exceeded when adding this command, send the previous commands and clear the input stringstream
            if(  streamsize + cmd.size() > 511){
                expectAcknowledge( connection_handler->sendAndReceive( Packet::gernerateSetConfigurationRequest( cmdstream.str() ) ) );
                cmdstream = stringstream();
            }
            
            //append the current command to the stringstream and add seperator
            cmdstream << cmd << ";";
            streamsize += cmd.size() + 1;

            // if this is the last command in the loop, send the stringstream even if max size is not reached
            if(  i == commands.size()-1 ){
                expectAcknowledge( connection_handler->sendAndReceive( Packet::gernerateSetConfigurationRequest( cmdstream.str() ) ) );
            }
        }
    }
    catch( AmptekException& e){
        std::cerr << e.what() << std::endl;
        current_state = ERROR;
        return false;
    }
    return true;
}

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
                
                if (settingResponse.at(PID1) != PX5_RESPONSE_CONFIG
                || settingResponse.at(PID2) != PX5_RESPONSE_CONFIG_TEXT)
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

bool AmptekHardwareInterface::updateStatus(double max_age_ms){
    if ( !statusAgeOk(max_age_ms)){
        Packet statusResponse = connection_handler->sendAndReceive( Packet::PX5_REQUEST_STATUS );
        //std::cout << statusResponse.size() << std::endl;
        if (statusResponse.at(PID1) != PX5_RESPONSE_STATUS
          || statusResponse.at(PID2) != PX5_RESPONSE_STATUS_INFO)
        {
              std::cerr<< "Response Packet did not match expected PIDs: " << statusResponse.toString() << std::endl;
              return false;
        }

        //std::cout << "copy from status response" << std::endl;
        memcpy(last_status,&(statusResponse.at(DATA)), statusResponse.dataLength );
        //std::cout << "done" << std::endl;
        last_status_update_time = std::chrono::system_clock::now();
    }
    return true;
}

bool AmptekHardwareInterface::updateSpectrum(double max_age_ms){
    if ( !spectrumAgeOk(max_age_ms)){
        Packet spectrumResponse = connection_handler->sendAndReceive( Packet::PX5_REQUEST_SPECTRUM_AND_STATUS );
        //std::cout << spectrumResponse.size() << std::endl;
        if (spectrumResponse.at(PID1) != PX5_RESPONSE_SPECTRUM )
        {
              std::cerr<< "Response Packet is not of type Spectrum: " << spectrumResponse.toString() << std::endl;
              return false;
        }
        bool with_status = true;
        switch( spectrumResponse.at(PID2) ){
            case PX5_RESPONSE_SPECTRUM256:
                with_status = false;        // not break! use the respose with status setting the length
            case PX5_RESPONSE_SPECTRUM256_STATUS:
                spectrum_length=256;
                break;

            case PX5_RESPONSE_SPECTRUM512:
                with_status = false;        // not break! use the respose with status setting the length
            case PX5_RESPONSE_SPECTRUM512_STATUS:
                spectrum_length=512;
                break;

            case PX5_RESPONSE_SPECTRUM1024:
                with_status = false;        // not break! use the respose with status setting the length
            case PX5_RESPONSE_SPECTRUM1024_STATUS:
                spectrum_length=1024;
                break;

            case PX5_RESPONSE_SPECTRUM2048:
                with_status = false;        // not break! use the respose with status setting the length
            case PX5_RESPONSE_SPECTRUM2048_STATUS:
                spectrum_length=2048;
                break;

            case PX5_RESPONSE_SPECTRUM4096:
                with_status = false;        // not break! use the respose with status setting the length
            case PX5_RESPONSE_SPECTRUM4096_STATUS:
                spectrum_length=4096;
                break;

            
            case PX5_RESPONSE_SPECTRUM8192:
                with_status = false;        // not break! use the respose with status setting the length
            case PX5_RESPONSE_SPECTRUM8192_STATUS:
                spectrum_length=8192;
                break;
        }
        
        int spectrum_bytesize = 3*spectrum_length;
        try{
            for (int i = 0; i < spectrum_length; ++i){
                last_spectrum[i] = mergeBytes( spectrumResponse.at(DATA + 3*i + 2),
                                            spectrumResponse.at(DATA + 3*i + 1),
                                            spectrumResponse.at(DATA + 3*i    )
                                            );
            }
        }
        catch(std::runtime_error& e){
            throw AmptekException(std::string("Failed in AmptekHardwareInterface::updateSpectrum \
                                   during spectrum update: ") + e.what());
        }

        try{
            last_spectrum_update_time = std::chrono::system_clock::now();
            if (with_status){
                memcpy(last_status,&(spectrumResponse.at(DATA + spectrum_bytesize)), spectrumResponse.dataLength - spectrum_bytesize );
                last_status_update_time = last_spectrum_update_time;
            }
        }
        catch(std::runtime_error& e){
            throw AmptekException(std::string("Failed in AmptekHardwareInterface::updateSpectrum \
                                   during status update: ") + e.what());
        }
        
    }
    return true;
}

const byte* AmptekHardwareInterface::readSpectrum(double max_age_ms){
    throw AmptekException("AmptekHardwareInterface::readSpectrum not implemented");

}
const byte* AmptekHardwareInterface::readSpectrumAndStatus(const byte* statusbuffer, double max_age_ms){
    throw AmptekException("AmptekHardwareInterface::readSpectrumAndStatus not implemented");

}
void AmptekHardwareInterface::connectUSB(int serialNb){
if (connection_handler != nullptr){
        throw AmptekException("Amptek is already connected");
    }
    else{
        connection_handler = new AmptekUsbConnectionHandler(serialNb);
        current_state = ON;
    }
}






std::vector<unsigned int> AmptekHardwareInterface::GetSpectrum(double max_age_ms){
    updateSpectrum(max_age_ms);
    std::vector<unsigned int> spec( last_spectrum, last_spectrum + spectrum_length );
	   return spec;
}


int AmptekHardwareInterface::GetSpectrum(unsigned int* spectrum, double max_age_ms) {
    updateSpectrum(max_age_ms);
    spectrum = last_spectrum;
    return spectrum_length;
}



int AmptekHardwareInterface::FastCount(double max_age_ms){
    updateStatus(max_age_ms);
    return mergeBytes(last_status[3],last_status[2],last_status[1],last_status[0]);
}

int AmptekHardwareInterface::SlowCount(double max_age_ms){
    updateStatus(max_age_ms);
    return mergeBytes(last_status[7],last_status[6],last_status[5],last_status[4]);
}
double AmptekHardwareInterface::DeadTime(double max_age_ms){
    updateStatus(max_age_ms);
    int sc = SlowCount(1e6);
    int fc = FastCount(1e6);
    if (fc ==0){
        if (sc ==0){
            return 0;
        }else{
            return 100.;
        }
    }
    return 100. * (1. - 1.0 * sc/fc);
}
double AmptekHardwareInterface::AccTime(double max_age_ms){
    updateStatus(max_age_ms);
    return 0.001*last_status[12] + 0.1 *  mergeBytes(last_status[15],last_status[14],last_status[13]);
}

double AmptekHardwareInterface::RealTime(double max_age_ms){
    updateStatus(max_age_ms);
    return 0.001 *  mergeBytes(last_status[23],last_status[22],last_status[21],last_status[20]);
}

int AmptekHardwareInterface::FirmwareMajor(double max_age_ms){
    updateStatus(max_age_ms);
    return (last_status[24] >> 4);
}

int AmptekHardwareInterface::FirmwareMinor(double max_age_ms){
    updateStatus(max_age_ms);
    return(last_status[24] & ~(0x0F << 4));
}

int AmptekHardwareInterface::FirmwareBuild(double max_age_ms){
    updateStatus(max_age_ms);
    return(last_status[37] & ~(0x0F << 4));
}

int AmptekHardwareInterface::FpgaMajor(double max_age_ms){
    updateStatus(max_age_ms);
    return (last_status[25] >> 4);
}

int AmptekHardwareInterface::FpgaMinor(double max_age_ms){
    updateStatus(max_age_ms);
    return(last_status[25] & ~(0x0F << 4));
}

int AmptekHardwareInterface::SerialNb(double max_age_ms){
    updateStatus(max_age_ms);
    return mergeBytes(last_status[29],last_status[28],last_status[27],last_status[26]);
}

double AmptekHardwareInterface::HighVoltage(double max_age_ms){
    updateStatus(max_age_ms);
    return 0.5 * static_cast<signed_word16>( mergeBytes(last_status[30],last_status[31]) );
}

double AmptekHardwareInterface::DetectorTemp(double max_age_ms){
    updateStatus(max_age_ms);
    return 0.1 * mergeBytes(last_status[32] & ~(0x0F << 4),last_status[33]);
}

int AmptekHardwareInterface::BoardTemp(double max_age_ms){
    updateStatus(max_age_ms);
    return last_status[34];
}

bool AmptekHardwareInterface::IsPresetTimeReached(double max_age_ms){
    updateStatus(max_age_ms);
    return last_status[35] & (1 << 7);
}

bool AmptekHardwareInterface::IsEnabled(double max_age_ms){
    updateStatus(max_age_ms);
    return last_status[35] & (1 << 5);
}
bool AmptekHardwareInterface::IsPresetCountReached(double max_age_ms){
    updateStatus(max_age_ms);
    return last_status[35] & (1 << 4);
}
bool AmptekHardwareInterface::IsGated(double max_age_ms){
    updateStatus(max_age_ms);
    return !( last_status[35] & (1 << 3) );
}
int AmptekHardwareInterface::FpgaClock(double max_age_ms){
    updateStatus(max_age_ms);
    return last_status[36] & (1 << 1) ?  80 : 20;
}
int AmptekHardwareInterface::DeviceType(double max_age_ms){
    updateStatus(max_age_ms);
    return last_status[39];
}


double AmptekHardwareInterface::TecVoltage(double max_age_ms){
    updateStatus(max_age_ms);
    return 1./758.5 * mergeBytes(last_status[40],last_status[41]);
}




void AmptekHardwareInterface::expectAcknowledge(Packet packet){
    if (packet.at(PID1) != PX5_ACK){
        throw AmptekException("Response Package was not an Acknowledge: " + packet.toString());
    }else{
        if (   packet.at(PID2) == PX5_ACK_OK
            || packet.at(PID2) == PX5_ACK_OK_SHARING
            || packet.at(PID2) == PX5_ACK_OK_FPGAADDR){
            return;
        }else{
            std::string error_msg;
            char databuffer[packet.dataLength];
            switch (packet.at(PID2) ){

                case PX5_ACK_SYNC_ERR:
                    error_msg = "Sync bytes in Request Packet were not correct, \
                                                        and therefore, the Request Packet was rejected.";
                    break;
                case PX5_ACK_PID_ERR:
                    error_msg =  "PID1 & PID2 combination is not recognized as a valid \
                                                     Request Packet, and therefore, the Request Packet was rejected.";
                    break;

                case PX5_ACK_LEN_ERR:
                    error_msg = "LEN field of the Request Packet was not consistent with \
                                                     Request Packet type defined by the PID1 & PID2 combination. \
                                                     It is not recognized as a valid Request Packet, \
                                                     and therefore, the Request Packet was rejected.";
                    break;

                case PX5_ACK_CHECKSUM_ERR:
                    error_msg = "Checksum of the Request Packet was incorrect, \
                                                     and therefore, the Request Packet was rejected.";
                    break;


                case PX5_ACK_BADPARAM_ERR:
                    error_msg = "Bad parameter.";
                    break;


                case PX5_ACK_BADHEX_ERR:
                    error_msg = "Microcontroller or FPGA upload packets error: \
                                                     hex record contained in the data field \
                                                     of the Request Packet had a checksum or other structural error.";
                    break;



                case PX5_ACK_BADCMD_ERR:
                    
                    byteToChar(&(packet.at(DATA)), databuffer, packet.dataLength);
                    error_msg ="Unrecognized command: " + std::string(databuffer);
                    break;



                case PX5_ACK_FPGA_ERR:
                    error_msg =  "FPGA initialization failed.";
                    break;



                case PX5_ACK_CP2201_ERR:
                    error_msg = "Set Ethernet Settings Request Packet was received, \
                                                     but an Ethernet controller was not detected on the DP5.";
                    break;



                case PX5_ACK_SCOPE_ERR:
                    error_msg = "Send Scope Data Request Packet was received, \
                                                     but the digital oscilloscope hasn’t triggered, \
                                                     so no data is available. [The digital oscilloscope must be armed, \
                                                     and then a trigger must occur for data to be available.]";
                    break;


                case PX5_ACK_PC5_ERR:
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
                

                case PX5_ACK_BUSY_ERR:
                    error_msg = "Busy, another interface in use.";
                    break;

                case PX5_ACK_I2C_ERR:
                    error_msg =  "‘I2C Transfer’ Request Packet, but no I2C ACK \
                                                     was detected from an I2C Slave.";
                    break;


                case PX5_ACK_VERSION_ERR:
                    error_msg ="Request Packet has been recognized as valid by the firmware, \
                                                     but it is not supported by the installed FPGA version. \
                                                     Update the FPGA to the latest FP version.";
                    break;
                    

                case PX5_ACK_CALIB_ERR:
                    error_msg = "Calibration Error";
                    break;
                default:
                    error_msg = "Unknown wrong PID2 in ACK Response Package.";

            }
            throw AmptekException( error_msg );
        }
    }
}
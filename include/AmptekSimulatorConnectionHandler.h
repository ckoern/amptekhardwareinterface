#ifndef AmptekSimulatorConnectionHandler_h
#define AmptekSimulatorConnectionHandler_h

#include "AmptekConnectionHandler.h"
#include <thread>         // std::thread
#include <map>

#define STATUS_SIZE 64
class AmptekSimulatorConnectionHandler : public AmptekConnectionHandler{
public:
    AmptekSimulatorConnectionHandler();
    virtual ~AmptekSimulatorConnectionHandler();
    virtual Packet sendAndReceive( const Packet& request);
    virtual void ClearCommunicationBuffer(){};

private:
    void readConfig(char* configs);
    void initConfigs();
    std::string buildConfigResponse(char* config_names);
    void createStatusData(byte* buffer);
    void createSpectrumData(byte* buffer);
    void enable();
    void disable();
    void clear();
    int speclen = 1024;
    unsigned int* spectrum;
    uint32_t total_counts;
    double acc_time = 0;

    std::map<std::string, std::string> text_configs;

    bool is_running = true;
    bool is_enabled = false;
    bool flag_enable = false;
    bool flag_disable = false;

    double acquisition_time = -1;   
    std::thread* spectrum_thread;
};

#endif
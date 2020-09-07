#ifndef AmptekHardwareInterface_h
#define AmptekHardwareInterface_h
#include "types.h"
#include <chrono>
#include <thread>
#include <fstream>
#include <utility>

#include "AmptekConnectionHandler.h"

#include "AmptekStatus.h"
#include "AmptekSpectrum.h"

#define SPECLEN 8192

enum AmptekState{
    NOT_CONNECTED,
    ON,
    RECORDING,
    ERROR
};


class AmptekHardwareInterface{

public:

    AmptekHardwareInterface();
    ~AmptekHardwareInterface();

    void connectUSB(int serialNb);
    void connectSimulator();
    void connectUDP(std::string hostname, int port, double timeout);


    bool Enable();
    bool Disable();
    bool Ping();
    bool ClearSpectrum();
    bool SetPresetAccumulationTime(double t);
    bool SetPresetRealTime(double t);
    bool SetPresetCounts(int c);
    bool SetTextConfiguration(std::vector<std::string> commands);
    AmptekStatus& updateStatus(double max_age_ms = 100 );
    AmptekStatus& getStatus(){return last_status;}

    bool EnableListMode(std::string targetfile);
    bool ResetListModeTimer();
    bool DisableListMode();

    bool startHardwareBuffering();
    bool stopHardwareBuffering();
    bool BufferSpectrum(uint16_t index);
    bool BufferAndClearSpectrum(uint16_t index);
    std::pair<AmptekSpectrum, AmptekStatus> GetBufferedSpectrum(size_t id);

    bool StartCommtestStreaming(uint16_t min_channel,uint16_t max_channel, 
                                    uint16_t increment, uint32_t rate);
    bool StopCommtestStreaming();


    // int GetSpectrum(unsigned int* spectrum, double max_age_ms = 100);
    std::vector<unsigned int> GetSpectrum(double max_age_ms = 100);
    std::vector<std::string> GetTextConfiguration(std::vector<std::string> commands);

    AmptekState GetState() const {return current_state;};

private:

    bool spectrumAgeOk( double max_age_ms ) const;
    bool statusAgeOk( double max_age_ms ) const;
    
    bool updateSpectrum( double max_age_ms);
    void expectAcknowledge(Packet);

    //byte last_status[STATUS_SIZE];
    AmptekStatus last_status;
    AmptekSpectrum last_spectrum;
    // unsigned int last_spectrum[SPECLEN];
    // unsigned int spectrum_length = 0;
    //std::chrono::time_point<std::chrono::system_clock> last_status_update_time;
    // std::chrono::time_point<std::chrono::system_clock> last_spectrum_update_time;

    bool listmode_flag;

    AmptekState current_state = NOT_CONNECTED;

    AmptekConnectionHandler* connection_handler = nullptr;

    std::thread* list_reader_thread = nullptr;
    std::ofstream streamfile;
};


#endif
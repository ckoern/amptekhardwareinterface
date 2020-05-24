#ifndef AmptekHardwareInterface_h
#define AmptekHardwareInterface_h
#include "types.h"
#include <chrono>
#include <thread>
#include <fstream>

#include "AmptekConnectionHandler.h"

#define STATUS_SIZE 64
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

    //bool readStatus(const byte*, int&, double max_age_ms = -1);
    const byte* readSpectrum(double max_age_ms = -1);
    const byte* readSpectrumAndStatus(const byte* statusbuffer, double max_age_ms = -1);

    bool Enable();
    bool Disable();
    bool Ping();
    bool ClearSpectrum();
    bool SetPresetAccumulationTime(double t);
    bool SetPresetRealTime(double t);
    bool SetPresetCounts(int c);
    bool SetTextConfiguration(std::vector<std::string> commands);
    bool UpdateStatus() {return updateStatus(-1);};

    bool EnableListMode(std::string targetfile);
    bool ResetListModeTimer();
    bool DisableListMode();

    bool StartCommtestStreaming(uint16_t min_channel,uint16_t max_channel, 
                                    uint16_t increment, uint16_t rate);
    bool StopCommtestStreaming();

    int FastCount(double max_age_ms = 100);
    int SlowCount(double max_age_ms = 100);
    double DeadTime(double max_age_ms = 100);
    double AccTime(double max_age_ms = 100);
    double RealTime(double max_age_ms = 100);
    int FirmwareMajor(double max_age_ms  = 1000000);
    int FirmwareMinor(double max_age_ms = 1000000);
    int FirmwareBuild(double max_age_ms = 1000000);
    int FpgaMajor(double max_age_ms = 1000000);
    int FpgaMinor(double max_age_ms = 1000000);
    int SerialNb(double max_age_ms = 1000000);
    double HighVoltage(double max_age_ms = 100);
    double DetectorTemp(double max_age_ms = 100);
    int BoardTemp(double max_age_ms = 100);
    bool IsPresetTimeReached(double max_age_ms = 100);
    bool IsEnabled(double max_age_ms = 100);
    bool IsPresetCountReached(double max_age_ms = 100);
    bool IsGated(double max_age_ms = 100);
    int FpgaClock(double max_age_ms = 100);
    int DeviceType(double max_age_ms = 1000000);
    double TecVoltage(double max_age_ms = 100);

    int GetSpectrum(unsigned int* spectrum, double max_age_ms = 100);
    std::vector<unsigned int> GetSpectrum(double max_age_ms = 100);
    std::vector<std::string> GetTextConfiguration(std::vector<std::string> commands);

    AmptekState GetState() const {return current_state;};

private:

    bool spectrumAgeOk( double max_age_ms ) const;
    bool statusAgeOk( double max_age_ms ) const;
    bool updateStatus(double max_age_ms );
    bool updateSpectrum( double max_age_ms);
    void expectAcknowledge(Packet);

    byte last_status[STATUS_SIZE];
    unsigned int last_spectrum[SPECLEN];
    unsigned int spectrum_length = 0;
    std::chrono::time_point<std::chrono::system_clock> last_status_update_time;
    std::chrono::time_point<std::chrono::system_clock> last_spectrum_update_time;

    bool listmode_flag;

    AmptekState current_state = NOT_CONNECTED;

    AmptekConnectionHandler* connection_handler = nullptr;

    std::thread* list_reader_thread = nullptr;
    std::ofstream streamfile;
};


#endif
#ifndef AmptekStatus_h
#define AmptekStatus_h

#include "types.h"
#include <chrono>
#define STATUS_SIZE 64

class AmptekStatus{

public:
    AmptekStatus(const byte* raw);
    AmptekStatus(const byte* raw, std::chrono::time_point<std::chrono::system_clock> timestamp);
    AmptekStatus();
    ~AmptekStatus();
    int FastCount();
    int SlowCount();
    double DeadTime();
    double AccTime();
    double RealTime();
    int FirmwareMajor();
    int FirmwareMinor();
    int FirmwareBuild();
    int FpgaMajor();
    int FpgaMinor();
    int SerialNb();
    double HighVoltage();
    double DetectorTemp();
    int BoardTemp();
    bool IsPresetTimeReached();
    bool IsEnabled();
    bool IsPresetCountReached();
    bool IsGated();
    int FpgaClock();
    int DeviceType();
    double TecVoltage();

    bool ListModeLMMO();
    int ListModeClock();
    int ListModeSync();
    bool SequentialBufferRunning();
    int SequentialBufferIndex();


    double AgeMillis() const;
private:
    byte raw_status[STATUS_SIZE];
    std::chrono::time_point<std::chrono::system_clock> timestamp;


};

#endif
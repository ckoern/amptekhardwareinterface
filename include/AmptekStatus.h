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
    int FastCount() const;
    int SlowCount() const;
    int GpCount() const;
    double DeadTime() const;
    double AccTime() const;
    double RealTime() const;
    int FirmwareMajor() const;
    int FirmwareMinor() const;
    int FirmwareBuild() const;
    int FpgaMajor() const;
    int FpgaMinor() const;
    int SerialNb() const;
    double HighVoltage() const;
    double DetectorTemp() const;
    int BoardTemp() const;
    bool IsPresetTimeReached() const;
    bool IsEnabled() const;
    bool IsPresetCountReached() const;
    bool IsGated() const;
    int FpgaClock() const;
    int DeviceType() const;
    double TecVoltage() const;

    bool ListModeLMMO()  const;
    int ListModeClock()  const;
    int ListModeSync()  const;
    bool SequentialBufferRunning()  const;
    int SequentialBufferIndex()  const;


    double AgeMillis() const;
private:
    byte raw_status[STATUS_SIZE];
    std::chrono::time_point<std::chrono::system_clock> timestamp;


};

#endif
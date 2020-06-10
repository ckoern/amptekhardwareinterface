#include "AmptekStatus.h"

#include <cstring>

AmptekStatus::AmptekStatus(const byte* raw) : AmptekStatus(raw, std::chrono::system_clock::now())
{
}



AmptekStatus::AmptekStatus(const byte* raw, std::chrono::time_point<std::chrono::system_clock> timestamp)
    : timestamp(timestamp)
{
    memcpy(raw_status,raw, STATUS_SIZE );
}
AmptekStatus::AmptekStatus(){
    timestamp = std::chrono::system_clock::from_time_t(0);
}
AmptekStatus::~AmptekStatus(){
    //delete[] raw_status;
}


double AmptekStatus::AgeMillis() const{
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp).count();
}


/**
 * @brief Total fast counts of the current acqusition
 * 
 * @return int 
 */
int AmptekStatus::FastCount() const{
    
    return mergeBytes(raw_status[3],raw_status[2],raw_status[1],raw_status[0]);
}


/**
 * @brief Total slow counts of the current acqusition
 * 
 * @return int counts
 */
int AmptekStatus::SlowCount() const{
    
    return mergeBytes(raw_status[7],raw_status[6],raw_status[5],raw_status[4]);
}
/**
 * @brief Current Count in the General Purpose Counter
 * 
 * @return int 
 */
int AmptekStatus::GpCount() const{
    return mergeBytes(raw_status[11],raw_status[10],raw_status[9],raw_status[8]);
}

/**
 * @brief Dead time of the current acquisition
 * 
 * @return double dead time in per cent
 */
double AmptekStatus::DeadTime() const{
    
    int sc = SlowCount();
    int fc = FastCount();
    if (fc ==0){
        if (sc ==0){
            return 0;
        }else{
            return 100.;
        }
    }
    return 100. * (1. - 1.0 * sc/fc);
}

/**
 * @brief Accumulation time of the current acquisition
 * 
 * This timer is stopped during gateing, transfer or reset lockout
 * 
 * @return double time in seconds
 */
double AmptekStatus::AccTime() const{
    
    return 0.001*raw_status[12] + 0.1 *  mergeBytes(raw_status[15],raw_status[14],raw_status[13]);
}

/**
 * @brief Total real time of the current acquisition
 * 
 * @return double time in seconds
 */
double AmptekStatus::RealTime() const{
    
    return 0.001 *  mergeBytes(raw_status[23],raw_status[22],raw_status[21],raw_status[20]);
}


/**
 * @brief Firmware major version number
 * 
 * @return int 
 */
int AmptekStatus::FirmwareMajor() const{
    
    return (raw_status[24] >> 4);
}


/**
 * @brief Firmware minor version number
 * 
 * @return int 
 */
int AmptekStatus::FirmwareMinor() const{
    
    return(raw_status[24] & ~(0x0F << 4));
}


/**
 * @brief Firmware build version number
 * 
 * @return int 
 */
int AmptekStatus::FirmwareBuild() const{
    
    return(raw_status[37] & ~(0x0F << 4));
}

/**
 * @brief FPGA major version number
 * 
 * @return int 
 */
int AmptekStatus::FpgaMajor() const{
    
    return (raw_status[25] >> 4);
}


/**
 * @brief FPGA minor version number
 * 
 * @return int 
 */
int AmptekStatus::FpgaMinor() const{
    
    return(raw_status[25] & ~(0x0F << 4));
}


/**
 * @brief Device Serial Number
 * 
 * @return int 
 */
int AmptekStatus::SerialNb() const{
    
    return mergeBytes(raw_status[29],raw_status[28],raw_status[27],raw_status[26]);
}


/**
 * @brief Voltage applied accross the chip
 * 
 * @return double in volts
 */
double AmptekStatus::HighVoltage() const{
    
    return 0.5 * static_cast<signed_word16>( mergeBytes(raw_status[30],raw_status[31]) );
}


/**
 * @brief Detector Chip Temperature
 * 
 * @return double temperature in Kelvin
 */
double AmptekStatus::DetectorTemp() const{
    
    return 0.1 * mergeBytes(raw_status[32] & ~(0x0F << 4),raw_status[33]);
}

/**
 * @brief Amptek Board Temperature
 * 
 * @return int in deg Celsius
 */
int AmptekStatus::BoardTemp() const{
    
    return raw_status[34];
}

/**
 * @brief Preset time reached flag
 * 
 * @return true  if acquisition was stopped due to reaching the set preset time
 * @return false  if not reached or not set
 */
bool AmptekStatus::IsPresetTimeReached() const{
    
    return raw_status[35] & (1 << 7);
}

/**
 * @brief MCA acquisition status
 * 
 * @return true if acquisition is running
 * @return false otherwise
 */
bool AmptekStatus::IsEnabled() const{
    
    return raw_status[35] & (1 << 5);
}

/**
 * @brief preset count reached flag
 * 
 * @return true if acquisition was stopped due to reaching the set preset counts
 * @return false if not reached or not set
 */
bool AmptekStatus::IsPresetCountReached() const{
    
    return raw_status[35] & (1 << 4);
}

/**
 * @brief Gating status
 * 
 * @return true if gate active (rejecting counts)
 * @return false if disabled or deactivated (accepting counts)
 */
bool AmptekStatus::IsGated() const{
    
    return !( raw_status[35] & (1 << 3) );
}

/**
 * @brief FPGA clock speed
 * 
 * @return int clock speed in MHz
 */
int AmptekStatus::FpgaClock() const{
    
    return raw_status[36] & (1 << 1) ?  80 : 20;
}


/**
 * @brief DP5 device type
 * 
 * @return int 0: DP5, 1: PX5, 2: DP5G, 3: MCA8000D, 4: TB5, 5: DP5-X
 */
int AmptekStatus::DeviceType() const{
    
    return raw_status[39];
}


/**
 * @brief Voltage applied to the termoelectric cooler
 * 
 * @return double voltage in volts
 */
double AmptekStatus::TecVoltage() const{
    
    return 1./758.5 * mergeBytes(raw_status[40],raw_status[41]);
}


/**
 * @brief list mode deadtime correction feature
 * 
 * @return true deadtime correction feature enabled
 * @return false normal list mode operation
 */
bool AmptekStatus::ListModeLMMO() const{
    return raw_status[43] & (1 << 3);
}


/**
 * @brief list mode clock speed
 * 
 * @return int list mode clock speed in nanoseconds
 */
int AmptekStatus::ListModeClock() const{
    int multiplier  = ListModeSync() == 1 ? 1000 : 1;
    return raw_status[43] & (1 << 2) ? multiplier * 1000 : multiplier * 100;
}


/**
 * @brief List Mode sync source
 * 
 * @return int 0: internal, 1: no timetag, 2: external, 3: frame
 */
int AmptekStatus::ListModeSync() const{
    return raw_status[43] & 0b11;
}


/**
 * @brief Checks if sequential buffering is running
 * 
 * @return true if running
 * @return false if finished or disabled
 */
bool AmptekStatus::SequentialBufferRunning() const{
    return raw_status[46] & (1 << 1);
}

/**
 * @brief Returns the buffer index of this status packet
 * 
 * @return int current buffer index
 */
int AmptekStatus::SequentialBufferIndex() const{
    return ( (raw_status[46] & (0b1)) << 8) + raw_status[47]; // 9 bit value
}



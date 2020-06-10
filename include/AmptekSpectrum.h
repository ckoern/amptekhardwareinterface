#ifndef AmptekSpectrum_h
#define AmptekSpectrum_h

#include "types.h"
#include <chrono>

struct AmptekSpectrum{
    std::vector<unsigned int> bins;
    size_t nbins;
    std::chrono::time_point<std::chrono::system_clock> timestamp;


    AmptekSpectrum(const byte* raw_spec, size_t nbins, std::chrono::time_point<std::chrono::system_clock> timestamp)
        : nbins(nbins), timestamp(timestamp)
    {
        for (size_t i = 0; i < nbins; ++i){
            bins.push_back(mergeBytes(  raw_spec[3*i + 2],
                                        raw_spec[3*i + 1],
                                        raw_spec[3*i    ]
                                    )
                            );
        }
    }

    AmptekSpectrum(const byte* raw_spec, size_t nbins) 
        : AmptekSpectrum(raw_spec, nbins,std::chrono::system_clock::now())
    {

    }

    AmptekSpectrum() 
        : AmptekSpectrum(nullptr, 0, std::chrono::system_clock::from_time_t(0))
    {

    }

    double AgeMillis() const{
        auto now = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(now - timestamp).count();
    }
};

#endif
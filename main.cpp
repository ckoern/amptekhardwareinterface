#include "AmptekHardwareInterface.h"
#include <iostream>

int main(int argc, char** argv){
    std::cout << "Create Interface.." << std::endl;
    AmptekHardwareInterface amptek;
    std::cout << "Connect via UDP.." << std::endl;
    amptek.connectUDP("192.168.0.10", 10001, 3.);
    std::cout << "Finished" << std::endl;

}
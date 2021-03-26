"""Example script for the basic usage of the detector interface
"""

__author__ = "Christian Koernig"


import sys
import time

import matplotlib.pyplot as plt 

import amptek_hardware_interface as ahi


# create the interface
amptek = ahi.AmptekHardwareInterface()

# connect the intrerface via USB to the first DP5 device.
# To connect to a specific device, change the -1 to the serial number
# For basic tests, the simulator interface can be used, of no hardware is available.
# Use amptek.connectSimulator() instead
amptek.connectUSB(-1)




# Send a ping to test the connection
if amptek.Ping() :
    print("Ping succeeded")
else:
    print("Ping failed! Exited")
    sys.exit()


# Print some common configuration values
config_names = ["RESC", "CLCK", "TPEA", "GAIF", "GAIN", "RESL", "TFLA", "TPFA", 
               "PURE", "RTDE", "MCAS", "MCAC", "SOFF", "AINP", "INOF", "GAIA",
               "CUSP", "PDMD", "THSL", "TLLD", "THFA", "DACO", "DACF", "DACF",
               "RTDS", "RTDT", "BLRM", "BLRD", "BLRU", "AUO1", "PRET", "PRER",
               "PREC", "PRCL", "PRCH", "HVSE", "TECS", "PAPZ", "PAPS", "SCOE",
               "SCOT", "SCOG", "MCSL", "MCSH", "MCST", "AUO2", "TPMO", "GPED",
               "GPGA", "GPMC", "MCAE", "VOLU", "CON1", "CON2"]
configs = amptek.GetTextConfiguration( config_names )
for config in configs:
    print(config)

# prepare a 10 second acquisition
amptek.ClearSpectrum()
amptek.SetPresetAccumulationTime(4)

# start acquisition
amptek.Enable()
print("Acquisition started")


# check the current status and some status attributes every second until finished
while True:
    time.sleep(1)    
    status = amptek.updateStatus(-1)
    # test if finished
    if not status.IsEnabled():
        break 
    
    print("\rAccumulation Time: {:.2f}s, Fast Counts: {:d}, Slow Counts: {:d}".format( status.AccTime(), status.FastCount(), status.SlowCount() ), end="", flush=True)

print("Acquisition finished")

# plot finished spectrum
fig, ax = plt.subplots()
ax.plot( amptek.GetSpectrum() )

plt.show()
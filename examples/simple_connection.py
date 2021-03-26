import AmptekHardwareInterface as ahi
amptek = ahi.AmptekHardwareInterface()
#amptek.connectUDP("192.168.1.10", 10001, 1)
amptek.connectUSB(-1)
print("ping")
print( amptek.Ping() )
# print("status")
# dat= amptek.readStatus(-1)
print( "fnished" )
amptek.GetSpectrum()

config_names = ["RESC", "CLCK", "TPEA", "GAIF", "GAIN", "RESL", "TFLA", "TPFA", 
               "PURE", "RTDE", "MCAS", "MCAC", "SOFF", "AINP", "INOF", "GAIA",
               "CUSP", "PDMD", "THSL", "TLLD", "THFA", "DACO", "DACF", "DACF",
               "RTDS", "RTDT", "BLRM", "BLRD", "BLRU", "AUO1", "PRET", "PRER",
               "PREC", "PRCL", "PRCH", "HVSE", "TECS", "PAPZ", "PAPS", "SCOE",
               "SCOT", "SCOG", "MCSL", "MCSH", "MCST", "AUO2", "TPMO", "GPED",
               "GPGA", "GPMC", "MCAE", "VOLU", "CON1", "CON2"]
print( "amptek.GetTextConfiguration( config_names )")
configs = amptek.GetTextConfiguration( config_names )
print(configs)
print(amptek.SerialNb())
print("done")
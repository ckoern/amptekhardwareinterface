#!/usr/bin/env python3

from tango import AttrQuality, AttrWriteType, DispLevel, DevState, DebugIt
from tango.server import Device, attribute, command, pipe, device_property
from collections import OrderedDict
import tango
import AmptekHardwareInterface
import os

class AmptekPX5(Device):


    Hostname = device_property(dtype=str, default_value="192.168.1.10")
    port = device_property(dtype=int, default_value=10001)
    serialNb = device_property(dtype=int, default_value="-1")
    timeout_seconds = device_property(dtype=float, default_value=1)
    detector_model = device_property(dtype=str, default_value="CdTe")
    connection_mode = device_property(dtype=str, default_value="UDP")
   

    MaxInfoAge = attribute(label = "MaxInfoAge", dtype=float,
                            fget = "get_maxinfoage", fset = "set_maxinfoage")
    

    #alias for PresetAccTime to be a dropin replacement of the standard AmptekPX5 DeviceServer
    AcquisitionTime = attribute(label="AcquisitionTime", dtype=float,
                      fget="get_presetacctime", fset = "set_presetacctime")

    PresetAccTime = attribute(label="PresetAccTime", dtype=float,
                      fget="get_presetacctime", fset = "set_presetacctime")

    PresetRealTime = attribute(label="PresetRealTime", dtype=float,
                      fget="get_presetrealtime", fset = "set_presetrealtime")
    
    
    DeadTime = attribute(label="DeadTime", dtype=float,
                      fget=lambda self: self.GetDeadtime(self._max_info_age))
                      
    FastCount = attribute(label="FastCount", dtype=float,
                      fget=lambda self: self.GetFastCount(self._max_info_age))

    SlowCount = attribute(label="SlowCount", dtype=int,
                      fget=lambda self: self.GetSlowCount(self._max_info_age))


    Spectrum = attribute(label="Spectrum", dtype=(int,),
                      fget=lambda self: self.GetSpectrum(self._max_info_age), max_dim_x = 8192)


    AccTime = attribute(label="AccTime", dtype=float,
                      fget=lambda self: self.GetAcctime(self._max_info_age))

    RealTime = attribute(label="RealTime", dtype=float,
                      fget=lambda self: self.GetRealtime(self._max_info_age))


    HighVoltage = attribute(label="HighVoltage", dtype=float,
                      fget=lambda self: self.GetHighVoltage(self._max_info_age))

    DetectorTemp = attribute(label="DetectorTemp", dtype=float,
                      fget=lambda self: self.GetDetectorTemp(self._max_info_age))

    BoardTemp = attribute(label="BoardTemp", dtype=float,
                      fget=lambda self: self.GetBoardTemp(self._max_info_age))

    PresetTimeReached = attribute(label="PresetTimeReached", dtype=bool,
                      fget=lambda self: self.IsPresetTimeReached(self._max_info_age))
    
    Enabled = attribute(label="Enabled", dtype=bool,
                      fget=lambda self: self.IsEnabled(self._max_info_age))
    Gated = attribute(label="Gated", dtype=bool,
                      fget=lambda self: self.IsGated(self._max_info_age))

    PresetCountReached = attribute(label="PresetCountReached", dtype=bool,
                      fget=lambda self: self.IsPresetCountReached(self._max_info_age))
    

    FirmwareMajor = attribute(label="FirmwareMajor", dtype=int,
                      fget="get_firmwaremajor")

    FirmwareMinor = attribute(label="FirmwareMinor", dtype=int,
                      fget="get_firmwareminor")
    
    FirmwareBuild = attribute(label="FirmwareBild", dtype=int,
                      fget="get_firmwarebuild")

    FpgaMajor = attribute(label="FpgaMajor", dtype=int,
                      fget="get_fpgamajor")

    FpgaMinor = attribute(label="FpgaMinor", dtype=int,
                      fget="get_fpgaminor")

    SerialNb = attribute(label="SerialNb", dtype=int,
                      fget="get_serialnb")


    FpgaClock = attribute(label="FpgaClock", dtype=int,
                      fget="get_fpgaclock")

    DeviceType = attribute(label="DeviceType", dtype=str,
                      fget="get_devtype")

    TecVoltage = attribute(label="TecVoltage", dtype=float,
                      fget=lambda self: self.GetTecVoltage(self._max_info_age))


    CalibrationSlope = attribute(label="CalibrationSlope", dtype=float,
                      fget=lambda self: self._calibrationslope, fset = "set_calibrationslope")
    CalibrationOffset = attribute(label="CalibrationOffset", dtype=float,
                      fget=lambda self: self._calibrationoffset, fset = "set_calibrationoffset")

    def init_device(self):
        Device.init_device(self)
        self._max_info_age = 100
        self.interface = AmptekHardwareInterface.AmptekHardwareInterface()
        if self.connection_mode == "UDP":
            self.interface.connectUDP( self.Hostname, self.port,  self.timeout_seconds)
        elif self.connection_mode == "USB":
            self.interface.connectUSB(self.serialNb)
        elif self.connection_mode == 'Simulator':
            self.interface.connectSimulator()
        else:
            raise RuntimeError("connection_mode must be 'USB' or 'UDP' or 'Simulator'")
        
        self._calibrationoffset = 0
        self._calibrationslope = 1.
        self.parameter_dicts = OrderedDict()
        self.basic_params = OrderedDict() 
        self.basic_params["CLCK"] = { "value": "N/A", "name": "Clock" , "options" : ["20", "80", "AUTO"] }

        self.basic_params["TPEA"] = { "value": "N/A", "name": "Peaking Time"  }
        self.basic_params["TPFA"] = { "value": "N/A", "name": "Fast Channel Peaking Time" , "options" : ["50","100","200","400","800","1600","3200"] }
        self.basic_params["RESL"] = { "value": "N/A", "name": "Reset Lockout Time" }
        self.basic_params["TFLA"] = { "value": "N/A", "name": "Flat top time" }

        self.basic_params["GAIN"] = { "value": "N/A", "name": "Total Gain" }
        self.basic_params["GAIF"] = { "value": "N/A", "name": "Fine Gain", "readonly" : True }
        self.basic_params["GAIA"] = { "value": "N/A", "name": "Analog Gain Index", "readonly" : True }

        self.basic_params["MCAS"] = { "value": "N/A", "name": "MCA Source", "options": ["NORM", "MCS", "FAST", "PUR", "RTD"] }
        self.basic_params["MCAC"] = { "value": "N/A", "name": "MCA Channels", "options": ["256", "512", "1024", "2048", "4096", "8192"] }
        self.basic_params["SOFF"] = { "value": "N/A", "name": "Spectrum Offset"}
        self.basic_params["AINP"] = { "value": "N/A", "name": "Input Polarity", "options": ["POS","NEG"]}
        self.basic_params["INOF"] = { "value": "N/A", "name": "Input Offset"}

        self.parameter_dicts['basic']= self.basic_params




        self.acquisition_params = OrderedDict()
        self.acquisition_params["PRET"] = { "value": "N/A", "name": "Preset Time" }
        self.acquisition_params["PRER"] = { "value": "N/A", "name": "Preset Real Time" }
        self.acquisition_params["PREC"] = { "value": "N/A", "name": "Preset Counts" }
        self.acquisition_params["PRCL"] = { "value": "N/A", "name": "Preset Counts Low Channel" }
        self.acquisition_params["PRCH"] = { "value": "N/A", "name": "Preset Counts High Channel" }

        self.parameter_dicts["acquisition"] = self.acquisition_params 






        self.correction_params = OrderedDict() 
        self.correction_params["PURE"] = { "value": "N/A", "name": "Pile-Up Rejection", "options" : ["ON", "OFF", "MAX"]  }
        if self.detector_model == "CdTe":
            self.correction_params["RTDE"] = { "value": "N/A", "name": "Risetime Discrimination" , "options" : ["ON", "OFF"] }
            self.correction_params["RTDS"] = { "value": "N/A", "name": "RTD Sensitivity"  }
            self.correction_params["RTDT"] = { "value": "N/A", "name": "RTD Threshold"  }
        
        self.correction_params["BLRM"] = { "value": "N/A", "name": "Baseline Restorerer Mode", "options": ["OFF", "1"]  }
        self.correction_params["BLRD"] = { "value": "N/A", "name": "BR Down Correction", "options": ["0", "1", "2", "3"]  }
        self.correction_params["BLRU"] = { "value": "N/A", "name": "BR Up Correction", "options": ["0", "1", "2", "3"]  }

        self.parameter_dicts["correction"] = self.correction_params 




        self.gate_params = OrderedDict() 
        self.gate_params["GATE"] = { "value": "N/A", "name": "Gate Input" , "options" : ["OFF", "HIGH", "LOW"] }
        self.gate_params["GPED"] = { "value": "N/A", "name": "Gate Counter Edge" , "options" : ["RISING", "FALLING"] }
        self.gate_params["GPGA"] = { "value": "N/A", "name": "GP Counter use Gate" , "options" : ["ON", "OFF"] }
        self.gate_params["GPIN"] = { "value": "N/A", "name": "Input of GP Counter" , "options" : ["AUX1", "AUX2", "PILEUP", "RTDREJ", "SCA8", "RESPER", "DETRES", "OFF"] }
        self.gate_params["GPMC"] = { "value": "N/A", "name": "GP Counter clear with MCA" , "options" : ["ON", "OFF"] }
        self.gate_params["GPME"] = { "value": "N/A", "name": "GP Counter uses MCA enable" , "options" : ["ON", "OFF"] }
        
        self.parameter_dicts["gate"] = self.gate_params 



        self.shaping_params = OrderedDict() 
        self.shaping_params["CUSP"] = { "value": "N/A", "name": "Non-Trapezoidal Shaping"  }
        self.shaping_params["PDMD"] = { "value": "N/A", "name": "Peak Detect Mode", "options": ["NORM", "MIN"]  }
        self.shaping_params["THSL"] = { "value": "N/A", "name": "Slow Threshold (in per cent)"  }
        self.shaping_params["TLLD"] = { "value": "N/A", "name": "Low Level Threshold"  }
        self.shaping_params["THFA"] = { "value": "N/A", "name": "Fast Threshold (0-256)"  }

        self.parameter_dicts["shaping"] = self.shaping_params




        self.diagnostic_params = OrderedDict() 
        self.diagnostic_params["DACO"] = { "value": "N/A", "name": "Output DAC", "options": ["OFF", "FAST", "SHAPED", "INPUT", "PEAK"]  }
        self.diagnostic_params["DACF"] = { "value": "N/A", "name": "Output DAC Offset" }
        
        self.parameter_dicts["diagnostic"] = self.diagnostic_params 





        self.other_params = OrderedDict() 
        self.other_params["HVSE"] = { "value": "N/A", "name": "High Voltage"  }
        self.other_params["TECS"] = { "value": "N/A", "name": "TEC Temperature Target"  }
        self.other_params["PAPS"] = { "value": "N/A", "name": "Preamp Volage", "options":  ["8.5", "5", "OFF"]  }
        self.other_params["MCAE"] = { "value": "N/A", "name": "Initial MCA Enable", "options":  ["ON", "OFF"]  }

        self.parameter_dicts['other']= self.other_params

        self.param_names_in_correct_order = []
        for _,c in self.parameter_dicts.items():
            for name,_ in c.items():
                self.param_names_in_correct_order.append(name)

        for _,c in self.parameter_dicts.items():
            self.load_config_dict(c)



    def load_config_dict( self, configs ):
        cmd_strings = []
        for cmd, setting in configs.items():
            cmd_strings.append( cmd )
        settings = self.interface.GetTextConfiguration( cmd_strings )
        for setting in settings:
            cmd, val = setting.split("=")
            try:
                self.__set_config(cmd,val)
            except RuntimeWarning as e:
                self.warn_stream("Cannot set '%s': %s"%(setting, e))

    def __find_config(self, parameter):
        for dictname, param_dict in self.parameter_dicts.items():
            if parameter in param_dict:
                return param_dict[parameter]["value"]

        raise RuntimeWarning("Parameter '%s' not found!"% parameter)

    def __find_parameter(self, parameter):
        for dictname, param_dict in self.parameter_dicts.items():
            if parameter in param_dict:
                return param_dict[parameter]

        raise RuntimeWarning("Parameter '%s' not found!"% parameter)


    def __set_config(self, parameter, value):
        for dictname, param_dict in self.parameter_dicts.items():
            if parameter in param_dict:
                param_dict[parameter]["value"] = value
                return
        raise RuntimeWarning("Parameter '%s' not found!"% parameter)

    @command
    def Enable(self):
        success = self.interface.Enable()
        if not success:
            self.set_state(tango.DevState.ALARM)
            self.error_stream("Failed starting the detector")
            raise RuntimeError("Failed starting the detector")

    @command
    def Disable(self):
        success = self.interface.Disable()
        if not success:
            self.set_state(tango.DevState.ALARM)
            self.error_stream("Failed stopping the detector")
            raise RuntimeError("Failed stopping the detector")


    @command
    def ClearSpectrum(self):
        success = self.interface.ClearSpectrum()
        if not success:
            self.set_state(tango.DevState.ALARM)
            self.error_stream("Failed clearing")
            raise RuntimeError("Failed clearing the detector")


    @command(dtype_in = (str,), dtype_out = (str,))
    def GetTextConfiguration(self, configs):
        settings = []
        for config in configs:
            try:
                settings.append( "%s=%s;"%(config,self.__find_config(config)) )
            except RuntimeWarning as e:
                self.error_stream("Cannot read '%s': %s"%(config, e))
                settings.append( "%s=N/F;"%(config) )
        return settings

    @command(dtype_in = str, dtype_out = (str,))
    def GetConfigurationParameters(self, config_type):
        configs = []
        if config_type == "all":
            for dictname, param_dict in self.parameter_dicts.items():
                for config_name in param_dict:
                    configs.append(config_name)
        else:
            for config_name in self.parameter_dicts[config_type]:
                    configs.append(config_name)
        return configs

    @command( dtype_out = (str,))
    def GetParameterCategories(self):
        return list(self.parameter_dicts.keys())

    @command(dtype_in = str, dtype_out = str)
    def GetParameterCategory(self, configname):
        for dictname, param_dict in self.parameter_dicts.items():
            if configname in param_dict:
                return dictname
        raise RuntimeError("Parameter %s is unknown"%configname)

    @command(dtype_in = str, dtype_out = str)
    def GetParameterTitle(self, configname):
        for dictname, param_dict in self.parameter_dicts.items():
            if configname in param_dict:
                return param_dict[configname]['name']
        raise RuntimeError("Parameter %s is unknown"%configname)

    @command(dtype_in = str, dtype_out = str)
    def GetParameterValue(self, configname):
        for dictname, param_dict in self.parameter_dicts.items():
            if configname in param_dict:
                return param_dict[configname]['value']
        raise RuntimeError("Parameter %s is unknown"%configname)

    @command(dtype_in = str, dtype_out = (str,))
    def GetParameterOptions(self, configname):
        for dictname, param_dict in self.parameter_dicts.items():
            if configname in param_dict:
                return param_dict[configname].get('options', [])
        raise RuntimeError("Parameter %s is unknown"%configname)

    @command(dtype_in = str, dtype_out = bool)
    def IsParameterReadonly(self, configname):
        for dictname, param_dict in self.parameter_dicts.items():
            if configname in param_dict:
                return param_dict[configname].get('readonly', False)
        raise RuntimeError("Parameter %s is unknown"%configname)

    @command(dtype_out=str)
    def DumpTextConfiguration(self):
        
        output = "{:-^50s}\n".format("AMPTEK ASCII CONFIGURATION" ) 
        for parameter_type, param_dict in self.parameter_dicts.items():
            output += "{:=^50s}\n".format(parameter_type)
            for cmd, setting in param_dict.items():
                output += "{:>4s}: {:10s} ({:s})\n".format(cmd, setting["value"], setting['name'])
            output += 50*'=' + "\n"
        self.info_stream( output )

        print(output)
        return output

    @command(dtype_in = (str,))
    def SetTextConfiguration(self, configs):
        success =  self.interface.SetTextConfiguration( configs )
        if not success:
            self.error_stream("Failed Setting the text configuration!")
        #sync the store in the device server by reading all configs
        for _,c in self.parameter_dicts.items():
            self.load_config_dict(c)

    @command(dtype_in = str)
    def LoadConfigurationFile(self, filepath):
        with open(filepath) as f:
            self.LoadConfiguration( f.read() )
        # configs = []
        # cmds = []
        # vals = []
        # with open(filepath) as f:
        #     for line in f:
        #         try:
        #             cmd,val = line.strip(";\n").split("=") # test if line can be split to the correct format
        #             cmds.append(cmd)
        #             vals.append(val)
        #             configs.append(line.strip(";"))
        #         except:
        #             self.warn_stream("Cannot interpret line '%s'. Will be ignored for configuration loading!"%line.strip())
        # success =  self.interface.SetTextConfiguration(  ["RESC=YES"] + self.sort_commands(cmds, vals) )
        # if not success:
        #     self.set_state(tango.DevState.ALARM)
        #     self.error_stream("Failed Setting the text configuration!")


    @command(dtype_in = str)
    def LoadConfiguration(self, configlines):
        configs = []
        cmds = []
        vals = []
        for line in configlines.splitlines():
            try:
                cmd,val = line.strip(";\n").split("=") # test if line can be split to the correct format
                cmds.append(cmd)
                vals.append(val)
                configs.append(line.strip(";"))
            except:
                self.warn_stream("Cannot interpret line '%s'. Will be ignored for configuration loading!"%line.strip())
        success =  self.interface.SetTextConfiguration(  ["RESC=YES"] + self.sort_commands(cmds, vals) )
        if not success:
            self.set_state(tango.DevState.ALARM)
            self.error_stream("Failed Setting the text configuration!")

    @command(dtype_in=str)
    def SaveConfigurationFile(self, filepath):
        if os.path.exists(filepath):
            raise RuntimeError("File already exists!")   
        if not os.path.exists(os.path.dirname(filepath)):
            raise RuntimeError("Directory of path does not exist!")   

        with open(filepath, "w+") as f:
            for _,param_dict in self.parameter_dicts.items():
                for cmd, setting in param_dict.items():
                    if setting !="N/A":
                        f.write( "{:s}={:s};\n".format(cmd, setting["value"]) )

    @command(dtype_in=str)
    def SaveSpectrumAsMCA(self, filepath):
        if os.path.exists(filepath):
            self.set_state(tango.DevState.ALARM)
            raise RuntimeError("File already exists!")   
        if not os.path.exists(os.path.dirname(filepath)):
            self.set_state(tango.DevState.ALARM)
            raise RuntimeError("Directory of path does not exist!")   

        with open(filepath, "w+") as f:
            f.write( self.create_pmca_spectrum_string() )
            f.write( self.create_calibration_string() )
            f.write( self.create_data_string() )
            f.write( self.create_dp5_config_string() )
            f.write( self.create_dpp_status_string() )


    def set_maxinfoage(self, x):
        self._max_info_age = x 
    
    def get_maxinfoage(self):
        return self._max_info_age
    
    @command(dtype_in = float, dtype_out=float)
    def GetDeadtime(self, max_age_ms):
        return self.interface.DeadTime( max_age_ms )

    @command(dtype_in = float, dtype_out=int)
    def GetSlowCount(self, max_age_ms):
        return self.interface.SlowCount( max_age_ms )

    @command(dtype_in = float, dtype_out=int)
    def GetFastCount(self, max_age_ms):
        return self.interface.FastCount( max_age_ms )

    @command(dtype_in=float, dtype_out=(int,))
    def GetSpectrum(self, max_age_ms):
        return self.interface.GetSpectrum( max_age_ms)

    @command(dtype_in = float, dtype_out=float)
    def GetAcctime(self, max_age_ms):
        return self.interface.AccTime( max_age_ms )

    @command(dtype_in = float, dtype_out=float)
    def GetRealtime(self, max_age_ms):
        return self.interface.RealTime( max_age_ms )

    @command(dtype_in = float, dtype_out=float)
    def GetHighVoltage(self, max_age_ms):
        return self.interface.HighVoltage( max_age_ms )

    @command(dtype_in = float, dtype_out=float)
    def GetDetectorTemp(self, max_age_ms):
        return self.interface.DetectorTemp( max_age_ms )

    @command(dtype_in = float, dtype_out=float)
    def GetBoardTemp(self, max_age_ms):
        return self.interface.BoardTemp( max_age_ms )

    @command(dtype_in = float, dtype_out=bool)
    def IsPresetTimeReached(self, max_age_ms):
        return self.interface.IsPresetTimeReached( max_age_ms )

    @command(dtype_in = float, dtype_out=bool)
    def IsEnabled(self, max_age_ms):
        return self.interface.IsEnabled( max_age_ms )

    
    @command(dtype_in = float, dtype_out=bool)
    def IsGated(self, max_age_ms):
        return self.interface.IsGated( max_age_ms )

    @command(dtype_in = float, dtype_out=bool)
    def IsPresetCountReached(self, max_age_ms):
        return self.interface.IsPresetCountReached( max_age_ms )
    
    @command(dtype_in = float, dtype_out=float)
    def GetTecVoltage(self, max_age_ms):
        return self.interface.TecVoltage( max_age_ms )



    def set_calibrationslope(self, val):
        self._calibrationslope = val

    def set_calibrationoffset(self, val):
        self._calibrationoffset = val

    def get_firmwaremajor(self):
        return self.interface.FirmwareMajor()

    def get_firmwareminor(self):
        return self.interface.FirmwareMinor()

    def get_firmwarebuild(self):
        return self.interface.FirmwareBuild()

    def get_fpgamajor(self):
        return self.interface.FpgaMajor()

    def get_fpgaminor(self):
        return self.interface.FpgaMinor()

    def get_serialnb(self):
        return self.interface.SerialNb()

    
    def get_fpgaclock(self):
        return self.interface.FpgaClock()

    def get_devtype(self):
        devId = self.interface.DeviceType()
        if (devId ==0):
            return "DP5"
        elif (devId ==1):
            return "PX5"
        elif (devId ==2):
            return "DP5G"
        elif (devId ==3):
            return "MCA8000D"
        if (devId ==4):
            return "TP5"
        if (devId ==5):
            return "DP5-X"
        else:
            return "UNKNOWN"


    def get_presetacctime(self):
        try:
            return float( self.__find_config("PRET") )
        except:
            return -1

    def get_presetrealtime(self):
        try:
            return float( self.__find_config("PRER") )
        except:
            return -1

    def set_presetacctime(self, t):
        self.interface.SetPresetAccumulationTime(t)

    def set_presetrealtime(self, t):
        self.interface.SetPresetRealTime(t)



    def dev_state(self):
        if Device.dev_state(self) == tango.DevState.ALARM:
            return tango.DevState.ALARM
            
        enabled = self.IsEnabled(self._max_info_age)
        if enabled:
            self.set_state(tango.DevState.MOVING) 
        else:
            self.set_state(tango.DevState.ON)
        return Device.dev_state(self)

    def sort_commands(self, parameters, parametervalues):
        command_strings = []
        #make copy of list since it will be emptied
        parameters = list(parameters)
        parametervalues = list(parametervalues)
        print( "Params in Order: ", self.param_names_in_correct_order )
        print( "Change parameters: ", parameters )
        for cmd in self.param_names_in_correct_order:
            try:
                idx = parameters.index(cmd)
                print( "index of %s is %d"%(cmd, idx) )
                p = parameters.pop(idx)
                v = parametervalues.pop(idx)
                command_string = "%s=%s"%(p, v)
                if v != "" and not self.__find_parameter(cmd).get("readonly", False):
                    command_strings.append( command_string )
            except Exception as e:
                print(e)
                pass #config parameter is not part of the commands
        for cmd,val in zip(parameters, parametervalues):
            if cmd == "RESC":
                print("RESC command cannot be given as part of the parameters. Will be prepended when loading a file")
                continue
            if cmd in self.param_names_in_correct_order:
                print( "command %s already added. (Value: %s). Ignored"%(cmd, val) )
                continue
            print("command %s not found. (Value: %s)"%(cmd, val))
            if val != "":
                command_strings.append("%s=%s"%(cmd,val))

        return command_strings



    def create_dpp_status_string(self):
        template = ("<<DPP STATUS>>\n"
                    "Device Type: {dev_type}\n"
                    "Serial Number: {serial_nb}\n"
                    "Firmware: {fw_major}.{fw_minor}  Build:  {fw_build}\n"
                    "FPGA: {fpga_major}.{fpga_minor}\n"
                    "Fast Count: {fastcount}\n"
                    "Slow Count: {slowcount}\n"
                    "GP Count: 0\n"
                    "Accumulation Time: {acc_time:.5f}\n"
                    "Real Time: {real_time:.5f}\n"
                    "Dead Time: {dead_time_pct:.2f}%\n"
                    "HV Volt: {high_volt:.1f}V\n"
                    "TEC Temp: {detector_temp:.1f}K\n"
                    "Board Temp: {board_temp:.1f}C\n"
                    "<<DPP STATUS END>>\n")
        self.interface.UpdateStatus()
        tmp_max_info_age =  self._max_info_age 
        self._max_info_age = 10000000
        try:
            outstring =  template.format( dev_type = self.get_devtype(),
                                serial_nb = self.get_serialnb(),
                                fw_major = self.get_firmwaremajor(),
                                fw_minor = self.get_firmwareminor(),
                                fw_build = self.get_firmwarebuild(),
                                fpga_major = self.get_fpgamajor(),
                                fpga_minor = self.get_fpgaminor(),
                                fastcount = self.GetFastCount(self._max_info_age),
                                slowcount = self.GetSlowCount(self._max_info_age),
                                dead_time_pct = self.GetDeadtime(self._max_info_age),
                                acc_time=self.GetAcctime(self._max_info_age),
                                real_time = self.GetRealtime(self._max_info_age),
                                dead_time = self.GetDeadtime(self._max_info_age),
                                high_volt = self.GetHighVoltage(self._max_info_age),
                                detector_temp = self.GetDetectorTemp(self._max_info_age),
                                board_temp = self.GetBoardTemp(self._max_info_age)
                            )
        except:
            self._max_info_age = tmp_max_info_age
            raise
        self._max_info_age = tmp_max_info_age
        return outstring


    def create_dp5_config_string(self):
        outstring = "<<DP5 CONFIGURATION>>\n"
        for _,param_dict in self.parameter_dicts.items():
                for cmd, setting in param_dict.items():
                    if setting !="N/A":
                        outstring += "{:s}={:s};\n".format(cmd, setting["value"]) 
        outstring += "<<DP5 CONFIGURATION END>>\n"
        return outstring


    def create_pmca_spectrum_string(self):
        return ""

    def create_calibration_string(self):
        return ""

    def create_data_string(self):
        outstring = "<<DATA>>\n"
        outstring += "\n".join([str(c) for c in self.GetSpectrum(self._max_info_age)] ) + "\n"
        outstring += "<<END>>\n"
        return outstring

if __name__ == "__main__":
    AmptekPX5.run_server()

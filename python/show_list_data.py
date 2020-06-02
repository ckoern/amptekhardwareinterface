

import sys 
import struct 
import numpy as np 

def read_records(f,datalength):
    records = []
    while(datalength >=4):
        f1,ts = struct.unpack(">HH",f.read(4))
        datalength -= 4
        d1 = f1 >> 15
        d2 = f1 >> 14
        ampl = f1 & 0b11111111111111
        record = {}
        if d1 == 0: #event
            record['type'] = 'event'
            record['amplitude'] = ampl
            record["ts_lower"] = ts 
        elif d2 == 0: #32-bit timtag
            record['type'] = 'timetag'
            record["ts_lower"] = ts 
            record["ts_upper"] = ampl
        else: #frame = timetag
            record['type'] = 'frame'
            frame_count = (ampl << 2) + (ts >> 14)
            ts_upper = ts & 0b11111111111111
            record["ts_upper"] = ts_upper 
            record["frame"] = frame_count
        records.append(record)
    return records

def read_packet(f):
    try:
        pid1 = struct.unpack("B",f.read(1) )[0]
        pid2 = struct.unpack("B",f.read(1) )[0]
        datalength = struct.unpack(">H",f.read(2))[0]
    except:
        return None
    packet = {}
    if pid1 != 0x82:
        return None
    if pid2 == 0x0A:
        packet["full"] = False 
    elif pid2 == 0x0B:
        packet["full"] = True
    else:
        return None 

    if datalength == 0:
        records = []
    else:
        records = read_records(f,datalength)
    packet["records"] = records
    return packet
    


with open( sys.argv[1] , "rb") as f:
    data = []
    amplitudes = []
    packet_overflow_indices = []
    packet_indices = []
    records = []
    i = 0
    while(True):
        packet = read_packet(f)
        i += 1
        if i == 1:
            continue
        
        if packet is None:
            break
        if packet["full"]:
            packet_overflow_indices.append(len(data))
        else:
            packet_indices.append(len(data))
        
        for record in packet["records"]:
            records.append(record)
            data.append( ["event","timetag", "frame"].index(record["type"]) )
            if data[-1] == 0:
                amplitudes.append( record["amplitude"] )

import matplotlib.pyplot as plt 
fig, ax = plt.subplots()
ax.plot( data )
for p in packet_indices:
    ax.axvline(p, color = "tab:green")
for p in packet_overflow_indices:
    ax.axvline(p, color = "tab:red")

print( sum( [1 for d in data if d == 0] ) )




for i in range(1,len(records)):
    record = records[i] 
    last_record = records[i-1]
    shared_keys = ["ts_upper", "frame"]
    for shared_key in shared_keys:
        if shared_key not in record:
            if shared_key in last_record:
                record[shared_key] = last_record[shared_key]

    if "ts_upper" in record and "ts_lower" in record and "frame" in record :
        record["full_ts"] = (record["ts_upper"] << 16) + record["ts_lower"]

fig2, ax2 = plt.subplots() 
event_records = [r for r in records if r["type"] == "event"]
t = [ r["full_ts"] for r in event_records if "full_ts" in r ]
a = [ r["amplitude"] for r in event_records if "full_ts" in r ]
ax2.plot(t,a)

figWF, axesWF = plt.subplots(2,1,sharex=True)
h2D, tedges, xedges = np.histogram2d( t,a,bins=(1000,1024), range=( (0,max(t)),(0,4096) ) )
print(h2D.shape)
axesWF[0].plot( h2D[-1] )
axesWF[1].imshow(h2D, extent = (0,1024,tedges[0],tedges[-1]), aspect = "auto", origin = "lower" )

fig3, ax3 = plt.subplots() 

event_records = [r for r in records if r["type"] == "event"]
t = [ r["full_ts"] for r in event_records if "full_ts" in r ]
a = [ r["frame"] for r in event_records if "full_ts" in r ]
ax3.plot(t,a)


# for i in range(len(records)):
#     if records[i]['type'] == 'frame':
#         print( "{:03d}:{:06d}".format( records[i]["frame"],records[i]["ts_upper"] ) )
plt.show()





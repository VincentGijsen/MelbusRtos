import serial
from time import sleep
import threading

ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=115200,
        bytesize=8, timeout=4, stopbits=serial.STOPBITS_ONE)

def registerAVRNotify(deviceid, event, p1, p2, p3, p4):
    data = [
        49, 0, 0, 5, event, p1, p2, p3, p4
    ]
    sendPacketArrayInt(11, 11, deviceId, data )

def sendPkg(cmd,  data):
    ser.write(b'\xaa')
    ser.write(b'\x00')
    ser.write(b'\x02') 
    chk = 2
    ser.write(chr(cmd))
    print ('1')
    chk = chk + cmd
    print("2")
    ser.write(chr(data))
    chk = chk + (data)
    ser.write(chr(0x100 - chk))

def sendPacketArrayInt( packetSize, cmd, deviceId,  data):
  ser.write(b'\xAA')
  msb = packetSize >> 8
  lsb = packetSize & 255
  ser.write(msb)
  chk = msb + lsb +cmd + deviceId
  ser.write(lsb)
  ser.write(cmd)
  
  ser.write(deviceId)
  

  for x in range(0 ,packetSize -2):
    #x=0
    ser.write(data[x])
    chk = chk +data[x]

  ser.write(256 - chk)
  print('done')


def printAsHex(prefix, input):
    res = ' '.join(format(x, '02x') for x in input)
    res = prefix + ' ' + res + "\n"
    print(res)

def handleEventAVRCP(event):
    devId = event[1]
    paramlength = (event[10]) << 8 + event[11]
    switcher_EVENT_STATUS = {
        0: "STOPPED",
        1: "PLAYING",
        2: "PAUSED",
        3: "FWD_SEEK",
        4: "REV_SEEK",
        255: "ERROR"
    }
    
    AVRCP_REGISTER_NOTIFICATION = 49
    AVRCP_EVENT_STATUS_CHANGED = 1
    print("HANDLING AVRCP")
    if event[8] == AVRCP_REGISTER_NOTIFICATION:
        if event[12] == AVRCP_EVENT_STATUS_CHANGED:
            print("AVRCP_EVENT_STATUS_CHANGED: %s" % switcher_EVENT_STATUS.get(event[13],"ERROR"))


def decodeEvent(evt):
    #print("got event: ")
    switcher = {
        b'\x00': "ACK",
        1: "BTM,status",
        2: "Call Status",
        3: "Caller_id",
        4: "SMS_REceived",
        5: "Missed call indicator",
        6: "PhoneMaxBatt",
        7: "PhoneCurrBatLevel",
        8: "Roaming-status",
        9: "PhoneMax signal level:",
        10: "PhonecurrentSignallevel",
        26 :"AVRCP Specific response",
        
    }
    print("Event Type: %s" % switcher.get(evt[0], "unkown"))

    if evt[0] == 26:
        handleEventAVRCP(evt)


def thread_function(name):
    print("starting trhead")
    event = []
    print("arr has been delcared")
    while True:
            b = ser.read()
            event = []

            if b != None:
                #print('got char')
                if b == b'\xaa':
                        #valid byte
                        print("start of event")
                        l = ser.read(2)
                        #print(l)
                        length = (l[0] << 8) + l[1] 
                        #print('length: %d' % length)
                        #+ (chr(ser.read()) & 255) 
                        #event.append(ser.read(length +1))
                        for x in range(0, length + 1):
                            #print('x: %d' %x)
                            event.append(ord(ser.read()))
                            #sleep(0.1)
                        print("decoding")
                        decodeEvent(event)
                        printAsHex("Event", event)
                #if len(read_byte) > 0 and read_byte != None:
                #    print('%x - %s' % (ord(read_byte), read_byte) )
            sleep(0.1)


if __name__ == "__main__":
     

    print("starting thread...")
    x = threading.Thread(target=thread_function, args=(1,))
    x.start()
    print("trying to open port: " + ser.name)
    #ser.open()
    #ser.write('abc')

    CMD_Read_Link_Status = b'\x0D'
    DUMMY_BYTE = 0
    #hex 34
    payload = 52
    data = [payload]
    deviceId = 0    
    CMD_Music_Control = 2



    #sendPkg(CMD_Read_Link_Status, DUMMY_BYTE)
    
    #next track
    #sendPacketArrayInt(3, CMD_Music_Control, deviceId, data);
    
    #list_playerAttributes:
    #p = [17, 0, 0, 1, 0]
    #sendPacketArrayInt(7, 11, deviceId, p);
    
    #query connected device name
    #p = [0]
    #print("sending dev-name query")
    #sendPkg(16, DUMMY_BYTE)
    
    #query connected-dev-items
    #p = [0]
    #sendPacketArrayInt(3, 22, deviceId, p);
    

    #p = [15]
    #sendPacketArrayInt(3, 2, deviceId, p);
    sleep(0.5)
    print("AVRCP Now Playing changed")
    registerAVRNotify(1,9,0,0,0,0) #conent changed
    sleep(0.5)
    print("AVRCP Track Changed")
    registerAVRNotify(1,2,0,0,0,0) #track-changed
    sleep(0.5)


    #get player
    attribute = 1
    setting = 1
    sendPacketArrayInt(7, 11, 0, [22, 0, 0, 1, attribute, 1, setting ] )
    #ser.write(b'\xAA\x00\x03\x30\x00\x00\xCD')
    sleep(2)
    
    
    x.join()
    print("closing port")
    ser.close()




import serial
from time import sleep
import threading

ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=115200,
        bytesize=8, timeout=1, stopbits=serial.STOPBITS_ONE)

def registerAVRNotify(deviceid, event, p1, p2, p3, p4):
    getEvents()
    print("registering notify")
    data = [
        49, 0, 0, 5, event, p1, p2, p3, p4
    ]
    sendPacketArrayInt(11, 11, deviceId, data )
    
    
def sendPkg(cmd,  data):
    getEvents()
    print("send pkg")
    ser.write(b'\xAA')
    ser.write(0)
    ser.write(2) 
    chk = 2
    ser.write(cmd)
    chk = chk + cmd
    print("2")
    ser.write(data)
    chk = chk + (data)
    ser.write(256 - chk)

    #getEvents()

def sendPacketArrayInt( packetSize, cmd, deviceId,  data):
  getEvents()
  #print("SendPKG arrayInt len: %d" % packetSize)
  ser.write(b'\xAA')
  msb = packetSize >> 8
  lsb = packetSize & 255
  #print("msb %d , lsb %d" %(msb, lsb))
  ser.write(msb)
  chk = packetSize+cmd + deviceId
  ser.write(lsb)
  ser.write(cmd)
  
  ser.write(deviceId)
  
  #print("PKG size: %d" % packetSize)

  for x in range(0 ,packetSize -2):
    #x=0
    ser.write(data[x])
    chk = chk +data[x]

  ser.write(256 - (chk % 256))
  print('done')


def printAsHex(prefix, input):
    res = ' '.join(format(x, '02x') for x in input)
    res = prefix + ' ' + res + "\n"
    print(res)

def requesMeta():
    getEvents()
    data=[32] #pdu
    data = data + [0] #reserved
    data = data + [0,17] #param len
    data = data + [0,0,0,0,0,0,0,0] # identifier playing
    data = data + [2] #atribute count
    data = data + [0,0,0,1] #attrib 1title of media
    data = data + [0,0,0,2] #attrib 2 playingtime
    #print(data)
    #print("sending track-requiest cmd, data len %d" % len(data))
    printAsHex("send to BT REQ:", data)
    pkgSize = len(data) + 2
    #print("META pkg size: $")
    sendPacketArrayInt(pkgSize, 11,0,data)

def handleEventAVRCP(event):
    getEvents()
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
    
    AVRCP_REGISTER_NOTIFICATION = 49 #hex31
    AVRCP_EVENT_STATUS_CHANGED = 1
    print("HANDLING AVRCP")
    if event[8] == AVRCP_REGISTER_NOTIFICATION:
        if event[12] == AVRCP_EVENT_STATUS_CHANGED:
            print("AVRCP_EVENT_STATUS_CHANGED: %s" % switcher_EVENT_STATUS.get(event[13],"ERROR"))
            if event[13] == 1:
                #request meta
                #requesMeta()
                True
        if event[12] == 3:
            print("AVRCP track end")

        if event[12] == 5:
            pos = (event[13] << 24 ) + (event[14] << 16) + (event[15] << 8) + event[16]
            print("POS changed: %d" + pos)
    else:
        printAsHex("BTM send SPECIAL AVRC",event)

def decodeEvent(evt):
    #print("got event: ")
    switcher = {
        0: "ACK",
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
        32 :"Title response?",
        41 : "ABS vol control"
        
    }
    print("BTM send: %s" % switcher.get(evt[0], "unkown"))
    printAsHex("Received Event", evt)
    if evt[0] == 26: #hex 49 see spec http://read.pudn.com/downloads780/doc/3087534/AVRCP_SPEC_V15.pdf
        handleEventAVRCP(evt)

def getEvents():
    #sleep(0.01)
    b = ser.read(1)
    event = []
    if b != None:
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
                #print("decoding")
                decodeEvent(event)
                
        #if len(read_byte) > 0 and read_byte != None:
        #   print('%x - %s' % (ord(read_byte), read_byte) )
        #else:
        #    print("received invalid dataz")
        #    printAsHex('invalid byte:', b)


def thread_function(name):
    print("starting trhead")
    event = []
    print("arr has been delcared")
    while True:
            getEvents()
            #sleep(0.01)


if __name__ == "__main__":
     
    #ser.open()
    
    print("starting thread...")
    #print("trying to open port: " + ser.name)
    ##ser.write('abc')

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
    print("sending dev-name query")
    sendPkg(16, DUMMY_BYTE)
    getEvents()
    #sleep(0.1)
    #r = ser.read(4)
    #printAsHex("dev name: ", r)
    #sleep(0.1)
    
    #r = ser.read(10)
    #printAsHex("dev name: ", r)
    

    #setting event mask
    #sendPacketArrayInt(6, 3,0,[252, 15, 2, 0])
    #query connected-dev-items
    #p = [0]
    #sendPacketArrayInt(3, 22, deviceId, p);
    #

    #p = [15]
    #sendPacketArrayInt(3, 2, deviceId, p);
    #sleep(0.5)
    #print("AVRCP Now Playing changed")
    #registerAVRNotify(1,9,0,0,0,0) #now-playing contentt changed
    #sleep(0.5)
    #print("AVRCP Track Changed")
    #registerAVRNotify(1,2,0,0,0,0) #track-changed
    
    
    
    #registerAVRNotify(1,3,0,0,0,0) #end-of-track
    #registerAVRNotify(1,4,0,0,0,0) #start-of-track
    #registerAVRNotify(1,5,0,0,19,136) #playback-pos-change
    #registerAVRNotify(1,6,0,0,0,0) #batt chanage
    #registerAVRNotify(1,8,0,0,0,0) #player-app-changed
    
    #query available players
    #data = [49]
    #data = data +[0] #res
    #data = data +[0,5] #param-len
    #data = data + [10] #event_id
    #data = data + [0,0] #res

    #dleng= len(data) +2
    #sendPacketArrayInt(dleng, 11,0,data)


    #sleep(2.5)
    #try query for artist stuff
    requesMeta()

    #get player
    attribute = 1
    setting = 1
    #sendPacketArrayInt(7, 11, 0, [22, 0, 0, 1, attribute, 1, setting ] )
    #ser.write(b'\xAA\x00\x03\x30\x00\x00\xCD')
    #sleep(2)
    
    getEvents()
    x = threading.Thread(target=thread_function, args=(1,))
    x.start()
    
    x.join()
    print("closing port")
    ser.close()




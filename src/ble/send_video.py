import btfpy
import os

# must hard-code the phone's MAC in devices.txt and set it to node 4
# for example:
# DEVICE=MOTO TYPE=classic node=4 ADDRESS = A0:46:5A:0B:9F:86

def init_bluetooth():
    if btfpy.Init_blue("devices.txt") == 0:
        exit(0)

    randadd = [0xD3, 0x56, 0xDB, 0x24, 0x32, 0xA0]
    btfpy.Set_le_random_address(randadd)
    btfpy.Set_le_wait(5000)
    btfpy.Le_pair(btfpy.Localnode(), btfpy.JUST_WORKS, 0)

def sendfileobex(node,filname):
  connect = [0x80,0x00,0x07,0x10,0x00,0x01,0x90]
  disconnect = [0x81,0x00,0x03]

  # convert to bytes object
  if isinstance(filname,str) == True:
    filename = filname.encode()
  else:
    filename = filname

  # strip this machine directory from filename
  sgs = filename.split(b'/')   # linux directory
  if(len(sgs) > 1):
    fname = sgs[len(sgs)-1]
  else:
    fname = filename

  print("Sending " + filename.decode() + " to " + fname.decode())

  # open file
  try:
    file = open(filename,'rb')
  except:
    print("File open error")
    return(0)

  # find file length
  file.seek(0,os.SEEK_END)
  flen = file.tell()
  file.seek(0)

  ntogo = flen
  nlen = len(fname)

  nblock = 400
  connect[5] = (nblock >> 8) & 0xFF
  connect[6] = nblock & 0xFF
  send = [0 for n in range(nblock)]

  # OBEX connect
  btfpy.Write_node(node,connect,0)

  # wait for Success reply 0x0A
  inbuf = btfpy.Read_node_endchar(node,btfpy.PACKET_ENDCHAR,btfpy.EXIT_TIMEOUT,5000)
  if(len(inbuf) == 0 or inbuf[0] != 0xA0):
    print("OBEX Connect failed")
    file.close()
    return(0)
  elif((inbuf[1] << 8) + inbuf[2] >= 7):
    n = (inbuf[5] << 8) + inbuf[6]
    if(n < nblock):
      nblock = n  # reduce chunk size

  send[3] = 0x01
  n = 2*nlen + 5
  send[4] = (n >> 8) & 0xFF
  send[5] = n & 0xFF
  k = 6
  for n in range(nlen):
    send[k] = 0
    send[k+1] = fname[n]
    k = k + 2

  send[k] = 0
  send[k+1] = 0
  k = k + 2

  send[k] = 0xC3
  send[k+1] = (flen >> 24) & 0xFF
  send[k+2] = (flen >> 16) & 0xFF
  send[k+3] = (flen >> 8) & 0xFF
  send[k+4] = flen & 0xFF
  k = k + 5
  err = 0
  # loop to send data chunks
  while(ntogo > 0 and err == 0):
    if(ntogo <= nblock - 3 - k):
      send[k] = 0x49
      send[0] = 0x82
      ndat = ntogo + 3
    else:
      send[k] = 0x48
      send[0] = 0x02
      ndat = nblock - k
    send[k+1] = (ndat >> 8) & 0xFF
    send[k+2] = ndat & 0xFF
    k = k + 3
    ndat = ndat - 3

    try:
      temps = file.read(ndat)
    except:
      err = 1

    if err == 0:
      for n in range(ndat):
        send[k+n] = temps[n]
      ntogo = ntogo - ndat
      k = k + ndat
      send[1] = (k >> 8) & 0xFF
      send[2] = k & 0xFF
      btfpy.Write_node(node,send,k)  # send k bytes
      # wait for Success reply 0x0A
      inbuf = btfpy.Read_node_endchar(node,btfpy.PACKET_ENDCHAR,btfpy.EXIT_TIMEOUT,5000)
      if(len(inbuf) == 0 or (inbuf[0] != 0xA0 and inbuf[0] != 0x90)):
        print("Send failed")
        err = 1

    k = 3
    # end chunk loop

  file.close()

  btfpy.Write_node(node,disconnect,0)
  # wait for Success reply 0x0A
  inbuf = btfpy.Read_node_endchar(node,btfpy.PACKET_ENDCHAR,btfpy.EXIT_TIMEOUT,5000)
  if(len(inbuf) == 0 or inbuf[0] != 0xA0):
    print("OBEX Disconnect failed")

  return(1)
  # end sendfileobex


init_bluetooth()

channel = btfpy.Find_channel(4,btfpy.UUID_2,btfpy.Strtohex("1105"))
if(channel <= 0):
  channel = btfpy.Find_channel(4,btfpy.UUID_16,btfpy.Strtohex("00001105-0000-1000-8000-00805F9B34FB"))
if(channel <= 0):
  print("OBEX service not found")
  btfpy.Close_all()
  exit(0)

btfpy.Connect_node(4,btfpy.CHANNEL_NEW,channel)

sendfileobex(4, "devices.txt")

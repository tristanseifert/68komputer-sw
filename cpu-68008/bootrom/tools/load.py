#!/usr/bin/env python3
################################################################################
# Loads a binary via serial.
################################################################################
import progressbar
import serial
from termcolor import colored, cprint
import sys
import os

################################################################################
# waits for an acknowledge
def waitForAck(port):
    # try to read a char
    ret = port.read(1)

    # handle not receiving anything
    if len(ret) is not 1:
        cprint('Failed to receive acknowledge!', 'red')
        sys.exit(-1)
    # ensure it was a dot for ack
    elif ret[0] is not 0x2e:
        cprint('Received 0x{char:02x} but expected "." as ack'.format(char=ret[0]))
        sys.exit(-1)

    # we good

################################################################################
# check arguments
if len(sys.argv) is not 5:
    print('usage: {exec} [binary path] [load address] [serial port] [baud rate]'.format(exec=sys.argv[0]))
    sys.exit(-1)

# get some infos
loadAddr = int(sys.argv[2], 0)
size = os.path.getsize(sys.argv[1])

# calculate the checksum over the file
cprint('• Calculating file checksum', attrs=['bold'])

checksum = 0

with open(sys.argv[1], "rb") as f:
    byte = f.read(1)

    while byte:
        # just sum all bytes up
        checksum += ord(byte)
        # read the next byte
        byte = f.read(1)

# open and configure the serial port with timeout of 1sec
port = serial.Serial(sys.argv[3], int(sys.argv[4]), timeout=1)

# send the load address and wait for an ack
cprint('• Sending load address: ${addr:08x}'.format(addr=loadAddr), attrs=['bold'])
port.write('{load:08x}'.format(load=loadAddr).encode('ascii'))
waitForAck(port)

# send the checksum and wait for an ack
cprint('• Sending checksum: ${check:08x}'.format(check=checksum), attrs=['bold'])
port.write('{check:08x}'.format(check=checksum).encode('ascii'))
waitForAck(port)

# send the file size
cprint('• Sending file size: {size} bytes'.format(size=size), attrs=['bold'])
port.write('{size:08x}'.format(size=size).encode('ascii'))
waitForAck(port)

# then, send each byte of the file
cprint('• Sending payload', attrs=['bold'])

with progressbar.ProgressBar(max_value=size) as bar:
    with open(sys.argv[1], "rb") as f:
        byte = f.read(1)
        counter = 0

        while byte:
            # send it and demand an ack
            port.write('{data:02x}'.format(data=ord(byte)).encode('ascii'))
            waitForAck(port)

            # read the next byte
            byte = f.read(1)

            # update progress
            counter += 1
            bar.update(counter)

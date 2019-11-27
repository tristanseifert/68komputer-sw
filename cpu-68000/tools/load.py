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
# block size
kBlockSize = 256

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

# load the entire file into memory and pad it to multiples of 128 bytes
data = open(sys.argv[1], "rb").read()
data = bytearray(data)

numPadRequired = (kBlockSize - (len(data) % kBlockSize))

if numPadRequired is not 0 and numPadRequired is not kBlockSize:
    for i in range(numPadRequired):
        data.append(0)

    cprint('• Added {added} bytes of padding to file'.format(added=numPadRequired), attrs=['bold'])
    cprint('\tBlock size is {block}'.format(block=kBlockSize))


# get some infos
loadAddr = int(sys.argv[2], 0)
size = len(data)

# calculate the checksum over the file
cprint('• Calculating file checksum', attrs=['bold'])

checksum = 0

for byte in data:
    # just sum all bytes up
    checksum += byte

cprint('\tChecksum: ${check:08x}'.format(check=checksum))

# open and configure the serial port with timeout of 1sec
port = serial.Serial(sys.argv[3], int(sys.argv[4]), timeout=0.5)

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

# then, send each block of the file
cprint('• Sending payload', attrs=['bold'])
counter = 0

with progressbar.ProgressBar(max_value=size) as bar:
    for blockOff in range(0, size, kBlockSize):
        # get block of data, send it and wait for acknowledgement
        block = data[blockOff:(blockOff+kBlockSize)]
        port.write(bytes(block))
        waitForAck(port)

        # update the progress bar
        counter += kBlockSize
        bar.update(counter)

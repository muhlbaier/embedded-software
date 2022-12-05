#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# PYTHON 3.x script
#-------------------------------------------------------------------------------

from __future__ import print_function
import sys
import os
import time
import binascii
import datetime
import logging
import struct
from binascii import unhexlify
import zlib


#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# functions
#-------------------------------------------------------------------------------
def delay_us(n):
    time.sleep(n / 1000000.)

def delay_ms(n):
    time.sleep(n / 1000.)


def int_to_bytes(val, num_bytes):
    return [(val & (0xff << pos*8)) >> pos*8 for pos in range(num_bytes)]

def getcsum(fdata, fsize):
    csum = 0;
    n = 0;
    for k in range(0,fsize/4):
        cbyte = fdata[n + 0];
        csum = csum + cbyte*(2**24);
        cbyte = fdata[n + 1];
        csum = csum + cbyte*(2**16);
        cbyte = fdata[n + 2];
        csum = csum + cbyte*(2**8);
        cbyte = fdata[n + 3];
        csum = csum + cbyte;
        n = n + 4;
        if(csum > 0x100000000):
            csum = csum - 0x100000000;                      # print("%d " %(k*4 , " %08X" %csum);
    csum = 0xFFFFFFFF - csum;
    csum = csum  + 1;
    if(csum > 0xFFFFFFFF):
        csum = csum - 0x100000000;
    return csum;
def add32bit(data, idx, value):
    tdata = struct.pack(">I", value);
    data[idx] = tdata[0];
    data[idx+1] = tdata[1];
    data[idx+2] = tdata[2];
    data[idx+3] = tdata[3];
    return data;

def readfwv(fname):
    return fwver;




print("\nwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww")
print("w FWINFO Python3 script v1.0                           w")
print("wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww")

fwver = 0;
fid = open("fwver.c", 'r+');
data = bytearray(fid.read(),encoding='utf8');
fid.seek(0, os.SEEK_SET);
size = int(len(data));
idx = 0;
fwver = 0x00000000;

try:
    idx = data.index(b'0x');
    idx = idx + 2;
    for k in range(0,8):
        if(data[k+idx] >= 65):
            fwver = fwver + (data[k+idx] - 65 + 10)*(2**((7-k)*4))
        else:
            fwver = fwver + (data[k+idx] - 48)*(2**((7-k)*4))
    # uncomment this on the github side as part of the git action
    fwver = fwver + 1;
except Exception as e:
    print("File empty\n");

datetime = int(time.time());

print("-> FWINFO: ver %s" %format(fwver, '08X'), " time %s" %format(datetime, '08X'))

# write *.c file
fid.write('#include "fwver.h"\n\n');
fid.write('const uint32_t FIRMWARE_VERSION = 0x%s;\n' %format(fwver, '08X'));
fid.write('const uint32_t BUILD_TIME = 0x%s;\n\n' %format(datetime, '08X'));

fid.close();


fid = open("fwver.h", 'w');
# write *.h file
fid.write('#ifndef __FWVER_H__\n');
fid.write('#define __FWVER_H__\n\n');
fid.write('#include <stdint.h>\n\n');
fid.write('extern const uint32_t FIRMWARE_VERSION;\n');
fid.write('extern const uint32_t BUILD_TIME;\n\n');
fid.write('#endif\n');

fid.close();





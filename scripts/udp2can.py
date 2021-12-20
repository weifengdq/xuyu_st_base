import socket
import struct
import can
import time

bus0 = can.interface.Bus(bustype='socketcan', channel='vxcan0', bitrate=500000, data_bitrate=2000000, fd=True)
bus1 = can.interface.Bus(bustype='socketcan', channel='vxcan1', bitrate=500000, data_bitrate=2000000, fd=True)

MCAST_GRP = '239.255.255.252'
MCAST_PORT = 20000
IS_ALL_GROUPS = True

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
if IS_ALL_GROUPS:
    sock.bind(('', MCAST_PORT))
else:
    sock.bind((MCAST_GRP, MCAST_PORT))
mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)

sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

while True:
    r_data = sock.recv(10240)

    # device identifier, 00-mcu, 01-pc
    if r_data[41]==0x01:
        continue

    i = 48

    while i < len(r_data):
        can_int = int().from_bytes(r_data[i:i+4], byteorder='little', signed=False)
        can_id  = can_int & 0x1FFFFFFF
        can_ext = 1 if (can_int & 0x80000000) else 0
        can_len = r_data[i+4]
        can_brs = r_data[i+5] & 0x01
        can_esi = (r_data[i+5]>>1) & 0x01
        can_fd  = (r_data[i+5]>>2) & 0x01
        can_road= r_data[i+6]
        can_data= r_data[i+8 : i+8+can_len]

        msg = can.Message(arbitration_id=can_id, \
                data=can_data, \
                is_extended_id=can_ext, \
                is_fd=can_fd, \
                bitrate_switch=can_brs, \
                error_state_indicator=can_esi)
        if (can_road==0):
            bus0.send(msg)
        elif (can_road==1):
            bus1.send(msg)
        i = i + 8 + can_len
        # print(time.time(), can_road, msg)
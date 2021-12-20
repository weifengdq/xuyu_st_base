import socket
import time
import sys
import can
import asyncio
import _thread

from can import message

multicast_addr = ("239.255.255.252", 10000)
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# local_port=30000
# sock.bind(('', local_port))

bus0 = can.Bus(bustype='socketcan', channel='vxcan0', bitrate=500000, data_bitrate=2000000, fd=True)
bus1 = can.Bus(bustype='socketcan', channel='vxcan1', bitrate=500000, data_bitrate=2000000, fd=True)
# bus0 = can.Bus(bustype='socketcan', channel='vcan0', bitrate=500000, data_bitrate=2000000, fd=True)
# bus1 = can.Bus(bustype='socketcan', channel='vcan1', bitrate=500000, data_bitrate=2000000, fd=True)

# msg
# msg = bytearray(32+16+1152)
# package
someip_header = bytearray(32)
head = bytearray(16)
body = bytearray(1152)
# package control
refresh = 0    # indicate for new frame
flag = 0       # ping0-pong1
over = 0       # overflow
rollingcnt = 0

head[8] = 0x01  # device identifier, 00-mcu, 01-pc
udp_frame = someip_header + head

start = time.time()

def timer_callback(thread_name, delay):
    global start
    global refresh
    global udp_frame
    # print(time.time())
    # overtime send
    if ((time.time()-start)>(delay-0.0008)) & refresh:
        refresh = 0
        sock.sendto(udp_frame, multicast_addr)
        udp_frame = someip_header + head
        start = time.time()
    time.sleep(delay-0.0008)
    _thread.start_new_thread(timer_callback, ('thread0', 0.01))

# coroutine
def print_message(msg):
    global rollingcnt
    global udp_frame
    global start
    global over
    global head
    global refresh
    # print(msg)

    refresh = 1

    if(len(msg) > (32+16+1152-72)):
        over = 1
    # can_to_eth_package
    can_id = msg.arbitration_id
    can_data = msg.data
    can_ext = msg.is_extended_id
    can_len = len(msg.data)
    can_brs = msg.bitrate_switch
    can_fd  = msg.is_fd
    can_esi = msg.error_state_indicator
    can_road = 0 if (msg.channel=='vxcan0') else 1

    frame = (can_id | can_ext<<31).to_bytes(length=4, byteorder='little', signed=False) + \
        can_len.to_bytes(length=1, byteorder='little', signed=False) + \
        (can_brs | (can_esi<<1) | (can_fd<<2)).to_bytes(length=1, byteorder='little', signed=False) + \
        can_road.to_bytes(length=1, byteorder='little', signed=False) + \
        rollingcnt.to_bytes(length=1, byteorder='little', signed=False) + \
        can_data
    rollingcnt = (rollingcnt + 1) & 0xFF

    udp_frame = udp_frame + frame
    # print(frame)
    # print(udp_frame)
    # print()
    # overflow send
    if over:
        sock.sendto(udp_frame, multicast_addr)
        udp_frame = someip_header + head
        over = 0
        start = time.time()
        return

def delay_us(t):
    end = 0
    start = time.time()
    t = (t-3)/1000000
    while (end-start)<t:
        end = time.time()

async def main():
    reader = can.AsyncBufferedReader()
    logger = can.Logger('logfile.asc')
    listeners = [
        print_message,  # Callback function
        reader,         # AsyncBufferedReader() listener
        logger          # Regular Listener object
    ]

    loop = asyncio.get_event_loop()
    notifier0 = can.Notifier(bus0, listeners, loop=loop)
    notifier1 = can.Notifier(bus1, listeners, loop=loop)
    _thread.start_new_thread(timer_callback, ('thread0', 0.01))

    while True:
        await reader.get_message()
        # await asyncio.sleep(0.001)
        # delay_us(10)


loop = asyncio.get_event_loop()
loop.run_until_complete(main())
loop.close()
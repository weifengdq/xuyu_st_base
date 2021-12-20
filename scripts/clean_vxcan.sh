#!/bin/sh
sudo ip link set dev vcan0 down
sudo ip link set dev vcan1 down

sudo ip link set dev vxcan0 down
sudo ip link set dev vxcan1 down

sudo ip link delete dev vcan0 type vcan
sudo ip link delete dev vcan1 type vcan

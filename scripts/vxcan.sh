#!/bin/sh
sudo modprobe can_raw
sudo modprobe vxcan

sudo ip link add dev vcan0  type vxcan
sudo ip link add dev vcan1  type vxcan

sudo ip link set dev vcan0  up
sudo ip link set dev vcan1  up

sudo ip link set dev vxcan0  up
sudo ip link set dev vxcan1  up

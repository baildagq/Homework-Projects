#!/bin/bash
#####################################
# Author: stout                     #
# mail: 2817253603@qq.com           #
# Created Time: 2019-12-11 04:15:14 #
#####################################

# setup namespaces
sudo ip netns add PC1
sudo ip netns add R1
sudo ip netns add R2
sudo ip netns add R3
sudo ip netns add PC2

# link namespaces by connecting ports
sudo ip link add veth-PC1 type veth peer name veth-R1-1
sudo ip link add veth-R1-2 type veth peer name veth-R2-1
sudo ip link add veth-R2-2 type veth peer name veth-R3-1
sudo ip link add veth-R3-2 type veth peer name veth-PC2

# connect ports to namespaces
sudo ip link set veth-PC1 netns PC1
sudo ip link set veth-R1-1 netns R1
sudo ip link set veth-R1-2 netns R1
sudo ip link set veth-R2-1 netns R2
sudo ip link set veth-R2-2 netns R2
sudo ip link set veth-R3-1 netns R3
sudo ip link set veth-R3-2 netns R3
sudo ip link set veth-PC2 netns PC2

# activate ports
sudo ip netns exec PC1 ip link set veth-PC1 up
sudo ip netns exec R1 ip link set veth-R1-1 up
sudo ip netns exec R1 ip link set veth-R1-2 up
sudo ip netns exec R2 ip link set veth-R2-1 up
sudo ip netns exec R2 ip link set veth-R2-2 up
sudo ip netns exec R3 ip link set veth-R3-1 up
sudo ip netns exec R3 ip link set veth-R3-2 up
sudo ip netns exec PC2 ip link set veth-PC2 up

# configurate IP addresses
sudo ip netns exec PC1 ip addr add 192.168.1.2/24 dev veth-PC1
sudo ip netns exec R1 ip addr add 192.168.1.1/24 dev veth-R1-1
sudo ip netns exec R1 ip addr add 192.168.3.1/24 dev veth-R1-2
sudo ip netns exec R2 ip addr add 192.168.3.2/24 dev veth-R2-1
sudo ip netns exec R2 ip addr add 192.168.4.1/24 dev veth-R2-2
sudo ip netns exec R3 ip addr add 192.168.4.2/24 dev veth-R3-1
sudo ip netns exec R3 ip addr add 192.168.5.2/24 dev veth-R3-2
sudo ip netns exec PC2 ip addr add 192.168.5.1/24 dev veth-PC2

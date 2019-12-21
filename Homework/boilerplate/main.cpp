#include "rip.h"
#include "router.h"
#include "router_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

extern uint16_t computeIPChecksum(uint8_t *packet, size_t len);
extern bool validateIPChecksum(uint8_t *packet, size_t len);
extern void update(bool insert, RoutingTableEntry entry);
extern bool query(uint32_t addr, uint32_t *nexthop, uint32_t *if_index);
extern bool forward(uint8_t *packet, size_t len);
extern bool disassemble(const uint8_t *packet, uint32_t len, RipPacket *output);
extern uint32_t assemble(const RipPacket *rip, uint8_t *buffer);
extern RoutingTableEntry routerTable[120];
extern int routerTableSize;

uint8_t packet[2048];
uint8_t output[2048];
// 0: 10.0.0.1
// 1: 10.0.1.1
// 2: 10.0.2.1
// 3: 10.0.3.1
// veth-R2-1 : 192.168.3.2  0x0203a8c0
// veth-R2-2 : 192.168.4.1  0x0104a8c0
// 你可以按需进行修改，注意端序
in_addr_t addrs[N_IFACE_ON_BOARD] = {0x0203a8c0, 0x0104a8c0, 0x0102000a,
                                    0x0103000a};

void printCurrentTable() {
  printf("current my table\n");
  for (int i = 0;i < routerTableSize;i ++) {
    printf("addr: %08x  len: %08x  if_index: %08x nexthop: %08x  metric: %08x\n", 
      routerTable[i].addr,
      routerTable[i].len,
      routerTable[i].if_index,
      routerTable[i].nexthop,
      routerTable[i].metric
      );
  }
}

void construct_IP_UDP_header(uint32_t total_len, uint32_t src, uint32_t dst) {
  // construct IP header
  output[0]  = 0x45;                                                            // version | header length
  output[1]  = 0;                                                               // DSCP
  output[2]  = (total_len >> 8) & 0xff; output[3] = total_len & 0xff;           // Total length
  output[4]  = 0; output[5] = 0;                                                // ID
  output[6]  = 0; output[7] = 0;                                                // Flags
  output[8]  = 1;                                                               // TTL
  output[9]  = 0x11;                                                            // Protocal

  *((uint32_t *)(output + 12)) = src;                                           // 12 - 15
  *((uint32_t *)(output + 16)) = dst;                                           // 16 - 19

  // construct UDP header
  output[20] = 0x02; output[21] = 0x08;                                         // source port
  output[22] = 0x02; output[23] = 0x08;                                         // destination port
  output[24] = ((total_len - 20) >> 8) & 0xff;   output[25] = (total_len - 20) & 0xff;   // length
  output[26] = 0;    output[27] = 0;                                            // checksum
}

uint32_t convert(uint32_t a, uint32_t b, uint32_t c, uint32_t d) {
  // 将4个数拼成一个32位数 
  return (a << 24) + (b << 16) + (c << 8) + (d);
}

uint32_t mask_to_len(uint32_t mask) {
  int zeroNum = 0;
  for(int i = 0;i < 32;i ++) {
    if (mask % 2 == 0) {
      mask = mask >> 1;
      zeroNum ++;
    } else {
      break;
    } 
  }
  return 32 - zeroNum;
}

uint32_t len_to_mask(uint32_t len) {
  return 0xffffffff << (32 - len);
}

bool isExist(uint32_t addr, uint32_t len, uint32_t *idx) {
  for (int i = 0;i < routerTableSize;i ++) {
    if (addr == routerTable[i].addr && len == routerTable[i].len) {
      *idx = i;
      return true;
    }
  }
  return false;
}

int main(int argc, char *argv[]) {
  // 0a.
  int res = HAL_Init(1, addrs);
  if (res < 0) {
    return res;
  }

  // 0b. Add direct routes
  // For example:
  // 10.0.0.0/24 if 0
  // 10.0.1.0/24 if 1
  // 10.0.2.0/24 if 2
  // 10.0.3.0/24 if 3
  for (uint32_t i = 0; i < N_IFACE_ON_BOARD; i++) {
    RoutingTableEntry entry = {
        .addr = addrs[i] & 0x00FFFFFF, // big endian
        .len = 24,        // small endian
        .if_index = i,    // small endian
        .nexthop = 0,     // big endian, means direct
        .metric = ntohl(1)
    };
    update(true, entry);
  }

  macaddr_t multicastMac = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x09};
  uint32_t multicastIP = 0x090000e0;

  uint64_t last_time = 0;
  while (1) {
    uint64_t time = HAL_GetTicks();
    if (time > last_time + 5 * 1000) {
      // What to do?
      // send complete routing table to every interface
      // ref. RFC2453 3.8
      // multicast MAC for 224.0.0.9 is 01:00:5e:00:00:09
      // 使所有端口发送发送广播包
      for (uint32_t i = 0; i < N_IFACE_ON_BOARD; i++) {
        // 根据routerTable 构建 RipPacket
        RipPacket _rip;
        int routerItemNum = 0;
        for (int j = 0;j < routerTableSize;j ++) {
          // 水平分割, 需要将改端口出的表项去掉
          if (routerTable[j].if_index == i)  {
            continue;
          }
          uint32_t mask = len_to_mask(routerTable[j].len);
          _rip.entries[routerItemNum].addr = routerTable[j].addr;
          _rip.entries[routerItemNum].mask = ntohl(mask);
          _rip.entries[routerItemNum].nexthop = routerTable[j].nexthop;
          _rip.entries[routerItemNum].metric = routerTable[j].metric;
          // 统计本端口考虑水平分割后可以接受的所有路由表的项数
          routerItemNum ++;
        }
        _rip.numEntries = routerItemNum;
        _rip.command = 2;
        // packet len
        uint32_t packetLen = 20 + 8 + 4 + routerItemNum * 20;
        // 构建 header
        construct_IP_UDP_header(packetLen, addrs[i], multicastIP);
        // 根据 header 计算 checksum
        uint16_t sum = computeIPChecksum(output, packetLen);
        output[10] = (sum >> 8) & 0xff;
        output[11] = sum & 0xff;
        // 根据 RipPacket 更新buffer
        assemble(&_rip, output + 20 + 8);
        HAL_SendIPPacket(i, output, packetLen, multicastMac);



      }
      printf("5s Timer\n");
      last_time = time;
    }

    int mask = (1 << N_IFACE_ON_BOARD) - 1;
    macaddr_t src_mac;
    macaddr_t dst_mac;
    int if_index;
    res = HAL_ReceiveIPPacket(mask, packet, sizeof(packet), src_mac, dst_mac,
                              1000, &if_index);
    if (res == HAL_ERR_EOF) {
      break;
    } else if (res < 0) {
      return res;
    } else if (res == 0) {
      // Timeout
      continue;
    } else if (res > sizeof(packet)) {
      // packet is truncated, ignore it
      continue;
    }

    // 1. validate
    if (!validateIPChecksum(packet, res)) {
      printf("Invalid IP Checksum\n");
      continue;
    }
    in_addr_t src_addr, dst_addr;
    // TODO: extract src_addr and dst_addr from packet (big endian)
    src_addr = ntohl(convert(packet[12], packet[13], packet[14], packet[15]));
    dst_addr = ntohl(convert(packet[16], packet[17], packet[18], packet[19]));
    

    // 2. check whether dst is me
    bool dst_is_me = false;
    for (int i = 0; i < N_IFACE_ON_BOARD; i++) {
      if (memcmp(&dst_addr, &addrs[i], sizeof(in_addr_t)) == 0) {
        dst_is_me = true;
        break;
      }
    }

    // TODO: Handle rip multicast address(224.0.0.9)?
    if (memcmp(&dst_addr, &multicastIP, sizeof(in_addr_t)) == 0) {
      dst_is_me = true;
    }

    if (dst_is_me) {
      // 3a.1
      RipPacket rip;
      // check and validate
      if (disassemble(packet, res, &rip)) {
        if (rip.command == 1) {
          // 3a.3 request, ref. RFC2453 Section 3.9.1
          // only need to respond to whole table requests in the lab

          RipPacket resp;
          int routerItemNum = 0;
          for (int j = 0;j < routerTableSize;j ++) {
            // 水平分割, 需要将改端口出的表项去掉
            if (routerTable[j].if_index == if_index)  {
              continue;
            }
            // 统计本端口考虑水平分割后可以接受的所有路由表的项数
            routerItemNum ++;
            uint32_t mask = len_to_mask(routerTable[j].len);
            resp.entries[j].addr = routerTable[j].addr;
            resp.entries[j].mask = ntohl(mask);
            resp.entries[j].nexthop = routerTable[j].nexthop;
            resp.entries[j].metric = routerTable[j].metric;
          }
          resp.numEntries = routerItemNum;
          resp.command = 2;

          // packet len
          uint32_t packetLen = 20 + 8 + 4 + routerItemNum * 20;
          // 构建 header
          construct_IP_UDP_header(packetLen, addrs[if_index], src_addr);
          // 根据 header 计算 checksum
          uint16_t sum = computeIPChecksum(output, packetLen);
          output[10] = (sum >> 8) & 0xff;
          output[11] = sum & 0xff;
          // 根据 RipPacket 更新buffer
          assemble(&resp, output + 20 + 8);
          HAL_SendIPPacket(if_index, output, packetLen, multicastMac);
        } else {
          // 3a.2 response, ref. RFC2453 Section 3.9.2
          // TODO: update routing table
          // new metric = ?
          // update metric, if_index, nexthop
          // HINT: handle nexthop = 0 case
          // HINT: what is missing from RoutingTableEntry?
          // you might want to use `query` and `update` but beware of the difference between exact match and longest prefix match
          // optional: triggered updates? ref. RFC2453 3.10.1
          printf("if_index: %d\n", if_index);
          printf("all give me rip\n");
          for (int i = 0;i < rip.numEntries;i ++) {
            printf("addr: %08x  mask: %08x  nexthop: %08x  metric: %08x\n", 
                    rip.entries[i].addr, 
                    rip.entries[i].mask, 
                    rip.entries[i].nexthop, 
                    rip.entries[i].metric);
          }
          bool ifupdate = false;
          for (int i = 0;i < rip.numEntries;i ++) {
            RipEntry entry = rip.entries[i];
            uint32_t idx = 0;
            uint32_t metric = ntohl(entry.metric) + 1;
            int entrylen = mask_to_len(ntohl(entry.mask));
            if (isExist(entry.addr, entrylen, &idx)) {
              if (routerTable[idx].nexthop != 0) {
                // 判断如果不是直连路由才更新
                if (routerTable[idx].if_index == if_index) {
                  if (metric > 16 ) {
                    // 删除
                    ifupdate = true;
                    printCurrentTable();
                    routerTable[idx] = routerTable[routerTableSize - 1];
                    routerTableSize -= 1;
                  } else {
                    // 无论好坏都更新
                    routerTable[i].if_index = if_index;
                    routerTable[i].nexthop = src_addr;
                    routerTable[i].metric = ntohl(metric); 
                  }
                } else if (metric < ntohl(routerTable[idx].metric)) {
                  // 只有变好的情况才会更新
                  routerTable[i].if_index = if_index;
                  routerTable[i].nexthop = src_addr;
                  routerTable[i].metric = ntohl(metric); 
                  ifupdate = true;
                }
              }
            } else if (metric <= 16) {
                ifupdate = true;
                // 小于等于16且已有表中不存在，则新加
                RoutingTableEntry new_entry;
                new_entry.addr = entry.addr;
                new_entry.if_index = if_index;
                new_entry.len = mask_to_len(ntohl(entry.mask));
                // 更新路由时如果是直连路由需要注意nexthtop是0
                // new_entry.nexthop = (entry.nexthop == 0) ? 0 : src_addr; 
                new_entry.nexthop = src_addr; 
                new_entry.metric = ntohl(metric); 
                routerTable[routerTableSize] = new_entry;
                routerTableSize += 1;
            }
          }
        }
      }
    } else {
      // 3b.1 dst is not me
      // forward
      // beware of endianness
      uint32_t nexthop, dest_if, idx;
      if (query(dst_addr, &nexthop, &dest_if)) {
        // found
        macaddr_t dest_mac;
        // direct routing
        if (nexthop == 0) {
          nexthop = dst_addr;
        }
        if (HAL_ArpGetMacAddress(dest_if, nexthop, dest_mac) == 0) {
          // found
          memcpy(output, packet, res);
          // update ttl and checksum
          forward(output, res);
          // TODO(optional): check ttl=0 case
          if (packet[8] == 0) {
            continue;
          }
          HAL_SendIPPacket(dest_if, output, res, dest_mac);
        } else {
          // not found
          // you can drop it
          printf("ARP not found for nexthop %x\n", nexthop);
        }
      } else {
        // not found
        // TODO(optional): send ICMP Host Unreachable
        printf("IP not found for src %x dst %x\n", src_addr, dst_addr);
      }
    }
  }
  return 0;
}

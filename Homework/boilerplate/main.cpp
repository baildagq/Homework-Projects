#include "rip.h"
#include "router.h"
#include "router_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern uint16_t computeIPChecksum(uint8_t *packet, size_t len);
extern bool validateIPChecksum(uint8_t *packet, size_t len);
extern void update(bool insert, RoutingTableEntry entry);
extern bool query(uint32_t addr, uint32_t *nexthop, uint32_t *if_index, uint32_t *idx);
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
// 你可以按需进行修改，注意端序
in_addr_t addrs[N_IFACE_ON_BOARD] = {0x0100000a, 0x0101000a, 0x0102000a,
                                     0x0103000a};

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

uint32_t convert(uint8_t a, uint8_t b, uint8_t c, uint8_t d) {
  // 将4个数拼成一个32位数 
  return a << 24 + b << 16 + c << 8 + d;
}

uint32_t mask_to_len(uint32_t mask) {
  int zeroNum = 0;
  for(int i = 0;i < 32;i ++) {
    if (mask % 2 == 0) {
      mask >> 1;
      zeroNum ++;
    } else {
      break;
    } 
  }
  return 32 - zeroNum;
}

uint32_t len_to_mask(uint32_t len) {
  uint32_t mask = 0;
  for (int h = 0;h < len;h ++) {
    mask = mask * 2 + 1;
  }
  for (int h = len; h < 32; h ++) {
    mask = mask * 2;
  }
  return mask;
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
        .metric = 0
    };
    update(true, entry);
  }

  macaddr_t multicastMac = {0x01, 0x00, 0x5e, 0x00, 0x00, 0x09};
  uint32_t multicastIP = 0x090000e0;

  uint64_t last_time = 0;
  while (1) {
    uint64_t time = HAL_GetTicks();
    if (time > last_time + 30 * 1000) {
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
          // 统计本端口考虑水平分割后可以接受的所有路由表的项数
          routerItemNum ++;
          uint32_t mask = len_to_mask(routerTable[j].len);
          _rip.entries[j].addr = routerTable[j].addr;
          _rip.entries[j].mask = mask;
          _rip.entries[j].nexthop = routerTable[j].nexthop;
          _rip.entries[j].metric = routerTable[j].metric;
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
      printf("30s Timer\n");
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
    // extract src_addr and dst_addr from packet
    // big endian
    // TODO 
    src_addr = convert(packet[12], packet[13], packet[14], packet[15]);
    dst_addr = convert(packet[16], packet[17], packet[18], packet[19]);
    

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
          // 3a.3 request, ref. RFC2453 3.9.1
          // only need to respond to whole table requests in the lab
          RipPacket resp;





          //
          resp.numEntries = 4;
          resp.command = 2;
          int routerItemNum = 0;
          for (int j = 0;j < routerTableSize;j ++) {
            // 水平分割, 需要将改端口出的表项去掉
            if (routerTable[j].if_index == if_index)  {
              continue;
            }
            // 统计本端口考虑水平分割后可以接受的所有路由表的项数
            routerItemNum ++;
            uint32_t mask = 0;
            for (int h = 0;h < routerTable[j].len;h ++) {
              mask = mask * 2 + 1;
            }
            for (int h = routerTable[j].len; h < 32; h ++) {
              mask = mask * 2;
            }
            resp.entries[j].addr = routerTable[j].addr;
            resp.entries[j].mask = mask;
            resp.entries[j].nexthop = routerTable[j].nexthop;
            resp.entries[j].metric = routerTable[j].metric;
          }

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






          // TODO: fill resp
          // // assemble
          // // IP
          // output[0] = 0x45;
          // // ...
          // // UDP
          // // port = 520
          // output[20] = 0x02;
          // output[21] = 0x08;
          // // ...
          // // RIP
          // uint32_t rip_len = assemble(&resp, &output[20 + 8]);
          // // checksum calculation for ip and udp
          // // if you don't want to calculate udp checksum, set it to zero
          // // send it back
          // HAL_SendIPPacket(if_index, output, rip_len + 20 + 8, src_mac);
        } else {
          // 3a.2 response, ref. RFC2453 3.9.2
          // update routing table
          // new metric = ?
          // update metric, if_index, nexthop
          // what is missing from RoutingTableEntry?
          // TODO: use query and update
          // triggered updates? ref. RFC2453 3.10.1
          for (int i = 0;i < rip.numEntries;i ++) {
            RipEntry& entry = rip.entries[i];
            uint32_t nexthop, index, idx;
            int metric = entry.metric + 1;
            if (query(entry.addr, &nexthop, &index, &idx)) {
              if (metric > 16) {
                // 删除
                routerTable[idx] = routerTable[routerTableSize - 1];
                routerTableSize -= 1;
              } else if (index == if_index || metric < routerTable[idx].metric) {
                  // 无论如何都更新 || metric < currentMetric才会更新
                  RoutingTableEntry new_entry;
                  new_entry.addr = entry.addr;
                  new_entry.if_index = if_index;
                  new_entry.len = mask_to_len(entry.mask);
                  new_entry.nexthop = src_addr;
                  new_entry.metric = metric; 
                  routerTable[idx] = new_entry;
              }
            } else if (metric <= 16) {
                // 小于等于16且已有表中不存在，则新加
                RoutingTableEntry new_entry;
                new_entry.addr = entry.addr;
                new_entry.if_index = if_index;
                new_entry.len = mask_to_len(entry.mask);
                new_entry.nexthop = src_addr;
                new_entry.metric = metric; 
                // 新加
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
      if (query(dst_addr, &nexthop, &dest_if, &idx)) {
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
          // TODO: you might want to check ttl=0 case
          if (packet[8] == 0) {
            continue;
          }
          HAL_SendIPPacket(dest_if, output, res, dest_mac);
        } else {
          // not found
          // you can drop it
          printf("ARP not found for %x\n", nexthop);
        }
      } else {
        // not found
        // optionally you can send ICMP Host Unreachable
        printf("IP not found for %x\n", src_addr);
      }
    }
  }
  return 0;
}

#include "rip.h"
#include <stdint.h>
#include <stdlib.h>
#include <cstdio>
#include <arpa/inet.h>

/*
  在头文件 rip.h 中定义了如下的结构体：
  #define RIP_MAX_ENTRY 25
  typedef struct {
    // all fields are big endian
    // we don't store 'family', as it is always 2(for response) and 0(for request)
    // we don't store 'tag', as it is always 0
    uint32_t addr;
    uint32_t mask;
    uint32_t nexthop;
    uint32_t metric;
  } RipEntry;

  typedef struct {
    uint32_t numEntries;
    // all fields below are big endian
    uint8_t command; // 1 for request, 2 for response, otherwsie invalid
    // we don't store 'version', as it is always 2
    // we don't store 'zero', as it is always 0
    RipEntry entries[RIP_MAX_ENTRY];
  } RipPacket;

  你需要从 IPv4 包中解析出 RipPacket 结构体，也要从 RipPacket 结构体构造出对应的 IP 包
  由于 Rip 包结构本身不记录表项的个数，需要从 IP 头的长度中推断，所以在 RipPacket 中额外记录了个数。
  需要注意这里的地址都是用 **大端序** 存储的，1.2.3.4 对应 0x04030201 。
*/

/**
 * @brief 从接受到的 IP 包解析出 Rip 协议的数据
 * @param packet 接受到的 IP 包
 * @param len 即 packet 的长度
 * @param output 把解析结果写入 *output
 * @return 如果输入是一个合法的 RIP 包，把它的内容写入 RipPacket 并且返回 true；否则返回 false
 * 
 * IP 包的 Total Length 长度可能和 len 不同，当 Total Length 大于 len 时，把传入的 IP 包视为不合法。
 * 你不需要校验 IP 头和 UDP 的校验和是否合法。
 * 你需要检查 Command 是否为 1 或 2，Version 是否为 2， Zero 是否为 0，
 * Family 和 Command 是否有正确的对应关系（见上面结构体注释），Tag 是否为 0，
 * Metric 转换成小端序后是否在 [1,16] 的区间内，
 * Mask 的二进制是不是连续的 1 与连续的 0 组成等等。
 */

extern uint32_t convert(uint32_t a, uint32_t b, uint32_t c, uint32_t d);


// IP 报头的长度根据 header Length 计算得到
// UDP 报头根据观察和简单查询假定为8个字节
bool disassemble(const uint8_t *packet, uint32_t len, RipPacket *output) {
    // TODO:
    uint32_t packetLen = ((uint32_t)packet[2] << 8) | ((uint32_t)packet[3]);  
    uint8_t headerLen = (packet[0] & 0xf) * 4; // 取出headerLen后，因为单位为4byte, 再乘4
    uint8_t udpLen = 8;

    const uint8_t *udpPacket = packet + headerLen; 
    const uint8_t *ripPacket = udpPacket + udpLen; 

    // for debug
    // check whether len < Total Length
    if (len < packetLen)
        return false;
    // check whether command == 1 or command == 2 
    if (ripPacket[0] != 1 && ripPacket[0] != 2)
        return false;
    // check whether Version == 2 
    if (ripPacket[1] != 2)
        return false;
    // check whether Zero == 0
    if (ripPacket[2] != 0 || ripPacket[3] != 0)
        return false;

    // begin construct output
    output->numEntries = (packetLen - headerLen - udpLen) / 20; // update number of Entry
    output->command = ripPacket[0]; // update command
    for (int i = 0;i < output->numEntries;i ++) {
        const uint8_t *ripEntry = ripPacket + 4 + 20 * i;
        uint16_t family = ((uint16_t)ripEntry[0] << 8) | (uint16_t)ripEntry[1];
        // check whether (command == 1 && family == 0)  || (command == 2 && family == 2)
        if (!((output->command == 1 && family == 0) || (output->command == 2 && family == 2)))
            return false;
        // check whether tag = 0;
        uint16_t tag = ((uint16_t)ripEntry[2] << 8) | (uint16_t)ripEntry[3];
        if (tag != 0)
            return false;
        // check whether metric belong [0, 16]
        uint32_t metric = convert(
            ripEntry[16],
            ripEntry[17],
            ripEntry[18],
            ripEntry[19]
            );
        if (!(metric >= 1 && metric <= 16))
            return false;
        // check mask format
        uint32_t mask = convert(
            ripEntry[8],
            ripEntry[9],
            ripEntry[10],
            ripEntry[11]
            );
        bool appear = false;
        for (int j = 0;j < 32; j++) {
            if (mask % 2 == 1) {
                appear = true;
            } else if (appear){
                return false;
            }
            mask >>= 1;
        }
        // pass all check, begin record
        output->entries[i].nexthop = convert(
            ripEntry[15],
            ripEntry[14],
            ripEntry[13],
            ripEntry[12]
            );
        output->entries[i].mask = convert(
            ripEntry[11],
            ripEntry[10],
            ripEntry[9],
            ripEntry[8]
            );
        output->entries[i].addr = convert(
            ripEntry[7],
            ripEntry[6],
            ripEntry[5],
            ripEntry[4]
            );
        output->entries[i].metric = ntohl(metric);
    }
    return true;
}

/**
 * @brief 从 RipPacket 的数据结构构造出 RIP 协议的二进制格式
 * @param rip 一个 RipPacket 结构体
 * @param buffer 一个足够大的缓冲区，你要把 RIP 协议的数据写进去
 * @return 写入 buffer 的数据长度
 * 
 * 在构造二进制格式的时候，你需要把 RipPacket 中没有保存的一些固定值补充上，包括 Version、Zero、Address Family 和 Route Tag 这四个字段
 * 你写入 buffer 的数据长度和返回值都应该是四个字节的 RIP 头，加上每项 20 字节。
 * 需要注意一些没有保存在 RipPacket 结构体内的数据的填写。
 */
uint32_t assemble(const RipPacket *rip, uint8_t *buffer) {
  // TODO:
  // update rip head
  buffer[0] = rip->command;
  buffer[1] = 0x2;
  buffer[2] = 0x0;
  buffer[3] = 0x0;
  for (int i = 0;i < rip->numEntries; i ++) {
      // update family
      buffer[4 + 20 * i] = 0;
      if (rip->command == 1)
          // command = 1, family = 0
          buffer[4 + 20 * i + 1] = 0;
      else 
          // command = 2, family = 2
          buffer[4 + 20 * i + 1] = 2;
      // update tag
      buffer[4 + 20 * i + 2] = 0;
      buffer[4 + 20 * i + 3] = 0;
      // update addr
      buffer[4 + 20 * i + 7] = (rip->entries[i].addr & 0xff000000) >> 24;
      buffer[4 + 20 * i + 6] = (rip->entries[i].addr & 0x00ff0000) >> 16;
      buffer[4 + 20 * i + 5] = (rip->entries[i].addr & 0x0000ff00) >> 8;
      buffer[4 + 20 * i + 4] = (rip->entries[i].addr & 0x000000ff);
      // update mask
      buffer[4 + 20 * i + 11] = (rip->entries[i].mask & 0xff000000) >> 24;
      buffer[4 + 20 * i + 10] = (rip->entries[i].mask & 0x00ff0000) >> 16;
      buffer[4 + 20 * i + 9]  = (rip->entries[i].mask & 0x0000ff00) >> 8;
      buffer[4 + 20 * i + 8]  = (rip->entries[i].mask & 0x000000ff);
      // update nexthop
      buffer[4 + 20 * i + 15] = (rip->entries[i].nexthop & 0xff000000) >> 24;
      buffer[4 + 20 * i + 14] = (rip->entries[i].nexthop & 0x00ff0000) >> 16;
      buffer[4 + 20 * i + 13] = (rip->entries[i].nexthop & 0x0000ff00) >> 8;
      buffer[4 + 20 * i + 12] = (rip->entries[i].nexthop & 0x000000ff);
      // update metric
      buffer[4 + 20 * i + 19] = (rip->entries[i].metric & 0xff000000) >> 24;
      buffer[4 + 20 * i + 18] = (rip->entries[i].metric & 0x00ff0000) >> 16;
      buffer[4 + 20 * i + 17] = (rip->entries[i].metric & 0x0000ff00) >> 8;
      buffer[4 + 20 * i + 16] = (rip->entries[i].metric & 0x000000ff);
  }
  return 4 + 20 * rip->numEntries;
}

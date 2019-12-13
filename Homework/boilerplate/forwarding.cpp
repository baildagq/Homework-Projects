#include <stdint.h>
#include <stdlib.h>
#include <cstdio>

extern bool validateIPChecksum(uint8_t *packet, size_t len);
extern uint16_t computeIPChecksum(uint8_t *packet, size_t len);
/**
 * @brief 进行转发时所需的 IP 头的更新：
 *        你需要先检查 IP 头校验和的正确性，如果不正确，直接返回 false ；
 *        如果正确，请更新 TTL 和 IP 头校验和，并返回 true 。
 *        你可以从 checksum 题中复制代码到这里使用。
 * @param packet 收到的 IP 包，既是输入也是输出，原地更改
 * @param len 即 packet 的长度，单位为字节
 * @return 校验和无误则返回 true ，有误则返回 false
 */

// uint16_t computeIPChecksum(uint8_t *packet, size_t len) {
//     size_t headerLen = (packet[0] & 0xf) * 4; // 取出headerLength后，因为单位为4byte, 再乘4
//     // uint16_t *header = new uint16_t[headerLen / 2];
//     uint32_t result = 0;
//     // 记录校验和，compute过程会将原始校验和更改
//     uint8_t p1 = packet[10];
//     uint8_t p2 = packet[11];
//     // 将校验码置为0
//     packet[10] = 0;
//     packet[11] = 0;
//     // 将所有包头信息相加
//     for (int i = 0;i < headerLen; i += 2) {
//         uint32_t merge = ((uint32_t)packet[i] << 8) | ((uint32_t)packet[i + 1]);
//         result += merge;
//     }
//     // 将校验码还原
//     packet[10] = p1;
//     packet[11] = p2;
//     // 对溢出进行截断相加
//     result = (result >> 16) + (result & 0xffff);
//     // 取反
//     result = ~result;
//     return (uint16_t)result;
// }

// bool validateIPChecksum(uint8_t *packet, size_t len) {
//     // method1: simply check whether true, but because we need compute new checksum code, so we directly implement method2

//     // method2: compute the checksum code, compare the computed checksum and given checksum to check whether true
//     size_t headerLen = (packet[0] & 0xf) * 4; // 取出headerLength后，因为单位为4byte, 再乘4
//     uint16_t realChecksum = ((uint16_t)packet[10] << 8) | (uint16_t)packet[11]; 
//     uint16_t computedChecksum = computeIPChecksum(packet, len);
//     return realChecksum == computedChecksum;
// }

bool forward(uint8_t *packet, size_t len) {
  // TODO:
  if (!validateIPChecksum(packet, len))
      return false;
  // TTL -1
  packet[8] = packet[8] - 1;
  uint16_t computedChecksum = computeIPChecksum(packet, len);
  packet[10] = (computedChecksum & 0xff00) >> 8;
  packet[11] = (computedChecksum & 0x00ff);
  return true;
}

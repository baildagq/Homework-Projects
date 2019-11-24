#include <stdint.h>
#include <stdlib.h>
#include <cstdio>


/**
 * @brief 进行 IP 头的校验和的验证
 * @param packet 完整的 IP 头和载荷
 * @param len 即 packet 的长度，单位是字节，保证包含完整的 IP 头
 * @return 校验和无误则返回 true ，有误则返回 false
 */

uint16_t computeIPChecksum(uint8_t *packet, size_t len) {
    size_t headerLen = (packet[0] & 0xf) * 4; // 取出headerLength后，因为单位为4byte, 再乘4
    // uint16_t *header = new uint16_t[headerLen / 2];
    uint32_t result = 0;
    // 将校验码置为0
    packet[10] = 0;
    packet[11] = 0;
    // 将所有包头信息相加
    for (int i = 0;i < headerLen; i += 2) {
        uint32_t merge = ((uint32_t)packet[i] << 8) | ((uint32_t)packet[i + 1]);
        result += merge;
    }
    // 对溢出进行截断相加
    result = (result >> 16) + (result & 0xffff);
    // 取反
    result = ~result;
    return (uint16_t)result;
}

bool validateIPChecksum(uint8_t *packet, size_t len) {
    // method1: simply check whether true, but because we need compute new checksum code, so we directly implement method2

    // method2: compute the checksum code, compare the computed checksum and given checksum to check whether true
    size_t headerLen = (packet[0] & 0xf) * 4; // 取出headerLength后，因为单位为4byte, 再乘4
    uint16_t realChecksum = ((uint16_t)packet[10] << 8) | (uint16_t)packet[11]; 
    uint16_t computedChecksum = computeIPChecksum(packet, len);
    return realChecksum == computedChecksum;
}

#include "router.h"
#include <stdint.h>
#include <stdlib.h>
#include <cstdio>
#include <arpa/inet.h>


using namespace std;

/*
  RoutingTable Entry 的定义如下：
  typedef struct {
    uint32_t addr; // 大端序，IPv4 地址
    uint32_t len; // 小端序，前缀长度
    uint32_t if_index; // 小端序，出端口编号
    uint32_t nexthop; // 大端序，下一跳的 IPv4 地址
  } RoutingTableEntry;

  约定 addr 和 nexthop 以 **大端序** 存储。
  这意味着 1.2.3.4 对应 0x04030201 而不是 0x01020304。
  保证 addr 仅最低 len 位可能出现非零。
  当 nexthop 为零时这是一条直连路由。
  你可以在全局变量中把路由表以一定的数据结构格式保存下来。
*/

/**
 * @brief 插入/删除一条路由表表项
 * @param insert 如果要插入则为 true ，要删除则为 false
 * @param entry 要插入/删除的表项
 * 
 * 插入时如果已经存在一条 addr 和 len 都相同的表项，则替换掉原有的。
 * 删除时按照 addr 和 len 匹配。
 */

// // 定义map全局变量，用于存储key-value表
// struct Key {
//     uint32_t addr;
//     int maskLen;
// }

// std::map<Key, RoutingTableEntry> routerTable;

RoutingTableEntry routerTable[120];
int routerTableSize = 0;

void update(bool insert, RoutingTableEntry entry) {
  // TODO:
    if (insert) {
        // insert
        for (int i = 0;i < routerTableSize; i ++){
            if (routerTable[i].addr  == entry.addr && routerTable[i].len == entry.len) {
                // 存在已有表项
                routerTable[i] = entry;
                // printf("Yes, update, current size: %d\n", routerTableSize);
                return;
            }
        }
        // 不存在已有表项，需要新增表项
        routerTable[routerTableSize] = entry;
        routerTableSize += 1;
        // printf("Yes, add, current size: %d\n", routerTableSize);
    } else {
        // delete
        for (int i = 0;i < routerTableSize; i ++){
            if (routerTable[i].addr  == entry.addr && routerTable[i].len == entry.len) {
                // 存在已有表项
                routerTable[i] = routerTable[routerTableSize - 1];
                routerTableSize -= 1;
                // printf("No, delete, current size: %d\n", routerTableSize);
                return;
            }
        }
    }
}

/**
 * @brief 进行一次路由表的查询，按照最长前缀匹配原则
 * @param addr 需要查询的目标地址，大端序
 * @param nexthop 如果查询到目标，把表项的 nexthop 写入
 * @param if_index 如果查询到目标，把表项的 if_index 写入
 * @return 查到则返回 true ，没查到则返回 false
 */

extern uint32_t len_to_mask(uint32_t len);

bool query(uint32_t addr, uint32_t *nexthop, uint32_t *if_index) {
  // TODO:
  *nexthop = 0;
  *if_index = 0;
  uint32_t maxMask = 0;
  // RoutingTableEntry entry;
  bool isFind = false;
//   printf("primate addr %08x\n", addr);
//   printf("convert addr %08x\n", ntohl(addr));
  for (int i = 0;i < routerTableSize;i ++) {
    //   uint32_t mask = 0xffffffff >> (32 - routerTable[i].len);
      uint32_t mask = len_to_mask(routerTable[i].len);
      if ((ntohl(routerTable[i].addr) & mask) == (ntohl(addr) & mask)) {
          if (routerTable[i].len > maxMask) {
              maxMask = routerTable[i].len;
              *nexthop = routerTable[i].nexthop;
              *if_index = routerTable[i].if_index;
              isFind = true;
          }
      }
  }
  return isFind;
}

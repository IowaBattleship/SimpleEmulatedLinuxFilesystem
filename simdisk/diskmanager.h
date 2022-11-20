#pragma once
#include <utility>
#include "structure.h"
#include "diskio.h"

//根据需要的盘块数量分配盘块 返回<分配的物理首地址, 分配的盘块首下标>
std::pair<uint, int> allocateBlock(const uint blockNum);

//为单个文件/目录分配一个i节点
int allocateINode();

//释放指定位置、长度的盘块
void freeBlock(const int blockFirstPos, const uint blockNum);

//释放一个i节点
void freeINode(const int iNodePos);
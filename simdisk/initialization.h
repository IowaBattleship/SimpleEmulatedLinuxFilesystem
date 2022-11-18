#pragma once
#include <cstdio>
#include <cstring>
#include "structure.h"
#include "diskio.h"
#include "shellio.h"

//从虚拟硬盘加载数据
bool loadDataFromDisk();

//清空并初始化所有数据
bool initAllData();
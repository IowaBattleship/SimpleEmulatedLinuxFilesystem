#pragma once
#include "structure.h"
#include "shellio.h"

//将系统信息写入虚拟磁盘
bool writeSystemData();

//从磁盘读取系统信息
bool readSystemData();

//将用户信息写入User.dat
bool writeUserInfo();

//从User.dat读取用户信息
bool readUserInfo();
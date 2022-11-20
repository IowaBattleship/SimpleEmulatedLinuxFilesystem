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

//写入指定信息
void writeSpecifiedInfo(InfoType infoType); //通过InfoType指定超级块/所有i节点/位图
void writeSpecifiedInfo(const int st, const int num, InfoType infoType); //指定写入数个i节点/位图
void writeSpecifiedInfo(const uint addr, Directory& targetDir); //指定写入单个目录
void writeSpecifiedInfo(const uint addr, const char targetFile[]); //指定写入单个文件

//从虚拟硬盘读取指定信息
void readSpecifiedInfo(const uint addr, Directory& targetDir); //读取目录
void readSpecifiedInfo(const uint addr, const int fileSize, char targetFile[]); //读取文件
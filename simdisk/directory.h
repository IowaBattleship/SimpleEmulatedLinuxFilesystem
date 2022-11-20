#pragma once
#include <cstdlib>
#include <tuple>
#include "structure.h"
#include "shellio.h"
#include "diskio.h"
#include "diskmanager.h"
#include "fileoperation.h"

//寻找当前目录下的目标目录 返回子目录下标 -1时为未找到
int findDirInDir(Directory& dir, const char targetDir[]);

//进入当前目录下的目标目录
bool enterDirInDir(Directory& dir, const char targetDir[]);
bool enterDirInDir(Directory& dir, const char targetDir[], int& dirPos); //同时记录子目录下标

//通过路径获取路径最终目录
bool getDirFromPath(const char path[], Directory& targetDir);

//分离路径和最终目录/文件名
void dividePathAndFileOrDir(char pathAndFileOrDir[], char path[], char fileOrDirName[]);

//从目标目录出发 通过递归扫描目录至根目录获取绝对路径
void getPathFromDir(Directory& dir, char path[]);

//显示当前目录下的内容
void displayDir(Directory& dir);

//判断目录a是否包含目录b
bool ifDirContainDir(Directory& aDir, Directory& bDir);

//释放目录所占资源 对子目录进行递归释放
void freeUpDir(Directory& dir);

//md命令创建目录
void md(char pathAndDirName[]);

//cd命令进入目录
void cd(char path[]);

//ls(dir)命令显示目录
void ls(const char path[], bool ifDisplaySubDir);

//rd命令删除目录
void rd(char pathAndDirName[]);
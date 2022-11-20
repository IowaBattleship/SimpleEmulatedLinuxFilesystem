#pragma once
#include <cmath>
#include <tuple>
#include "structure.h"
#include "diskio.h"
#include "shellio.h"
#include "diskmanager.h"
#include "directory.h"

//寻找当前目录下的目标文件 返回子文件下标 -1时为未找到
int findFileInDir(Directory& dir, const char targetFile[]);

//释放一个文件所占的盘块、i节点
void freeUpFile(const int fileInodePos);

//在dir下创建指定文件
bool createFile(Directory& dir, char fileName[], char fileContent[]);

//确认是否为Windows路径 若是则去除<host> 并根据是否为源来找出文件名
bool ifWinAndDividePath(char path[]); //目标路径
bool ifWinAndDividePath(char pathAndFile[], char path[], char fileName[]); //源路径

//newfile命令创建文件
void newfile(char pathAndFileName[]);

//cat命令打开文件
void cat(char pathAndFileName[]);

//del命令删除文件
void del(char pathAndFileName[]);

//copy命令复制文件
void copy(char sourcePathAndFileName[], char targetPath[]);
#pragma once
#include "structure.h"
#include "shellio.h"
#include "directory.h"
#include "fileoperation.h"

//登录
bool login();

//确认输入的命令
Commands findCmd(char cmd[]);

//执行命令
void executeCmd(char cmd[], char arg1[], char arg2[], CmdErrors& err);

//info命令
void info();

//exit命令 区别于exit函数
void exitSys();

//check命令 检查超级块的信息能否对应位图信息
void check();
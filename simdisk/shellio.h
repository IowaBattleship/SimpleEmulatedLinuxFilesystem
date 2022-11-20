#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "structure.h"
#include "sharedmemory.h"

//shell的输入
void shellInput(char cmd[], char arg1[], char arg2[], CmdErrors& err); //命令
void shellInput(char str[], uint len); //单参数

//shell的输出
void shellOutput(std::string str);

//专门处理Y/N的输入
bool shellInputYN();
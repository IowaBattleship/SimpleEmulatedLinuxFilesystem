#pragma once
#include <cstring>
#include <iostream>
#include <Windows.h>

typedef unsigned int uint;

constexpr uint MAX_CONTENT_LEN = 1024; //共享内存大小

struct SharedMemory {
	bool ifShell; //simdisk通知shell使用共享内存
	bool ifSimdisk; //shell通知simdisk使用共享内存
	bool ifNeedInput; //通知shell是否需要输入
	char contents[MAX_CONTENT_LEN]; //共享内存内容

	void clearContents(); //清空内容
};

const char sharedMemoryName[20] = "SimdiskSharedMemory"; //共享内存名
extern HANDLE hMapFile; //共享文件句柄
extern SharedMemory* pMapBuffer; //指向共享内存的指针

//创建共享内存
void createSharedMemory();

//关闭共享内存
void closeSharedMemory();
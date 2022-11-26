#include <cstring>
#include <cstdlib>
#include <iostream>
#include <Windows.h>

typedef unsigned int uint;

constexpr uint MAX_CONTENT_LEN = 1024; //共享内存大小
constexpr uint MAX_COMMAND_LEN = 256; //命令/路径 最大长度
constexpr uint MAX_INPUT_LEN = MAX_COMMAND_LEN * 3 + 2; //单行输入最大长度

struct SharedMemory {
	bool ifShell; //simdisk通知shell使用共享内存
	bool ifSimdisk; //shell通知simdisk使用共享内存
	bool ifNeedInput; //通知shell是否需要输入
	char contents[MAX_CONTENT_LEN]; //共享内存内容

	void clearContents()
	{
		memset(this->contents, 0, sizeof(this->contents));
	}
};

const char sharedMemoryName[20] = "SimdiskSharedMemory"; //共享内存名
HANDLE hMapFile; //共享文件句柄
SharedMemory* pMapBuffer; //指向共享内存的指针

char cmd[MAX_INPUT_LEN];

//打开共享内存
bool openSharedMemory()
{
	//创建映射对象
	hMapFile = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,
		0,
		(LPCWSTR)sharedMemoryName
	);

	//创建失败
	if (hMapFile == NULL)
	{
		std::cout << "Can not open shard memory!\nWill retry after 1s\n";
		return false;
	}

	//指针指向映射后的地址
	pMapBuffer = (SharedMemory*)MapViewOfFile(
		hMapFile,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		sizeof(SharedMemory)
	);

	//失败
	if (pMapBuffer == NULL)
	{
		CloseHandle(hMapFile);
		std::cout << "Can not open shard memory!\nExit shell\n";
		exit(2);
	}

	return true;
}

//关闭共享内存
void closeSharedMemory()
{
	if (pMapBuffer != NULL)
		UnmapViewOfFile(pMapBuffer);
	if (hMapFile != NULL)
		CloseHandle(hMapFile);
}

int main()
{
	system("title shell");

	while (!openSharedMemory())
		Sleep(1000);

	bool ifExit = false;
	while (!ifExit)
	{
		//等待simdisk通知shell可以使用共享内存
		while (!pMapBuffer->ifShell);

		//输入
		if (pMapBuffer->ifNeedInput)
		{
			std::cin.getline(cmd, MAX_INPUT_LEN);

			pMapBuffer->clearContents();
			strcpy_s(pMapBuffer->contents, MAX_CONTENT_LEN, cmd);
		}
		//输出
		//exit命令
		else if (!strcmp(pMapBuffer->contents, "exit"))
			ifExit = true;
		else
			std::cout << pMapBuffer->contents;

		//通知simdisk使用共享内存
		pMapBuffer->ifNeedInput = false;
		pMapBuffer->ifShell = false;
		pMapBuffer->ifSimdisk = true;
	}

	closeSharedMemory();

	return 0;
}
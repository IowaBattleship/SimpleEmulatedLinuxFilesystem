#include "sharedmemory.h"

HANDLE hMapFile;
SharedMemory* pMapBuffer;

void SharedMemory::clearContents()
{
	memset(this->contents, 0, sizeof(this->contents));
}

void createSharedMemory()
{
	//创建映射对象
	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,
		NULL,
		PAGE_READWRITE,
		0,
		sizeof(SharedMemory),
		(LPCWSTR)sharedMemoryName
	);

	//创建失败
	if (hMapFile == NULL)
	{
		std::cout << "Shard memory created failed!\nExit system\n";
		exit(2);
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
		std::cout << "Shard memory created failed!\nExit system\n";
		exit(2);
	}

	pMapBuffer->ifShell = false;
	pMapBuffer->ifSimdisk = true;
	pMapBuffer->ifNeedInput = false;
	pMapBuffer->clearContents();
}

void closeSharedMemory()
{
	if (pMapBuffer != NULL)
		UnmapViewOfFile(pMapBuffer);
	if (hMapFile != NULL)
		CloseHandle(hMapFile);
}
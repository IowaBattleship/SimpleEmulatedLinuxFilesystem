#include "shellio.h"

void shellInput(char cmd[], char arg1[], char arg2[], CmdErrors& err)
{
	//等待shell通知simdisk可以使用共享内存
	while (!pMapBuffer->ifSimdisk);

	//通知shell进行输入
	pMapBuffer->ifSimdisk = false;
	pMapBuffer->ifNeedInput = true;
	pMapBuffer->ifShell = true;

	//等待shell通知simdisk可以使用共享内存
	while (!pMapBuffer->ifSimdisk);

	std::cout << pMapBuffer->contents << '\n';

	std::stringstream cmdStream;
	cmdStream << pMapBuffer->contents;
	cmdStream >> cmd >> arg1 >> arg2;
	//如果参数大于3个 说明参数过多
	if (!cmdStream.eof())
		err = CmdErrors::TOO_MANY_PARAMETERS;
}

void shellInput(char str[], uint len)
{
	//等待shell通知simdisk可以使用共享内存
	while (!pMapBuffer->ifSimdisk);

	//通知shell进行输入
	pMapBuffer->ifSimdisk = false;
	pMapBuffer->ifNeedInput = true;
	pMapBuffer->ifShell = true;

	//等待shell通知simdisk可以使用共享内存
	while (!pMapBuffer->ifSimdisk);

	std::cout << pMapBuffer->contents << '\n';

	strcpy_s(str, len, pMapBuffer->contents);
}

void shellOutput(const std::string str)
{
	//等待shell通知simdisk可以使用共享内存
	while (!pMapBuffer->ifSimdisk);

	//清空内容
	pMapBuffer->clearContents();

	std::cout << str;
	strcpy_s(pMapBuffer->contents, str.c_str());
	pMapBuffer->ifSimdisk = false;
	pMapBuffer->ifNeedInput = false;
	pMapBuffer->ifShell = true; //通知shell可以使用
}

bool shellInputYN()
{
	char buffer[MAX_COMMAND_LEN];
	while (1)
	{
		shellInput(buffer, MAX_COMMAND_LEN);
		if (!strcmp(buffer, "N") || !strcmp(buffer, "n"))
			return false;
		if (!strcmp(buffer, "Y") || !strcmp(buffer, "y"))
			return true;
		shellOutput("Please input Y/N: ");
	}
}

void shellExit()
{
	//等待shell通知simdisk可以使用共享内存
	while (!pMapBuffer->ifSimdisk);

	//清空内容
	pMapBuffer->clearContents();

	strcpy_s(pMapBuffer->contents, "exit");
	pMapBuffer->ifSimdisk = false;
	pMapBuffer->ifNeedInput = false;
	pMapBuffer->ifShell = true; //通知shell可以使用
}
#include <cstdio>
#include "simdisk.h"

int main()
{
	system("title simdisk");

	//创建共享内存
	createSharedMemory();

	//加载数据 若未存在虚拟硬盘则初始化
	if (loadDataFromDisk())
		//登录进入文件系统
		if (login())
		{
			ifExit = false;
			char cmd[MAX_COMMAND_LEN], arg1[MAX_COMMAND_LEN], arg2[MAX_COMMAND_LEN];
			while (!ifExit)
			{
				CmdErrors cmdInputErr = CmdErrors::NONE_ERROR;
				memset(cmd, 0, sizeof(cmd));
				memset(arg1, 0, sizeof(arg1));
				memset(arg2, 0, sizeof(arg2));
				shellOutput(std::string("\n") + std::string(currentUser.userName) +
					"@pc:" + std::string(currentPath) +
					(currentUser.userType == UserType::ADMIN ? "# " : "$ "));
				shellInput(cmd, arg1, arg2, cmdInputErr);

				//未输入命令
				if (!strlen(cmd))
					continue;
				//参数过多
				if (cmdInputErr == CmdErrors::TOO_MANY_PARAMETERS)
				{
					shellOutput("\nToo many parameters\n");
					continue;
				}

				executeCmd(cmd, arg1, arg2, cmdInputErr);
				switch (cmdInputErr)
				{
				case CmdErrors::TOO_MANY_PARAMETERS:
					shellOutput("\nToo many parameters\n");
					break;

				case CmdErrors::WRONG_PARAMETER:
					shellOutput("\nWrong parameter\n");
					break;

				case CmdErrors::MISSING_PARAMETERS:
					shellOutput("\nMissing parameters\n");
					break;

				case CmdErrors::CMD_NOT_FOUND:
					shellOutput("\nCommand not found\n");
					break;

				default: //none error
					break;
				}
			}
		}

	//通知shell退出
	shellExit();

	//关闭共享内存
	closeSharedMemory();

	return 0;
}
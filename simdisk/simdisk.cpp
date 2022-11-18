#include <cstdio>
#include "simdisk.h"

int main()
{
	loadDataFromDisk();
	if (login())
	{
		//user@pc:path ($general/#admin)
		char cmd[MAX_COMMAND_LEN], arg1[MAX_COMMAND_LEN], arg2[MAX_COMMAND_LEN];
		while (1)
		{
			memset(cmd, 0, sizeof(cmd));
			memset(arg1, 0, sizeof(arg1));
			memset(arg2, 0, sizeof(arg2));
			shellOutput(std::string(currentUser.userName) +
				"@pc:" + std::string(currentPath) +
				(currentUser.userType == UserType::ADMIN ? " # " : " $ "));

		}
	}
	return 0;
}
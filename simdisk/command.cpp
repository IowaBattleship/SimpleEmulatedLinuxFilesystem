#include "command.h"

bool login()
{
	shellOutput("\n----LOGIN----\n");

	char userName[MAX_USER_NAME_LEN];
	char userPassword[MAX_USER_PASSWORD_LEN];
	while (1)
	{
		shellOutput("\nPlease input username: ");
		shellInput(userName, MAX_USER_NAME_LEN);
		shellOutput("Please input password: ");
		shellInput(userPassword, MAX_USER_PASSWORD_LEN);

		bool ifMatch = false;
		for (const auto& user : users)
			if (!strcmp(user.userName, userName) &&
				!strcmp(user.userPassword, userPassword))
			{
				currentUser = user;
				ifMatch = true;
				break;
			}
		if (ifMatch)
			break;
		shellOutput("Incorrect username or password\n");
		shellOutput("Whether to retry: (Y/N) ");
		if (!shellInputYN())
		{
			shellOutput("Exit system\n");
			return false;
		}
	}

	shellOutput("Login successful\n");
	return true;
}

Commands findCmd(char cmd[])
{
	if (!strcmp(cmd, "info"))
		return Commands::info;
	else if (!strcmp(cmd, "cd"))
		return Commands::cd;
	else if (!strcmp(cmd, "dir"))
		return Commands::dir;
	else if (!strcmp(cmd, "md"))
		return Commands::md;
	else if (!strcmp(cmd, "rd"))
		return Commands::rd;
	else if (!strcmp(cmd, "newfile"))
		return Commands::newfile;
	else if (!strcmp(cmd, "cat"))
		return Commands::cat;
	else if (!strcmp(cmd, "copy"))
		return Commands::copy;
	else if (!strcmp(cmd, "del"))
		return Commands::del;
	else if (!strcmp(cmd, "check"))
		return Commands::check;
	else if (!strcmp(cmd, "exit"))
		return Commands::exit;

	return Commands::NOT_FOUND;
}

void executeCmd(char cmd[], char arg1[], char arg2[], CmdErrors& err)
{
	switch (findCmd(cmd))
	{
	case Commands::info:
		if (strlen(arg1))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else
			info();
		break;

	case Commands::cd:
		if (strlen(arg2))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else if (!strlen(arg1))
			err = CmdErrors::MISSING_PARAMETERS;
		else
			cd(arg1);
		break;

	case Commands::dir:
		if (strlen(arg2))
		{
			if (!strcmp(arg2, "/s"))
				ls(arg1, true);
			else
				err = CmdErrors::WRONG_PARAMETER;
		}
		else if (strlen(arg1))
		{
			if (!strcmp(arg1, "/s"))
				ls(".", true);
			else
				ls(arg1, false);
		}
		else
			ls(".", false);
		break;

	case Commands::md:
		if (strlen(arg2))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else if (!strlen(arg1))
			err = CmdErrors::MISSING_PARAMETERS;
		else
			md(arg1);
		break;

	case Commands::rd:
		if (strlen(arg2))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else if (!strlen(arg1))
			err = CmdErrors::MISSING_PARAMETERS;
		else
			rd(arg1);
		break;

	case Commands::newfile:
		if (strlen(arg2))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else if (!strlen(arg1))
			err = CmdErrors::MISSING_PARAMETERS;
		else
			newfile(arg1);
		break;

	case Commands::cat:
		if (strlen(arg2))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else if (!strlen(arg1))
			err = CmdErrors::MISSING_PARAMETERS;
		else
			cat(arg1);
		break;

	case Commands::copy:
		if (!strlen(arg2))
			err = CmdErrors::MISSING_PARAMETERS;
		else
			copy(arg1, arg2);
		break;

	case Commands::del:
		if (strlen(arg2))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else if (!strlen(arg1))
			err = CmdErrors::MISSING_PARAMETERS;
		else
			del(arg1);
		break;

	case Commands::check:
		if (strlen(arg1))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else
			check();
		break;

	case Commands::exit:
		if (strlen(arg1))
			err = CmdErrors::TOO_MANY_PARAMETERS;
		else
			exitSys();
		break;

	default: //cmd not found
		err = CmdErrors::CMD_NOT_FOUND;
		break;
	}
}

void info()
{
	typedef std::string string;
	using std::to_string;
	shellOutput(
		string("\n----------Simple Emulated Linux File System Info----------") +
		string("\nDisk size: ") +
		to_string(superBlock.blockSize * superBlock.blockNum) + string(" B") +
		string("\nDisk free size: ") +
		to_string(superBlock.blockSize * superBlock.freeBlockNum) + string(" B") +
		string("\nBlock size: ") + to_string(superBlock.blockSize) + string(" B") +
		string("\nNumber of blocks: ") + to_string(superBlock.blockNum) +
		string("\nNumber of free blocks: ") + to_string(superBlock.freeBlockNum) +
		string("\n----------------------------------------------------------\n")
	);
}

void exitSys()
{
	writeSpecifiedInfo(InfoType::SUPER_BLOCK);
	writeSpecifiedInfo(InfoType::BLOCK_BITMAP);
	writeSpecifiedInfo(InfoType::INODE_BITMAP);
	writeSpecifiedInfo(InfoType::INODES);
	writeSpecifiedInfo(iNodes[currentDir.iNodeId].fileAddr, currentDir);
	ifExit = true;
}

void check()
{
	uint freeBlockNum = 0, freeINodeNum = 0;
	for (auto blockBit : blockBitmap)
		if (!blockBit)
			freeBlockNum++;
	for (auto iNodeBit : iNodeBitmap)
		if (!iNodeBit)
			freeINodeNum++;

	//不对应则对超级块信息进行修正
	if (superBlock.freeBlockNum != freeBlockNum ||
		superBlock.freeINodeNum != freeINodeNum)
	{
		shellOutput("\nSystem exception, start repairing...\n");
		superBlock.freeBlockNum = freeBlockNum;
		superBlock.freeINodeNum = freeINodeNum;
		writeSpecifiedInfo(InfoType::SUPER_BLOCK);
		shellOutput("Repair completed\n");
	}
	else
		shellOutput("\nThe system is running normally\n");
}
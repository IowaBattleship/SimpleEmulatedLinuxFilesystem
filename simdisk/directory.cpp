#include "directory.h"

int findDirInDir(Directory& dir, const char targetDir[])
{
	//遍历所有子目录
	for (int i = 0; i < dir.fileNum; i++)
		if (!strcmp(dir.fileInterface[i].fileName, targetDir) &&
			iNodes[dir.fileInterface[i].iNodeId].fileType == FileType::DIRECTORY)
		{
			return i;
		}
	return -1;
}

bool enterDirInDir(Directory& dir, const char targetDir[])
{
	int tmp = findDirInDir(dir, targetDir);
	if (~tmp)
	{
		readSpecifiedInfo(iNodes[dir.fileInterface[tmp].iNodeId].fileAddr, dir);
		return true;
	}
	return false;
}

bool enterDirInDir(Directory& dir, const char targetDir[], int& dirPos)
{
	int tmp = findDirInDir(dir, targetDir);
	if (~tmp)
	{
		dirPos = tmp;
		readSpecifiedInfo(iNodes[dir.fileInterface[dirPos].iNodeId].fileAddr, dir);
		return true;
	}
	return false;
}


bool getDirFromPath(const char path[], Directory& targetDir)
{
	//当前目录
	if (!strcmp(path, "."))
	{
		targetDir = currentDir;
		return true;
	}
	//父目录
	else if (!strcmp(path, ".."))
	{
		readSpecifiedInfo(iNodes[currentDir.fileInterface[0].iNodeId].fileAddr, targetDir);
		return true;
	}
	else
	{
		char tmpPath[MAX_COMMAND_LEN];
		strcpy_s(tmpPath, path);
		const char sep[2] = "/";
		char* buffer = NULL;
		//通过strtok划分出每一级目录
		char* tmpDirName = strtok_s(tmpPath, sep, &buffer);

		//以当前目录为相对路径
		if (!strcmp(tmpDirName, "."))
		{
			targetDir = currentDir;
			tmpDirName = strtok_s(NULL, sep, &buffer);
			while (tmpDirName != NULL)
			{
				if (!enterDirInDir(targetDir, tmpDirName))
					return false;
				tmpDirName = strtok_s(NULL, sep, &buffer);
			}
			return true;
		}
		//以父目录为相对路径
		else if (!strcmp(tmpDirName, ".."))
		{
			readSpecifiedInfo(iNodes[currentDir.fileInterface[0].iNodeId].fileAddr, targetDir);
			tmpDirName = strtok_s(NULL, sep, &buffer);
			while (tmpDirName != NULL)
			{
				if (!enterDirInDir(targetDir, tmpDirName))
					return false;
				tmpDirName = strtok_s(NULL, sep, &buffer);
			}
			return true;
		}
		//绝对路径
		else if (!strcmp(tmpDirName, "root"))
		{
			//从根目录往下找
			readSpecifiedInfo(iNodes[0].fileAddr, targetDir);
			tmpDirName = strtok_s(NULL, sep, &buffer);
			while (tmpDirName != NULL)
			{
				if (!enterDirInDir(targetDir, tmpDirName))
					return false;
				tmpDirName = strtok_s(NULL, sep, &buffer);
			}
			return true;
		}
		//以当前目录为相对路径的子目录
		else
		{
			targetDir = currentDir;
			while (tmpDirName != NULL)
			{
				if (!enterDirInDir(targetDir, tmpDirName))
					return false;
				tmpDirName = strtok_s(NULL, sep, &buffer);
			}
			return true;
		}
	}
	return false;
}

void dividePathAndFileOrDir(char pathAndFileOrDir[], char path[], char fileOrDirName[])
{
	int dirNamePos = -1, len = strlen(pathAndFileOrDir);
	for (int i = len - 1; i; i--)
		if (pathAndFileOrDir[i] == '/')
		{
			dirNamePos = i;
			break;
		}

	//采用相对路径并且就在当前目录下
	if (!~dirNamePos)
	{
		strcpy_s(path, 2, ".");
		strcpy_s(fileOrDirName, strlen(pathAndFileOrDir) + 1, pathAndFileOrDir);
		return;
	}

	for (int i = 0; i < dirNamePos; i++)
		path[i] = pathAndFileOrDir[i];
	for (int i = dirNamePos + 1, j = 0; i < len; i++, j++)
		fileOrDirName[j] = pathAndFileOrDir[i];
}

void getPathFromDir(Directory& dir, char path[])
{
	//非根目录 继续递归
	if (dir.iNodeId != 0)
	{
		Directory fatherDir;
		//读取父目录
		readSpecifiedInfo(iNodes[dir.fileInterface[0].iNodeId].fileAddr, fatherDir);
		//递归
		getPathFromDir(fatherDir, path);
	}

	//递归完成 写入path
	//根目录
	if (!dir.iNodeId)
	{
		memset(path, 0, sizeof(path));
		strcpy_s(path, strlen(dir.dirName) + 1, dir.dirName);
	}
	else
	{
		strcat_s(path, strlen(path) + 2, "/");
		strcat_s(path, strlen(path) + strlen(dir.dirName) + 1, dir.dirName);
	}
}

void displayDir(Directory& dir)
{
	typedef std::string string;
	using std::to_string;

	shellOutput(string("\n") + string(dir.dirName) + string(":\n"));
	//除去父目录和当前目录
	for (int i = 2; i < dir.fileNum; i++)
	{
		INode fileINode = iNodes[dir.fileInterface[i].iNodeId];
		shellOutput(
			string("Name: ") + string(dir.fileInterface[i].fileName) +
			string("\nType: ") +
			(fileINode.fileType == FileType::DIRECTORY ? string("DIRECTORY") : string("FILE")) +
			string("\nSize: ") + to_string(fileINode.fileSize) + string(" B") +
			string("\nPermission: ") +
			(fileINode.filePermission == FilePermission::READ_ONLY ? string("R") : string("RW")) +
			string("\nOwner(UserId): ") + to_string(fileINode.fileUserId) +
			string("\nifModified: ") + (fileINode.ifModified ? string("TRUE") : string("FALSE")) +
			string("\n")
		);
	}
}

bool ifDirContainDir(Directory& aDir, Directory& bDir)
{
	Directory tmp = bDir;
	//通过b目录的父目录往上搜寻
	while (tmp.iNodeId)
	{
		readSpecifiedInfo(iNodes[tmp.fileInterface[0].iNodeId].fileAddr, tmp);
		if (tmp.iNodeId == aDir.iNodeId)
			return true;
	}
	return false;
}

void freeUpDir(Directory& dir)
{
	for (int i = 2; i < dir.fileNum; i++)
	{
		//对目录递归释放资源
		if (iNodes[dir.fileInterface[i].iNodeId].fileType == FileType::DIRECTORY)
		{
			Directory subDir;
			readSpecifiedInfo(iNodes[dir.fileInterface[i].iNodeId].fileAddr, subDir);
			freeUpDir(subDir);
		}
		//文件直接释放
		else
			freeUpFile(dir.fileInterface[i].iNodeId);
	}

	//释放当前目录所占资源
	freeBlock((iNodes[dir.iNodeId].fileAddr - superBlock.firstDataAddr) / BLOCK_SIZE,
		iNodes[dir.iNodeId].fileBlockNum);
	freeINode(dir.iNodeId);
}

void md(char pathAndDirName[])
{
	char dirName[MAX_FILE_NAME_LEN] = "";
	char path[MAX_COMMAND_LEN] = "";
	dividePathAndFileOrDir(pathAndDirName, path, dirName);
	Directory workDir;

	//根据path进入工作目录
	if (getDirFromPath(path, workDir))
	{
		//查询是否有重名目录
		if (~findDirInDir(workDir, dirName))
		{
			shellOutput(std::string("\n\"") + std::string(dirName) +
				std::string("\" exists under path \"") +
				std::string(path) + std::string("\"\n"));
		}
		else
		{
			uint dirAddr = 0;
			int blockFirstPos = -1;
			std::tie(dirAddr, blockFirstPos) = allocateBlock(DIRECTORY_BLOCK_NUM);

			//分配成功
			if (~blockFirstPos)
			{
				int iNodePos = allocateINode();
				//分配成功
				if (~iNodePos)
				{
					//创建目录
					Directory mdDir;
					strcpy_s(mdDir.dirName, dirName);
					mdDir.iNodeId = iNodePos;
					mdDir.fileNum = 2;
					mdDir.fileInterface[0].iNodeId = workDir.iNodeId; //父目录为工作目录
					strcpy_s(mdDir.fileInterface[0].fileName, "..");
					mdDir.fileInterface[1].iNodeId = mdDir.iNodeId; //当前目录
					strcpy_s(mdDir.fileInterface[1].fileName, ".");

					//i节点信息
					iNodes[iNodePos] = INode(
						FileType::DIRECTORY,
						currentUser.userId,
						DIRECTORY_SIZE,
						DIRECTORY_BLOCK_NUM,
						false,
						dirAddr,
						FilePermission::READ_WRITE
					);

					//修改父目录信息
					strcpy_s(workDir.fileInterface[workDir.fileNum].fileName, dirName);
					workDir.fileInterface[workDir.fileNum].iNodeId = iNodePos;
					workDir.fileNum++;
					//若工作目录即为当前目录 更新当前目录
					if (workDir.iNodeId == currentDir.iNodeId)
						currentDir = workDir;

					//写入创建的目录、父目录、i节点信息
					writeSpecifiedInfo(iNodePos, 1, InfoType::INODES);
					writeSpecifiedInfo(dirAddr, mdDir);
					writeSpecifiedInfo(iNodes[workDir.iNodeId].fileAddr, workDir);

					shellOutput("\nCreated successfully\n");
				}
				else
				{
					shellOutput("\nNo free iNode, failed to create directory\n");
					//释放分配的盘块
					freeBlock(blockFirstPos, DIRECTORY_BLOCK_NUM);
				}
			}
			else
				shellOutput("\nNot enough disk space, failed to create directory\n");
		}
	}
	else
	{
		shellOutput(std::string("\nPath \"") +
			std::string(path) + std::string("\" not found\n"));
	}
}

void cd(char path[])
{
	Directory tmp;
	if (getDirFromPath(path, tmp))
	{
		currentDir = tmp;
		getPathFromDir(tmp, currentPath);
	}
	else
		shellOutput(std::string("\nPath \"") +
			std::string(path) + std::string("\" not found\n"));
}

void ls(const char path[], bool ifDisplaySubDir)
{
	Directory dir;
	if (getDirFromPath(path, dir))
	{
		//显示当前目录下内容
		displayDir(dir);
		//带/s参数 显示所有子目录
		if (ifDisplaySubDir)
		{
			Directory subDir;
			//除去父目录和当前目录
			for (int i = 2; i < dir.fileNum; i++)
				if (iNodes[dir.fileInterface[i].iNodeId].fileType == FileType::DIRECTORY)
				{
					readSpecifiedInfo(iNodes[dir.fileInterface[i].iNodeId].fileAddr, subDir);
					displayDir(subDir);
				}
		}
	}
	else
		shellOutput(std::string("\nPath \"") +
			std::string(path) + std::string("\" not found\n"));
}

void rd(char pathAndDirName[])
{
	char dirName[MAX_FILE_NAME_LEN] = "";
	char path[MAX_COMMAND_LEN] = "";
	dividePathAndFileOrDir(pathAndDirName, path, dirName);
	Directory workDir;

	//根据path进入工作目录
	if (getDirFromPath(path, workDir))
	{
		//查询是否存在该目录
		int dirPos = -1;
		Directory rdDir = workDir;
		if (enterDirInDir(rdDir, dirName, dirPos))
		{
			//若要删除的目录包含当前所在目录 则拒绝删除
			//此处保证根目录不会被删除
			if (ifDirContainDir(rdDir, currentDir))
			{
				shellOutput(std::string("\n\"") + std::string(dirName) +
					std::string("\" contain work directory\n") +
					std::string("Please retry \"rd\" after changing work directory\n"));
				return;
			}

			//若要删除的目录包含文件/目录 则询问是否继续删除
			if (rdDir.fileNum > 2)
			{
				shellOutput(std::string("\n\"") + std::string(dirName) +
					std::string("\" contain sub directories or sub files\n") +
					std::string("Whether to continue the \"rd\" command: (Y/N) "));
				if (!shellInputYN())
					return;
			}

			//释放删除目录所占资源
			freeUpDir(rdDir);

			//修改父目录信息
			for (int i = dirPos + 1; i < workDir.fileNum; i++)
			{
				strcpy_s(workDir.fileInterface[i - 1].fileName, workDir.fileInterface[i].fileName);
				workDir.fileInterface[i - 1].iNodeId = workDir.fileInterface[i].iNodeId;
			}
			workDir.fileNum--;
			//若工作目录即为当前目录 更新当前目录
			if (workDir.iNodeId == currentDir.iNodeId)
				currentDir = workDir;

			//写入工作目录
			writeSpecifiedInfo(iNodes[workDir.iNodeId].fileAddr, workDir);

			shellOutput("\nDeleted successfully\n");
		}
		else
		{
			shellOutput(std::string("\n\"") + std::string(dirName) +
				std::string("\" not exists under path \"") +
				std::string(path) + std::string("\"\n"));
		}
	}
	else
	{
		shellOutput(std::string("\nPath \"") +
			std::string(path) + std::string("\" not found\n"));
	}
}
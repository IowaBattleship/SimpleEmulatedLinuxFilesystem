#include "fileoperation.h"

int findFileInDir(Directory& dir, const char targetFile[])
{
	//遍历所有子文件
	for (int i = 0; i < dir.fileNum; i++)
		if (!strcmp(dir.fileInterface[i].fileName, targetFile) &&
			iNodes[dir.fileInterface[i].iNodeId].fileType == FileType::FILE)
		{
			return i;
		}
	return -1;
}

void freeUpFile(const int fileInodePos)
{
	freeBlock((iNodes[fileInodePos].fileAddr - superBlock.firstDataAddr) / BLOCK_SIZE,
		iNodes[fileInodePos].fileBlockNum);
	freeINode(fileInodePos);
}

bool createFile(Directory& dir, char fileName[], char fileContent[])
{
	uint fileSize = strlen(fileContent) + 1;
	uint blockNum = ceil(fileSize * 1.0 / BLOCK_SIZE); //文件占用盘块数
	//分配盘块
	uint fileAddr = 0;
	int blockFirstPos = -1;
	std::tie(fileAddr, blockFirstPos) = allocateBlock(blockNum);

	if (~blockFirstPos)
	{
		//分配i节点
		int iNodePos = allocateINode();
		if (~iNodePos)
		{
			//创建文件
			//i节点信息
			iNodes[iNodePos] = INode(
				FileType::FILE,
				currentUser.userId,
				fileSize,
				blockNum,
				false,
				fileAddr,
				FilePermission::READ_WRITE
			);

			//修改目录信息
			strcpy_s(dir.fileInterface[dir.fileNum].fileName, fileName);
			dir.fileInterface[dir.fileNum].iNodeId = iNodePos;
			dir.fileNum++;
			//若工作目录即为当前目录 更新当前目录
			if (dir.iNodeId == currentDir.iNodeId)
				currentDir = dir;

			//写入创建的文件、目录、i节点信息
			writeSpecifiedInfo(iNodePos, 1, InfoType::INODES);
			writeSpecifiedInfo(fileAddr, fileContent);
			writeSpecifiedInfo(iNodes[dir.iNodeId].fileAddr, dir);

			return true;
		}
		else
		{
			shellOutput("\nNo free iNode, failed to create file\n");
			//释放分配的盘块
			freeBlock(blockFirstPos, DIRECTORY_BLOCK_NUM);
		}
	}
	else
		shellOutput("\nNot enough disk space, failed to create file\n");
	return false;
}

bool ifWinAndDividePath(char path[])
{
	const char host[7] = "<host>";
	if (strstr(path, host) == NULL)
		return false;

	std::string tmp = std::string(path).substr(6);
	strcpy_s(path, tmp.length() + 1, tmp.c_str());
	return true;
}

bool ifWinAndDividePath(char pathAndFile[], char path[], char fileName[])
{
	const char host[7] = "<host>";
	if (strstr(pathAndFile, host) == NULL)
		return false;

	int fileNamePos = -1, len = strlen(pathAndFile);
	for (int i = len - 1; ~i; i--)
		if (pathAndFile[i] == '\\')
		{
			fileNamePos = i;
			break;
		}

	for (int i = 6, j = 0; i < len; i++, j++)
		path[j] = pathAndFile[i];
	for (int i = fileNamePos + 1, j = 0; i < len; i++, j++)
		fileName[j] = pathAndFile[i];
	return true;
}

void newfile(char pathAndFileName[])
{
	char fileName[MAX_FILE_NAME_LEN] = "";
	char path[MAX_COMMAND_LEN] = "";
	dividePathAndFileOrDir(pathAndFileName, path, fileName);
	Directory workDir;

	//根据path进入工作目录
	if (getDirFromPath(path, workDir))
	{
		//查询是否有重名文件
		if (~findFileInDir(workDir, fileName))
		{
			shellOutput(std::string("\n\"") + std::string(fileName) +
				std::string("\" exists under path \"") +
				std::string(path) + std::string("\"\n"));
		}
		else
		{
			//输入文件内容以确认文件大小
			char buffer[MAX_FILE_SIZE] = "";
			shellOutput("\nPlease enter the content of the file (Type \"ENTER\" to terminate - MAX SIZE 1023): \n");
			shellInput(buffer, MAX_FILE_SIZE);

			//创建该文件
			if (createFile(workDir, fileName, buffer))
				shellOutput("\nCreated successfully\n");
		}
	}
	else
	{
		shellOutput(std::string("\nPath \"") +
			std::string(path) + std::string("\" not found\n"));
	}
}

void cat(char pathAndFileName[])
{
	char fileName[MAX_FILE_NAME_LEN] = "";
	char path[MAX_COMMAND_LEN] = "";
	dividePathAndFileOrDir(pathAndFileName, path, fileName);
	Directory workDir;

	//根据path进入工作目录
	if (getDirFromPath(path, workDir))
	{
		//查询是否有该文件
		int filePos = findFileInDir(workDir, fileName);
		if (~filePos)
		{
			char buffer[MAX_FILE_SIZE] = "";
			readSpecifiedInfo(iNodes[workDir.fileInterface[filePos].iNodeId].fileAddr,
				iNodes[workDir.fileInterface[filePos].iNodeId].fileSize, buffer);
			shellOutput(std::string("\n") + std::string(fileName) +
				std::string("content: \n") + std::string(buffer) + std::string("\n"));
		}
		else
		{
			shellOutput(std::string("\n\"") + std::string(fileName) +
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

void del(char pathAndFileName[])
{
	char fileName[MAX_FILE_NAME_LEN] = "";
	char path[MAX_COMMAND_LEN] = "";
	dividePathAndFileOrDir(pathAndFileName, path, fileName);
	Directory workDir;

	//根据path进入工作目录
	if (getDirFromPath(path, workDir))
	{
		//查询是否有该文件
		int filePos = findFileInDir(workDir, fileName);
		if (~filePos)
		{
			//释放该文件所占盘块、i节点
			freeUpFile(workDir.fileInterface[filePos].iNodeId);

			//修改所属目录信息
			for (int i = filePos + 1; i < workDir.fileNum; i++)
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
			shellOutput(std::string("\n\"") + std::string(fileName) +
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

void copy(char sourcePathAndFileName[], char targetPath[])
{
	char sourcePath[MAX_COMMAND_LEN] = "";
	char fileName[MAX_COMMAND_LEN] = "";
	//源路径为Windows
	if (ifWinAndDividePath(sourcePathAndFileName, sourcePath, fileName))
	{
		std::ifstream inFile(sourcePath, std::ios::in | std::ios::binary);
		if (!inFile.is_open())
		{
			shellOutput(std::string("\n\"") + std::string(fileName) +
				std::string("\" not exists under path \"") +
				std::string(sourcePath) + std::string("\"\n"));
			return;
		}

		//获取文件大小
		uint st = inFile.tellg();
		inFile.seekg(0, std::ios::end);
		uint ed = inFile.tellg();
		uint fileSize = ed - st;
		//过大则拒绝copy
		if (fileSize >= MAX_FILE_SIZE)
		{
			shellOutput(std::string("\n\"") + std::string(fileName) +
				std::string("\" is too big, can not copy\n"));
			return;
		}

		//读入文件
		char buffer[MAX_FILE_SIZE] = "";
		inFile.seekg(0, std::ios::beg);
		inFile.read(buffer, fileSize);

		inFile.close();

		Directory workDir;
		if (getDirFromPath(targetPath, workDir))
		{
			//创建该文件
			if (createFile(workDir, fileName, buffer))
				shellOutput("\nCopy successfully\n");
		}
		else
		{
			shellOutput(std::string("\nPath \"") +
				std::string(targetPath) + std::string("\" not found\n"));
		}
	}
	//目标路径为Windows
	else if (ifWinAndDividePath(targetPath))
	{
		dividePathAndFileOrDir(sourcePathAndFileName, sourcePath, fileName);
		Directory workDir;
		if (getDirFromPath(sourcePath, workDir))
		{
			//查询是否有该文件
			int filePos = findFileInDir(workDir, fileName);
			if (~filePos)
			{
				//读取该文件
				char buffer[MAX_FILE_SIZE] = "";
				uint fileSize = iNodes[workDir.fileInterface[filePos].iNodeId].fileSize;
				readSpecifiedInfo(iNodes[workDir.fileInterface[filePos].iNodeId].fileAddr,
					fileSize, buffer);

				std::ofstream outFile(std::string(targetPath) + std::string("\\") +
					std::string(fileName), std::ios::out | std::ios::binary);
				if (!outFile.is_open())
				{
					shellOutput(std::string("\nPath \"") +
						std::string(targetPath) + std::string("\" not found\n"));
					return;
				}
				//写入该文件
				outFile.write(buffer, fileSize);

				outFile.close();

				shellOutput("\nCopy successfully\n");
			}
			else
			{
				shellOutput(std::string("\n\"") + std::string(fileName) +
					std::string("\" not exists under path \"") +
					std::string(sourcePath) + std::string("\"\n"));
			}
		}
		else
		{
			shellOutput(std::string("\nPath \"") +
				std::string(sourcePath) + std::string("\" not found\n"));
		}
	}
	//系统内部copy
	else
	{
		dividePathAndFileOrDir(sourcePathAndFileName, sourcePath, fileName);
		Directory sourceDir, targetDir;
		if (getDirFromPath(sourcePath, sourceDir))
		{
			//查询是否有该文件
			int filePos = findFileInDir(sourceDir, fileName);
			if (~filePos)
			{
				//读取该文件
				char buffer[MAX_FILE_SIZE] = "";
				uint fileSize = iNodes[sourceDir.fileInterface[filePos].iNodeId].fileSize;
				readSpecifiedInfo(iNodes[sourceDir.fileInterface[filePos].iNodeId].fileAddr,
					fileSize, buffer);

				//查询目标路径
				if (getDirFromPath(targetPath, targetDir))
				{
					//查询是否有重名文件
					if (~findFileInDir(targetDir, fileName))
					{
						shellOutput(std::string("\n\"") + std::string(fileName) +
							std::string("\" exists under path \"") +
							std::string(targetPath) + std::string("\"\n"));
					}
					else
					{
						//创建该文件
						if (createFile(targetDir, fileName, buffer))
							shellOutput("\nCopy successfully\n");
					}
				}
				else
				{
					shellOutput(std::string("\nPath \"") +
						std::string(sourcePath) + std::string("\" not found\n"));
				}
			}
			else
			{
				shellOutput(std::string("\n\"") + std::string(fileName) +
					std::string("\" not exists under path \"") +
					std::string(sourcePath) + std::string("\"\n"));
			}
		}
		else
		{
			shellOutput(std::string("\nPath \"") +
				std::string(sourcePath) + std::string("\" not found\n"));
		}
	}
}
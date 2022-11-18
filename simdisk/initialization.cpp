#include "initialization.h"

bool loadDataFromDisk()
{
	shellOutput("\nLoading data from virtual disk...\n");

	//读取系统数据 若失败 尝试初始化数据
	if (!readSystemData())
	{
		shellOutput("Trying to initialize data\n");
		return initAllData();
	}

	//读取用户信息
	if (!readUserInfo())
	{
		shellOutput("Exit system\n");
		return false;
	}

	//初始化当前路径为根目录
	strcpy_s(currentPath, "root/");

	return true;
}

bool initAllData()
{
	shellOutput("\nInitializing Data...\nTrying to create virtual disk...\n");

	//尝试创建虚拟硬盘
	std::ofstream newVirtualDisk(VIRTUAL_DISK);
	if (!newVirtualDisk.is_open())
	{
		shellOutput("Creation failed!\nExit System\n");
		return false;
	}
	char* buffer = new char[SYSTEM_SIZE];
	memset(buffer, 0, SYSTEM_SIZE);
	newVirtualDisk.write(buffer, SYSTEM_SIZE);
	delete[] buffer;

	shellOutput("Created successfully\n");
	newVirtualDisk.close();


	//初始化超级块
	superBlock.blockNum = BLOCK_NUM;
	superBlock.blockSize = BLOCK_SIZE;
	superBlock.firstDataAddr = FIRST_DATA_ADDR;
	superBlock.freeBlockNum = BLOCK_NUM;
	superBlock.freeINodeNum = INODE_NUM;
	superBlock.iNodeNum = INODE_NUM;

	//初始化i节点
	for (int i = 0; i < INODE_NUM; i++)
	{
		iNodes[i].fileAddr = 0;
		iNodes[i].fileBlockNum = 0;
		iNodes[i].filePermission = FilePermission::READ_ONLY;
		iNodes[i].fileSize = 0;
		iNodes[i].fileType = FileType::FILE;
		iNodes[i].fileUserId = 0;
		iNodes[i].ifModified = false;
	}

	//初始化位图
	for (auto& blockBit : blockBitmap)
		blockBit = false;
	for (auto& iNodeBit : iNodeBitmap)
		iNodeBit = false;

	//初始化根目录
	//i节点
	iNodes[0].fileAddr = FIRST_DATA_ADDR;
	iNodes[0].fileBlockNum = DIRECTORY_BLOCK_NUM;
	iNodes[0].filePermission = FilePermission::READ_ONLY;
	iNodes[0].fileSize = DIRECTORY_SIZE;
	iNodes[0].fileType = FileType::DIRECTORY;
	iNodes[0].fileUserId = 114514;
	iNodes[0].ifModified = false;

	//更新超级块、位图的信息
	superBlock.freeBlockNum -= DIRECTORY_BLOCK_NUM;
	--superBlock.freeINodeNum;
	iNodeBitmap[0] = true;
	for (int i = 0; i < DIRECTORY_BLOCK_NUM; i++)
		blockBitmap[i] = true;

	//设置当前目录为根目录 父目录也为根
	strcpy_s(currentDir.dirName, "root");
	currentDir.iNodeId = 0;
	currentDir.fileNum = 2;
	strcpy_s(currentDir.fileInterface[0].fileName, "..");
	currentDir.fileInterface[0].iNodeId = 0;
	strcpy_s(currentDir.fileInterface[1].fileName, ".");
	currentDir.fileInterface[1].iNodeId = 0;

	//写入虚拟硬盘
	bool err = writeSystemData();
	if (!err)
		return false;

	//初始化用户
	users.clear();

	//设置管理员admin
	shellOutput("\nCreating initial user: admin\n");

	//设置密码
	char adminPassword[MAX_USER_PASSWORD_LEN];
	shellOutput("Please set \"admin\" password: ");
	shellInput(adminPassword, MAX_USER_PASSWORD_LEN);
	users.push_back(User("admin", adminPassword, 114514, UserType::ADMIN));

	//询问是否创建其余用户
	while (1)
	{
		shellOutput("\nWhether to continue to create user: (Y/N) ");
		if (!shellInputYN())
			break;

		char userName[MAX_USER_NAME_LEN];
		char userPassword[MAX_USER_PASSWORD_LEN];

		shellOutput("Please input username: ");
		shellInput(userName, MAX_USER_NAME_LEN);
		shellOutput("Please set password: ");
		shellInput(userPassword, MAX_USER_PASSWORD_LEN);

		users.push_back(User(userName, userPassword, users.size(), UserType::GENERAL));
	}

	//存储用户信息
	err = writeUserInfo();
	if (!err)
		return false;

	//初始化当前路径为根目录
	strcpy_s(currentPath, "root/");

	shellOutput("Initialized successfully\n");
	return true;
}

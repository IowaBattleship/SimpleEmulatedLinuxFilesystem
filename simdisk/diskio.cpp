#include "diskio.h"

bool writeSystemData()
{
	shellOutput("\nWriting system data to virtual disk...\n");
	fsVirtualDisk.open(VIRTUAL_DISK, std::ios::out | std::ios::binary);

	if (!fsVirtualDisk.is_open())
	{
		shellOutput("Virtual disk not found!\nWrite failed\n");
		return false;
	}

	fsVirtualDisk.write((const char*)&superBlock, SUPER_BLOCK_SIZE);
	for (auto blockBit : blockBitmap)
		fsVirtualDisk.write((const char*)&blockBit, sizeof(bool));
	for (auto iNodeBit : iNodeBitmap)
		fsVirtualDisk.write((const char*)&iNodeBit, sizeof(bool));
	for (const auto& iNode : iNodes)
		fsVirtualDisk.write((const char*)&iNode, INODE_SIZE);
	fsVirtualDisk.write((const char*)&currentDir, DIRECTORY_SIZE);

	fsVirtualDisk.close();

	shellOutput("Write successfully\n");
	return true;
}

bool readSystemData()
{
	shellOutput("\nReading system data from virtual disk...\n");

	fsVirtualDisk.open(VIRTUAL_DISK, std::ios::in | std::ios::binary);

	if (!fsVirtualDisk.is_open())
	{
		shellOutput("Virtual disk not found!\nRead failed\n");
		return false;
	}

	fsVirtualDisk.read((char*)&superBlock, SUPER_BLOCK_SIZE);
	for (auto& blockBit : blockBitmap)
		fsVirtualDisk.read((char*)&blockBit, sizeof(bool));
	for (auto& iNodeBit : iNodeBitmap)
		fsVirtualDisk.read((char*)&iNodeBit, sizeof(bool));
	for (int i = 0; i < INODE_NUM; i++)
		fsVirtualDisk.read((char*)&iNodes[i], INODE_SIZE);
	fsVirtualDisk.read((char*)&currentDir, DIRECTORY_SIZE);

	fsVirtualDisk.close();

	shellOutput("Read successfully\n");
	return true;
}

bool writeUserInfo()
{
	shellOutput("\nWriting users info to User.dat...\n");

	std::ofstream userInfoFile("Users.dat", std::ios::out | std::ios::binary);
	if (!userInfoFile.is_open())
	{
		shellOutput("Users.dat file not found!\nWrite failed\n");
		return false;
	}
	int tmp = users.size();
	userInfoFile.write((const char*)&tmp, sizeof(int));
	for (const auto& user : users)
		userInfoFile.write((const char*)&user, USER_SIZE);

	shellOutput("Write successfully\n");
	return true;
}

bool readUserInfo()
{
	shellOutput("\nReading users info from User.dat...\n");

	std::ifstream userInfoFile("Users.dat", std::ios::in | std::ios::binary);
	if (!userInfoFile.is_open())
	{
		shellOutput("Users.dat file not found!\nRead failed\n");
		return false;
	}
	int num = 0; User tmp;
	users.clear();
	userInfoFile.read((char*)&num, sizeof(int));
	for (int i = 0; i < num; i++)
	{
		userInfoFile.read((char*)&tmp, USER_SIZE);
		users.push_back(tmp);
	}

	shellOutput("Read successfully\n");
	return true;
}
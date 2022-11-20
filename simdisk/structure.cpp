#include "structure.h"

extern const char* VIRTUAL_DISK = "SystemVirtualDisk";

//定义文件系统的基础变量
SuperBlock superBlock;
INode iNodes[INODE_NUM];
Directory currentDir; //当前目录
char currentPath[1000]; //当前路径
User currentUser; //当前用户
std::vector<User> users;
bool blockBitmap[BLOCK_NUM]; //盘块位图
bool iNodeBitmap[INODE_NUM]; //i节点位图
std::fstream fsVirtualDisk; //读写虚拟硬盘
bool ifExit; //是否退出

SuperBlock::SuperBlock(uint _blockSize, uint _blockNum, uint _iNodeNum,
	uint _freeBlockNum, uint _freeINodeNum, uint _firstDataAddr)
{
	this->blockSize = _blockSize;
	this->blockNum = _blockNum;
	this->iNodeNum = _iNodeNum;
	this->freeBlockNum = _freeBlockNum;
	this->freeINodeNum = _freeINodeNum;
	this->firstDataAddr = _firstDataAddr;
}

INode::INode(FileType _fileType, uint _fileUserId, uint _fileSize,
	uint _fileBlockNum, bool _ifModified, uint _fileAddr, FilePermission _filePermission)
{
	this->fileType = _fileType;
	this->fileUserId = _fileUserId;
	this->fileSize = _fileSize;
	this->fileBlockNum = _fileBlockNum;
	this->ifModified = _ifModified;
	this->fileAddr = _fileAddr;
	this->filePermission = _filePermission;
}

FileInterface::FileInterface(const char _fileName[], uint _iNodeId)
{
	strcpy_s(this->fileName, _fileName);
	this->iNodeId = _iNodeId;
}

Directory::Directory()
{
	memset(this->dirName, 0, sizeof(this->dirName));
	this->fileNum = 0;
	this->iNodeId = 0;
}

User::User(const char _userName[], const char _userPassword[],
	uint _userId, UserType _userType)
{
	strcpy_s(this->userName, _userName);
	strcpy_s(this->userPassword, _userPassword);
	this->userId = _userId;
	this->userType = _userType;
}
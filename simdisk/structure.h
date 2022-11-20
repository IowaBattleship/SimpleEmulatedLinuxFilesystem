#pragma once
#include <fstream>
#include <cstdio>
#include <vector>

typedef unsigned int uint;

enum class FilePermission { READ_ONLY, READ_WRITE }; //文件读写权限
enum class FileType { FILE, DIRECTORY }; //文件类型 普通文件 or 目录

enum class UserType { ADMIN, GENERAL }; //用户类型

enum class Commands { info, cd, dir, md, rd, newfile, cat,
	copy, del, check, exit, NOT_FOUND }; //所有命令

enum class CmdErrors { TOO_MANY_PARAMETERS, WRONG_PARAMETER,
	MISSING_PARAMETERS, CMD_NOT_FOUND, NONE_ERROR }; //命令错误类型

enum class InfoType { SUPER_BLOCK, INODES, INODE_BITMAP, BLOCK_BITMAP }; //信息类型


constexpr uint BLOCK_SIZE = 1024; //盘块大小 1KB
constexpr uint BLOCK_NUM = 1024 * 100; //盘块数量
constexpr uint INODE_NUM = 1024 * 100; //i节点数量

constexpr uint MAX_FILE_NAME_LEN = 128; //文件名最大长度
constexpr uint MAX_FILE_NUM = 256; //单个目录下最大文件数量
constexpr uint MAX_FILE_SIZE = 1024; //单个文件最大大小

constexpr uint MAX_USER_NAME_LEN = 16; //用户昵称最大长度
constexpr uint MAX_USER_PASSWORD_LEN = 16; //用户密码最大长度

constexpr uint MAX_COMMAND_LEN = 256; //命令/路径 最大长度
constexpr uint MAX_INPUT_LEN = MAX_COMMAND_LEN * 3 + 2; //单行输入最大长度

extern const char* VIRTUAL_DISK; //虚拟硬盘名称

//超级块
struct SuperBlock {
	uint blockSize; //盘块大小
	uint blockNum; //盘块数量
	uint iNodeNum; //i节点数量
	uint freeBlockNum; //空闲盘块数量
	uint freeINodeNum; //空闲i节点数量
	uint firstDataAddr; //第一个数据块的物理地址

	SuperBlock(uint _blockSize = 0, uint _blockNum = 0, uint _iNodeNum = 0,
		uint _freeBlockNum = 0, uint _freeINodeNum = 0, uint _firstDataAddr = 0);
};

//i节点
struct INode {
	FileType fileType; //文件类型
	uint fileUserId; //创建文件的用户id
	uint fileSize; //文件大小
	uint fileBlockNum; //文件占用的盘块数量
	bool ifModified; //文件是否被修改过
	uint fileAddr; //文件存放的首个物理地址
	FilePermission filePermission; //文件读写权限

	INode(FileType _fileType = FileType::FILE, uint _fileUserId = 0,
		uint _fileSize = 0, uint _fileBlockNum = 0, bool _ifModified = false,
		uint _fileAddr = 0, FilePermission _filePermission = FilePermission::READ_ONLY);
};

//文件接口
struct FileInterface {
	char fileName[MAX_FILE_NAME_LEN]; //文件名
	uint iNodeId; //对应的i节点索引

	FileInterface(const char _fileName[] = "", uint _iNodeId = 0);
};

//目录文件
struct Directory {
	char dirName[MAX_FILE_NAME_LEN]; //目录名
	uint iNodeId; //对应的i节点索引
	uint fileNum; //此目录下的子文件数量
	FileInterface fileInterface[MAX_FILE_NUM]; //子文件 下标0指向父目录 1指向当前目录

	Directory();
};

//用户
struct User {
	char userName[MAX_USER_NAME_LEN]; //用户昵称
	char userPassword[MAX_USER_PASSWORD_LEN]; //密码
	uint userId; //用户ID
	UserType userType; //用户类型

	User(const char _userName[] = "", const char _userPassword[] = "",
		uint _userId = 0, UserType _userType = UserType::GENERAL);
};


//声明文件系统的基础变量
extern SuperBlock superBlock;
extern INode iNodes[INODE_NUM];
extern Directory currentDir; //当前目录
extern char currentPath[1000]; //当前路径
extern User currentUser; //当前用户
extern std::vector<User> users;
extern bool blockBitmap[BLOCK_NUM]; //盘块位图
extern bool iNodeBitmap[INODE_NUM]; //i节点位图
extern std::fstream fsVirtualDisk; //读写虚拟硬盘
extern bool ifExit; //是否退出


constexpr uint SUPER_BLOCK_SIZE = sizeof(SuperBlock); //超级块占用空间大小
constexpr uint INODE_SIZE = sizeof(INode); //单个i节点占用空间大小
constexpr uint DIRECTORY_SIZE = sizeof(Directory); //单个目录占用空间大小
constexpr uint USER_SIZE = sizeof(User);
constexpr uint DIRECTORY_BLOCK_NUM = DIRECTORY_SIZE / BLOCK_SIZE + 1; //单个目录占用盘块数
constexpr uint FIRST_DATA_ADDR = SUPER_BLOCK_SIZE + sizeof(blockBitmap) + sizeof(iNodeBitmap) + sizeof(iNodes); //第一块数据块的物理地址
constexpr uint SYSTEM_SIZE = FIRST_DATA_ADDR + BLOCK_NUM * BLOCK_SIZE; //整个文件系统大小
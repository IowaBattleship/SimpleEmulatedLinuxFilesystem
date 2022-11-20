#include "diskmanager.h"

std::pair<uint, int> allocateBlock(const uint blockNum)
{
	uint addr = 0; int blockFirstPos = -1;

	//剩余空闲块数量>=需求时分配
	if (superBlock.freeBlockNum >= blockNum)
	{
		//检查是否有满足要求的连续空间块
		int cnt = 0;
		for (int i = 0; i < BLOCK_NUM; i++)
		{
			if (blockBitmap[i] == false)
			{
				++cnt;
				//满足要求
				if (cnt == blockNum)
				{
					blockFirstPos = i - cnt + 1;
					break;
				}
			}
			else
				cnt = 0;
		}
		if (~blockFirstPos)
		{
			//分配盘块
			for (int i = blockFirstPos; i < blockFirstPos + blockNum; i++)
				blockBitmap[i] = true;
			//修改超级块信息
			superBlock.freeBlockNum -= blockNum;
			addr = superBlock.firstDataAddr + blockFirstPos * BLOCK_SIZE;

			//写入盘块位图、超级块信息
			writeSpecifiedInfo(InfoType::SUPER_BLOCK);
			writeSpecifiedInfo(blockFirstPos, blockNum, InfoType::BLOCK_BITMAP);
		}
	}
	return std::make_pair(addr, blockFirstPos);
}

int allocateINode()
{
	int iNodePos = -1;

	//剩余有i节点时分配
	if (superBlock.freeINodeNum > 0)
	{
		for (int i = 0; i < INODE_NUM; i++)
			if (iNodeBitmap[i] == false)
			{
				iNodePos = i;
				break;
			}
		if (~iNodePos)
		{
			iNodeBitmap[iNodePos] = true;
			//修改超级块
			superBlock.freeINodeNum--;

			//写入i节点位图、超级块信息
			writeSpecifiedInfo(InfoType::SUPER_BLOCK);
			writeSpecifiedInfo(iNodePos, 1, InfoType::INODE_BITMAP);
		}
	}
	return iNodePos;
}

void freeBlock(const int blockFirstPos, const uint blockNum)
{
	for (int i = blockFirstPos; i < blockFirstPos + blockNum; i++)
		blockBitmap[i] = false;
	superBlock.freeBlockNum += blockNum;
	
	//写入超级块、盘块位图信息
	writeSpecifiedInfo(InfoType::SUPER_BLOCK);
	writeSpecifiedInfo(blockFirstPos, blockNum, InfoType::BLOCK_BITMAP);
}

void freeINode(const int iNodePos)
{
	iNodeBitmap[iNodePos] = false;
	superBlock.freeINodeNum++;

	//写入i节点位图、超级块信息
	writeSpecifiedInfo(InfoType::SUPER_BLOCK);
	writeSpecifiedInfo(iNodePos, 1, InfoType::INODE_BITMAP);
}
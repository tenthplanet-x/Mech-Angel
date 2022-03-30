#ifndef _LC_CLIENT_ITEM_FACTORY_H
#define _LC_CLIENT_ITEM_FACTORY_H
//负责创建和获取所有物品
#include "LC_ItemFactoryBase.h"

class LC_ClientItemFactory : public GameLogic::LC_ItemFactoryBase
{
public:
	LC_ClientItemFactory();
	virtual ~LC_ClientItemFactory();

	virtual bool Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	virtual bool UnInit();

	int GetActiveSkillItemID(int iSkillID);

protected:
	//缓存激活技能的道具IDs
	mem::vector<int> m_kActiveSkillItemIDs;
};
#endif
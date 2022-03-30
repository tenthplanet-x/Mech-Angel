#ifndef _LC_CLIENT_ITEM_FACTORY_H
#define _LC_CLIENT_ITEM_FACTORY_H
//���𴴽��ͻ�ȡ������Ʒ
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
	//���漤��ܵĵ���IDs
	mem::vector<int> m_kActiveSkillItemIDs;
};
#endif
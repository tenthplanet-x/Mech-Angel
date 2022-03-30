//负责服务器物品创建
#pragma once
#include "LC_PackStruct.h"
#include "LC_PackEntryFactory.h"
#include <vector>

class LC_ServerPackEntryFactory : public GameLogic::LC_PackEntryFactory
{
public:
	LC_ServerPackEntryFactory();
	virtual ~LC_ServerPackEntryFactory(){};

	virtual bool	CreateItemEntry(int32_t lItemID, int32_t lItemCount, int32_t iCreateType, GameLogic::LC_BackPackEntryList& rEntryList, int32_t bindMode=-1, int32_t param1=0, int32_t param2=0, bool bAutoSplit=false);

	virtual bool	CreateItemEntry(const Utility::UT_SIMDataInfo& itemInfo, int32_t iCreateType, GameLogic::LC_BackPackEntryList& rEntryList, bool bAutoSplit=false);

	void			LoadItemLimit(char* pMsg, uint16_t wSize);

	void			UpdateItemLimit(char* pMsg, uint16_t wSize);

private:
	typedef mem::vector<Protocol::PS_ItemControl> ItemControlList;
	ItemControlList		m_akLimitInfo;
};

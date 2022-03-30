#pragma once
#include "LC_CommonShopAsset.h"
#include "char_data.pb.h"
using namespace GameLogic;

class LC_ServerShopManager: public LC_CommonShopManager
{
public:
	LC_ServerShopManager();
	virtual ~LC_ServerShopManager();

	virtual bool Init(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	virtual bool Unit();

	virtual void CheckReset(uint32_t timestamp);
	virtual ResultType CheckShop(LC_PlayerBase* pkPlayer, const LC_ShopIDMap& kShopIDMap);
	virtual	void RegShopping(const LC_ShopIDMap& kShopIDMap);
	virtual void Tick(uint32_t ntimeStramp);

	virtual LC_CommonShopStat* GetShopStat(shop_id_type iShopID, int32_t iStatType);

	virtual bool loadGlobalLimitFromRedis();
	virtual bool saveGlobalLimitFromRedis();
	virtual uint32_t ShopAssetDirtyTime() { return m_lastDirtyTime; }
	virtual LC_CommonShopAsset* GetCommonShopAsset() { return &m_kGlobalShopAsset;}
private:
	bool	IsTimeViable(uint64_t nNowTime, uint64_t nStartTime, uint64_t nMergeTime, LC_ShopDataEntry* pEntry);
private:
	LC_CommonShopAsset	m_kGlobalShopAsset;
	uint32_t			m_lastTimeStramp;
	uint32_t			m_lastDirtyTime;
	mem::list<int32_t>	m_noticeIdList;
};
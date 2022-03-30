#pragma once

#include "IdDefine.h"
#include "LC_ServerPlayer.h"
#include "SystemError.h"
#include "CF_MysticalShopRefresh.h"

#include <list>
#include <map>
//////////////////////////////////////////////////////////////////////////////////////////
struct shopChooseNode
{
	uint32_t	m_nWeight;
	uint32_t	m_nID;
};
struct shopChooseList
{
	uint32_t	m_nTotalWeight;
	mem::list<shopChooseNode>	m_ChooseList;
};
typedef mem::map<uint32_t, shopChooseList> ShopChooseNodeMap;
//////////////////////////////////////////////////////////////////////////////////////////
class ServerMysticalShop : public Memory::MM_BaseObject, public Utility::UT_Singleton<ServerMysticalShop>
{
public:
	ServerMysticalShop();
	~ServerMysticalShop();

	void	Clear();
	void	InitConfig(CSVFile::CSVDataLoader* pkLoader=NULL, const CSVFile::CSVDataSourceList* pkCSVs=NULL);
	ResultType	RefreshItems(LC_ServerPlayer* pPlayer, bool bCheck = true);
	ResultType	BuyItem(LC_ServerPlayer* pPlayer, uint32_t itemIndex, uint32_t nRefreshTime);
	ResultType	ResetBuyCount(LC_ServerPlayer* pPlayer, uint32_t itemIndex, uint32_t nRefreshTime);
	ResultType	ReqRefresh(LC_ServerPlayer* pPlayer);
private:
	ShopChooseNodeMap	m_ChooseMap;
	time_t	m_tRefreshActive;
	const CSVFile::CF_MysticalShopRefresh::DataEntry*	m_pMysticalShop;
};

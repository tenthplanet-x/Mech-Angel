#pragma once

#include "LC_AuctionStruct.h"
#include "MG_CS_Auction.h"

#define  AUCTION_CATEGORY_OTHER			20
namespace GameLogic
{
	class LC_ClientAuctionManager : public Utility::UT_Singleton<LC_ClientAuctionManager>, public Memory::MM_BaseObject
	{
	public:
		LC_ClientAuctionManager();
		virtual ~LC_ClientAuctionManager();
		DECL_RTTI_NAME( LC_ClientAuctionManager );

		enum AuctionDirtyFlagType
		{
			AUCTION_DIRTY_FLAG_SEARCH = 0,
			AUCTION_DIRTY_FLAG_BUY,
			AUCTION_DIRTY_FLAG_SELL,
			AUCTION_DIRTY_FLAG_MARKET,

			MAX_DIRTY_FLAG_TYPE 
		};

		enum SearchSortType
		{
			SEARCH_SORTTYPE_NORMAL,
			SEARCH_SORTTYPE_PRICE_UP,
			SEARCH_SORTTYPE_PRICE_DOWN,
			SEARCH_SORTTYPE_TOTALPRICE_UP,
			SEARCH_SORTTYPE_TOTALPRICE_DOWN
		};

		enum HistoryType
		{
			HISTORY_TYPE_BUY,
			HISTORY_TYPE_SELL
		};
		
		struct HistoryData
		{
			int64_t iTimeVal;
			unique_id_type iAuctionID;
			StringType sContent;
		};

		typedef mem::vector<HistoryData> AuctionHistoryVec;

		void		Reset();

		//! 请求开始拍卖
		void	RequestAuctionSell(uint32_t iFixPrice, int iCashType, int iPackType, int iIndex, int iCount);

		//! 请求取消拍卖
		ResultType	RequestCancelAuction(int iIndex);

		void LC_ClientAuctionManager::RequestAuctionBuyByAuctionID(unique_id_type iAuction, int iCount, int iCashType, int iPrice);

		//生成查找结果
		void		Search(const StringType& strSearchName, int iItemCategory, int iStartLevel, int iGrade, int eSortType);
	
		//! 请求查找: 竞标信息、拍卖信息
		void		RequestAuctionList(int iType);
		
		void		ReqGetReward(int iIndex);
		void		ReqOnekeyGetReward();

		void		UpdateAuctionBuyDataList(const Protocol::MG_RLT_AuctionList& kAuctionList);
		void 		UpdateAuctionBuyData(const Protocol::PS_Auction& kAuction);

		void		UpdateAuctionSellDataList(const Protocol::MG_RLT_AuctionList& kAuctionList);
		void 		UpdateAuctionSellData(const Protocol::PS_Auction& kAuction);

		void 		DeleteAuctionSellData(unique_id_type uiAuctionID);
		void		DeleteAuctionBuyData(unique_id_type uiAuctionID);
		bool		DeleteAuctionMarketData(unique_id_type uiAuctionID, int iCount=-1);

		void 		ClearAuctionBuyList();
		void 		ClearAuctionSellList();

		void		UpdateAuctionSearchList(const Protocol::MG_RLT_AuctionList& kAuctionList);
		void 		ClearAuctionSearchList();

		bool		UpdateAuctionMarketList(const Protocol::MG_RLT_AuctionList& kAuctionList);
		void 		UpdateAuctionMarket(const Protocol::PS_Auction& kAuction);

		void		UpdateRewardNum(int iNum) { m_iRewardNum = iNum; }
		bool		IsRewardPrompt() { return m_iRewardNum > 0; }

	public:
		LC_ClientAddItemInfo*		GetAddItemInfo();
		
		int							GetAuctionBuyCount();
		LC_ClientAuctionBuyData*	GetAuctionBuyData(int iIndex);

		bool						IsSellItemInBuyList(unique_id_type lAuctionID);

		int							GetAuctionSellCount();
		LC_ClientAuctionSellData*	GetAuctionSellData(int iIndex);

		int							GetAuctionSearchCount();
		LC_ClientAuctionSellData*	GetAuctionSearchData(int iIndex);

		LC_ClientAuctionSellData*	GetAuctionMarketDataByAuctionID(unique_id_type lAuctionID);

		bool						IsInit() { return m_bInit; }

	public:
		void		SetDirtyFlag(int iType, bool bFlag);
		bool		GetDirtyFlag(int iType);
		void		ClearDirtyFlag();

		void		AddDirtyFlagRevisionID(int iType);
		long		GetDirtyFlagRevisionID(int iType);
		void		SetDirtyFlagRevisionID(int iType, long lRevisionID);
		void		ClearDirtyFlagRevisionID();
	public:
		// 获取拍卖分类数目
		int GetCategoryCount();
		int GetCategoryID(int iIndex);
		StringType GetCategoryName(int iIndex);
		int GetCategoryParentID(int iIndex);
		StringType GetCategoryPic(int iIndex);

	public:
		void LoadHistory(unique_id_type uiID);
		void SaveHistory();
		void AddHistory(LC_ClientAuctionSellData* data, int iCount);
		void AddHistory(LC_ClientAuctionBuyData* data);
		bool IsHistoryExist(HistoryType eType, unique_id_type iAuctionID);
		int GetHistoryCount(int eType);
		const StringType& GetHistoryStr(int eType, int iIndex);
	private:
		void _onBuyDataUpdated();
		void _initHistoryData(const StringType& str);
		void _setRecord(AuctionHistoryVec& kVec, const StringType& sContent);
		StringType _getSaveString();
	
	private:
		static bool _sortNormal(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2);
		static bool _sortByPriceDown(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2);
		static bool _sortByPriceUp(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2);
		static bool _sortByTotalPriceDown(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2);
		static bool _sortByTotalPriceUp(const LC_ClientAuctionSellData* v1, const LC_ClientAuctionSellData* v2);

		static bool _sortRecordByTime(const HistoryData& v1, const HistoryData& v2);

		LC_ClientAddItemInfo	m_kAddItemInfo;
		AUCTION_BUY_LIST		m_kBuyList;
		AUCTION_SELL_LIST		m_kSellList;
		AUCTION_SELL_PTR_LIST	m_kSearchList;
		AUCTION_SELL_MAP		m_kMarketMap;

		bool					m_bDirtyFlag[MAX_DIRTY_FLAG_TYPE];
		long					m_lDirtyFlagRevisionID[MAX_DIRTY_FLAG_TYPE];

		AuctionHistoryVec m_vHistoryBuy;
		AuctionHistoryVec m_vHistorySell;

		unique_id_type			m_uiID;
		int						m_iRewardNum;
		bool					m_bInit;
	};
};
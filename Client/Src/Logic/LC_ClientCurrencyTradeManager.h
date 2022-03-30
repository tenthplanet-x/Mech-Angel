#pragma once
#include "LC_CurrencyTradeData.h"
#include "MG_CS_CurrencyTradeProtocol.h"
namespace GameLogic
{
	#define MAX_TRADE_RECORD_PAGES_NUM	5		// 最多请求5个交易记录页面，每个页面MAX_EXCHANGE_HISTORY_PER_MSG条记录

	class LC_ClientCurrencyTradeManager : public Utility::UT_Singleton<LC_ClientCurrencyTradeManager>, public Memory::MM_BaseObject
	{
	public:
					LC_ClientCurrencyTradeManager();
		virtual		~LC_ClientCurrencyTradeManager();
					DECL_RTTI_NAME( LC_ClientCurrencyTradeManager );

		enum DirtyType
		{ 
			DIRTY_FLAG_INVALID = -1,

			DIRTY_FLAG_QUICK_TRADE			= 0,
			DIRTY_FLAG_COMMISSION			= 1,
			DIRTY_FLAG_TRADE_RECORD			= 2,
			DIRTY_FLAG_ACCOUNT_EXCHANGE		= 3,
			DIRTY_FLAG_COMMISSION_REVOKE	= 4,

			MAX_DIRTY_TYPE_NUM				= 5
		};
		
		void		Reset();
		
		void		RequestQuickBuy(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper, int32_t iRequestCount);
		void		RequestQuickSell(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper, int32_t iRequestCount);
		void		RequestCommissionBuy(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper, int32_t iRequestCount);
		void		RequestCommissionSell(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper, int32_t iRequestCount);
		//账户转账
		void		RequestYuanBaoDeposit(int32_t iYuanBaoCount);
		void		RequestYuanBaoDraw(int32_t iYuanBaoCount);
		void		RequestMoneyDeposit(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper);
		void		RequestMoneyDraw(MoneyType iRequestPriceGold, MoneyType iRequestPriceSilver, MoneyType iRequestPriceCopper);

		void		SetAccountMoney(MoneyType iMoney);
		StringType	GetAccountMoney();
		void		SetAccountYuanBao(MoneyType iYuanBao);
		StringType	GetAccountYuanBao();
		MoneyType	GetAccountYuanBaoNum() const;

		void		RequestQuickTradeList();
		void		RequestTradeRecordList(uint32_t uiPage);		// 请求更新需要的列表
		void		RequestTradeRecordPages(uint32_t uiPage);		// 请求更新记录页面（大于100条），不重复加载已经请求过的页面。
		void		RequestCommissionItemList();

		int			GetCurrencyQuickBuyListSize();
		int			GetCurrencyQuickSellListSize();
		int			GetCurrencyCommissionListSize();
		int			GetCurrencyTradeRecordListSize();
		int			GetTradeRecordListPage();
		bool		GetRevokeCommissionFlag();
		void		SetRevokeCommissionFlag(bool bFlag);

		LC_CurrencyTradeQuickTradeItem* GetCurrencyQuickTradeItem(int iTradeType, int index); 
		LC_CurrencyTradeCommssionItem*	GetCurrencyCommissionItem(int index); 
		LC_CurrencyTradeRecordItem*		GetCurrencyTradeRecordItem(int index); 

		

		void		RequestRevokeCommission(uint32_t uiCommissionID, int index);

		void		UpdateQuickTradeList(Protocol::MG_RLT_CurrencyQuickTradeList& kMsg);
		void		UpdateTradeRecordList(Protocol::MG_RLT_CurrencyTradeRecordList& kMsg);
		void		UpdateCommissionItemList(Protocol::MG_RLT_CurrencyCommissionItemList& kMsg);

		void		ClearQuickTradeList();
		void		ClearTradeRecordList();
		void		ClearCommissionItemList();
		void		ResetTradeRecordPageLoadFlag();

		StringType	GetCurrencyTipString(int iGold, int iSilver, int iCopper);

	public:
		//设置更新标志
		void		SetDirtyFlag(bool bFlag, DirtyType eDirtyType);
		bool		GetDirtyFlag(int iDirtyType);
		void		ClearDirtyFlag();

		void		AddDirtyFlagRevisionID(DirtyType eDirtyType);
		long		GetDirtyFlagRevisionID(int iDirtyType);
		void		SetDirtyFlagRevisionID(uint32_t lRevisionID, DirtyType eDirtyType);
		void		ClearDirtyFlagRevisionID();

	private:
		MoneyType	m_llAccountMoney;
		MoneyType	m_llAccountYuanBao;
		uint32_t	m_uiTradeRecordPage;

		bool		m_bRevokeFlag;
		bool		m_bRecordPageLoadFlag[MAX_TRADE_RECORD_PAGES_NUM];
		bool		m_bDirtyFlag[MAX_DIRTY_TYPE_NUM];				
		long		m_lDirtyFlagRevisionID[MAX_DIRTY_TYPE_NUM];

		CurrencyTradeQuickTradeArray 		m_kCurrencyTradeQuickBuyArray;
		CurrencyTradeQuickTradeArray 		m_kCurrencyTradeQuickSellArray;
		CurrencyTradeCommssionItemArray 	m_kCurrencyTradeCommssionItemArray;
		CurrencyTradeRecordItemArray		m_kCurrencyTradeRecordItemArray;		
	};
}
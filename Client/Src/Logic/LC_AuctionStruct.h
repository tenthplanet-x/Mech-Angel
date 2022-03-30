#pragma once

#include "MG_ProtocolStruct.h"

namespace GameLogic
{
	// 竞标数据
	class LC_ClientAuctionBuyData : public Memory::MM_BaseObject
	{
	public:
		LC_ClientAuctionBuyData()
		: m_uiAuctionID(0)
		, m_iLeftTime(0)
		, m_bStatus(false)
		, m_iFixedPrice(0)
		, m_iCashType(0)
		{
			
		}
		~LC_ClientAuctionBuyData() {}
		DECL_RTTI_NAME( LC_ClientAuctionBuyData );

		MEMBER_UNINT64(AuctionID);
		MEMBER_INT(LeftTime);
		MEMBER_BOOL(Status);

		MoneyType GetFixedPrice()						{ return m_iFixedPrice; }
		int	GetCashType()								{ return m_iCashType; }

		LC_BackPackEntry* GetEntry() { return &m_kEntry; }

	public:
		LC_BackPackEntry	m_kEntry;

		MoneyType			m_iFixedPrice;
		int					m_iCashType;
		bool				m_bIsCurrentBuyer;
	};

	typedef mem::vector<LC_ClientAuctionBuyData> AUCTION_BUY_LIST;
	
	// 拍卖数据
	class LC_ClientAuctionSellData : public Memory::MM_BaseObject
	{
	public:
		LC_ClientAuctionSellData()
		: m_uiAuctionID(0)
		, m_iLeftTime(0)
		, m_iFixedPrice(0)
		, m_iTotalPrice(0)
		, m_iCashType(0)
		, m_iItemGrade(0)
		, m_iItemLevel(0)
		, m_bIsItemOwner(false)
		{
		}
		~LC_ClientAuctionSellData() {}
		DECL_RTTI_NAME( LC_ClientAuctionSellData );

		MEMBER_UNINT64(AuctionID);
		MEMBER_INT(LeftTime);
		MEMBER_STRING(Name);

		MoneyType GetFixedPrice()						{ return m_iFixedPrice; }
		int	GetCashType()								{ return m_iCashType; }

		void SetIsItemOwner(bool bIsItemOwner)			{ m_bIsItemOwner = bIsItemOwner; }
		bool GetIsItemOwner()							{ return m_bIsItemOwner; }

		LC_BackPackEntry* GetEntry() { return &m_kEntry; }

	public:
		LC_BackPackEntry	m_kEntry;

		MoneyType			m_iFixedPrice;
		MoneyType			m_iTotalPrice;
		int					m_iCashType;
		int					m_iItemGrade;
		int					m_iItemLevel;
		bool				m_bIsItemOwner;
	};
	typedef mem::vector<LC_ClientAuctionSellData> AUCTION_SELL_LIST;
	typedef mem::vector<LC_ClientAuctionSellData*> AUCTION_SELL_PTR_LIST;
	typedef mem::map<unique_id_type, LC_ClientAuctionSellData> AUCTION_SELL_MAP;

	//
	class LC_ClientAddItemInfo : public Memory::MM_BaseObject
	{
	public:
		LC_ClientAddItemInfo()
		: m_bDirtyFlag(false)
		, m_lDirtyFlagRevisionID(0)
		{
			Reset();
		}
		~LC_ClientAddItemInfo() {}
		DECL_RTTI_NAME( LC_ClientAddItemInfo );

		void 		Reset();
		
		bool 		AddItemAffix(int iPackType, int iIndex);
		bool 		DeleletItemAffix();

	public:
		bool 		IsValid();
		int  		GetPackType();
		int	 		GetIndex();

	public:
		void		SetDirtyFlag(bool bFlag);
		bool		GetDirtyFlag();
		void		ClearDirtyFlag();

		void		AddDirtyFlagRevisionID();
		long		GetDirtyFlagRevisionID();
		void		SetDirtyFlagRevisionID(long lRevisionID);
		void		ClearDirtyFlagRevisionID();

	public:
		Protocol::PS_ClientAuctionSellItemAffix	m_kItemAffix;

		bool					m_bDirtyFlag;
		long					m_lDirtyFlagRevisionID;		
	};
};
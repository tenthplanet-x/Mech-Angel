
//物品tooltip的基类

#ifndef _LC_ITEMTOOLTIP_BASE_H
#define _LC_ITEMTOOLTIP_BASE_H
#include "LM_LocaleManager.h"
#include "LC_TooltipBase.h"
#include "LC_ActorBase.h"
#include "CF_ItemUseLimitFunc.h"
#include "CF_ItemEquipLimitFunc.h"
#include "LC_PackStruct.h"
#include "LC_SubordinateAsset.h"

using namespace CSVFile;
using namespace Utility;

namespace GameLogic
{
	class LC_ItemTooltipBase : public LC_TooltipBase
	{
	public:
		LC_ItemTooltipBase(void);
		virtual ~LC_ItemTooltipBase(void);

		//Begin 构造tooltip所需要的参数接口
		void			  Init(CSVFile::CF_ItemList::DataEntry *pkItemEntry, LC_BackPackEntry* pkBackPackEntry, LC_NPCShopItemEntry* pkShopEntry, LC_PlayerBase* pkObserver, 
			CSVFile::CF_ItemExistenceFunc::DataEntry *pkItemExistenceFuncEntry = NULL, bool bShopOpen = false, bool bGuildWarehouse = false, int iStarry = 0);
		CSVFile::CF_ItemList::DataEntry* GetItemInfoEntry() { return m_pkItemInfoEntry; };
		CSVFile::CF_ItemExistenceFunc::DataEntry* GetItemExistenceFuncEntry() { return m_pkItemExistenceFuncEntry; }
		LC_BackPackEntry* GetBackPackEntry(){return m_pkBackPackEntry;};
		LC_NPCShopItemEntry*	  GetShopEntry(){return m_pkShopEntry;};
		//End

	protected:
		//构造tooltip的提示自身装备字段
		bool _addTooltipFieldSelfEquipHit(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品名字字段
		bool _addTooltipFieldName(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品子类型字段
		bool _addTooltipFieldSubCategory(TooltipFieldVector& kFieldVector);
		////构造tooltip的物品等级字段
		//bool _addTooltipFieldLevel(TooltipFieldVector& kFieldVector);
		//构造tooltip的绑定字段
		bool _addTooltipFieldBind(TooltipFieldVector& kFieldVector);
		//构造tooltip的可分解字段
		bool _addTooltipFieldDecomposition(TooltipFieldVector& kFieldVector);
		//构造tooltip的拥有唯一字段
		bool _addToolTipFieldOwnUnique(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品叠加数量字段
		bool _addTooltipFieldOverlapCount(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品来源字段
		bool _addTooltipFieldOrigin(TooltipFieldVector& kFieldVector);
		//构造拆分提示字段
		bool _addTooltipSplitInfo(TooltipFieldVector &kFieldVector);
		//构造出售提示字段
		bool _addTooltipSellTip(TooltipFieldVector &kFieldVector);
		//tooltip的物品使用限制相关字段：等级限制、性别限制、武器限制、职业限制
		//构造tooltip的物品等级限制字段
		template<typename LimitFunc>
		bool _addTooltipFieldLevelLimit(TooltipFieldVector& kFieldVector)
		{
			CF_ItemList::DataEntry* pkItemData = GetItemInfoEntry();
			if (NULL == pkItemData)
			{
				return false;
			}
			
			CF_ItemUseLimitFunc *pkItemLimit = CF_ItemUseLimitFunc::GetSingletonPtr();
			CF_ItemUseLimitFunc::DataEntry *pkItemLimitData = pkItemLimit->GetEntryPtr(pkItemData->_lUseLimitFuncID);
			if (NULL == pkItemLimitData)
			{
				GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemUseLimitFunc.csv key==%ld  is not found!", pkItemData->_lUseLimitFuncID );
				return false;
			}

			int lMinLevelLimit = 0;
			int lMaxLevelLimit = 0;
			int iSubType = -1;
			int iSubLevel = 0;
			int iVipLevel = 0;

			CF_ItemEquipLimitFunc* pkItemEquip = CF_ItemEquipLimitFunc::GetSingletonPtr();
			CF_ItemEquipLimitFunc::DataEntry* pkItemEquipData = pkItemEquip->GetEntryPtr(pkItemData->_lEquipLimitFuncID);
			if (NULL == pkItemEquipData)
			{
				GfxWriteLog( LOG_TAG_ERROR, LOG_SWITCH_ERROR, "ItemList.csv gets foreign data ERROR: In ItemEquipLimitFunc.csv key==%ld  is not found!", pkItemData->_lEquipLimitFuncID );
			    return false;
			}
			if (sizeof(LimitFunc) == sizeof(*pkItemLimitData))
			{
				if ( pkItemLimitData->_iID <= 0 )
				{
					return false;
				}
				lMinLevelLimit = pkItemLimitData->_lMinLevelLimit;
				lMaxLevelLimit = pkItemLimitData->_lMaxLevelLimit;
				iVipLevel = pkItemLimitData->_vipLevel;
			}
			else if (sizeof(LimitFunc) == sizeof(*pkItemEquipData))
			{
				if ( pkItemEquipData->_iID <= 0 )
				{
					return false;
				}

				lMinLevelLimit = pkItemEquipData->_lLevelLimit;
				if(pkItemEquipData->_kSubLimits.size() > 0)
				{
					Utility::UT_SIMDataInfo &kList = pkItemEquipData->_kSubLimits.front();
					iSubType = kList.ID();
					iSubLevel = kList.ParamA();
				}
			}
			else
			{
				return false;
			}
			if (lMinLevelLimit <= 0 && iSubType < 0)
			{
				return false;
			}

			unsigned short usFont = FONT_MIDDLE;
			unsigned short usFontSize = FONTSIZE_MIDDLE;
			int iFontColor = WHITE;

			StringStreamType kLevelStrStream;

			//当不提供观察者时，统一设置成白色显示
			LC_PlayerBase* pkObserver = GetObserver();
			if (NULL == pkObserver)
			{
				iFontColor = WHITE;
			}
			else if(iSubType >= 0 && iSubLevel > 0)
			{
				LC_SubUpgradeData *pkSubData = pkObserver->GetSubordinateAsset().GetSubData(iSubType);
				if(pkSubData && pkSubData->GetStage() < iSubLevel)
				{
					iFontColor = RED;
				}
			}
			else if(iVipLevel > 0)
			{
				if (pkObserver->GetVIPLevel() < iVipLevel)
				{
					iFontColor = RED;
				}
				kLevelStrStream << "<color va=0xFFE7E69A>" << GET_MSG(CLIENTTEXTDEFINE_H_018) << ": </color>" << iVipLevel;
				kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_LEVEL_LIMIT, GET_MSG(LC_ITEMTOOLTIPBASE_H_002), true, kLevelStrStream.str(), usFont, usFontSize, iFontColor, 0, 0, false));
				return true;
			}
			else
			{
				//判断最低等级要求
				//if (pkObserver->GetLevel() < lMinLevelLimit)
				if (pkObserver->GetCommanderLevel() < lMinLevelLimit)
				{
					iFontColor = RED;
				}
				//判断最高等级要求
				if (lMaxLevelLimit > 0)
				{
					//if (pkObserver->GetLevel() > lMaxLevelLimit)
					if (pkObserver->GetCommanderLevel() > lMaxLevelLimit)
					{
						iFontColor = RED;
					}
				}
			}

			LC_BackPackEntry* pkPackEntry = GetBackPackEntry();
			bool bNewLine = !IS_EQUIP_ID(pkItemData->_iID);
			if(pkPackEntry && lMinLevelLimit == 0 && iSubType < 0)//( Long2Char(LHEST_NO_LEVEL_LIMIT)) > 0)
			{
				iFontColor = GOLD;
				kLevelStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_H_001);
				kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_LEVEL_LIMIT, GET_MSG(LC_ITEMTOOLTIPBASE_H_002), true, kLevelStrStream.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine));

			}
			else
			{
				if(iSubLevel > 0)
				{
					if(iSubType < LC_SUB_TP_END)
						kLevelStrStream << "<color va=0xFFE7E69A>" << SUB_TP_NAME(iSubType) << GET_MSG(CLIENTTEXTDEFINE_H_013) << ": </color>" << iSubLevel;
				}
				else
				{
					if (lMinLevelLimit > 0)
					{
						/*if (lMinLevelLimit > PEAK_LEVEL_BEGIN)
						{
							kLevelStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_H_003) << GET_MSG(LC_ITEMTOOLTIPBASE_H_004)<<(lMinLevelLimit-PEAK_LEVEL_BEGIN);
						}
						else
						{*/
						kLevelStrStream << GET_MSG(LC_ITEMTOOLTIPBASE_H_003) << lMinLevelLimit;
						//}						
					}
					if (lMaxLevelLimit > 0)
					{
						kLevelStrStream << "- " << lMaxLevelLimit;
					}
				}
				kFieldVector.push_back(LC_TooltipField(TOOLTIP_FIELD_ITEM_LEVEL_LIMIT, GET_MSG(LC_ITEMTOOLTIPBASE_H_002), true, kLevelStrStream.str(), usFont, usFontSize, iFontColor, 0, 0, bNewLine));
			}

			return true;
		}
		//构造tooltip的物品性别限制字段
		bool _addTooltipFieldSexLimit(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品门派限制字段
		bool addTooltipFieldSchoolLimit( TooltipFieldVector& kFieldVector);
		//构造tooltip的物品职业限制字段
		bool _addTooltipFieldCareerLimit(TooltipFieldVector& kFieldVector);
		//tooltip的物品使用效果相关字段：瞬发效果、触发技能效果
		//构造tooltip的物品使用瞬发效果字段
		//bool _addTooltipFieldUseResultProperty(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品触发技能效果字段
		//bool _addTooltipFieldUseResultTriggerSkill(TooltipFieldVector& kFieldVector);
		//构造tooltip的物品使用次数字段
		bool _addTooltipFieldUseCount(TooltipFieldVector& kFieldVector);
		//构造tooltip的价格字段
		bool _addTooltipFieldPrice(TooltipFieldVector& kFieldVector);
		//构造tooltip的公会币字段
		bool _addTooltipFieldGuildCash(TooltipFieldVector& kFieldVector);
		//构造tooltip的描述字段
		bool _addTooltipFieldDescript(TooltipFieldVector& kFieldVector);
		//构造tooltip的详细描述字段
		bool _addTooltipDetailDescript(TooltipFieldVector &kFieldVector);
		//构造tooltip的氛围描述字段
		bool _addTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector);
		//构造tooltip的时间字段
		bool _addTooltipFieldTimeLimitDescript(TooltipFieldVector& kFieldVector);
		//构造tooltip的批量使用字段
		bool _addTooltipFieldBatchUseDescript(TooltipFieldVector& kFieldVector);
		//构造tooltip的发送道具信息字段
		bool _addTooltipFieldAddItemLinkInfo(TooltipFieldVector& kFieldVector);

		/*
		//构造tooltip的数据格式说明字段
		bool _addTooltipFieldPackDataFormatDescript(TooltipFieldVector& kFieldVector);
		*/

		bool			  m_bShopOpen;
		bool			  m_bGuildWarehouse;

		//传入的强化等级，用于还未获取的装备，但是又要显示强化信息
		int					m_iStarry;

	private:
		CSVFile::CF_ItemList::DataEntry *m_pkItemInfoEntry;
		LC_BackPackEntry* m_pkBackPackEntry;
		LC_NPCShopItemEntry*	  m_pkShopEntry;
		CSVFile::CF_ItemExistenceFunc::DataEntry *m_pkItemExistenceFuncEntry;
	};
}

#endif


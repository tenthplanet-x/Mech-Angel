
//��Ʒtooltip�Ļ���

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

		//Begin ����tooltip����Ҫ�Ĳ����ӿ�
		void			  Init(CSVFile::CF_ItemList::DataEntry *pkItemEntry, LC_BackPackEntry* pkBackPackEntry, LC_NPCShopItemEntry* pkShopEntry, LC_PlayerBase* pkObserver, 
			CSVFile::CF_ItemExistenceFunc::DataEntry *pkItemExistenceFuncEntry = NULL, bool bShopOpen = false, bool bGuildWarehouse = false, int iStarry = 0);
		CSVFile::CF_ItemList::DataEntry* GetItemInfoEntry() { return m_pkItemInfoEntry; };
		CSVFile::CF_ItemExistenceFunc::DataEntry* GetItemExistenceFuncEntry() { return m_pkItemExistenceFuncEntry; }
		LC_BackPackEntry* GetBackPackEntry(){return m_pkBackPackEntry;};
		LC_NPCShopItemEntry*	  GetShopEntry(){return m_pkShopEntry;};
		//End

	protected:
		//����tooltip����ʾ����װ���ֶ�
		bool _addTooltipFieldSelfEquipHit(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ�����ֶ�
		bool _addTooltipFieldName(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ�������ֶ�
		bool _addTooltipFieldSubCategory(TooltipFieldVector& kFieldVector);
		////����tooltip����Ʒ�ȼ��ֶ�
		//bool _addTooltipFieldLevel(TooltipFieldVector& kFieldVector);
		//����tooltip�İ��ֶ�
		bool _addTooltipFieldBind(TooltipFieldVector& kFieldVector);
		//����tooltip�Ŀɷֽ��ֶ�
		bool _addTooltipFieldDecomposition(TooltipFieldVector& kFieldVector);
		//����tooltip��ӵ��Ψһ�ֶ�
		bool _addToolTipFieldOwnUnique(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ���������ֶ�
		bool _addTooltipFieldOverlapCount(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ��Դ�ֶ�
		bool _addTooltipFieldOrigin(TooltipFieldVector& kFieldVector);
		//��������ʾ�ֶ�
		bool _addTooltipSplitInfo(TooltipFieldVector &kFieldVector);
		//���������ʾ�ֶ�
		bool _addTooltipSellTip(TooltipFieldVector &kFieldVector);
		//tooltip����Ʒʹ����������ֶΣ��ȼ����ơ��Ա����ơ��������ơ�ְҵ����
		//����tooltip����Ʒ�ȼ������ֶ�
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

			//�����ṩ�۲���ʱ��ͳһ���óɰ�ɫ��ʾ
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
				//�ж���͵ȼ�Ҫ��
				//if (pkObserver->GetLevel() < lMinLevelLimit)
				if (pkObserver->GetCommanderLevel() < lMinLevelLimit)
				{
					iFontColor = RED;
				}
				//�ж���ߵȼ�Ҫ��
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
		//����tooltip����Ʒ�Ա������ֶ�
		bool _addTooltipFieldSexLimit(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ���������ֶ�
		bool addTooltipFieldSchoolLimit( TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒְҵ�����ֶ�
		bool _addTooltipFieldCareerLimit(TooltipFieldVector& kFieldVector);
		//tooltip����Ʒʹ��Ч������ֶΣ�˲��Ч������������Ч��
		//����tooltip����Ʒʹ��˲��Ч���ֶ�
		//bool _addTooltipFieldUseResultProperty(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒ��������Ч���ֶ�
		//bool _addTooltipFieldUseResultTriggerSkill(TooltipFieldVector& kFieldVector);
		//����tooltip����Ʒʹ�ô����ֶ�
		bool _addTooltipFieldUseCount(TooltipFieldVector& kFieldVector);
		//����tooltip�ļ۸��ֶ�
		bool _addTooltipFieldPrice(TooltipFieldVector& kFieldVector);
		//����tooltip�Ĺ�����ֶ�
		bool _addTooltipFieldGuildCash(TooltipFieldVector& kFieldVector);
		//����tooltip�������ֶ�
		bool _addTooltipFieldDescript(TooltipFieldVector& kFieldVector);
		//����tooltip����ϸ�����ֶ�
		bool _addTooltipDetailDescript(TooltipFieldVector &kFieldVector);
		//����tooltip�ķ�Χ�����ֶ�
		bool _addTooltipFieldAtmosDescript(TooltipFieldVector& kFieldVector);
		//����tooltip��ʱ���ֶ�
		bool _addTooltipFieldTimeLimitDescript(TooltipFieldVector& kFieldVector);
		//����tooltip������ʹ���ֶ�
		bool _addTooltipFieldBatchUseDescript(TooltipFieldVector& kFieldVector);
		//����tooltip�ķ��͵�����Ϣ�ֶ�
		bool _addTooltipFieldAddItemLinkInfo(TooltipFieldVector& kFieldVector);

		/*
		//����tooltip�����ݸ�ʽ˵���ֶ�
		bool _addTooltipFieldPackDataFormatDescript(TooltipFieldVector& kFieldVector);
		*/

		bool			  m_bShopOpen;
		bool			  m_bGuildWarehouse;

		//�����ǿ���ȼ������ڻ�δ��ȡ��װ����������Ҫ��ʾǿ����Ϣ
		int					m_iStarry;

	private:
		CSVFile::CF_ItemList::DataEntry *m_pkItemInfoEntry;
		LC_BackPackEntry* m_pkBackPackEntry;
		LC_NPCShopItemEntry*	  m_pkShopEntry;
		CSVFile::CF_ItemExistenceFunc::DataEntry *m_pkItemExistenceFuncEntry;
	};
}

#endif


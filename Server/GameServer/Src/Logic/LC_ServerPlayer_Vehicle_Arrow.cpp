#include "LC_ServerPlayer.h"

#include "CF_Subordinate.h"

#include "oss_define.h"

#include "ConfigManager.h"

#include "UT_ServerHelper.h"

#include "SK_Define.h"

#include "LC_ServerLogicManager.h"
#include "LC_ServerNPC.h"

#include "ACT_ActivityDataManager.h"
#include "systemdefine.h"
#include "UT_OperateLog.h"
// #define MAX_VEHICLE_LEVEL  20
// #define MAX_ARROW_LEVEL	10
// #define MAX_DEVA_EYE_LEVEL 10
// #define MAX_MUHUN_LEVEL 10

#define UPGRADE_STATE_ID 32200001
#define UPGRADE_STATE_TIME 5

using namespace CSVFile;
using namespace GameLogic;
using namespace Activity;
using namespace OPERATELOG;

//-----------------------------------------------------------------------------------------
void LC_ServerPlayer::OnSubUpgradeSucessfully(int32_t iSubType)
{
	LC_SubUpgradeData* pkSubData = GetSubordinateAsset().GetSubData(iSubType);
	if (NULL == pkSubData)
	{
		return;
	}

	int32_t iSubStage = pkSubData->GetStage();

	//升阶返还，返还的是上一阶配的道具
	LC_SubordinateDataEntry* pkDataEntry = SafeGetSubDataEntry(iSubType, iSubStage-1);
	if (NULL == pkDataEntry)
	{
		return;
	}
	
	//是否有领取进阶返还的资格（VIP特权卡）
	int flag = (iSubType == LC_SUB_TP_TALISMAN) ? true : LC_Helper::GetVipPrivilegeTimesByType(VIP_PC_UPGRADE_PROFIT_ITEM, this);
	if(flag)
	{
		//萌宠,名号,幻兽,背饰 升级--埋点日志
		vector<int> nParams;
		nParams.clear();
		nParams.push_back(pkSubData->GetAttrLevel());
		int nLogCode = 0;
		switch(iSubType)
		{
		case LC_SUB_TP_VEHICLE:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MOUNTS_OPERATE, OPERATE_LOGTYPE1);
			break;
		case LC_SUB_TP_DEVA_EYE:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, PETS_OPERATE, OPERATE_LOGTYPE1);
			break;
		case LC_SUB_TP_MUHUN:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BEISHI_OPERATE, OPERATE_LOGTYPE1);;
			break;
		case LC_SUB_TP_TALISMAN:
			nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MINGHAO_OPERATE, OPERATE_LOGTYPE1);;
			break;
		}
		//to reward profit
		Utility::UT_SIMDataList kProfits;
		pkDataEntry->_kProfitItems.Resolve(this, kProfits, NULL);
		UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kProfits, this, SYS_FUNC_TP_SUB_PROFIT, iSubType, pkDataEntry->_sMailTitle, pkDataEntry->_sMailDesc, true, false,false, nLogCode, nParams);
	}
	
	//fov
	m_dataChangeFlag.GetBroadCastFlag().SetSubordinate();

	//achievement
	AchievementEvent kEvent;
	kEvent.SetConditionInfo(iSubType);
	kEvent.SetNum(iSubStage);
	UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_SUB_STAGE, kEvent);

	GetOpenAbilityAsset().OnSubLevelUp(iSubType);

	static Activity::ACT_ScheduleDataManager* pkACTDataMgr = Activity::ACT_ScheduleDataManager::GetSingletonPtr();
	pkACTDataMgr->OnSubUpgrade(this, iSubType, iSubStage);

	//用于更新排行榜
	if (GetLoadDataFromDBFlag())
	{
		SetCharInfoDirty();
	}

	//广播
	int iBroadcastID = 0;
	CSVFile::CF_Subordinate::DataEntry* pkEt = LC_Helper::GetCimeliaInfoEntry(iSubType, iSubStage);
	if (pkEt)
	{
		iBroadcastID = pkEt->_lBroadcastID;
	}
	if (0 != iBroadcastID)
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		noticeFmort.SetMsgID(iBroadcastID);
		noticeFmort.AddString(GetOwnerCharName());//玩家名
		noticeFmort.AddSubordinateName(iSubType);//进阶线名
		noticeFmort.AddNumber(pkEt->_iStage);//等阶
		
		CF_CharType::DataEntry* pkEntry = SafeGetCSVFileDataEntryPtr<CF_CharType>(pkEt->_CharTypeID);
		if(pkEntry)
		{
			noticeFmort.AddString(pkEntry->_kCharName);//进阶宠物的名字
		}
		noticeFmort.AddNumber(iSubType);//进阶线typeId
		UT_ServerHelper::BrocastNoticeMsg(noticeFmort, GetCitizenship());
	}
}
void LC_ServerPlayer::OnSubDowngrade(int32_t iSubType)
{
	LC_SubUpgradeData* pkSubData = GetSubordinateAsset().GetSubData(iSubType);
	if (NULL == pkSubData)
		return;
	//fov
	m_dataChangeFlag.GetBroadCastFlag().SetSubordinate();

	//用于更新排行榜
	if (GetLoadDataFromDBFlag())
	{
		SetCharInfoDirty();
	}
}
//------------------------------------------------------------------------------------------
ResultType LC_ServerPlayer::Upgrad_Subordinate(int32_t iSubType, int32_t iPayIndex, std::vector<int32_t>& attrIDVec, std::vector<int32_t>& attrLevelVec)
{
	if(!IsInstance())
	{
		return RE_FAIL;
	}

	LC_ServerPlayer* pkOwnerPlayer = GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return RE_FAIL;
	}

	LC_SubordinateAsset& kSubAsset = GetSubordinateAsset();
	LC_SubUpgradeData* pkSub = kSubAsset.GetSubData(iSubType);

	int32_t iPreStage = pkSub ? pkSub->GetStage() : 0;
	int32_t iCurAttrLevel = pkSub ? pkSub->GetAttrLevel() : 0;
	int64_t gp = GetUnBindCash().GetMoney();

	LC_PackAsset* pkOwnerBasicAsset = pkOwnerPlayer->GetPackAsset();
	if(NULL == pkOwnerBasicAsset)
	{
		return RE_FAIL;
	}
	//萌宠,名号,幻兽,背饰 升级--埋点日志
	vector<int> nParams;
	nParams.clear();
	nParams.push_back(iCurAttrLevel);
	int nLogCode = 0;
	switch(iSubType)
	{
	case LC_SUB_TP_VEHICLE:
		nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MOUNTS_OPERATE, OPERATE_LOGTYPE1);
		break;
	case LC_SUB_TP_DEVA_EYE:
		nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, PETS_OPERATE, OPERATE_LOGTYPE1);
		break;
	case LC_SUB_TP_MUHUN:
		nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, BEISHI_OPERATE, OPERATE_LOGTYPE1);;
		break;
	case LC_SUB_TP_TALISMAN:
		nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, MINGHAO_OPERATE, OPERATE_LOGTYPE1);;
		break;
	}

	ResultType res = kSubAsset.Upgrade(this, iSubType, attrIDVec, attrLevelVec, iPayIndex, false, nLogCode,nParams);
	pkSub = kSubAsset.GetSubData(iSubType);
	if (pkSub)
	{
		LC_BackPackEntry * pkItem = NULL;
		PackOPRecordSet * record_set = GetPackAsset()->GetRecordSet();
		if(!record_set->m_Delete_Items.empty())
		{
			pkItem = &(*record_set->m_Delete_Items.begin()).m_CloneEntry;
		}
		
		if (RE_UPGRAD_SUB_SUCCESS==res || RE_UPGRAD_SUB_FAIL_RP==res)
		{
			const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(iSubType, pkSub->GetStage());
			oss_role_vehicle_up(this, iPayIndex, iSubType, iPreStage, pkSub->GetStage(), gp-GetUnBindCash().GetMoney()
				, pkItem, res, 0, 0, entry ? entry->PeekASkill() : 0);
		}
	}
	
	if(LC_SUB_TP_VEHICLE == iSubType && GetRiderCharType() != 0)
	{
		if (res==RE_UPGRAD_SUB_SUCCESS)
		{
			OnVehicleUpgrade(iPreStage);
		}
	}

	return res;
}

void LC_ServerPlayer::OnVehicleUpgrade(int32_t iPreLevel)
{
	if (GetRiderCharType() == 0)
		return;
	DismissVehicle(true, iPreLevel);
	SummonVehicle();
}


void LC_ServerPlayer::SetRiderCharType(int lCharType)
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayer();
	if (m_pOwnerPlayer == NULL)
		return;

	if(!IsMainController())
	{
		return m_pOwnerPlayer->SetRiderCharType(lCharType);
	}


	LC_PlayerBase::SetRiderCharType(lCharType);

	LC_ServerPlayer* pkCurrentPlayer = GetCurrentActivateBuddy();
	if(NULL == pkCurrentPlayer)
	{
		return;
	}

	m_dataChangeFlag.GetBroadCastFlag().SetProperty();
	m_dataChangeFlag.GetBroadCastFlag().SetControllerProperty();

	if ( GetDirtyFlag(DIRTY_FLAG_PROPERTY) )
	{
		pkCurrentPlayer->m_RunSpeedUpDirty = true;
	}
}

int LC_ServerPlayer::GetRiderCharType()
{
	return LC_PlayerBase::GetRiderCharType();
}
//------------------------------------------------------------------------------------------
void LC_ServerPlayer::SummonVehicle()
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayer();
	if (m_pOwnerPlayer == NULL)
		return;

	if(!IsMainController())
	{
		return m_pOwnerPlayer->SummonVehicle();
	}

	LC_SubUpgradeData* pkSubData = GetSubordinateAsset().GetSubData(LC_SUB_TP_VEHICLE);
	
	if (NULL==pkSubData || !pkSubData->IsEnabled())
	{
		return;
	}
	int lVehicleStage = pkSubData->GetStage();
	if(lVehicleStage == 0)
		return;

	const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(LC_SUB_TP_VEHICLE, lVehicleStage);
	if (entry != NULL)
	{
		SetRiderCharType(entry->_iCharType);
	}	
	//修改速度
	RefreshAllHeroAllNewFinalAttrMapByTypeValue(ATT_MOVE_SPEED,entry->_iSpeed);
}
//------------------------------------------------------------------------------------------
void LC_ServerPlayer::DismissVehicle(bool upgrade, int PreLevel)
{
	LC_ServerPlayer* m_pOwnerPlayer = GetOwnerPlayer();
	if (m_pOwnerPlayer == NULL)
		return;
	if(!IsMainController())
		return m_pOwnerPlayer->DismissVehicle(upgrade, PreLevel);
	int lVehicle = 0;
	if(upgrade)
		lVehicle = PreLevel;
	else
		lVehicle = GetVehicleLevel();
	if(lVehicle == 0)
		return;
	if(GetRiderCharType()==0)
		return;
	//修改速度
	const LC_SubordinateDataEntry* entry = SafeGetSubDataEntry(LC_SUB_TP_VEHICLE, lVehicle);
	//修改速度
	RefreshAllHeroAllNewFinalAttrMapByTypeValue(ATT_MOVE_SPEED,entry->_iSpeed,false);
	SetRiderCharType(0);
}

void LC_ServerPlayer::GoRider()
{
	//给主角加上对应等级的坐骑buff，buff里面会设置骑上坐骑
	if ( GetVehicleLevel() > 0)
	{
		int stateID = RiderStateBegin + GetVehicleLevel() - 1;
		ActiveSkillStateToAllBuddyInstance(stateID,-1,m_iID,0,0,0,0,0);
	}
}

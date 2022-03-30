#include "LM_LocaleManager.h"

#include "UT_GameServerLogManager.h"
#include "UT_DirConfig.h"
#include "../Utility/UT_OperateLog.h"
#ifndef ACTIVATION_CODE
#include "LC_Helper.h"
#include "LC_ServerItemFlopManager.h"
#include "LC_PlayerBase.h"
#include "LC_WorldManagerBase.h"
#include "LC_ActorBase.h"
#include "LC_LogicManagerBase.h"
#include "GameServerApp.h"
#include "GlobalSettings.h"
using namespace CSVFile;
using namespace GameLogic;
#else
#include "ACGameServerApp.h"
#endif

using namespace Utility;
using namespace OPERATELOG;

UT_GameServerLogManager::UT_GameServerLogManager(void)
: m_pkLogManager(0)
, m_bPrintSkillValidateInfo(false)
{
	m_pkLogManager				= T_NEW_D UT_LogManager;

	//下面将客户端需要使用的LogStream注册到各自的LogTag
	_registerLog();
}

UT_GameServerLogManager::~UT_GameServerLogManager(void)
{
	T_SAFE_DELETE(m_pkLogManager);
}

void UT_GameServerLogManager::set_console( bool flag )
{
	console_debug->set_enable(flag);
}

void UT_GameServerLogManager::_registerLog( void )
{
	int serverId =	GetGameServerApp()->GetServerManager()->GetServerIdFromArgv();
	if(serverId == -1)
		serverId = GetGameServerApp()->GetServerManager()->GetCurServerId();
	StringType strLogDir = GetGameServerApp()->GetLogPath();
	std::wstring wLogDir = CodeConvert::MbsToWcs(strLogDir);	// mbs or utf8?取决于GlobalSetting.lua编码
	//log文件
	{
		LOG_TAG eaTag[] = {LOG_TAG_SYSTEM, LOG_TAG_INFO, LOG_TAG_WARNING, LOG_TAG_ERROR, LOG_TAG_SKILL_EVENT, LOG_TAG_GUILD};
		int iTagCount = sizeof(eaTag) / sizeof(LOG_TAG_START);
		m_pkLogManager->RegisterLog( eaTag, iTagCount, T_NEW_D UT_LogStreamFileOSS(L"GAME", wLogDir.c_str(),serverId) );

		LOG_TAG eaTagRES = LOG_TAG_RES;
		m_pkLogManager->RegisterLog( eaTagRES, T_NEW_D UT_LogStreamFileOSS(L"RES", wLogDir.c_str(),serverId) );

		LOG_TAG eaTagERROR = LOG_TAG_ERROR;
		m_pkLogManager->RegisterLog( eaTagERROR, T_NEW_D UT_LogStreamFileOSS(L"ERROR", wLogDir.c_str(),serverId) );
	
		LOG_TAG eaTagGM = LOG_TAG_GM;
		m_pkLogManager->RegisterLog( eaTagGM, T_NEW_D UT_LogStreamFileOSS(L"GM", wLogDir.c_str(),serverId) );

		LOG_TAG eaTagBILL = LOG_TAG_BILL;
		m_pkLogManager->RegisterLog( eaTagBILL, T_NEW_D UT_LogStreamFileOSS(L"BILL", wLogDir.c_str(),serverId) );

		LOG_TAG eaTagItem = LOG_TAG_ITEM;
		m_pkLogManager->RegisterLog( eaTagItem, T_NEW_D UT_LogStreamFileOSS(L"ITEM", wLogDir.c_str(),serverId) );

		LOG_TAG eaTagTimer = LOG_TAG_TIMER;
		m_pkLogManager->RegisterLog( eaTagTimer, T_NEW_D UT_LogStreamFileOSS(L"TIMER", wLogDir.c_str(),serverId) );

		LOG_TAG eaTagOperate = LOG_TAG_OPERATE;
		wLogDir = CodeConvert::MbsToWcs(GetGlobalSetting.sOperateLogDir);	// mbs or utf8?
		m_pkLogManager->RegisterLog( eaTagOperate, T_NEW_D UT_OperateLogStreamFileOSS(L"YUNYING", wLogDir.c_str(), serverId) );
	}

	//debug窗口
	{
		LOG_TAG eaTag[] = {LOG_TAG_SYSTEM, LOG_TAG_WARNING, LOG_TAG_ERROR, LOG_TAG_RES, LOG_TAG_INFO};
		int iTagCount = sizeof(eaTag) / sizeof(LOG_TAG_START);
		console_debug = T_NEW_D UT_LogStreamConsoleDebug();
		m_pkLogManager->RegisterLog( eaTag, iTagCount, console_debug);
	}
}

#ifndef	ACTIVATION_CODE
void UT_GameServerLogManager::SKA_LogActorInfo( LC_ActorBase* pkActor, const StringType& rkRemark, const StringType& rkFieldPrefix )
{
	//如果不需要打印技能验证信息
	if (!m_bPrintSkillValidateInfo)
	{
		return;
	}

	LC_ActorAttributeMap* pkAttributeMap = pkActor->GetAttributeMap();

	object_id_type lID = pkActor->GetID();
	StringType kName = pkActor->GetOwnerCharName();
	int lLevel = pkActor->GetLevel();
	
	attr_value_type lHP = pkActor->GetHP();
	attr_value_type iMaxHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);
	
	attr_value_type lMP = pkActor->GetMP();
	attr_value_type iMaxMP = pkAttributeMap->GetAttribute(ATT_MAX_MP);

	attr_value_type iMaxPHA = pkAttributeMap->GetAttribute(ATT_MAX_ATK);
	attr_value_type iPHD = pkAttributeMap->GetAttribute(ATT_DEF);

	float fAttackFrequency = 1.0f;
	attr_value_type iDodgeRate = pkAttributeMap->GetAttribute(ATT_DOG_RATE);

	attr_value_type iCriticalRate = pkAttributeMap->GetAttribute(ATT_CIR_RATE);
	attr_value_type iCriticalDamage = pkAttributeMap->GetAttribute(ATT_CIR_DAM);

	StringType kMustHit = pkActor->GetSkillFlag(SK_ATTACK_FLAG) ? "yes" : "no";
	StringType kMustCritical = pkActor->GetSkillFlag(SK_CRITICAL_FLAG) ? "yes" : "no";

	double fHPRecoverRate = LC_Helper::GetAttributeRate(pkActor, ATT_HP_RECOVER_RATE);
	double fHPCostRate = LC_Helper::GetAttributeRate(pkActor, ATT_HP_COST_RATE);
	double fMPRecoverRate = LC_Helper::GetAttributeRate(pkActor, ATT_MP_RECOVER_RATE);
	double fMPCostRate = LC_Helper::GetAttributeRate(pkActor, ATT_MP_COST_RATE);

	attr_value_type iHatred = pkAttributeMap->GetAttribute(ATT_HATRED);

	float fGlobalCoolDownTime = 0.0f;
	if ( IS_PLAYER_ID(pkActor->GetID()) )
	{
		LC_SkillCoolDownMap* pkCoolDownMap = ((LC_PlayerBase*)pkActor)->GetSkillCoolDownMap();
		fGlobalCoolDownTime = pkCoolDownMap->GetGlobalCoolDownTime();
	}

	StringStreamType kStrStream;
	kStrStream << "********************" << rkRemark << "********************\n"
		<< rkFieldPrefix << "ID:" << lID << "," << rkFieldPrefix << "Name:" << kName << "," << rkFieldPrefix << "Level:" << lLevel << ",\n"
		<< rkFieldPrefix << "HP:" << lHP << "/" << iMaxHP << "," << rkFieldPrefix << "MP:" << lMP << "/" << iMaxMP << ",\n"
		<< rkFieldPrefix << "PH:" <<  iMaxPHA << "," << rkFieldPrefix << "PHD:" << iPHD << ",\n"
		<< rkFieldPrefix << "Attack_frequency:" << fAttackFrequency << "," << rkFieldPrefix << "Dodge_rate:" << iDodgeRate << ",\n"
		<< rkFieldPrefix << "Critical_rate:" << iCriticalRate << "," << rkFieldPrefix << "Critical_damage:" << iCriticalDamage << "," << rkFieldPrefix << ",\n"
		<< rkFieldPrefix << "True_target:" << kMustHit << ",\n"
		<< rkFieldPrefix << "True_critical:" << kMustCritical << ",\n"
		<< rkFieldPrefix << "HP_reg_rate:" << fHPRecoverRate << "," << rkFieldPrefix << "HP_con_rate:" << fHPCostRate << ",\n"
		<< rkFieldPrefix << "MP_reg_rate:" << fMPRecoverRate << "," << rkFieldPrefix << "MP_con_rate:" << fMPCostRate << ",\n"
		<< rkFieldPrefix << "Hatred:" << iHatred << ",\n"
		<< rkFieldPrefix << "Global_cool_down:" << fGlobalCoolDownTime << "\n\n";

	GfxWriteLog(LOG_TAG_VALIDATE_SKILL, LOG_SWITCH_VALIDATE_SKILL, kStrStream.str().c_str());
}

void UT_GameServerLogManager::SKA_LogSkillInfo( LC_ActorBase* pkAttacker, LC_ActorBase* pkTarget, CF_SkillSpecialAttrib::DataEntry* pkSkillData, float fFinalAttackRate, float fFinalCriticalRate, bool bAttack, bool bCritical, attr_value_type iDamage, int iDefence, int iMPCost, attr_value_type iFinalDamage, const StringType& kRemark )
{
	//如果不需要打印技能验证信息
	if (!m_bPrintSkillValidateInfo)
	{
		return;
	}

	int lSkillID = pkSkillData->_iID;
	CF_SkillExecuteList::DataEntry *pkSkillInfo = LC_Helper::SafeGetSkillExecuteDataEntryPtr(lSkillID);
	if(!pkSkillInfo)
	{
		return;
	}

	
	float fSkillAttackRate = pkSkillData->_fAttackRate;
	float fDamageRate = pkSkillData->_fDamageRate;				//攻击系数
	float fSkillCriticalRatte = pkSkillData->_fCriticalRate;
	int lCriticalDamage = pkSkillData->_lCriticalDamageAdd;
	float fHatred = pkSkillInfo->_fHatredQuot;					//仇恨系数
	StringType kAttack = bAttack ? "yes" : "no";
	StringType kCritical = bCritical ? "yes" : "no";
	StringType kDamageType = "";
	switch (pkSkillInfo->_lDamageType)
	{
	case SDT_NORMAL:
	case SDT_SKILL:
	case SDT_LC_SKILL:
		kDamageType = "attack";
		break;
	default:
		break;
	}

	StringType kAttackerName = pkAttacker->GetOwnerCharName();
	StringType kTargetName = pkTarget->GetOwnerCharName();
	UT_Vec3Int kAttackerPos = pkAttacker->GetCurrentLocation();
	UT_Vec3Int kTargetPos = pkTarget->GetCurrentLocation();
	int iDistance = UT_MathBase::LineLengthXYInt(kAttackerPos, kTargetPos);

	//本次技能产生的仇恨值
	attr_value_type lHatredValue = LC_Helper::ComputeHatredValue(iFinalDamage, pkAttacker->GetID(), pkSkillData->_iID);

	StringStreamType kStrStream;
	kStrStream << "********************" << kRemark << "********************\n"
		<< "Skill_ID:" << lSkillID << "\n"
		<< "Skill_attack_rate:" << fSkillAttackRate << ",Skill_damage_rate:" << fDamageRate << ",Skill_damage_type:" << kDamageType << ",\n"
		<< "Skill_critical_rate:" << fSkillCriticalRatte << ",Skill_critical_damage:" << lCriticalDamage << ",\n"
		<< "Skill_hatred:" << fHatred << ",\n"
		//<< "Attacker:" << kAttackerName << ",Attacker_pos:(" << kAttackerPos.x << "," << kAttackerPos.y << "," << kAttackerPos.z << "),\n" 
		//<< "Target:" << kTargetName << ",Target_pos:(" << kTargetPos.x << "," << kTargetPos.y << "," << kTargetPos.z << "),\n"
		<< "Attacker:" << kAttackerName << ",Target:" << kTargetName << ",Distance:" << iDistance << ",\n"
		<< "Attacker_damage:" << iDamage << ",Target_defence:" << iDefence << ",\n"
		<< "Final_attack_rate:" << fFinalAttackRate << ",Final_critical_rate:" << fFinalCriticalRate << ",MP_cost:" << iMPCost << ",Final_damage:" << iFinalDamage << "\n"
		<< "Attack:" << kAttack << ",Critical:" << kCritical << ",Hatred:" << lHatredValue <<"\n\n";
	
	GfxWriteLog(LOG_TAG_VALIDATE_SKILL, LOG_SWITCH_VALIDATE_SKILL, kStrStream.str().c_str());
}

void UT_GameServerLogManager::SKA_LogRemark( const StringType& kRemark )
{
	//如果不需要打印技能验证信息
	if (!m_bPrintSkillValidateInfo)
	{
		return;
	}

	StringStreamType kStrStream;
	kStrStream << "------------------------------" << kRemark << "------------------------------\n";
	GfxWriteLog(LOG_TAG_VALIDATE_SKILL, LOG_SWITCH_VALIDATE_SKILL, kStrStream.str().c_str());
}

void UT_GameServerLogManager::LogFlopInfo(LC_ItemFlopInfoList* pkFlopInfoList)
{
	//如果不需要打印技能验证信息
	if (!m_bPrintSkillValidateInfo)
	{
		return;
	}

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();

	int iFlopInfoCount = pkFlopInfoList->GetItemFlopInfoCount();
	LC_ItemFlopInfo* pkFlopInfo = NULL;
	object_id_type lOwnerID[5];
	int lOwnerLevel[5];
	int lItemID[10];
	memset(lOwnerID, 0, sizeof(lOwnerID));
	memset(lOwnerLevel, 0, sizeof(lOwnerLevel));
	memset(lItemID, 0, sizeof(lItemID));
	for (int i = 0; i < iFlopInfoCount; i ++)
	{
		pkFlopInfo = pkFlopInfoList->GetItemFlopInfoByIndex(i);
		if (NULL == pkFlopInfo)
		{
			continue;
		}
		object_id_type lNPCID = pkFlopInfo->GetProducerID();
		if (!IS_NPC_ID(lNPCID))
		{
			continue;
		}
		LC_ActorBase* pkNPC = (LC_ActorBase*)pkWorldManager->FindObject(pkFlopInfo->GetProducerID());
		if (NULL == pkNPC)
		{
			continue;
		}
		int lNPCCharType = pkNPC->GetCharType();
		int lNPCLevel = pkNPC->GetLevel();
		const object_id_vector& rkTargetIDVector = pkFlopInfo->GetOwnerIDVector();
		if (rkTargetIDVector.empty())
		{
			GfxWriteLog(LOG_TAG_VALIDATE_FLOP, LOG_SWITCH_VALIDATE_FLOP, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",
				lNPCCharType, lNPCLevel, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			continue;
		}
		
		int iOwnerCount = sizeof(lOwnerID) / sizeof(object_id_type);
		iOwnerCount = (int)rkTargetIDVector.size() > iOwnerCount ? iOwnerCount : (int)rkTargetIDVector.size();
		for (int i = 0; i < iOwnerCount; i ++)
		{
			lOwnerID[i] = rkTargetIDVector[i];
			if (IS_PLAYER_ID(lOwnerID[i]))
			{
				LC_ActorBase* pkTarget = (LC_ActorBase*)pkWorldManager->FindObject(lOwnerID[i]);
				if (NULL != pkTarget)
				{
					lOwnerLevel[i] = pkTarget->GetLevel();
				}
			}
		}

		LC_BackPackEntryList& rkPackEntryList = pkFlopInfo->GetBackPackEntryList();
		if (rkPackEntryList.empty())
		{
			GfxWriteLog(LOG_TAG_VALIDATE_FLOP, LOG_SWITCH_VALIDATE_FLOP, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",
				lNPCCharType, lNPCLevel, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			continue;
		}
		int iPackEntryCount = sizeof(lItemID) / sizeof(int);
		iPackEntryCount = (int)rkPackEntryList.size() > iPackEntryCount ? iPackEntryCount : (int)rkPackEntryList.size();
		for (int i = 0; i < iPackEntryCount; i ++)
		{
			lItemID[i] = rkPackEntryList[i].GetBackPackValueByType(LPSFET_TYPE_ID);
		}

		GfxWriteLog(LOG_TAG_VALIDATE_FLOP, LOG_SWITCH_VALIDATE_FLOP, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",
			lNPCCharType, lNPCLevel, 1, 
			lOwnerID[0], lOwnerLevel[0], 
			lOwnerID[1], lOwnerLevel[1], 
			lOwnerID[2], lOwnerLevel[2],
			lOwnerID[3], lOwnerLevel[3], 
			lOwnerID[4], lOwnerLevel[4], 
			lItemID[0], lItemID[1], lItemID[2], lItemID[3], lItemID[4], lItemID[5], lItemID[6], lItemID[7], lItemID[8], lItemID[9]);
	}
}
#endif



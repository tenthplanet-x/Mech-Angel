#include "BotInstance.h"
#include "CF_Bot.h"
#include "CF_SkillLevelUp.h"
#include "CF_SkillExecuteList.h"
#include "CF_WorldMapList.h"
#include "CF_LevelUp.h"
#include "CF_ItemGen.h"
#include "CF_ItemList.h"
#include "CF_Helper.h"
#include "CF_ItemEquipLimitFunc.h"
#include "LC_ServerPlayer.h"
#include "LC_ItemFactoryBase.h"
#include "LC_ServerItem.h"
#include "LC_GemReinforceAsset.h"
#include "SK_SkillExecute.h"
#include "SK_Factory.h"
#include "MG_Stream.h"
#include "MG_PB.h"
#include "MG_AssistProtocol.h"
#include "MG_Common.h"
#include "MG_Forbid.h"
#include "MG_BotProtocol.h"
#include "GlobalSettings.h"
#include "LC_GameEventManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "../Buddy/BuddyHelper.h"

using namespace GameLogic;
using namespace Skill;

#define			MAX_PLAYER_LEVEL		60
#define			DEFAULT_INIT_LEVEL		1

BotInstance::BotInstance(object_id_type nOwnerPlayerId, BotInstanceMgr* pMgr)
	: m_nOwnerPlayerId(nOwnerPlayerId)
	, m_pConfig( NULL )
	, m_nTimeStamp(0)
	, m_pInstanceMgr(pMgr)
	, m_nLevel(1)
{
}

BotInstance::~BotInstance()
{
	//T_SAFE_DELETE(m_Data.m_Data_PlayerBase.m_pkShortcutBar);
}

void BotInstance::InitGameData()
{
}

void BotInstance::InitTemplateData(const CSVFile::CF_Bot::DataEntry* pData, bool bInit)
{
	if (pData == NULL)
		return;

	m_pConfig = const_cast<CSVFile::CF_Bot::DataEntry*>(pData);

	m_nLevel = DEFAULT_INIT_LEVEL;
	m_nTimeStamp = time(NULL);
}

void BotInstance::Save(char_data_define::bot_single_instance_data& data)
{
	data.set_level(m_nLevel);
	data.set_resid(m_pConfig->_cID);
	data.set_timestamp(m_nTimeStamp);
}

void BotInstance::Load(const char_data_define::bot_single_instance_data& data)
{
	m_nLevel = data.level();
	int32_t nResId = data.resid();
	m_nTimeStamp = data.timestamp();
	OnLevelData( m_nLevel, m_nLevel, true);
}

void BotInstance::OnActivate()
{
	// Flush property
}

LC_ServerPlayer* BotInstance::GetOwnerPlayer()
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(m_nOwnerPlayerId);
	return pPlayer;
}

void BotInstance::SyncAllDataToClient()
{
}

int BotInstance::GetQuality()
{
	// 字段修改之后这里直接返回 
	return m_pConfig->_iQuality;
}

// for subordinate logic.
// commented by weixin
void BotInstance::OnCreate()
{
#if 0
#endif
}

int32_t BotInstance::GetTimeStamp()
{
	return m_nTimeStamp;
}

uint32_t BotInstance::GetTemplateId()
{
	if (m_pConfig == NULL)
		return INVALID_ACTOR_ID;
	return m_pConfig->_cID;
}

int16_t BotInstance::GetLevel()
{
	return m_nLevel;
}

CSVFile::CF_Bot::DataEntry* BotInstance::GetConfig()
{
	return m_pConfig;
}

int BotInstance::UpgradeLevel()
{
	DoUpgrade();
	return true;
}

int BotInstance::DoUpgrade()
{
	int nResult = true;
	int32_t nOrigStarLevel = m_nLevel;
	m_nLevel++;
	OnLevelData( nOrigStarLevel, m_nLevel, false);

	return nResult;
}

int BotInstance::GetNormalRewardByType( int32_t nType, uint32_t& nItemId, uint32_t& nCount, CSVFile::CF_BotUpgrade::DataEntry* pCfg  )
{
	switch ( nType )
	{
	case eNormalBonus_0:
		nItemId = pCfg->_iReward1;
		nCount = pCfg->_iEfficiency1;
		break;
	case eNormalBonus_1:
		nItemId = pCfg->_iReward2;
		nCount = pCfg->_iEfficiency2;
		break;
	case eNormalBonus_2:
		nItemId = pCfg->_iReward3;
		nCount = pCfg->_iEfficiency3;
		break;
	case eNormalBonus_3:
		nItemId = pCfg->_iReward4;
		nCount = pCfg->_iEfficiency4;
		break;
	case eNormalBonus_4:
		nItemId = pCfg->_iReward5;
		nCount = pCfg->_iEfficiency5;
		break;
	case eNormalBonus_5:
		nItemId = pCfg->_iReward6;
		nCount = pCfg->_iEfficiency6;
		break;
	case eNormalBonus_6:
		nItemId = pCfg->_iReward7;
		nCount = pCfg->_iEfficiency7;
		break;
	case eNormalBonus_7:
		nItemId = pCfg->_iReward8;
		nCount = pCfg->_iEfficiency8;
		break;
	case eNormalBonus_8:
		nItemId = pCfg->_iReward9;
		nCount = pCfg->_iEfficiency9;
		break;
	case eNormalBonus_9:
		nItemId = pCfg->_iReward10;
		nCount = pCfg->_iEfficiency10;
		break;
	default:
		return false;
		break;
	}

	return true;
}

int BotInstance::DoNormalReward( LC_ServerPlayer* pOwner, BotInstanceMgr* pMgr, int nCount, bool bSimple, int nType )
{
	CSVFile::CF_BotUpgrade::DataEntry* pCfg = const_cast<CSVFile::CF_BotUpgrade::DataEntry*>(BuddyHelper::GetBotUpgradeData(m_pConfig->_cID, m_nLevel ));
	if ( pCfg == NULL )
		return false;

	uint32_t nSimpleItemId = 0;
	uint32_t nSimpleCount = 0;
	int nRetCode = GetNormalRewardByType(nType, nSimpleItemId, nSimpleCount, pCfg);
	if (nRetCode == false)
		return false;

	if ( bSimple )
	{
		pMgr->ModifyBonusCache_Simple( nSimpleItemId, nSimpleCount * nCount );
	}
	else
	{
		pMgr->ModifyBonusCache( nSimpleItemId, nSimpleCount * nCount );
	}
	if (nSimpleCount == 0)
		return false;
	return true;
}

int BotInstance::DoTalentReward( LC_ServerPlayer* pOwner, BotInstanceMgr* pMgr, int nCount )
{
	CSVFile::CF_BotUpgrade::DataEntry* pCfg = const_cast<CSVFile::CF_BotUpgrade::DataEntry*>(BuddyHelper::GetBotUpgradeData(m_pConfig->_cID, m_nLevel ));
	if ( pCfg == NULL )
		return false;

	uint32_t nProductId = pCfg->_sRewardTalent;
	uint16_t nProductCount = pCfg->_iEfficiencyTalent;
	if ( nProductId == -1 )
		return false;
	//nProductId = 3965;
	//nProductCount = 1 * nCount;
	//pOwner->AddDropByDisassemble(nProductId, true, nProductCount);
	for ( int i = 0; i < nProductCount; i++ )
		pOwner->AddDropByDisassemble(nProductId, true, 1);

	return true;
}

void BotInstance::OnLevelData( int nOrigLevel, int nLevel, bool bInit)
{
	const CF_BotUpgrade::DataEntry* pOrigData = BuddyHelper::GetBotUpgradeData(m_pConfig->_cID, nOrigLevel);
	const CF_BotUpgrade::DataEntry* pCurData = BuddyHelper::GetBotUpgradeData(m_pConfig->_cID, nLevel);

	if (pOrigData == NULL || pCurData == NULL)
		return;

	//LC_ActorAttributeMap attr_for_score;
	//OnBothLevelData_Attribute(attr_for_score, m_pConfig, pkLevelData, pCurStarData, true);
}


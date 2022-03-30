
#include "GameServerApp.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerGuildManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerNPC.h"
#include "LC_ServerWarManager.h"
#include "LC_ServerMapLogic_BatchPlay.h"
#include "LC_ServerMapLogicFactory.h"
#include "LC_ServerMapLogic_Place.h"
#include <boost/algorithm/string/replace.hpp>
#include "LC_ServerPlayerGroup.h"
#include "UT_ServerHelper.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "oss_define.h"
#include "CF_GuildWarCatapult.h"
#include "LC_ServerWarManager.h"
#include "LC_ServerMapLogic_WarPlay.h"
#include "CF_GuildReward.h"
#include "SC_ScriptAction.h"

using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

#define SYNC_POWERPLAYER_POS 1
#define SYNC_ALLENEMYPLAYER_POS 2

bool WarCatapultBullet::Init(int Id, int launcher, int operater, float cruiseTime, float warnTime, int ResId, int LogicId, const Utility::UT_Vec3Int& pos, int bulletlevel, int chartype)
{
	m_Id = Id;
	m_Launcher = launcher; //发射器
	m_Operator = operater; //发射者
	m_CruiseTime = cruiseTime;
	m_WarnTime = warnTime;
	m_MapResId = ResId;
	m_MapLogicId = LogicId;
	m_TargetPos = pos;
	m_BullteState = CatapultBulletState_Inited;
	m_Time = GET_PROCESS_TIME + 10;
	m_BulletLevel = bulletlevel;
	m_CharType = chartype;
	return true;
}

void WarCatapultBullet::UnInit()
{

}

void WarCatapultBullet::Update(float fCurrentTime, float fDeltaTime)
{
	if (m_BullteState == CatapultBulletState_Inited)
	{
		if (fCurrentTime >= m_Time)
		{
			m_BullteState = CatapultBulletState_Dud;
		}
	}
	else if (m_BullteState == CatapultBulletState_Cruise)
	{
		if (fCurrentTime >= m_Time)
		{
			CatapultWarn();
		}
	}
	else if (m_BullteState == CatapultBulletState_Warn)
	{
		if (fCurrentTime >= m_Time)
		{
			CatapultExplode();
		}
	}
}

void WarCatapultBullet::CatapultCruise()//巡航
{
	m_Time = GET_PROCESS_TIME + (m_CruiseTime - m_WarnTime);
	m_BullteState = CatapultBulletState_Cruise; //导弹状态
}

void WarCatapultBullet::CatapultWarn()//预警
{
	m_Time = GET_PROCESS_TIME + m_WarnTime;
	m_BullteState = CatapultBulletState_Warn; //导弹状态
	//刷出NPC
	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkSpawnManager = pkLogicManager->GetSpawnManager();
	if (NULL == pkSpawnManager)
	{
		return;
	}
	LC_TranscriptionListEntry transEntry;
	transEntry.m_iMapID = m_MapLogicId;
	transEntry.m_kSpawnLocation	= m_TargetPos;
	transEntry.m_iSpawnCharType		= m_CharType;
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkSpawnManager->ManualSpawnNPC(transEntry);
	pkNPC->SetFollowedTargetID(m_Operator);
}

void WarCatapultBullet::CatapultExplode()//爆炸
{
	m_BullteState = CatapultBulletState_Explode; //导弹状态
}

void WarCatapultBullet::CatapultDud()//爆炸
{
	m_BullteState = CatapultBulletState_Dud; //导弹状态
}

bool WarCatapultBullet::isEnd()
{
	if (m_BullteState == CatapultBulletState_Explode || m_BullteState == CatapultBulletState_Dud)
	{
		return true;
	}
	return false;
}

bool WarCatapultBullet::isInit()
{
	if (m_BullteState == CatapultBulletState_Inited)
	{
		return true;
	}
	return false;
}

int WarCatapultBullet::GetOperator()
{
	return m_Operator;
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
bool WarCatapult::Init(LC_ServerMapLogic_WarPlay* parent, int chartype, const Utility::UT_Vec3Int& pos, int radius, float cruiseTime, float warnTime, int angel, int ResId, int LogicId, int vaildDis)
{
	m_Pos = pos;
	m_Radius = radius;
	m_CruiseTime = cruiseTime;
	m_WarnTime = warnTime;
	m_Angel = angel;
	m_CDTime = 0;
	m_CDTimeStamp = 0;
	m_MapResId = ResId;
	m_MapLogicId = LogicId;
	m_VaildDis = vaildDis;
	m_CatapultBullet.clear();
	m_IdBegin = 0;
	m_Parent = parent;
	//创建NPC
	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkSpawnManager = pkLogicManager->GetSpawnManager();
	if (NULL == pkSpawnManager)
	{
		return false;
	}
	LC_TranscriptionListEntry transEntry;
	transEntry.m_iMapID = LogicId;
	transEntry.m_kSpawnLocation	= m_Pos;
	transEntry.m_iSpawnCharType	= chartype;
	transEntry.m_iZRotAngle = m_Angel;
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkSpawnManager->ManualSpawnNPC(transEntry);
	if (!pkNPC)
	{
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "WarCatapult::Init NPC ID error");
		return false;
	}
	m_Id = pkNPC->GetID();
	return true;
}

void WarCatapult::UnInit()
{
	WarCatapultBulletVector::iterator beg = m_CatapultBullet.begin();
	for (; beg != m_CatapultBullet.end(); ++beg)
	{
		T_SAFE_DELETE(*beg);
	}
	m_CatapultBullet.clear();
}

void WarCatapult::Update(float fCurrentTime, float fDeltaTime)
{
	WarCatapultBulletVector::iterator beg = m_CatapultBullet.begin();
	for (; beg != m_CatapultBullet.end();)
	{
		if (NULL == (*beg) || (*beg)->isEnd())
		{
			T_SAFE_DELETE(*beg);
			beg = m_CatapultBullet.erase(beg);
		}
		else
		{
			(*beg)->Update(fCurrentTime, fDeltaTime);
			++beg;
		}
	}
	if (m_Operator != INVALID_ACTOR_ID)
	{
		if (__CheckControl(m_Operator) != RE_SUCCESS)
		{
			ClearControl(false);
		}
	}
	//发送敌对方信息
	if (m_Operator != INVALID_ACTOR_ID)
	{
		m_Parent->SyncAllEnemyPlayerInfo(m_Operator);
	}
}

WarCatapultBullet* WarCatapult::GetBullte(int id)
{
	WarCatapultBulletVector::iterator beg = m_CatapultBullet.begin();
	for (; beg != m_CatapultBullet.end(); ++beg)
	{
		if (*beg && (*beg)->m_Id == id)
		{
			return *beg;
		}
	}
	return NULL;
}

int WarCatapult::GetControlPlayerBullteCount()
{
	if (m_Operator == INVALID_ACTOR_ID)
	{
		return 0;
	}
	int count = 0;
	WarCatapultBulletVector::iterator beg = m_CatapultBullet.begin();
	for (; beg != m_CatapultBullet.end(); ++beg)
	{
		if ((*beg) && !(*beg)->isEnd())
		{
			++count;
		}
	}
	return count;
}

ResultType WarCatapult::Fire(LC_ServerPlayer* player, Utility::UT_Vec3Int& TargetPos, int bulletlevel, int& iCost)
{
	if (NULL == player)
	{
		return RE_FAIL;
	}
	ResultType ret = __CheckControl(player->GetID());
	if (ret != RE_SUCCESS)
	{
		return ret;
	}
	if (player->GetID() != (uint32_t)m_Operator)
	{
		return RE_GUILD_WAR_CATAPULT_IN_CONTROL;
	}
	if (GetControlPlayerBullteCount() > 0)
	{
		return RE_GUILD_WAR_CATAPULT_IN_CD;
	}
	CF_GuildWarCatapult::DataEntry* pkData = CF_GuildWarCatapult::GetSingletonPtr()->GetEntryPtr(bulletlevel);
	if (pkData == NULL)
	{
		return RE_FAIL;
	}

	if (NULL == m_Parent || NULL == m_Parent->GetPlace())
	{
		return RE_FAIL;
	}

	if (!m_Parent->GetPlace()->CheckPosVaild(TargetPos.x, TargetPos.y))
	{
		return RE_FAIL;
	}
	LC_ServerGuildInfo* guildinfo = m_Parent->GetGuild(player->GetGuildID());
	if (guildinfo == NULL)
	{
		return RE_GUILD_WAR_CANT_FIRE;
	}
	//检查钱
	int64_t costint = pkData->_Cost;
	iCost = (int)costint;
	//金币
	mem::vector<PS_CostEntry> entry;
	PS_CostEntry temp;
	temp.m_ItemType = 0;
	temp.m_ItemCount = costint;
	entry.push_back(temp);
	ret = guildinfo->CheckCost(entry);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}

	WarCatapultBullet* bullet = T_NEW_D WarCatapultBullet;
	if (!bullet->Init(++m_IdBegin, m_Id, player->GetID(), m_CruiseTime, m_WarnTime, m_MapResId, m_MapLogicId, TargetPos, bulletlevel, pkData->_CharID))
	{
		return RE_FAIL;
	}
	m_CatapultBullet.push_back(bullet);
	mem::map<int, int64_t> cost;
	cost.insert(std::make_pair(0, costint));
	ret = guildinfo->HandleAssert(player, cost, true, false, GuildItemCost_WarCatapult, m_Id, bullet->m_Id);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}
	return RE_SUCCESS;
}

bool WarCatapult::FireResult(LC_ServerPlayer* player, ResultType result, int bullteid)
{
	if (NULL == m_Parent || NULL == m_Parent->GetPlace())
	{
		return false;
	}

	WarCatapultBullet* bullet = GetBullte(bullteid);
	if (bullet == NULL)
	{
		return false;
	}
	MG_GuildWar_FireCatapult_Resp msg;
	msg.m_CatapultId = m_Id;
	msg.m_TargetX = bullet->m_TargetPos.x;
	msg.m_TargetY = bullet->m_TargetPos.y;
	msg.m_CruiseTime = m_CruiseTime;
	msg.m_CatapultLevel = bullet->m_BulletLevel;
	if (result != RE_SUCCESS)
	{
		bullet->CatapultDud();
		msg.m_Result = result;
	}
	else
	{
		bullet->CatapultCruise();
		msg.m_Result = RE_SUCCESS;
	}
	player->SendMsgToClient(MGPT_RESP_GUILDWAR_CATAPULT_FIRE, &msg);
	LC_ServerNPC* npc = m_Parent->GetPlace()->GetActorManger().GetNpc(m_Id);
	if (npc != NULL)
	{
		npc->SetLockedLocation(bullet->m_TargetPos);
		npc->ExecuteSkill(21915641);
	}
	return true;
}

ResultType WarCatapult::__CheckControl(object_id_type playerid)
{
	if (playerid == INVALID_ACTOR_ID || NULL == m_Parent || NULL == m_Parent->GetPlace())
	{
		return RE_FAIL;
	}
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* player = (LC_ServerPlayer*)playManager->FindPlayer(playerid);
	if (player == NULL)
	{
		return RE_FAIL;
	}
	if (player->GetDead())
	{
		return RE_FAIL;
	}
	if (!m_Parent->GetPlace()->CheckPlayerIn(playerid))
	{
		return RE_FAIL;
	}
	uint64_t guildid = player->GetGuildID();
	if (guildid == INVALID_GUILD_ID)
	{
		return RE_GUILD_WAR_CANT_FIRE;
	}
	ResultType ret = m_Parent->CheckPowerOwnerless();
	if (ret != RE_SUCCESS)
	{
		if (m_Parent->GetWarType() == WarType_GuildWar)
		{
			ret = RE_GUILDWAR_CAPATA_OWNER;
		}
		else if (m_Parent->GetWarType() == WarType_CrossRealmGuildWar)
		{
			ret = RE_GUILDWAR_CROSS_CAPATA_OWNER;
		}
		return ret;
	}
	int iDistance = UT_MathBase::LineLengthXYInt(m_Pos, player->GetCurrentLocation());
	if (iDistance > m_VaildDis)
	{
		return RE_GUILD_WAR_CATAPULT_FAR;
	}
	return RE_SUCCESS;
}

//0 有人控制不是你，1 你以控制，2 无人控制但你不满足控制条件，3 无人控制，你满足控制条件
int WarCatapult::CheckCanControl(LC_ServerPlayer* player)
{
	if (NULL == player || NULL == m_Parent || NULL == m_Parent->GetPlace())
	{
		return 2;
	}
	WarCatapult* thecatapult = m_Parent->GetPlayerControlCatapult(player->GetID());
	if (thecatapult != NULL && thecatapult->m_Id != m_Id)
	{
		m_Parent->GetPlace()->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, RE_GUILD_POWER_CANT_CONTROL_MORE);
		if (m_Operator == INVALID_ACTOR_ID)
		{
			return 2;
		}
		else
		{
			return 0;
		}
	}
	if (m_Operator != INVALID_GUILD_ID) //有人控制
	{
		if (player->GetID() != (uint32_t)m_Operator)
		{
			m_Parent->GetPlace()->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, RE_GUILD_WAR_CATAPULT_IN_CONTROL);
			return 0;
		}
		return 1;
	}
	ResultType ret = __CheckControl(player->GetID());
	if (ret != RE_SUCCESS)
	{
		m_Parent->GetPlace()->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, ret);
		return 2;
	}
	return 3;
}

ResultType WarCatapult::Control(LC_ServerPlayer* player)
{
	if (NULL == player || NULL == m_Parent || NULL == m_Parent->GetPlace())
	{
		return RE_FAIL;
	}
	int ret = CheckCanControl(player);
	if (ret == 0 || ret == 2)
	{
		return RE_FAIL;
	}
	if (ret == 3)
	{
		SetControl(player);
	}
	m_Parent->GetPlace()->NotifyGameEventToClient(player, EVENT_TYPE_CATAPULT_CONTROL_MSG, player->GetID(), m_Id);
	return RE_SUCCESS;
}

void WarCatapult::ClearBullet()
{
	WarCatapultBulletVector::iterator beg = m_CatapultBullet.begin();
	for (; beg != m_CatapultBullet.end(); ++beg)
	{
		if (*beg)
		{
			(*beg)->UnInit();
		}
		T_SAFE_DELETE(*beg);
	}
	m_CatapultBullet.clear();
}

void WarCatapult::ClearControl(bool clearbullet)
{
	if (clearbullet)
	{
		ClearBullet();
	}
	if (NULL == m_Parent || NULL == m_Parent->GetPlace())
	{
		return;
	}
	if (m_Operator != INVALID_ACTOR_ID)
	{
		m_Parent->ClearCatapultControlIndex(m_Operator);
	}
	m_Operator = INVALID_ACTOR_ID;
	LC_ServerNPC* thenpc = m_Parent->GetPlace()->GetActorManger().GetNpc(m_Id);
	if (thenpc != NULL)
	{
		thenpc->SetFollowedTargetID(INVALID_ACTOR_ID);
	}
}

void WarCatapult::SetControl(LC_ServerPlayer* player)
{
	if (NULL == player || NULL == m_Parent || NULL == m_Parent->GetPlace())
	{
		return;
	}
	m_Operator = player->GetID();
	m_Parent->AddCatapultControlIndex(m_Id, m_Operator);
	LC_ServerNPC* thenpc = m_Parent->GetPlace()->GetActorManger().GetNpc(m_Id);
	if (thenpc != NULL)
	{
		thenpc->SetFollowedTargetID(player->GetID());
	}
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------
LC_ServerMapLogic_WarPlay::LC_ServerMapLogic_WarPlay():
	m_Config(NULL), m_WarType(WarType_GuildWar), m_State(WarStateType_Peace), m_PowerTransId(0),
	m_PowerGuild(0), m_PowerPlayer(0), m_PowerTimeStamp(0),
	m_PowerNpcId(0), m_LogicArea(), m_RuneState(), m_RuneTime(0),
	m_CatapultMap(), m_CatapultPlayerMap(), m_CatapultIdBegin(0),
	m_KillMap(), m_SyncTimeStamp(), m_SyncEmenyTimeStamp()
{

}

LC_ServerMapLogic_WarPlay::~LC_ServerMapLogic_WarPlay()
{
	UnInit();
}

bool LC_ServerMapLogic_WarPlay::Init(LC_ServerMapLogic_Place* place, War_Config* config)
{
	if (place == NULL || config == NULL)
	{
		return false;
	}
	bool ret = LC_ServerMapLogic_PlayBase::Init(place, PlayType_War);
	if (!ret)
	{
		return false;
	}
	m_Config = config;
	m_WarType = WarType_GuildWar;

	//处理火令台
	LC_TranscriptionListEntry transEntry;
	transEntry.m_iMapID = place->GetMapLogicID();
	transEntry.m_iSpawnCharType = m_Config->Power.CharType;
	transEntry.m_kSpawnLocation = m_Config->Power.Pos;
	transEntry.m_iZRotAngle = m_Config->Power.Angel;
	LC_ServerNPC* npc = place->SpawnNPC(transEntry);
	if (npc == NULL)
	{
		return false;
	}
	m_PowerNpcId = npc->GetID();
	__setPowerNPCState(false);

	m_PowerTimeStamp = 0;
	m_PowerGuild = INVALID_GUILD_ID;
	m_PowerPlayer = INVALID_ACTOR_ID;
	m_PowerTransId = INVALID_GUILD_ID;

	m_LogicArea.clear();
	//逻辑区域
	LogicAreaSetType::iterator beg = m_Config->LogicArea.begin();
	for (int index = 0; beg != m_Config->LogicArea.end(); ++beg, ++index)
	{
		m_LogicArea[beg->LogicAreaId] = index;
	}

	m_RuneState = RuneState_Init;
	m_RuneTime = 0.0f;

	//攻城车
	m_CatapultIdBegin = 0;
	m_CatapultMap.clear();
	m_CatapultPlayerMap.clear();

	m_KillMap.clear();

	m_SyncTimeStamp = 0.0f;
	m_SyncEmenyTimeStamp = 0.0f;

	m_State = WarStateType_Peace;
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "war play init", place->GetMapResID(), m_PowerNpcId, m_Config->Power.CharType);
	return true;
}

void LC_ServerMapLogic_WarPlay::UnInit()
{
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "war play unit", GetPlace()?GetPlace()->GetMapResID():0, m_PowerNpcId);
	End();
	if (m_PowerNpcId > 0 && GetPlace())
	{
		LC_ServerNPC* npc = GetPlace()->GetActorManger().GetNpc(m_PowerNpcId);
		if (npc)
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "war play clear npc", npc->GetID(), npc->GetCharType(), npc->GetCurrentLocation().x, npc->GetCurrentLocation().y, npc->GetCurrentLocation().z) ;
			GetPlace()->ClearNPC(npc);
		}
	}
	m_PowerNpcId = 0;

	m_PowerGuild = INVALID_GUILD_ID;
	m_PowerPlayer = INVALID_ACTOR_ID;
	m_PowerTransId = INVALID_GUILD_ID;
	m_PowerTimeStamp = 0;

	m_LogicArea.clear();

	m_RuneState = RuneState_End;
	m_RuneTime = 0.0f;

	CatapultMap::iterator catapultbeg = m_CatapultMap.begin();
	for (; catapultbeg != m_CatapultMap.end(); ++catapultbeg)
	{
		if (catapultbeg->second)
		{
			catapultbeg->second->UnInit();
			T_SAFE_DELETE(catapultbeg->second);
		}
	}
	m_CatapultMap.clear();
	m_CatapultPlayerMap.clear();
	m_CatapultIdBegin = 0;

	m_KillMap.clear();

	m_SyncTimeStamp = 0.0f;
	m_SyncEmenyTimeStamp = 0.0f;

	m_State = WarStateType_Peace;
	m_Config = NULL;
	LC_ServerMapLogic_PlayBase::UnInit();
}

bool LC_ServerMapLogic_WarPlay::Start()
{
	if (m_State != WarStateType_Peace && m_State != WarStateType_End)
	{
		return false;
	}

	if (NULL == GetPlace())
	{
		return false;
	}

	m_PowerPlayer = INVALID_ACTOR_ID;
	m_PowerGuild = INVALID_GUILD_ID;
	m_PowerTimeStamp = 0;
	m_PowerTransId = 0;
	m_SyncTimeStamp = 0;
	m_SyncEmenyTimeStamp = 0;
	m_KillMap.clear();
	//攻城车
	m_CatapultIdBegin = 0;
	m_CatapultMap.clear();
	m_CatapultPlayerMap.clear();
	CatapultSetType::iterator Catapultbeg = m_Config->Catapults.begin();
	for (; Catapultbeg != m_Config->Catapults.end(); ++Catapultbeg)
	{
		WarCatapult* catapult = T_NEW_D WarCatapult;
		Utility::UT_Vec3Int thepos(Catapultbeg->Pos.x, Catapultbeg->Pos.y, Catapultbeg->Pos.z);
		bool ret = catapult->Init(this, Catapultbeg->CharType, thepos, Catapultbeg->Radius, Catapultbeg->CruiseTime, Catapultbeg->WarnTime, Catapultbeg->Angel, GetPlace()->GetMapResID(), GetPlace()->GetMapLogicID(), Catapultbeg->VaildDistance);
		if (ret)
		{
			m_CatapultMap[catapult->m_Id] = catapult;
		}
	}
	SetState(WarStateType_Ownerless);

	m_RuneState = RuneState_Prepare;
	m_RuneTime = 0;

	//SyncPowerGuildInfo();

	if (m_WarType == WarType_DomainWar)
	{
		GetPlace()->SetRecordPK(false);
	}

	GameLogic::LC_ActorPtrList allplayer;
	GetPlace()->GetAllRealPlayer(allplayer);
	GameLogic::LC_ActorPtrList::iterator beg = allplayer.begin();
	for (; beg != allplayer.end(); ++beg)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)(*beg);
		PlayerEnterWar(pkServerPlayer);
	}
	GfxWriteFmtLog(LOG_GUILD_FILELINE, LOG_SWITCH_GUILD, "war play start", m_State, m_PowerPlayer, m_PowerGuild, m_PowerNpcId);
	return true;
}

void LC_ServerMapLogic_WarPlay::End()
{
	if (!CheckRun())
	{
		return;
	}
	if (NULL == GetPlace())
	{
		return;
	}
	//清除所有逻辑
	OwnerlessFinish();//盟主结束
	//清理攻城车
	CatapultMap::iterator catapultbeg = m_CatapultMap.begin();
	for (; catapultbeg != m_CatapultMap.end(); ++catapultbeg)
	{
		if (catapultbeg->second)
		{
			catapultbeg->second->UnInit();
			T_SAFE_DELETE(catapultbeg->second);
		}
	}
	m_CatapultMap.clear();
	m_CatapultPlayerMap.clear();
	m_CatapultIdBegin = 0;
	//符文结束
	m_RuneState = RuneState_End;
	m_RuneTime = 0;

	SetState(WarStateType_End);
	SyncPowerGuildInfo();
	LC_ActorPtrList allplayer;
	GetPlace()->GetAllRealPlayer(allplayer);
	LC_ActorPtrList::iterator beg = allplayer.begin();
	for (; beg != allplayer.end(); ++beg)
	{
		PlayerLeaveWar((LC_ServerPlayer*)(*beg));
	}
	Settle();
	if (m_WarType == WarType_DomainWar)
	{
		GetPlace()->SetRecordPK(true);
	}
	GfxWriteFmtLog(LOG_GUILD_FILELINE, LOG_SWITCH_GUILD, "war play end", m_State, m_PowerPlayer, m_PowerGuild, m_PowerNpcId);
}

bool LC_ServerMapLogic_WarPlay::Update(float fCurrentTime, float fDeltaTime)
{
	bool ret = UpdateRune(fCurrentTime, fDeltaTime);
	if (!ret)
	{
		return false;
	}
	//处理20分钟逻辑
	UpdateOwnerless(fCurrentTime, fDeltaTime);
	return true;
}

Utility::UT_Vec3Int	LC_ServerMapLogic_WarPlay::ComputeEnterLocation(LC_ServerPlayer* player, const Utility::UT_Vec3Int& targetlocation)
{
	if (!CheckRun())
	{
		return Utility::UT_Vec3Int();
	}
	if (targetlocation.x != 0 || targetlocation.y != 0)
	{
		return targetlocation;
	}
	Utility::UT_Vec3Int pos;
	if (isinGuild(player, m_PowerTransId))
	{
		return m_Config->PowerRespawn.Pos;
	}
	else
	{
		return m_Config->ChallengeRespawn.Pos;
	}
}

void LC_ServerMapLogic_WarPlay::EventNotify(MapLogicEvent& kEvent)
{
	if (m_WarType==WarType_CrossRealmGuildWar && (kEvent.Type() == MapLogicNotifyEvent_Org || kEvent.Type()==MapLogicNotifyEvent_MonsterCount))
	{
		GfxWriteFmtLog(LOG_GUILD_FILELINE, LOG_SWITCH_GUILD, "war play event", kEvent.Type(), kEvent.Param1(), kEvent.Param2(), kEvent.Param3(), kEvent.Param4(), kEvent.Param5());
	}
}

void LC_ServerMapLogic_WarPlay::PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent)
{
	if (NULL == player || NULL == GetPlace() || !CheckRun())
	{
		return;
	}
	if (kEvent.Type() == PlayerEvent_Enter)
	{
		PlayerEnterWar(player);
	}
	else if (kEvent.Type() == PlayerEvent_OffLine)
	{
		if (CheckPowerOwnerless() == RE_SUCCESS)
		{
			if (GetPowerGuild() != INVALID_GUILD_ID && GetPowerPlayer() != INVALID_ACTOR_ID && GetPowerPlayer() == (int)player->GetID())
			{
				__setPowerGuild(INVALID_GUILD_ID, INVALID_ACTOR_ID);
			}
		}
		WarCatapult* catapult = GetPlayerControlCatapult(player->GetID());
		if (catapult != NULL)
		{
			catapult->ClearControl();
		}
	}
	else if (kEvent.Type() == PlayerEvent_Recover)
	{
		SyncPowerGuildInfo(player->GetID());
	}
	else if (kEvent.Type() == PlayerEvent_Leave)
	{
		PlayerLeaveWar(player);
		if (CheckPowerOwnerless() == RE_SUCCESS)
		{
			if (GetPowerGuild() != INVALID_GUILD_ID && GetPowerPlayer() != INVALID_ACTOR_ID && GetPowerPlayer() == (int)player->GetID())
			{
				__setPowerGuild(INVALID_GUILD_ID, INVALID_ACTOR_ID);
			}
		}
		WarCatapult* catapult = GetPlayerControlCatapult(player->GetID());
		if (catapult != NULL)
		{
			catapult->ClearControl();
		}
	}
	else if (kEvent.Type() == PlayerEvent_ChangeLogicArea)
	{
		int prearea = kEvent.Param(0);
		int curarea = kEvent.Param(1);
		int preAreaType = kEvent.Param(2);
		int curAreaType = kEvent.Param(3);
		int prebuffid = 0;
		int currentbuffid = 0;
		int multiple = 0;
		LogicAreaMap::iterator it = m_LogicArea.find(prearea);
		if (it != m_LogicArea.end())
		{
			prebuffid = m_Config->LogicArea[it->second].BuffId;
		}
		it = m_LogicArea.find(curarea);
		if (it != m_LogicArea.end())
		{
			currentbuffid = m_Config->LogicArea[it->second].BuffId;
			multiple = m_Config->LogicArea[it->second].Multiple;
		}

		if (prebuffid != 0)
		{
			int buddyID = player->GetBuddyID();
			player->GetStateMap()->RemoveSkillStateInStateMap(prebuffid);
		}

		if (currentbuffid != 0)
		{
			player->GetStateMap()->AddSkillStateToStateMap(currentbuffid, 0, 0, (float)multiple);
		}

		player->GetGuildWarAssert()->SetMultiple(multiple);

		//如果区域发生变化，而且玩家当前处于安全区了
		if (preAreaType != curAreaType && curAreaType == 0)
		{
			if ( m_WarType == WarType_GuildWar )
			{
				player->DeactiveSkillStateToAllBuddyInstance(GuildWarTmpStateID);
			}
			else if ( m_WarType ==WarType_CrossRealmGuildWar ) 
			{
				player->DeactiveSkillStateToAllBuddyInstance(GuildWarTmpStateID_T);
			}
			else if ( m_WarType == WarType_DomainWar )
			{
				player->DeactiveSkillStateToAllBuddyInstance(DomainWarTmpState_ID);
			}
			player->SyncControllerSkillStatesToClient(SSC_ALL);
			
			if (CheckPowerOwnerless() == RE_SUCCESS)
			{
				if (GetPowerGuild() != INVALID_GUILD_ID && GetPowerPlayer() != INVALID_ACTOR_ID && GetPowerPlayer() == (int)player->GetID())
				{
					__setPowerGuild(INVALID_GUILD_ID, INVALID_ACTOR_ID);
				}
			}
		}
	}
	else if (kEvent.Type() == PlayerEvent_Death)
	{
		if (CheckPowerOwnerless() == RE_SUCCESS)
		{
			if (GetPowerGuild() != INVALID_GUILD_ID && GetPowerPlayer() != INVALID_ACTOR_ID && GetPowerPlayer() == (int)player->GetID())
			{
				__setPowerGuild(INVALID_GUILD_ID, INVALID_ACTOR_ID);
			}
			WarCatapult* catapult = GetPlayerControlCatapult(player->GetID());
			if (catapult != NULL)
			{
				catapult->ClearControl();
			}
		}
		do
		{
			//是否是守方
			if (isinPowerGuild(player))
			{
				LogicAreaSet::iterator it = std::find(m_Config->PowerRespawn.Punish.begin(), m_Config->PowerRespawn.Punish.end(), player->GetCurrentLogicAreaID());
				if (it == m_Config->PowerRespawn.Punish.end())
				{
					break;
				}
			}
			else
			{
				LogicAreaSet::iterator it = std::find(m_Config->ChallengeRespawn.Punish.begin(), m_Config->ChallengeRespawn.Punish.end(), player->GetCurrentLogicAreaID());
				if (it == m_Config->ChallengeRespawn.Punish.end())
				{
					break;
				}
			}
			PlayerRespawnData* the_data = GetPlace()->GetPlayerRespawn(player->GetID());
			if (the_data == NULL)
			{
				break;
			}
			if (the_data->m_DeadTime <= 1)
			{
				break;
			}
			the_data->m_SafeRespawnTime += 3;
			if (the_data->m_SafeRespawnTime > 30)
			{
				the_data->m_SafeRespawnTime = 30;
			}
			the_data->m_CurRespawnTime += 3;
			if (the_data->m_CurRespawnTime > 40)
			{
				the_data->m_CurRespawnTime = 40;
			}
		}
		while (0);
		//复活惩罚
		int lSrcActorID = kEvent.Param(0);
		if (!IS_PLAYER_ID(lSrcActorID))
		{
			return;
		}
		if (!GetPlace()->CheckPlayerIn(lSrcActorID))
		{
			return;
		}
		LC_ServerPlayer* SrcPlayer = GetPlace()->GetActorManger().GetPlayer(lSrcActorID);
		if (SrcPlayer == NULL)
		{
			return;
		}

		if ((SrcPlayer->GetGuildID() == INVALID_GUILD_ID || player->GetGuildID() == INVALID_GUILD_ID || SrcPlayer->GetGuildID() != player->GetGuildID()) && player->GetLevel() >= (SrcPlayer->GetLevel() - 5))
		{
			KillMap::iterator it = m_KillMap.find(SrcPlayer->GetID());
			if (it == m_KillMap.end())
			{
				std::pair<KillMap::iterator, bool> ret = m_KillMap.insert(std::make_pair(SrcPlayer->GetID(), mem::map<object_id_type, int>()));
				it = ret.first;
			}
			mem::map<object_id_type, int>::iterator playit = it->second.find(player->GetID());
			if (playit == it->second.end())
			{
				std::pair<mem::map<object_id_type, int>::iterator, bool> ret = it->second.insert(std::make_pair(player->GetID(), 0));
				playit = ret.first;
			}
			if (playit->second >= 3)
			{
				return;
			}
			++playit->second;

			MoneyType guildMoney = 0;
			attr_value_type exp = 0;
			MoneyType money = 0;
			attr_value_type sp = 0;
			if (m_WarType == WarType_GuildWar || m_WarType == WarType_DomainWar)
			{
				guildMoney = 10;
				exp = (SrcPlayer->GetNoPeakLevel() * 5 + 20) * 180;
				money = (MoneyType)(((float)(SrcPlayer->GetNoPeakLevel() * 5 + 20)) * 180 * 0.075);
				sp = 240;
			}
			else if (m_WarType == WarType_CrossRealmGuildWar)
			{
				guildMoney = 20;
				exp = (SrcPlayer->GetNoPeakLevel() * 5 + 20) * 360;
				money = (MoneyType)(((float)(SrcPlayer->GetNoPeakLevel() * 5 + 20)) * 360 * 0.075);
				sp = 480;
			}

			SrcPlayer->AddCash(money, CT_UNBIND_CASH, SYS_FUNC_TP_GUILD_WAR, m_WarType);
			SrcPlayer->AddExpWithGamePlay(exp, SYS_FUNC_TP_GUILD_WAR, m_WarType);
			SrcPlayer->AddSkillLevelupPoints(sp, SYS_FUNC_TP_GUILD_WAR, m_WarType);
			if (SrcPlayer->GetGuildID() != INVALID_GUILD_ID)
			{
				SrcPlayer->AddGuildMoney(guildMoney, SYS_FUNC_TP_GUILD_WAR, m_WarType);
				LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
				LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(SrcPlayer->GetGuildID());
				if (guild != NULL)
				{
					guild->UpdateMemberInfo(SrcPlayer);
				}
			}
		}
	}
}

bool LC_ServerMapLogic_WarPlay::CheckEnd()
{
	return m_State == WarStateType_End;
}

bool LC_ServerMapLogic_WarPlay::CheckRun()
{
	if (m_State == WarStateType_Ownerless || m_State == WarStateType_Owner)
	{
		return true;
	}
	return false;
}

void LC_ServerMapLogic_WarPlay::Settle()
{
	if (NULL == GetPlace())
	{
		return;
	}
	//计算所有超过15分钟玩家的经验
	MG_Notify_WarSettle settlemsg;
	settlemsg.m_WarType = GetWarType();
	int totalsecond = 0;//30*ONE_MINUTE_SECONDS;
	LC_ActorPtrList allplayer;
	GetPlace()->GetAllRealPlayer(allplayer);
	LC_ActorPtrList::iterator beg = allplayer.begin();
	for (; beg != allplayer.end(); ++beg)
	{
		LC_ServerPlayer* player = (LC_ServerPlayer*)(*beg);
		if (player != NULL)
		{
			player->GetGuildWarAssert()->ComputeLiveTime();
			if (player->GetGuildWarAssert()->GetLiveTime() >= totalsecond)
			{
				settlemsg.m_result = true;
				if (player->GetGuildID() == INVALID_GUILD_ID || GetPowerGuild() == INVALID_GUILD_ID || player->GetGuildID() != GetPowerGuild())
				{
					settlemsg.m_result = false;
				}
				int nWin = settlemsg.m_result ? 1 : 0;
				Utility::UT_SIMDataList kItems;
				StringType sMailTitle;
				StringType sMailDesc;
				CF_GuildReward::DataEntryMapExternIterator iter = CF_GuildReward::Instance()->GetIterator();
				while(iter.HasMoreElements())
				{
					CF_GuildReward::DataEntry* entry = iter.PeekNextValuePtr();
					if(/*entry != NULL &&*/ entry->_iType == m_WarType && entry->_iWin == nWin)
					{
						settlemsg.m_guildmoney = entry->_GuildMoney;
						settlemsg.m_exp = SA_RT_LUA_FUNC(entry->_iExp, player, nWin);
						settlemsg.m_money = SA_RT_LUA_FUNC(entry->_iCash, player, nWin);
						settlemsg.m_sp = entry->_iSp;
						TRANSFER_CONTAINER_DATAS(Utility::UT_SIMDataList, entry->_kItem, kItems, push_back);
						sMailTitle = entry->_sMailTitle;
						sMailDesc = entry->_sMailDesc;
						break;
					}
					iter.MoveNext();
				}

				if(settlemsg.m_money > 0)
				{
					player->AddCash(settlemsg.m_money, CT_UNBIND_CASH, SYS_FUNC_TP_GUILD_WAR, m_WarType);
				}
				if(settlemsg.m_exp > 0)
				{
					player->AddExpWithGamePlay(settlemsg.m_exp, SYS_FUNC_TP_GUILD_WAR, m_WarType);
				}
				if(settlemsg.m_sp > 0)
				{
					player->AddSkillLevelupPoints(settlemsg.m_sp, SYS_FUNC_TP_GUILD_WAR, m_WarType);
				}
				if(!kItems.empty())
				{
					if (RE_SUCCESS == UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, player, SYS_FUNC_TP_GUILD_WAR, m_WarType, sMailTitle, sMailDesc))
					{
						Utility::UT_SIMDataList::iterator iIter = kItems.begin();
						for (; iIter != kItems.end(); ++iIter)
						{
							settlemsg.m_itemMap[iIter->ID()] += iIter->ItemCount();
						}
					}
				}
				if (player->GetGuildID() != INVALID_GUILD_ID && settlemsg.m_guildmoney > 0)
				{
					player->AddGuildMoney(settlemsg.m_guildmoney, SYS_FUNC_TP_GUILD_WAR, m_WarType);
					LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
					LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(player->GetGuildID());
					if (guild != NULL)
					{
						guild->UpdateMemberInfo(player);
					}
				}
				player->SendMsgToClient(MGPT_NOTIFY_WAR_SETTLE, &settlemsg);
			}
		}
	}
}

bool LC_ServerMapLogic_WarPlay::UpdateOwnerless(float fCurrentTime, float fDeltaTime)
{
	if (CheckPowerOwnerless() != RE_SUCCESS)
	{
		return true;
	}
	//攻城车
	CatapultMap::iterator beg = m_CatapultMap.begin();
	for (; beg != m_CatapultMap.end(); ++beg)
	{
		if (beg->second)
		{
			beg->second->Update(fCurrentTime, fDeltaTime);
		}
	}
	//发送圣火令位置
	if (GetPowerGuild() != INVALID_GUILD_ID && GetPowerPlayer() != INVALID_ACTOR_ID)
	{
		if (fCurrentTime >= m_PowerTimeStamp)
		{
			OwnerlessFinish();
		}
		else
		{
			SyncPowerPlayerInfo();
		}
	}
	return true;
}


void LC_ServerMapLogic_WarPlay::OwnerlessFinish()//盟主产生
{
	if (CheckPowerOwnerless() != RE_SUCCESS)
	{
		return;
	}
	//通知盟主产生
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	LC_ServerPlayer* theplayer = NULL;
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace != NULL)
	{
		unique_id_vector idList;
		pPlace->GetAllRealPlayer(idList);
		unique_id_impl uiPowerId = 0;
		theplayer = pPlace->GetActorManger().GetPlayer(m_PowerPlayer);
		if (theplayer != NULL)
		{
			uiPowerId = theplayer->GetInstance();
		}
		manager->NotifyPowerEnd(GetPowerGuild(), m_WarType, pPlace->GetMapLogicID(), idList, uiPowerId);
	}
	SetState(WarStateType_Owner);

	SyncPowerGuildInfo();
	//停止盟主产生的逻辑
	ClearAllCatapultControl();
	//去除BUFF
	if (GetPowerGuild() != INVALID_GUILD_ID && GetPowerPlayer() != INVALID_ACTOR_ID)
	{
		if (pPlace != NULL)
		{
			if (theplayer == NULL)
			{
				theplayer = pPlace->GetActorManger().GetPlayer(m_PowerPlayer);
			}
			if (theplayer != NULL)
			{
				theplayer->DeactiveSkillStateToAllBuddyInstance(GuildWarTmpStateID);
				theplayer->DeactiveSkillStateToAllBuddyInstance(GuildWarTmpStateID_T);
				theplayer->DeactiveSkillStateToAllBuddyInstance(DomainWarTmpState_ID);
				theplayer->SyncControllerSkillStatesToClient(SSC_ALL);
			}
		}
	}
	//重置王座
	__setPowerNPCState(false);
}

bool LC_ServerMapLogic_WarPlay::UpdateRune(float fCurrentTime, float fDeltaTime)
{
	if (m_RuneState == RuneState_Prepare)
	{
		if (fCurrentTime >= m_RuneTime)
		{
			m_RuneState = RuneState_Normal;
			GenRune();
			m_RuneTime = fCurrentTime + m_Config->Rune.IntervalTime;
		}
	}
	else if (m_RuneState == RuneState_Normal)
	{
		if (fCurrentTime >= m_RuneTime)
		{
			GenRune();
			m_RuneTime = fCurrentTime + m_Config->Rune.IntervalTime;
		}
	}
	return true;
}

void LC_ServerMapLogic_WarPlay::GenRune()
{
	if (m_Config == NULL || NULL == GetPlace())
	{
		return;
	}
	int runesize = (int)m_Config->Rune.RuneList.size();
	PositionSetType temp;
	temp.insert(temp.begin(), m_Config->Rune.PosGroup.begin(), m_Config->Rune.PosGroup.end());
	std::random_shuffle(temp.begin(), temp.end());
	int cursize = (int)temp.size();
	int dis = runesize - cursize;
	for (int i = 0; i < dis; ++i)
	{
		temp.push_back(temp[i % cursize]);
	}
	Utility::UT_Vec3Int thepos;
	int index = 0;
	RuneSetType::iterator beg = m_Config->Rune.RuneList.begin();
	for (; beg != m_Config->Rune.RuneList.end(); ++beg, ++index)
	{
		thepos.x = temp[index].x;
		thepos.y = temp[index].y;
		thepos.z = temp[index].z;
		int allweight = 0;
		mem::vector<RuneEntry>::iterator runebeg = beg->begin();
		for (; runebeg != beg->end(); ++runebeg)
		{
			allweight += runebeg->Rate;
		}
		if (allweight > 0)
		{
			int weight = UT_MathBase::RandInRangeInt(1, allweight);
			int curweight = 0;
			runebeg = beg->begin();
			for (; runebeg != beg->end(); ++runebeg)
			{
				curweight += runebeg->Rate;
				if (weight <= curweight)
				{
					break;
				}
			}
			if (runebeg == beg->end())
			{
				runebeg = beg->end() - 1;
			}

			UT_ServerHelper::AddPlaceHolderEffect(GetPlace()->GetMapLogicID(), runebeg->RuneId, (int)m_Config->Rune.LiveTime, thepos);
		}
	}
	//发出公告
	if (!m_Config->Rune.NotifyMsg.empty())
	{
		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		bool res = UT_ServerHelper::GetGameNoticeFromString(STDSTR2TPSTR(m_Config->Rune.NotifyMsg), noticeFmort);
		if(res)
		{
			noticeFmort.SetChannelType(PROMPT_CHANNEL_ACTIVITY_TIP);
			MG_GameNotice noticeMsg;
			UT_ServerHelper::FillGameNoticeMsg(noticeFmort, noticeMsg);
			GetPlace()->BroadcastMsgToClient(MGPT_NOTIFY_MSG, &noticeMsg);
		}
	}
}

void LC_ServerMapLogic_WarPlay::PlayerEnterWar(LC_ServerPlayer* player)
{
	if (NULL == player)
	{
		return;
	}

	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	int64_t begtime = 0;
	int64_t endtime = 0;
	manager->GetWarTimeStamp(m_WarType, begtime, endtime);
	player->GetGuildWarAssert()->SetTimeStamp(begtime);
	player->GetGuildWarAssert()->SetStateBegin();
	int currentbuffid = 0;
	int multiple = 0;
	LogicAreaMap::iterator it = m_LogicArea.find(player->GetCurrentLogicAreaID());
	if (it != m_LogicArea.end())
	{
		currentbuffid = m_Config->LogicArea[it->second].BuffId;
		multiple = m_Config->LogicArea[it->second].Multiple;
	}

	if (currentbuffid != 0 && player->GetGuildWarAssert()->GetMultiple() != multiple)
	{
		player->ActiveSkillState(currentbuffid, 0, 0, (float)multiple);
	}
	player->GetGuildWarAssert()->SetMultiple(multiple);
	//势力争夺战并不切换攻击模式
	if ( GetWarType() == WarType_GuildWar )
	{
		player->GetPKRecorder()->SetPKMode(PKM_SOCIETY);
	}
	else if ( GetWarType() == WarType_CrossRealmGuildWar)
	{
		player->GetPKRecorder()->SetPKMode(PKM_SERVER);
	}
	
	//player->DeactiveSkillState(31005001);
	SyncPowerGuildInfo(player->GetID());
}

void LC_ServerMapLogic_WarPlay::PlayerLeaveWar(LC_ServerPlayer* player)
{
	player->GetGuildWarAssert()->SetStateEnd();
	//势力争夺战并不切换攻击模式
	if (GetWarType() != WarType_DomainWar)
	{
		player->GetPKRecorder()->SetPKMode(PKM_PEACE);
	}
	//清除buff
	LogicAreaMap::iterator beg = m_LogicArea.begin();
	for (; beg != m_LogicArea.end(); ++beg)
	{
		player->DeactiveSkillState(m_Config->LogicArea[beg->second].BuffId);
	}
}

ResultType LC_ServerMapLogic_WarPlay::CheckPowerOwnerless()
{
	if (!CheckRun())
	{
		return RE_GUILDWAR_END;
	}
	if (m_State == WarStateType_Owner)
	{
		return RE_GUILDWAR_OWNER;
	}
	return RE_SUCCESS;
}

bool LC_ServerMapLogic_WarPlay::isinPowerGuild(LC_ServerPlayer* player)
{
	return isinGuild(player, GetPowerGuild());
}

bool LC_ServerMapLogic_WarPlay::isinGuild(LC_ServerPlayer* player, uint64_t guildid)
{
	if (guildid == INVALID_GUILD_ID)
	{
		return false;
	}
	uint64_t playerguildid = player->GetGuildID();
	if (playerguildid == INVALID_GUILD_ID)
	{
		return false;
	}
	return guildid == playerguildid;
}

bool LC_ServerMapLogic_WarPlay::isPowerGuild(int guildid)
{
	if (GetPowerGuild() == INVALID_GUILD_ID)
	{
		return false;
	}
	return GetPowerGuild() == guildid;
}

bool LC_ServerMapLogic_WarPlay::isChallengeGuild(int guildid)
{
	if (isPowerGuild(guildid))
	{
		return false;
	}
	return true;
}

WarCatapult* LC_ServerMapLogic_WarPlay::GetCatapult(int catapultId)
{
	CatapultMap::iterator it = m_CatapultMap.find(catapultId);
	if (it == m_CatapultMap.end())
	{
		return NULL;
	}
	return it->second;
}

void LC_ServerMapLogic_WarPlay::AddCatapultControlIndex(int Catapultid, object_id_type playerid)
{
	m_CatapultPlayerMap[playerid] = Catapultid;
}

void LC_ServerMapLogic_WarPlay::ClearCatapultControlIndex(object_id_type playerid)
{
	mem::map<int, int>::iterator it = m_CatapultPlayerMap.find(playerid);
	if (it != m_CatapultPlayerMap.end())
	{
		m_CatapultPlayerMap.erase(it);
	}
}

void LC_ServerMapLogic_WarPlay::ClearAllCatapultControl()
{
	CatapultMap::iterator beg = m_CatapultMap.begin();
	for (; beg != m_CatapultMap.end(); ++beg)
	{
		if (beg->second)
		{
			beg->second->ClearControl();
		}
	}
}

WarCatapult* LC_ServerMapLogic_WarPlay::GetPlayerControlCatapult(object_id_type playerid)
{
	mem::map<int, int>::iterator it = m_CatapultPlayerMap.find(playerid);
	if (it == m_CatapultPlayerMap.end())
	{
		return NULL;
	}
	return GetCatapult(it->second);
}

ResultType LC_ServerMapLogic_WarPlay::CheckGivenPower(GameLogic::LC_PlayerBase* pkPlayer, bool checkmoney)
{
	LC_ServerPlayer* player = ((LC_ServerPlayer*)pkPlayer)->GetOwnerPlayer();
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (player == NULL || NULL == pPlace)
	{
		return RE_FAIL;
	}
	//检查是否在场景中
	if (!pPlace->CheckPlayerIn(player->GetID()))
	{
		return RE_FAIL;
	}
	if (pkPlayer->GetDead())
	{
		return RE_FAIL;
	}
	ResultType ret = CheckPowerOwnerless();
	if(ret == RE_GUILDWAR_END)
	{
		return ret;
	}

	if (ret == RE_GUILDWAR_OWNER)
	{
		if (m_WarType == WarType_DomainWar)
		{
			ret = RE_GUILDWAR_DOMAINWAR_OWNER;
		}
		else if (m_WarType == WarType_GuildWar)
		{
			ret = RE_GUILDWAR_NORMAL_OWNER;
		}
		else if (m_WarType == WarType_CrossRealmGuildWar)
		{
			ret = RE_GUILDWAR_CROSSREAL_OWNER;
		}
		pPlace->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, ret);
		return ret;
	}
	//查看是否已经拔起
	if (GetPowerGuild() != INVALID_GUILD_ID)
	{
		pPlace->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, RE_GUILDWAR_POWER_GIVENED);
		return RE_GUILDWAR_POWER_GIVENED;
	}
	//检查是否是会长
	ResultType nRel = RE_SUCCESS;
	static LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	Protocol::PS_PowerGuildInfo* localpower = pkGuildManager->GetPlayerLocalPowerGuildInfo(player);
	LC_ServerGuildInfo* guildinfo = GetPlayerGuild(player);
	if (NULL == guildinfo)
	{//无公会
		return RE_GUILD_NONE;
	}

	if((m_WarType != WarType_CrossRealmGuildWar || NULL==localpower || guildinfo->GetId()==localpower->m_GuildId))
	{
		switch(m_WarType)
		{
		case WarType_DomainWar:
			{
				if(guildinfo->IsMaster(player->GetInstance()) == false &&
					guildinfo->IsViceMaster(player->GetInstance()) == false)
				{
					nRel = RE_GUILDWAR_MUST_MASTER;
				}
			}
			break;
		case WarType_GuildWar:
		case WarType_CrossRealmGuildWar:
			{
				if(guildinfo->IsMaster(player->GetInstance()) == false &&
					guildinfo->IsViceMaster(player->GetInstance()) == false &&
					guildinfo->IsElder(player->GetInstance()) == false)
				{
					nRel = RE_GUILDWAR_MUST_MASTER;
				}
			}
			break;
		default:
			break;
		}
	}
	else
	{
		nRel = RE_GUILDWAR_MUST_MASTER;
	}
	if(nRel != RE_SUCCESS)
	{
		pPlace->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, nRel);
		return nRel;
	}
	//30级才能拔旗子
	if (player->GetLevel() < 30)
	{
		pPlace->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, RE_GUILDWAR_LEVEL_LIMITED);
		return RE_GUILDWAR_LEVEL_LIMITED;
	}
	if (checkmoney)
	{
		//检查钱
		//金币
		mem::vector<PS_CostEntry> entry;
		PS_CostEntry temp;
		temp.m_ItemType = 0;
		temp.m_ItemCount = m_Config->Power.Cost;
		entry.push_back(temp);
		ret = guildinfo->CheckCost(entry);
		if (ret != RE_SUCCESS)
		{
			pPlace->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, ret, (int)temp.m_ItemCount);
			return ret;
		}
	}
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_WarPlay::PreGivenPower(GameLogic::LC_PlayerBase* PowerPlayer)
{
	LC_ServerPlayer* player = (LC_ServerPlayer*)PowerPlayer;
	if (player == NULL || NULL == GetPlace())
	{
		return RE_FAIL;
	}
	ResultType ret = CheckGivenPower(PowerPlayer);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}
	LC_ServerGuildInfo* guildinfo = GetPlayerGuild((LC_ServerPlayer*)PowerPlayer);
	if (guildinfo == NULL)
	{
		return RE_FAIL;
	}
	//扣钱
	mem::map<int, int64_t> cost;
	cost.insert(std::make_pair(0, m_Config->Power.Cost));
	ret = guildinfo->HandleAssert((LC_ServerPlayer*)PowerPlayer, cost, true, false, GuildItemCost_WarPower, 0, 0);
	if (ret != RE_SUCCESS)
	{
		GetPlace()->NotifyGameEventToClient(player, EVENT_TYPE_POWER_GUILD_MSG, ret);
	}
	return ret;
}

ResultType LC_ServerMapLogic_WarPlay::GivenPower(GameLogic::LC_PlayerBase* PowerPlayer, ResultType result, uint64_t guildid, int64_t transid)
{
	if (NULL == PowerPlayer || NULL == GetPlace())
	{
		return RE_FAIL;
	}

	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (result != RE_SUCCESS)
	{
		//发送消息
		GetPlace()->NotifyGameEventToClient((LC_ServerPlayer*)PowerPlayer, EVENT_TYPE_POWER_GUILD_MSG, result);
		return result;
	}
	ResultType ret = CheckGivenPower(PowerPlayer, false);
	if (ret != RE_SUCCESS)
	{
		pkGuildManager->ConfirmGuildAssertTransaction(guildid, transid, true);
		return ret;
	}
	if (guildid != PowerPlayer->GetGuildID())
	{
		pkGuildManager->ConfirmGuildAssertTransaction(guildid, transid, true);
		return RE_FAIL;
	}
	__setPowerGuild(guildid, PowerPlayer->GetID());
	pkGuildManager->ConfirmGuildAssertTransaction(guildid, transid, false);
	if (m_WarType == WarType_GuildWar)
	{
		PowerPlayer->ActiveSkillStateToAllBuddyInstance(GuildWarTmpStateID, 0, 0);
	}
	else if (m_WarType == WarType_CrossRealmGuildWar)
	{
		PowerPlayer->ActiveSkillStateToAllBuddyInstance(GuildWarTmpStateID_T, 0, 0);
	}
	else if (m_WarType == WarType_DomainWar)
	{
		PowerPlayer->ActiveSkillStateToAllBuddyInstance(DomainWarTmpState_ID, 0, 0);
	}
	LC_ServerPlayer* pServerPlayer = (LC_ServerPlayer*)PowerPlayer;
	pServerPlayer->SyncControllerSkillStatesToClient(SSC_ALL);
	return RE_SUCCESS;
}

void LC_ServerMapLogic_WarPlay::GuildCatapultCostResult(LC_ServerPlayer* player, ResultType result, uint64_t guildid, int64_t transid, int p1, int p2, int p3)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	WarCatapult* catapult = GetCatapult(p2);
	if (catapult == NULL)
	{
		if (result == RE_SUCCESS)
		{
			pkGuildManager->ConfirmGuildAssertTransaction(guildid, transid, true);
		}
		return;
	}
	bool ret = catapult->FireResult(player, result, p3);
	if (ret)
	{
		if (result == RE_SUCCESS)
		{
			pkGuildManager->ConfirmGuildAssertTransaction(guildid, transid, false);
		}
	}
	else
	{
		if (result == RE_SUCCESS)
		{
			pkGuildManager->ConfirmGuildAssertTransaction(guildid, transid, true);
		}
	}
}

uint64_t LC_ServerMapLogic_WarPlay::GetPowerGuild()
{
	return m_PowerGuild;
}

int LC_ServerMapLogic_WarPlay::GetPowerPlayer()
{
	return m_PowerPlayer;
}

void LC_ServerMapLogic_WarPlay::SetState(WarStateType state)
{
	m_State = state;
}

LC_ServerGuildInfo* LC_ServerMapLogic_WarPlay::GetPlayerGuild(LC_ServerPlayer* player)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return NULL;
	}
	uint64_t guildid = player->GetGuildID();
	if (guildid == INVALID_GUILD_ID)
	{
		return NULL;
	}
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetGuildInfoByID(guildid);
	if (guildinfo == NULL)
	{
		return NULL;
	}
	return guildinfo;
}

LC_ServerGuildInfo* LC_ServerMapLogic_WarPlay::GetGuild(uint64_t guildid)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return NULL;
	}
	if (guildid == INVALID_GUILD_ID)
	{
		return NULL;
	}
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetGuildInfoByID(guildid);
	if (guildinfo == NULL)
	{
		return NULL;
	}
	return guildinfo;
}

void LC_ServerMapLogic_WarPlay::SyncPowerPlayerInfo(bool force, object_id_type playerid)
{
	if (NULL == GetPlace())
	{
		return;
	}
	float curtime = GET_PROCESS_TIME;
	if (!force)
	{
		if (curtime < m_SyncTimeStamp)
		{
			return;
		}
	}
	if (CheckPowerOwnerless() != RE_SUCCESS)
	{
		return;
	}
	if (GetPowerPlayer() == INVALID_ACTOR_ID)
	{
		return;
	}
	m_SyncTimeStamp = curtime + SYNC_POWERPLAYER_POS;
	MG_WarPlayerInfo info;
	FillGuildWarPlayerInfo(info);
	if (playerid == INVALID_ACTOR_ID)
	{
		GetPlace()->BroadcastMsgToClient(MGPT_NOTIFY_GUILDPLAYER_INFO, &info);
	}
	else
	{
		LC_ServerPlayer* player = GetPlace()->GetActorManger().GetPlayer(playerid);
		if (player != NULL)
		{
			player->SendMsgToClient(MGPT_NOTIFY_GUILDPLAYER_INFO, &info);
		}
	}
}

void LC_ServerMapLogic_WarPlay::SyncPowerGuildInfo(object_id_type playerid)
{
	if (NULL == GetPlace())
	{
		return;
	}
	MG_WarInfo info;
	FillGuildWarInfo(info);
	if (playerid == INVALID_ACTOR_ID)
	{
		float curtime = GET_PROCESS_TIME;
		m_SyncTimeStamp = curtime + SYNC_POWERPLAYER_POS;
		GetPlace()->BroadcastMsgToClient(MGPT_NOTIFY_POWERGUILD_INFO, &info);
	}
	else
	{
		LC_ServerPlayer* player = GetPlace()->GetActorManger().GetPlayer(playerid);
		if (player != NULL)
		{
			player->SendMsgToClient(MGPT_NOTIFY_POWERGUILD_INFO, &info);
		}
	}
}

void LC_ServerMapLogic_WarPlay::SyncAllEnemyPlayerInfo(object_id_type playerid)
{
	if (NULL == GetPlace())
	{
		return;
	}
	if (CheckPowerOwnerless() != RE_SUCCESS)
	{
		return;
	}
	if (GetPowerGuild() == INVALID_GUILD_ID)
	{
		return;
	}
	float curtime = GET_PROCESS_TIME;
	if (curtime < m_SyncEmenyTimeStamp)
	{
		return;
	}
	LC_ServerPlayer* theplayer = GetPlace()->GetActorManger().GetPlayer(playerid);
	if (theplayer == NULL)
	{
		return;
	}
	bool ispower = isinPowerGuild(theplayer);
	LC_ActorPtrList playerlist;
	GetPlace()->GetAllRealPlayer(playerlist);
	MG_WarEmenyPlayerInfo msg;
	int count = 0;
	LC_ActorPtrList::iterator beg = playerlist.begin();
	for (; beg != playerlist.end(); ++beg)
	{
		if (NULL == (*beg))
		{
			continue;
		}
		LC_ServerPlayer* player = (LC_ServerPlayer*)(*beg);
		if (player->GetID() != playerid)
		{
			if (isinPowerGuild(player) != ispower)
			{
				if (count >= MAX_GUILDWAR_EMENY_SIZE)
				{
					msg.m_Size = count;
					theplayer->SendMsgToClient(MGPT_NOTIFY_POWERGUILDEMENY_INFO, &msg);
					count = 0;
				}
				msg.m_PlayerInfo[count].m_CharID = player->GetInstance();
				msg.m_PlayerInfo[count].m_PowerGuildX = player->GetCurrentLocation().x;
				msg.m_PlayerInfo[count].m_PowerGuildY = player->GetCurrentLocation().y;
				++count;
			}
		}
	}
	if (count > 0)
	{
		msg.m_Size = count;
		theplayer->SendMsgToClient(MGPT_NOTIFY_POWERGUILDEMENY_INFO, &msg);
	}
	m_SyncEmenyTimeStamp += SYNC_ALLENEMYPLAYER_POS;
}

void LC_ServerMapLogic_WarPlay::FillGuildWarInfo(Protocol::MG_WarInfo& guildwarinfo)
{
	if (NULL == GetPlace())
	{
		return;
	}
	guildwarinfo.m_IsActive = true;
	if (!CheckRun())
	{
		guildwarinfo.m_IsActive = false;
	}

	guildwarinfo.m_WarType = m_WarType;
	guildwarinfo.m_PowerRespawnChar = m_Config->PowerRespawn.CharType;
	guildwarinfo.m_PowerRespawnX = m_Config->PowerRespawn.Pos.x;
	guildwarinfo.m_PowerRespawnY = m_Config->PowerRespawn.Pos.y;

	guildwarinfo.m_ChallengeRespawnChar = m_Config->ChallengeRespawn.CharType;
	guildwarinfo.m_ChallengeRespawnX = m_Config->ChallengeRespawn.Pos.x;
	guildwarinfo.m_ChallengeRespawnY = m_Config->ChallengeRespawn.Pos.y;

	if (CheckPowerOwnerless() != RE_SUCCESS)
	{
		guildwarinfo.m_PowerGenerate = true;
		if (GetPowerGuild() != INVALID_GUILD_ID)
		{
			LC_ServerGuildInfo* guildinfo = GetGuild(GetPowerGuild());
			if (guildinfo != NULL)
			{
				StringType mastname;
				guildinfo->GetMasterName(mastname);
				guildwarinfo.m_PowerChar = guildinfo->GetMasterID();
				TPSTR2STDSTR(mastname, guildwarinfo.m_PowerGuildPlayerName);
				TPSTR2STDSTR(guildinfo->GetName(), guildwarinfo.m_PowerGuildName);
				guildwarinfo.m_PowerGuildId = GetPowerGuild();
			}
		}
	}
	else
	{
		guildwarinfo.m_PowerGenerate = false;
		if (GetPowerGuild() != INVALID_GUILD_ID && GetPowerPlayer() != INVALID_ACTOR_ID)
		{
			LC_ServerGuildInfo* guildinfo = GetGuild(GetPowerGuild());
			LC_ServerPlayer* powerplayer = GetPlace()->GetActorManger().GetPlayer(GetPowerPlayer());
			if (powerplayer != NULL && guildinfo != NULL)
			{
				TPSTR2STDSTR(powerplayer->GetOwnerCharName(), guildwarinfo.m_PowerGuildPlayerName);
				guildwarinfo.m_PowerChar = powerplayer->GetInstance();
				guildwarinfo.m_PowerGuildX = powerplayer->GetCurrentLocation().x;
				guildwarinfo.m_PowerGuildY = powerplayer->GetCurrentLocation().y;
				guildwarinfo.m_PowerGuildTime = (int)(GET_PROCESS_TIME - (m_PowerTimeStamp - m_Config->Power.DurationTime));
				TPSTR2STDSTR(guildinfo->GetName(), guildwarinfo.m_PowerGuildName);
				guildwarinfo.m_PowerGuildId = GetPowerGuild();
			}
		}
		else
		{
			guildwarinfo.m_PowerGuildX = m_Config->Power.NaviPos.x;
			guildwarinfo.m_PowerGuildY = m_Config->Power.NaviPos.y;
		}
	}
}

void LC_ServerMapLogic_WarPlay::FillGuildWarPlayerInfo(Protocol::MG_WarPlayerInfo& guildwarplayerinfo)
{
	if (NULL == GetPlace())
	{
		return;
	}

	if (!CheckRun())
	{
		return;
	}
	if (CheckPowerOwnerless() != RE_SUCCESS)
	{
		guildwarplayerinfo.m_PowerGenerate = true;
		guildwarplayerinfo.m_PowerGuildX = m_Config->Power.NaviPos.x;
		guildwarplayerinfo.m_PowerGuildY = m_Config->Power.NaviPos.y;
		if (GetPowerGuild() != INVALID_GUILD_ID)
		{
			LC_ServerGuildInfo* guildinfo = GetGuild(GetPowerGuild());
			if (guildinfo != NULL)
			{
				guildwarplayerinfo.m_PowerChar = guildinfo->GetMasterID();
			}
		}
	}
	else
	{
		guildwarplayerinfo.m_PowerGenerate = false;
		if (GetPowerGuild() != INVALID_GUILD_ID && GetPowerPlayer() != INVALID_ACTOR_ID)
		{
			LC_ServerPlayer* powerplayer = GetPlace()->GetActorManger().GetPlayer(GetPowerPlayer());
			if (powerplayer != NULL)
			{
				guildwarplayerinfo.m_PowerGuildX = powerplayer->GetCurrentLocation().x;
				guildwarplayerinfo.m_PowerGuildY = powerplayer->GetCurrentLocation().y;
				guildwarplayerinfo.m_PowerChar = powerplayer->GetInstance();
			}
		}
		else
		{
			guildwarplayerinfo.m_PowerGuildX = m_Config->Power.NaviPos.x;
			guildwarplayerinfo.m_PowerGuildY = m_Config->Power.NaviPos.y;
		}
	}
}

void LC_ServerMapLogic_WarPlay::__setPowerNPCState(bool sleep)
{
	if (m_PowerNpcId == 0 || NULL == GetPlace())
	{
		return;
	}

	LC_ServerNPC* thepower = GetPlace()->GetActorManger().GetNpc(m_PowerNpcId);
	if (thepower == NULL)
	{
		return;
	}
	if (sleep)
	{
		//进入睡眠状态要播一个动作，所以放一个技能
		if (GetPlace()->IsCrossRealm())
		{
			thepower->ExecuteSkill(21915649);
		}
		else
		{
			thepower->ExecuteSkill(21915647);
		}

	}
	else
	{
		//因为台子进入睡眠状态在释放一个技能（技能动作需要时间）后在设置睡眠的标志位，打断技能是为了防止，回收圣火令后，台子技能继续执行，从而标志位错误
		//如果处于进入睡眠这个技能中，就中断技能
		if (IS_SKILL_TYPE(thepower->GetCurrentStateType()))
		{
			thepower->InterruptSkill(GET_PROCESS_TIME, thepower->GetCurrentStateType(), SKI_FORCE);
		}
		thepower->SetSkillFlag(SK_SLEEP, sleep);
	}
	return;
}

void LC_ServerMapLogic_WarPlay::__setPowerGuild(uint64_t guildid, object_id_type playerid, bool notify)//用来设置内部执令人
{
	if (NULL == GetPlace())
	{
		return;
	}
	m_PowerGuild = guildid;
	m_PowerPlayer = playerid;
	if (guildid == INVALID_GUILD_ID || playerid == INVALID_ACTOR_ID)
	{
		m_PowerTransId = INVALID_GUILD_ID;
		m_PowerTimeStamp = 0;
		//设置npc状态
		__setPowerNPCState(false);
	}
	else
	{
		m_PowerTransId = guildid;
		m_PowerTimeStamp = GET_PROCESS_TIME + m_Config->Power.DurationTime;
		__setPowerNPCState(true);
	}
	SyncPowerGuildInfo();
	if (notify)
	{
		LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
		unique_id_impl uiPowerId = 0;
		LC_ServerMapLogic_Place* pPlace = GetPlace();
		if (pPlace != NULL)
		{
			LC_ServerPlayer* theplayer = pPlace->GetActorManger().GetPlayer(m_PowerPlayer);
			if (theplayer != NULL)
			{
				uiPowerId = theplayer->GetInstance();
			}
		}
		manager->NotifyGivenPower(m_PowerGuild, m_WarType, GetPlace()->GetMapLogicID(), uiPowerId);
	}
}

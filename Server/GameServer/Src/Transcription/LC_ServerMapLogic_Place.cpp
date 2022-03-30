
#include "GameServerApp.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerNPCManager.h"
#include "LC_ServerNPC.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "LC_StoryLogic_Manager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_AutoFitNPCManager.h"
#include "LC_ServerMapLogic_TranscriptionRuleManger.h"
#include "LC_GameStory_Manager.h"
#include "LC_ServerItemFlopManager.h"
#include "LC_ServerPlaceHolderManager.h"
#include "LC_ServerTaskManager.h"
#include <boost/algorithm/string/replace.hpp>
#include "oss_define.h"
#include "SC_ServerSkillInterface.h"
#include "SC_ServerScriptInterface.h"
#include "PF_ServerPathManager.h"
#include "SK_ServerSkillEventManager.h"
#include "LC_ServerMapLogic_BatchPlay.h"
#include "LC_ServerGuildManager.h"
#include "LC_GameEventManager.h"
#include "LC_RankManager.h"
#include "ConfigManager.h"
#include "SC_ScriptAction.h"

#include "CF_TranscriptionSettleGrade.h"
#include "CF_TranscriptionRule.h"
#include "CF_WorldMapList.h"
#include "CF_TransferArea.h"

#include "UT_MathBase.h"
#include "UT_ServerHelper.h"
#include "LC_ServerMapLogic_PlayManager.h"
#include "MG_GroupProtocol.h"
#include "../Buddy/BuddyInstance.h"
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;
using namespace GameStory;
using namespace Skill;


//-------------------------------------------------------------------------------------------------------------
int get_hpbase(int size)
{
	if (size <= 1)
	{
		return 10000;
	}
	else if (size == 2)
	{
		return 17500;
	}
	else if (size == 3)
	{
		return 25000;
	}
	else
	{
		return 32500;
	}
}

int get_damagebase(int size)
{
	if (size <= 1)
	{
		return 10000;
	}
	else if (size == 2)
	{
		return 10000;
	}
	else if (size == 3)
	{
		return 10000;
	}
	else
	{
		return 10000;
	}
}

void adjust_Npc_Hp_Dm_by_playersize(LC_ServerNPC* npc, int presize, int cursize)
{
	attr_value_type prehpbase = get_hpbase(presize);
	attr_value_type curhpbase = get_hpbase(cursize);
	attr_value_type prdmpbase = get_damagebase(presize);
	attr_value_type curdmbase = get_damagebase(cursize);
	bool hpchange = false;
	bool dmpchange = false;
	attr_value_type hp = 0;
	if (prehpbase != curhpbase)
	{
		attr_value_type curhp = npc->GetHP();
		hp = (attr_value_type)((double)curhp * ((double)curhpbase / (double)prehpbase));
		npc->GetAttributeMap()->RemoveAttributePercentFunc(ATT_MAX_HP, prehpbase - 10000);
		npc->GetAttributeMap()->AddAttributePercentFunc(ATT_MAX_HP, curhpbase - 10000);
		hpchange = true;
	}
	if (prdmpbase != curdmbase)
	{
		npc->GetAttributeMap()->RemoveAttributePercentFunc(ATT_MAX_ATK, prdmpbase - 10000);
		npc->GetAttributeMap()->AddAttributePercentFunc(ATT_MAX_ATK, curdmbase - 10000);
		dmpchange = true;
	}
	if (hpchange || dmpchange)
	{
		npc->GetAttributeMap()->RecomputeAll();
		if (hpchange)
		{
			npc->SetHP(hp);
		}
	}
}
//-------------------------------------------------------------------------------------------------------------
void LC_ServerMapLogic_Record::Init()
{
	m_KillMonsterRecord = 0;
	m_DeadRecord = 0;
	m_starttime = 0;
	m_endtime = 0;
}

void LC_ServerMapLogic_Record::UnInit()
{
	m_KillMonsterRecord = 0;
	m_DeadRecord = 0;
	m_starttime = 0;
	m_endtime = 0;
}

float LC_ServerMapLogic_Record::getPassTime()
{
	//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "m_endtime:%f,begintime:%f,curtime:%f", m_endtime, m_starttime, GET_PROCESS_TIME);
	return (m_endtime > m_starttime) ? (m_endtime - m_starttime) : (GET_PROCESS_TIME - m_starttime);
}

int LC_ServerMapLogic_Record::getScore(int resid, bool& preEnd)
{
	//计算分数
	LC_ServerMapLogic_TranscriptionRuleManger* rulemanager = SERVER_GET_TRANSCRIPTIONRULE_MANAGER;
	CF_TranscriptionRule::DataEntry* pkentry = rulemanager->GetRule(resid);
	if (pkentry == NULL)
	{
		return 0;
	}

	int32_t iScore = 0;
	if(!pkentry->_Score.empty())
	{
		TStringVector kParams;
		kParams.push_back("DEATHTIME");
		kParams.push_back("REMAINTIME");
		//kParams.push_back("PASSTIME");
		//SA_RT_TRIGGER_EXPRESSION(int32_t, iScore, pkentry->_Score, kParams, m_DeadRecord, getPassTime());
		SA_RT_TRIGGER_EXPRESSION(int32_t, iScore, pkentry->_Score, kParams, m_DeadRecord, pkentry->_iExistTime-getPassTime());
	}
	else if(!pkentry->_Stage.empty())
	{
		TStringVector kParams;
		kParams.push_back("PASSTIME");
		kParams.push_back("PASSTIME");
		kParams.push_back("PASSTIME");

		float passTime = getPassTime();
		SA_RT_TRIGGER_EXPRESSION(int32_t, iScore, pkentry->_Stage, kParams, passTime, passTime, passTime);
	}
	preEnd = pkentry->_iExistTime-getPassTime() >= pkentry->_StarTime ? true : false;
	return iScore;
}
//--------------------------------------------------------------------------------------------------------------
LC_ServerMapLogic_Door::LC_ServerMapLogic_Door():m_ID(0), m_TypeId(0), m_State(DoorState_Hide), m_Angel(0), m_Obb()
{

}

LC_ServerMapLogic_Door::LC_ServerMapLogic_Door(DungeonConfig::DoorEntry& door):m_ID(0), m_TypeId(door.TypeId), m_State((DoorState)door.InitState), m_Pos(door.Position), m_Angel(door.Angel), m_Obb()
{
	m_Obb.kCenterPoint = door.CenterPoint;
	m_Obb.kAxisX = door.XAxis;
	m_Obb.kAxisY = door.YAxis;
	m_Obb.iHalfExtentX = door.HalfExtentX;
	m_Obb.iHalfExtentY = door.HalfExtentY;
}

LC_ServerMapLogic_Door::LC_ServerMapLogic_Door(const LC_ServerMapLogic_Door& other):m_ID(other.m_ID), m_TypeId(other.m_TypeId), m_State(other.m_State), m_Pos(other.m_Pos), m_Angel(other.m_Angel), m_Obb(other.m_Obb)
{

}

LC_ServerMapLogic_Door& LC_ServerMapLogic_Door::operator =(const LC_ServerMapLogic_Door& other)
{
	m_ID = other.m_ID;
	m_TypeId = other.m_TypeId;
	m_State = other.m_State;
	m_Angel = other.m_Angel;
	m_Pos = other.m_Pos;
	m_Obb = other.m_Obb;
	return *this;
}

bool LC_ServerMapLogic_Door::SetDoorState(DoorState state)
{
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerNPC* the_door = (LC_ServerNPC*)pkServerWorldManager->FindObject(m_ID);
	if (the_door == NULL)
	{
		return false;
	}
	if (state == DoorState_Open)
	{
		the_door->SetSkillFlag(SK_SLEEP, true);
	}
	else if (state == DoorState_Close)
	{
		the_door->SetSkillFlag(SK_SLEEP, false);
	}
	else
	{
		the_door->SetSkillFlag(SK_SLEEP, false);
	}
	return true;
}
//---------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
LC_ServerMapLogic_Place::LC_ServerMapLogic_Place():
	m_controllerState(false), m_PlaceState(PLACESTATE_UNINITED), m_TimeStamp(0.0f), m_PersistTotalTime(0), m_CountDownTime(0),m_PlayerVaildTime(0),
	m_AutoFinish(false), m_FirstEnterActive(false), m_AllPlayerOrg(0), m_AllMonsterOrg(0), m_AllGroup(),
	m_GroupShare(Share_None), m_Exclusive(Exclusive_Together), m_FactionContain(),
	m_inerId2Faction(), m_logicRecord(), m_TransType(TransPointType_None), m_TransPort(), m_DoorVector(), m_dialogNpcId(0), m_dialoguePlayerMap(),
	m_bdialogue(false), m_CostVigor(0), m_CheckRule(false), m_Rule(NULL), m_Autofit(false),
	m_AutofitType(AUTOFIT_TYPE_NORMAL), m_AutofitLevel(0), m_AutoChangeMonsterHP(false), m_MaxPlayer(0), m_Elites(), m_MonsterLevel(0),
	m_CrossRealm(false), m_CrossRealmType(CrossRealmType_Team), m_CrossRealmId(0), m_CanPK(true),
	m_AndPlays(), m_OrPlays(), m_AllPlayEnd(false), m_AutoFinishPlayEnd(true), m_Start(false), m_ActorManger(), m_GuildTransport(),
	m_bEnterLocked(false), m_iTributaryMaxPlayer(0), m_bCreateCross(false), m_bVaild(false), m_bIShowRaidStage(false), m_bTributaryAutoRelease(false)
{
}

LC_ServerMapLogic_Place::~LC_ServerMapLogic_Place()
{
	UnInit();
}

void LC_ServerMapLogic_Place::Init()
{
	//std::stringstream logStr;
	//logStr <<" LC_ServerMapLogic_Place::Init ";
	////m_AndPlays.clear();
	////m_OrPlays.clear();
	//if(m_AndPlays.empty())
	//	logStr<<" m_AndPlays is empty ";
	//else 
	//	logStr<<" m_AndPlays had sometihing  ";
	//if(m_OrPlays.empty())
	//	logStr<<" m_OrPlays is empty ";
	//else 
	//	logStr<<" m_OrPlays had sometihing  ";
	//GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "%s \n",logStr.str().c_str());
	mem::set<int> setA, setB;
	ClearAllNPC(setA, setB);
	ClearAllPlay();
	LC_ServerMapLogicBase::Init();
	m_ActorManger.Init(this);
	m_TimeStamp = 0;

	LC_ServerMapLogic_Organize* org = GetActorManger().CreateOrganize();
	if (org == NULL)
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	m_AllPlayerOrg = org->Organize_id;

	org = GetActorManger().CreateOrganize();
	if (org == NULL)
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	m_AllMonsterOrg = org->Organize_id;
	//创建阵营
	m_FactionContain.clear();
	m_inerId2Faction.clear();
	if (!CreateFaction(FACTION_PLAYER))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_NEUTRAL))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}

	if (!CreateFaction(FACTION_JUSTICE))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_EVIL))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_RAIDEVIL))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_TARGET))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_GUILDGUARD))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_SERVERGUARD))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_DEFENDER))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_DEF_MONSTER))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_ATTACKER))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_ATK_MONSTER))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_RED))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_GREEN))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}
	if (!CreateFaction(FACTION_ATK_MONSTER))
	{
		SetPlaceState(PLACESTATE_UNINITED);
		SetActive(false);
		return;
	}

	m_AllGroup.clear();

	m_logicRecord.Init();
	m_TransType = TransPointType_None;
	m_TransPort.clear();
	m_DoorVector.clear();
	m_dialogNpcId = 0;
	m_dialoguePlayerMap.clear();
	m_bdialogue = false;
	m_controllerState = false;

	m_Elites.clear();
	m_MonsterLevel = 0;
	m_CrossRealm = false;
	m_CrossRealmType = CrossRealmType_Team;
	m_CrossRealmId = 0;

	m_AndPlays.clear();
	m_OrPlays.clear();
	m_AllPlayEnd = false;
	m_GuildTransport.clear();
	//初始化场地规则
	LC_ServerMapLogic_TranscriptionRuleManger* rulemanager = SERVER_GET_TRANSCRIPTIONRULE_MANAGER;
	CF_TranscriptionRule::DataEntry* pkEntry = rulemanager->GetRule(GetMapResID());
	CF_WorldMapList::DataEntry* pkWorldEntry = CF_WorldMapList::GetSingleton().GetEntryPtr(GetMapResID());
	if (pkWorldEntry == NULL)
	{
		SetActive(false);
		return;
	}
	if (pkEntry == NULL)
	{
		m_CheckRule = false;
		m_Rule = NULL;
		m_AutoChangeMonsterHP = false;
		m_MaxPlayer = 0;
		m_CostVigor = 0;

		m_PersistTotalTime = 7200;
		m_CountDownTime = 300;
		m_PlayerVaildTime = 0;
		SetOfflineTime(0);
		m_FirstEnterActive = false;
		m_AutoFinish = false;
		m_AutoFinishPlayEnd = false;
		m_bVaild = true;
		m_GroupShare = Share_None;
		m_Exclusive = Exclusive_Together;
		m_TransType = TransPointType_None;
		m_bIShowRaidStage = false;
	}
	else
	{
		m_CheckRule = true;
		m_Rule = pkEntry;
		m_AutoChangeMonsterHP = false;
		if (pkEntry->_AutoChangeNPCHP > 0)
		{
			m_AutoChangeMonsterHP = true;
		}
		m_MaxPlayer = 0;
		if (pkEntry->_iGroupMaxNum > 0)
		{
			m_MaxPlayer = pkEntry->_iGroupMaxNum;
		}
		m_CostVigor = pkEntry->_iStamina;
		m_PersistTotalTime = pkEntry->_iExistTime;
		m_CountDownTime = pkEntry->_CountDownTime;
		m_PlayerVaildTime = pkEntry->_PlayerVaildTime;
		m_bIShowRaidStage = pkEntry->_iShowRaidStage;
		SetOfflineTime(pkEntry->_iPlayerStayTime);
		m_FirstEnterActive = false;
		if (pkEntry->_FirstPlayerEnterActive > 0)
		{
			m_FirstEnterActive = true;
		}
		m_AutoFinish = false;
		if (pkEntry->_AllPLayerLeaveEnd > 0)
		{
			m_AutoFinish = true;
		}
		m_AutoFinishPlayEnd = false;
		if (pkEntry->_PlayEnd > 0)
		{
			m_AutoFinishPlayEnd = true;
		}
		m_bVaild = true;
		m_GroupShare = (Share_Type)pkEntry->_Share;
		m_Exclusive = (Exclusive_Type)pkEntry->_Exclusive;
		m_TransType = (TransPointType)pkEntry->_Transport;
	}

	m_CanPK = true;
	if (pkWorldEntry->_CanPK == 0)
	{
		m_CanPK = false;
	}
	SetRecordPK(true);
	if (pkWorldEntry->_RecordPK == 0)
	{
		SetRecordPK(false);
	}
	m_Autofit = (1 == pkWorldEntry->_iAutoFitLevel) ? true : false;
	m_AutofitType = (AutoFitType)pkWorldEntry->_iAutoFitType;
	m_AutofitLevel = 0;

	SetPlaceState(PLACESTATE_PROCESS);
	m_Start = false;
	if (m_FirstEnterActive)
	{
		SetActive(false);
	}
	else
	{
		SetActive(true);
	}
	m_bEnterLocked = false;
	m_bCreateCross = false;
	m_iTributaryIndex = 0;
}

void LC_ServerMapLogic_Place::UnInit()
{
	LetAllPlayerLeave();
	//先把地图中的NPC和宝箱清掉
	ClearAllObjectsInMapWhenQuit();
	ClearAllPlay();
	mem::set<int> setA, setB;
	ClearAllNPC(setA, setB);
	if (m_AllPlayerOrg > 0)
	{
		GetActorManger().UnOrganizeActor(m_AllPlayerOrg);
		m_AllPlayerOrg = 0;
	}
	if (m_AllMonsterOrg > 0)
	{
		GetActorManger().UnOrganizeActor(m_AllMonsterOrg);
		m_AllMonsterOrg = 0;
	}
	DelFaction(FACTION_PLAYER);
	DelFaction(FACTION_NEUTRAL);
	DelFaction(FACTION_JUSTICE);
	DelFaction(FACTION_EVIL);
	DelFaction(FACTION_RAIDEVIL);
	DelFaction(FACTION_TARGET);
	DelFaction(FACTION_GUILDGUARD);
	DelFaction(FACTION_SERVERGUARD);
	DelFaction(FACTION_DEFENDER);
	DelFaction(FACTION_DEF_MONSTER);
	DelFaction(FACTION_ATTACKER);
	DelFaction(FACTION_ATK_MONSTER);
	m_FactionContain.clear();
	m_inerId2Faction.clear();
	m_GuildTransport.clear();

	//删除门
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerNPCManager* pkNPCManager		= SERVER_GET_NPC_MANAGER();
	DoorVector::iterator doorbeg = m_DoorVector.begin();
	for (; doorbeg != m_DoorVector.end(); ++doorbeg)
	{
		LC_ServerNPC* pkNPC  = (LC_ServerNPC*)pkNPCManager->FindNPC(doorbeg->m_ID);
		if (pkNPC)
		{
			pkWorldManager->RemoveObjectFromMap(pkNPC);
			pkNPCManager->DestoryNPC(pkNPC);
		}
	}
	m_DoorVector.clear();

	m_PersistTotalTime = 0;
	m_CountDownTime = 0;
	m_PlayerVaildTime = 0;
	m_AutoFinish = false;
	m_FirstEnterActive = false;
	m_AllGroup.clear();
	m_GroupShare = Share_None;
	m_Exclusive = Exclusive_Together;

	m_logicRecord.UnInit();
	m_TransType = TransPointType_None;
	m_TransPort.clear();
	m_dialogNpcId = 0;
	m_dialoguePlayerMap.clear();
	m_bdialogue = false;
	m_CostVigor = 0;
	m_AutoChangeMonsterHP = false;
	m_MaxPlayer = 0;
	m_Elites.clear();
	m_MonsterLevel = 0;

	m_CrossRealm = false;
	m_CrossRealmType = CrossRealmType_Team;
	m_CrossRealmId = 0;
	m_CanPK = true;
	m_AndPlays.clear();
	m_OrPlays.clear();
	m_AllPlayEnd = false;
	m_AutoFinishPlayEnd = false;
	m_bVaild = false;
	m_bIShowRaidStage = false;

	SetPlaceState(PLACESTATE_UNINITED);
	m_ActorManger.UnInit();
	m_Start = false;
	SetActive(false);
	m_bEnterLocked = true;
	m_bCreateCross = false;
	m_iTributaryIndex = 0;
	LC_ServerMapLogicBase::UnInit();
}

void LC_ServerMapLogic_Place::Update(float fCurrentTime, float fDeltaTime)
{
	GameServerManager* pkAppMgr = GetGameServerApp()->GetServerManager();
	if (!pkAppMgr->isLoaded())
	{
		return;
	}

	LC_ServerMapLogicBase::Update(fCurrentTime, fDeltaTime);
	PlaceLogicState result = GetPlaceState();
	if (result == PLACESTATE_UNINITED)
	{
		SetActive(false);
		return;
	}
	else if(result == PLACESTATE_PROCESS)//进行中
	{
		if (m_Start)
		{
			int resMapID = GetMapResID();
			int mapID = GetMapLogicID();
			float time = GET_PROCESS_TIME;
			float fRemainTime = fCurrentTime - m_TimeStamp - m_CountDownTime;
			if(GetControllerState() && GetAllRealPlayerSize()==1)
			{
				SetPlaceState(PLACESTATE_FAIL);//单人副本，指挥官死亡，失败
			}
			else if (m_TimeStamp > 1.0f && fCurrentTime >= m_TimeStamp+m_CountDownTime)
			{
				SetPlaceState(PLACESTATE_FAIL);//时间到，失败	
			}			
			else
			{
				UpdateAllPlay(fCurrentTime, fDeltaTime);
			}
		}
	}
	else if (result == PLACESTATE_PREEND) //进入倒计时
	{
		//副本提前结束后，结算界面弹出之前，清除场景中的所有怪（副本中的怪和玩家用技能召唤的怪都清掉）
		ClearAllMonster();

		//倒计时开始到副本结束的时间 时间到退出副本
		if (fCurrentTime >= m_TimeStamp)
		{
			SetPlaceState(PLACESTATE_END);
		}
		else
		{
			if (m_Start)
			{
				UpdateAllPlay(fCurrentTime, fDeltaTime);
			}
		}
	}
	else if(result == PLACESTATE_FAIL)
	{
		//副本失败后，结算界面弹出之前，清除场景中的所有怪（副本中的怪和玩家用技能召唤的怪都清掉）
		ClearAllMonster();

		if(m_TimeStamp>1.0f && fCurrentTime >= m_TimeStamp)
		{
			SetPlaceState(PLACESTATE_END);
		}
	}
	else if (result == PLACESTATE_END)
	{
		EndAllPlay();
		UnInit();
		return;
	}
}

void LC_ServerMapLogic_Place::Finsh()
{
	SetPlaceState(PLACESTATE_END);
}

void LC_ServerMapLogic_Place::PreFinsh()
{
	SetPlaceState(PLACESTATE_PREEND);
}

void LC_ServerMapLogic_Place::AllPlayEnd()
{
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (IsCrossRealm())
	{
		CrossRealmType crosstype = CrossRealmType_Team;
		int crossid = 0;
		GetCrossRealm(crosstype, crossid);
		LC_ServerMapLogic_BatchPlay* play = (LC_ServerMapLogic_BatchPlay*)GetPlay(PlayType_Batch);
		if (play == NULL || !play->GetResult())
		{
			enterLeaveManager->CrossRealmEnd(crosstype, crossid, 0);
		}
		else
		{
			enterLeaveManager->CrossRealmEnd(crosstype, crossid, 1, (int)GetRecord()->getPassTime());
		}
	}
	if (CheckGuildTransport())
	{
		GuildTransportMap::iterator beg = m_GuildTransport.begin();
		for (; beg != m_GuildTransport.end(); ++beg)
		{
			enterLeaveManager->CrossRealmEnd(CrossRealmType_GuildTransport, beg->second.guildid, beg->second.transportid);
		}
	}
}

void LC_ServerMapLogic_Place::SetActiveFirstEnter(bool active)
{
	m_FirstEnterActive = active;
	if (m_FirstEnterActive)
	{
		if (GetAllRealPlayerSize() > 0)
		{
			SetActive(true);
		}
		else
		{
			SetActive(false);
		}
	}
}

void LC_ServerMapLogic_Place::SetPersistTime(int persisttime)
{
	if (m_PersistTotalTime == persisttime)
	{
		return;
	}
	int pretime = m_PersistTotalTime;
	m_PersistTotalTime = persisttime;
	if (GetPlaceState() == PLACESTATE_PROCESS)
	{
		if (persisttime <= 0)
		{
			m_TimeStamp = 0.0f;
		}
		else
		{
			m_TimeStamp += (float)(persisttime - pretime);
		}
	}
}

void LC_ServerMapLogic_Place::SetCountDownTime(int countdown)
{
	if (m_CountDownTime == countdown)
	{
		return;
	}
	int pretime = m_CountDownTime;
	m_CountDownTime = countdown;
	if (GetPlaceState() == PLACESTATE_PROCESS)
	{
		if (m_TimeStamp > 1.0f)
		{
			m_TimeStamp -= (float)(countdown - pretime);
		}
	}
}

int LC_ServerMapLogic_Place::GetLeftTime()
{
	float curtime = GET_PROCESS_TIME;
	if (m_PlaceState == PLACESTATE_PROCESS)
	{
		if (m_TimeStamp <= 1.0f)
		{
			return 0;
		}
		return (int)(m_TimeStamp - curtime + m_CountDownTime);
	}
	else if (m_PlaceState == PLACESTATE_PREEND)
	{
		return (int)(m_TimeStamp - curtime);
	}
	return 0;
}

void LC_ServerMapLogic_Place::SetControllerState(bool dead)
{
	if (m_controllerState != dead)
	{
		m_controllerState = dead;
	}
}

bool LC_ServerMapLogic_Place::GetControllerState()
{
	return m_controllerState;
}

void LC_ServerMapLogic_Place::SetPlaceState(PlaceLogicState flag)
{
	if (m_PlaceState == flag)
	{
		return;
	}

	if(flag != PLACESTATE_PROCESS)
	{
		SetVaild(false);
	}

	m_PlaceState = flag;
	if (m_PlaceState == PLACESTATE_PROCESS)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "LC_ServerMapLogic_Place SetPlaceState RecordStarttime", GetMapResID(), GET_PROCESS_TIME);
		m_logicRecord.RecordStarttime(GET_PROCESS_TIME);
		if (m_PersistTotalTime <= 0)
		{
			m_TimeStamp = 0.0f;
		}
		else
		{
			float curtime = GET_PROCESS_TIME;
			m_TimeStamp = curtime + (float)(m_PersistTotalTime - m_CountDownTime);
		}
	}
	else if (m_PlaceState == PLACESTATE_PREEND)
	{//倒计时开始到副本结束的时间
		m_TimeStamp = GET_PROCESS_TIME + m_CountDownTime;
		BroadCastGameEventToClient(EVENT_TYPE_SHUTDOWN_TIME_LEFT, m_CountDownTime);	
	}
	else if(m_PlaceState == PLACESTATE_FAIL)
	{
		float curtime = GET_PROCESS_TIME;
		float countDownTime = m_CountDownTime;
		if(m_bIShowRaidStage)
		{
			countDownTime = (float)GetGlobalSetting.failCountDownTime;
		}

		m_TimeStamp = curtime + (float)(countDownTime);

		if(m_Rule&&0 == m_Rule->_kSettle)
		{
			int resMapID = GetMapResID();
			int mapID = GetMapLogicID();
			BroadCastGameEventToClient(EVENT_TYPE_FAIL, (int)countDownTime);
		}		
	}
	else if (m_PlaceState == PLACESTATE_END)
	{
		//GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "RecordEndtime mapResID:%d, time:%d", GetMapResID(), GET_PROCESS_TIME);
		m_logicRecord.RecordEndtime(GET_PROCESS_TIME);
		//需要删除队伍共享副本
		if (GetGroupShare() == Share_None)
		{
			return;
		}
		__clearAllGroupShare();
	}
}

//todo::根据玩法确定
Utility::UT_Vec3Int	LC_ServerMapLogic_Place::ComputeEnterLocation(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, const Utility::UT_Vec3Int& rkTargetLocation)
{
	const unique_id_impl& playerInstance = ((LC_ServerPlayer*)pkPlayer)->GetInstance();
	int index = 0;
	BeginReadPlay(index);
	PlayVector::iterator it;
	while (ReadOnePlay(it, index))
	{
		Utility::UT_Vec3Int pos = (*it)->ComputeEnterLocation((LC_ServerPlayer*)pkPlayer, rkTargetLocation);
		if (pos != UT_Vec3Int(0, 0, 0))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "ComputeEnterLocation pos", playerInstance.detail(), playerInstance.catagory(), ((LC_ServerPlayer*)pkPlayer)->GetUserID(), pkPlayer->GetID(), pos.x, pos.y,pos.z);
			return pos;
		}
	}
	if (rkTargetLocation != UT_Vec3Int(0, 0, 0) && (NULL==m_pkMap || m_pkMap->TestLocationValid(rkTargetLocation.x, rkTargetLocation.y)))
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "ComputeEnterLocation rkTargetLocation", playerInstance.detail(), playerInstance.catagory(), ((LC_ServerPlayer*)pkPlayer)->GetUserID(), pkPlayer->GetID(), rkTargetLocation.x, rkTargetLocation.y,rkTargetLocation.z);
		return rkTargetLocation;
	}
	if (m_TransType == TransPointType_Random) //随机点
	{
		Utility::UT_Vec3Int newpos;
		if (RandomPos(newpos))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "ComputeEnterLocation newpos", playerInstance.detail(), playerInstance.catagory(), ((LC_ServerPlayer*)pkPlayer)->GetUserID(), pkPlayer->GetID(), newpos.x, newpos.y,newpos.z);
			return newpos;
		}
	}
	else if (m_TransType == TransPointType_Point)
	{
		GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "ComputeEnterLocation m_TransPort", playerInstance.detail(), playerInstance.catagory(), ((LC_ServerPlayer*)pkPlayer)->GetUserID(), pkPlayer->GetID(), m_TransPort.x, m_TransPort.y,m_TransPort.z);
		return m_TransPort;
	}

	if (GetGroupShare() == Share_Normal)
	{
		GameLogic::LC_ActorPtrList playerlist;
		GetAllRealPlayer(playerlist);
		if (!playerlist.empty())
		{
			LC_ServerPlayer* theplayer = (LC_ServerPlayer*)(playerlist.front());
			if (theplayer != NULL)
			{
				mem::vector<Utility::UT_Vec3Int> posVec;
				GenRandomPosition(theplayer->GetCurrentLocation(), posVec, 5, GetMapResID());
				int index = UT_MathBase::RandInRangeInt(0, 4);
				return posVec[index];
			}
		}
	}
	UT_Vec3Int det(0, 0, 0);
	int userid = 0;
	int MapResID = GetMapResID();
	int MapResID_1 = 0;
	int citizenship = 0;
	LC_ServerPlayer* pkOwnerPlayer = ((LC_ServerPlayer*)pkPlayer)->GetOwnerPlayer();
	if(pkOwnerPlayer)
	{
		userid = pkOwnerPlayer->GetUserID();
		MapResID_1 = pkOwnerPlayer->GetMapResID();
		citizenship = pkPlayer->GetCitizenship();
	}
	bool crossFlag = UT_ServerHelper::IsInCross((LC_ServerPlayer*)pkPlayer);
	if(pkPlayer->IsMainController())
	{
		if (false == UT_ServerHelper::GetRandomLocationXYZ(det,MapResID,citizenship,crossFlag))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "ComputeEnterLocation GetRandomLocationXYZ error!",playerInstance.detail(), playerInstance.catagory(), ((LC_ServerPlayer*)pkPlayer)->GetUserID(), pkPlayer->GetID(), MapResID_1 ,MapResID, citizenship, det.x, det.y,det.z);
		}
	}
	else
	{
		det = pkOwnerPlayer->GetCurrentLocation();
	}
	GfxWriteFmtLog(LOG_INFO_FILELINE,LOG_SWITCH_INFO, "ComputeEnterLocation success!", playerInstance.detail(), playerInstance.catagory(), ((LC_ServerPlayer*)pkPlayer)->GetUserID(), pkPlayer->GetID(), MapResID_1, MapResID, citizenship, det.x, det.y, det.z, m_TransPort.x, m_TransPort.y, m_TransPort.z);
	return det;
}

ResultType LC_ServerMapLogic_Place::CheckEnterMap(object_id_type playerID, int level, GameLogic::LC_MapInOutType eInOutType)
{
	if (CheckPlayerIn(playerID))
	{
		return RE_SUCCESS;
	}
	ResultType ret = RE_SUCCESS;
	ret = LC_Helper::CanEnterMap(GetMapResID(), level);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}
	if (IsCheckRule())
	{
		ResultType ret = RE_SUCCESS;
		LC_ServerMapLogic_TranscriptionRuleManger* rulemanager = SERVER_GET_TRANSCRIPTIONRULE_MANAGER;
		ret = rulemanager->CheckEnter(playerID, level, m_Rule);
		if (ret != RE_SUCCESS)
		{
			return ret;
		}
		//检查人数
		LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(playerID, m_AllPlayerOrg);
		if (actor == NULL || actor->IsLeave())
		{
			if (m_Rule->_iGroupMaxNum > 0 && GetAllRealPlayerSize() >= m_Rule->_iGroupMaxNum)
			{
				ret = RE_TRANSCRIPTION_GROUP_NUM_TOO_LARGE;
				return ret;
			}
		}
	}
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_Place::CheckEnterMap(LC_PlayerBase* pkPlayer, LC_MapInOutType eInOutType)
{
	LC_ServerPlayer* pServerPlayer = (LC_ServerPlayer*)pkPlayer;
	if (!pServerPlayer->CheckEnterMap_Task(GetMapResID()))
	{
		return RE_FAIL;
	}

	if (CheckPlayerIn(pkPlayer->GetID()))
	{
		return RE_FAIL;
	}

	ResultType ret = RE_SUCCESS;
	ret = LC_Helper::CanEnterMap(GetMapResID(), pkPlayer);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}

	do
	{
		int index = 0;
		BeginReadPlay(index);
		PlayVector::iterator it;
		while (ReadOnePlay(it, index))
		{
			ret = (*it)->CheckEnter((LC_ServerPlayer*)pkPlayer);
			if (ret != RE_SUCCESS)
			{
				break;
			}
		}

		ret = CheckExclusive((LC_ServerPlayer*)pkPlayer);
		if (ret != RE_SUCCESS)
		{
			break;
		}

		if (eInOutType!=LMIOT_RECOVER &&IsCheckRule())
		//if (IsCheckRule())
		{
			LC_ServerMapLogic_TranscriptionRuleManger* rulemanager = SERVER_GET_TRANSCRIPTIONRULE_MANAGER;
			ret = rulemanager->CheckEnter(pkPlayer, m_Rule);
			if (ret != RE_SUCCESS)
			{
				break;
			}
			//检查人数
			LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(pkPlayer->GetID(), m_AllPlayerOrg);
			if (actor == NULL || actor->IsLeave())
			{
				if (m_Rule->_iGroupMaxNum > 0 && GetAllRealPlayerSize() >= m_Rule->_iGroupMaxNum)
				{
					ret = RE_TRANSCRIPTION_GROUP_NUM_TOO_LARGE;
					break;
				}
			}
			if (GetStoryID() && GetMapLogicType()!=MT_GUILD_MAP)
			{
				GameStory::LC_GameStory_Manager* pkGameStoryMgr = SERVER_GET_GAMESTORY_MANAGER;
				ret = pkGameStoryMgr->IsAllowedEnterTranscription(pkPlayer, GetMapResID(), pkPlayer->GetGameStoryAsset().GetGameStoryEntry(GetStoryID()));
				if (ret != RE_SUCCESS)
				{
					break;
				}
				// 判断是否占领 前置塔
				
				if(UT_ServerHelper::IsBestGuildStoryID(GetStoryID()))
				{
					CF_WorldMapList::DataEntry* pkMapEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(m_Rule->_iID);
					if (pkMapEntry == NULL)
					{
						ret = RE_FAIL;
						break;
					}
					LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
					LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
					if(pkGuildInfo == NULL)
					{
						ret =  RE_ENTER_ERROR_NOT_GUILD;
						break;
					}
					if(pkMapEntry->_iMapLogicType == MT_WATCHTOWER
						&& !pkGuildInfo->AllowEntryNextTower(ret,m_Rule))
						break;
				}
			}
		}
	}
	while (0);

	if (ret != RE_SUCCESS)
	{
		LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
		MG_RLT_ENTER_Transcription rltMsg;
		rltMsg.m_iResult			= ret;
		rltMsg.m_lTranscriptionType	= m_eMapLogicType;
		rltMsg.m_lResId = GetMapResID();
		rltMsg.m_lFailurePlayerID = pkServerPlayer->GetID();
		TPSTR2STDSTR(pkServerPlayer->GetOwnerCharName(), rltMsg.m_cFailedPlayerName);
		pkServerPlayer->SendMsgToClient(MGPT_RLT_ENTER_TRANSCRIPTION, &rltMsg);
		return ret;
	}
	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_Place::CheckLeaveMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, float fOfflineTime)
{
	if (eInOutType == LMIOT_OFFLINE)
	{
		//刚掉线
		if (fOfflineTime <= 0.0f)
		{
			if (!CheckPlayerActiveOrLoading(pkPlayer->GetID()))
			{
				return RE_SUCCESS;
			}
			bool ret = GetActorManger().SetActorState(pkPlayer->GetID(), Logic_Actor_State_OFFLINE, GetOfflineTime());
			if (!ret)
			{
				return RE_SUCCESS;
			}
			LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(pkPlayer->GetID(), m_AllPlayerOrg);
			if (actor == NULL || !actor->IsOffline())
			{
				return RE_SUCCESS;
			}
			if (actor->IsOfflineTimeOut())
			{
				return RE_SUCCESS;
			}
			//处理掉线
			MapLogicEvent kPlayerEvent(PlayerEvent_OffLine, pkPlayer->GetID());
			int index = 0;
			BeginReadPlay(index);
			PlayVector::iterator it;
			while (ReadOnePlay(it, index))
			{
				(*it)->PlayerEventNotify((LC_ServerPlayer*)pkPlayer, kPlayerEvent);
			}
			return RE_FAIL;
		}
		else
		{
			LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(pkPlayer->GetID(), m_AllPlayerOrg);
			if (actor == NULL || !actor->IsOffline())
				return RE_SUCCESS;
			if (actor->IsOfflineTimeOut())
			{
				return RE_SUCCESS;
			}
			return RE_FAIL;
		}
	}
	return RE_SUCCESS;
}

void LC_ServerMapLogic_Place::NotifyEnterOfflinePlayer(LC_PlayerBase* pkPlayer)
{
	GameStory::LC_StoryLogic_Manager* pkStoryManager = SERVER_GET_STORYLOGIC_MANAGER;
	pkStoryManager->NotifyEnterOfflinePlayer(pkPlayer, this);
}

ResultType LC_ServerMapLogic_Place::RequestEnterMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, const Utility::UT_Vec3Int& rkTargetLocation)
{
	//if leave jump, fuck...............
	IF_A_NA_PLAYER(pkPlayer){return RE_FAIL;}
	if (GetPlaceState() == PLACESTATE_UNINITED || GetPlaceState() == PLACESTATE_END)
	{
		return RE_FAIL;
	}
	LC_ServerPlayer* theplayer = (LC_ServerPlayer*)pkPlayer;

	theplayer->StopTrade();
	oss_role_copy_enter(theplayer, GetMapResID());
	//查看是否是第一个进入
	if (!IsCrossRealm() && GetGroupShare() == Share_Normal) //分享
	{
		//给队伍增加传送
		LC_PlayerGroupBase* pkPlayerGroup = pkPlayer->GetPlayerGroup();
		if (pkPlayerGroup != NULL)
		{
			pkPlayerGroup->SetRaidPortal(GetMapLogicID(), GetMapResID(), GetMapLogicType(), GetGroupShare(), GetExclusive());
			m_AllGroup.insert(pkPlayerGroup->GetID());
		}
	}
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(pkPlayer->GetID());
	if (NULL == actor || actor->CheckState(Logic_Actor_State_Leave))
	{
		if (m_Rule && !m_Rule->_kConsumeItems.empty())
		{
			PackOperateInfoList kConsumeOPs;
			LC_Helper::BuildConsumeItemOPs(m_Rule->_kConsumeItems, kConsumeOPs);
			LC_Helper::DoItemOPs(theplayer, kConsumeOPs, SYS_FUNC_TP_RAID_ENTER, GetMapResID());
		}
	}
	if (actor == NULL)
	{
		GetActorManger().AddActor(theplayer->GetID(), Logic_Actor_State_Loading);
		GetActorManger().AddActor2Organize(theplayer->GetID(), m_AllPlayerOrg);
		if (GetCostVigor() > 0)
		{
			theplayer->AddVigor(0 - GetCostVigor());
		}
	}
	else
	{
		//增加设置正常代码
		GetActorManger().SetActorState(theplayer->GetID(), Logic_Actor_State_Loading);
	}

	//设置场地PK属性
	//pkPlayer->SetPlaceCanPK(GetCanPK());
	if (LC_Helper::isTranscriptClass(GetMapLogicType()))
	{
		MG_RLT_ENTER_Transcription rltMsg;
		rltMsg.m_iResult = RE_SUCCESS;
		rltMsg.m_lTranscriptionType	= m_eMapLogicType;
		rltMsg.m_lPlayerID	= theplayer->GetID();
		rltMsg.m_iAutoFitLevel	= m_Autofit ? 1 : 0;
		rltMsg.m_iLevelNUM		= m_AutofitLevel;
		theplayer->SendMsgToClient(MGPT_RLT_ENTER_TRANSCRIPTION, &rltMsg);

		//for task added by serein
		//EVENT_TYPE_PLAYER_ENTER
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
		if (pkGameEvent != NULL)
		{
			pkGameEvent->SetEventType(GLET_TRANSCRIPTION_EVENT);
			pkGameEvent->SetEventParam(EP_TRANSCRIPTION_EVENT_TYPE, EVENT_TYPE_PLAYER_ENTER);
			pkGameEvent->SetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_1, pkPlayer->GetID());
			pkGameEvent->SetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_2, GetMapResID());
			pkEventManager->InsertGameEvent(pkGameEvent);
		}
		LC_ActionForbid* pa = theplayer->GetActionForbid();
		pa->AddActionForbid(LAFT_TRADE, LAF_DUEL_MAX_COUNT);
	}

	UT_Vec3Int kTargetLocation		= rkTargetLocation;
	//切换玩家地图
	ResultType ret = LC_ServerMapLogicBase::RequestEnterMap(pkPlayer, eInOutType, rkTargetLocation);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}
	if (IsCrossRealm() && m_CrossRealmType == CrossRealmType_Team)
	{
		pkPlayer->GetCrossRealmTeamAssert()->AddDailyMapTime(GetMapResID());
	}
	if (m_FirstEnterActive)
	{
		SetActive(true);
	}
	_onPlayerEnterMap(pkPlayer, eInOutType);

	return RE_SUCCESS;
}

ResultType LC_ServerMapLogic_Place::RequestLeaveMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, CSVFile::CF_WorldMapList::DataEntry* pkDataEntry)
{
	bool bBack = true;
	GameLogic::LC_MapInOutType eSOutType = eInOutType;
	if(pkDataEntry != NULL && pkDataEntry->_BackToSelf > 0)
	{
		bBack = false;
	}
	ResultType ret = PlayerBeforeQuit(pkPlayer, eSOutType, bBack);
	if (ret != RE_SUCCESS)
	{
		return ret;
	}
	ResultType nRel = LC_ServerMapLogicBase::RequestLeaveMap(pkPlayer, eInOutType, pkDataEntry);
	if(nRel == RE_SUCCESS)
	{
		_onPlayerLeaveMap(pkPlayer, eInOutType);
	}
	return nRel;
}

void	LC_ServerMapLogic_Place::_onPlayerEnterMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType)
{
	//进入地图组队检查
	{
		CF_WorldMapList::DataEntry* pkEntry = this->GetMapResPtr();
		if (pkEntry != NULL && pkEntry->_TeamLimitIn != LC_ServerPlayer::E_PlayerGroupLimit_None)
		{
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
			if(pkServerPlayer != NULL)
			{
				//先删除不匹配
				pkServerPlayer->ClearPlayerGroup(LC_ServerPlayer::E_PlayerGroupMapOpt_InMap, pkEntry->_TeamLimitIn);
				//校验组队状态，不一致的需要客户端提示
				uint32_t nGroupType = pkServerPlayer->GetPlayerGroupType();
				switch(pkEntry->_TeamLimitIn)
				{
				case LC_ServerPlayer::E_PlayerGroupLimit_Local:
					{
						if(nGroupType != GameLogic::E_PlayerTeamType_Local)
						{
							MG_Notify_Group_Not_Match msg;
							msg.m_NeedType = GameLogic::E_PlayerTeamType_Local;
							msg.m_NowType = nGroupType;
							pkServerPlayer->SendMsgToClient(MGPT_NOTIFY_GROUP_NOT_MATCH, &msg);
						}
					}
					break;
				case LC_ServerPlayer::E_PlayerGroupLimit_Cross:
					{
						if(nGroupType != GameLogic::E_PlayerTeamType_Cross)
						{
							MG_Notify_Group_Not_Match msg;
							msg.m_NeedType = GameLogic::E_PlayerTeamType_Local;
							msg.m_NowType = nGroupType;
							pkServerPlayer->SendMsgToClient(MGPT_NOTIFY_GROUP_NOT_MATCH, &msg);
						}
					}
					break;
				case LC_ServerPlayer::E_PlayerGroupLimit_None:
				case LC_ServerPlayer::E_PlayerGroupLimit_All:
				default:
					break;
				}
			}
		}
	}
}

void	LC_ServerMapLogic_Place::_onPlayerLeaveMap(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType)
{
	//离开地图组队检查
	{
		CF_WorldMapList::DataEntry* pkEntry = this->GetMapResPtr();
		if (pkEntry != NULL && pkEntry->_TeamLimitOut != LC_ServerPlayer::E_PlayerGroupLimit_None)
		{
			LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
			if(pkServerPlayer != NULL)
			{
				pkServerPlayer->ClearPlayerGroup(LC_ServerPlayer::E_PlayerGroupMapOpt_OutMap, pkEntry->_TeamLimitOut);
			}
		}
	}
}
ResultType LC_ServerMapLogic_Place::SetPlayerQuitState(LC_ServerPlayer* pkPlayer)
{
	MapLogicEvent kPlayerEvent(PlayerEvent_Leave, pkPlayer->GetID());
	//处理离开
	int index = 0;
	BeginReadPlay(index);
	PlayVector::iterator it;
	while (ReadOnePlay(it, index))
	{
		(*it)->PlayerEventNotify(pkPlayer, kPlayerEvent);
	}

	GetActorManger().SetActorState(pkPlayer->GetID(), Logic_Actor_State_Leave);
	pkPlayer->SetAIControl(false);
	pkPlayer->RefreshSMController();
	pkPlayer->Reset2DefaultFaction();
	return RE_SUCCESS;
}
ResultType LC_ServerMapLogic_Place::PlayerBeforeQuit(GameLogic::LC_PlayerBase* pkPlayer, GameLogic::LC_MapInOutType eInOutType, bool bBack)
{
	LC_ServerPlayer* pkServerPlayer = (LC_ServerPlayer*)pkPlayer;
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(pkPlayer->GetID(), m_AllPlayerOrg);
	if (actor == NULL || (!actor->CheckIn() && !actor->IsLoading()))
	{
		return RE_PLAYER_NOT_IN_RAID;
	}


	/*
	//死亡自动复活
	if (pkPlayer->GetDead())
	{
		//切换到合适的位置
		LC_ActorAttributeMap* pkAttributeMap = pkServerPlayer->GetAttributeMap();
		if (pkAttributeMap != NULL)
		{
			attr_value_type lHP = pkAttributeMap->GetAttribute(ATT_MAX_HP);
			int lMP = (int)(pkAttributeMap->GetAttribute(ATT_MAX_MP));
			pkServerPlayer->Respawn(lHP, lMP);
			//返回复活请求
			MG_RLT_Respawn rltMsg;
			rltMsg.m_lPlayerID		= pkPlayer->GetID();
			rltMsg.m_iRespawnType	= MGPT_REQ_RESPAWN_ON_CURRENT_LOCATION;
			rltMsg.m_iHP			= lHP;
			rltMsg.m_iMP			= lMP;
			rltMsg.m_iReduceExp		= 0;
			pkServerPlayer->SendMsgToClient(MGPT_RLT_RESPAWN, &rltMsg);
		}

		CF_WorldMapList::DataEntry* pkMapEntry = SafeGetCSVFileDataEntryPtr<CF_WorldMapList>(GetMapResID());
		if (NULL == pkMapEntry)
		{
			return RE_FAIL;
		}
		if ( pkMapEntry->_iRespawnSafeTime > 0)
		{
			pkServerPlayer->ActiveSkillState(BAOHU_BUFF,pkMapEntry->_iRespawnSafeTime,0);
		}
	}
	*/


	//离开副本时清除身上的某些Buff
	LC_ActorStateMap* pkActorStateMap = pkPlayer->GetStateMap();
	if (pkActorStateMap)
	{
		pkActorStateMap->ClearStatesOnLeaveStory(LC_Helper::isTranscriptClass(GetMapLogicType()));
	}

	if (LC_Helper::isTranscriptClass(GetMapLogicType()))
	{
		pkServerPlayer->ClearTemporaryLocationAssetData();

		MG_RLT_QUIT_Transcription RltMsg;
		RltMsg.m_iResult		= RE_SUCCESS;
		RltMsg.m_lPlayerID		= pkPlayer->GetID();
		pkServerPlayer->SendMsgToClient(MGPT_RLT_QUIT_TRANSCRIPTION, &RltMsg);
		LC_ActionForbid* pa = pkServerPlayer->GetActionForbid();
		pa->RemoveActionForbid(LAFT_TRADE, LAF_DUEL_MAX_COUNT);
	}

	SetPlayerQuitState(pkServerPlayer);

	int cursize = GetAllRealPlayerSize();
	AdjustMonster(cursize + 1, cursize);
	ComputeAutoFitLevel();
	//打开玩家数据存储功能
	//pkServerPlayer->SetDBSaveMaskFlag(LPDST_CHAR_MAX_COUNT, true);
	pkServerPlayer->CancelCurrentSkill();

	//处理跨服
	//记录最后位置
	if (CheckGuildTransport(pkServerPlayer))
	{
		LC_ServerGuildManager* manager = (LC_ServerGuildManager*)LC_ServerGuildManager::GetSingletonPtr();
		LC_ServerGuildInfo* guild = manager->GetPlayerGuild(pkServerPlayer->GetInstance());
		if (guild != NULL)
		{
			guild->UpdateTransportLocation(pkServerPlayer);
		}
	}

	//处理传送
	GameStory::LC_StoryLogic_Manager* pkStoryManager = SERVER_GET_STORYLOGIC_MANAGER;
	pkStoryManager->PrePlayerLeaveMap(pkPlayer, this);

	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	bool isleave = false;
	if (bBack == true && (eInOutType == LMIOT_LEAVE || eInOutType == LMIOT_RESET))
	{
		bBack = false;
	}
	if (CheckAllPlayEnd())
	{
		isleave = enterLeaveManager->PlayerTryLeaveCrossRealm(pkServerPlayer, false, bBack);
	}
	else
	{
		isleave = enterLeaveManager->PlayerTryLeaveCrossRealm(pkServerPlayer, true, bBack);
	}
	if (isleave)
	{
		return RE_PLAYER_LEAVE_CURSERVER;
	}
	return RE_SUCCESS;
}

void LC_ServerMapLogic_Place::AllPlayerLeave()
{
	if (m_AutoFinish)
	{
		Finsh();
	}
}

void LC_ServerMapLogic_Place::PlayerEnterEvent(LC_ServerPlayer* player, int32_t playersize)
{
	MapLogicEvent kEvent(MapLogicNotifyEvent_PlayerEnter, player->GetID());
	//处理离开
	int index = 0;
	BeginReadPlay(index);
	PlayVector::iterator it;
	while (ReadOnePlay(it, index))
	{
		(*it)->EventNotify(kEvent);
	}
}

void LC_ServerMapLogic_Place::LetPlayerLeave(LC_ServerPlayer* pkPlayer)
{
	//回到正常地图
	if (LC_Helper::isTranscriptClass(GetMapLogicType()))
	{
		LC_ServerWorldManager*  pkWorldManager	= SERVER_GET_WORLD_MANAGER();
		pkWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_QUIT_TRANSCRIPTION, 0,	1,	UT_Vec3Int());
	}
	else
	{
		SC_ServerSkillInterface* pkSkillInterface = SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
		pkSkillInterface->ReturnToCity(pkPlayer);
	}
	return;
}

void LC_ServerMapLogic_Place::LetAllPlayerLeave()
{
	LC_ActorPtrList playerlist;
	GetAllRealPlayer(playerlist);
	LC_ActorPtrList::iterator pkActorListIter = playerlist.begin();
	for (; pkActorListIter != playerlist.end(); ++pkActorListIter)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*pkActorListIter);
		LetPlayerLeave(pkPlayer);
	}
}

void LC_ServerMapLogic_Place::ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent)
{
	switch (pkEvent->GetEventType())
	{
		case GLET_CLIENT_ALREADY_IN_GAME:
			{
				_receivedClientAlreadyInGame(pkEvent);
			}
			break;
		case GLET_PLAYER_LEAVE_GROUP:
			{
				_kickoffPlayerFromGroup(pkEvent);
			}
			break;
		case GLET_PLAYERGROUP_DISMISS:
			{
				_groupDismiss(pkEvent);
			}
			break;
		default:
			break;
	}
}

void LC_ServerMapLogic_Place::SetVaild(bool value)
{
	if(m_bVaild == value)
	{
		return;
	}

	m_bVaild = value;
}

bool LC_ServerMapLogic_Place::GetVaild()
{
	return m_bVaild;
}

bool	LC_ServerMapLogic_Place::_bCareOfEvent(GameLogic::LC_GameEvent* pkEvent)
{
	switch (pkEvent->GetEventType())
	{
	case GLET_CLIENT_ALREADY_IN_GAME:
	case GLET_PLAYER_LEAVE_GROUP:
	case GLET_PLAYERGROUP_DISMISS:
		{
			return true;
		}
		break;
	}
	return false;
}

void LC_ServerMapLogic_Place::ClearAllObjectsInMapWhenQuit()
{
	if (m_pkMap != NULL)
	{
		m_pkMap->ClearAllObjects();
	}
}

bool LC_ServerMapLogic_Place::CheckPosVaild(int x, int y)
{
	LC_MapBase* pkMap = GetMap();
	if (pkMap == NULL)
	{
		return false;
	}
	SectorIndex index = pkMap->ComputeSecctorIndex(x, y);
	LC_SectorBase* pkSector = pkMap->GetSector(index);
	if (pkSector == NULL)
	{
		return false;
	}
	return true;
}

bool LC_ServerMapLogic_Place::RandomPos(Utility::UT_Vec3Int& pos)
{
	PF_ServerPathManager* pathmanager = PF_ServerPathManager::GetSingletonPtr();
	return pathmanager->RandomVaildPoint(GetMapResID(), pos);
}

void LC_ServerMapLogic_Place::UpdateTaskBatchPlay(int taskID)
{
	MapLogicEvent kEvent(MapLogicNotifyEvent_FinishTask, MapLogicFinishEvent_FinishTask);
	kEvent.SetParam(0, taskID);
	int index = 0;
	BeginReadPlay(index);
	PlayVector::iterator playit;
	while (ReadOnePlay(playit, index))
	{
		(*playit)->EventNotify(kEvent);
	}
}


//todo::通知玩法
void LC_ServerMapLogic_Place::ActorGroupSizeChange(object_id_type organizeId, Group_Change_Map& changemap, object_id_type changeid)
{
	if (organizeId == (uint32_t)m_AllPlayerOrg)
	{
		Group_Change_Map::iterator it = changemap.find(Group_Change_Type_In);
		if (it != changemap.end())
		{
			if (it->second[1] <= 0)
			{
				AllPlayerLeave();
			}
		}
	}
	else if (organizeId == (uint32_t)m_AllMonsterOrg)
	{
		Group_Change_Map::iterator it = changemap.find(Group_Change_Type_In);
		if (it != changemap.end())
		{
			MapLogicEvent kEvent(MapLogicNotifyEvent_MonsterCount, changeid);
			kEvent.SetParam(0, it->second[1]);
			int index = 0;
			BeginReadPlay(index);
			PlayVector::iterator playit;
			while (ReadOnePlay(playit, index))
			{
				(*playit)->EventNotify(kEvent);
			}
		}
	}
	else if (isElite(organizeId)) //处理精英怪掉落
	{
		Group_Change_Map::iterator it = changemap.find(Group_Change_Type_In);
		if (it != changemap.end())
		{
			if (it->second[1] <= 0)
			{
				if (IS_NPC_ID(changeid))
				{
					LC_ServerNPC* pkServerNPC = m_ActorManger.GetNpc(changeid);
					if (pkServerNPC == NULL)
						return;

					_dropItem(pkServerNPC);
					_addCourageStateWhenEliteMonsterDie(pkServerNPC);

					LC_ServerNPCOwnerList& rkOwnerList = pkServerNPC->GetNPCOwnerList();
					if (rkOwnerList.GetOwnerCount())
					{
						object_id_type plyid = rkOwnerList.GetOwnerByIndex(0);
						
						LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
						LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
						if (pkGameEvent)
						{
							pkGameEvent->SetEventType(GLET_KILL_ELITE);
							pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,	GENT_SELF);
							pkGameEvent->SetEventParam(EP_SP_ADD_PLAYER_ID, plyid);
							pkGameEvent->SetEventParam(EP_KILL_ELITE_TYPE_ID,	pkServerNPC->GetCharType());
							pkGameEvent->SetEventParam(EP_KILL_ELITE_ID,	changeid);
							pkGameEvent->SetEventParam(EP_KILL_ELITE_ORGANIZE_ID,	organizeId);
							pkGameEvent->SetEventParam(EP_KILL_ELITE_LEVEL,	pkServerNPC->GetGroupEliteLevel());
							pkGameEvent->SetEventParam(EP_KILL_ELITE_MAP_ID,	pkServerNPC->GetMapResID());
							pkEventManager->InsertGameEvent(pkGameEvent);
						}
					}
				}
				m_Elites.erase(organizeId);
				m_ActorManger.UnOrganizeActor(organizeId);
			}
		}
	}
	else
	{
		Group_Change_Map::iterator it = changemap.find(Group_Change_Type_In);
		if (it != changemap.end())
		{
			MapLogicEvent kEvent(MapLogicNotifyEvent_Org, changeid);
			kEvent.SetParam(0, organizeId);
			kEvent.SetParam(1, it->second[1]);
			kEvent.SetParam(2, changeid);
			int index = 0;
			BeginReadPlay(index);
			PlayVector::iterator playit;
			while (ReadOnePlay(playit, index))
			{
				(*playit)->EventNotify(kEvent);
			}
		}
	}
}

//todo::通知玩法
void LC_ServerMapLogic_Place::PlayerChangeLogicArea(LC_ServerPlayer* player, int prearea, int curarea, int preAreaType, int curAreaType)
{
	MapLogicEvent kEvent(MapLogicNotifyEvent_LogicArea, player ? player->GetID() : INVALID_ACTOR_ID);
	kEvent.SetParam(0, curarea);
	MapLogicEvent kPlayerEvent(PlayerEvent_ChangeLogicArea, player ? player->GetID() : INVALID_ACTOR_ID);
	kPlayerEvent.SetParam(0, prearea);
	kPlayerEvent.SetParam(1, curarea);
	kPlayerEvent.SetParam(2, preAreaType);
	kPlayerEvent.SetParam(3, curAreaType);
	int index = 0;
	BeginReadPlay(index);
	PlayVector::iterator playit;
	while (ReadOnePlay(playit, index))
	{
		(*playit)->EventNotify(kEvent);
		(*playit)->PlayerEventNotify(player, kPlayerEvent);
	}
	LC_ServerMapLogicBase::PlayerChangeLogicArea(player, prearea, curarea);
}

void LC_ServerMapLogic_Place::PlayerBeKilled(LC_ServerPlayer* pkServerPlayer, object_id_type lSrcActorID)
{
	LC_ServerMapLogicBase::PlayerBeKilled(pkServerPlayer, lSrcActorID);
	LC_ServerPlayer* pOwnerPlayer = pkServerPlayer->GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
		return;
	//LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(pkServerPlayer->GetID());
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(pOwnerPlayer->GetID());
	if (actor != NULL && actor->CheckIn())
	{
		m_logicRecord.RecordPlayer(1);
	}

	if(/*NULL!=pOwnerPlayer &&*/ pOwnerPlayer->CheckOwnerDead())
	{
		MapLogicEvent kPlayerEvent(PlayerEvent_Death, pOwnerPlayer->GetID());
		kPlayerEvent.SetParam(0, lSrcActorID);
		int index = 0;
		BeginReadPlay(index);
		PlayVector::iterator playit;
		while (ReadOnePlay(playit, index))
		{
			(*playit)->PlayerEventNotify(pOwnerPlayer, kPlayerEvent);
		}
	}
}

void LC_ServerMapLogic_Place::NPCDead(LC_ServerNPC* pkServerNPC)
{
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(pkServerNPC->GetID());
	if (actor != NULL && actor->CheckIn() && GetActorManger().GetActorInOrganize(pkServerNPC->GetID(), m_AllMonsterOrg) != NULL)
	{
		m_logicRecord.RecordMonster(1);
	}
	//容器处理
	GetActorManger().DelActor(pkServerNPC->GetID());
	LC_ServerMapLogicBase::NPCDead(pkServerNPC);

	LC_NPCManagerBase* const pkNPCManager = ENGINE_GET_NPC_MANAGER();

	bool clearNpc = pkServerNPC->GetClearEpigoneFlag();
	if(clearNpc)
	{
		//删除pkServerNPC招出来的小怪
		const mem::vector<object_id_type>& epigoneList = pkServerNPC->GetEpigoneList();
		for(int i=0; i<epigoneList.size(); ++i)
		{
			object_id_type lNPCID = epigoneList[i];
			LC_NPCBase* pkNPC = pkNPCManager->FindNPC(lNPCID);
			if (pkNPC != NULL)
			{
				pkNPC->SetHP(0);
			}
		}
	}
}

void LC_ServerMapLogic_Place::DropItemsOnNPCDead(LC_ServerNPC* pkServerNPC)
{
	if (isInElite(pkServerNPC))
	{
		return;
	}
	//LC_Helper::isTranscriptClass(GetMapLogicType());
	_dropItem(pkServerNPC);
	LC_ServerMapLogicBase::DropItemsOnNPCDead(pkServerNPC);
}

bool LC_ServerMapLogic_Place::ComputeAutoFitLevel(bool changeAtOnce)
{
	if (!m_Autofit)
	{
		return false;
	}
	GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();

	int iTotalLevelSum		=	0;
	int iPlayerNum			=	0;
	int iMaxPlayerLevel		=	0;
	int iMinPlayerLevel		=	999999;

	if (IS_AUTOFIT_WITHIN_PLAYER(m_AutofitType))
	{
		GameLogic::LC_ActorPtrList playerlist;
		GetAllRealPlayer(playerlist);
		if (playerlist.empty())
		{
			m_AutofitLevel = 0;
			return true;
		}

		GameLogic::LC_ActorPtrList::iterator iter = playerlist.begin();
		for (; iter != playerlist.end(); ++iter)
		{
			int iPlayerTrueLevel = (*iter)->GetLevel();

			if (iPlayerTrueLevel >= iMaxPlayerLevel)
			{
				iMaxPlayerLevel = iPlayerTrueLevel;
			}

			if (iPlayerTrueLevel <= iMinPlayerLevel)
			{
				iMinPlayerLevel = iPlayerTrueLevel;
			}

			iTotalLevelSum += iPlayerTrueLevel;
			++iPlayerNum;
		}
	}

	switch(m_AutofitType)
	{
	case AUTOFIT_TYPE_UP:
		{
			m_AutofitLevel = iMaxPlayerLevel;
		}
		break;
	case AUTOFIT_TYPE_NORMAL:
		{
			m_AutofitLevel = iTotalLevelSum / UT_MathBase::MaxInt(1, iPlayerNum);
		}
		break;
	case AUTOFIT_TYPE_DOWN:
		{
			m_AutofitLevel = iMinPlayerLevel;
		}
		break;
	case AUTOFIT_TYPE_WORLD:
		{
			m_AutofitLevel = pkServerMgr->GetCrossServerLevel(pkServerMgr->GetCurServerId());
		}
		break;
	case AUTOFIT_TYPE_CROSS_AVG:
		{
			m_AutofitLevel = GetGameServerApp()->GetServerManager()->GetCrossAverageLevel();
		}
		break;
	default:
		m_AutofitLevel = 0;
		break;
	}

	if (changeAtOnce || (!IS_COMMON_MAP(m_eMapLogicType) && IS_AUTOFIT_WITHIN_PLAYER(m_AutofitType)))
	{
		//调整血量
		GameLogic::LC_ActorPtrList pkNPCList;
		mem::set<int> setA, setB;
		GetAllMonster(pkNPCList, setA, setB);
		if (!pkNPCList.empty())
		{
			GameLogic::LC_ActorPtrList::iterator beg = pkNPCList.begin();
			for (; beg != pkNPCList.end(); ++beg)
			{
				((LC_ServerNPC*)(*beg))->AutoFitLevelReInit(m_AutofitLevel);
			}
		}	
	}
	return true;
}

void LC_ServerMapLogic_Place::AdjustMonster(int presize, int cursize)
{
	if (!GetAutoChangeMonsterHp())
	{
		return;
	}
	//调整血量
	GameLogic::LC_ActorPtrList pkNPCList;
	mem::set<int> setA, setB;
	GetAllMonster(pkNPCList, setA, setB);
	if (!pkNPCList.empty())
	{
		GameLogic::LC_ActorPtrList::iterator beg = pkNPCList.begin();
		for (; beg != pkNPCList.end(); ++beg)
		{
			adjust_Npc_Hp_Dm_by_playersize(((LC_ServerNPC*)(*beg)), presize, cursize);
		}
	}
}

void LC_ServerMapLogic_Place::GetDropItemPartakeKillNpcReward(LC_ServerNPC* pkServerNPC, std::map<int, int>& dropItems)
{
	LC_ServerItemFlopManager* pkServerItemFlopManager = LC_ServerItemFlopManager::GetSingletonPtr();

	pkServerItemFlopManager->GenerateRewaredOnNPCDeadPartakeKillNpcReward(pkServerNPC, dropItems);
}

bool LC_ServerMapLogic_Place::CheckPlayerActive(object_id_type playerid)
{
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(playerid, m_AllPlayerOrg);
	return actor && actor->IsActive();
}
bool LC_ServerMapLogic_Place::CheckPlayerActiveOrLoading(object_id_type playerid)
{
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(playerid, m_AllPlayerOrg);
	return actor&&actor->IsActiveOrLoading();
}

bool LC_ServerMapLogic_Place::CheckPlayerIn(object_id_type playerid)
{
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(playerid, m_AllPlayerOrg);
	return actor && actor->CheckIn();
}
bool LC_ServerMapLogic_Place::CheckPlayerPreIn(object_id_type playerid)
{
	return NULL!= GetActorManger().GetActorInOrganize(playerid, m_AllPlayerOrg);
}

void LC_ServerMapLogic_Place::GetAllRealPlayer(GameLogic::LC_ActorPtrList& playerlist)
{
	LC_PlayerManagerBase* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerMapLogic_ActorManger& kActorMgr = GetActorManger();
	const object_id_set* allactor = kActorMgr.GetAllActorIDsInOrganize(m_AllPlayerOrg);
	if (NULL == allactor || allactor->empty())
	{
		return;
	}

	object_id_set::const_iterator beg = allactor->begin();
	for (; beg != allactor->end(); ++beg)
	{
		LC_ServerMapLogic_Actor* pkActor = kActorMgr.GetActor(*beg);
		if (pkActor && pkActor->CheckIn())
		{
			LC_PlayerBase* theplayer = pkPlayerManager->FindPlayer(*beg);
			if (theplayer)
			{
				playerlist.push_back(theplayer);
			}
		}
	}
}

void LC_ServerMapLogic_Place::GetAllRealPlayer(unique_id_vector& playerlist)
{
	playerlist.clear();
	LC_PlayerManagerBase* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerMapLogic_ActorManger& kActorMgr = GetActorManger();
	const object_id_set* allactor = kActorMgr.GetAllActorIDsInOrganize(m_AllPlayerOrg);
	if (NULL == allactor || allactor->empty())
	{
		return;
	}

	object_id_set::const_iterator beg = allactor->begin();
	for (; beg != allactor->end(); ++beg)
	{
		LC_ServerMapLogic_Actor* pkActor = kActorMgr.GetActor(*beg);
		if (pkActor && pkActor->CheckIn())
		{
			LC_PlayerBase* theplayer = pkPlayerManager->FindPlayer(*beg);
			if (theplayer != NULL)
			{
				playerlist.push_back(theplayer->GetInstance());
			}
		}
	}
}

int LC_ServerMapLogic_Place::GetAllRealPlayerSize()
{
	return GetActorManger().GetInActorSizeInOrganize(m_AllPlayerOrg);
}

int	LC_ServerMapLogic_Place::GetAllRealActivePlayerSize()
{
	return GetActorManger().GetActiveActorSizeInOrganize(m_AllPlayerOrg);
}

int LC_ServerMapLogic_Place::GetAllControllerPlayerSize()
{
	LC_MapBase* themap = GetMap();
	if (themap == NULL)
	{
		return 0;
	}
	return themap->GetAllControllerPlayerSize();
}

LC_PlayerIDPtrMap*	LC_ServerMapLogic_Place::GetControllerPlayerMapPtr()
{
	LC_MapBase* themap = GetMap();
	if (themap == NULL)
	{
		return NULL;
	}
	
	return themap->GetControllerPlayerMapPtr();
}

void LC_ServerMapLogic_Place::GetAllPlayer(GameLogic::LC_ActorPtrList& playerlist)
{
	LC_MapBase* themap = GetMap();
	if (themap == NULL)
	{
		return;
	}
	themap->GetAllPlayer(playerlist);
}

//人员的阵营如何管理
LC_ServerPlayer* LC_ServerMapLogic_Place::SpawnPlayer(const unique_id_impl& charID, const Utility::UT_Vec3Int& pos, uint32_t bits, LC_ServerPlayer* owner, on_summonfinish_handler finishhandle, FactionType faction)
{
	//刷出对方
	LC_ServerPlayerManager* pkServerPlayerMgr = SERVER_GET_PLAYER_MANAGER();
	bool isRobot = (IS_SOLODULE_ROBOT_PLAYERID(charID)) ? true : false;
	user_id_type userID = charID.detail();
	if(isRobot)
		userID = charID;
	LC_ServerPlayer* theplayer = pkServerPlayerMgr->SummorControlPlayer(charID, GetMapLogicID(), pos, bits, isRobot,NULL,userID);
	if (theplayer == NULL)
		return NULL;
	if (finishhandle != NULL)
	{
		theplayer->SetSummonFinishHandle(finishhandle);
	}
	if (owner != NULL)
	{
		theplayer->SetFollowedTargetID(owner->GetID());
	}
	GetActorManger().AddActor(theplayer->GetID(), Logic_Actor_State_Loading);
	GetActorManger().AddActor2Organize(theplayer->GetID(), m_AllPlayerOrg);
	if (faction != FACTION_NONE)
	{
		AddActor2Faction(theplayer, faction);
		return theplayer;
	}
	//交给玩法确定
	//玩法无法确定加入默认阵营
	AddActor2Faction(theplayer, FACTION_PLAYER);
	theplayer->SetPlaceCanPK(GetCanPK());
	return theplayer;
}

LC_ServerPlayer* LC_ServerMapLogic_Place::SpawnBuddyPlayer(object_id_type playerid, const unique_id_impl& charID, const Utility::UT_Vec3Int& pos, uint32_t bits, LC_ServerPlayer* owner, on_summonfinish_handler finishhandle, FactionType faction)
{
	//刷出Buddy
	LC_ServerPlayerManager* pkServerPlayerMgr = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* theplayer = pkServerPlayerMgr->SummorControlPlayer(charID, GetMapLogicID(), pos, bits, false, owner);
	if (theplayer == NULL)
		return NULL;
	
	if (finishhandle != NULL)
	{
		theplayer->SetSummonFinishHandle(finishhandle);
	}
	if (owner != NULL)
	{
		theplayer->SetFollowedTargetID(owner->GetID());
	}
	GetActorManger().AddActor(theplayer->GetID(), Logic_Actor_State_Loading);
	GetActorManger().AddActor2Organize(theplayer->GetID(), m_AllPlayerOrg);
	if (faction != FACTION_NONE)
	{
		AddActor2Faction(theplayer, faction);
		return theplayer;
	}
	//交给玩法确定
	//玩法无法确定加入默认阵营
	AddActor2Faction(theplayer, FACTION_PLAYER);
	theplayer->SetPlaceCanPK(GetCanPK());
	return theplayer;
}

void LC_ServerMapLogic_Place::ClearPlayer(LC_ServerPlayer* player)
{
	if (player->IsInstance())
	{
		return;
	}
	GetActorManger().DelActor(player->GetID());
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	pkWorldManager->RemoveObjectFromMap(player);
	pkPlayerManager->DestroyPlayer(player);
}

GameLogic::LC_ActorPtrList LC_ServerMapLogic_Place::SpawnEliteNPC(GameLogic::LC_TranscriptionListEntry& entry, int index, EliteSkillRuleSetType& skillrule_list, int autofitlevel, FactionType faction, int32_t iElite)
{
	GameLogic::LC_ActorPtrList npclist;
	CF_CharType* pkCharType = CF_CharType::GetSingletonPtr();
	CF_CharType::DataEntry* pkData = pkCharType->GetEntryPtr(entry.m_iSpawnCharType);
	if (pkData == NULL)
	{
		return npclist;
	}
	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkSpawnManager = pkLogicManager->GetSpawnManager();

	entry.m_iMapID = GetMapLogicID();
	int elite_level = (iElite>=(ELT_STAR_MID_MIN+1)) ? iElite : UT_MathBase::RandInRangeInt(ELT_BLUE_MID+1, pkData->_lElite);
	EliteMonsterParam* param = DungeonConfigDataManager::GetSingletonPtr()->GetParamManager().GetEliteMonsterParam(elite_level);
	if (param == NULL)
	{
		return npclist;
	}
	if (autofitlevel <= 0)
	{
		if (m_Autofit && /*m_AutofitLevel > 0 &&*/ CheckMonster((FactionType)pkData->_lFaction))
		{
			autofitlevel = entry.CalculateAutoFitLevel(m_AutofitLevel);
		}
	}
	else
	{
		autofitlevel = entry.CalculateAutoFitLevel(autofitlevel);
	}
	int skill_count = 0; //怪物技能数量
	if (autofitlevel > 0) //自适应怪
	{
		AutoFitNPCManager* inst = AutoFitNPCManager::GetSingletonPtr();
		inst->AutoFitLevel(entry.m_iSpawnCharType, autofitlevel);
		CF_AutoFitCharType::DataEntry* pkData = inst->FindData(entry.m_iSpawnCharType, autofitlevel);
		if (pkData == NULL)
		{
			return npclist;
		}
		if (elite_level == ELT_BLUE_MID+1)
		{
			skill_count = pkData->_lEliteSkillCount1;
		}
		else
		{
			skill_count = pkData->_lEliteSkillCount2;
		}
	}
	else
	{
		if (elite_level == ELT_BLUE_MID+1)
		{
			skill_count = pkData->_lEliteSkillCount1;
		}
		else
		{
			skill_count = pkData->_lEliteSkillCount2;
		}
	}
	LC_TranscriptionListEntry transEntry;
	transEntry.m_iMapID = GetMapLogicID();
	int count = param->m_GroupCount;
	mem::vector<Utility::UT_Vec3Int> poslist;
	EliteExternSkillListType skill_list;
	if (skillrule_list.empty())
	{
		param->RandomSkill(skill_list);
	}
	else
	{
		//筛选出可以使用的技能
		EliteSkillRuleSetType rulelist;
		EliteSkillRuleSetType::iterator skillbeg = skillrule_list.begin();
		for (; skillbeg != skillrule_list.end(); ++skillbeg)
		{
			GameLogic::LC_SkillTriggerEntry* entry = DungeonConfigDataManager::GetSingletonPtr()->GetParamManager().GetSkillTrigger(elite_level, skillbeg->SkillRuleID);
			if (entry != NULL)
			{
				rulelist.push_back(*skillbeg);
			}
		}
		//插入技能
		int weightcount = 0;
		skillbeg = rulelist.begin();
		for (; skillbeg != rulelist.end(); ++skillbeg)
		{
			weightcount += skillbeg->Rate;
		}
		EliteSkillRuleSetType copyskilldata = rulelist;
		int needcount = skill_count; //param->m_SkillCount;
		while (weightcount > 0 && needcount > 0)
		{
			int randomweight = UT_MathBase::RandInRangeInt(1, weightcount);
			EliteSkillRuleSetType::iterator skillcopybeg = copyskilldata.begin();
			int tempcount = 0;
			while (skillcopybeg != copyskilldata.end())
			{
				if (randomweight <= tempcount)
				{
					break;
				}
				tempcount += skillcopybeg->Rate;
				++skillcopybeg;
			}
			if (skillcopybeg == copyskilldata.end())
			{
				skillcopybeg = copyskilldata.end() - 1;
			}
			GameLogic::LC_SkillTriggerEntry* entry = DungeonConfigDataManager::GetSingletonPtr()->GetParamManager().GetSkillTrigger(elite_level, skillcopybeg->SkillRuleID);
			if (entry != NULL)
			{
				skill_list.push_back(entry);
			}
			--needcount;
			weightcount -= skillcopybeg->Rate;
			copyskilldata.erase(skillcopybeg);
		}
	}

	int i = 0;
	int MinorCount = param->m_MinorCount;
	for (EliteExternSkillListType::iterator skill_iter = skill_list.begin(); skill_iter != skill_list.end() && i < skill_count; ++skill_iter)
	{
		GameLogic::LC_SkillTriggerEntry* entry = *skill_iter;
		if (entry->GetSkillTypeID() == 21914018)
		{
			MinorCount += 3;
		}
		++i;
	}
	i = 0;
	GenRandomPosition(entry.m_kSpawnLocation, poslist, index + count + MinorCount, GetMapResID());
	while (i < count)
	{
		transEntry.m_kSpawnLocation		= poslist[index + i];
		transEntry.m_iSpawnCharType		= entry.m_iSpawnCharType;
		transEntry.m_iWanderRadius  = 500;
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkSpawnManager->ManualSpawnNPC(transEntry);
		if (pkNPC == NULL)
		{
			++i;
			GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "The process of applying for NPC ID returns an error value, and duplicate data appears");
			continue;
		}
		if (autofitlevel > 0)
		{
			pkNPC->AutoFitLevelReInit(autofitlevel);
		}

		Upgrade_Elite_Monster(pkNPC, elite_level, false, param, skill_list, skill_count);
		GetActorManger().AddActor(pkNPC->GetID(), Logic_Actor_State_ACTIVE, entry.m_iSpawnCharType);
		if (faction != FACTION_NONE)
		{
			AddActor2Faction(pkNPC, faction);
			if (CheckMonster(faction))
			{
				GetActorManger().AddActor2Organize(pkNPC->GetID(), m_AllMonsterOrg);
			}
		}
		else
		{
			AddActor2Faction(pkNPC, (FactionType)pkNPC->GetFactionID());
			if (CheckMonster((FactionType)pkNPC->GetFactionID()))
			{
				GetActorManger().AddActor2Organize(pkNPC->GetID(), m_AllMonsterOrg);
			}
		}
		npclist.push_back(pkNPC);
		++i;
	}
	while (i < count + MinorCount)
	{
		transEntry.m_kSpawnLocation		= poslist[index + i];
		transEntry.m_iSpawnCharType		= entry.m_iSpawnCharType;
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkSpawnManager->ManualSpawnNPC(transEntry);
		if (pkNPC == NULL)
		{
			++i;
			GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "The process of applying for NPC ID returns an error value, and duplicate data appears");
			continue;
		}
		if (autofitlevel > 0)
		{
			pkNPC->AutoFitLevelReInit(autofitlevel);
		}
		Upgrade_Elite_Monster(pkNPC, elite_level, true, param, skill_list, skill_count);
		GetActorManger().AddActor(pkNPC->GetID(), Logic_Actor_State_ACTIVE, entry.m_iSpawnCharType);
		if (faction != FACTION_NONE)
		{
			AddActor2Faction(pkNPC, faction);
			if (CheckMonster(faction))
			{
				GetActorManger().AddActor2Organize(pkNPC->GetID(), m_AllMonsterOrg);
			}
		}
		else
		{
			AddActor2Faction(pkNPC, (FactionType)pkNPC->GetFactionID());
			if (CheckMonster((FactionType)pkNPC->GetFactionID()))
			{
				GetActorManger().AddActor2Organize(pkNPC->GetID(), m_AllMonsterOrg);
			}
		}
		npclist.push_back(pkNPC);
		++i;
	}
	object_id_vector ActorIds;
	GameLogic::LC_ActorPtrList::iterator npcbeg = npclist.begin();
	for (; npcbeg != npclist.end(); ++npcbeg)
	{
		ActorIds.push_back((*npcbeg)->GetID());
	}
	LC_ServerMapLogic_Organize* orz = GetActorManger().OrganizeActor(ActorIds);
	if (orz != NULL)
	{
		m_Elites.insert(orz->Organize_id);
	}
	return npclist;
}

bool LC_ServerMapLogic_Place::isElite(int innerid)
{
	EliteMap::iterator it = m_Elites.find(innerid);
	if (it == m_Elites.end())
	{
		return false;
	}
	return true;
}

bool LC_ServerMapLogic_Place::isInElite(LC_ServerNPC* pkServerNPC)
{
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(pkServerNPC->GetID());
	if (actor == NULL)
	{
		return false;
	}
	LC_ServerMapLogic_Actor::OrganizeSet::iterator beg = actor->organizeIds.begin();
	for (; beg != actor->organizeIds.end(); ++beg)
	{
		if (isElite(*beg))
		{
			return true;
		}
	}
	return false;
}

int LC_ServerMapLogic_Place::GetEliteCount(LC_ServerNPC* pkServerNPC)
{
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(pkServerNPC->GetID());
	if (actor == NULL)
	{
		return 0;
	}
	LC_ServerMapLogic_Actor::OrganizeSet::iterator beg = actor->organizeIds.begin();
	for (; beg != actor->organizeIds.end(); ++beg)
	{
		if (isElite(*beg))
		{
			return GetActorManger().GetAllActorSizeInOrganize(*beg);
		}
	}
	return 0;
}

LC_ServerNPC* LC_ServerMapLogic_Place::GetInEliteNpc(LC_ServerNPC* pkServerNPC, int index)
{
	if (index < 0)
	{
		return NULL;
	}
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(pkServerNPC->GetID());
	if (NULL == actor)
	{
		return NULL;
	}
	LC_ServerMapLogic_Actor::OrganizeSet::iterator beg = actor->organizeIds.begin();
	for (; beg != actor->organizeIds.end(); ++beg)
	{
		if (isElite(*beg))
		{
			const object_id_set* npcids = GetActorManger().GetAllActorIDsInOrganize(*beg);
			if (NULL == npcids || npcids->empty())
			{
				continue;
			}
			object_id_set::const_iterator cit = npcids->begin();
			for (; cit != npcids->end() && index > 0; ++cit, --index);
			if (cit != npcids->end())
			{
				return GetActorManger().GetNpc(*cit);
			}
			else
			{
				return NULL;
			}
		}
	}
	return NULL;
}

LC_ServerNPC* LC_ServerMapLogic_Place::SpawnNPC(GameLogic::LC_TranscriptionListEntry& transEntry, int autofitlevel, FactionType faction )
{
	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkSpawnManager = pkLogicManager->GetSpawnManager();

	transEntry.m_iMapID = GetMapLogicID();
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkSpawnManager->ManualSpawnNPC(transEntry);
	if (!pkNPC)
	{
		GfxWriteLog(LOG_SYSTEM_FILELINE, LOG_SWITCH_SYSTEM, "The process of applying for NPC ID returns an error value, and duplicate data appears");
		return NULL;
	}

	//公会秘境地图
	uint64_t guildID = GetGuildID();
	if(INVALID_GUILD_ID!=guildID && MT_GUILD_MAP==GetMapLogicType())
	{
		int64_t hp = UT_ServerHelper::GetGuildMapBossHP(GetMapResID(), guildID, pkNPC->GetCharType());
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GuildMapPlace create npc! mapID:%d, guildID:%lld, bossID:%d, HP:%lld", GetMapResID(), guildID, pkNPC->GetCharType(), hp);
		if(0 != hp)
		{
			pkNPC->SetHP(hp);
		}
	}

	if (autofitlevel > 0)
	{
		pkNPC->AutoFitLevelReInit(transEntry.CalculateAutoFitLevel(autofitlevel));
	}
	else if (m_Autofit && /*m_AutofitLevel > 0 &&*/ CheckMonster((FactionType)pkNPC->GetFactionID()))
	{
		pkNPC->AutoFitLevelReInit(transEntry.CalculateAutoFitLevel(m_AutofitLevel));
	}
	if (GetAutoChangeMonsterHp())
	{
		int cursize = GetAllRealPlayerSize();
		adjust_Npc_Hp_Dm_by_playersize(pkNPC, 0, cursize);
	}
	GetActorManger().AddActor(pkNPC->GetID(), Logic_Actor_State_ACTIVE, transEntry.m_iSpawnCharType);
	if (faction != FACTION_NONE)
	{
		AddActor2Faction(pkNPC, faction);
		if (CheckMonster(faction))
		{
			GetActorManger().AddActor2Organize(pkNPC->GetID(), m_AllMonsterOrg);
		}
	}
	else
	{
		AddActor2Faction(pkNPC, (FactionType)pkNPC->GetFactionID());
		if (CheckMonster((FactionType)pkNPC->GetFactionID()))
		{
			GetActorManger().AddActor2Organize(pkNPC->GetID(), m_AllMonsterOrg);
		}
	}
	return pkNPC;
}
///////////////////////////////////////////////////////////////////////////////////
struct FilterNPCOP: public LC_ActorOperator
{
public:
	FilterNPCOP(GameLogic::LC_ActorPtrList& npcList, mem::set<int>& npctype, mem::set<int>& factiontype):_npcList(npcList), _npctype(npctype), _factiontype(factiontype) {}

	virtual bool operator()(LC_ActorBase* pkNPC)
	{
		if (pkNPC)
		{
			if (!_npctype.empty() && _npctype.count(pkNPC->GetCharType()) <= 0)
			{
				return false;
			}
			if (!_factiontype.empty() && _factiontype.count(pkNPC->GetFactionID()) <= 0)
			{
				return false;
			}
			_npcList.push_back(pkNPC);
			return true;
		}
		return false;
	}
public:
	GameLogic::LC_ActorPtrList& _npcList;
	mem::set<int>&				_npctype;
	mem::set<int>&				_factiontype;
};
///////////////////////////////////////////////////////////////////////////////////
void LC_ServerMapLogic_Place::GetAllNPC(GameLogic::LC_ActorPtrList& pkNPCList, mem::set<int>& npctype, mem::set<int>& factiontype)
{
	LC_MapBase* themap = GetMap();
	if (themap == NULL)
	{
		return;
	}
	if (npctype.empty() && factiontype.empty())
	{
		themap->GetAllNPC(pkNPCList);
	}
	else
	{
		FilterNPCOP kOP(pkNPCList, npctype, factiontype);
		themap->ForAllNPC(&kOP);
	}
}

void LC_ServerMapLogic_Place::ClearAllNPC(mem::set<int>& npctype, mem::set<int>& factiontype)
{
	GameLogic::LC_ActorPtrList allnpc;
	GetAllNPC(allnpc, npctype, factiontype);
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager		= ENGINE_GET_NPC_MANAGER();
	GameLogic::LC_ActorPtrList::iterator beg = allnpc.begin();
	for (; beg != allnpc.end(); ++beg)
	{
		GetActorManger().DelActor((*beg)->GetID());
		pkWorldManager->RemoveObjectFromMap(*beg);
		pkNPCManager->DestoryNPC((GameLogic::LC_NPCBase*)(*beg));
	}
}

void LC_ServerMapLogic_Place::ClearNPC(LC_ServerNPC* npc)
{
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager		= ENGINE_GET_NPC_MANAGER();
	GetActorManger().DelActor(npc->GetID());
	pkWorldManager->RemoveObjectFromMap(npc);
	pkNPCManager->DestoryNPC(npc);
}
///////////////////////////////////////////////////////////////////////////////////
struct CountNPCOP: public LC_ActorOperator
{
public:
	CountNPCOP(mem::set<int>& npctype, mem::set<int>& factiontype): _npctype(npctype), _factiontype(factiontype), _count(0) {}

	virtual bool operator()(LC_ActorBase* pkNPC)
	{
		if (pkNPC)
		{
			if (!_npctype.empty() && _npctype.count(pkNPC->GetCharType()) <= 0)
			{
				return false;
			}
			if (!_factiontype.empty() && _factiontype.count(pkNPC->GetFactionID()) <= 0)
			{
				return false;
			}

			++_count;
			return true;
		}
		return false;
	}
public:
	mem::set<int>&		_npctype;
	mem::set<int>&		_factiontype;
	int					_count;
};
///////////////////////////////////////////////////////////////////////////////////
int	LC_ServerMapLogic_Place::GetAllNPCSize(mem::set<int>& npctype, mem::set<int>& factiontype)
{
	LC_MapBase* themap = GetMap();
	if (themap == NULL)
	{
		return 0;
	}

	if (npctype.empty() && factiontype.empty())
	{
		return themap->GetAllNPCSize();
	}
	else
	{
		CountNPCOP kOP(npctype, factiontype);
		themap->ForAllNPC(&kOP);
		return kOP._count;
	}
}

void LC_ServerMapLogic_Place::GetAllMonster(GameLogic::LC_ActorPtrList& pkNPCList, mem::set<int>& npctype, mem::set<int>& factiontype)
{
	LC_NPCManagerBase* pkNPCManager = SERVER_GET_NPC_MANAGER();
	LC_ServerMapLogic_ActorManger& kActorMgr = GetActorManger();
	const object_id_set* allactor = kActorMgr.GetAllActorIDsInOrganize(m_AllMonsterOrg);
	if (NULL == allactor || allactor->empty())
	{
		return;
	}
	object_id_set::const_iterator beg = allactor->begin();
	for (; beg != allactor->end(); ++beg)
	{
		LC_ServerMapLogic_Actor* pkActor = kActorMgr.GetActor(*beg);
		if (pkActor && pkActor->CheckIn())
		{
			LC_ServerNPC* thenpc = (LC_ServerNPC*)pkNPCManager->FindNPC(*beg);
			if (NULL == thenpc)
			{
				continue;
			}
			if (!npctype.empty() && npctype.count(thenpc->GetCharType()) <= 0)
			{
				continue;
			}
			if (!factiontype.empty() && factiontype.count(thenpc->GetFactionID()) <= 0)
			{
				continue;
			}
			pkNPCList.push_back(thenpc);
		}
	}
}

void LC_ServerMapLogic_Place::ClearAllMonster(mem::set<int>& npctype, mem::set<int>& factiontype)
{
	GameLogic::LC_ActorPtrList allnpc;
	GetAllMonster(allnpc, npctype, factiontype);
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager		= ENGINE_GET_NPC_MANAGER();
	GameLogic::LC_ActorPtrList::iterator beg = allnpc.begin();
	for (; beg != allnpc.end(); ++beg)
	{
		GetActorManger().DelActor((*beg)->GetID());
		pkWorldManager->RemoveObjectFromMap(*beg);
		pkNPCManager->DestoryNPC((GameLogic::LC_NPCBase*)(*beg));
	}
}

void LC_ServerMapLogic_Place::ClearAllMonster()
{
	GameLogic::LC_ActorPtrList allnpc;
	GetAllMonster(allnpc);
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager		= ENGINE_GET_NPC_MANAGER();
	GameLogic::LC_ActorPtrList::iterator beg = allnpc.begin();
	for (; beg != allnpc.end(); ++beg)
	{
		GetActorManger().DelActor((*beg)->GetID());
		pkWorldManager->RemoveObjectFromMap(*beg);
		pkNPCManager->DestoryNPC((GameLogic::LC_NPCBase*)(*beg));
	}
}

int	LC_ServerMapLogic_Place::GetAllMonsterSize(mem::set<int>& npctype, mem::set<int>& factiontype)
{
	if (npctype.empty() && factiontype.empty())
	{
		return GetActorManger().GetInActorSizeInOrganize(m_AllMonsterOrg);
	}
	else
	{
		GameLogic::LC_ActorPtrList allnpc;
		GetAllMonster(allnpc, npctype, factiontype);
		return (int)allnpc.size();
	}
}

static FactionType MonsterFaction[MONSTER_FACTION_SIZE] = {FACTION_EVIL, FACTION_RAIDEVIL, FACTION_DEF_MONSTER};
bool LC_ServerMapLogic_Place::CheckMonster(FactionType faction)
{
	for (int i = 0; i < MONSTER_FACTION_SIZE; ++i)
	{
		if (faction == MonsterFaction[i])
		{
			return true;
		}
	}
	return false;
}

void LC_ServerMapLogic_Place::GetAllMonster(GameLogic::LC_ActorPtrList& pkNPCList)
{
	m_pkMap->GetAllNPC(pkNPCList);
}

bool LC_ServerMapLogic_Place::SpanDoor(DungeonConfig::DoorEntry& door)
{
	LC_ServerLogicManager* pkLogicManager = SERVER_GET_LOGIC_MANAGER();
	GameLogic::LC_SpawnManagerBase* pkSpawnManager = pkLogicManager->GetSpawnManager();

	if (door.InitState < (int)DoorState_Min || door.InitState > (int)DoorState_Max)
	{
		return false;
	}
	LC_ServerMapLogic_Door thedoor(door);
	LC_TranscriptionListEntry transEntry;
	transEntry.m_iMapID = GetMapLogicID();
	transEntry.m_kSpawnLocation		= door.Position;
	transEntry.m_iSpawnCharType		= door.TypeId;
	transEntry.m_iZRotAngle = door.Angel;
	LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkSpawnManager->ManualSpawnNPC(transEntry);
	if (pkNPC == NULL)
	{
		return false;
	}
	GetActorManger().AddActor(pkNPC->GetID(), Logic_Actor_State_ACTIVE, transEntry.m_iSpawnCharType);
	AddActor2Faction(pkNPC, FACTION_NEUTRAL);
	thedoor.m_ID = pkNPC->GetID();
	m_DoorVector.push_back(thedoor);
	//设置门的状态
	return m_DoorVector.back().SetDoorState(thedoor.m_State);
}

LC_ServerMapLogic_Door* LC_ServerMapLogic_Place::GetDoorById(int doorid)
{
	DoorVector::iterator beg = m_DoorVector.begin();
	for (; beg != m_DoorVector.end(); ++beg)
	{
		if (beg->m_ID == (uint32_t)doorid)
		{
			return &(*beg);
		}
	}
	return NULL;
}

LC_ServerMapLogic_Door* LC_ServerMapLogic_Place::GetDoorByIndex(int index)
{
	if (index < 0 || index >= GetAllDoorSize())
	{
		return NULL;
	}
	return &m_DoorVector[index];
}

mem::vector<LC_ServerMapLogic_Door*> LC_ServerMapLogic_Place::GetAllDoor()
{
	mem::vector<LC_ServerMapLogic_Door*> doors;
	DoorVector::iterator beg = m_DoorVector.begin();
	for (; beg != m_DoorVector.end(); ++beg)
	{
		doors.push_back(&(*beg));
	}
	return doors;
}

void LC_ServerMapLogic_Place::ClearAllDoor()
{
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager		= ENGINE_GET_NPC_MANAGER();
	DoorVector::iterator beg = m_DoorVector.begin();
	for (; beg != m_DoorVector.end(); ++beg)
	{
		GetActorManger().DelActor(beg->m_ID);
		LC_ServerNPC* npc = GetActorManger().GetNpc(beg->m_ID);
		if (npc != NULL)
		{
			pkWorldManager->RemoveObjectFromMap(npc);
			pkNPCManager->DestoryNPC(npc);
		}
	}
	m_DoorVector.clear();
}

int32_t LC_ServerMapLogic_Place::GetAllDoorSize()
{
	return (int32_t)m_DoorVector.size();
}

bool LC_ServerMapLogic_Place::SetDoorState(int index, DoorState state)
{
	if (index < 0 || index >= GetAllDoorSize())
	{
		return false;
	}
	return m_DoorVector[index].SetDoorState(state);
}

bool LC_ServerMapLogic_Place::BeginDialogue(int dialogueid)
{
	GameLogic::LC_ActorPtrList playerlist;
	GetAllRealPlayer(playerlist);
	GameLogic::LC_ActorPtrList::iterator beg = playerlist.begin();

	for (; beg != playerlist.end(); ++beg)
	{
		LC_ServerPlayer* play = (LC_ServerPlayer*)(*beg);
		NotifyGameEventToClient(play, EVENT_TYPE_DIALOGUE, dialogueid);
		m_dialoguePlayerMap[play->GetID()] = false;
	}
	if (!m_dialoguePlayerMap.empty())
	{
		m_bdialogue = true;
	}
	return true;
}

void LC_ServerMapLogic_Place::__enddialogue()
{
	m_bdialogue = false;
	m_dialoguePlayerMap.clear();
	if (m_dialogNpcId != 0)
	{
		ClearNPC(GetActorManger().GetNpc(m_dialogNpcId));
	}
	m_dialogNpcId = 0;
}

bool LC_ServerMapLogic_Place::__checkdialoguefinish()
{
	GameLogic::LC_ActorPtrList playerlist;
	GetAllRealPlayer(playerlist);
	GameLogic::LC_ActorPtrList::iterator beg = playerlist.begin();
	for (; beg != playerlist.end(); ++beg)
	{
		DialoguePlayerMap::iterator it = m_dialoguePlayerMap.find((*beg)->GetID());
		if (it != m_dialoguePlayerMap.end())
		{
			if (!it->second)
			{
				return false;
			}
		}
	}
	return true;
}

void LC_ServerMapLogic_Place::SpawnDialogNPC(GameLogic::LC_TranscriptionListEntry& transEntry)
{
	LC_ServerNPC* npc = SpawnNPC(transEntry, -1, FACTION_NEUTRAL);
	if (npc == NULL)
	{
		return;
	}
	m_dialogNpcId = npc->GetID();
}

bool LC_ServerMapLogic_Place::CheckDialogueBegin()
{
	return m_bdialogue;
}

void LC_ServerMapLogic_Place::ClientBeginDialogue(LC_ServerPlayer* player, int dialogueid)
{
	if (m_bdialogue)
	{
		return;
	}
	if (m_dialogNpcId == 0)
	{
		return;
	}
	if (!CheckPlayerIn(player->GetID()))
	{
		return;
	}
	BeginDialogue(dialogueid);
}

void LC_ServerMapLogic_Place::ClientFinishDialogue(LC_ServerPlayer* player)
{
	if (!m_bdialogue)
	{
		return;
	}
	if (!CheckPlayerIn(player->GetID()))
	{
		return;
	}
	DialoguePlayerMap::iterator it = m_dialoguePlayerMap.find(player->GetID());
	if (it == m_dialoguePlayerMap.end())
	{
		return;
	}
	it->second = true;
	if (__checkdialoguefinish())
	{
		__enddialogue();
	}
}

void LC_ServerMapLogic_Place::DialogueDelayOut()
{
	__enddialogue();
}

void LC_ServerMapLogic_Place::SetTransPoint(TransPointType transtype, int x, int y)
{
	m_TransType = transtype;
	m_TransPort.x = x;
	m_TransPort.y = y;
}

bool LC_ServerMapLogic_Place::CreateFaction(FactionType FactionId)
{
	LC_ServerMapLogic_Organize* org = GetActorManger().CreateOrganize();
	if (org == NULL)
		return false;
	bool ret = _AddFaction(FactionId, org->Organize_id);
	if (!ret)
	{
		GetActorManger().UnOrganizeActor(org->Organize_id);
		return false;
	}
	return true;
}

bool LC_ServerMapLogic_Place::AddActor2Faction(LC_ActorBase* actor, FactionType FactionId)
{
	if (actor == NULL)
	{
		return false;
	}
	if (FactionId < FACTION_MIN || FactionId > FACTION_MAX)
	{
		return false;
	}
	//获取原来的阵营
	FactionType oldfaction = (FactionType)actor->GetFactionID();
	if (oldfaction != FactionId)
	{
		DelActorInFaction(actor->GetID(), oldfaction, false);
	}
	int innerid = GetInerId_FactionId(FactionId);
	if (innerid < 0)
	{
		return false;
	}
	bool ret = GetActorManger().AddActor2Organize(actor->GetID(), innerid);
	if (!ret)
	{
		return false;
	}
	actor->SetFactionID(FactionId);
	return true;
}

bool LC_ServerMapLogic_Place::DelActorInFaction(object_id_type actorId, FactionType FactionId, bool reset)
{
	if (FactionId < FACTION_MIN || FactionId > FACTION_MAX)
	{
		return false;
	}
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(actorId);
	if (actor == NULL)
	{
		return false;
	}
	int innerid = GetInerId_FactionId(FactionId);
	if (innerid < 0)
	{
		return false;
	}
	GetActorManger().DelActorInOrganize(actorId, innerid);
	if (!reset)
	{
		return true;
	}
	LC_ActorBase* actorbase = NULL;
	if (IS_NPC_ID(actorId))
	{
		actorbase = GetActorManger().GetNpc(actorId);
	}
	else if (IS_PLAYER_CLASS_ID(actorId))
	{
		actorbase = GetActorManger().GetPlayer(actorId);
	}
	if (actorbase == NULL)
	{
		return false;
	}
	actorbase->Reset2DefaultFaction();
	return true;
}

bool LC_ServerMapLogic_Place::DelFaction(FactionType FactionId)
{
	int innerid = GetInerId_FactionId(FactionId);
	if (innerid < 0)
	{
		return false;
	}
	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();
	{
		const object_id_set* pkIDs = GetActorManger().GetAllActorIDsInOrganize(innerid);
		if (pkIDs && !pkIDs->empty())
		{
			object_id_set::const_iterator beg = pkIDs->begin();
			for (; beg != pkIDs->end(); ++beg)
			{
				if (IS_PLAYER_ID(*beg))
				{
					LC_ServerPlayer* theplayer = GetActorManger().GetPlayer(*beg);
					if (theplayer != NULL)
					{
						theplayer->Reset2DefaultFaction();
					}
				}
			}
		}
	}
	GetActorManger().UnOrganizeActor(innerid);
	_DelFaction(FactionId);
	return true;
}

const object_id_set* LC_ServerMapLogic_Place::GetAllActorListInFaction(FactionType FactionId)
{
	int innerid = GetInerId_FactionId(FactionId);
	if (innerid < 0)
	{
		return NULL;
	}
	return GetActorManger().GetAllActorIDsInOrganize(innerid);
}

int32_t LC_ServerMapLogic_Place::GetAllActorSizeInFaction(FactionType FactionId)
{
	int32_t innerid = GetInerId_FactionId(FactionId);
	if (innerid < 0)
	{
		return 0;
	}
	return GetActorManger().GetAllActorSizeInOrganize(innerid);
}

object_id_vector LC_ServerMapLogic_Place::GetInActorListInFaction(FactionType FactionId)
{
	object_id_vector ret;
	int innerid = GetInerId_FactionId(FactionId);
	if (innerid < 0)
	{
		return ret;
	}
	ret = GetActorManger().GetInActorListInOrganize(innerid);
	return ret;
}

int LC_ServerMapLogic_Place::GetInActorSizeInFaction(FactionType FactionId)
{
	int innerid = GetInerId_FactionId(FactionId);
	if (innerid < 0)
	{
		return 0;
	}
	return GetActorManger().GetInActorSizeInOrganize(innerid);
}


FactionType LC_ServerMapLogic_Place::GetFactionId_InerId(int innerid)
{
	InnerMap::iterator it = m_inerId2Faction.find(innerid);
	if (it != m_inerId2Faction.end())
	{
		return it->second;
	}
	return FACTION_NONE;
}

int LC_ServerMapLogic_Place::GetInerId_FactionId(FactionType fationid)
{
	FactionMap::iterator it = m_FactionContain.find(fationid);
	if (it != m_FactionContain.end())
	{
		return it->second.m_InterId;
	}
	return -1;
}

FactionResult LC_ServerMapLogic_Place::GetFactionResult(FactionType fationId)
{
	FactionMap::iterator it = m_FactionContain.find(fationId);
	if (it != m_FactionContain.end())
	{
		return it->second.m_Result;
	}
	return FACTION_RESULT_NONE;
}

void LC_ServerMapLogic_Place::SetFactionResult(FactionType fationId, FactionResult result)
{
	FactionMap::iterator it = m_FactionContain.find(fationId);
	if (it != m_FactionContain.end())
	{
		it->second.m_Result = result;
	}
}

FactionType LC_ServerMapLogic_Place::GetActorFaction(GameLogic::LC_ActorBase* actor)
{
	if (actor == NULL)
	{
		return FACTION_NONE;
	}
	FactionType faction = (FactionType)actor->GetFactionID();
	if (faction < FACTION_MIN || faction >= FACTION_MAX)
	{
		return FACTION_NONE;
	}
	return faction;
}

mem::vector<FactionType> LC_ServerMapLogic_Place::GetAllFactionId()
{
	mem::vector<FactionType> ret;
	FactionMap::iterator beg = m_FactionContain.begin();
	for (; beg != m_FactionContain.end(); ++beg)
	{
		ret.push_back(beg->first);
	}
	return ret;
}

void LC_ServerMapLogic_Place::SetAutoFit(bool autofit, AutoFitType fittype)
{
	m_Autofit = autofit;
	m_AutofitType = fittype;
}

void LC_ServerMapLogic_Place::GetAutoFit(bool& autofit, AutoFitType& fittype, int& fitlevel)
{
	autofit = m_Autofit;
	if (autofit)
	{
		fittype = m_AutofitType;
		fitlevel = m_AutofitLevel;
	}
}

int LC_ServerMapLogic_Place::GetTributaryFreePlayerCount()
{
	int currentCount = GetControllerPlayerCount();
	return m_iTributaryMaxPlayer - currentCount;;
}

bool LC_ServerMapLogic_Place::IsCrossRealm()
{
	return m_CrossRealm;
}

void LC_ServerMapLogic_Place::SetCrossRealm(bool crossrealm, CrossRealmType crossrealmtype, int crossrealmid)
{
	m_CrossRealm = crossrealm;
	m_CrossRealmType = crossrealmtype;
	m_CrossRealmId = crossrealmid;
}

void LC_ServerMapLogic_Place::GetCrossRealm(CrossRealmType& crossrealmtype, int& crossrealmid)
{
	crossrealmtype = m_CrossRealmType;
	crossrealmid = m_CrossRealmId;
}

bool LC_ServerMapLogic_Place::_AddFaction(FactionType fationId, int innerid)
{
	LC_ServerMapLogic_Faction& temp = m_FactionContain[fationId];
	temp.m_InterId = innerid;
	temp.m_FactionType = fationId;
	m_inerId2Faction[innerid] = fationId;
	return true;
}

bool LC_ServerMapLogic_Place::_DelFaction(FactionType fationId)
{
	int innerid = GetInerId_FactionId(fationId);
	FactionMap::iterator it1 = m_FactionContain.find(fationId);
	if (it1 != m_FactionContain.end())
	{
		m_FactionContain.erase(it1);
		InnerMap::iterator it2 = m_inerId2Faction.find(innerid);
		if (it2 != m_inerId2Faction.end())
		{
			m_inerId2Faction.erase(it2);
		}
	}
	return true;
}

bool LC_ServerMapLogic_Place::_InFaction_InerId(int innerid)
{
	InnerMap::iterator it = m_inerId2Faction.find(innerid);
	if (it != m_inerId2Faction.end())
	{
		return true;
	}
	return false;
}

bool LC_ServerMapLogic_Place::_InFaction_FactionId(FactionType fationid)
{
	FactionMap::iterator it = m_FactionContain.find(fationid);
	if (it != m_FactionContain.end())
	{
		return true;
	}
	return false;
}

ResultType LC_ServerMapLogic_Place::CheckExclusive(LC_ServerPlayer* pkServerPlayer)
{
	LC_PlayerGroupBase* pkPlayerGroup = pkServerPlayer->GetPlayerGroup();
	if (pkPlayerGroup != NULL)
	{
		RaidPortalEntry* entry = pkPlayerGroup->GetRaidPortal();
		if (entry != NULL && entry->m_LogicId != GetMapLogicID())
		{
			Exclusive_Type nexttype = GetExclusive();
			Exclusive_Type curtype = entry->m_Excludetype;
			if (ExclusiveTable[nexttype][curtype] != 0)
			{
				return RE_RAID_EXCLUDE_HAVE_GROUPRAID;
			}
		}
	}
	return RE_SUCCESS;
}

void LC_ServerMapLogic_Place::SetCanPK(bool pk)
{
	if (m_CanPK == pk)
	{
		return;
	}
	m_CanPK = pk;
	GameLogic::LC_ActorPtrList playerlist;
	GetAllPlayer(playerlist);
	GameLogic::LC_ActorPtrList::iterator beg = playerlist.begin();
	for (; beg != playerlist.end(); ++beg)
	{
		LC_ServerPlayer* player = (LC_ServerPlayer*)(*beg);
		player->SetPlaceCanPK(pk);
	}
}

bool LC_ServerMapLogic_Place::AddPlay2And(LC_ServerMapLogic_PlayBase* play, bool start)
{
	if (play == NULL)
	{
		return false;
	}
	if (!IsProcess())
	{
		return false;
	}
	m_AndPlays.push_back(play);
	if (start)
	{
		play->SetAutoStart(start);
		if (m_Start)
		{
			play->Start();
		}
	}
	return true;
}

bool LC_ServerMapLogic_Place::AddPlay2Or(LC_ServerMapLogic_PlayBase* play, bool start)
{
	if (play == NULL)
	{
		return false;
	}
	if (!IsProcess())
	{
		return false;
	}
	m_OrPlays.push_back(play);
	if (start)
	{
		play->SetAutoStart(start);
		if (m_Start)
		{
			play->Start();
		}
	}
	return true;
}

void LC_ServerMapLogic_Place::DelPlay(PlayType playtype)
{
	LC_ServerMapLogic_PlayBase* theplay = NULL;
	PlayVector::iterator beg = m_AndPlays.begin();
	for (; beg != m_AndPlays.end(); ++beg)
	{
		if ((*beg)->GetType() == playtype)
		{
			if ((*beg)->CheckRun())
			{
				(*beg)->End();
			}
			theplay = *beg;
			m_AndPlays.erase(beg);
			break;
		}
	}
	if (theplay != NULL)
	{
		(*beg)->UnInit();
		T_DELETE_D(*beg);
		return;
	}
	beg = m_OrPlays.begin();
	for (; beg != m_OrPlays.end(); ++beg)
	{
		if ((*beg)->GetType() == playtype)
		{
			if ((*beg)->CheckRun())
			{
				(*beg)->End();
			}
			theplay = *beg;
			m_OrPlays.erase(beg);
			break;
		}
	}
	if (theplay != NULL)
	{
		(*beg)->UnInit();
		T_DELETE_D(*beg);
		return;
	}
}

LC_ServerMapLogic_PlayBase* LC_ServerMapLogic_Place::GetPlay(PlayType playtype)
{
	PlayVector::iterator beg = m_AndPlays.begin();
	for (; beg != m_AndPlays.end(); ++beg)
	{
		if ((*beg)->GetType() == playtype)
		{
			return *beg;
		}
	}
	beg = m_OrPlays.begin();
	for (; beg != m_OrPlays.end(); ++beg)
	{
		if ((*beg)->GetType() == playtype)
		{
			return *beg;
		}
	}
	return NULL;
}

void LC_ServerMapLogic_Place::StartPlay(PlayType playtype)
{
	PlayVector::iterator beg = m_AndPlays.begin();
	for (; beg != m_AndPlays.end(); ++beg)
	{
		if (playtype == PlayType_None || (*beg)->GetType() == playtype)
		{
			if (!(*beg)->CheckRun() && !(*beg)->CheckEnd())
			{
				(*beg)->Start();
			}
		}
	}
	beg = m_OrPlays.begin();
	for (; beg != m_OrPlays.end(); ++beg)
	{
		if (playtype == PlayType_None || (*beg)->GetType() == playtype)
		{
			if (!(*beg)->CheckRun() && !(*beg)->CheckEnd())
			{
				(*beg)->Start();
			}
		}
	}
}

void LC_ServerMapLogic_Place::EndPlay(PlayType playtype)
{
	if(playtype == PlayType_None)
	{
		EndAllPlay();
	}
	bool bHasOther = false;
	PlayVector::iterator beg = m_AndPlays.begin();
	for (; beg != m_AndPlays.end(); ++beg)
	{
		LC_ServerMapLogic_PlayBase* pBase = *beg;
		if(pBase != NULL && pBase->CheckRun())
		{
			if(pBase->GetType() == playtype)
			{
				pBase->End();
			}
			else
			{
				bHasOther = true;
			}
		}
	}
	beg = m_OrPlays.begin();
	for (; beg != m_OrPlays.end(); ++beg)
	{
		LC_ServerMapLogic_PlayBase* pBase = *beg;
		if(pBase != NULL && pBase->CheckRun())
		{
			if(pBase->GetType() == playtype)
			{
				pBase->End();
			}
			else
			{
				bHasOther = true;
			}
		}
	}
	if(bHasOther == false)
	{
		mem::set<int> setA, setB;
		ClearAllMonster(setA, setB);
		AllPlayEnd();
	}
}

void LC_ServerMapLogic_Place::EndAllPlay()
{
	PlayVector::iterator beg = m_AndPlays.begin();
	for (; beg != m_AndPlays.end(); ++beg)
	{
		if ((*beg)->CheckRun())
		{
			(*beg)->End();
		}
	}
	beg = m_OrPlays.begin();
	for (; beg != m_OrPlays.end(); ++beg)
	{
		if ((*beg)->CheckRun())
		{
			(*beg)->End();
		}
	}
	mem::set<int> setA, setB;
	ClearAllMonster(setA, setB);
	AllPlayEnd();
}

void LC_ServerMapLogic_Place::ClearAllPlay()
{
	PlayVector temp = m_AndPlays;
	m_AndPlays.clear();
	PlayVector::iterator beg = temp.begin();
	for (; beg != temp.end(); ++beg)
	{
		(*beg)->UnInit();
		T_DELETE_D(*beg);
	}
	temp.clear();
	temp = m_OrPlays;
	m_OrPlays.clear();
	beg = temp.begin();
	for (; beg != temp.end(); ++beg)
	{
		(*beg)->UnInit();
		T_DELETE_D(*beg);
	}
}

bool LC_ServerMapLogic_Place::RestartLogic()
{
	uint64_t guildID = GetGuildID();
	int32_t placeInvaildTime = GetPlaceInvaildTime();

	Init();

	//防止重启服务器时设置好的公会秘境数据被清掉
	if(MT_GUILD_MAP==GetMapLogicType() && INVALID_GUILD_ID!=guildID)
	{
		SetGuildID(guildID);
		SetPlaceInvaildTime(placeInvaildTime);
	}
	
	ResultType ret = LC_ServerMapLogic_PlayManager::AssemblePlay(this);
	if (ret != RE_SUCCESS)
	{
		GfxWriteLog( LOG_TAG_INFO, LOG_SWITCH_ERROR, "LC_ServerMapLogic_Place::RestartLogic LC_ServerMapLogic_PlayManager::AssemblePlay failed !!! ret:%d",  ret);
		return false;
	}
	Start();
	return true;
}

bool LC_ServerMapLogic_Place::CheckAllPlayEnd()
{
	PlayVector::iterator beg = m_AndPlays.begin();
	for (; beg != m_AndPlays.end(); ++beg)
	{
		if (!(*beg)->CheckEnd())
		{
			return false;
		}
	}
	beg = m_OrPlays.begin();
	for (; beg != m_OrPlays.end(); ++beg)
	{
		if ((*beg)->CheckEnd())
		{
			return true;
		}
	}
	if (m_OrPlays.empty())
	{
		return true;
	}
	return false;
}

void LC_ServerMapLogic_Place::BeginReadPlay(int& index)
{
	index = 0;
}

bool LC_ServerMapLogic_Place::ReadOnePlay(PlayVector::iterator& it, int& index)
{
	if (index < (int)m_AndPlays.size())
	{
		if (index == 0)
		{
			it = m_AndPlays.begin();
		}
		else
		{
			++it;
		}
		++index;
		return true;
	}
	else if (index < (int)(m_AndPlays.size() + m_OrPlays.size()))
	{
		if (index == (int)m_AndPlays.size())
		{
			it = m_OrPlays.begin();
		}
		else
		{
			++it;
		}
		++index;
		return true;
	}
	return false;
}

void LC_ServerMapLogic_Place::UpdateAllPlay(float fCurrentTime, float fDeltaTime)
{
	if (m_AndPlays.empty() && m_OrPlays.empty())
	{
		return;
	}
	bool isend1 = true;
	PlayVector::iterator beg = m_AndPlays.begin();
	for (; beg != m_AndPlays.end(); ++beg)
	{
		if ((*beg)->CheckRun())
		{
			(*beg)->Update(fCurrentTime, fDeltaTime);
			isend1 = false;
		}
		else if (!(*beg)->CheckEnd())
		{
			isend1 = false;
		}
	}
	int isend2 = false;
	beg = m_OrPlays.begin();
	for (; beg != m_OrPlays.end(); ++beg)
	{
		if ((*beg)->CheckRun())
		{
			(*beg)->Update(fCurrentTime, fDeltaTime);
		}
		else if ((*beg)->CheckEnd())
		{
			isend2 = true;
		}
	}
	if (m_OrPlays.empty())
	{
		isend2 = true;
	}
	bool isend = (isend1 && isend2);
	if (isend)
	{
		if (!m_AllPlayEnd)
		{
			EndAllPlay();
			if (GetAutoFinishPlayEnd())
			{
				PreFinsh();
			}
			m_AllPlayEnd = true;
		}
	}
	else
	{
		m_AllPlayEnd = false;
	}
}

LC_ServerMap_Trigger* LC_ServerMapLogic_Place::AddCircleTrigger(Utility::UT_Circle& circle, LC_ServerMap_Trigger::TriggerCallBack callback)
{
	LC_ServerMap* themap = (LC_ServerMap*)GetMap();
	if (themap == NULL)
	{
		return NULL;
	}
	return themap->AddCircleTrigger(circle, this, callback);
}

LC_ServerMap_Trigger* LC_ServerMapLogic_Place::AddRectTrigger(Utility::UT_OBB& obb, LC_ServerMap_Trigger::TriggerCallBack callback)
{
	LC_ServerMap* themap = (LC_ServerMap*)GetMap();
	if (themap == NULL)
	{
		return NULL;
	}
	return themap->AddRectTrigger(obb, this, callback);
}

LC_ServerMap_Trigger* LC_ServerMapLogic_Place::GetTrigger(int id)
{
	LC_ServerMap* themap = (LC_ServerMap*)GetMap();
	if (themap == NULL)
	{
		return NULL;
	}
	return themap->GetTrigger(id);
}

void LC_ServerMapLogic_Place::DelTrigger(int id)
{
	LC_ServerMap* themap = (LC_ServerMap*)GetMap();
	if (themap == NULL)
	{
		return;
	}
	themap->DelTrigger(id);
}

void LC_ServerMapLogic_Place::AddGuildTransport(const unique_id_impl& charid, uint64_t guildid, int transportid)
{
	GuildTransport& temp = m_GuildTransport[charid];
	temp.guildid = guildid;
	temp.charid = charid;
	temp.transportid = transportid;
}

bool LC_ServerMapLogic_Place::CheckGuildTransport(LC_ServerPlayer* player)
{
	if (NULL == player)
	{
		return !m_GuildTransport.empty();
	}
	return m_GuildTransport.count(player->GetInstance()) > 0;
}

void LC_ServerMapLogic_Place::_receivedClientAlreadyInGame(GameLogic::LC_GameEvent* pkEvent)
{
	object_id_type lPlayerID	= (object_id_type)pkEvent->GetEventParam(EVENT_BASE_PARAM_1);
	int lLogicMapID	= (int)pkEvent->GetEventParam(EVENT_BASE_PARAM_2);
	if (lLogicMapID != GetMapLogicID())
	{
		return;
	}

	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkPlayerManager->FindPlayer(lPlayerID);
	IF_A_NA_PLAYER(pkPlayer)
	{
		return;
	}
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActorInOrganize(lPlayerID, m_AllPlayerOrg);
	if (actor == NULL)
	{
		return;
	}
	const unique_id_impl& instance = pkPlayer->GetInstance();
	if (!actor->IsLoading() && !actor->IsOffline())
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "_receivedClientAlreadyInGame actorState:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, mapLogicID:%d mapResID:%d", 
			actor->State, instance.detail(), instance.catagory(), pkPlayer->GetUserID(), lLogicMapID, GetMapResID());
		return;
	}
	bool isrecover = actor->IsOffline();
	if (pkPlayer->IsInstance())
	{
		AddActor2Faction(pkPlayer, FACTION_PLAYER);
	}

	MapLogicEvent kPlayerEvent(PlayerEvent_Enter, pkPlayer->GetID());
	if (!actor->IsLoading())
	{
		kPlayerEvent.SetType(PlayerEvent_Recover);
	}
	//todo::向玩法通知
	//处理进入
	int index = 0;
	BeginReadPlay(index);
	PlayVector::iterator playit;
	while (ReadOnePlay(playit, index))
	{
		(*playit)->PlayerEventNotify((LC_ServerPlayer*)pkPlayer, kPlayerEvent);
	}

	GetActorManger().SetActorState(lPlayerID, Logic_Actor_State_ACTIVE);
	if (!isrecover)
	{
		PlayerChangeLogicArea(pkPlayer, pkPlayer->GetCurrentLogicAreaID(), pkPlayer->GetCurrentLogicAreaID(),0,0);
		ComputeAutoFitLevel();
		int cursize = GetAllRealActivePlayerSize();
		AdjustMonster(cursize - 1, cursize);
		PlayerEnterEvent(pkPlayer,cursize);
	}

	//事件
	GameStory::LC_StoryLogic_Manager* pkStoryManager = SERVER_GET_STORYLOGIC_MANAGER;
	if (pkStoryManager)
	{
		pkStoryManager->PostPlayerEnterMap(pkPlayer, this);
	}

	//通知副本存在有效时间
	NotifyGameEventToClient(pkPlayer, EVENT_TYPE_VAILD_TIME, GetLeftTime());
	
	SK_ServerSkillEventManager* pkEventManager = (SK_ServerSkillEventManager*)SK_ServerSkillEventManager::GetSingletonPtr();
	SK_SkillEvent* pkskillEvent = pkEventManager->AllocEvent();
	if (NULL != pkskillEvent)
	{
		pkskillEvent->SetEventType(SETE_PLAYER_ENTER_MAP);
		pkskillEvent->SetSrcActorID(pkPlayer->GetID());
		pkEventManager->AddEvent(pkskillEvent);
	}
	
	LC_GameEvent pkGameEvent;
	pkGameEvent.SetEventType(GLET_TRANSCRIPTION_EVENT);
	pkGameEvent.SetEventParam(EP_TRANSCRIPTION_EVENT_TYPE, EVENT_TYPE_FIRSTENTER_TRANSCRIPTION);
	int iStoryID	= GetStoryID();
	pkGameEvent.SetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_1, iStoryID);
	MG_PB<msg_define::game_event> EventMsg;
	msg_define::game_event& msg_val = EventMsg.m_value;
	pkGameEvent.SetToProtocol(msg_val);
	((LC_ServerPlayer*) pkPlayer)->SendMsgToClient(MGPT_RLT_GAME_EVENT, &EventMsg);

}

void LC_ServerMapLogic_Place::_kickoffPlayerFromGroup(GameLogic::LC_GameEvent* pkEvent)
{
	if (GetGroupShare() == Share_None)
	{
		return;
	}

	object_id_type playerLogicID	= (object_id_type)pkEvent->GetEventParam(EVENT_BASE_PARAM_1);
	int lGroupID		= (int)pkEvent->GetEventParam(EVENT_BASE_PARAM_2);
	if (m_AllGroup.count(lGroupID) <= 0)
	{
		return;
	}
	LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(playerLogicID);
	if (actor == NULL || !actor->CheckIn())
	{
		return;
	}
	//查看是否有成员在副本中
	int count = 0;
	__checkGroupMemberIn(lGroupID, playerLogicID, count);
	if (count <= 0)
	{
		__clearAllGroupShare();
		return;
	}
	LetPlayerLeave(GetActorManger().GetPlayer(playerLogicID));
}

void LC_ServerMapLogic_Place::_groupDismiss(GameLogic::LC_GameEvent* pkEvent)
{
	if (NULL == pkEvent || GetGroupShare() == Share_None)
	{
		return;
	}
	//object_id_type playerLogicID	= pkEvent->GetEventParam(EVENT_BASE_PARAM_1);
	int lGroupID		= (int)pkEvent->GetEventParam(EVENT_BASE_PARAM_2);
	if (m_AllGroup.find(lGroupID) == m_AllGroup.end())
	{
		return;
	}

	//查看是否有成员在副本中
	int count = GetAllRealPlayerSize();
	if (count <= 1)
	{
		return;
	}

	SetPlaceState(PLACESTATE_END);
}

void LC_ServerMapLogic_Place::__clearAllGroupShare()
{
	mem::set<int>::iterator beg = m_AllGroup.begin();
	for (; beg != m_AllGroup.end(); ++beg)
	{
		LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
		LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->FindPlayerGroupByGroupID(*beg);
		if (pkPlayerGroup != NULL)
		{
			pkPlayerGroup->ClearRaidPortal(GetMapLogicID());
		}
	}
	m_AllGroup.clear();
}

void LC_ServerMapLogic_Place::__checkGroupMemberIn(int groupid, int exceptid, int& count)
{
	//calculate award when npc killed monster
	LC_ServerPlayerGroupManager* pkPlayerGroupManager = SERVER_GET_PLAYER_GROUP_MANAGER();
	if (NULL == pkPlayerGroupManager)
	{
		return;
	}
	LC_PlayerGroupBase* pkPlayerGroup = pkPlayerGroupManager->FindPlayerGroupByGroupID(groupid);
	if (pkPlayerGroup != NULL)
	{
		for (int i = 0; i < MAX_PLAYER_GROUP_MEMBER_LIST_SIZE; i++)
		{
			LC_PlayerGroupEntry* pMemList = pkPlayerGroup->GetGroupEntryInMemberList(i);
			if (pMemList != NULL)
			{
				LC_PlayerBase* pPlayerBase = pMemList->GetPlayer();
				if (pPlayerBase != NULL && pPlayerBase->GetID() != (uint32_t)exceptid)
				{
					LC_ServerMapLogic_Actor* actor = GetActorManger().GetActor(pPlayerBase->GetID());
					if (actor != NULL && actor->CheckIn())
					{
						++count;
					}
				}
			}
		}
	}
}

void LC_ServerMapLogic_Place::_addCourageStateWhenEliteMonsterDie(LC_ServerNPC* npc)
{
	LC_ServerPlayerManager* pkServerPlayerManager = SERVER_GET_PLAYER_MANAGER();

	LC_ServerNPCOwnerList& list = npc->GetNPCOwnerList();
	{
		for (int i = 0 ; i < list.GetOwnerCount(); ++i)
		{
			object_id_type player_id = list.GetOwnerByIndex(i);
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)pkServerPlayerManager->FindPlayer(player_id);
			if (pkPlayer)
			{
				pkPlayer->AddCourageState();
			}
		}
	}
}

void LC_ServerMapLogic_Place::_dropItem(LC_ServerNPC* pkServerNPC)
{
	if(NULL == pkServerNPC)
	{
		return;
	}

	LC_ServerItemFlopManager* pkServerItemFlopManager = LC_ServerItemFlopManager::GetSingletonPtr();
	LC_ServerTaskManager* pkServerTaskManager = (LC_ServerTaskManager*)LC_ServerTaskManager::GetSingletonPtr();

	LC_ItemFlopInfoList kItemFlopList;
	pkServerTaskManager->GenerateTaskItemFlopInfoOnNPCDead(kItemFlopList, pkServerNPC);
	pkServerItemFlopManager->GenerateRewaredOnNPCDeadAuto(pkServerNPC);

	pkServerItemFlopManager->GenerateRewaredOnNPCDeadActivity(pkServerNPC);

	{
		CF_WorldMapList::DataEntry* pkEntry = this->GetMapResPtr();
		if (pkEntry != NULL && pkEntry->_SpeMonsterDrop > 0)
		{
			pkServerItemFlopManager->GenerateRewaredOnNPCDeadSpecial(pkServerNPC, pkEntry->_SpeMonsterDrop);
		}
		if (pkEntry != NULL && pkEntry->_kActivityDrop.size() != 0)
		{
			GameStory::LC_GameStory_Manager * pkGameStoryMgr = SERVER_GET_GAMESTORY_MANAGER;
			for (UT_SIMDataList::iterator it = pkEntry->_kActivityDrop.begin();
				 it != pkEntry->_kActivityDrop.end(); ++it)
			{
				UT_SIMDataInfo& dataInfo = *it;
				if (!pkGameStoryMgr->CheckIsActiveStory(dataInfo.ID()))
				{
					continue;
				}
				pkServerItemFlopManager->GenerateRewaredOnNPCDeadSpecial(pkServerNPC, dataInfo.ParamA());
			}
		}
	}

	pkServerNPC->ClearRandIndex();
}

void LC_ServerMapLogic_Place::Start()
{
	m_Start = true;
	int index = 0;
	BeginReadPlay(index);
	PlayVector::iterator playit;
	while (ReadOnePlay(playit, index))
	{
		if ((*playit)->GetAutoStart())
		{
			(*playit)->Start();
		}
	}

	GameStory::LC_StoryLogic_Manager* pkLogicMgr = SERVER_GET_STORYLOGIC_MANAGER;
	if (pkLogicMgr)
	{
		if (m_MonsterLevel <= 0)
		{
			pkLogicMgr->PostMapCreated(this, -1);
		}
		else
		{
			pkLogicMgr->PostMapCreated(this, m_MonsterLevel);
		}
	}
}

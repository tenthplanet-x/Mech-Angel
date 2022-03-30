#include "LC_GameEventManager.h"
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
#include "CF_TranscriptionSettleGrade.h"
#include "CF_CrossRealmMap.h"
#include "LC_ServerMapLogic_TranscriptionRuleManger.h"
#include <boost/algorithm/string/replace.hpp>
#include "LC_ServerPlayerGroup.h"
#include "LC_StoryLogic_Manager.h"
#include "UT_ServerHelper.h"
#include "LC_GameStory_Manager.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "oss_define.h"
#include "oss_360_define.h"
#include "LC_NPCManagerBase.h"
#include "CF_WorldMapList.h"
#include "MG_Common.h"
#include "SC_ScriptAction.h"
#ifdef _SERVER_BUILD
#include "UT_OperateLog.h"
using namespace OPERATELOG;
#endif
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;
//--------------------------------------------------------------------------
CF_TranscriptionSettleGrade::DataEntry* get_Grade(int score, int& level)
{
	CF_TranscriptionSettleGrade* pkgrade = CF_TranscriptionSettleGrade::GetSingletonPtr();
	CF_TranscriptionSettleGrade::DataEntryMapExternIterator pkIter = pkgrade->GetIterator();
	CF_TranscriptionSettleGrade::DataEntry* pkEntry = NULL;
	bool  bFindFlag = false;
	level = 1;
	while (pkIter.HasMoreElements())
	{
		pkEntry = pkIter.PeekNextValuePtr();
		//if (pkEntry->_lGradeMin <= score && score <= pkEntry->_lGradeMax)
		if (score == pkEntry->_TranscriptionScore)
		{
			level = pkEntry->_iID;
			bFindFlag = true;
			break;
		}
		pkIter.MoveNext();
	}
	if (bFindFlag)
	{
		return pkEntry;
	}
	return NULL;
}
//------------------------------------------------------------------------------
LC_ServerMapLogic_Batch::LC_ServerMapLogic_Batch():
	m_curIndex(0), m_spanmap(), m_starttime(0.0), m_state(BATCHNONE), m_Finishevent(), m_Finishhandle()
	, m_Starthandle(), m_focusorz(0), m_logicdelayTime(0.0f), m_StatCatagory(LC_ECT_CAT_ACTOR)
{
}

LC_ServerMapLogic_Batch::~LC_ServerMapLogic_Batch()
{
	uninit();
}

bool LC_ServerMapLogic_Batch::init(const DungeonConfig::BatchEntry& Data, int index, bool belite, LC_ServerMapLogic_BatchPlay* parent)
{
	this->m_delaytime = Data.DelayTime;
	this->m_parent = parent;
	this->m_Finishevent = Data.FinishEvent;
	this->m_Finishhandle = Data.FinishHandle;
	this->m_Starthandle = Data.StartHandle;
	this->m_Listeners = Data.Listeners;
	this->m_curIndex = index;
	this->m_focusorz = 0;
	m_logicdelayTime = 0.0f;
	m_spanmap.clear();
	m_starttime = 0;
	m_state = BATCHNONE;

	m_StatCatagory = Data.StatCatagory;

	//从组合中选出怪物
	LongSetType posset;
	BatchGroupSetType::const_iterator groupbeg = Data.Group.begin();
	for (; groupbeg != Data.Group.end(); ++groupbeg)
	{
		if (groupbeg->PosIndex.empty())
		{
			continue;
		}
		posset.insert(posset.end(), groupbeg->PosIndex.begin(), groupbeg->PosIndex.end());
		//获取位置
		int index = UT_MathBase::RandInRangeInt(0, (int)groupbeg->PosIndex.size() - 1);
		int posindex = groupbeg->PosIndex[index];
		if (posindex < -1 || posindex >= m_parent->GetBatchPosSize())
		{
			continue;
		}
		PosAndLoad pos;
		//考虑是-1的情况
		if (posindex != -1)
		{
			m_parent->GetBatchPos(posindex, pos);
		}
		//获取怪物索引
		int weightcount = 0;
		MonsteWeightSetType::const_iterator weightbeg = groupbeg->MonsterCombins.begin();
		for (; weightbeg != groupbeg->MonsterCombins.end(); ++weightbeg)
		{
			weightcount += weightbeg->Rate;
		}
		if (weightcount <= 0)
		{
			continue;
		}
		int weight = UT_MathBase::RandInRangeInt(0, weightcount);
		weightcount = 0;
		weightbeg = groupbeg->MonsterCombins.begin();
		int weightindex = 0;
		for (; weightbeg != groupbeg->MonsterCombins.end(); ++weightbeg)
		{
			weightcount += weightbeg->Rate;
			if (weight < weightcount)
			{
				break;
			}
			++weightindex;
		}
		if (weightindex >= (int)groupbeg->MonsterCombins.size())
		{
			weightindex = (int)groupbeg->MonsterCombins.size() - 1;
		}
		int monsterindex = groupbeg->MonsterCombins[weightindex].MonsterIndex;
		//根据索引取出怪物列表
		if (monsterindex >= m_parent->GetBatchMonsterSize())
		{
			continue;
		}
		mem::vector<MonsterEntry> monsters;
		m_parent->GetBatchMonster(monsterindex, monsters);
		mem::vector<MonsterEntry>::iterator monsterit = monsters.begin();
		for (; monsterit != monsters.end(); ++monsterit)
		{
			LC_ServerMapLogic_Spawn_Monster* pkEntry = T_NEW_D LC_ServerMapLogic_Spawn_Monster();
			pkEntry->TypeId = monsterit->TypeId;
			pkEntry->Pos = pos;
			pkEntry->Count = monsterit->Count;
			SpawnMapIt mapit = m_spanmap.find(posindex);
			if (mapit != m_spanmap.end())
			{
				mapit->second.push_back((LC_ServerMapLogic_Spawn_Base*)pkEntry);
			}
			else
			{
				SpawnList temp;
				temp.push_back((LC_ServerMapLogic_Spawn_Base*)pkEntry);
				m_spanmap[posindex] = temp;
			}
		}
	}
	if (belite)
	{
		//精英怪
		EliteWeightSetType::const_iterator elitebeg = Data.Elite.begin();
		int weightcount = 0;
		for (; elitebeg != Data.Elite.end(); ++elitebeg)
		{
			weightcount += elitebeg->Rate;
		}
		if (weightcount > 0)
		{
			elitebeg = Data.Elite.begin();
			int weight = UT_MathBase::RandInRangeInt(0, weightcount);
			weightcount = 0;
			int weightindex = 0;
			for (; elitebeg != Data.Elite.end(); ++elitebeg)
			{
				weightcount += elitebeg->Rate;
				if (weight < weightcount)
				{
					break;
				}
				++weightindex;
			}
			if (weightindex >= (int)Data.Elite.size())
			{
				weightindex = (int)Data.Elite.size() - 1;
			}
			int eliteindex = Data.Elite[weightindex].EliteIndex;
			//根据索引取出怪物列表
			if (!posset.empty() && eliteindex < m_parent->GetBatchEliteMonsterSize())
			{
				LC_ServerMapLogic_Spawn_Elite* pkEntry = T_NEW_D LC_ServerMapLogic_Spawn_Elite();
				m_parent->GetBatcElitehMonster(eliteindex, pkEntry->TypeId);
				int posit = UT_MathBase::RandInRangeInt(0, (int)posset.size() - 1);
				int posindex = posset[posit];
				if (posindex >= m_parent->GetBatchPosSize())
				{
					posindex = m_parent->GetBatchPosSize() - 1;
				}
				else if (posindex < -1)
				{
					posindex = -1;
				}
				PosAndLoad pos;
				//考虑是-1的情况
				if (posindex != -1)
				{
					m_parent->GetBatchPos(posindex, pos);
				}
				pkEntry->Pos = pos;
				//插入技能
				pkEntry->m_skillrule = m_parent->GetBatchEliteSkillRule();

				SpawnMapIt mapit = m_spanmap.find(posindex);
				if (mapit != m_spanmap.end())
				{
					mapit->second.push_back((LC_ServerMapLogic_Spawn_Base*)pkEntry);
				}
				else
				{
					SpawnList temp;
					temp.push_back((LC_ServerMapLogic_Spawn_Base*)pkEntry);
					m_spanmap[posindex] = temp;
				}
			}
		}
	}

	return true;
}


void LC_ServerMapLogic_Batch::uninit()
{
	for (SpawnMapIt iter = m_spanmap.begin(); iter != m_spanmap.end(); ++iter)
	{
		for (SpawnListIt beg = iter->second.begin(); beg != iter->second.end(); ++beg)
		{
			T_SAFE_DELETE(*beg);
		}
	}
	m_spanmap.clear();
	m_curIndex = 0;
	m_starttime = 0;
	m_delaytime = 0;
	m_state = BATCHNONE;
	m_Finishevent.reset();
	m_Finishhandle.reset();
	m_Starthandle.reset();
	m_Listeners.clear();
	if (m_focusorz != 0)
	{
		GetPlace()->GetActorManger().UnOrganizeActor(m_focusorz);
	}
	m_focusorz = 0;
	m_logicdelayTime = 0.0f;
	this->m_parent = NULL;
}

LC_ServerMapLogic_Place* LC_ServerMapLogic_Batch::GetPlace()
{
	return m_parent->GetPlace();
}

void LC_ServerMapLogic_Batch::EventNotify(MapLogicEvent& kEvent)
{
	if (this->m_state != BATCHNORMAL)
		return;
	if (kEvent.Type() == MapLogicNotifyEvent_Org)
	{
		if (kEvent.Param(0) == m_focusorz)
		{
			if (this->m_Finishevent.Type == MapLogicFinishEvent_SpecialMonster)
			{
				if (kEvent.Param2() <= this->m_Finishevent.Value2)
				{
					DoFinish();
				}
			}
		}
	}
	else if (kEvent.Type() == MapLogicNotifyEvent_LogicArea)
	{
		if (this->m_Finishevent.Type == MapLogicFinishEvent_EnterLogicArea)
		{
			if (kEvent.Param1() == this->m_Finishevent.Value1)
			{
				DoFinish();
			}
		}
	}
	else if (kEvent.Type() == MapLogicNotifyEvent_MonsterCount)
	{
		if (this->m_Finishevent.Type == MapLogicFinishEvent_MonsterCount)
		{
			if (kEvent.Param1() <= m_Finishevent.Value1)
			{
				DoFinish();
			}
		}
	}
	else if (kEvent.Type() == MapLogicNotifyEvent_DialogFinish)
	{
		if (this->m_Finishevent.Type == MapLogicFinishEvent_Finish_Dialogue)
		{
			DoFinish();
		}
	}
	else if (kEvent.Type() == MapLogicNotifyEvent_Target)
	{
		if (this->m_Finishevent.Type == MapLogicFinishEvent_Target_Die)
		{
			DoFinish();
		}
	}
	else if(kEvent.Type() == MapLogicNotifyEvent_FinishTask)
	{
		if(this->m_Finishevent.Type == MapLogicFinishEvent_FinishTask)
		{
			if (kEvent.Param1() == m_Finishevent.Value1)
			{
				DoFinish();
				SetTaskLogicFinish();
			}
		}
	}

	for (DungeonConfig::EventListenerList::iterator it = m_Listeners.begin(); it != m_Listeners.end(); ++it)
	{
		EventListener& kListener = *it;
		if (kListener.Concern.Type == kEvent.Type() &&
				(kListener.Concern.Value1 == 0 || kListener.Concern.Value1 == kEvent.Param1()) &&
				(kListener.Concern.Value2 == 0 || kListener.Concern.Value2 == kEvent.Param2()) &&
				(kListener.Concern.Value3 == 0 || kListener.Concern.Value3 == kEvent.Param3()) &&
				(kListener.Concern.Value4 == 0 || kListener.Concern.Value4 == kEvent.Param4()) &&
				(kListener.Concern.Value5 == 0 || kListener.Concern.Value5 == kEvent.Param5()))
		{
			m_parent->BatchPlayEventHandle(kListener.Action, &kEvent);
		}
	}
}

void LC_ServerMapLogic_Batch::SetTaskLogicFinish()
{
	if(m_Finishevent.Type == MapLogicFinishEvent_FinishTask)
	{
		LC_ServerMapLogic_Place* place = GetPlace();
		place->PreFinsh();
	}
}

bool LC_ServerMapLogic_Batch::Start(float fCurrentTime)
{
	if (this->m_state != BATCHNONE)
		return false;
	this->m_starttime = fCurrentTime;
	this->m_state = BATCHPREPARE;
	return true;
}

void LC_ServerMapLogic_Batch::End()
{
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return;
	}
	this->m_state = BATCHFINISH;
	if (this->m_Finishevent.Type == MapLogicFinishEvent_SpecialMonster)
	{
		//指定怪物死亡是由事件控制
		if (m_focusorz != 0)
		{
			pPlace->GetActorManger().UnOrganizeActor(m_focusorz);
			m_focusorz = 0;
		}
	}
	else if (this->m_Finishevent.Type == MapLogicFinishEvent_MonsterHP)
	{
		if (m_focusorz != 0)
		{
			pPlace->GetActorManger().UnOrganizeActor(m_focusorz);
			m_focusorz = 0;
		}
	}
}

bool LC_ServerMapLogic_Batch::GenMonsters()
{
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return false;
	}
	//刷怪
	LC_TranscriptionListEntry transEntry;
	transEntry.m_iMapID = pPlace->GetMapLogicID();
	SpawnMapIt mapbeg = m_spanmap.begin();
	for (; mapbeg != m_spanmap.end(); ++mapbeg)
	{
		bool changepos = false;
		Utility::UT_Vec3Int the_pos;
		if (mapbeg->first == -1)
		{
			GameLogic::LC_ActorPtrList plays;
			pPlace->GetAllPlayer(plays);
			if (!plays.empty())
			{
				int x = 0;
				int y = 0;
				int count = 0;
				GameLogic::LC_ActorPtrList::iterator playbeg = plays.begin();
				for (; playbeg != plays.end(); ++playbeg)
				{
					LC_ServerPlayer* the_player = (LC_ServerPlayer*)(*playbeg);
					if (the_player != NULL)
					{
						x += the_player->GetCurrentLocation().x;
						y += the_player->GetCurrentLocation().y;
						++count;
					}
				}
				if (count != 0)
				{
					x /= count;
					y /= count;
					the_pos.x = x;
					the_pos.y = y;
					changepos = true;
				}
			}
		}

		bool isautofit = false;
		AutoFitType fittype;
		int autofitlevel = -1;
		pPlace->GetAutoFit(isautofit, fittype, autofitlevel);

		SpawnListIt listbeg = mapbeg->second.begin();
		mem::vector<Utility::UT_Vec3Int> poslist;
		int index = 1;
		for (; listbeg != mapbeg->second.end(); ++listbeg)
		{
			if (!changepos)
			{
				the_pos.x = (*listbeg)->Pos.x;
				the_pos.y = (*listbeg)->Pos.y;
				the_pos.z = (*listbeg)->Pos.z;
			}
			if ((*listbeg)->Flag == SPAN_INFO_MONSTER)
			{
				LC_ServerMapLogic_Spawn_Monster* monster = (LC_ServerMapLogic_Spawn_Monster*)(*listbeg);
				poslist.clear();
				GenRandomPosition(the_pos, poslist, index + monster->Count, pPlace->GetMapResID());
				int i = 0;
				while (i < monster->Count)
				{
					transEntry.m_kSpawnLocation		= poslist[i + index];
					transEntry.m_iSpawnCharType		= monster->TypeId;
					//transEntry.m_iZRotAngle=__getAngle(poslist[i+index]);
					if ((*listbeg)->Pos.l != 0)
					{
						transEntry.m_lWanderPathID		= (*listbeg)->Pos.l;
					}
					if ((*listbeg)->Pos.r != 0)
					{
						transEntry.m_iWanderRadius		= (*listbeg)->Pos.r;
					}
					if ((*listbeg)->Pos.a != 0)
					{
						transEntry.m_iZRotAngle		= (*listbeg)->Pos.a;
					}
					LC_ServerNPC* thenpc = pPlace->SpawnNPC(transEntry, autofitlevel);
					if (thenpc)
					{
						if (LC_ECT_CAT_GUILD==m_StatCatagory ||  LC_ECT_CAT_ACTOR_AND_GUILD==m_StatCatagory)
						{
							int64_t iThreshold = (thenpc->GetAttributeMap()->GetAttribute(ATT_MAX_HP) * 2 / 1000);
							thenpc->RegEncountedStatGuild(LC_ECT_TP_Damage, true, iThreshold);
						}

						if(NULL != m_parent->CheckTarget())
						{
							LC_ServerNPC* target = m_parent->GetTarget();
							if (target)
							{
								thenpc->SetAdditionTargetID(target->GetID());
								thenpc->SetAdditionTargetBeginTime(GET_PROCESS_TIME);
								thenpc->SetAdditionTargetDurationTime(999999);
							}
						}
					}
					++i;
				}
				index += monster->Count;
			}
			else//精英怪
			{
				LC_ServerMapLogic_Spawn_Elite* monster = (LC_ServerMapLogic_Spawn_Elite*)(*listbeg);
				transEntry.m_kSpawnLocation		= the_pos;
				transEntry.m_iSpawnCharType		= monster->TypeId;
				//transEntry.m_iZRotAngle=__getAngle(the_pos);
				if ((*listbeg)->Pos.l != 0)
				{
					transEntry.m_lWanderPathID		= (*listbeg)->Pos.l;
				}
				if ((*listbeg)->Pos.r != 0)
				{
					transEntry.m_iWanderRadius		= (*listbeg)->Pos.r;
				}
				if ((*listbeg)->Pos.a != 0)
				{
					transEntry.m_iZRotAngle		= (*listbeg)->Pos.a;
				}
				//刷新技能
				GameLogic::LC_ActorPtrList spanlist = pPlace->SpawnEliteNPC(transEntry, index, (*monster->m_skillrule), autofitlevel);
				if (m_parent->CheckTarget())
				{
					GameLogic::LC_ActorPtrList::iterator beg = spanlist.begin();
					for (; beg != spanlist.end(); ++beg)
					{
						LC_ServerNPC* thenpc = (LC_ServerNPC*)(*beg);
						LC_ServerNPC* target = m_parent->GetTarget();
						if (target != NULL)
						{
							thenpc->SetAdditionTargetID(target->GetID());
							thenpc->SetAdditionTargetBeginTime(GET_PROCESS_TIME);
							thenpc->SetAdditionTargetDurationTime(999999);
						}
					}
				}

				index += (int)spanlist.size();
			}
		}
	}

	return true;
}


bool LC_ServerMapLogic_Batch::DoStart()
{
	bool ret = m_parent->BatchPlayEventHandle(m_Starthandle);
	if (!ret)
	{
		return false;
	}
	ret = GenMonsters();
	if (!ret)
	{
		return false;
	}
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return false;
	}
	if (m_parent->CheckReachEvent(true, m_Finishevent))
	{
		DoFinish();
		return true;
	}
	//处理事件
	if (this->m_Finishevent.Type == MapLogicFinishEvent_SpecialMonster)
	{
		LC_ServerMapLogic_Organize* ret = pPlace->GetActorManger().OrganizeActorbyType(this->m_Finishevent.Value1);
		if (ret != NULL)
			m_focusorz = ret->Organize_id;
	}
	else if (this->m_Finishevent.Type == MapLogicFinishEvent_MonsterHP)
	{
		LC_ServerMapLogic_Organize* ret = pPlace->GetActorManger().OrganizeActorbyType(this->m_Finishevent.Value1);
		if (ret != NULL)
			m_focusorz = ret->Organize_id;
	}
	this->m_state = BATCHNORMAL;
	return true;
}

bool LC_ServerMapLogic_Batch::DoFinish()
{
	if (!m_parent->BatchPlayEventHandle(m_Finishhandle))
	{
		return false;
	}
	this->m_state = BATCHFINISH;
	return true;
}

void LC_ServerMapLogic_Batch::CheckFinish(float fCurrentTime, float fDeltaTime)
{
	if (this->m_Finishevent.Type == MapLogicFinishEvent_Time)
	{
		float endtime = (float)this->m_Finishevent.Value1;
		float deltatime = fCurrentTime - (m_starttime + m_delaytime);
		if (deltatime >= endtime)
		{
			DoFinish();
		}
	}
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return;
	}
	else if (this->m_Finishevent.Type == MapLogicFinishEvent_MonsterHP)
	{
		//int montertype=this->m_Finishevent.Value1;
		int percent = this->m_Finishevent.Value2;
		const object_id_set* pkGroup = pPlace->GetActorManger().GetAllActorIDsInOrganize(m_focusorz);
		if (NULL == pkGroup || pkGroup->empty())
		{
			DoFinish();
		}
		else
		{
			bool flag = true;
			for (object_id_set::const_iterator beg = pkGroup->begin(); beg != pkGroup->end(); ++beg)
			{
				LC_ServerNPC* npc = pPlace->GetActorManger().GetNpc(*beg);
				if (NULL == npc)
					continue;
				int64_t lMaxHP			=	npc->GetAttributeMap()->GetAttribute(ATT_MAX_HP);
				int64_t lCurrentHP		=	npc->GetHP();
				if ((lCurrentHP * 100) > (percent * lMaxHP))
				{
					flag = false;
					break;
				}
			}
			if (flag)
			{
				DoFinish();
			}
		}
	}
	else if (this->m_Finishevent.Type == MapLogicFinishEvent_Finish_Immediately)
	{
		DoFinish();
	}
	else if (this->m_Finishevent.Type == MapLogicFinishEvent_Finish_Dialogue)
	{
		//最长延迟时间
		int delaytime = this->m_Finishevent.Value1;
		if (m_logicdelayTime <= 0.001f)
		{
			if (pPlace->CheckDialogueBegin())
			{
				m_logicdelayTime = fCurrentTime;
			}
		}
		else
		{
			if (fCurrentTime - m_logicdelayTime >= delaytime)
			{
				pPlace->DialogueDelayOut();
				DoFinish();
			}
		}
		return;
	}
}

void LC_ServerMapLogic_Batch::update(float fCurrentTime, float fDeltaTime)
{
	if (this->m_state == BATCHNONE)
		return;
	if (this->m_state == BATCHFINISH)
		return;
	switch (this->m_state)
	{
		case BATCHPREPARE:
			{
				float deltatime = fCurrentTime - m_starttime;
				if (deltatime >= m_delaytime)
				{
					DoStart();
				}
			}
			break;
		case BATCHNORMAL://关注结束事件
			{
				CheckFinish(fCurrentTime, fDeltaTime);
			}
			break;
		default:
			break;
	}
}

//------------------------------------------------------------------
LC_ServerMapLogic_BatchPlay::LC_ServerMapLogic_BatchPlay()
{
	m_Config = NULL;
	m_BatchSlot.clear();
	m_CurBatchIndex = 0;
	m_TotalBatchSize = 0;
	m_eliteset.clear();
	m_State = MapLogic_NormalPlayState_None;
	m_StageProgress = -1;
	m_StageProgressState = 0;
	m_SaveBatchIndex = 0;
	m_Pass = false;
	m_TargetTime = 0;
	m_TargetRaid = false;
	m_TargetOrg = 0;
	m_AllSettlePlayer.clear();
	m_NeedSettle = true;
	m_pGuildInfo = NULL;
	m_UserData = 0;
}

LC_ServerMapLogic_BatchPlay::~LC_ServerMapLogic_BatchPlay()
{
	UnInit();
}

bool LC_ServerMapLogic_BatchPlay::Init(LC_ServerMapLogic_Place* place, Dungeon_Config* config, bool settle)
{
	if (place == NULL || config == NULL)
	{
		return false;
	}
	bool ret = LC_ServerMapLogic_PlayBase::Init(place, PlayType_Batch);
	if (!ret)
	{
		return false;
	}
	m_Config = config;
	m_BatchSlot.clear();
	m_CurBatchIndex = 0;
	m_TotalBatchSize = m_Config->get_batchcount();
	m_State = MapLogic_NormalPlayState_Inited;
	m_StageProgress = -1;
	m_StageProgressState = 0;
	m_SaveBatchIndex = 0;
	m_Pass = false;
	m_TargetTime = 0;
	m_TargetRaid = false;
	m_TargetOrg = 0;
	m_AllSettlePlayer.clear();
	m_NeedSettle = settle;
	//求出精英怪的波次
	m_eliteset.clear();
	m_UserData = 0;
	int count = 0;
	EliteRefleshWeightSetType::iterator eliteit = m_Config->EliteMonsterRefleshRate.begin();
	for (; eliteit != m_Config->EliteMonsterRefleshRate.end(); ++eliteit)
	{
		count += eliteit->Rate;//100
	}
	if (count > 0)
	{
		int weight = UT_MathBase::RandInRangeInt(0, count);//20
		int weightindex = 0;
		int weightcount = 0;
		eliteit = m_Config->EliteMonsterRefleshRate.begin();
		for (; eliteit != m_Config->EliteMonsterRefleshRate.end(); ++eliteit)
		{
			weightcount += eliteit->Rate;//100
			if (weight < weightcount)
			{
				break;
			}
			++weightindex;
		}
		if (weightindex >= (int)m_Config->EliteMonsterRefleshRate.size())
		{//1>=1
			weightindex = (int)m_Config->EliteMonsterRefleshRate.size() - 1;
		}//weightindex =0
		int reflashcount = (m_Config->EliteMonsterRefleshRate[weightindex]).RefleshSize;//1
		if (reflashcount > 0)
		{
			//插入必须有的
			int i = 0;
			mem::set<object_id_type> excludeset;
			while (i < (int)this->m_Config->IncludeElite.size())
			{
				m_eliteset.insert(this->m_Config->IncludeElite[i]);
				excludeset.insert(this->m_Config->IncludeElite[i]);
				++i;
			}
			int leftcount = reflashcount - (int)this->m_Config->IncludeElite.size();//0
			if (leftcount > 0)
			{
				i = 0;
				while (i < (int)this->m_Config->ExcludeElite.size())
				{
					excludeset.insert(this->m_Config->ExcludeElite[i]);
					++i;
				}
				LC_IDVector temp;
				i = 0;
				while (i < m_TotalBatchSize)
				{
					if (excludeset.count(i) <= 0)
					{
						temp.push_back(i);
					}
					++i;
				}
				i = 0;
				while (i < leftcount && !temp.empty())
				{
					int index = UT_MathBase::RandInRangeInt(0, (int)temp.size() - 1);
					m_eliteset.insert(temp[index]);
					temp.erase(temp.begin() + index);
					++i;
				}
			}
		}
	}
	m_pGuildInfo = NULL;
	return true;
}

void LC_ServerMapLogic_BatchPlay::UnInit()
{
	End();
	m_UserData = 0;
	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	BatchSlotType::iterator beg = m_BatchSlot.begin();
	for (; beg != m_BatchSlot.end(); ++beg)
	{
		(*beg)->uninit();
		pkServerMapLogicFactory->ReleaseBatch(*beg);
	}
	m_BatchSlot.clear();
	m_CurBatchIndex = 0;
	m_TotalBatchSize = 0;
	m_State = MapLogic_NormalPlayState_None;
	m_eliteset.clear();
	m_StageProgress = -1;
	m_StageProgressState = 0;
	m_SaveBatchIndex = 0;
	m_Pass = false;

	ClearTarget();
	m_TargetTime = 0;
	m_TargetRaid = false;
	m_TargetOrg = 0;

	m_AllSettlePlayer.clear();
	m_NeedSettle = false;
	m_Config = NULL;
	LC_ServerMapLogic_PlayBase::UnInit();
	m_pGuildInfo = NULL;
}

bool LC_ServerMapLogic_BatchPlay::Start()
{
	if (m_State != MapLogic_NormalPlayState_Inited && m_State != MapLogic_NormalPlayState_End)
	{
		return false;
	}
	m_State = MapLogic_NormalPlayState_Prepare;
	m_Pass = false;

	if (m_TotalBatchSize <= 0)
	{
		End();
	}
	//刷出门
	DoorSetType::iterator doorbeg = m_Config->Doors.begin();
	for (; doorbeg != m_Config->Doors.end(); ++doorbeg)
	{
		GetPlace()->SpanDoor(*doorbeg);
	}
	//塔防
	if (!m_Config->Targets.empty())
	{
		SpanTarget(m_Config->Targets[0]);
	}
	if (CheckReachEvent(false, m_Config->StartEvent))
	{
		__doStart();
	}
	return true;
}

void LC_ServerMapLogic_BatchPlay::End()
{
	if (!CheckRun())
	{
		return;
	}
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return;
	}
	LC_MapBase* pkMap = pPlace->GetMap();
	if (NULL == pkMap)
	{
		return;
	}
	if (m_TargetOrg > 0)
	{
		object_id_vector kIDs = pPlace->GetActorManger().GetActiveActorListInOrganize(m_TargetOrg);

		object_id_vector::iterator it = kIDs.begin();
		for (; it != kIDs.end(); ++it)
		{
			LC_ActorBase* pkActor = (LC_ActorBase*)pkMap->FindObject(*it);
			if (pkActor)
			{
				m_UserData = pkActor->GetHP() * 100  / pkActor->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
			}
		}
	}
	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	BatchSlotType::iterator beg = m_BatchSlot.begin();
	for (; beg != m_BatchSlot.end(); ++beg)
	{
		(*beg)->uninit();
		pkServerMapLogicFactory->ReleaseBatch(*beg);
	}
	m_BatchSlot.clear();
	m_State = MapLogic_NormalPlayState_End;
	//结算
	if (GetSettle())
	{
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		GameStory::LC_StoryLogic_Manager* pkStoryManager = SERVER_GET_STORYLOGIC_MANAGER;
		GameStory::GSTranscriptResult result;
		result.lLogicID = pPlace->GetMapLogicID();
		result.lMapResID = pPlace->GetMapResID();
		result.lClearTime = (int32_t)pPlace->GetRecord()->getPassTime();
		GameLogic::LC_ActorPtrList playerlist;
		pPlace->GetAllRealPlayer(playerlist);
		LC_ActorPtrList::iterator beg = playerlist.begin();
		for (; beg != playerlist.end(); ++beg)
		{
			if (NULL == (*beg))
			{
				continue;
			}

			if (m_Pass)
			{
				result.kSucIds.insert((*beg)->GetID());
				LC_GameEvent* pkGameEvent = pkEventManager->AllocGameEvent();
				if (pkGameEvent != NULL)
				{
					pkGameEvent->SetEventType(GLET_TRANSCRIPTION_EVENT);
					pkGameEvent->SetEventParam(EP_TRANSCRIPTION_EVENT_TYPE, EVENT_TYPE_FINISH);
					pkGameEvent->SetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_1, (*beg)->GetID());
					pkGameEvent->SetEventParam(EP_TRANSCRIPTION_EVENT_PARAM_2, pPlace->GetMapResID());
					pkEventManager->InsertGameEvent(pkGameEvent);
				}
			}
			else
			{
				result.kFailIds.insert((*beg)->GetID());
			}
		}

		pkStoryManager->TranscriptionFinished(pPlace, result);

		LC_ServerWorldManager* pkWorldMgr = SERVER_GET_WORLD_MANAGER();
		if(pPlace->IsProcess())
		{
			Settle();
		}
	}
}

void LC_ServerMapLogic_BatchPlay::Settle()
{
	//查看是否有人
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return;
	}
	GameLogic::LC_ActorPtrList players;
	pPlace->GetAllRealPlayer(players);
	GameLogic::LC_ActorPtrList::iterator actorbeg = players.begin();
	for (; actorbeg != players.end();)
	{
		if (NULL==(*actorbeg) || CheckPlayerSettle((*actorbeg)->GetID()))
		{
			actorbeg = players.erase(actorbeg);
		}
		else
		{
			++actorbeg;
		}
	}
	if (players.empty())
	{
		return;
	}
	int level = 0;
	bool PreEnd = false;
	mem::map<object_id_type, MG_Notify_RaidSettle*> allsettle;
	//处理副本本身结算
	int score = pPlace->GetRecord()->getScore(pPlace->GetMapResID(), PreEnd);
	bool isRongGuangYuanZheng = false;
	do
	{
		LC_ServerMapLogic_TranscriptionRuleManger* rulemanager = SERVER_GET_TRANSCRIPTIONRULE_MANAGER;
		CF_TranscriptionRule::DataEntry* rule = rulemanager->GetRule(pPlace->GetMapResID());
		if (rule == NULL)
		{
			break;
		}
		if (!m_Pass)
		{
			break;
		}
		if (rule->_kSettle) //控制是否结算
		{
			break;
		}
		if (score < 0)
		{
			break;
		}
		CF_TranscriptionSettleGrade::DataEntry* pk_Entry = get_Grade(score, level);
		if (pk_Entry == NULL)
		{
			break;
		}

		isRongGuangYuanZheng = rule->_Stage.empty() ? false : true;
		Utility::UT_SIMDataList itemlist;
		
		
		//物品奖励
		if (pk_Entry->_lGradeItem == 3)
		{
			itemlist.insert(itemlist.end(), rule->_kItem1.begin(), rule->_kItem1.end());
			itemlist.insert(itemlist.end(), rule->_kItem2.begin(), rule->_kItem2.end());
			itemlist.insert(itemlist.end(), rule->_kItem3.begin(), rule->_kItem3.end());
		}
		else if (pk_Entry->_lGradeItem == 2)
		{
			itemlist.insert(itemlist.end(), rule->_kItem1.begin(), rule->_kItem1.end());
			itemlist.insert(itemlist.end(), rule->_kItem2.begin(), rule->_kItem2.end());
		}
		else if (pk_Entry->_lGradeItem == 1)
		{
			itemlist.insert(itemlist.end(), rule->_kItem1.begin(), rule->_kItem1.end());
		}
		//StringType expstr = rule->_kExp;
		//StringType spstr = rule->_Ksp;
		//StringType moneystr = rule->_kMoney;

		CrossRealmType crossrealmtype = CrossRealmType_Team;
		int crossrealmid = 0;
		pPlace->GetCrossRealm(crossrealmtype, crossrealmid);

		actorbeg = players.begin();
		while (actorbeg != players.end())
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*actorbeg);
			if (pkPlayer != NULL)
			{
				// 经验奖励
				int expItemCount = 0;
				int expRatio = 1;
				//荣光远征
				if(isRongGuangYuanZheng)
				{
					bool firstLight = true;
					const RaidInfo* raidData = pkPlayer->GetRaidAsset()->FindRaid(pPlace->GetMapResID());
					int newScore = score;
					int oldScore = 0;
					int deltaScore = newScore;
					if(NULL != raidData)
					{
						firstLight = false;
						oldScore = raidData->Score();
						deltaScore = newScore>oldScore ? newScore-oldScore : 0;
					}
					pkPlayer->AddRaidStarNumByChapterID(pPlace->GetMapResID(), rule->_chapterID, deltaScore);

					itemlist.clear();
					if(deltaScore != 0)
					{
						//第一次通关  
						if(firstLight)
						{							
							//首通奖励
							itemlist.insert(itemlist.end(), rule->_lFirstLightReward.begin(), rule->_lFirstLightReward.end());

							if (pk_Entry->_lGradeItem == 3)
							{
								itemlist.insert(itemlist.end(), rule->_kItem1.begin(), rule->_kItem1.end());
								itemlist.insert(itemlist.end(), rule->_kItem2.begin(), rule->_kItem2.end());
								itemlist.insert(itemlist.end(), rule->_kItem3.begin(), rule->_kItem3.end());
								expRatio = rule->_BuddyExp3;
								expRatio += rule->_BuddyExp2;
								expRatio += rule->_BuddyExp1;
							}
							else if (pk_Entry->_lGradeItem == 2)
							{
								itemlist.insert(itemlist.end(), rule->_kItem1.begin(), rule->_kItem1.end());
								itemlist.insert(itemlist.end(), rule->_kItem2.begin(), rule->_kItem2.end());
								expRatio = rule->_BuddyExp2;
								expRatio += rule->_BuddyExp1;
							}
							else if (pk_Entry->_lGradeItem == 1)
							{
								itemlist.insert(itemlist.end(), rule->_kItem1.begin(), rule->_kItem1.end());
								expRatio = rule->_BuddyExp1;
							}
						}
						else
						{
							//不是第一次通关
							if(oldScore==1 && newScore==2)
							{
								//领第二个奖励
								itemlist.insert(itemlist.end(), rule->_kItem2.begin(), rule->_kItem2.end());
								expRatio = rule->_BuddyExp2;
							}
							else if(oldScore==1 && newScore==3)
							{
								//领第二个和第三个
								itemlist.insert(itemlist.end(), rule->_kItem2.begin(), rule->_kItem2.end());
								itemlist.insert(itemlist.end(), rule->_kItem3.begin(), rule->_kItem3.end());
								expRatio = rule->_BuddyExp2;
								expRatio += rule->_BuddyExp3;
							}
							else if(oldScore==2 && newScore==3)
							{
								//领第三个奖励
								itemlist.insert(itemlist.end(), rule->_kItem3.begin(), rule->_kItem3.end());
								expRatio = rule->_BuddyExp3;
							}
						}
					}
				}
				//计算经验
				int playerlevel = pkPlayer->GetLevel();
				int _lBaseBuddyXP = 1;
				CF_EXPList::DataEntry* pkData111 = SafeGetCSVFileDataEntryPtr<CF_EXPList>(playerlevel);
				if(pkData111)
					_lBaseBuddyXP = pkData111->_lBaseBuddyXP ;
				expItemCount = _lBaseBuddyXP* expRatio;
				Utility::UT_SIMDataInfo kItem;
				kItem.SetItemInfo(SPEC_ITEM_TID_BUDDY_EXP, expItemCount, 0);
				itemlist.push_back(kItem);

				MG_Notify_RaidSettle* settle = T_NEW_E(MG_Notify_RaidSettle, Memory::MEMCATEGORY_GENERAL);
				//副本结算--埋点日志
				vector<int> nParams;
				nParams.clear();
				nParams.push_back(pPlace->GetMapResID());
				int nLogCode = LC_Helper::SerializeOperateLogCode(FUBEN_LOGTYPE, 0, OPERATE_LOGTYPE1);
				UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, itemlist, pkPlayer,
												  SYS_FUNC_TP_RAID_SETTLE, pPlace->GetMapResID(), rule->_sMailTitle, rule->_sMailDesc,true, false, false, nLogCode,nParams);
			

				TStringVector kParams;
				kParams.push_back("BASE");
				kParams.push_back("PLAYERLV");

				attr_value_type iExp = 0;
				SA_RT_TRIGGER_EXPRESSION(attr_value_type, iExp, rule->_kExp, kParams, LC_Helper::BaseLevelExp(playerlevel), playerlevel);

				MoneyType iCash = 0;
				SA_RT_TRIGGER_EXPRESSION(MoneyType, iCash, rule->_kMoney, kParams, LC_Helper::BaseLevelCash(playerlevel), playerlevel);

				attr_value_type iSP = 0;
				SA_RT_TRIGGER_EXPRESSION(attr_value_type, iSP, rule->_Ksp, kParams, LC_Helper::BaseLevelSP(playerlevel), playerlevel);

				pkPlayer->AddEXP(iExp, SYS_FUNC_TP_RAID_SETTLE, pPlace->GetMapResID());

				pkPlayer->AddCash(iCash, CT_UNBIND_CASH, SYS_FUNC_TP_RAID_SETTLE, pPlace->GetMapResID());

				pkPlayer->AddSkillLevelupPoints(iSP, SYS_FUNC_TP_RAID_SETTLE, pPlace->GetMapResID());

				mem::vector<int32_t> vecBuddyIDs;
				pkPlayer->GetActivateBuddyIDs(vecBuddyIDs);

				T_ASSERT_CRITICAL_B(vecBuddyIDs.size() <= MAX_ACTIVE_BUDDY_NUM);
				for(int i=0;i<vecBuddyIDs.size(); ++i)
				{
					int32_t buddyId = vecBuddyIDs[i];
					LC_ServerPlayer* pkBuddy = pkPlayer->GetActivateBuddyByID(buddyId);
					if (pkBuddy == NULL)
						continue;
					int32_t buddyLV = pkBuddy->GetBuddyLevel();

					++settle->m_buddyCount;
					settle->m_buddyId[i] = buddyId;		
					settle->m_buddyOldLevel[i] = buddyLV;

					//给出战buddy加经验
					TStringVector Params;
					Params.push_back("BASE");
					Params.push_back("BUDDYLV");
					uint32_t buddyExp = 0;
					SA_RT_TRIGGER_EXPRESSION(uint32_t, buddyExp, rule->_kExpBuddy, Params, LC_Helper::BaseLevelExp(buddyLV), buddyLV);
					pkPlayer->AddExpToActivateBuddyByID(buddyId, buddyExp, SYS_FUNC_TP_RAID_SETTLE);
					
					settle->m_buddyExp[i] = buddyExp;
					settle->m_buddyCurrentLevel[i] = pkBuddy->GetBuddyLevel();
				}

				//限时活动事件
				LC_PlayerBase::TimeLimitEvent actEvent;
				actEvent.resMapID = pPlace->GetMapResID();
				pkPlayer->TimeLimitInfoEvent(actEvent);

				//更新副本记录
				bool flag = pkPlayer->GetRaidAsset()->IsAlreadyPassRaid(pPlace->GetMapResID());
				settle->m_bFirstPass = !flag;
				int32_t iPassTime = (int32_t)pPlace->GetRecord()->getPassTime();
				pkPlayer->GetRaidAsset()->UpdateRaid(pkPlayer, pPlace->GetMapResID(), score, iPassTime);

				//记录需要上榜的gamestory的副本完成记录
				pkPlayer->UpdateFinishCountFromGameStoryID(pPlace->GetMapResID());

				bool record = false;
				if (pPlace->IsCrossRealm() && crossrealmtype == CrossRealmType_Team)
				{
					record = pkPlayer->GetCrossRealmTeamAssert()->UpdateFastTime(pPlace->GetMapResID(), iPassTime);
				}
				//记录副本通关次数
				CF_WorldMapList* pkCSVWorldMapList = CF_WorldMapList::GetSingletonPtr();
				CF_WorldMapList::DataEntry* data = pkCSVWorldMapList->GetEntryPtr(pPlace->GetMapResID());
				if (data != NULL)
				{
					AchievementEvent kEvent;
					kEvent.SetConditionInfo(pPlace->GetMapResID());
					kEvent.SetNum(1);
					pkPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_RAID_MAP, kEvent);
				}

				//复活并满血buddy
				int gameStory = pPlace->GetStoryID();				
				CF_GameStory::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_GameStory>(gameStory);
				if(NULL != pkData && pkData->_RecoverBuddyHPWin)
				{
					pkPlayer->ReliveAllBuddyBeforeEnterMap(true, true);
				}

				//打包要发送的通知
				settle->m_exp = (uint32_t)iExp;
				settle->m_sp = (uint32_t)iSP;
				settle->m_money = (uint32_t)iCash;
				settle->m_itemlen = (int8_t)itemlist.size();
				settle->m_Record = record;
				settle->m_controllerLevel =  LC_Helper::CalcPlayerNextLevel(pkPlayer->GetEXP(), pkPlayer->GetLevel());
				T_ASSERT_CRITICAL_B(settle->m_itemlen <= MAX_ITEM_LEN);
				for (int8_t i = 0; i < settle->m_itemlen; ++i)
				{
					settle->m_item[i] = itemlist[i].ID();
					settle->m_itemCount[i] = itemlist[i].ItemCount();
				}
				mem::map<object_id_type, MG_Notify_RaidSettle *>::_Pairib pb = allsettle.insert(std::make_pair(pkPlayer->GetID(), settle));
				T_ASSERT_CRITICAL_B(pb.second);
			}
			++actorbeg;
		}
	} while (0);

	//处理活动奖励
	//活动奖励
	if (pPlace->GetStoryID() > 0)
	{
		GameStory::LC_GameStory_Manager* pkGameStoryMgr = SERVER_GET_GAMESTORY_MANAGER;

		actorbeg = players.begin();
		while (actorbeg != players.end())
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)(*actorbeg);
			if (pkPlayer != NULL)
			{
				GameStory::GSServerReward storyreward;
				pkGameStoryMgr->GetStatReward(pPlace->GetStoryID(), pkPlayer->GetInstance(), storyreward);

				mem::map<object_id_type, MG_Notify_RaidSettle*>::iterator it = allsettle.find(pkPlayer->GetID());
				if (it == allsettle.end())
				{
					MG_Notify_RaidSettle* settle = T_NEW_E(MG_Notify_RaidSettle, Memory::MEMCATEGORY_GENERAL);
					settle->m_exp = (uint32_t)storyreward.dExp;
					settle->m_sp = (uint32_t)storyreward.dSP;
					settle->m_money = (uint32_t)storyreward.dCash;
					settle->m_itemlen = (int8_t)storyreward.kItems.size();
					settle->m_controllerLevel =  LC_Helper::CalcPlayerNextLevel(pkPlayer->GetEXP(), pkPlayer->GetLevel());
					bool flag = pkPlayer->GetRaidAsset()->IsAlreadyPassRaid(pPlace->GetMapResID());
					settle->m_bFirstPass = !flag;
					for (int j = 0; j < (int)settle->m_itemlen; ++j)
					{
						settle->m_item[j] = storyreward.kItems[j].ID();
						settle->m_itemCount[j] = storyreward.kItems[j].ItemCount();
					}
					allsettle[pkPlayer->GetID()] = settle;
				}
				else
				{
					it->second->m_exp += (uint32_t)storyreward.dExp;
					it->second->m_sp += (uint32_t)storyreward.dSP;
					it->second->m_money += (uint32_t)storyreward.dCash;
					it->second->m_controllerLevel = LC_Helper::CalcPlayerNextLevel(pkPlayer->GetEXP(), pkPlayer->GetLevel());
					size_t i = it->second->m_itemlen;
					it->second->m_itemlen += (int8_t)storyreward.kItems.size();
					for (size_t j = 0; j < storyreward.kItems.size(); ++j, ++i)
					{
						it->second->m_item[i] = storyreward.kItems[j].ID();
						it->second->m_itemCount[i] = storyreward.kItems[j].ItemCount();
					}
				}
			}
			++actorbeg;
		}
	}
	//发送消息
	mem::map<object_id_type, MG_Notify_RaidSettle*>::iterator beg = allsettle.begin();
	for (; beg != allsettle.end(); ++beg)
	{
		LC_ServerPlayer* pkPlayer = pPlace->GetActorManger().GetPlayer(beg->first);
		if (pkPlayer != NULL)
		{
			LC_ServerMapLogic_Actor * pkMapLogicActor = pPlace->GetActorManger().GetActor(pkPlayer->GetID());
			beg->second->m_isEnd = true;
			beg->second->m_level = level;
			beg->second->m_monsters = (uint32_t)pPlace->GetRecord()->m_KillMonsterRecord;
			beg->second->m_deads = (uint32_t)pPlace->GetRecord()->m_DeadRecord;
			beg->second->m_hasBuddyDead = (uint32_t)pPlace->GetRecord()->m_DeadRecord > 0 ? false : true;
			beg->second->m_preEnd = PreEnd;
			if (!m_Pass && pkMapLogicActor)
			{
				beg->second->m_result = 0;
				beg->second->m_passtime = (int)pkMapLogicActor->GetDuringTime();
			}
			else
			{
				beg->second->m_result = 1;
				beg->second->m_passtime = (uint32_t)pPlace->GetRecord()->getPassTime();
			}
			const unique_id_impl& instance = pkPlayer->GetInstance();
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Settle m_result:%d, serverid:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, m_passtime:%d, MapResID:%d\n", 
				beg->second->m_result, GetGameServerID(), instance.detail(), instance.catagory(), pkPlayer->GetUserID(), beg->second->m_passtime, pPlace->GetMapResID());
			pkPlayer->SendMsgToClient(MGPT_NOTIFY_RAID_SETTLE, beg->second);

			LC_BackPackEntryPtrList blist;
			make_new_item_info(blist, pkPlayer);
			oss_role_copy_exit(pkPlayer, score, beg->second->m_money, beg->second->m_exp, beg->second->m_sp, blist);
			//记录用户打副本的行为
			//TIPS
			//result 填写 数字 0,1，或者2 ，不要写中文。不然影响到任务相关的统计计算。
			//字段名称 类型 含义
			//group_id int 分组标识符
			//type int 副本类型
			//0 单人
			//1 组队
			//result int 副本结果
			//0 未进入
			//1 获胜
			//2 失败
			//level int 当前等级
			//map_id varchar(255) 前往地图id
			//dlevel int 代表副本的难度等级
			//dscore varchar(16) 副本的评分（比如SSS,SS,S,A等）
			//Tip: 分组标示符为组队副本时，对应的组队的id
			//void oss_360_log_dupl(uint32_t group_id, int type, uint32_t result, uint32_t dlevel, uint32_t dscore, LC_ServerPlayer* pPlayer );
			//oss_360_log_dupl(0, 1, 1, score, 1, pkPlayer );
			oss_360_log_dupl(0, GetType(), 1, 1, score,  pkPlayer );
		}
		T_DELETE_E(beg->second, MG_Notify_RaidSettle, Memory::MEMCATEGORY_GENERAL);
	}
}

bool LC_ServerMapLogic_BatchPlay::PlayerSettle(object_id_type playerid)
{
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return false;
	}
	if (!pPlace->CheckPlayerIn(playerid))
	{
		return false;
	}
	if (CheckPlayerSettle(playerid))
	{
		return false;
	}

	LC_ServerPlayer* theplayer = pPlace->GetActorManger().GetPlayer(playerid);
	if (theplayer == NULL)
	{
		return false;
	}
	if (pPlace->GetStoryID() <= 0)
	{
		return false;
	}
	//结算
	GameStory::LC_StoryLogic_Manager* pkStoryManager = SERVER_GET_STORYLOGIC_MANAGER;
	GameStory::GSTranscriptResult result;
	result.lLogicID = pPlace->GetMapLogicID();
	result.lMapResID = pPlace->GetMapResID();
	result.lClearTime = MAX_RAID_CLEAR_RECORD;
	result.kFailIds.insert(playerid);
	pkStoryManager->TranscriptionFinished(pPlace, result);

	GameStory::GSServerReward storyreward;
	GameStory::LC_GameStory_Manager* pkGameStoryMgr = SERVER_GET_GAMESTORY_MANAGER;
	pkGameStoryMgr->GetStatReward(pPlace->GetStoryID(), theplayer->GetInstance(), storyreward);

	MG_Notify_RaidSettle settle;
	settle.m_isEnd = false;
	settle.m_exp = (uint32_t)storyreward.dExp;
	settle.m_sp = (uint32_t)storyreward.dSP;
	settle.m_money = (uint32_t)storyreward.dCash;
	settle.m_itemlen = (int8_t)storyreward.kItems.size();
	for (int j = 0; j < (int)settle.m_itemlen; ++j)
	{
		settle.m_item[j] = storyreward.kItems[j].ID();
		settle.m_itemCount[j] = storyreward.kItems[j].ItemCount();
	}

	settle.m_level = 0;
	settle.m_monsters = (uint32_t)pPlace->GetRecord()->m_KillMonsterRecord;
	settle.m_deads = (uint32_t)pPlace->GetRecord()->m_DeadRecord;
	settle.m_result = 0;
	settle.m_passtime = (uint32_t)pPlace->GetRecord()->getPassTime();
	settle.m_controllerLevel = LC_Helper::CalcPlayerNextLevel(theplayer->GetEXP(), theplayer->GetLevel());
	const unique_id_impl& instance = theplayer->GetInstance();
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "PlayerSettle m_result:%d, serverid:%d, c_cid:%d, c_citizenship:%d, c_uid:%d, m_passtime:%d, MapResID:%d, m_controllerLevel:%d\n", 
		settle.m_result, GetGameServerID(), instance.detail(), instance.catagory(), theplayer->GetUserID(), settle.m_passtime, pPlace->GetMapResID(), settle.m_controllerLevel);
	theplayer->SendMsgToClient(MGPT_NOTIFY_RAID_SETTLE, &settle);
	return true;
}

void LC_ServerMapLogic_BatchPlay::__doStart()//装载
{
	m_State = MapLogic_NormalPlayState_Run;
	BatchPlayEventHandle(m_Config->StartHandle);
	if (CheckReachEvent(true, m_Config->FinishEvent))
	{
		End();
		return;
	}
	if (m_TotalBatchSize <= 0)
		return;
	if (!m_BatchSlot.empty())
	{
		return;
	}
	if (m_CurBatchIndex > 0) //需设置传送点以及开门
	{
		for (int i = 0; i < m_CurBatchIndex; ++i)
		{
			DungeonConfig::BatchEntry batch;
			if (!GetBatch(i, batch))
			{
				break;
			}
			if (batch.FinishHandle.Type == MapLogicEventHandle_CloseDoor || batch.FinishHandle.Type == MapLogicEventHandle_OpenDoor)
			{
				BatchPlayEventHandle(batch.FinishHandle);
			}
			if (batch.StartHandle.Type == MapLogicEventHandle_CloseDoor || batch.StartHandle.Type == MapLogicEventHandle_OpenDoor)
			{
				BatchPlayEventHandle(batch.StartHandle);
			}
		}
	}

	DungeonConfig::BatchEntry batch;
	bool ret = GetBatch(m_CurBatchIndex, batch);
	if (!ret)
	{
		return;
	}
	bool has_elite = false;
	if (m_eliteset.count(m_CurBatchIndex) > 0)
	{
		has_elite = true;
	}
	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	LC_ServerMapLogic_Batch* pkEntry = pkServerMapLogicFactory->RequestBatch();
	if (pkEntry == NULL)
		return;
	if (!pkEntry->init(batch, m_CurBatchIndex, has_elite, this))
	{
		pkEntry->uninit();
		pkServerMapLogicFactory->ReleaseBatch(pkEntry);
		return;
	}
	if (!pkEntry->Start(GET_PROCESS_TIME))
	{
		pkEntry->uninit();
		pkServerMapLogicFactory->ReleaseBatch(pkEntry);
		return;
	}
	this->m_BatchSlot.push_back(pkEntry);
}

void LC_ServerMapLogic_BatchPlay::__checkStart()
{
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return;
	}
	if (m_State != MapLogic_NormalPlayState_Prepare)
	{
		return;
	}
	if (this->m_Config->StartEvent.Type == (int)MapLogicStartEvent_EnterDoor)
	{
		//判断是否有人有人进入door
		int doorindex = this->m_Config->StartEvent.Value1;
		LC_ServerMapLogic_Door* thedoor = pPlace->GetDoorByIndex(doorindex);
		if (thedoor == NULL)
		{
			return;
		}
		LC_ServerNPC* pkServerNPC = pPlace->GetActorManger().GetNpc(thedoor->m_ID);
		if (pkServerNPC == NULL)
		{
			return;
		}

		CF_ActorFilterFunc* pkCSVActorFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
		CF_ActorFilterFunc::DataEntry* pkFilterData = pkCSVActorFilterFunc->GetEntryPtr(0);
		if (NULL == pkFilterData)
		{
			return;
		}

		LC_MapBase* pkMap = pkServerNPC->GetMap();
		if (NULL == pkMap)
			return;

		LC_ActorPtrVector* pkTargetList = pkMap->LockActorsInRect(thedoor->m_Obb, pkFilterData);
		if (pkTargetList->empty())
		{
			return;
		}
		LC_ActorPtrVector::iterator beg = pkTargetList->begin();
		for (; beg != pkTargetList->end(); ++beg)
		{
			if (NULL == (*beg))
			{
				continue;
			}
			LC_ServerMapLogic_Actor* actor = pPlace->GetActorManger().GetActor((*beg)->GetID());
			if (actor != NULL && IS_PLAYER_ID(actor->ActorID) && actor->CheckIn())
			{
				__doStart();
				break;
			}
		}
	}
}

void LC_ServerMapLogic_BatchPlay::__checkEnd()
{
	if (!CheckRun())
	{
		return;
	}
}

bool LC_ServerMapLogic_BatchPlay::CheckReachEvent(bool finish, DungeonConfig::Event& e)
{
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return false;
	}
	if (finish)
	{
		if (e.Type == (int)MapLogicFinishEvent_Finish_Immediately)
		{
			return true;
		}
		else if (e.Type == (int)MapLogicFinishEvent_EnterLogicArea)
		{
			GameLogic::LC_ActorPtrList playerlist;
			pPlace->GetAllRealPlayer(playerlist);
			GameLogic::LC_ActorPtrList::iterator beg = playerlist.begin();
			for (; beg != playerlist.end(); ++beg)
			{
				LC_ServerPlayer* player = (LC_ServerPlayer*)(*beg);
				if (player->GetLogicAreaId() == e.Value1)
				{
					return true;
				}
			}
		}
		else if (e.Type == MapLogicFinishEvent_MonsterCount)
		{
			mem::set<int> setA, setB;
			if (pPlace->GetAllMonsterSize(setA, setB) <= e.Value1)
			{
				return true;
			}
		}
		else if (e.Type == MapLogicFinishEvent_SpecialMonster)
		{
			mem::set<int> npctype;
			npctype.insert(e.Value1);
			mem::set<int> setA;
			if (pPlace->GetAllMonsterSize(npctype, setA) <= e.Value2)
			{
				return true;
			}
		}
		else if (e.Type == MapLogicFinishEvent_Target_Die)
		{
			if (pPlace->GetActorManger().GetInActorSizeInOrganize(m_TargetOrg) <= 0)
			{
				return true;
			}
		}
	}
	else
	{
		if (e.Type == (int)MapLogicStartEvent_PlayerEnter)
		{
			if (pPlace->GetAllRealPlayerSize() > 0)
			{
				return true;
			}
		}
		else if (e.Type == (int)MapLogicStartEvent_Immediately)
		{
			return true;
		}
		else if (e.Type == (int)MapLogicStartEvent_EnterLogicArea)
		{
			GameLogic::LC_ActorPtrList playerlist;
			pPlace->GetAllRealPlayer(playerlist);
			GameLogic::LC_ActorPtrList::iterator beg = playerlist.begin();
			for (; beg != playerlist.end(); ++beg)
			{
				LC_ServerPlayer* player = (LC_ServerPlayer*)(*beg);
				if (player->GetLogicAreaId() == e.Value1)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool LC_ServerMapLogic_BatchPlay::Update(float fCurrentTime, float fDeltaTime)
{
	if (!CheckRun())
	{
		return true;
	}
	__checkStart();
	if (!CheckProcess())
	{
		return true;
	}
	//处理塔防
	if (CheckTarget())
	{
		if (fCurrentTime >= m_TargetTime)
		{
			m_TargetTime = fCurrentTime + 2.0f;
			__sendTargetHp();
		}
	}

	LC_ServerMapLogicFactory* pkServerMapLogicFactory	= SERVER_GET_MAP_LOGIC_FACTORY();
	if (pkServerMapLogicFactory == NULL)
	{
		return false;
	}
	BatchSlotType::iterator beg = m_BatchSlot.begin();
	bool newbatch = false;
	while (beg != m_BatchSlot.end())
	{
		LC_ServerMapLogic_Batch::BatchState ret = (*beg)->get_state();
		if (ret == LC_ServerMapLogic_Batch::BATCHPREPARE)
		{
			(*beg)->update(fCurrentTime, fDeltaTime);
		}
		else if (ret == LC_ServerMapLogic_Batch::BATCHNORMAL)
		{
			(*beg)->update(fCurrentTime, fDeltaTime);
		}
		else if (ret == LC_ServerMapLogic_Batch::BATCHFINISH)
		{
			if (m_CurBatchIndex == (*beg)->get_Index())
			{
				newbatch = true;
			}
			(*beg)->End();
			(*beg)->uninit();
			pkServerMapLogicFactory->ReleaseBatch(*beg);
			beg = m_BatchSlot.erase(beg);
			continue;
		}
		else if (ret == LC_ServerMapLogic_Batch::BATCHNONE)
		{
			break;
		}
		++beg;
	}
	if (newbatch)
	{
		if (m_CurBatchIndex < m_TotalBatchSize - 1)
		{
			LC_ServerMapLogic_Batch* pkEntry = pkServerMapLogicFactory->RequestBatch();
			if (pkEntry == NULL)
			{
				End();
				return false;
			}
			int nextindex = m_CurBatchIndex + 1;
			bool has_elite = false;
			if (m_eliteset.count(nextindex) > 0)
			{
				has_elite = true;
			}
			DungeonConfig::BatchEntry batch;
			bool ret = GetBatch(nextindex, batch);
			if (!ret)
			{
				pkEntry->uninit();
				pkServerMapLogicFactory->ReleaseBatch(pkEntry);
				End();
				return false;
			}
			if (!pkEntry->init(batch, nextindex, has_elite, this))
			{
				pkEntry->uninit();
				pkServerMapLogicFactory->ReleaseBatch(pkEntry);
				End();
				return false;
			}
			if (!pkEntry->Start(fCurrentTime))
			{
				pkEntry->uninit();
				pkServerMapLogicFactory->ReleaseBatch(pkEntry);
				End();
				return false;
			}
			this->m_BatchSlot.push_back(pkEntry);
		}
		++m_CurBatchIndex;
		Protocol::MG_NOTIFY_TRANSCRIPTION_PROGRESS msg;
		GetPlace()->BroadcastMsgToClient(MGPT_NOTIFY_TRANSCRIPTION_PROGRESS, progress2proto(msg));
	}
	//结束逻辑
	if (m_CurBatchIndex >= m_TotalBatchSize)
	{
		SetResult(true);
		End();
		Protocol::MG_NOTIFY_TRANSCRIPTION_PROGRESS msg;
		GetPlace()->BroadcastMsgToClient(MGPT_NOTIFY_TRANSCRIPTION_PROGRESS, progress2proto(msg));
	}
	__checkEnd();
	return true;
}

void LC_ServerMapLogic_BatchPlay::EventNotify(MapLogicEvent& kEvent)
{
	if (!CheckRun())
	{
		return;
	}

	MapLogicEvent kSubEvent = kEvent;
	if (kEvent.Type() == MapLogicNotifyEvent_Org && CheckTarget() && kEvent.Param1() == m_TargetOrg && kEvent.Param2() <= 0)
	{
		if (m_Config->FinishEvent.Type == MapLogicFinishEvent_Target_Die)
		{
			End();
			return;
		}
		kSubEvent.SetType(MapLogicNotifyEvent_Target);
	}
	else if (kEvent.Type() == MapLogicNotifyEvent_LogicArea)
	{
		if (m_Config->StartEvent.Type == (int)MapLogicStartEvent_EnterLogicArea && m_State == MapLogic_NormalPlayState_Prepare)
		{
			int targetlogicarea = m_Config->StartEvent.Value1;
			if (kEvent.Param1() == targetlogicarea)
			{
				__doStart();
			}
		}
	}
	else if (kEvent.Type() == MapLogicNotifyEvent_PlayerEnter)
	{
		if (m_Config->StartEvent.Type == (int)MapLogicStartEvent_PlayerEnter && m_State == MapLogic_NormalPlayState_Prepare)
		{
			__doStart();
		}
	}

	BatchSlotType::iterator beg = m_BatchSlot.begin();
	while (beg != m_BatchSlot.end())
	{
		(*beg)->EventNotify(kSubEvent);
		++beg;
	}
}

void LC_ServerMapLogic_BatchPlay::PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent)
{
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return;
	}
	if (kEvent.Type() == PlayerEvent_Enter || kEvent.Type() == PlayerEvent_Recover)
	{
		Protocol::MG_NOTIFY_TRANSCRIPTION_PROGRESS msg;
		player->SendMsgToClient(MGPT_NOTIFY_TRANSCRIPTION_PROGRESS, progress2proto(msg));
		int stage = 0;
		int stagestate = 0;
		GetStageProgress(stage, stagestate);
		if (stage >= 0)
		{
			pPlace->NotifyGameEventToClient(player, EVENT_TYPE_StageProgressNotify, stage, stagestate);
		}
		if (kEvent.Type() == PlayerEvent_Enter && pPlace->GetStoryID() > 0)
		{
			LC_ServerLogicManager* logicMgr = SERVER_GET_LOGIC_MANAGER();
			GameStory::LC_StoryLogic_Manager* pkGameLogicMgr = logicMgr->GetStoryLogicManager();
			pkGameLogicMgr->TranscriptionEntered(pPlace, player);
		}
	}
}

bool LC_ServerMapLogic_BatchPlay::CheckTarget()
{
	if (!m_TargetRaid)
	{
		return false;
	}
	if (m_TargetOrg <= 0)
	{
		return false;
	}
	return true;
}

bool LC_ServerMapLogic_BatchPlay::SpanTarget(TargetEntry& target)
{
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return false;
	}

	LC_ServerMapLogic_Organize* org = pPlace->GetActorManger().CreateOrganize();
	if (org == NULL)
	{
		return false;
	}

	LC_TranscriptionListEntry transEntry;
	transEntry.m_iMapID = pPlace->GetMapLogicID();
	transEntry.m_kSpawnLocation		= target.Position;
	transEntry.m_iSpawnCharType		= target.TypeId;
	transEntry.m_iZRotAngle = target.Angel;
	LC_ServerNPC* npc = pPlace->SpawnNPC(transEntry, -1, FACTION_TARGET);
	if (npc == NULL)
	{
		return false;
	}
	m_TargetOrg = org->Organize_id;
	pPlace->GetActorManger().AddActor2Organize(npc->GetID(), m_TargetOrg);
	m_TargetRaid = true;
	return true;
}

LC_ServerNPC* LC_ServerMapLogic_BatchPlay::GetTarget()
{
	if (!CheckTarget())
	{
		return NULL;
	}
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return NULL;
	}
	const object_id_set* pkTargets = pPlace->GetActorManger().GetAllActorIDsInOrganize(m_TargetOrg);
	if (NULL == pkTargets || pkTargets->empty())
	{
		return NULL;
	}
	return pPlace->GetActorManger().GetNpc(*pkTargets->begin());
}

void LC_ServerMapLogic_BatchPlay::ClearTarget()
{
	LC_ServerNPC* target = GetTarget();
	if (target == NULL)
	{
		return;
	}
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return;
	}
	pPlace->GetActorManger().DelActor(target->GetID());
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager		= ENGINE_GET_NPC_MANAGER();
	pkWorldManager->RemoveObjectFromMap(target);
	pkNPCManager->DestoryNPC(target);
	pPlace->GetActorManger().UnOrganizeActor(m_TargetOrg);
	m_TargetOrg = 0;
}

void LC_ServerMapLogic_BatchPlay::__sendTargetHp()
{
	if (CheckTarget())
	{
		int64_t percent = 0;
		LC_ServerNPC* target = GetTarget();
		if (target != NULL)
		{
			attr_value_type hpmax = target->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
			attr_value_type curhp = target->GetHP();
			if (hpmax > 0)
			{
				percent = curhp * 100 / hpmax;
			}
		}
		GetPlace()->BroadCastGameEventToClient(EVENT_TYPE_TARGET_HP, (int)percent);
	}
}

bool LC_ServerMapLogic_BatchPlay::BatchPlayEventHandle(DungeonConfig::Event& handledata, MapLogicEvent* pkSrc)
{
	if (handledata.Type == (int)MapLogicEventHandle_None)
	{
		return true;
	}
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if (pPlace == NULL)
	{
		return false;
	}

	LC_MapBase* pkMap = pPlace->GetMap();
	if (NULL == pkMap)
	{
		return false;
	}

	if (handledata.Type == (int)MapLogicEventHandle_TransAll)
	{
		//取传送位置
		if (handledata.Value1 >= GetBatchPosSize() || handledata.Value1 < 0)
		{
			return false;
		}
		PosAndLoad pos;
		GetBatchPos(handledata.Value1, pos);
		GameLogic::LC_ActorPtrList playerlist;
		pPlace->GetAllPlayer(playerlist);
		mem::vector<Utility::UT_Vec3Int> posVec;
		if (!playerlist.empty())
		{
			UT_Vec3Int temp(pos.x, pos.y, pos.z);
			GenRandomPosition(temp, posVec, (int)playerlist.size(), pPlace->GetMapResID());
		}
		GameLogic::LC_ActorPtrList::iterator beg = playerlist.begin();
		int index = 0;
		for (; beg != playerlist.end(); ++beg)
		{
			LC_ServerPlayer* play = (LC_ServerPlayer*)(*beg);
			if (play != NULL)
			{
				LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
				pkServerWorldManager->RequestChangePlayerLocation(play, LMIOT_NPC_TRANSFER, pPlace->GetMapLogicID(), pPlace->GetMapResID(), posVec[index]);
				index++;
			}
		}
		return true;
	}
	else if (handledata.Type == (int)MapLogicEventHandle_Cutscenes)
	{
		pPlace->BroadCastGameEventToClient(EVENT_TYPE_CUTSCENES, handledata.Value1, handledata.Value2);
		return true;
	}
	else if (handledata.Type == (int)MapLogicEventHandle_CloseDoor)
	{
		int doorindex = handledata.Value1;
		//关门
		return pPlace->SetDoorState(doorindex, DoorState_Close);
	}
	else if (handledata.Type == (int)MapLogicEventHandle_OpenDoor)
	{
		int doorindex = handledata.Value1;
		//开门
		return pPlace->SetDoorState(doorindex, DoorState_Open);
	}
	else if (handledata.Type == (int)MapLogicEventHandle_TransLogicArea)
	{
		//取传送位置
		if (handledata.Value1 >= GetBatchPosSize() || handledata.Value1 < 0)
		{
			return false;
		}
		PosAndLoad pos;
		GetBatchPos(handledata.Value1, pos);
		GameLogic::LC_ActorPtrList playerlist;
		pPlace->GetAllPlayer(playerlist);
		mem::vector<Utility::UT_Vec3Int> posVec;
		if (!playerlist.empty())
		{
			UT_Vec3Int temp(pos.x, pos.y, pos.z);
			GenRandomPosition(temp, posVec, (int)playerlist.size(), pPlace->GetMapResID());
		}
		int areaid = handledata.Value2;
		GameLogic::LC_ActorPtrList::iterator beg = playerlist.begin();
		int index = 0;
		for (; beg != playerlist.end(); ++beg)
		{
			LC_ServerPlayer* play = (LC_ServerPlayer*)(*beg);
			if (play != NULL && play->GetCurrentLogicAreaID() != areaid)
			{
				LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
				pkServerWorldManager->RequestChangePlayerLocation(play, LMIOT_TRANSFER, pPlace->GetMapLogicID(), pPlace->GetMapResID(), posVec[index]);
				index++;
			}
		}
		return true;
	}
	else if (handledata.Type == (int)MapLogicEventHandle_GenPortal)
	{
		int posindex = handledata.Value2;
		//int chartype=handledata.Value1;
		//int nextresid=handledata.Value3;
		if (posindex < 0 || posindex >= GetBatchPosSize())
		{
			return false;
		}
		PosAndLoad pos;
		GetBatchPos(posindex, pos);
		pPlace->BroadCastGameEventToClient(EVENT_TYPE_GENPORTAL, handledata.Value1, handledata.Value3, pos.x, pos.y, pos.z);
		return true;
	}
	else if (handledata.Type == (int)MapLogicEventHandle_StageProgressNotify)
	{
		int curindex = handledata.Value1;
		int state = handledata.Value2;
		pPlace->BroadCastGameEventToClient(EVENT_TYPE_StageProgressNotify, curindex, state);
		SetStageProgress(curindex, state);
		return true;
	}
	else if (handledata.Type == (int)MapLogicEventHandle_MsgNotify)
	{
		int curindex = handledata.Value1;
		StringType notify;
		GetNotifyMsg(curindex, notify);

		UT_ServerHelper::_GameNoticeFmort noticeFmort;
		bool res = UT_ServerHelper::GetGameNoticeFromString(notify, noticeFmort);
		if(!res)
		{
			return false;
		}
		noticeFmort.SetChannelType(handledata.Value3);

		MG_GameNotice noticeMsg;
		UT_ServerHelper::FillGameNoticeMsg(noticeFmort, noticeMsg);

		if (handledata.Value2 == 0) //当前副本
		{
			pPlace->BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &noticeMsg);
		}
		else if (handledata.Value2 == 1) //全服
		{
			BroadcastMsgToClient(MGPT_GAME_NOTICE_MSG, &noticeMsg);
		}
		else if (handledata.Value2 == 2) //同组服务器
		{
			LC_ServerLogicManager* pkServerLogicManager = SERVER_GET_LOGIC_MANAGER();
			LC_WorldGuildWarAsset* pWorldGuildWarAsset = pkServerLogicManager->GetGuildWarAssert();
			if(pWorldGuildWarAsset != NULL)
			{
				noticeMsg.m_nGuildGroupID = pWorldGuildWarAsset->GetGroupId();
				SendMsgToSociety(MGPT_GAME_NOTICE_MSG, &noticeMsg);
			}
		}
		else if (handledata.Value2 == 3 && m_pGuildInfo != NULL) //同帮会
		{
			m_pGuildInfo->BroadCastMsg(MGPT_GAME_NOTICE_MSG, &noticeMsg, INVALID_UNIQUE_ID, true);
		}
		return true;
	}
	else if (handledata.Type == (int)MapLogicEventHandle_ClearNPC)
	{
		int cleartype = handledata.Value1;
		if (cleartype == 0) //所有NPC
		{
			mem::set<int> setA, setB;
			pPlace->ClearAllNPC(setA, setB);
		}
		else if (cleartype == 1) //特定阵营
		{
			mem::set<int> npctype;
			mem::set<int> npcfaction;
			if (handledata.Value2 > 0)
			{
				npcfaction.insert(handledata.Value2);
			}
			if (handledata.Value3 > 0)
			{
				npcfaction.insert(handledata.Value3);
			}
			if (handledata.Value4 > 0)
			{
				npcfaction.insert(handledata.Value4);
			}
			if (handledata.Value5 > 0)
			{
				npcfaction.insert(handledata.Value5);
			}
			pPlace->ClearAllNPC(npctype, npcfaction);

		}
		else if (cleartype == 2) //特定类型
		{
			mem::set<int> npctype;
			mem::set<int> npcfaction;
			if (handledata.Value2 > 0)
			{
				npctype.insert(handledata.Value2);
			}
			if (handledata.Value3 > 0)
			{
				npctype.insert(handledata.Value3);
			}
			if (handledata.Value4 > 0)
			{
				npctype.insert(handledata.Value4);
			}
			if (handledata.Value5 > 0)
			{
				npctype.insert(handledata.Value5);
			}
			pPlace->ClearAllNPC(npctype, npcfaction);
		}
		return true;
	}
	else if (handledata.Type == (int)MapLogicEventHandle_SpanDialogueNpc)
	{
		int posindex = handledata.Value2;
		int chartype = handledata.Value1;
		//int dialogid=handledata.Value3;
		if (posindex < 0 || posindex >= GetBatchPosSize())
		{
			return false;
		}
		PosAndLoad pos;
		GetBatchPos(posindex, pos);
		LC_TranscriptionListEntry transEntry;
		transEntry.m_iMapID = pPlace->GetMapLogicID();
		transEntry.m_kSpawnLocation.x		= pos.x;
		transEntry.m_kSpawnLocation.y		= pos.y;
		transEntry.m_kSpawnLocation.z		= pos.z;
		transEntry.m_iSpawnCharType		= chartype;
		pPlace->SpawnDialogNPC(transEntry);
		return true;
	}
	else if (handledata.Type == (int)MapLogicEventHandle_Dialogue)
	{
		return pPlace->BeginDialogue(handledata.Value1);
	}
	else if (handledata.Type == (int)MapLogicEventHandle_SetTransPoint)
	{
		pPlace->SetTransPoint((TransPointType)handledata.Value1, handledata.Value2, handledata.Value3);
		return true;
	}
	return true;
}

bool LC_ServerMapLogic_BatchPlay::CheckEnd()
{
	return m_State == MapLogic_NormalPlayState_End;
}

bool LC_ServerMapLogic_BatchPlay::CheckRun()
{
	return (m_State == MapLogic_NormalPlayState_Prepare || m_State == MapLogic_NormalPlayState_Run);
}

void LC_ServerMapLogic_BatchPlay::Save(Protocol::MG_OUTPUT_CDR& rkStream)
{
	if (m_StageProgress >= 0)
	{
		if (m_StageProgressState != 2)
		{
			rkStream << m_StageProgress;
		}
		else
		{
			rkStream << (m_StageProgress + 1);
		}
	}
	else
	{
		rkStream << 0;
	}
	rkStream << m_SaveBatchIndex;
}

void LC_ServerMapLogic_BatchPlay::Load(Protocol::MG_INPUT_CDR& rkStream)
{
	rkStream >> m_StageProgress;
	if (m_StageProgress <= 0)
	{
		m_StageProgress = -1;
	}
	rkStream >> m_SaveBatchIndex;
	m_CurBatchIndex = m_SaveBatchIndex;

	if (m_CurBatchIndex > 0) //需设置传送点
	{
		for (int i = 0; i < m_CurBatchIndex; ++i)
		{
			DungeonConfig::BatchEntry batch;
			if (!GetBatch(i, batch))
			{
				break;
			}
			if (batch.FinishHandle.Type == MapLogicEventHandle_SetTransPoint)
			{
				BatchPlayEventHandle(batch.FinishHandle);
			}
			if (batch.StartHandle.Type == MapLogicEventHandle_SetTransPoint)
			{
				BatchPlayEventHandle(batch.StartHandle);
			}
		}
	}
}

void LC_ServerMapLogic_BatchPlay::SetGuild(LC_ServerGuildInfo* pGuild)
{
	m_pGuildInfo = pGuild;
}

void LC_ServerMapLogic_BatchPlay::SetStageProgress(int index, int state)
{
	int savestage = 0;
	if (index < 0)
	{
		m_SaveBatchIndex = 0;
		savestage = 0;
	}
	else
	{
		savestage = index;
		if (index != m_StageProgress)
		{
			m_SaveBatchIndex = m_CurBatchIndex;
			if (state == 2)
			{
				++m_SaveBatchIndex;
				++savestage;
			}
		}
		else
		{
			if (state == 2)
			{
				m_SaveBatchIndex = m_CurBatchIndex + 1;
				++savestage;
			}
		}
	}
	m_StageProgress = index;
	m_StageProgressState = state;
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	pkGuildManager->NotifyRaidStage(GetPlace()->GetMapLogicID(), savestage, m_SaveBatchIndex);
}

Protocol::MG_NOTIFY_TRANSCRIPTION_PROGRESS* LC_ServerMapLogic_BatchPlay::progress2proto(Protocol::MG_NOTIFY_TRANSCRIPTION_PROGRESS& msg)
{
	msg.clear();
	if (m_Config->BatchsNotify.empty())
	{
		msg.m_lTotalStep = m_TotalBatchSize;
		msg.m_lCurStep = m_CurBatchIndex;
	}
	else
	{
		msg.m_lTotalStep = (uint32_t)m_Config->BatchsNotify.size();
		int i = 0;
		while (i < (int)msg.m_lTotalStep)
		{
			if (m_CurBatchIndex <= m_Config->BatchsNotify[i])
			{
				break;
			}
			++i;
		}
		msg.m_lCurStep = i;
		msg.m_UserData = (int32_t)m_UserData;
	}
	if (CheckEnd())
	{
		msg.m_Finish = true;
	}
	else
	{
		msg.m_Finish = false;
	}
	return &msg;
}

void LC_ServerMapLogic_BatchPlay::SetResult(bool pass)
{
	m_Pass = pass;
}

bool LC_ServerMapLogic_BatchPlay::GetBatch(int index, DungeonConfig::BatchEntry& batch)
{
	if (index < 0 || index >= m_Config->get_batchcount())
	{
		return false;
	}
	BatchSetType::iterator batchindex;
	BatchSetType::iterator beg = m_Config->Batchs.begin();
	for (; beg != m_Config->Batchs.end(); ++beg)
	{
		LongSetType::iterator indexbeg = beg->Scope.begin();
		for (; indexbeg != beg->Scope.end(); ++indexbeg)
		{
			if ((*indexbeg) == index)
			{
				batch = (*beg);
				return true;
			}
		}
	}
	return false;
}

int LC_ServerMapLogic_BatchPlay::GetBatchPosSize()
{
	return (int)m_Config->PosGroup.size();
}

bool LC_ServerMapLogic_BatchPlay::GetBatchPos(int index, PosAndLoad& pos)
{
	if (index < 0 || index >= GetBatchPosSize())
	{
		return false;
	}
	pos = m_Config->PosGroup[index];
	return true;
}

int LC_ServerMapLogic_BatchPlay::GetBatchMonsterSize()
{
	return (int)m_Config->MonsterGroup.size();
}

bool LC_ServerMapLogic_BatchPlay::GetBatchMonster(int index, mem::vector<MonsterEntry>& monsters)
{
	if (index < 0 || index >= GetBatchMonsterSize())
	{
		return false;
	}
	monsters = m_Config->MonsterGroup[index];
	return true;
}

void LC_ServerMapLogic_BatchPlay::GetNotifyMsg(int index, StringType& msg)
{
	int count = (int)m_Config->NotifyMsgs.size();
	if (index < 0 || index >= count)
	{
		return;
	}
	msg = m_Config->NotifyMsgs[index].c_str();
}


int LC_ServerMapLogic_BatchPlay::GetBatchEliteMonsterSize()
{
	return (int)m_Config->EliteGroup.size();
}

bool LC_ServerMapLogic_BatchPlay::GetBatcElitehMonster(int index, int& elite)
{
	if (index < 0 || index >= GetBatchEliteMonsterSize())
	{
		return false;
	}
	elite = m_Config->EliteGroup[index];
	return true;
}

EliteSkillRuleSetType* LC_ServerMapLogic_BatchPlay::GetBatchEliteSkillRule()
{
	return &(m_Config->EliteSkillRule);
}

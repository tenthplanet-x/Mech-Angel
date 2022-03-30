#include "SM_ServerStateFactory.h"
#include "SM_ServerPlayerState_STAND.h"
#include "SM_ServerPlayerState_RUN.h"
#include "SM_ServerPlayerState_DIZZ.h"
#include "SM_ServerPlayerState_SLEEP.h"
#include "SM_ServerPlayerState_FOLLOW.h"
#include "SM_ServerPlayerState_CHASE.h"
#include "SM_ServerPlayerState_WANDER.h"
#include "SM_ServerPlayerState_TALK.h"
#include "SM_ServerPlayerState_BORN.h"
#include "SM_PlayerState_PUNCH_BACK.h"
#include "SM_PlayerState_PassiveMove.h"

#include "SM_ServerNPCState_IDLE.h"
#include "SM_ServerNPCState_WANDER.h"
#include "SM_ServerNPCState_CHASE.h"
#include "SM_ServerNPCState_RETREAT.h"
#include "SM_ServerNPCState_DIZZ.h"
#include "SM_ServerNPCState_SLEEP.h"
#include "SM_ServerNPCState_FOLLOW.h"
#include "SM_ServerNPCState_TALK.h"
#include "SM_ServerNPCState_BORN.h"
#include "SM_NPCState_DEAD.h"
#include "SM_NPCState_PUNCH_BACK.h"
#include "SM_NPCState_PassiveMove.h"
#include "CF_MonsterAIFunc.h"
#include "UT_DirConfig.h"
#include "LS_LuaStateFactory.h"
#include "UT_Log.h"
#include "UT_BaseStringUtility.h"
using namespace Utility;
using namespace StateMachine;
//--------------------------------------------------------
SM_ServerStateFactory::SM_ServerStateFactory()
: SM_StateFactory()
{
}
//--------------------------------------------------------
SM_ServerStateFactory::~SM_ServerStateFactory()
{
}
//--------------------------------------------------------
bool SM_ServerStateFactory::Init()
{
	SM_StateFactory::Init();
	
	m_ScriptStateName.clear();

	m_ScriptStateName[ST_NONE]			= "xx";
	m_ScriptStateName[ST_NPC_BORN]		= "StateBorn";
	m_ScriptStateName[ST_NPC_IDLE]		= "StateIdle";
	m_ScriptStateName[ST_NPC_WANDER]	= "StateWander";
	m_ScriptStateName[ST_NPC_TALK]		= "StateTalk";
	m_ScriptStateName[ST_NPC_FOLLOW]	= "StateFollow";
	m_ScriptStateName[ST_NPC_CHASE]		= "StateChase";
	m_ScriptStateName[ST_NPC_RETREAT]	= "StateRetreat";
	m_ScriptStateName[ST_NPC_DIZZ]		= "StateDizz";
	m_ScriptStateName[ST_NPC_SLEEP]		= "StateSleep";
	m_ScriptStateName[ST_NPC_DEAD]		= "StateDead";
	m_ScriptStateName[ST_NPC_PUNCH_BACK]= "StatePunchBack";

	m_states_.insert(std::make_pair(ST_PLAYER_STAND, T_NEW_D  SM_ServerPlayerState_STAND));
	m_states_.insert(std::make_pair(ST_PLAYER_RUN, T_NEW_D  SM_ServerPlayerState_RUN));
	m_states_.insert(std::make_pair(ST_PLAYER_DIZZ, T_NEW_D  SM_ServerPlayerState_DIZZ));
	m_states_.insert(std::make_pair(ST_PLAYER_SLEEP, T_NEW_D  SM_ServerPlayerState_SLEEP));
	m_states_.insert(std::make_pair(ST_PLAYER_FOLLOW, T_NEW_D  SM_ServerPlayerState_FOLLOW));
	m_states_.insert(std::make_pair(ST_PLAYER_CHASE, T_NEW_D  SM_ServerPlayerState_CHASE));
	m_states_.insert(std::make_pair(ST_PLAYER_WANDER, T_NEW_D  SM_ServerPlayerState_WANDER));
	m_states_.insert(std::make_pair(ST_PLAYER_TALK, T_NEW_D  SM_ServerPlayerState_TALK));
	m_states_.insert(std::make_pair(ST_PLAYER_BORN, T_NEW_D  SM_ServerPlayerState_BORN));
	m_states_.insert(std::make_pair(ST_PLAYER_PUNCH_BACK, T_NEW_D  SM_PlayerState_PUNCH_BACK));
	m_states_.insert(std::make_pair(ST_PLAYER_PASSIVE_MOVE, T_NEW_D SM_PlayerState_PassiveMove));

	m_states_.insert(std::make_pair(ST_NPC_BORN, T_NEW_D  SM_ServerNPCState_BORN));
	m_states_.insert(std::make_pair(ST_NPC_RETREAT, T_NEW_D  SM_ServerNPCState_RETREAT));
	m_states_.insert(std::make_pair(ST_NPC_DIZZ, T_NEW_D  SM_ServerNPCState_DIZZ));
	m_states_.insert(std::make_pair(ST_NPC_SLEEP, T_NEW_D  SM_ServerNPCState_SLEEP));
	m_states_.insert(std::make_pair(ST_NPC_FOLLOW, T_NEW_D  SM_ServerNPCState_FOLLOW));
	m_states_.insert(std::make_pair(ST_NPC_CHASE, T_NEW_D  SM_ServerNPCState_CHASE));
	m_states_.insert(std::make_pair(ST_NPC_IDLE, T_NEW_D  SM_ServerNPCState_IDLE));
	m_states_.insert(std::make_pair(ST_NPC_WANDER, T_NEW_D  SM_ServerNPCState_WANDER));
	m_states_.insert(std::make_pair(ST_NPC_TALK, T_NEW_D  SM_ServerNPCState_TALK));
	m_states_.insert(std::make_pair(ST_NPC_PUNCH_BACK, T_NEW_D  SM_NPCState_PUNCH_BACK));
	m_states_.insert(std::make_pair(ST_NPC_PASSIVE_MOVE, T_NEW_D SM_NPCState_PassiveMove));

	return this->_InitScriptFile();
}
//--------------------------------------------------------
bool SM_ServerStateFactory::_InitScriptFile()
{
	CSVFile::CF_MonsterAIFunc* pkAIList = CSVFile::CF_MonsterAIFunc::GetSingletonPtr();
	CSVFile::CF_MonsterAIFunc::DataEntryMapExternIterator iter = pkAIList->GetIterator(); 
	while (iter.HasMoreElements())
	{
		CSVFile::CF_MonsterAIFunc::DataEntry* pkAIData = iter.PeekNextValuePtr();
		const StringType & kScriptName = pkAIData->_kAIControlScriptPath;
		//如果存在脚本文件名（"xx"是csv文件使用中的字符串占位符）
		if (kScriptName != "" && kScriptName != "xx")
		{
			mem::set<StringType>::_Pairib _pb = m_scriptFileNameList.insert(kScriptName);	//防止重入
			if(_pb.second)
			{
				bool b = this->_InitScriptState(kScriptName);
				if (!b)
					return false;
			}
		}

		iter.MoveNext();
	}

	return true;
}

bool SM_ServerStateFactory::_InitScriptState(const StringType &rkScriptName)
{
	LS_LuaState* pkLuaState = LS_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	StringType kScriptPath = GfxGetDirConfig().GetDir(CF_NODE_AI_SCRIPT);
	StringType kScriptFileFullName = kScriptPath + rkScriptName;

	bool bLoadScript = pkLuaState->DoFile(kScriptFileFullName);
	if(!bLoadScript)
	{
		GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"Script file error [%s]",kScriptFileFullName.c_str());
		T_ASSERT_CRITICAL_S(false);
		return false;
	}

	FilePathNamePair kPathNamePair = UT_BaseStringUtility::SplitFileName(rkScriptName);
	StringType kBaseFileName = kPathNamePair.second;
	FilePrePostFixPair kPrePostFixPair = UT_BaseStringUtility::SplitBaseFileName(kBaseFileName);
	TStringVector kFileNameTable = UT_BaseStringUtility::SplitString(kPrePostFixPair.first,'-');
	StringType kTableName = kFileNameTable[0] + "_AI_State";

	//检查表是否存在
	T_CHECK_LUAOBJ_EXIST(pkLuaState,kTableName);

	bool bReload = false;

	for(int i = ST_NPC_BORN;i < ST_NPC_STATE_END__;i++)
	{
		StringType strStateName = kTableName + "."+ GetScriptStateName(i);
		LS_LuaObject* pkLuaObject = pkLuaState->GetLuaObject(strStateName,bReload);
		if(pkLuaObject)
		{
			SM_StateBase* pkServerNPCState = this->_createState(i);
			if (pkServerNPCState)
			{
				if (pkServerNPCState->InitScriptData(pkLuaState,strStateName))
				{
					m_kScriptStateMap[strStateName] = pkServerNPCState;
				}
				else
				{
					GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"Error in script status[%s]",strStateName.c_str());
					T_SAFE_DELETE(pkServerNPCState);
					return false;
				}
			}
			else
			{
				GfxWriteLog(LOG_ERROR_FILELINE, LOG_SWITCH_ERROR,"Error in script status[%s]",strStateName.c_str());
				return false;
			}
		}
	}

	return true;
}
//--------------------------------------------------------

const StringType& SM_ServerStateFactory::GetScriptStateName( int iStateID )
{
	mem::map<int,StringType>::iterator Iter = m_ScriptStateName.find(iStateID);
	if (Iter != m_ScriptStateName.end())
		return (*Iter).second;

	return m_ScriptStateName[ST_NONE];
}

StateMachine::SM_StateBase* SM_ServerStateFactory::_createState( int iStateID )
{
	SM_StateBase* pkResult = NULL;

	switch (iStateID)
	{
	case ST_PLAYER_STAND:
		pkResult = T_NEW_D SM_ServerPlayerState_STAND;
		break;
	case ST_PLAYER_RUN:
		pkResult = T_NEW_D SM_ServerPlayerState_RUN;
		break;
	case ST_PLAYER_DIZZ:
		pkResult = T_NEW_D SM_ServerPlayerState_DIZZ;
		break;
	case ST_PLAYER_SLEEP:
		pkResult = T_NEW_D SM_ServerPlayerState_SLEEP;
		break;
	case ST_PLAYER_FOLLOW:
		pkResult = T_NEW_D SM_ServerPlayerState_FOLLOW;
		break;
	case ST_PLAYER_CHASE:
		pkResult = T_NEW_D SM_ServerPlayerState_CHASE;
		break;
	case ST_PLAYER_WANDER:
		pkResult = T_NEW_D SM_ServerPlayerState_WANDER;
		break;
	case ST_PLAYER_TALK:
		pkResult = T_NEW_D SM_ServerPlayerState_TALK;
		break;
	case ST_PLAYER_BORN:
		pkResult = T_NEW_D SM_ServerPlayerState_BORN;
		break;
	case ST_PLAYER_PUNCH_BACK:
		pkResult = T_NEW_D SM_PlayerState_PUNCH_BACK;
		break;
	case ST_NPC_IDLE:
		pkResult = T_NEW_D SM_ServerNPCState_IDLE;
		break;
	case ST_NPC_WANDER:
		pkResult = T_NEW_D SM_ServerNPCState_WANDER;
		break;
	case ST_NPC_CHASE:
		pkResult = T_NEW_D SM_ServerNPCState_CHASE;
		break;
	case ST_NPC_RETREAT:
		pkResult = T_NEW_D SM_ServerNPCState_RETREAT;
		break;
	case ST_NPC_DIZZ:
		pkResult = T_NEW_D SM_ServerNPCState_DIZZ;
		break;
	case ST_NPC_SLEEP:
		pkResult = T_NEW_D SM_ServerNPCState_SLEEP;
		break;
	case ST_NPC_TALK:
		pkResult = T_NEW_D SM_ServerNPCState_TALK;
		break;
	case ST_NPC_FOLLOW:
		pkResult = T_NEW_D SM_ServerNPCState_FOLLOW;
		break;
	case ST_NPC_BORN:
		pkResult = T_NEW_D SM_ServerNPCState_BORN;
		break;
	case ST_NPC_DEAD:
		pkResult = T_NEW_D SM_NPCState_DEAD;
		break;
	case ST_NPC_PUNCH_BACK:
		pkResult = T_NEW_D SM_NPCState_PUNCH_BACK;
		break;
	default:
		T_ASSERT_CRITICAL_S(false);
		break;
	}

	return pkResult;
}
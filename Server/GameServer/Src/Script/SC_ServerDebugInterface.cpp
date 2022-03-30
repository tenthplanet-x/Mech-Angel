#include "SC_ServerDebugInterface.h"
#include "SK_Factory.h"
#include "SK_SkillExecute.h"
#include "CF_Helper.h"
#include "LS_LuaState.h"
#include "LS_LuaRegister.h"
#include "LS_LuaCall.h"
#include "CF_ChatMonitor.h"
#include "LC_TextFilterChannel.h"
#include "LC_NPCManagerBase.h"
#include "LC_PlayerManagerBase.h"
#include "LC_NPCManagerBase.h"
#include "LC_ActorAttributeMap.h"
#include "LC_LogicIDManager.h"
#include "LC_ServerTaskManager.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerPlayerManager.h"
#include "LC_ServerItemFlopManager.h"
#include "LC_PKRecorderBase.h"
#include "LC_Dule_Manager.h"
#include "GameServerApp.h"
#include "LC_ServerPlayerGroup.h"
#include "LC_ServerPlayerGroupManager.h"
#include "LC_ServerNPC.h"
#include "LC_VIPAsset.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerGuildManager.h"
#include "LC_ServerWarManager.h"
#include "LC_SequenceManager.h"
#include "LC_GameStory_Manager.h"
#include "UT_ServerHelper.h"
#include "SK_ServerSkillEventManager.h"
#include "LC_GameEventManager.h"
#include "LC_ServerNPCManager.h"
#include "CF_SystemTitleList.h"
#include "UT_Timer.h"
#include "ExportRobot.h"
#include "LC_ServerPlayerEnterLeaveManager.h"
#include "ajson_mem.hpp"
#include "CF_MailItemList.h"
#include "zlib.h"
#include "redis.pb.h"
#include "UT_Redis.h"
#include "CF_Mental.h"
#include "LC_PackEntryFactory.h"
#include "LC_ShortcutBar.h"
#include "UT_ServerCommandParser.h"
#include "UT_GameServerLogManager.h"
#include "LS_LuaStateFactory.h"
#include "LC_ServerChestCSVManager.h"
#include "LC_HttpHandler.h"
#include "ConfigManager.h"
#include "SC_ServerScriptInterface.h"
#include "SC_ServerSkillInterface.h"
#include "ACT_ActivityDataManager.h"
#include "ACT_Monopoly.h"
#include "LC_GuildLadderManager.h"
#include "LC_ServerLogicManager.h"

#include "MG_PACKET.h"
#include "MG_Game2Guild.h"
#include "MG_GM2Login.h"
#include "MG_GM2Game.h"
#include "MG_DebugProtocol.h"
#include "MG_TaskProtocol.h"
#include "MG_Game2Group.h"
#include "MG_Common.h"
#include "MG_ArenaProtocol.h"
#include "NW_ServerSession.h"
#include "NW_Helper.h"

#include "TS_GMRequest.h"

#include "../Buddy/BuddyInstance.h"
#include "GlobalSettings.h"
#include <sstream>
#include "oss_define.h"
#include "oss_360_define.h"
#include "gsnode.h"

#include "CF_SkillBookLevelUp.h"
#include "LC_RankManager.h"
#include "MG_VehicleProtocol.h"
#include "LC_ServerShopAsset.h"
#include "LC_RumorManager.h"
#include "MG_LoginProtocol.h"
#include "LC_ServerTaskManager.h"
#include "CF_WorldMapList.h"
#include "CF_EndlessTower.h"
#include "../Buddy/BuddyInstance.h"
#include "../Buddy/BuddyHelper.h"
#include "LC_ServerTaskMap.h"
#include "systemdefine.h"
using namespace GameLogic;
using namespace Utility;
using namespace Skill;
using namespace CSVFile;
using namespace StateMachine;
using namespace Protocol;
using namespace NetWork;
using namespace GameStory;
using namespace Activity;

class LC_ServerShopManager;

bool SC_ServerDebugInterface::m_gsGMDead  = false;

SC_ServerDebugInterface::SC_ServerDebugInterface()
{

}
SC_ServerDebugInterface::~SC_ServerDebugInterface()
{

}
struct OnlinRole
{
	StringType						uid;
	StringType						charid;
};
AJSON(OnlinRole, (uid)(charid));
struct RolemountInfo
{
	int								mountid;
	int								mountlevel;
	mem::vector<int>				mountskill;
};
AJSON(RolemountInfo, (mountid)(mountlevel)(mountskill));
struct RoleInfo
{
	RolemountInfo					mount;
	mem::vector<int>				skill;
};
AJSON(RoleInfo, (mount)(skill));

struct ItemList
{
	int						id;
	StringType				name;
	StringType				type;
	long					price;
	int						bind_type;
	StringType				description;

};
AJSON(ItemList, (id)(name)(type)(price)(bind_type)(description));

struct Item_list
{

	mem::vector<ItemList>				prop_list;

};
AJSON(Item_list, (prop_list));

struct UnionItem
{
	int						ItemID;
	int						ItemType;
	StringType				ItemName;
	int						DeadlineType;

};
AJSON(UnionItem, (ItemID)(ItemType)(ItemName)(DeadlineType));

struct Union_ItemList
{
	int						Count;
	int						Page;
	mem::vector<UnionItem>	ItemList;
};
AJSON(Union_ItemList, (Count)(Page)(ItemList));

struct Configlist
{
	mem::map<StringType, StringType>				coin_list;
	/*coinconfig			coin_list;*/

};
AJSON(Configlist, (coin_list));

struct GMCharInfo
{
	std::string account_id;
	unique_id_type role_id;
	std::string role_name;
	int gm_level;

};
AJSON(GMCharInfo, (account_id)(role_id)(role_name)(gm_level));



struct RoleBaseInfo
{
	StringType						userid;
	unique_id_type					charid;
	int								charlevel;
};
AJSON(RoleBaseInfo, (userid)(charid)(charlevel));

struct BaseInfo
{
	RoleBaseInfo				Base;
};
AJSON(BaseInfo, (Base));






struct AllTaskList
{
	int						id;
	StringType				name;
	StringType				description;

};
AJSON(AllTaskList, (id)(name)(description));

struct VTaskInfo
{
	mem::vector<AllTaskList>				Task;
};
AJSON(VTaskInfo, (Task));

struct Role_info
{
	StringType				base;
	StringType				vehicle;
	StringType				arrow;
	StringType				devaeye;
	StringType				vehiclefighting;
	StringType				sectskill;
	StringType				mentalskill;
};
AJSON(Role_info, (base)(vehicle)(arrow)(devaeye)(vehiclefighting)(sectskill)(mentalskill));

struct ALL_Role_info
{
	Role_info role_info_property;
};
AJSON(ALL_Role_info, (role_info_property));


struct GMlevel
{
	mem::map<StringType, StringType>				gm_level;
};
AJSON(GMlevel, (gm_level));

struct Propatt
{
	StringType	phd;
	StringType	movespeed;
};
AJSON(Propatt, (phd)(movespeed));

struct Prop_att
{
	Propatt	prop_att;
};
AJSON(Prop_att, (prop_att));



struct Tasktype
{
	mem::map<StringType, StringType>				task_result;

};
AJSON(Tasktype, (task_result));



struct Itemoperation
{
	mem::map<StringType, StringType>   item_operation;
};
AJSON(Itemoperation, (item_operation));



struct Cifutype
{
	mem::map<StringType, StringType>   cifu_type;
};
AJSON(Cifutype, (cifu_type));



struct Dupltype
{
	mem::map<StringType, StringType>   dupl_type;
};
AJSON(Dupltype, (dupl_type));


struct Duplresult
{
	mem::map<StringType, StringType>   dupl_result;

};
AJSON(Duplresult, (dupl_result));



struct Ybincome
{
	mem::map<StringType, StringType>   yb_income;

};
AJSON(Ybincome, (yb_income));


struct Ybconsume
{
	mem::map<StringType, StringType>   yb_consume;

};
AJSON(Ybconsume, (yb_consume));


struct Giftmoneyincome
{
	mem::map<StringType, StringType>   giftmoney_income;

};
AJSON(Giftmoneyincome, (giftmoney_income));


struct Giftmoneyconsume
{
	mem::map<StringType, StringType>   giftmoney_consume;

};
AJSON(Giftmoneyconsume, (giftmoney_consume));


struct Jboperation
{
	mem::map<StringType, StringType>   jb_operation;

};
AJSON(Jboperation, (jb_operation));


struct _Profession
{
	mem::map<StringType, StringType>   profession;

};
AJSON(_Profession, (profession));



struct Viplevel
{
	mem::map<StringType, StringType>   vip_level;


};
AJSON(Viplevel, (vip_level));



struct Scoretype
{
	mem::map<StringType, StringType>   warpot_type;

};
AJSON(Scoretype, (warpot_type));
////////////////////////////////////////
//---------------------------------------------------
void SC_ServerDebugInterface::SendToDebugConsole(const StringType& strTemp)
{
	const PS_ServerInfo stSvrInfo = GetGameServerApp()->GetServerInfo();

	CS_MG_HEAD kMsgHead;
	kMsgHead.m_usMsgType = MGPT_RLT_DEBUG_COMMANDLINE;

	MG_RLT_DebugCommandLine kMsgBody;
	TPSTR2STDSTR(strTemp, kMsgBody.m_strMsg);

	MG_PACKET_ENCODE_HANDLER kPacketTool;
	if (kPacketTool.EncodeCSMsg(kMsgHead, &kMsgBody))
	{
		NW_NetSessionManager* pkManager = NW_NetSessionManager::GetSingletonPtr();
		if (pkManager != NULL)
		{
			NW_NetSessionBase* pkSession = pkManager->GetDebugSession();
			if (pkSession != NULL)
			{
				pkSession->Send(kPacketTool.GetBuff(), kPacketTool.GetMsgSize());
			}
		}
	}
}
//---------------------------------------------------
void SC_ServerDebugInterface::LogError(const StringType& strError)
{
	SendToDebugConsole(strError);
}
uint32_t SC_ServerDebugInterface::GetCurrentTimestamp(void)
{
	return GET_CURRENT_TIMESTAMP_IN_SECONDS();
}
int SC_ServerDebugInterface::GetServerID()
{
	return GetGameServerApp()->GetServerInfo().m_iServerID;
}
uint32_t SC_ServerDebugInterface::GetServerStartTime()
{
	return (uint32_t)GetGameServerApp()->GetServerManager()->GetServerStartTime();
}
int SC_ServerDebugInterface::GetBackPackTypeID()
{
	return 2;
}
//--------------------------------------------------
StringType SC_ServerDebugInterface::FindOwerType(object_id_type nID)
{
	StringType strOwerType = "";

	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(nID);

	if (pkNPC)
	{
		strOwerType = "NPC";
		return strOwerType;
	}

	// 	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
	// 	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(nID);
	//
	// 	if (pkNPC)
	// 	{
	// 		strOwerType = "npc";
	// 		return strOwerType;
	// 	}

	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(nID);

	if (pkPlayer)
	{
		strOwerType = "player";
		return strOwerType;
	}

	strOwerType = "errorid";
	return strOwerType;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::SetState(int nState, int nID, const StringType& strOwerType)
{
	if (strOwerType == "NPC" || strOwerType == "npc")
	{
		LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
		LC_NPCBase* pkNPC = pkNPCManager->FindNPC(nID);

		if (pkNPC)
		{
			pkNPC->SetNextState(nState);
			return true;
		}
	}

	return false;
}
//--------------------------------------------------
LC_ActorAttributeMap* SC_ServerDebugInterface::GetAttributeMap(object_id_type nID)
{
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	LC_PlayerBase* pkPlayer = pkPlayerManager->FindPlayer(nID);

	if (pkPlayer)
		return pkPlayer->GetFinalAttributeMap();

	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(nID);

	if (pkNPC)
		return pkNPC->GetAttributeMap();
	else
		return NULL;
}
//--------------------------------------------------
void SC_ServerDebugInterface::NPCScript(int nNPCID)
{
	//	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
	//	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(nNPCID);
	//#pragma TODO("Fix this!")
	//LS_LuaState* pkLuaState = LC_LuaStateFactory::GetSingletonPtr()->GetLogicLuaState();
	//pkNPC->ControlByScript(pkLuaState,"SampleNPCStateMachine.lua");
}
//--------------------------------------------------
int SC_ServerDebugInterface::GetServerPlayerNum()
{
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	const LC_PlayerIDPtrMap& kPlayerIDPtrMap = pkPlayerManager->GetPlayerIDMap();

	return (int)kPlayerIDPtrMap.size();
}
//--------------------------------------------------
object_id_type SC_ServerDebugInterface::GetPlayerID(int nIndex)
{
	LC_PlayerManagerBase* pkPlayerManager = ENGINE_GET_GAMEPLAYER_MANAGER();
	const LC_PlayerIDPtrMap& kPlayerIDPtrMap = pkPlayerManager->GetPlayerIDMap();

	LC_PlayerIDPtrMap::const_iterator it = kPlayerIDPtrMap.begin();

	if (nIndex <= UInt2Int((unsigned int)kPlayerIDPtrMap.size()))
	{
		for (int i = 1; i < nIndex; i++)
		{
			++it;
		}

		if (it != kPlayerIDPtrMap.end())
		{
			return it->first;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
//--------------------------------------------------
// int SC_ServerDebugInterface::GetServerEvilNPCNum()
// {
// 	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
// 	LC_NPCManagerBase::LC_NPCIDPtrMap kNPCIDPtrMap = pkNPCManager->GetNPCIDMap();
//
//
// 	for ()
// 	{
// 	}
// 	return kNPCIDPtrMap.size();
// }
//--------------------------------------------------
// object_id_type SC_ServerDebugInterface::GetNPCID(int nIndex)
// {
// 	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
// 	LC_NPCManagerBase::LC_NPCIDPtrMap kNPCIDPtrMap = pkNPCManager->GetNPCIDMap();
//
// 	LC_NPCManagerBase::LC_NPCIDPtrMapIter it = kNPCIDPtrMap.begin();
//
// 	if( nIndex <= UInt2Int( kNPCIDPtrMap.size() ) )
// 	{
// 		for(int i = 1; i < nIndex; i++)
// 		{
// 			it++;
// 		}
//
// 		if(it != kNPCIDPtrMap.end())
// 		{
// 			return it->first;
// 		}
// 		else
// 		{
// 			return 0;
// 		}
// 	}
// 	else
// 	{
// 		return 0;
// 	}
// }
//--------------------------------------------------
int SC_ServerDebugInterface::GetServerNPCNum()
{
	LC_NPCManagerBase*		pkNPCManager = ENGINE_GET_NPC_MANAGER();
	const LC_NPCIDPtrMap&	rkNPCIDPtrMap = pkNPCManager->GetNPCIDMap();

	return (int)rkNPCIDPtrMap.size();
}
int32_t SC_ServerDebugInterface::GetMapPlayerCount(int32_t iMapResID)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	mem::list<int32_t> kMapLCIDs;
	pkWorldManager->GetLogicMapIDListFromResID(kMapLCIDs, iMapResID);

	int32_t iPlayerCount = 0;
	mem::list<int32_t>::iterator itLCID = kMapLCIDs.begin();
	for (; itLCID != kMapLCIDs.end(); ++itLCID)
	{
		LC_ServerMapLogic_Place* pkMaplogic = pkWorldManager->GetMapLogic(*itLCID);
		if (NULL == pkMaplogic || pkMaplogic->GetMapLogicType() == MT_NEWBEE)
		{
			continue;
		}
		iPlayerCount += pkMaplogic->GetControllerPlayerCount();
	}
	return iPlayerCount;
}
//--------------------------------------------------
object_id_type SC_ServerDebugInterface::GetNPCID(int nIndex)
{
	LC_NPCManagerBase*	pkNPCManager = ENGINE_GET_NPC_MANAGER();
	LC_NPCIDPtrMap 		kNPCIDPtrMap = pkNPCManager->GetNPCIDMap();
	LC_NPCIDPtrMapIter	it	= kNPCIDPtrMap.begin();

	if (nIndex <= UInt2Int((unsigned int)kNPCIDPtrMap.size()))
	{
		for (int i = 1; i < nIndex; i++)
		{
			++it;
		}

		if (it != kNPCIDPtrMap.end())
		{
			return it->first;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
//--------------------------------------------------
int SC_ServerDebugInterface::GetNPCShopFuncID(int nNPCID)
{
	// 	LC_NPCManagerBase* pkPlayerManager = ENGINE_GET_NPC_MANAGER();
	// 	LC_NPCBase* pkNPC = pkPlayerManager->FindNPC(nNPCID);
	//
	// 	if (pkNPC)
	// 		return pkNPC->GetProperty_ShopFuncID();

	return 0;
}
//--------------------------------------------------
void SC_ServerDebugInterface::SetListeningFlag(int nID, bool bIsListening)
{
	if (IS_NPC_ID(nID))
	{
		LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
		LC_NPCBase* pkNPC = pkNPCManager->FindNPC(nID);
		if (pkNPC)
			pkNPC->SetListeningFlag(bIsListening);
	}

	if (IS_PLAYER_ID(nID))
	{
		LC_ServerPlayer* pkPlayer = _findPlayer(nID);
		if (pkPlayer)
			pkPlayer->SetListeningFlag(bIsListening);
	}
}
//--------------------------------------------------
bool SC_ServerDebugInterface::SetHP(int nPlayerID, attr_value_type v)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;

	//gm sethp 不支持直接死亡
	if(v <= 0 && !m_gsGMDead)
	{
		v = 1;
	}
	pkPlayer->SetHP(v);

	return true;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::SetSkillLevelupPoints(int nPlayerID, int v)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;

	pkPlayer->SetSkillLevelupPoints(v);

	return true;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::SetOwnerMaxMP(object_id_type nPlayerID, int nValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;
	pkPlayer->SetOwnerMaxMP(nValue);
	return true;
}

bool SC_ServerDebugInterface::ModifyOwnerMaxMP(object_id_type nPlayerID, int nDeltaValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;
	pkPlayer->ModifyOwnerMaxMP(nDeltaValue);
	return true;
}

bool SC_ServerDebugInterface::SetOwnerMP(object_id_type nPlayerID, int nValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;

	pkPlayer->SetOwnerMP(nValue);
	return true;
}

bool SC_ServerDebugInterface::ModifyOwnerMP(object_id_type nPlayerID, int nDeltaValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;
	pkPlayer->ModifyOwnerMP(nDeltaValue);
	return true;
}

bool SC_ServerDebugInterface::SyncOwnerMP(object_id_type nPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;
	pkPlayer->SyncOwnerMP();
	return true;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::SetMP(int nPlayerID, attr_value_type v)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;

	pkPlayer->SetMP(v);

	return true;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::AddSkill(int nPlayerID, int lSkillID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;

	int32_t iSlotIndex = -1;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if(NULL == pkPlayer)
		return false;

	BuddyInstance *pBuddy = pkPlayer->GetAssignedBuddyInstance();
	if(NULL == pBuddy)
		return false;

	return pBuddy->LearnSkill(lSkillID, iSlotIndex);
}
bool SC_ServerDebugInterface::SetCombatRecorderEnabled(int nPlayerID, int nStatus)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;

	if ( nStatus != 0 )
		pkPlayer->EnableCombatRecorder(MT_REANSCRIPTION_RANK);
	else
		pkPlayer->DisableCombatRecorder(1, 1000, 1000);

	return true;
}

bool SC_ServerDebugInterface::FinishCombatRecorder(int nPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;

	pkPlayer->FinishCombatRecorder(1, 0, 0, false);

	return true;
}

//--------------------------------------------------
bool SC_ServerDebugInterface::RemoveSkill(int nPlayerID, int lSkillID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	if (!pkPlayer)
		return false;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if(NULL == pkPlayer)
		return false;

	BuddyInstance *pBuddy = pkPlayer->GetAssignedBuddyInstance();
	if(NULL == pBuddy)
		return false;

	return pBuddy->RemoveSkill(lSkillID);
}
//--------------------------------------------------
bool SC_ServerDebugInterface::RemoveAllSkill(int nPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
		return false;
	pkPlayer->GetSkillAsset()->Clear();
	pkPlayer->GetSkillAsset()->UpdateDirtyFlag();
	return true;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::AddSkillBook(object_id_type nPlayerID, int iSkillBookID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if(NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkPlayer)
	{
		return false;
	}

	if(RE_SUCCESS != pkPlayer->ActivateSkillBook(iSkillBookID))
	{
		return false;
	}

	pkPlayer->SyncOpenNewSkillBook(iSkillBookID);
	pkPlayer->SyncSkillBookInfo();
	return true;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::RemoveSkillBook(object_id_type nPlayerID, int iSkillBookID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkPlayer)
		return false;

	int level = pkPlayer->GetSkillBookAsset().GetLevel(iSkillBookID);
	if (RE_SUCCESS != pkPlayer->GetSkillBookAsset().CloseSkillBook(iSkillBookID))
	{
		return false;
	}
	
	pkPlayer->SyncSkillBookInfo();
	return true;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::SkillBookNoteSoul(object_id_type nPlayerID, int32_t id, int32_t item1, int32_t item2, int32_t item3)
{
	/*
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	mem::vector<int32_t> vec;
	vec.push_back(item1);
	vec.push_back(item2);
	vec.push_back(item3);
	pkPlayer->SkillBookNoteSoul(id, vec);
	*/

	return true;
}


bool SC_ServerDebugInterface::SkillBookUpgrade(object_id_type nPlayerID, int32_t id, int pos)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_SkillBookUpgrade rltMsg;
	rltMsg.m_iID =id ;
	rltMsg.m_iPos = pos;
	rltMsg.m_lResult =pkPlayer->SkillBookUpgrade(id, pos);
	pkPlayer->SendMsgToClient(MGPT_RLT_SKILL_BOOK_UPGRADE, &rltMsg);

	if(RE_SKILL_BOOK_UPGRADE_SUCCESS == rltMsg.m_lResult)
	{
		pkPlayer->SyncSkillBookInfo();
	}

	return true;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::QueryUserChallenge(object_id_type nPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();
	ResultType ret = rank_Manager->QueryUserChallenge(pkPlayer);
	if (ret != RE_SUCCESS)
	{
		MG_GG_RESP_Query_User_Challenge sendmsg;
		sendmsg.m_Size = 0;
		sendmsg.m_TotalSize = 0;
		pkPlayer->SendMsgToClient(MGPT_RESP_RANK_QUERY_CHALLENGE_INFO, &sendmsg);
	}

	return true;
}

bool SC_ServerDebugInterface::QueryRankUserInfo(object_id_type nPlayerID, int32_t nRankID)
{
	return true;
}

bool SC_ServerDebugInterface::ActivateStarVein(object_id_type nPlayerID, int32_t nStarVeinID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_Rlt_ActivateStarVein rltMsg;
	rltMsg.m_nStarVeinID = nStarVeinID;
	rltMsg.m_nResult = pkPlayer->ActivateStarVein(nStarVeinID);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncActivateStarVein();
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_ACTIVATE_STARVEIN, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::GetFDRewardTime(object_id_type nPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_RewardTime rltMsg;
	rltMsg.m_nResult = pkPlayer->CanGetFirstDailyChargeReward();
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		rltMsg.m_time = pkPlayer->GetNextFDTime();
	}

	pkPlayer->SendMsgToClient(MG_RLT_REWARD_TIME, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::GetFDFreeReward(object_id_type nPlayerID, int32_t nDay, int32_t nRewardID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_GetFDFreeReward rltMsg;
	rltMsg.m_nDayID = nDay;
	rltMsg.m_nResult = RE_SUCCESS;
	if(nDay != (uint32_t)pkPlayer->GetFDDay())
	{
		rltMsg.m_nResult = RE_FAIL;
	}

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		rltMsg.m_nResult = pkPlayer->GetFreeReward(nDay, nRewardID);
	}

	pkPlayer->SendMsgToClient(MG_RLT_GET_FD_FREE_REWARD, &rltMsg);
	return true;
}

bool SC_ServerDebugInterface::GetFDDisCountReward(object_id_type nPlayerID, int32_t nDay, int32_t nRewardID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_GetFDDisCountReward rltMsg;
	rltMsg.m_nDayID = nDay;
	if(nDay != (uint32_t)pkPlayer->GetFDDay())
	{
		rltMsg.m_nResult = RE_FAIL;
	}

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		rltMsg.m_nResult = pkPlayer->GetDisCountReward(nDay, nRewardID);
	}

	pkPlayer->SendMsgToClient(MG_RLT_GET_FD_DISCOUNT_REWARD, &rltMsg);
	return true;
}

bool SC_ServerDebugInterface::ActivateSub(object_id_type nPlayerID, int32_t subType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return false;
	}

	pkOwnerPlayer->OpenAbility(0, LC_ABT_SUB, subType);
	return true;
}

bool SC_ServerDebugInterface::UpgradeSubordinate(object_id_type nPlayerID, int32_t subType, int32_t payIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MoneyType preYuanBao = pkPlayer->GetUnBindCash().GetMoney();

	MG_RLT_UpgraedeSubordinate rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_iSubType = subType;
	rltMsg.m_iPayIndex = payIndex;

	rltMsg.m_iResult = pkPlayer->Upgrad_Subordinate(subType, payIndex, rltMsg.m_ModifyAttrIDVec, rltMsg.m_ModifyAttrLevelVec);
	rltMsg.m_iStage = pkPlayer->GetSubordinateStage(subType);

	MoneyType nowYuanBao = pkPlayer->GetUnBindCash().GetMoney();
	if (nowYuanBao >= preYuanBao)
	{
		rltMsg.m_iYuanBaoDelta = (int32_t)(nowYuanBao - preYuanBao);
	}
	else
	{
		rltMsg.m_iYuanBaoDelta = (int32_t)(preYuanBao - nowYuanBao);
		rltMsg.m_iYuanBaoDelta = -rltMsg.m_iYuanBaoDelta;
	}
	rltMsg.m_iYuanBaoDelta = (int32_t)(pkPlayer->GetUnBindCash().GetMoney() - preYuanBao);

	pkPlayer->SendMsgToClient(MGPT_RLT_UPGRADE_SUBORDINATE, &rltMsg);
	return true;
}


bool SC_ServerDebugInterface::BuyBuddyGrowthPackage(object_id_type nPlayerID, int32_t buddyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_BuyGrowthPackage rltMsg;
	rltMsg.m_nBuddyID = buddyID;
	rltMsg.m_nResult = pkPlayer->BuyBuddyGrowthPackage(buddyID);
	pkPlayer->SendMsgToClient(MG_RLT_BUY_GROWTH_PACKAGE, &rltMsg);

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncBuddyGrowthPackage();
	}
	return true;
}

bool SC_ServerDebugInterface::GetBuddyGrowthReward(object_id_type nPlayerID, int32_t buddyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_GetGrowthReward rltMsg;
	rltMsg.m_nBuddyID = buddyID;
	rltMsg.m_nResult = pkPlayer->GetBuddyGrowthReward(buddyID);
	pkPlayer->SendMsgToClient(MG_RLT_GET_GROWTH_REWARD, &rltMsg);

	if(RE_SUCCESS==rltMsg.m_nResult || RE_BUDDY_GROWTH_REWARD_PACKET_HAS_FULL==rltMsg.m_nResult)
	{
		pkPlayer->SyncBuddyGrowthPackage();
	}

	return true;
}

bool SC_ServerDebugInterface::CollectionActivation(object_id_type nPlayerID, int32_t collectActivateID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_CollectionActivation rltMsg;
	rltMsg.m_nCollectActivationIDs.push_back(collectActivateID);
	rltMsg.m_nResult = pkPlayer->CollectionActivation(collectActivateID);
	pkPlayer->SendMsgToClient(MG_RLT_COLLECTION_ACTIVATION, &rltMsg);
	return true;
}

bool SC_ServerDebugInterface::CollectionReward(object_id_type nPlayerID, int32_t collectRewardID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_CollectionReward rltMsg;
	rltMsg.m_nCollectRewardID = collectRewardID;
	rltMsg.m_nResult = pkPlayer->CollectionReward(collectRewardID);
	pkPlayer->SendMsgToClient(MG_RLT_COLLECTION_ACTIVATION, &rltMsg);
	return true;
}

bool SC_ServerDebugInterface::TurnPlayer(object_id_type nPlayerID, int16_t nForward)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if ( pkPlayer == NULL )
		return false;

	nForward = nForward % 360;
	pkPlayer->SetForwardDirAngle(nForward);

	MG_SyncPlayerAngle syncMsg2;
	syncMsg2.m_usEntryCount = 0;
	PlayerAngleEntry eeyy;
	eeyy.m_lPlayerID = pkPlayer->GetID();
	eeyy.m_chCurrentDirAngle = LC_Helper::Angle_Float2Char( pkPlayer->GetForwardDirAngle() );
	syncMsg2.m_rEntries[syncMsg2.m_usEntryCount] = eeyy;
	syncMsg2.m_usEntryCount++;

	pkPlayer->BroadCastToArea(MGPT_SYNC_PLAYERS_ANGLE_IN_FOV, &syncMsg2);
	return true;
}

bool SC_ServerDebugInterface::RuneReward(object_id_type nPlayerID, int32_t runeItemID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer->RuneReward(runeItemID);
	return true;
}

bool SC_ServerDebugInterface::RefreshRewardTask(object_id_type nPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_RefershRewardTask rltMsg;
	rltMsg.m_nResult = pkPlayer->RefreshRewardTask();
	pkPlayer->SendMsgToClient(MGPT_RLT_REFRESHREWARDTASK, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::WriteRaidDataTest(object_id_type nPlayerID, int32_t size)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if ( pkPlayer == NULL )
		return false;
	//PS_Transport_Raid_AssetData
	MG_RaidAsset syncMsg;
	//m_kraidAssert.SetDataToProtocolStruct(syncMsg.m_kAssetData);
	uint16_t count = size;
	if (count > MAX_RAID_ASSET_NUM)
	{
		count = MAX_RAID_ASSET_NUM;
	}

	syncMsg.m_kAssetData.m_lTransportNum = count;
	for ( int i = 0; i < count; i++ )
	{
		syncMsg.m_kAssetData.m_akTransportData[i] = i + time(NULL);
	}
	//
	syncMsg.m_kAssetData.m_lRaidNum = count;
	for ( int i = 0; i < count; i++ )
	{
		syncMsg.m_kAssetData.m_akRaidData[i].m_iRaidId=i;
		syncMsg.m_kAssetData.m_akRaidData[i].m_iScore=i+6;
		syncMsg.m_kAssetData.m_akRaidData[i].m_iClearRecord=i;
	}
	syncMsg.m_kAssetData.m_iRaidStep = count /  2;

	pkPlayer->SendMsgToClient(MGPT_SYNC_RAID_ASSET, &syncMsg);
	return true;
}

bool SC_ServerDebugInterface::WriteCharbaseTest(object_id_type nPlayerID, int32_t size)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer->WriteCharbaseTest(size);
	return true;
}

bool SC_ServerDebugInterface::GainOfflineEXP(object_id_type nPlayerID, int32_t gainType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RTL_GainOfflineExp rtlMsg;
	rtlMsg.m_gainType = gainType;
	rtlMsg.m_result = pkPlayer->GainOffLineExp(rtlMsg.m_gainType);
	pkPlayer->SendMsgToClient(MGPT_RLT_GAIN_OFFLINE_EXP, &rtlMsg);

	if (rtlMsg.m_result == RE_SUCCESS)
	{
		MG_RTL_GetOfflineExp rtlMsg;
		rtlMsg.m_nExp = pkPlayer->GetOffLineExp();
		rtlMsg.m_nOffTime = pkPlayer->GetOfflineExpTime();
		pkPlayer->SendMsgToClient(MGPT_RLT_GET_OFFLINE_EXP, &rtlMsg);
	}
	return true;
}

bool SC_ServerDebugInterface::Console(object_id_type nPlayerID, int bEnabled)
{
	UT_GameServerLogManager* pkLogManager = UT_GameServerLogManager::GetSingletonPtr();
	if ( bEnabled )
	{
		pkLogManager->set_console(true);
		freopen("CON", "w", stdout);
	}
	else
	{
		pkLogManager->set_console(false);
		freopen("NUL", "w+", stdout);
	}
	return true;
}

bool SC_ServerDebugInterface::LeaveGame(object_id_type nPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_LeaveGame rltMsg;
	pkPlayer->SendMsgToClient(MGPT_GC_RLT_LEAVE_GAME, &rltMsg);

	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if(enterLeaveManager)
	{
		LC_ServerPlayer* pOwner = pkPlayer->GetOwnerPlayer();
		if (pOwner == NULL)
			return false;

		mem::vector<int32_t> vecBuddyIDs;
		pOwner->GetActivateBuddyIDs( vecBuddyIDs);
		for (int i = 0; i < vecBuddyIDs.size(); i++)
		{
			LC_ServerPlayer* pBuddy = pOwner->GetActivateBuddyByID(vecBuddyIDs[i]);
			enterLeaveManager->UserLeaveServer(pBuddy, LMIOT_LEAVE);
		}
	}

	return true;
}

bool SC_ServerDebugInterface::ActivateSkillBook(object_id_type nPlayerID, int skillBookID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_ActivateSkillBook rltMsg;
	rltMsg.m_nSkillBookID = skillBookID;
	rltMsg.m_nResult = pkPlayer->ActivateSkillBook(skillBookID);
	pkPlayer->SendMsgToClient(MG_RLT_ACTIVATE_SKILLBOOK, &rltMsg);

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncOpenNewSkillBook(skillBookID);
		pkPlayer->SyncSkillBookInfo();
	}
	return true;
}

bool SC_ServerDebugInterface::UnlockSkillBookSlot(object_id_type nPlayerID, int index, int slotID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_Unlock_SkillBookSlot rltMsg;
	rltMsg.m_nIndex = index;
	rltMsg.m_nSlotID = slotID;
	rltMsg.m_nResult = pkPlayer->UnlockSkillBookSlot(index, slotID);
	rltMsg.m_nSlotState = (rltMsg.m_nResult==RE_SUCCESS) ? true : false;
	pkPlayer->SendMsgToClient(MG_RLT_UNLOCK_SKILLBOOKSLOT, &rltMsg);

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncSkillBookSlotInfo();
	}

	return true;
}

bool SC_ServerDebugInterface::WearSkillBook(object_id_type nPlayerID, int index, int slotID, int skillBookID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_WearSkillBook rltMsg;
	rltMsg.m_nIndex = index;
	rltMsg.m_nSlotID = slotID;
	rltMsg.m_nSkillBookID = skillBookID;
	rltMsg.m_nResult = pkPlayer->WearSkillBook(index, slotID, skillBookID);
	pkPlayer->SendMsgToClient(MG_RLT_WEAR_SKILLBOOK, &rltMsg);

	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncSkillBookInfo();
		pkPlayer->SyncSkillBookSlotInfo();
	}
	return true;
}

bool SC_ServerDebugInterface::ReliveBeforeEnterMap(object_id_type nPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if ( pkPlayer == NULL )
		return false;

	MG_RLT_Relive_BeforeEnterMap rltMsg;
	//rltMsg.m_iResult = pkPlayer->ReliveAllBuddy() ? RE_SUCCESS : RE_FAIL;
	pkPlayer->ReliveAllBuddyBeforeEnterMap(true, true);

	pkPlayer->SendMsgToClient(MG_RLT_RELIVE_BEFOREENTERMAP, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::TakeChapterReward(object_id_type nPlayerID, int chapterID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_TakeChapterReward respMsg;
	respMsg.m_lChapterID = chapterID;
	respMsg.m_lResult = pkPlayer->TaskChapterRewardOP(chapterID);
	pkPlayer->SendMsgToClient(MGPT_RLT_TAKECHAPTERREWARD, &respMsg);
	if(RE_SUCCESS == respMsg.m_lResult)
	{
		pkPlayer->SyncTaskChapterRecord();
	}
	return true;
}

bool SC_ServerDebugInterface::BuyItem(object_id_type nPlayerID, int32_t shopID, int32_t count)
{
	return true;
}

//--------------------------------------------------
bool SC_ServerDebugInterface::AddItem(int nPlayerID, int lItemID, int nItemNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(nPlayerID);

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
		return false;

	pkPlayer->AddItem(lItemID, nItemNum);
	pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
	return true;
}
//--------------------------------------------------
LC_ActorBase* SC_ServerDebugInterface::GetActor(int nActorID)
{
	if (!IS_PLAYER_CLASS_ID(nActorID) && !IS_NPC_ID(nActorID))
		return NULL;

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ActorBase* pkActor = (LC_ActorBase*)(pkWorldManager->FindObject(nActorID));
	if (!pkActor)
		return NULL;

	return pkActor;
}
//--------------------------------------------------
void SC_ServerDebugInterface::CreatePlayer(object_id_type lPlayerID, int lCharType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	pkServerWorldManager->AddObjectToMap(1, pkPlayer);
}
//-------------------------------------------------------------
void SC_ServerDebugInterface::ReloadSkillScript(object_id_type lPlayerID)
{
	return;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::AddCash(object_id_type lPlayerID, MoneyType llAddCashValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		if (llAddCashValue > 0)
		{
			/*pkPlayer->GetBindCash().Add(llAddCashValue);
			pkPlayer->GetUnBindCash().Add(llAddCashValue);
			pkPlayer->GetBindYuanBao().Add(llAddCashValue);
			pkPlayer->GetUnBindYuanBao().Add(llAddCashValue);*/
			//pkPlayer->AddCash(llAddCashValue, CT_BIND_CASH, ACST_OTHER, 0);
			pkPlayer->AddCash(llAddCashValue, CT_UNBIND_CASH, SYS_FUNC_TP_GM, 0);
			pkPlayer->AddCash(llAddCashValue, CT_BIND_YUANBAO, SYS_FUNC_TP_GM, 0);
			//pkPlayer->AddCash(llAddCashValue, CT_UNBIND_YUANBAO, ACST_OTHER, 0);
		}
		else
		{
			//pkPlayer->ReduceCash(-llAddCashValue,CT_BIND_CASH,RCST_OTHER,0);
			pkPlayer->ReduceCash(-llAddCashValue, CT_UNBIND_CASH, SYS_FUNC_TP_GM, 0);
			pkPlayer->ReduceCash(-llAddCashValue, CT_BIND_YUANBAO, SYS_FUNC_TP_GM, 0);
			//pkPlayer->ReduceCash(-llAddCashValue,CT_UNBIND_YUANBAO,RCST_OTHER,0);
		}

		return true;
	}

	return false;
}

bool SC_ServerDebugInterface::AddCouragePoint(object_id_type lPlayerID, int lNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		if (lNum > 0)
		{
			pkPlayer->AddCash(lNum, CT_COURAGE_PT, SYS_FUNC_TP_GM, 0);
		}
		else
		{
			pkPlayer->ReduceCash(-lNum, CT_COURAGE_PT, SYS_FUNC_TP_GM, 0);
		}

		return true;
	}

	return false;
}
bool SC_ServerDebugInterface::AddJusticePoint(object_id_type lPlayerID, int lNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		if (lNum > 0)
		{

			pkPlayer->AddCash(lNum, CT_JUSTICE_PT, SYS_FUNC_TP_GM, 0);
			//pkPlayer->AddCash(llAddCashValue, CT_UNBIND_YUANBAO, ACST_OTHER, 0);
		}
		else
		{
			pkPlayer->ReduceCash(-lNum, CT_JUSTICE_PT, SYS_FUNC_TP_GM, 0);
		}

		return true;
	}

	return false;
}
bool SC_ServerDebugInterface::AddMoneyCourage(object_id_type lPlayerID, int lNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		if (lNum > 0)
		{
			pkPlayer->AddCash(lNum, CT_COURAGE, SYS_FUNC_TP_GM, 0);
		}
		else
		{
			pkPlayer->ReduceCash(-lNum, CT_COURAGE, SYS_FUNC_TP_GM, 0);
		}

		return true;
	}

	return false;
}

bool SC_ServerDebugInterface::AddEnergy(object_id_type lPlayerID, int lNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		if (lNum > 0)
		{
			pkPlayer->AddCash(lNum, CT_ENERGY, SYS_FUNC_TP_GM, 0);
		}
		else
		{
			pkPlayer->ReduceCash(-lNum, CT_ENERGY, SYS_FUNC_TP_GM, 0);
		}

		return true;
	}

	return false;
}

bool SC_ServerDebugInterface::BuyInvestmentCard(object_id_type lPlayerID, int typeID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_BuyInvesmentCard rltMsg;
	rltMsg.m_nInvesmentType = typeID;
	rltMsg.m_nResult = pkPlayer->BuyInvestmentCard(typeID);
	pkPlayer->SendMsgToClient(MGPT_RLT_ADDINVESTMENTCARDREWARD, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::AddInvestmentCardReward(object_id_type lPlayerID, int id)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_AddInvestmentCardReward rltMsg;
	rltMsg.m_nID = id;
	rltMsg.m_nResult = pkPlayer->AddInvestmentCardReward(id, rltMsg.m_nRewardType);
	pkPlayer->SendMsgToClient(MGPT_RLT_BUYINVESMENTCARD, &rltMsg);
	return true;
}

bool SC_ServerDebugInterface::GetStarVeinGoalReward(object_id_type lPlayerID, int goalID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	MG_RLT_GetStarVeinReward rltMsg;
	rltMsg.m_nID = goalID;
	rltMsg.m_nResult = pkPlayer->GetStarVeinReward(goalID);
	if(RE_SUCCESS == rltMsg.m_nResult)
	{
		pkPlayer->SyncStarVeinGoalRecord();
	}

	pkPlayer->SendMsgToClient(MGPT_RLT_GET_STARVEIN_REWARD, &rltMsg);

	return true;
}
bool SC_ServerDebugInterface::OneKeyEquipRune(object_id_type lPlayerID, int buddyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer->OneKeyEquipRuneToBuddy(buddyID);
	return true;
}
bool SC_ServerDebugInterface::OneKeyUnEquipRune(object_id_type lPlayerID, int buddyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (!pkPlayer)
	{
		return false;
	}

	pkPlayer->OneKeyUnEquipRuneFromBuddy(buddyID);
	return true;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::AddBindCash(object_id_type lPlayerID, MoneyType lAddCash)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (NULL != pkPlayer)
	{
		if (lAddCash > 0)
		{
			//pkPlayer->GetBindCash().Add(lAddCash);
			//pkPlayer->AddCash(lAddCash, CT_BIND_CASH, ACST_OTHER, 0);
		}
		else
		{
			//pkPlayer->ReduceCash(-lAddCash,CT_BIND_CASH,RCST_OTHER,0);
		}
		return true;
	}

	return false;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::AddUnBindCash(object_id_type lPlayerID, MoneyType lAddCash)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		if (lAddCash > 0)
		{
			//pkPlayer->GetUnBindCash().Add(lAddCash);
			pkPlayer->AddCash(lAddCash, CT_UNBIND_CASH, SYS_FUNC_TP_GM, 0);
		}
		else
		{
			pkPlayer->ReduceCash(-lAddCash, CT_UNBIND_CASH, SYS_FUNC_TP_GM, 0);
		}
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::AddRuneCream(object_id_type lPlayerID, MoneyType value)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		if (value > 0)
		{
			pkPlayer->AddCash(value, CT_RUNE_CREAM, SYS_FUNC_TP_GM, 0);
		}
		else
		{
			pkPlayer->ReduceCash(-value, CT_RUNE_CREAM, SYS_FUNC_TP_GM, 0);
		}
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::AddStarSoul(object_id_type lPlayerID, MoneyType value)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		if (value > 0)
		{
			pkPlayer->AddCash(value, CT_STAR_SOUL, SYS_FUNC_TP_GM, 0);
		}
		else
		{
			pkPlayer->ReduceCash(-value, CT_STAR_SOUL, SYS_FUNC_TP_GM, 0);
		}
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::BroadcastMsg(object_id_type lPlayerID, int nMsgType, int nMsgLevel)
{
	return true;
}

bool SC_ServerDebugInterface::AddPentacle(object_id_type lPlayerID, MoneyType value)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		if (value > 0)
		{
			pkPlayer->AddCash(value, CT_PENTACLE, SYS_FUNC_TP_GM, 0);
		}
		else
		{
			pkPlayer->ReduceCash(-value, CT_PENTACLE, SYS_FUNC_TP_GM, 0);
		}
		return true;
	}
	return false;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::AddExp(object_id_type lPlayerID, int64_t lAddExpValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		if (lAddExpValue > 0)
		{
			pkPlayer->AddExpWithGamePlay(lAddExpValue, SYS_FUNC_TP_GM, 0);
		}
		else
		{
			pkPlayer->ReduceEXP(-lAddExpValue, SYS_FUNC_TP_GM);
		}

		return true;
	}

	return false;
}

bool SC_ServerDebugInterface::AddExpToActivateBuddy(object_id_type lPlayerID, int64_t lAddExpValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return false;

	if (lAddExpValue > 0)
	{
		pkPlayer->AddExpToActivateBuddy(lAddExpValue, SYS_FUNC_TP_GM, 0);
	}

	return true;
}

bool SC_ServerDebugInterface::AddExpToAllBuddy(object_id_type lPlayerID, int64_t lAddExpValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return false;

	if (lAddExpValue > 0)
	{
		pkPlayer->AddExpToAllBuddyInstance(lAddExpValue, SYS_FUNC_TP_GM, 0);
	}

	return true;
}

bool SC_ServerDebugInterface::AddExpToBuddy(object_id_type lPlayerID, int nBuddyId, int64_t lAddExpValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return false;

	switch(nBuddyId)
	{
	case 1:
		{
			MG_REQ_GetSummitSingleData rltMsg;
			rltMsg.m_nSummitType = (uint32_t)lAddExpValue;
			pkPlayer->GetSummitGuessAsset().GetSummitSingleData(pkPlayer, rltMsg);
		}
		break;
	case 2:
		{
			MG_REQ_SetSummitGuessInfo msg;
			msg.m_nSummitType = 2;
			msg.m_nAgainstID = nBuddyId;
			msg.m_nChoseCharID = lAddExpValue;
			pkPlayer->GetSummitGuessAsset().SetSummitGuessInfo(pkPlayer, msg);
		}
		break;
	case 3:
		{
			MG_REQ_GetSummitSupportInfo rltMsg;
			rltMsg.m_nSummitType = 2;
			rltMsg.m_nAgainstID = lAddExpValue;
			pkPlayer->GetSummitGuessAsset().GetSummitSupportInfo(pkPlayer, rltMsg);
		}
		break;
	case 4:
		{
			MG_REQ_GetSummitWiner msg;
			msg.m_nSummitType = 2;
			msg.m_nAgainstID = lAddExpValue;
			pkPlayer->GetSummitGuessAsset().GetSummitWiner(pkPlayer, msg.m_nSummitType, msg.m_nAgainstID);
		}
		break;
	default:
		break;
	}

	if (lAddExpValue > 0)
	{
		pkPlayer->AddExpToActivateBuddyByID( nBuddyId, lAddExpValue, SYS_FUNC_TP_GM, 0);
	}

	return true;
}

bool SC_ServerDebugInterface::SetBaseDataStr( object_id_type lPlayerID, int nIndex, const StringType& strInput )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if ( pkPlayer == NULL )
		return false;

	pkPlayer->SetBaseDataStr(nIndex, strInput);

	return true;
}

bool SC_ServerDebugInterface::EchoBaseDataStr( object_id_type lPlayerID, int nIndex )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if ( pkPlayer == NULL )
		return false;

	pkPlayer->EchoBaseDataStr(nIndex);

	return true;
}

bool SC_ServerDebugInterface::SetBaseDataInt( object_id_type lPlayerID, int nIndex, int nValue )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if ( pkPlayer == NULL )
		return false;

	pkPlayer->SetBaseDataInt(nIndex, nValue);

	return true;
}

bool SC_ServerDebugInterface::EchoBaseDataInt( object_id_type lPlayerID, int nIndex )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if ( pkPlayer == NULL )
		return false;

	pkPlayer->EchoBaseDataInt(nIndex);

	return true;
}

bool SC_ServerDebugInterface::RegisterCombatEvent(object_id_type lPlayerID, int eEventType, int nKey, int nValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer->RegisterCombatEventData((COMBAT_EVENT_TYPE)eEventType, nKey, nValue);

	return true;
}

bool SC_ServerDebugInterface::UnregisterCombatEvent(object_id_type lPlayerID, int eEventType, int nKey)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer->UnregisterCombatEventData((COMBAT_EVENT_TYPE)eEventType, nKey);
	return true;
}
bool SC_ServerDebugInterface::GMDead(object_id_type lPlayerID, int nVal)
{
	m_gsGMDead = (bool)nVal;
	return true;
}

bool SC_ServerDebugInterface::AddLevel(object_id_type lPlayerID, int lAddLevelValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		pkPlayer->SetDebugCode(lAddLevelValue);
		int lCurLevelValue = pkPlayer->GetLevel();
		int lLevel = lCurLevelValue + lAddLevelValue;
#if 0
		// crt error invalid parameter testing code.
		char szExcep[64] = {'\0'};
		StringType szFmt = "test%";
		char t = 9;
		szFmt += t;
		t = -87;
		szFmt += t;
		t = -64;
		szFmt += t;
		t = -34;
		szFmt += t;
		//std::string strFmt = "test%";
		sprintf_s(szExcep, 63, szFmt.c_str());
#endif

		if(lLevel > GetGlobalSetting.nPlayerMaxLevel)
		{
			return false;
		}

		pkPlayer->SendNotifyStrToClient("player levelUp");
		for(int level = lCurLevelValue+1; level<=lLevel; ++level)
		{
			pkPlayer->SetLevel(level);
			oss_360_log_user_upgrade(pkPlayer, SYS_FUNC_TP_GM);
		}

		return true;
	}

	return false;
}

bool SC_ServerDebugInterface::UpgradeBuddyStar(object_id_type lPlayerID, int nBuddyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer->UpgradeBuddyInstanceStar(nBuddyID);
	return true;
}

bool SC_ServerDebugInterface::SetBuddySchemeData( object_id_type lPlayerID, int mapID, int schemeType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}

	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return false;
	}

	mem::vector<BuddyInstance*> vecAllBuddys;
	pkOwnerPlayer->GetAllBuddyInstnace(vecAllBuddys);
	
	int totalSize = vecAllBuddys.size();
	int num = totalSize%3==0 ? 0: 1;
	int groupSize = (totalSize/3) + num;
	int maxGroupSize = groupSize>MAX_ACTIVE_BUDDY_GROUP_SCHEME_NUM ? MAX_ACTIVE_BUDDY_GROUP_SCHEME_NUM : groupSize;

	int k = 0;
	std::vector<PS_SingleSchemeData> schemeData;
	for(int i=1; i<=maxGroupSize; ++i)
	{
		PS_SingleSchemeData data;
		data.m_nSchemeGroupID = i;
		for(int j=0; j<MAX_ACTIVE_BUDDY_NUM&&k<vecAllBuddys.size(); ++j)
		{
			BuddyInstance* pkBuddy = vecAllBuddys[k];
			++k;
			if(NULL != pkBuddy)
			{
				data.m_vSchemeData[j] = pkBuddy->GetTemplateID();
			}
		}
		schemeData.push_back(data);
	}

	pkPlayer->GetBuddySchemeMgr().SetBuddyActivateScheme_Client(pkOwnerPlayer, mapID, schemeType, schemeData);

	return true;
}

bool SC_ServerDebugInterface::SwitchToBuddyScheme( object_id_type lPlayerID, int nType)
{
	return true;
}

bool SC_ServerDebugInterface::ResetBuddyScheme( object_id_type lPlayerID, int nType )
{
	return true;
}

bool SC_ServerDebugInterface::AddBuddyChain( object_id_type lPlayerID, int nIndex, int nBuddyId )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer == NULL)
		return false;

	pkPlayer->BuddyChain_Add(nIndex, nBuddyId);
	return true;
}

bool SC_ServerDebugInterface::DelBuddyChain( object_id_type lPlayerID, int nIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer == NULL)
		return false;

	pkPlayer->BuddyChain_Del(nIndex);
	return true;
}

bool SC_ServerDebugInterface::SwitchBuddyChain( object_id_type lPlayerID, int nIndex, int nBuddyId )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer == NULL)
		return false;

	pkPlayer->BuddyChain_Switch(nIndex, nBuddyId);
	return true;
}

bool SC_ServerDebugInterface::SetBuddyChainCount( object_id_type lPlayerID, int nCount )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer == NULL)
		return false;

	pkPlayer->BuddyChain_SetIndex(nCount);
	return true;
}

bool SC_ServerDebugInterface::SyncBuddyChain( object_id_type lPlayerID )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer == NULL)
		return false;

	pkPlayer->BuddyChain_SyncToClient();
	return true;
}

bool SC_ServerDebugInterface::FireTimerById( object_id_type lPlayerID, int timer_id )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	UT_TimerManager* pkTimerMgr = UT_TimerManager::GetSingletonPtr();
	pkTimerMgr->FireTriTimestamp(timer_id);
	return true;
}

bool SC_ServerDebugInterface::BindBuddyToPlayer(object_id_type lPlayerID, int nIndex, int nBuddyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer == NULL)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
		return false;

	if ( nBuddyID == -1 )
		pkPlayer->UnBindBuddyInstance(nIndex);
	else
		pkPlayer->BindBuddyInstance(nIndex, nBuddyID);

	return true;
}

bool SC_ServerDebugInterface::SavePlayer(object_id_type lPlayerID, int nIsLeave)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;
	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer)
	{
		//pkPlayer->BindBuddyInstance(nBuddyID);
		pkPlayer->SaveCharPB( nIsLeave );
		return true;
	}

	return false;
}

bool SC_ServerDebugInterface::SaveBuddy(object_id_type lPlayerID, int nBuddyID,int nIsLeave)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		//pkPlayer->BindBuddyInstance(nBuddyID);
		return true;
	}

	return false;
}

bool SC_ServerDebugInterface::SwitchBuddy(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		pkPlayer->SwitchCurrentBuddy();
		return true;
	}

	return false;
}

bool SC_ServerDebugInterface::BuddyLevelUp(object_id_type lPlayerID, int nBuddyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer->TryBuddyLevelUp(nBuddyID);
	return true;
}

bool SC_ServerDebugInterface::SwitchLine(object_id_type lPlayerID, int nLineIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	int32_t nMapResId = pkPlayer->GetMapResID();
	LC_ServerPlayerEnterLeaveManager* pkPlayerEnterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	pkPlayerEnterLeaveManager->TrySwitchMapLine( pkPlayer, nMapResId, 0, nLineIndex );
	return true;
}

bool SC_ServerDebugInterface::GuildOfflineSolo(object_id_type lPlayerID, uint64_t nTargetCharId)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	ResultType ret = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_GuildOffline)->ChallengeCheck(pkPlayer, nTargetCharId, 0);
	if (ret != RE_SUCCESS)
	{
		/*MG_FAILED_Challenge sendmsg;
		sendmsg.m_beChallengeCharID = nTargetCharId;
		sendmsg.m_iResult = ret;
		pkPlayer->SendMsgToClient(MGPT_RLT_RANK_CHALLENGE, &sendmsg);*/
		//pkPlayer->SendChallengeResToClient(ret,nTargetCharId,GameLogic::SOLO_DULE_TYPE::eSoloDuleType_Arena );
	}

	return true;
}

bool SC_ServerDebugInterface::SetNewVIPLevel(object_id_type lPlayerID, int lNewVIPLevel)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer->SetNewVipLevel(lNewVIPLevel);

	return true;
}

bool SC_ServerDebugInterface::AddBuddyLevel(object_id_type lPlayerID, int lAddLevelValue, int nBuddyIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		int lCurLevelValue = pkPlayer->GetLevel();
		int lLevel = lCurLevelValue + lAddLevelValue;

		return true;
	}

	return false;
}

//-------------------------------------------------------------
bool SC_ServerDebugInterface::FinishRaid(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer)
	{
		if (LC_Helper::isInTranscript(pkPlayer))
		{
			int maplogicid = pkPlayer->GetMapLogicID();
			LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
			LC_ServerMapLogic_Place* logic = (LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(maplogicid);
			if (logic == NULL)
			{
				return false;
			}
			logic->PreFinsh();
		}
		return true;
	}

	return false;
}
//-----------------------------------------------------------------
//开始行会战
bool SC_ServerDebugInterface::StartGuildWar(int span)
{
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager == NULL)
	{
		return false;
	}
	manager->OrderLocal(span);
	return true;
}
//----------------------------------------------------------------
//结束行会战
bool SC_ServerDebugInterface::EndGuildWar()
{
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager == NULL)
	{
		return false;
	}
	manager->ForceEndLocalWar();
	return true;
}
//------------------------------------------------------------------
//开始跨服行会战
bool SC_ServerDebugInterface::StartCrossRealmGuildWar(int span)
{
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager == NULL)
	{
		return false;
	}
	manager->OrderCrossRealm(span);
	return true;
}
//------------------------------------------------------------------
//结束跨服行会战
bool SC_ServerDebugInterface::EndCrossRealmGuildWar()
{
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager == NULL)
	{
		return false;
	}
	manager->ForceEndCrossRealmWar();
	return true;
}
//------------------------------------------------------------------
bool SC_ServerDebugInterface::ClearGuildRaid(unique_id_type lCharID)
{
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(lCharID);
	if (pkPlayer == NULL)
	{
		return false;
	}
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return false;
	}
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetPlayerGuild(lCharID);
	if (guildinfo != NULL)
	{
		guildinfo->GetGuildBasicInfo().m_GuildRaidPage = 0;
		guildinfo->GetGuildBasicInfo().m_GuildRaidStage = 0;
		guildinfo->GetGuildBasicInfo().m_GuildRaidBatchIndex = 0;
		guildinfo->GetGuildBasicInfo().m_GuildRaidStageSpan.clear();
		guildinfo->UpdateGuildRaid(true);
		pkPlayer->GetPlayerGuildAssert()->SetRewardTimeStamp(0);
	}
	return true;
}
//------------------------------------------------------------------
//开始势力战
bool SC_ServerDebugInterface::StartDomainWar(int span)
{
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager == NULL)
	{
		return false;
	}
	manager->OrderDomain(span);
	return true;
}
//----------------------------------------------------------------
//结束势力战
bool SC_ServerDebugInterface::EndDomainWar()
{
	LC_WarManager* manager = (LC_WarManager*)SERVER_GET_GUILDWAR_MANAGER();
	if (manager == NULL)
	{
		return false;
	}
	manager->EndDomainWar();
	return true;
}
//------------------------------------------------------------------
//设置盟主
bool SC_ServerDebugInterface::SetPowerGuild(const StringType& strOwerType)
{
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	if (pkGuildManager == NULL)
	{
		return false;
	}
	LC_ServerGuildInfo* guildinfo = pkGuildManager->GetGuildInfoByName(strOwerType);
	if (guildinfo != NULL)
	{
		unique_id_vector idList;
		pkGuildManager->ReSetLocalPower(guildinfo->GetId(), GET_CURRENT_TIMESTAMP_IN_SECONDS(), idList);
	}
	return true;

}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::AddSkillPoint(object_id_type lPlayerID, attr_value_type lAddSkillPoint)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer)
	{
		pkPlayer->AddSkillLevelupPoints(lAddSkillPoint, SYS_FUNC_TP_GM, 0);
		pkPlayer->SetTotalSkillLevelupPoints(pkPlayer->GetSkillLevelupPoints());
		return true;
	}
	return false;
}
//-------------------------------------------------------------
void SC_ServerDebugInterface::ChangedCareerType(object_id_type lPlayerID, int nCareerType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer)
	{
		pkPlayer->SetCareerType((CareerType)nCareerType);
	}
}
//-------------------------------------------------------------
void SC_ServerDebugInterface::ChangedWeaponType(object_id_type lPlayerID, int nWeaponType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer)
	{
		pkPlayer->SetWeaponAttribute((WeaponType)nWeaponType);
	}
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::AssignTask(object_id_type lPlayerID, int lTaskID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}

	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL != pkOwnerPlayer)
	{
		LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
		if(NULL != pkTaskManager)
		{
			LC_ServerTaskMap* pkTaskMap  = (LC_ServerTaskMap*)pkOwnerPlayer->GetTaskMap();
			if (pkTaskMap != NULL)
			{
				LC_AssignTaskInfo* pkAssignInfo = pkTaskMap->GetAssignTaskInfo();
				{
					CF_TaskList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_TaskList>(lTaskID);
					if(NULL != pkData)
					{
						int32_t assignType = pkTaskManager->GetAssignType(pkData->_iTaskType);
						return pkAssignInfo->AddTask(assignType, lTaskID);
					}
				}
			}
		}
	}

	return false;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::AcceptTask(object_id_type lPlayerID, int lTaskID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}

	pkPlayer->AcceptTaskOP(lTaskID);
	return true;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::FinishTask(object_id_type lPlayerID, int lTaskID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}
	MG_RLT_FinishTask rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_lTaskID = lTaskID;
	rltMsg.m_iBonusItemIndex = -1;
	rltMsg.m_iResult = pkPlayer->FinishTaskOP(lTaskID);

	pkPlayer->SendMsgToClient(MGPT_RLT_FINISH_TASK, &rltMsg);
	return true;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::CancelTask(object_id_type lPlayerID, int lTaskID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}

	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return false;
	}

	MG_RLT_CancelTask rltMsg;
	rltMsg.m_lPlayerID = pkOwnerPlayer->GetID();
	rltMsg.m_lTaskID = lTaskID;
	rltMsg.m_iResult = pkOwnerPlayer->CancleTaskOP(lTaskID);
	pkPlayer->SendMsgToClient(MGPT_RLT_CANCEL_TASK, &rltMsg);
	return true;
}
//----------------------------------------------------------------
bool SC_ServerDebugInterface::DeleteTask(object_id_type lPlayerID, int lTaskID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}

	LC_TaskManager* pkTaskManager = LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
	{
		return false;
	}

	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return false;
	}

	LC_Task* pkTask = pkTaskManager->GetTask(lTaskID);
	if (pkTask == NULL)
	{
		return false;
	}

	pkPlayer->CancelTask(lTaskID);
	pkTaskMap->RemoveTaskFinishRecord(lTaskID);

	//发送网络应答
	MG_RLT_CancelTask rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_lTaskID = lTaskID;
	rltMsg.m_iResult = RE_TASK_CANCEL_SUCCESS;
	pkPlayer->SendMsgToClient(MGPT_RLT_CANCEL_TASK, &rltMsg);
	return true;
}

bool SC_ServerDebugInterface::JumpTask(object_id_type lPlayerID, int lTaskID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
	{
		return false;
	}

	LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
	if (pkTaskManager == NULL)
	{
		return false;
	}

	LC_TaskMap* pkTaskMap = pkPlayer->GetTaskMap();
	if (pkTaskMap == NULL)
	{
		return false;
	}

	LC_Task* pkTask = pkTaskManager->GetTask(lTaskID);
	if (pkTask == NULL)
	{
		return false;
	}

	LC_Task* pretask = pkTask;
	mem::list<LC_Task*> kTasks;
	do
	{
		int pre_task_id = pretask->GetPreTaskID();
		pretask = pkTaskManager->GetTask(pre_task_id);
		if (pretask)
		{
			kTasks.push_front(pretask);
		}
	} while (pretask != NULL);

	mem::list<LC_Task*>::iterator it = kTasks.begin();
	for (; it != kTasks.end(); ++it)
	{
		LC_Task* curtask = *it;
		if (curtask && !pkTaskMap->IsInFinishedTaskMap(curtask->GetTaskID()))
		{
			if(!pkTaskMap->IsInProcessingTaskMap(curtask->GetTaskID()))
			{
				if(RE_TASK_START_SUCCESS != pkPlayer->AcceptTask(curtask->GetTaskID()))
				{
					break;
				}
			}
			pkPlayer->FinishTaskOP(curtask->GetTaskID(), 10000, true);
		}
	}

	pkTaskManager->ClearPostTask(pkPlayer, pkTask);

	ResultType iResult = pkTask->CanStart(pkPlayer);
	if (RE_TASK_START_FAILED_LEVEL == iResult)
	{
		pkPlayer->SetLevel(pkTask->GetAcceptLevel());
	}

	MG_RLT_AcceptTask rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetID();
	rltMsg.m_lTaskID = lTaskID;
	rltMsg.m_iResult = pkPlayer->AcceptTask(lTaskID);
	pkPlayer->SendMsgToClient(MGPT_RLT_ACCEPT_TASK, &rltMsg);
	
	if(pkPlayer->GetSkyArenaChallengeTimes() >= GetGlobalSetting.iSkyArenaDailyFreeChallengeTimes)
	{
		if (pkPlayer->CheckConsumeItemByID(GetGlobalSetting.iSkyArenaChallengeItemId, 1) == RE_SUCCESS)
			pkPlayer->ConsumeItemsInPackageByID(GetGlobalSetting.iSkyArenaChallengeItemId, 1);
		else
			return true;
	}
	/*
	/*if(lTaskID < 10)
	{
		pkPlayer->AddSkyArenaChallengeTimes(lTaskID);
		MG_Req_G2G_SummitArenaCheckChallenge reqMsg;
		reqMsg.m_nChallengeId = lTaskID;
		SendMsgToGroup(MGPT_SS_RANK_CHALLENGE_RESULT_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}
	else
	{
		MG_Req_G2G_SummitArenaCheckChallenge reqMsg;
		reqMsg.m_nChallengeId = lTaskID;
		SendMsgToGroup(MGPT_SS_RANK_CHALLENGE_RESULT_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}*/
	
	return true;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::SetBuddyRuneSlotState(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
	{
		return false;
	}

	pkPlayer->SetBuddyRuneSlotState();
	return true;
}

bool SC_ServerDebugInterface::SpawnNPCByCount(object_id_type lPlayerID,int lNPCType, int nCount)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager     = ENGINE_GET_NPC_MANAGER();
	LC_LogicIDManager* pkLogicIDManager = LC_LogicIDManager::GetSingletonPtr();

	if (pkPlayer)
	{
		for ( int i = 0; i < nCount; i++ )
		{
			Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
			int nMapID = pkPlayer->GetMapLogicID();

			//获得NPC ID号
			object_id_type lNPCID = pkLogicIDManager->RequestLogicID(LOT_NPC);

			//创建NPC 对象
			LC_NPCBase* pkNPC = pkNPCManager->CreateNPC(lNPCID, lNPCType);
			if (pkNPC == NULL)
				continue;
			pkNPC->InitLogicData();
			pkNPC->SetProperty_WanderRadius(0);
			pkNPC->SetCurrentLocation(kCurrentLocation);
			pkNPC->SetTargetLocation(kCurrentLocation);
			pkNPC->SetSpawnLocation(kCurrentLocation);
			pkNPC->SetSpawnZRotAngle(180);
			pkNPC->SetSpawnPointID(0);
			pkNPC->SetSurvivalStartTime(GET_PROCESS_TIME);
			pkNPC->SetSpawnTime(GET_PROCESS_TIME);

			//将NPC 加入世界
			pkWorldManager->AddObjectToMap(nMapID, pkNPC);
		}
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::SpawnNPC(object_id_type lPlayerID, int lNPCType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	return UT_ServerHelper::SpawnNPC(pkPlayer, lNPCType);
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::SpawnNPCDir(object_id_type lPlayerID, int lNPCType, int dir)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager     = ENGINE_GET_NPC_MANAGER();
	LC_LogicIDManager* pkLogicIDManager = LC_LogicIDManager::GetSingletonPtr();

	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();

		//获得NPC ID号
		object_id_type lNPCID = pkLogicIDManager->RequestLogicID(LOT_NPC);

		//创建NPC 对象
		LC_NPCBase* pkNPC = pkNPCManager->CreateNPC(lNPCID, lNPCType);
		pkNPC->InitLogicData();
		pkNPC->SetProperty_WanderRadius(0);
		pkNPC->SetCurrentLocation(kCurrentLocation);
		pkNPC->SetTargetLocation(kCurrentLocation);
		pkNPC->SetSpawnLocation(kCurrentLocation);
		pkNPC->SetSpawnZRotAngle(dir);
		pkNPC->SetSpawnPointID(0);
		pkNPC->SetSurvivalStartTime(GET_PROCESS_TIME);

		//将NPC 加入世界
		pkWorldManager->AddObjectToMap(nMapID, pkNPC);
		return true;
	}
	return false;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::SpawnNPCByLocation(int lNPCType, int iMapID, int x, int y, int z, int iFactionType)
{
	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager     = ENGINE_GET_NPC_MANAGER();
	LC_LogicIDManager* pkLogicIDManager = LC_LogicIDManager::GetSingletonPtr();

	Utility::UT_Vec3Int kCurrentLocation(x, y, z);

	//获得NPC ID号
	object_id_type lNPCID = pkLogicIDManager->RequestLogicID(LOT_NPC);

	//创建NPC 对象
	LC_NPCBase* pkNPC = pkNPCManager->CreateNPC(lNPCID, lNPCType);
	pkNPC->InitLogicData();
	pkNPC->SetProperty_WanderRadius(0);
	pkNPC->SetCurrentLocation(kCurrentLocation);
	pkNPC->SetTargetLocation(kCurrentLocation);
	pkNPC->SetSpawnLocation(kCurrentLocation);
	pkNPC->SetSpawnZRotAngle(0);
	pkNPC->SetSpawnPointID(0);
	//pkNPC->SetFactionType(iFactionType);

	//将NPC 加入世界
	pkWorldManager->AddObjectToMap(iMapID, pkNPC);
	return true;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::SpawnAutoFitNPC(object_id_type lPlayerID, int lNPCType, int iLevel)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	LC_WorldManagerBase* pkWorldManager = ENGINE_GET_WORLD_MANAGER();
	LC_NPCManagerBase* pkNPCManager     = ENGINE_GET_NPC_MANAGER();
	LC_LogicIDManager* pkLogicIDManager = LC_LogicIDManager::GetSingletonPtr();

	if (NULL != pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();

		//获得NPC ID号
		object_id_type lNPCID = pkLogicIDManager->RequestLogicID(LOT_NPC);

		//创建NPC 对象
		LC_ServerNPC* pkNPC = (LC_ServerNPC*)pkNPCManager->CreateNPC(lNPCID, lNPCType);
		pkNPC->InitLogicData();
		pkNPC->SetProperty_WanderRadius(0);
		pkNPC->SetCurrentLocation(kCurrentLocation);
		pkNPC->SetTargetLocation(kCurrentLocation);
		pkNPC->SetSpawnLocation(kCurrentLocation);
		pkNPC->SetSpawnZRotAngle(0);
		pkNPC->SetSpawnPointID(0);

		//将NPC 加入世界
		pkWorldManager->AddObjectToMap(nMapID, pkNPC);

		pkNPC->AutoFitLevelReInit(iLevel);

		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::SpawnGuildNPC(object_id_type lPlayerID, int lNPCType, uint64_t iGuildId)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}
		LC_TranscriptionListEntry transEntry;
		transEntry.m_iMapID = pkPlace->GetMapLogicID();
		transEntry.m_kSpawnLocation		= kCurrentLocation;
		transEntry.m_iSpawnCharType		= lNPCType;
		transEntry.m_iZRotAngle = 180;
		LC_ServerNPC* pkNpc = pkPlace->SpawnNPC(transEntry, -1, FACTION_GUILDGUARD);
		pkNpc->SetGuildID(iGuildId);
		return true;
	}
	return false;
}
//-------------------------------------------------------------
bool SC_ServerDebugInterface::SpawnServerNPC(object_id_type lPlayerID, int lNPCType, int iServerId)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}
		LC_TranscriptionListEntry transEntry;
		transEntry.m_iMapID = pkPlace->GetMapLogicID();
		transEntry.m_kSpawnLocation		= kCurrentLocation;
		transEntry.m_iSpawnCharType		= lNPCType;
		transEntry.m_iZRotAngle = 180;
		LC_ServerNPC* pkNpc = pkPlace->SpawnNPC(transEntry, -1, FACTION_GUILDGUARD);
		pkNpc->SetServerID(iServerId);
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::TransferToTarget(object_id_type lPlayerID, int eTargetType)
{
	//TargetType
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer = pkPlayer->GetCurrentActivateBuddy();
	if ( pkPlayer == NULL )
		return false;

	SC_ServerSkillInterface* pkSkillInterface =  SC_ServerScriptInterface::GetSingleton().GetSkillInterface();
	if ( pkSkillInterface == NULL )
		return false;

	pkSkillInterface->TransferToTarget( pkPlayer, eTargetType, false );
	return true;
}

bool SC_ServerDebugInterface::SetActivateBuddy(object_id_type lPlayerID, int nIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}
		pkPlayer->SetCurrentBuddy(nIndex);
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::ReliveBuddy(object_id_type lPlayerID, int nIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}
		//LC_TranscriptionListEntry transEntry;
		//transEntry.m_iMapID = pkPlace->GetMapLogicID();
		//transEntry.m_kSpawnLocation		= kCurrentLocation;
		//transEntry.m_iSpawnCharType		= lNPCType;
		//transEntry.m_iZRotAngle = 180;
		//LC_ServerNPC* pkNpc = pkPlace->SpawnNPC(transEntry, -1, FACTION_GUILDGUARD);
		//pkNpc->SetServerID(iServerId);
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::BuyBuddy(object_id_type lPlayerID, int nTemplateID, int nForce)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	if ( nForce == 2 )
	{
		CF_Buddy::DataEntryMapExternIterator it = CF_Buddy::GetSingletonPtr()->GetIterator();
		for(; it.HasMoreElements(); it.MoveNext())
		{
			CF_Buddy::DataEntry* pkData = it.PeekNextValuePtr();
			if ( pkData == NULL )
				continue;
			pkPlayer->BuyBuddy(pkData->_cID, nForce);
		}
	}
	else
		pkPlayer->BuyBuddy(nTemplateID, nForce);
	return true;
}

bool SC_ServerDebugInterface::AddBot(object_id_type lPlayerID, int nBotTemplateId)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->ActivateBot(nBotTemplateId, true);

	return true;
}

bool SC_ServerDebugInterface::UpgradeBot(object_id_type lPlayerID, int nBotTemplateId)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->UpgradeBot(nBotTemplateId);
	return true;
}

bool SC_ServerDebugInterface::SyncBot(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->SyncBotInfoToClient();
	return true;
}

bool SC_ServerDebugInterface::BotBonus(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->TryFetchBotBonus();
	return true;
}

bool SC_ServerDebugInterface::BotBonusSimple(object_id_type lPlayerID, int nType, int nSeconds)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->FetchBotNormalBonus(nSeconds, nType);
	return true;
}

bool SC_ServerDebugInterface::BotBonusNormal(object_id_type lPlayerID, int nCount)
{
	return true;
}

bool SC_ServerDebugInterface::BotBonusTalent(object_id_type lPlayerID, int nCount)
{
	return true;
}

//--------------------------------------------------------------
bool SC_ServerDebugInterface::AddPet(object_id_type lPlayerID, int nPetTemplateId)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->CapturePet(nPetTemplateId);

	return true;
}

bool SC_ServerDebugInterface::EnablePet(object_id_type lPlayerID, int nPetIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->EnablePet(nPetIndex);

	return true;
}

bool SC_ServerDebugInterface::DisablePet(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->DisablePet();

	return true;
}

bool SC_ServerDebugInterface::DecomposePet(object_id_type lPlayerID, int nPetIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->DecomposePet(nPetIndex);

	return true;
}

bool SC_ServerDebugInterface::UpgradePetStarLevel(object_id_type lPlayerID, int nPetIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->UpgradePetStarLevel(nPetIndex, true);

	return true;
}

bool SC_ServerDebugInterface::SyncPet(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (pkPlayer == NULL)
		return false;

	pkPlayer->SyncPet();

	return true;
}
bool SC_ServerDebugInterface::SubScore(object_id_type lPlayerID, int32_t nSubType)
{
	return true;
}

bool SC_ServerDebugInterface::AddDrop(object_id_type lPlayerID, int32_t nDropID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	pkPlayer->AddDropByDisassemble(nDropID);

	return true;
}

bool SC_ServerDebugInterface::SetDenyBuddyDead(object_id_type lPlayerID, int nIndex, int bDenyDead)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	pkPlayer->SetDenyBuddyDead(bDenyDead);

	return true;
}

bool SC_ServerDebugInterface::ClearChallengeCD(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if ( pkPlayer == NULL )
		return false;

	LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
	ResultType ret = dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Arena)->ClearChallengeCD(pkPlayer);
	if (ret != RE_SUCCESS)
	{
		MG_RESP_ClearChallengeCD sendmsg;
		sendmsg.m_iResult = ret;
		pkPlayer->SendMsgToClient(MGPT_RESP_CLEAR_CHALLENGE_CD, &sendmsg);
	}

	return true;
}

bool SC_ServerDebugInterface::ReleaseAllUIOpenLevel(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if ( pkPlayer == NULL )
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if ( pkPlayer == NULL )
		return false;

	pkPlayer->SetOpenLevelCheat(true);
	LC_OpenAbilityAsset& asset = pkPlayer->GetOpenAbilityAsset();
	LC_OpenAbilityEntryMap& mapEntry = asset.GetEntryMap();
	LC_OpenAbilityEntryMap::iterator it = mapEntry.begin();
	for ( ;it != mapEntry.end(); ++it )
	{
		LC_OpenAbilityEntry& entry = it->second;
		LC_OpenAbilityDataEntry* pkDataEntry = entry.GetDataEntry();
		if(NULL!=pkDataEntry && ((pkDataEntry->m_iType!=LC_ABT_TASK) || (pkDataEntry->m_iKeyID!=STT_DAILY_TASK && pkDataEntry->m_iKeyID!=STT_GUILD_TASK)))
		{
			entry.EnableAbility(pkPlayer, time(NULL), true, true);
		}		
	}
	return true;
}

bool SC_ServerDebugInterface::SetDenyDead(object_id_type lPlayerID, int bDenyDead)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	//static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer == NULL)
		return false;

	pkPlayer->SetDenyDead(bDenyDead);

	return true;
}

bool SC_ServerDebugInterface::BuddyDebugMode( object_id_type lPlayerID,  int nIndex, int nMode )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer->SetBuddyControllerDebugMode(nIndex, nMode);

	return true;
}

bool SC_ServerDebugInterface::BuddyDebug(object_id_type lPlayerID, int nMode)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer->SetBuddyAIParam_DebugMode(nMode);

	return true;
}

bool SC_ServerDebugInterface::ActivateSkill( object_id_type lPlayerID, int32_t nBuddyId, int32_t nSkillId )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	int nErr = 0;
	pkPlayer->ActivateSkill(nBuddyId, nSkillId, nErr);

	return true;
}

bool SC_ServerDebugInterface::ModifySkillUsableCount( object_id_type lPlayerID, int32_t nBuddyId, int32_t nSkillId, int32_t nUsableCount )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer->ModifySkillUsableCount( nBuddyId, nSkillId, nUsableCount );

	return true;
}

bool SC_ServerDebugInterface::GetSkillUsableCount( object_id_type lPlayerID,  int32_t nBuddyId, int32_t nSkillId )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	int nRetCode = pkPlayer->GetSkillUsableCount( nBuddyId, nSkillId );

	return true;
}

bool SC_ServerDebugInterface::ModifySkillMaxUsableCount( object_id_type lPlayerID, int32_t nBuddyId, int32_t nSkillId, int32_t nUsableCount )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	pkPlayer->ModifySkillMaxUsableCount( nBuddyId, nSkillId, nUsableCount );

	return true;
}

bool SC_ServerDebugInterface::GetSkillMaxUsableCount( object_id_type lPlayerID,  int32_t nBuddyId, int32_t nSkillId )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	int nRetCode = pkPlayer->GetSkillMaxUsableCount( nBuddyId, nSkillId );

	return true;
}

bool SC_ServerDebugInterface::CastSkill( object_id_type lPlayerID, int32_t nSkillId )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;

	// Call Internal interface.
	UT_Vec3Int rkTarget;
	pkPlayer->GetCurrentLocationWithZ(rkTarget);
	UT_Vec3Int rkLocked;
	pkPlayer->GetCurrentLocationWithZ(rkLocked);

	pkPlayer->CastSkill(nSkillId, GET_PROCESS_TIME, 0, 0, rkTarget, rkTarget, UT_Vec3Int(0, 0, 0), pkPlayer->GetForwardDirAngle() );

	return true;
}

bool SC_ServerDebugInterface::CastBuddySkillBySkillIndex(object_id_type lPlayerID, int32_t buddyIndex, int32_t nSkillIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	IF_A_NA_PLAYER(pkPlayer)
		return false;
	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	IF_A_NA_PLAYER(pkOwnerPlayer)
		return false;

	LC_ServerPlayer* BuddyPlayer = pkOwnerPlayer->GetActivateBuddyByIndex(buddyIndex);
	IF_A_NA_PLAYER(BuddyPlayer)
		return false;

	int templateID = BuddyPlayer->GetBuddyID();
	BuddyInstance* pkBuddy = pkOwnerPlayer->GetBuddyInstanceByID(templateID);
	if(NULL == pkBuddy)
	{
		return false;
	}

	LC_SkillAsset* pkSkillAsset = pkBuddy->GetSkillAsset();
	LC_SkillAssetData* pkSkillAssetData = pkSkillAsset->GetSkillAssetData();
	LC_SkillAssetEntry* pkSkillEntry = pkSkillAssetData->GetSkillAssetEntry((skill_cat_type)SKILL_TYPE_SECT, nSkillIndex);
	if(NULL == pkSkillEntry)
	{
		return false;
	}

	int skillID = pkSkillEntry->GetTypeID();
	if(0 == skillID)
	{
		return false;
	}

	// Call Internal interface.
	UT_Vec3Int rkTarget;
	BuddyPlayer->GetCurrentLocationWithZ(rkTarget);
	UT_Vec3Int rkLocked;
	BuddyPlayer->GetCurrentLocationWithZ(rkLocked);

	UT_Vec3Int vec;
	pkPlayer->CastSkill(skillID, GET_PROCESS_TIME, 0, 0, vec, rkTarget, UT_Vec3Int(0, 0, 0), pkPlayer->GetForwardDirAngle());
	
	return true;
}

//UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD
bool SC_ServerDebugInterface::ActBegin( object_id_type lPlayerID, int nActID )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;
	int ret = 0;
	int nServerID = pkPlayer->GetServerID();
	GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD;
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	//std::string strDetailKey = "act" + ;
	char strDetailKey[64] = { '\0' };
	time_t nTime = time(NULL);
	sprintf( strDetailKey, "act:%d:",  nActID );
	std::string strValue = "init_value";

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	//redispp::StringReply kCharReply = pkRedisConn->hget(sKey, sCharID);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (kCharReply.result())// || !kgmchar.ParseFromString(kCharReply))
	{
		printf( "No result\n" );
		return true;
	}

	pkRedisConn->hset(sKey, strDetailKey, strValue);

	UT_REDIS_CMD_END(ret, pkRedisHandler);

	return true;
}

bool SC_ServerDebugInterface::ActGet( object_id_type lPlayerID, int nActID )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;
	int ret = 0;
	int nServerID = pkPlayer->GetServerID();
	GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD;
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	//std::string strDetailKey = "act" + ;
	char strDetailKey[64] = { '\0' };
	time_t nTime = time(NULL);
	sprintf( strDetailKey, "act:%d:",  nActID );
	std::string strValue = "";

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	//redispp::StringReply kCharReply = pkRedisConn->hget(sKey, sCharID);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (!kCharReply.result())// || !kgmchar.ParseFromString(kCharReply))
	{
		return false;
	}

	strValue = kCharReply;

	//pkRedisConn->hset(sKey, strDetailKey, strValue);

	UT_REDIS_CMD_END(ret, pkRedisHandler);

	return true;
}

bool SC_ServerDebugInterface::ActSet( object_id_type lPlayerID, int nActID, int nValue )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
		return false;
	int ret = 0;
	int nServerID = pkPlayer->GetServerID();
	GameServerApp* pkApp = GetGameServerApp();
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD;
	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	//std::string strDetailKey = "act" + ;
	char strDetailKey[64] = { '\0' };
	time_t nTime = time(NULL);
	sprintf( strDetailKey, "act:%d:",  nActID );
	char strValue[64] = { '\0' };
	sprintf( strValue, "%d", nValue );

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	//redispp::StringReply kCharReply = pkRedisConn->hget(sKey, sCharID);
	redispp::StringReply kCharReply = pkRedisConn->hget(sKey, strDetailKey);
	if (!kCharReply.result())// || !kgmchar.ParseFromString(kCharReply))
	{
		return false;
	}

	pkRedisConn->hset(sKey, strDetailKey, strValue);

	UT_REDIS_CMD_END(ret, pkRedisHandler);

	return true;
}


bool SC_ServerDebugInterface::RedisBeginData(object_id_type lPlayerID, const StringType& strNameSpace, int nKey1, int nKey2, int nKey3)
{
	mem::vector<uint64_t> nKeys;
	nKeys.push_back(nKey1);
	nKeys.push_back(nKey2);
	nKeys.push_back(nKey3);
	bool bResult = UT_ServerHelper::RedisCreateData(strNameSpace.c_str(), nKeys);
	// check result
	return true;
}

bool SC_ServerDebugInterface::RedisGetDataAsString(object_id_type lPlayerID, const StringType& strNameSpace, int nKey1, int nKey2, int nKey3)
{
	mem::vector<uint64_t> nKeys;
	std::string strResult;
	nKeys.push_back(nKey1);
	nKeys.push_back(nKey2);
	nKeys.push_back(nKey3);
	bool bResult = UT_ServerHelper::RedisGetData_String(strNameSpace.c_str(), nKeys, strResult);
	// check result
	return true;
}

bool SC_ServerDebugInterface::RedisSetDataAsString(object_id_type lPlayerID, const StringType& strNameSpace, int nKey1, int nKey2, int nKey3, const StringType& strInput)
{
	mem::vector<uint64_t> nKeys;
	nKeys.push_back(nKey1);
	nKeys.push_back(nKey2);
	nKeys.push_back(nKey3);
	bool bResult = UT_ServerHelper::RedisSetData_String(strNameSpace.c_str(), nKeys, strInput.c_str());
	// check result
	return true;
}

bool SC_ServerDebugInterface::RedisReadElement(object_id_type lPlayerID, const StringType& strNameSpace, uint64_t nKey)
{
	uint64_t nScore = 0;
	uint32_t nV1 = 0;
	uint32_t nV2 = 0;
	int32_t nIndex = 0;
	bool bResult = UT_ServerHelper::RedisRank_ReadElement(strNameSpace.c_str(), nKey, nScore, nIndex, true);
	LC_Helper::DecodeScore(nScore, nV1, nV2);
	nV2 = LC_Helper::TimeStampReverse(nV2);
	GfxWriteLog( LOG_TAG_SYSTEM, LOG_SWITCH_GM, "RedisRank_ReadElement v1: %d v2: %d idx: %d", nV1 , nV2, nIndex);
	return true;
}

bool SC_ServerDebugInterface::RedisWriteElement(object_id_type lPlayerID, const StringType& strNameSpace, uint64_t nKey, uint32_t nScore, uint32_t nParam)
{
	nParam = LC_Helper::TimeStampReverse(nParam);
	uint64_t nInputScore = LC_Helper::EncodeScore(nScore, nParam);
	bool bResult = UT_ServerHelper::RedisRank_WriteElement(strNameSpace.c_str(), nKey, nInputScore, true);
	return true;
}

bool SC_ServerDebugInterface::RedisReadRange(object_id_type lPlayerID, const StringType& strNameSpace, int32_t nTopCount)
{
	mem::vector<uint64_t> vecResult;
	bool bResult = UT_ServerHelper::RedisRank_ReadSimpleRange(strNameSpace.c_str(), vecResult, nTopCount, true );
	int nSize = vecResult.size();
	GfxWriteLog( LOG_TAG_SYSTEM, LOG_SWITCH_GM, "RedisRank_ReadSimpleRange Total result: %d", nSize);
	for (int i = 0; i < nSize; i++)
	{
		GfxWriteLog( LOG_TAG_SYSTEM, LOG_SWITCH_GM, "RedisRank_ReadSimpleRange SingleResult: %d row: %d", vecResult[i], i );	
	}
	return true;
}

bool SC_ServerDebugInterface::RedisCreateSnapshot(object_id_type lPlayerID, const StringType& strNameSpace)
{
	bool bResult = UT_ServerHelper::RedisRank_CreateSnapshot(strNameSpace.c_str(), true);
	return true;
}

//EquipSlotReinforce
bool SC_ServerDebugInterface::EquipSlotReinforce(object_id_type lPlayerID, int buddyId, int equipSlotIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		pkPlayer->ReinforceEquipSlot(0, buddyId, equipSlotIndex);	
		return true;
	}

	return false;
}

//EquipSlotStar
bool SC_ServerDebugInterface::EquipSlotStar(object_id_type lPlayerID, int buddyId, int32_t equipIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		pkPlayer->EquipSlotStar(buddyId, equipIndex);		
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::RuneUpgrade(object_id_type lPlayerID, int32_t buddyIndex, int32_t slotIndex, int32_t targetLevel)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		pkPlayer->RuneUpgrade_New(buddyIndex, slotIndex, targetLevel);
		return true;
	}
	return false;
}


bool SC_ServerDebugInterface::RuneStar(object_id_type lPlayerID, int32_t slotType, int32_t runeIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		pkPlayer->RuneStar_New(slotType, runeIndex);

		return true;
	}
	return false;
}

bool	SC_ServerDebugInterface::ResetRune(object_id_type lPlayerID, int32_t buddyID, int32_t runeIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		pkPlayer->ResetRune_New(buddyID, runeIndex);
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::EquipRune(object_id_type lPlayerID, int32_t buddyID, int32_t srcSlotIndex, int32_t descSlotIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		pkPlayer->EquipRuneToBuddy(0, srcSlotIndex, buddyID, descSlotIndex);

		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::UnEquipRune(object_id_type lPlayerID, int buddyID, int32_t slotIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		pkPlayer->UnEquipRuneFromBuddy(buddyID, slotIndex);

		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::BiographyPartReward(object_id_type lPlayerID, int32_t biographyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		MG_RLT_BiographyPartReward rltMsg;
		rltMsg.m_nBiographyPartID = biographyID;
		rltMsg.m_nResult = pkPlayer->BiographyPartReward(biographyID);
		pkPlayer->SendMsgToClient(MG_RLT_BIOGRAPHYPART_REWARD, &rltMsg);
		if(RE_SUCCESS == rltMsg.m_nResult)
		{
			pkPlayer->SyncBiographyRewardInfo();

			LC_ServerTaskManager* pkTaskManager = (LC_ServerTaskManager*)LC_TaskManager::GetSingletonPtr();
			pkTaskManager->AssignTask(pkPlayer, pkTaskManager->GetAssignType(STT_BIOGRAPHY_TASK), false, NONE_ASSIGN, NONE_ASSIGN, biographyID);
		}
		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::Charge(object_id_type lPlayerID, int RMB)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	return pkPlayer->Charge(RMB, SYS_FUNC_TP_GM_CHARGE," Debug  GM ");
}

bool SC_ServerDebugInterface::GetShopItem(object_id_type lPlayerID, int shopType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		pkPlayer = pkPlayer->GetOwnerPlayer();
		if(pkPlayer)
		{
			Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
			int nMapID = pkPlayer->GetMapLogicID();
			LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
			if (!pkPlace)
			{
				return false;
			}

			MG_RLT_GetShopItem rltMsg;
			rltMsg.m_nShopType = shopType;
			pkPlayer->GetShopItem(shopType, rltMsg.m_nItemInfo);
			pkPlayer->SendMsgToClient(MGPT_RLT_GET_SHOPITEM, &rltMsg);
		}
	}

	return true;
}

bool SC_ServerDebugInterface::MainTaskReachReward(object_id_type lPlayerID, int reachRewardID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		MG_RLT_MainTaskReachReward respMsg;
		respMsg.m_lReachRewardID = reachRewardID;
		respMsg.m_lResult = pkPlayer->MainTaskReachRewardOP(reachRewardID);
		pkPlayer->SendMsgToClient(MGPT_RLT_MAINTASKREACHREWARD, &respMsg);
		if(RE_SUCCESS == respMsg.m_lResult)
		{
			pkPlayer->SyncMainTaskReachRecord();
		}
	}

	return true;
}

bool SC_ServerDebugInterface::ResetShopTimes(object_id_type lPlayerID, int shopType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		MG_RLT_ResetShopTimes rltMsg;
		rltMsg.m_nShopType = shopType;
		rltMsg.m_nResult = pkPlayer->ResetShopTimes(shopType, rltMsg.m_nResetTimes);
		pkPlayer->SendMsgToClient(MG_RLT_RESET_SHOPTIMES, &rltMsg);
	}

	return true;
}
bool SC_ServerDebugInterface::GetShopResetTimes(object_id_type lPlayerID, int shopType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		MG_RLT_GetShopResetTimes rltMsg;
		rltMsg.m_nShopType = shopType;
		rltMsg.m_nResetTimes = pkPlayer->GetShopResetTimes(shopType);
		pkPlayer->SendMsgToClient(MQ_RLT_GET_SHOPRESETTIMES, &rltMsg);
	}

	return true;
}

bool SC_ServerDebugInterface::DecompositionItem(object_id_type lPlayerID, int index1, int index2, int index3, int index4, int index5, int index6)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		std::vector<int32_t> equipIndex;
		if(index1 != -1)
		{
			equipIndex.push_back(index1);
		}
		if(index2 != -1)
		{
			equipIndex.push_back(index2);
		}
		if(index3 != -1)
		{
			equipIndex.push_back(index3);
		}
		if(index4 != -1)
		{
			equipIndex.push_back(index4);
		}
		if(index5 != -1)
		{
			equipIndex.push_back(index5);
		}
		if(index6 != -1)
		{
			equipIndex.push_back(index6);
		}

		MG_RLT_DecompositionItem respMsg;
		respMsg.m_nResult = pkPlayer->DecompositionEquip(equipIndex, respMsg.m_nDecompositionResult);
		pkPlayer->SendMsgToClient(MGPT_RLT_DECOMPOSITION_ITEM, &respMsg);
		if(RE_SUCCESS == respMsg.m_nResult)
		{
			pkPlayer->SyncBackPackToClient(PACK_TYPE_BASIC);
		}

		return true;
	}
	return false;
}

bool SC_ServerDebugInterface::SetDecompositionSetting(object_id_type lPlayerID, int level)
{
	return false;
}

bool SC_ServerDebugInterface::RecommendFriend(object_id_type IPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(IPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return false;
		}

		pkPlayer->RecommendFriend();
		return true;
	}
	return false;
}

void SC_ServerDebugInterface::ExternWarehouse(object_id_type lPlayerID, int32_t rowNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return ;
		}

		pkPlayer->ExternWarehouse(rowNum);
	}
}

bool SC_ServerDebugInterface::GetBuyMapStepInfo(object_id_type IPlayerID, uint32_t storyID)
{
	return false;
}

bool SC_ServerDebugInterface::BuyVIPTimes(object_id_type IPlayerID, uint32_t storyID, uint32_t buyType, uint32_t buyTimes)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(IPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		pkPlayer->BuyVIPTimes(storyID, buyType, buyTimes);
	}
	return false;
}

bool SC_ServerDebugInterface::QuickRaidAdvance(object_id_type IPlayerID, uint32_t mapID)
{
	return false;
}

void SC_ServerDebugInterface::GuildList(object_id_type IPlayerID, int index, int sortType, int sortCol, int  autoAdd, int searchNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(IPlayerID);

	static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	if (pkPlayer)
	{
		Utility::UT_Vec3Int kCurrentLocation = pkPlayer->GetCurrentLocation();
		int nMapID = pkPlayer->GetMapLogicID();
		LC_ServerMapLogic_Place* pkPlace =pkWorldManager->GetMapLogic(nMapID);
		if (!pkPlace)
		{
			return;
		}

		MG_SS_REQ_GuildList kSGMsg;
		kSGMsg.m_ServerId = pkPlayer->GetCitizenship();
		kSGMsg.m_uiIndex = index;
		kSGMsg.m_uiCount = searchNum;
		kSGMsg.m_SortType =sortType;
		kSGMsg.m_SortCol = sortCol;
		kSGMsg.m_AutoAdd = autoAdd;
		SendMsgToSociety(MGPT_GGU_REQ_GUILDLIST, &kSGMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}
}

bool SC_ServerDebugInterface::EnterTranscript(object_id_type lPlayerID, int transferType, int resId, int index, int serverId, int tribuMap)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if(pkPlayer == NULL)
		return false;

	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if(enterLeaveManager == NULL)
	{
		return false;
	}

	int32_t nTargetServer = 0;
	if(serverId > 0)
	{
		//nTargetServer = UT_ServerHelper::GetActiveId(serverId);
		nTargetServer = GSNodeNetwork::Instance().getEntryIdByMergeId(serverId);
	}
	if(nTargetServer > 0 && nTargetServer == UT_ServerHelper::GetServerId())
	{
		nTargetServer = 0;
	}

	if(nTargetServer == 0)
	{
		bool flag = (tribuMap ? true : false);
		enterLeaveManager->TransferToServerByType(pkPlayer, resId, transferType, index, flag);
	}
	else
	{
		enterLeaveManager->RequstCrossChangeLocation(pkPlayer, nTargetServer, resId, transferType, index);
	}
	return true;
}

//-------------------------------------------------------------
void SC_ServerDebugInterface::SetFlopProbability(float fProb)
{
	LC_ServerItemFlopManager::GetSingleton().SetDebugFlopCoeff(fProb);
}
//------------------------------------------------------------------------------
bool SC_ServerDebugInterface::SummonVehicle(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	LC_ServerPlayer* pkOwnerPlayer =NULL;
	if (pkPlayer)
	{
		pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
		if(pkOwnerPlayer)
		{
			pkOwnerPlayer->SummonVehicle();
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
bool SC_ServerDebugInterface::DismissVehicle(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	LC_ServerPlayer* pkOwnerPlayer =NULL;
	if (pkPlayer)
	{
		pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
		if(pkOwnerPlayer)
		{
			if (pkOwnerPlayer->GetRiderCharType() > 0)
			{
				pkOwnerPlayer->SetRiderCharType(0);
				return true;
			}
		}
	}
	return false;
}
//--------------------------------------------------
bool SC_ServerDebugInterface::AddSystemTilte(object_id_type lPlayerID, int lSytemTitleID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
	{
		return false;
	}
	pkPlayer->AddSystemTitle(lSytemTitleID);
	return true;
}
//--------------------------------------------------
void SC_ServerDebugInterface::RemoveSystemTitle(object_id_type lPlayerID, int lSystemTitleID)
{
	

	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	ResultType iResult = RE_SUCCESS;
	if (NULL != pkPlayer)
	{
		LC_TitleAsset* pkTitleAsset = pkPlayer->GetTitleAsset();
		{
			if(pkTitleAsset->GetCurrentTitleType() == TNT_SYSTEM_TITLE && pkTitleAsset->GetCurrentTitle() == lSystemTitleID)
			{
				pkTitleAsset->SetCurrentTitle(TNT_NONE_TITLE, -1);
				if (iResult == RE_SUCCESS)
				{
					pkPlayer->SetCharTitle("");
					pkPlayer->SetChangeTitleFlag(true);
				}
			}
			pkPlayer->RemoveSystemTitle(lSystemTitleID, GET_CURRENT_TIMESTAMP_IN_SECONDS());
		}
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::SetSystemTitle(object_id_type lPlayerID, int lSystemTitleID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		LC_TitleAsset* pkTitleAsset = pkPlayer->GetTitleAsset();
		{
			pkPlayer->SetSystemTitle(lSystemTitleID);
			CF_SystemTitleList* pkTitleList = CF_SystemTitleList::GetSingletonPtr();
			
			CF_SystemTitleList::DataEntry* pkEntry = pkTitleList->GetEntryPtr(lSystemTitleID);
			if (pkEntry == NULL)
				return;
			
		}
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::GetAchievement(unique_id_type charID, StringType sAchievementIDs)
{
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (NULL == pkPlayer)
	{
		SetGMCommandResult(7, "role doesn't online", "");
		return;
	}

	StringStreamType ss;
	mem::vector<StringType> strList;
	Utility::tokenize(sAchievementIDs, strList, ",");
	size_t nSize = strList.size(); 
	ss << "{";
	for (size_t i = 0; i < strList.size(); ++i)
	{
		int lAchievementID = 0;
		int iHold = 0;
		__Get_Token_Value(lAchievementID, strList, nSize, i, int, atoi);
		if (pkPlayer->GetAchievementAsset().IsInFinishAchievementRecord(lAchievementID))
		{
			iHold = 1;
		}
		if (0 != i)
		{
			ss << ",";
		}
		ss << "\"" << lAchievementID << "\":" <<  iHold;
	}
	ss << "}";
	StringType result = ss.str();
	SetGMCommandResult(RE_SUCCESS, "ok", result);
}
//--------------------------------------------------
void SC_ServerDebugInterface::AddAchievement(object_id_type lPlayerID, int lAchievementID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (!pkPlayer)
	{
		return;
	}
	pkPlayer->AddAchievementReward(lAchievementID);
}
//--------------------------------------------------
void SC_ServerDebugInterface::RemoveAchievement(object_id_type lPlayerID, int lAchievementID)
{

}
//--------------------------------------------------
void SC_ServerDebugInterface::KickPlayer(unique_id_type lCharID)
{
	_kickPlayer(lCharID);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}
//--------------------------------------------------
void SC_ServerDebugInterface::AddVigor(object_id_type lPlayerID, int lPoint)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return;
	}
	pkPlayer->AddVigor(lPoint);
}
//--------------------------------------------------
void SC_ServerDebugInterface::SetCurrentLocation(object_id_type lPlayerID, int iMapLogicID, int lX, int lY)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		UT_Vec3Int kPos;
		kPos.x = lX;
		kPos.y = lY;

		LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
		pkServerWorldManager->RequestChangePlayerLocation(pkPlayer, LMIOT_NPC_TRANSFER, 0, iMapLogicID, kPos);
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::ChangeCurrentLocation(object_id_type lPlayerID, int iMapInOutType, int iMapLogicType, int iMapLogicID, int iMapResID, int iX, int iY)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		UT_Vec3Int kPos;
		kPos.x = iX;
		kPos.y = iY;

		LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
		pkServerWorldManager->RequestChangePlayerLocation(pkPlayer,	LC_MapInOutType(iMapInOutType),	iMapLogicID, iMapResID, kPos);
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::SetInteractionValue(object_id_type lNPCID, int lValue)
{
	LC_NPCManagerBase* pkNPCManager = ENGINE_GET_NPC_MANAGER();
	LC_NPCBase* pkNPC = pkNPCManager->FindNPC(lNPCID);
	if (pkNPC)
	{
		pkNPC->SetInterationValue(lValue);
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::SetArenaExp(object_id_type lPlayerID, int lEXP)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		LC_ArenaAsset* pkArenaAsset = pkPlayer->GetArenaAsset();
		{
			PS_CharArenaInfo kArenaInfo;
			pkArenaAsset->SetDataToProtocolStruct(kArenaInfo);
			kArenaInfo.m_arenaExp = lEXP;
			pkPlayer->SetArenaData(kArenaInfo);
		}
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::SetArenaScore(object_id_type lPlayerID, int lScore)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		LC_ArenaAsset* pkArenaAsset = pkPlayer->GetArenaAsset();
		{
			PS_CharArenaInfo kArenaInfo;
			pkArenaAsset->SetDataToProtocolStruct(kArenaInfo);
			kArenaInfo.m_arena33Score = lScore;
			kArenaInfo.m_arena55Score = lScore;
			kArenaInfo.m_arenaFreeScore = lScore;

			pkPlayer->SetArenaData(kArenaInfo);
		}
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::SetArenaTotalData(object_id_type lPlayerID, int lWinnum, int lLossNum, int lDrawNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		LC_ArenaAsset* pkArenaAsset = pkPlayer->GetArenaAsset();
		{
			PS_CharArenaInfo kArenaInfo;
			pkArenaAsset->SetDataToProtocolStruct(kArenaInfo);

			kArenaInfo.m_total33Win = lWinnum;
			kArenaInfo.m_total33Lose = lLossNum;
			kArenaInfo.m_total33Draw = lDrawNum;
			kArenaInfo.m_quarter33Win = lWinnum;
			kArenaInfo.m_quarter33Lose = lLossNum;
			kArenaInfo.m_quarter33Draw = lDrawNum;
			kArenaInfo.m_total55Win = lWinnum;
			kArenaInfo.m_total55Lose = lLossNum;
			kArenaInfo.m_total55Draw = lDrawNum;
			kArenaInfo.m_quarter55Win = lWinnum;
			kArenaInfo.m_quarter55Lose = lLossNum;
			kArenaInfo.m_quarter55Draw = lDrawNum;
			kArenaInfo.m_totalFreeWin = lWinnum;
			kArenaInfo.m_totalFreeLose = lLossNum;
			kArenaInfo.m_totalFreeDraw = lDrawNum;
			kArenaInfo.m_quarterFreeWin = lWinnum;
			kArenaInfo.m_quarterFreeLose = lLossNum;
			kArenaInfo.m_quarterFreeDraw = lDrawNum;

			pkPlayer->SetArenaData(kArenaInfo);
		}
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::SetArenaQuarterData(object_id_type lPlayerID, int lWinnum, int lLossNum, int lDrawNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		LC_ArenaAsset* pkArenaAsset = pkPlayer->GetArenaAsset();
		{
			PS_CharArenaInfo kArenaInfo;
			pkArenaAsset->SetDataToProtocolStruct(kArenaInfo);
			kArenaInfo.m_quarter33Win = lWinnum;
			kArenaInfo.m_quarter33Lose = lLossNum;
			kArenaInfo.m_quarter33Draw = lDrawNum;
			pkPlayer->SetArenaData(kArenaInfo);
		}
	}
}

bool SC_ServerDebugInterface::AddPKValue(object_id_type lPlayerID, int lPKValue)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer)
	{
		LC_PKRecorderBase* pRecord = pkPlayer->GetPKRecorder();
		if (pRecord)
		{
			pRecord->AddPKValue(lPKValue);
		}
	}
	return true;
}
//--------------------------------------------------
LC_ServerPlayer* SC_ServerDebugInterface::_findPlayer(object_id_type lPlayerID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*) pkPlayerManager->FindPlayer(lPlayerID);
	return pkPlayer;
}

LC_ServerPlayer* SC_ServerDebugInterface::_findPlayerByUID(unique_id_type lCharID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*) pkPlayerManager->FindPlayer(lCharID);
	return pkPlayer;
}

void SC_ServerDebugInterface::_kickPlayer(unique_id_type lCharID)
{
	LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	if (lCharID == INVALID_UNIQUE_ID)
	{
		LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
		if (NULL == playManager)
		{
			return;
		}
		mem::vector<LC_ServerPlayer*> allPlayers = playManager->GetAllPlayerPtr();
		mem::vector<LC_ServerPlayer*>::iterator Iter = allPlayers.begin();
		for (; Iter != allPlayers.end(); ++Iter)
		{
			LC_ServerPlayer* pkPlayer = *Iter;
			if (pkPlayer->IsInstance())
			{
				enterLeaveManager->UserLeaveServer(pkPlayer, LMIOT_KICKOFF);
			}

		}
	}
	else
	{
		LC_ServerPlayer* pkPlayer = _findPlayerByUID(lCharID);
		if (!pkPlayer)
		{
			return;
		}
		enterLeaveManager->UserLeaveServer(pkPlayer, LMIOT_KICKOFF);
	}
}

//--------------------------------------------------
LC_ServerPlayer* SC_ServerDebugInterface::FindPlayerByID(object_id_type lPlayerID)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*) pkPlayerManager->FindPlayer(lPlayerID);
	return pkPlayer;
}
//--------------------------------------------------
LC_ServerPlayer* SC_ServerDebugInterface::FindPlayerByName(const StringType& rkPlayerName)
{
	LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*) pkPlayerManager->FindPlayerByName(rkPlayerName);
	return pkPlayer;
}
void SC_ServerDebugInterface::SetEquipEndurance(object_id_type lPlayerID, int EquipSlotIndex, int iEndurance)
{

}
//--------------------------------------------------
void SC_ServerDebugInterface::AddShortcut(object_id_type lPlayerID, int lShortcutTypeID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return;
	}

	LC_ShortcutBar* pkShortcutBar = pkPlayer->GetShortcutBar();
	if (NULL == pkShortcutBar)
	{
		return;
	}

	LC_ShortcutEntry* pkShortcutEntry = NULL;
	for (int i = 0; i < MAX_SHORTCUTBAR_COUNT; ++i)
	{
		for (int j = 0; j < MAX_SINGLE_SHORTCUTBAR_SLOT_COUNT; ++j)
		{
			pkShortcutEntry = pkShortcutBar->GetShortcutEntry(i, j);
			if (NULL != pkShortcutEntry
					&& !pkShortcutEntry->GetValid())
			{
				break;
			}
		}
	}

	if (NULL != pkShortcutEntry && !pkShortcutEntry->GetValid())
	{
		int iShortcutType = -1;
		if (IS_ITEM_ID(lShortcutTypeID))
		{
			CF_ItemList* pkItemList = CF_ItemList::GetSingletonPtr();
			CF_ItemList::DataEntry* pkItemData = pkItemList->GetEntryPtr(lShortcutTypeID);
			if (NULL != pkItemData)
			{
				//物品快捷
				if (pkItemData->_lCategory1 == 11)
				{
					iShortcutType = 0;
				}
				//装备快捷
				else if (pkItemData->_lCategory1 == 12)
				{
					iShortcutType = 1;
				}
			}
		}
		if (IS_SKILL_TYPE(lShortcutTypeID))
		{
			//坐骑快捷
			if (lShortcutTypeID > 29200000 && lShortcutTypeID < 29201000)
			{
				iShortcutType = 3;
			}
			//技能快捷
			else
			{
				iShortcutType = 2;
			}
		}
		if (-1 != iShortcutType)
		{
			pkShortcutEntry->SetValid(true);
			pkShortcutEntry->SetShortCutType(iShortcutType);
			pkShortcutEntry->SetTypeID(lShortcutTypeID);

			pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_SHORTCUT);
		}
	}
}
//--------------------------------------------------
void SC_ServerDebugInterface::DisarmTarget(object_id_type lPlayerID, int iDurationTime)
{
	return;
}
//--------------------------------------------------
void SC_ServerDebugInterface::ConsumeItemByIndex(object_id_type lPlayerID, int iPackType, int iPackIndex, int iCount)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return;
	}

	if (pkPlayer->CheckConsumeItemsInPackageByIndex(iPackType, iPackIndex, iCount) == RE_SUCCESS)
		pkPlayer->ConsumeItemsInPackageByIndex(iPackType, iPackIndex, iCount);
}
//--------------------------------------------------
void SC_ServerDebugInterface::ConsumeItemByID(object_id_type lPlayerID, int lItemTypeID, int iCount)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return;
	}

	if (pkPlayer->CheckConsumeItemByID(lItemTypeID, iCount) == RE_SUCCESS)
		pkPlayer->ConsumeItemsInPackageByID(lItemTypeID, iCount);
}

void SC_ServerDebugInterface::SetBow(object_id_type lPlayerID, int nVal)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);

	if (!pkPlayer)
		return ;

	pkPlayer->AddBowAmount(nVal);
}
StringType SC_ServerDebugInterface::GetVIPInfo(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return "";
	}

	return pkPlayer->GetVIPAsset().GetVIPAssetInfoStr();
}
BOOL SC_ServerDebugInterface::SendToPlayerConsole(object_id_type playerID, const StringType& msg)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer)
	{
		MG_RLT_DebugCommandLine kMsgBody;
		TPSTR2STDSTR(msg, kMsgBody.m_strMsg);
		pkPlayer->SendMsgToClient(MGPT_RLT_DEBUG_COMMANDLINE, &kMsgBody);
	}

	return FALSE;
}

StringType SC_ServerDebugInterface::DrawVIPGift(object_id_type playerID, int type)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);

	StringType sret = "";
	ResultType ret = RE_FAIL;
	char buf[56];

	const char* prev = "unkown";
	if (pkPlayer)
	{
		LC_VIPAsset& kVIPAsset = pkPlayer->GetVIPAsset();
		if (0 == type)
		{
			prev = "buygift";
			ret = kVIPAsset.BuyGift();
		}
		else if (1 == type)
		{
			prev = "vipgift";
			VIPLevelSet set;
			kVIPAsset.GetAvailVipLevels(set);
			for (VIPLevelSet::iterator it = set.begin(); it != set.end(); ++it)
			{
				ret = kVIPAsset.DrawGiftVip(*it);
				sprintf(buf, "type:%s\tret:%d\n", prev, ret);
				sret.append(buf);
			}
		}
		else if (2 == type)
		{
			prev = "dailygift";
			int level = kVIPAsset.GetVIPLevel();
			ret = kVIPAsset.DrawGiftDaily(level);
		}
	}
	sprintf(buf, "type:%s\tret:%d\n", prev, ret);
	sret.append(buf);
	return sret;
}
void SC_ServerDebugInterface::SignIn(object_id_type playerID, int32_t iCnt)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer)
	{
		pkPlayer->GetSignInAsset()->GMSignIn(iCnt, GET_CURRENT_TIMESTAMP_IN_SECONDS());

		pkPlayer->GetOpenAbilityAsset().OnSignIn();
		//do event dispatch
		LC_GameEventManager* pkEventManager = LC_GameEventManager::GetSingletonPtr();
		LC_GameEvent* pkGameEvent			= pkEventManager->AllocGameEvent();
		if (pkGameEvent)
		{
			pkGameEvent->SetEventType(GLET_SIGNIN);
			pkGameEvent->SetEventParam(EVENT_SEND_TYPE_PARAM,	GENT_SELF);
			pkGameEvent->SetEventParam(EP_SIGNIN_PLAYER_ID, pkPlayer->GetID());
			pkGameEvent->SetEventParam(EP_SIGNIN_TTL_CNT, pkPlayer->GetSignInAsset()->GetTotalCnt());
			pkGameEvent->SetEventParam(EP_SIGNIN_MAP_ID, pkPlayer->GetMapResID());

			pkEventManager->InsertGameEvent(pkGameEvent);
		}
	}
}

void SC_ServerDebugInterface::ScheduleGameStory(int32_t iStoryID, const StringType& cronStr)
{
	LC_GameStory_Manager* pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	pkGSMgr->ScheduleGameStory(iStoryID, cronStr);
}

void SC_ServerDebugInterface::EndGameStory(int32_t iStoryID)
{
	LC_GameStory_Manager* pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	pkGSMgr->AyncDeactiveStory(iStoryID);
}

void SC_ServerDebugInterface::SetGameStoryRealDay(int32_t iStoryID, int32_t iDay)
{
	LC_GameStory_Manager*  pkGSMgr = SERVER_GET_GAMESTORY_MANAGER;
	pkGSMgr->SetRealDay(iStoryID, iDay);
}

void SC_ServerDebugInterface::GMLogin(object_id_type playerID, const StringType& usr, const StringType& pwd)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer)
	{
		Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetRedisHandler();
		// 		MG_GMG_REQ_GM_Login kReqBody;
		// 		kReqBody.m_sUser.append(usr.begin(), usr.end());
		// 		kReqBody.m_sPwd.append(pwd.begin(), pwd.end());
		// 		SendMsgToGM(MGPT_GMG_REQ_GM_LOGIN, &kReqBody, pkPlayer->GetUserID(), pkPlayer->GetInstance());

		int ret = 0;
		redispp::StringReply kGMReply;
		UT_REDIS_CMD_WITH_REPLY(ret, pkRedisHandler, kGMReply, get, GetGameServerApp()->GetRedisServerPredix() + UT_REDIS_KEY_GM_ACCOUNTS);

		int iServerID = GetGameServerApp()->GetServerInfo().m_iServerID;
		if (0 == ret && kGMReply.result())
		{
			redis_define::gm_account_setting k_gm_setting;
			std::string s_gm_setting = kGMReply;
			if (k_gm_setting.ParseFromString(s_gm_setting))
			{
				for (int i = 0; i < k_gm_setting.accounts_size(); ++i)
				{
					const redis_define::gm_account_info& kaccount = k_gm_setting.accounts(i);
					if (kaccount.account().compare(usr.c_str()) == 0)
					{
						if (kaccount.password().compare(pwd.c_str()) == 0)
						{
							for (int j = 0; j < kaccount.servers_size(); ++j)
							{
								const redis_define::gm_server_info& kserver = kaccount.servers(j);
								if (kserver.server() == 0 || kserver.server() == iServerID)
								{
									pkPlayer->SetGMInfo(kaccount.account().c_str(), kserver.priority());
									break;
								}
							}
						}
						GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GMLogin bad passwrd!");
						return;
					}
				}
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GMLogin no account hit!");
			}
			else
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GMLogin bad gminfo[%u] %s!", s_gm_setting.size(), s_gm_setting.c_str());
			}
		}
		else
		{
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "GMLogin redis ret[%d] err[%s]!", ret, pkRedisHandler->GetLastError().c_str());
		}
	}
}


void SC_ServerDebugInterface::GMCharLists()
{
	Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetRedisHandler();
	int ret = 0;
	redispp::MultiBulkEnumerator kChars;

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);

	kChars = pkRedisConn->hgetAll(GetGameServerApp()->GetRedisServerPredix() + UT_REDIS_KEY_GM_CHARS);
	StringStreamType gmcharlist;
	mem::vector<GMCharInfo>  vGMChar;
	GMCharInfo gmchar;
	std::string sgmchar;
	while (kChars.next(&sgmchar))
	{
		redis_define::gm_char_info kgmchar;
		if (kgmchar.ParseFromString(sgmchar))
		{
			gmchar.account_id = kgmchar.user_name();
			gmchar.role_id = kgmchar.char_id();
			gmchar.role_name = kgmchar.char_name();
			gmchar.gm_level = kgmchar.priority();
			vGMChar.push_back(gmchar);
		}
	}
	boost::ajson::save_to_buff(vGMChar, gmcharlist);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", gmcharlist.str());

	UT_REDIS_CMD_END(ret, pkRedisHandler);
}

void SC_ServerDebugInterface::SetGmLevel(unique_id_type charID, const StringType& name, uint32_t uiPriority)
{
	Utility::Redis_Handler* pkRedisHandler = GetGameServerApp()->GetRedisHandler();
	int ret = 0;

	redis_define::gm_char_info kgmchar;
	std::string sCharID = boost::lexical_cast<std::string>(charID);
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);

	UT_REDIS_CMD_BEGIN(ret, pkRedisHandler);
	if (pkPlayer)
	{
		kgmchar.set_user_id(pkPlayer->GetUserID());
		kgmchar.set_char_id(pkPlayer->GetInstance());
		kgmchar.set_user_name(pkPlayer->GetUserName().c_str());
		kgmchar.set_char_name(pkPlayer->GetOwnerCharName().c_str());
	}
	else
	{
		redispp::StringReply kCharReply = pkRedisConn->hget(GetGameServerApp()->GetRedisServerPredix() + UT_REDIS_KEY_GM_CHARS, sCharID);
		if (!kCharReply.result() || !kgmchar.ParseFromString(kCharReply))
		{
			kgmchar.set_char_id(charID);
			if (name.size() && name != "0")
			{
				kgmchar.set_user_name(Utility::Utf8ToMbcs(name.c_str()).c_str());
			}
		}
	}

	kgmchar.set_priority(UT_MathBase::MaxInt(INIT_GM_PRIORITY, uiPriority));

	pkRedisConn->hset(GetGameServerApp()->GetRedisServerPredix() + UT_REDIS_KEY_GM_CHARS, sCharID, kgmchar.SerializeAsString());

	UT_REDIS_CMD_END(ret, pkRedisHandler);

	if (pkPlayer)
	{
		pkPlayer->SetGMInfo("", kgmchar.priority());
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
	}
}

void SC_ServerDebugInterface::OpenAbility(object_id_type playerID, int32_t iAbilityID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer)
	{
		pkPlayer->GetOpenAbilityAsset().GMOpenAbility(iAbilityID);
	}
}

bool SC_ServerDebugInterface::OpenPetAbility(object_id_type playerID, int32_t iAbilityID, int32_t petID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if(NULL==pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL==pkPlayer)
	{
		return false;
	}

	MG_RespOpenAbility resp;
	resp.m_iID = iAbilityID;
	resp.m_Result = RE_FAIL;
	pkPlayer->OpenPetAbility(iAbilityID, petID);
	pkPlayer->SendMsgToClient(MGPT_RESP_OPEN_ABILITY, &resp);

	return true;
}

void SC_ServerDebugInterface::OpenAllMaps(object_id_type playerID, bool bOpen)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer)
	{
		pkPlayer->EnableOpenAllMaps(bOpen);
	}
}
void SC_ServerDebugInterface::GetServerInfo(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	(LC_ServerPlayer*)playManager->FindPlayer(playerID);
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
	SK_ServerSkillEventManager* pkEventManager = SERVER_GET_SKILL_EVENT_MANAGER();
	LC_ServerNPCManager* pkNPCManager = SERVER_GET_NPC_MANAGER();
	LC_GameEventManager* pkGameEventManager = LC_GameEventManager::GetSingletonPtr();
	/*int num = playManager->GetPlayerCount();*/
	StringStreamType sStream("");
	sStream << "onlineNum[";
	sStream << playManager->GetPlayerCount();
	sStream << "]\n";
	sStream << "mapSize[";
	sStream << pkServerWorldManager->GetMapLogicsize();
	sStream << "]\n";
	sStream << pkEventManager->GetRTInfo();
	sStream << pkGameEventManager->GetRTInfo();


	pkPlayer->SendNotifyStrToClient(sStream.str(), PROMPT_CHANNEL_SYSTEM_TIP, PROMPT_LEVEL_NORMAL);
	pkPlayer->SendNotifyStrToClient(pkNPCManager->GetRTInfo(), PROMPT_CHANNEL_SYSTEM_TIP, PROMPT_LEVEL_NORMAL);
	return;
}

void SC_ServerDebugInterface::Announceinfo(const StringType& msg)
{
	return;
}

void SC_ServerDebugInterface::AnnounceinfoUTF8(const StringType& msg)
{
	return;
}

void SC_ServerDebugInterface::ChangeVehicle(object_id_type playerID , int iLevel, int lCharTypeID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer)
	{
		const LC_SubordinateDataEntry* pkDataEntry = SafeGetSubDataEntry(LC_SUB_TP_VEHICLE, iLevel);
		if (pkDataEntry)
		{}
	}
}
void SC_ServerDebugInterface::SetSubLevel(object_id_type playerID , int iSubType, int iSubLevel)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer)
	{
		pkPlayer->GetSubordinateAsset().GMSetLevel(iSubType, iSubLevel);
	}
}

void SC_ServerDebugInterface::SetCrossRealmServer(int serverid)
{
	Protocol::MG_SS_SET_CROSSREALM_SERVER msg;
	msg.m_ServerId = serverid;
	SendMsgToSociety(MGPT_GGU_SET_CROSSREALM_SERVER, &msg);
}

void SC_ServerDebugInterface::CrossRaidServerList(object_id_type playerID)
{
	SendMsgToSociety(MGPT_GGU_GET_RAIDSERVERLIST, NULL, 0, playerID);
}

void SC_ServerDebugInterface::GetServerConnectTime(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}
	SendMsgToLogin(MGPT_GL_REQ_ALLSERVER_CONNECTTIME, NULL, pkPlayer->GetUserID(), playerID, 0, 0);
}

void SC_ServerDebugInterface::Closeserver(object_id_type playerID, int32_t serverid)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}
	MG_GL_REQ_CloseServer kMsg;
	kMsg.m_ServerId = serverid;
	SendMsgToLogin(MGPT_GL_REQ_CLOSESERVER, &kMsg, pkPlayer->GetUserID(), playerID, 0, 0);
}

void SC_ServerDebugInterface::ExportAllRobot(object_id_type playerID)
{
	
}

void SC_ServerDebugInterface::ExportSoloDuleRobot(object_id_type playerID, int exportType, int robotType)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	LC_ServerPlayer* pkOwnerPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkOwnerPlayer)
	{
		return;
	}

	switch(exportType)
	{
	case ERT_REFRESH:
		{
			ExportRobot* pkExportRobot = (ExportRobot*)ExportRobot::GetSingletonPtr();
			pkExportRobot->InitExportRobotToGroup(ERT_REFRESH, robotType);
		}
		break;
	case ERT_ADD:
	case ERT_CLEAR:
		{
			MG_GG_REQ_Operator_Robot msg;
			msg.m_nOperatorType = exportType;
			msg.m_nRobotType = robotType;
			SendMsgToGroup(MGPT_SS_REQ_OPERATOR_ROBOT, &msg, pkOwnerPlayer->GetUserID(), pkOwnerPlayer->GetInstance());
		}
		break;
	case ERT_WHEEL:
		{
			
		}
		break;
	default:
		return;
	}
}
void SC_ServerDebugInterface::WheelFightReq(uint64_t uChallengeId,uint64_t uBeChallengeId)
{
	uint64_t uniqueID = 3;
	UT_ServerHelper::WheelFightReq(uniqueID,uChallengeId, uBeChallengeId);
}
bool SC_ServerDebugInterface::GetNearbyPlayer(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return false;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if(NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	MG_RLT_GetNearbyPlayer rltMsg;
	pkPlayer->GetNearbyPlayer(rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETNEARBYPLAYERINFO, &rltMsg);

	return true;
}

void SC_ServerDebugInterface::RefreshSkillCD(object_id_type playerID, int iPage, int iSlot)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ShortcutBar* bar = pkPlayer->GetShortcutBar();
	if (bar)
	{
		LC_ShortcutEntry* entry = bar->GetShortcutEntry(iPage, iSlot);
		if (entry && entry->GetValid())
		{
			int lSkillID = entry->GetTypeID();
			CF_SkillExecuteList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteList>(lSkillID);
			if(NULL == pkData)
			{
				return;
			}

			pkPlayer->GetSkillCoolDownMap()->ClearSkillCD(lSkillID);
		}
	}
}

bool SC_ServerDebugInterface::SetSkillCD(object_id_type playerID, int32_t nSkillID, int32_t nCDTimeInMs)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if ( pkPlayer == NULL )
		return false;

	// Call Internal interface.
	float fTime = (float)nCDTimeInMs * 0.001f;
	int nSlot = -1;
	pkPlayer->LearnSkill(nSkillID, nSlot);

	CF_SkillExecuteList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteList>(nSkillID);
	if(NULL == pkData)
	{
		return false;
	}

	pkPlayer->GetSkillCoolDownMap()->SetSkillCD(nSkillID, fTime );
	pkPlayer->UpdateDirtyFlag(DIRTY_FLAG_SKILL_CD);

	return true;
}

void SC_ServerDebugInterface::RefreshSkillIDCD(object_id_type playerID, int iSkillID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	CF_SkillExecuteList::DataEntry* pkData = SafeGetCSVFileDataEntryPtr<CF_SkillExecuteList>(iSkillID);
	if(NULL == pkData)
	{
		return;
	}
	
	pkPlayer->GetSkillCoolDownMap()->ClearSkillCD(iSkillID);
}

void SC_ServerDebugInterface::ForbidTalking(unique_id_type charID, int itime)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
		return;
	}
	pkPlayer->CheckGagTime(itime);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
	return;
}

void SC_ServerDebugInterface::AddLoginDays(object_id_type playerID, int idays)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);

	{
		for (int i = 0; i < idays; i++)
		{
			pkPlayer->GetCommonStatAsset().IncLoginDays();
		}
		pkPlayer->GetCommonStatAsset().UpdateDirtyFlag();
		pkPlayer->GetOpenAbilityAsset().OnLogin();
	}
}

void SC_ServerDebugInterface::SetGMCommandResult(ResultType rslt, const StringType& reltmsg, const StringType& datamsg)
{
	Utility::LC_CommandParserBase::GetSingletonPtr()->SetResult(rslt, reltmsg, datamsg);
}

void SC_ServerDebugInterface::GetOnlineUser()
{
	//LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	const LC_PlayerIDPtrMap& kPlayerIDPtrMap = playManager->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator Iter = kPlayerIDPtrMap.begin();
	OnlinRole onlinerole;
	StringStreamType onlinerolelist;
	mem::vector<OnlinRole> vOnlinRole;
	for (; Iter != kPlayerIDPtrMap.end(); ++Iter)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)Iter->second;
		if (pkPlayer->IsInstance())
		{
			onlinerole.uid = pkPlayer->GetUserName().c_str();
			StringStreamType stream;
			stream << pkPlayer->GetID();
			onlinerole.charid = stream.str();
			vOnlinRole.push_back(onlinerole);
		}
	}

	boost::ajson::save_to_buff(vOnlinRole, onlinerolelist);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", onlinerolelist.str());
}

void SC_ServerDebugInterface::GetOnlineUserCount()
{
	//LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	//const LC_PlayerIDPtrMap& kPlayerIDPtrMap = playManager->GetPlayerIDMap();

	StringStreamType ss;
	int onlineroleCount = playManager->GetPlayerCount();
	boost::ajson::save_to_buff(onlineroleCount, ss);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", ss.str());
}
void SC_ServerDebugInterface::GetRoleInfo(object_id_type playerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}
	StringStreamType roleinfo;
	RoleInfo playerinfo;

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_MOUNT);
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& mountskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)mountskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				playerinfo.mount.mountskill.push_back(pEntry->GetTypeID());
			}
		}
	}
	playerinfo.mount.mountid = 0;
	playerinfo.mount.mountlevel = pkPlayer->GetVehicleCurrentLevel();
	LC_SkillCategoryMap::const_iterator cits = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_SECT);
	if (cits != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cits->second;
		const LC_SkillIDVector& roleskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)roleskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				playerinfo.skill.push_back(pEntry->GetTypeID());
			}
		}
	}
	boost::ajson::save_to_buff(playerinfo, roleinfo);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", roleinfo.str());

}

void SC_ServerDebugInterface::SendMailToAllFunc(const StringType& subject, const StringType& content, const StringType& msg)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	
	Utility::UT_SIMDataList kItems;
	Utility::ParseFmtString2DataIFs(msg, kItems);

	LC_BackPackEntryList kEntryList;
	for (Utility::UT_SIMDataList::const_iterator cit = kItems.begin(); cit != kItems.end(); ++cit)
	{
		if (pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mail item to all:", cit->ToString().c_str());
		}
	}

	UT_ServerHelper::SendToMailAll(subject, content, 0, kEntryList, GetGameServerID(), "system");

	return;
	const LC_PlayerIDPtrMap& kPlayerIDPtrMap = playManager->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator Iter = kPlayerIDPtrMap.begin();
	for (; Iter != kPlayerIDPtrMap.end(); ++Iter)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)Iter->second;
		if (pkPlayer->IsInstance())
		{

			MG_RLT_IsExitNewMail kRltMsg;
			kRltMsg.m_bHint = true;
			kRltMsg.m_iCount = 10;
			kRltMsg.m_sendcharID = 10000;
			kRltMsg.m_notreadCount = 1;
			kRltMsg.m_globalmail = true;
			pkPlayer->SendMsgToClient(MGPT_RLT_IS_EXIT_NEWMAIL, &kRltMsg);
		}

	}
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}

void SC_ServerDebugInterface::SendMailToAll(const StringType& subject, const StringType& content, const StringType& msg)
{
	SendMailToAllFunc(subject, content, msg);
}



void SC_ServerDebugInterface::RechargeMailToOneUtf(unique_id_type charID, const StringType& subject, const StringType& content, const StringType& iteminfo, const StringType& token)
{
	SendMailToOneFunc(charID, subject, content, iteminfo, true);
	
	{
		MoneyType iYuanbao = 0;
		Utility::UT_SIMDataList kItems;
		Utility::ParseFmtString2DataIFs(iteminfo, kItems);
		Utility::UT_SIMDataList::iterator it = kItems.begin();
		for (; it!=kItems.end(); ++it)
		{
			if (it->ID() == SPEC_ITEM_TID_YUANBAO)
			{
				iYuanbao += it->ItemCount();
			}
		}
		oss_recharge(charID, (int)iYuanbao, token.c_str(), SYS_FUNC_TP_RECHARGE);
		//oss_360_log_recharge( iYuanbao, iYuanbao * YUANBAO_RMB_RATE,  );
	}
}

void SC_ServerDebugInterface::SendMailToOnline(const StringType& subject, const StringType& content, const StringType& msg)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	Utility::UT_SIMDataList kItems;
	Utility::ParseFmtString2DataIFs(msg, kItems);

	LC_BackPackEntryList kEntryList;
	for (Utility::UT_SIMDataList::const_iterator cit = kItems.begin(); cit != kItems.end(); ++cit)
	{
		if (pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mail item to online:", cit->ToString().c_str());
		}
	}

	const LC_PlayerIDPtrMap& kPlayerIDPtrMap = playManager->GetPlayerIDMap();
	LC_PlayerIDPtrMap::const_iterator Iter = kPlayerIDPtrMap.begin();
	for (; Iter != kPlayerIDPtrMap.end(); ++Iter)
	{
		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)Iter->second;
		if (pkPlayer->IsInstance())
		{
			UT_ServerHelper::SendToMail(subject, content, pkPlayer->GetInstance(), pkPlayer->GetOwnerCharName(), 0, kEntryList, "system");
		}
	}
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}

void SC_ServerDebugInterface::SendMailToOneFunc(unique_id_type charID, const StringType& subject, const StringType& content, const StringType& msg, bool bGb2312)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();

	LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
	
	Utility::UT_SIMDataList kItems;
	Utility::ParseFmtString2DataIFs(msg, kItems);

	LC_BackPackEntryList kEntryList;
	for (Utility::UT_SIMDataList::const_iterator cit = kItems.begin(); cit != kItems.end(); ++cit)
	{
		if (pkPackEntryFactory->CreateItemEntry(*cit, CIET_NONE, kEntryList))
		{
			GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "mail item to", charID, cit->ToString().c_str());
		}
	}

	UT_ServerHelper::SendToMail(subject, content, charID, "", 0, kEntryList, "system");
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}

void SC_ServerDebugInterface::SendMailToOne(unique_id_type charID, const StringType& subject, const StringType& content, const StringType& msg)
{
	SendMailToOneFunc(charID, subject, content, msg, false);
}

void SC_ServerDebugInterface::SendMailToOneUtf(unique_id_type charID, const StringType& subject, const StringType& content, const StringType& msg)
{
	SendMailToOneFunc(charID, subject, content, msg, true);
}

void SC_ServerDebugInterface::SendMailToOneMoney(unique_id_type charID, const StringType& subject, const StringType& content, const StringType& msg, const StringType& order)
{
	SendMailToOne(charID, subject, content, msg);
	StringStreamType strInfo;
	strInfo << "[{\"uid\":" << charID << ",\"order_id\":\"" << order << "\"}]";
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", strInfo.str());
	//GfxWriteFmtLog(LOG_BILL_FILELINE, LOG_SWITCH_BILL, "SendMailToOneMoney", playerID, content, msg, msg, order);
}

void SC_ServerDebugInterface::AddItemToAllUtf(const StringType& iteminfo, const StringType& subject, const StringType& content, const StringType& token)
{
	GfxWriteFmtLog(LOG_BILL_FILELINE, LOG_SWITCH_BILL, "AddItemToAllUtf", iteminfo, subject, content, token);
	UT_SIMDataList kItems;
	Utility::ParseFmtString2DataIFs(iteminfo, kItems);
	if (!kItems.empty())
	{
		LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
		const LC_PlayerIDPtrMap& kPlayerIDPtrMap = playManager->GetPlayerIDMap();
		LC_PlayerIDPtrMap::const_iterator Iter = kPlayerIDPtrMap.begin();
		for (; Iter != kPlayerIDPtrMap.end(); ++Iter)
		{
			LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)Iter->second;
			if (pkPlayer && pkPlayer->IsInstance())
			{
				UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_GM, 0, subject, content);
			}
		}
	}
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}
void SC_ServerDebugInterface::AddItemToOneUtf(unique_id_type charID, const StringType& iteminfo, const StringType& subject, const StringType& content, const StringType& token)
{
	GfxWriteFmtLog(LOG_BILL_FILELINE, LOG_SWITCH_BILL, "AddItemToOneUtf", charID, iteminfo, subject, content, token);
	UT_SIMDataList kItems;
	Utility::ParseFmtString2DataIFs(iteminfo, kItems);
	if (!kItems.empty())
	{
		LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
		if (playManager == NULL)
		{
			return;
		}

		LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)playManager->FindPlayer(charID);
		if (NULL == pkPlayer)
		{
			return;
		}

		UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_GM, 0, subject, content);
	}
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}

void SC_ServerDebugInterface::GetItemList()
{
#if 0
	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();

	StringStreamType Iteminfolist;
	mem::vector<ItemList>  vitemlist;

	CF_ItemList::DataEntryMapExternIterator pkIter = pkCSVItemList->GetIterator();
	CF_ItemList::DataEntry* pkmEntry = NULL;

	while (pkIter.HasMoreElements())
	{
		pkmEntry = pkIter.PeekNextValuePtr();

		ItemList pkitem;
		pkitem.id = pkmEntry->_iID;
		pkitem.name = pkmEntry->_kName;
		pkitem.price = pkmEntry->_lUnBindPrice;
		pkitem.bind_type = 0;
		pkitem.description = pkmEntry->_kDescript;
		if (pkmEntry->_lCategory1 == 12)
		{
			pkitem.type = "装备";
		}
		else
		{
			pkitem.type = "道具";
		}
		vitemlist.push_back(pkitem);
		pkIter.MoveNext();
	}
	boost::ajson::save_to_buff(vitemlist, Iteminfolist);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", Iteminfolist.str());
#endif
}

void SC_ServerDebugInterface::GetConfigList(const StringType& type)
{
#if 0
	StringStreamType stringconfig;
	if (type == "role_info_property")
	{
		ALL_Role_info aLL_Role_info;
		aLL_Role_info.role_info_property.base = "基础";
		aLL_Role_info.role_info_property.vehicle = "坐骑";
		aLL_Role_info.role_info_property.arrow = "弩炮";
		aLL_Role_info.role_info_property.devaeye = "宠物";
		aLL_Role_info.role_info_property.vehiclefighting = "骑战";
		aLL_Role_info.role_info_property.sectskill = "门派技能";
		aLL_Role_info.role_info_property.mentalskill = "心法技能";
		boost::ajson::save_to_buff(aLL_Role_info, stringconfig);
	}
	if (type == "coin_list")
	{
		StringStreamType coinstr;
		Configlist configlist;

		configlist.coin_list.insert(std::make_pair(TOSTRING(SPEC_ITEM_TID_CASH), "金币"));
		configlist.coin_list.insert(std::make_pair(TOSTRING(SPEC_ITEM_TID_YUANBAO), "元宝"));
		configlist.coin_list.insert(std::make_pair(TOSTRING(SPEC_ITEM_TID_LIJIN), "礼金"));

		boost::ajson::save_to_buff(configlist, stringconfig);

	}
	if (type == "gm_level")
	{
		StringStreamType coinstr;
		GMlevel gmlevel;
		gmlevel.gm_level.insert(std::make_pair("998", "gm"));
		gmlevel.gm_level.insert(std::make_pair("999", "无GM权限"));

		boost::ajson::save_to_buff(gmlevel, stringconfig);

	}
	if (type == "prop_att")
	{
		Prop_att _prop_att;
		_prop_att.prop_att.phd = "防御";
		_prop_att.prop_att.movespeed = "移动速度";
		boost::ajson::save_to_buff(_prop_att, stringconfig);
	}
	if (type == "task_list")
	{
		CF_TaskList* pkCSVTaskList = CF_TaskList::GetSingletonPtr();
		CF_TaskList::DataEntryMapExternIterator pkIter = pkCSVTaskList->GetIterator();
		CF_TaskList::DataEntry* pkEntry = NULL;
		StringStreamType Taskinfolist;
		VTaskInfo tasklist;
		Taskinfolist << "{\"task_list\":{\"";

		while (pkIter.HasMoreElements())
		{
			pkEntry = pkIter.PeekNextValuePtr();
			if (pkEntry->_iTaskTitleType != 1)
			{
				pkIter.MoveNext();
				continue;
			}
			Taskinfolist << pkEntry->_iID;
			Taskinfolist << "\":\"";

			Taskinfolist << pkEntry->_kTaskGoal;


			pkIter.MoveNext();
			Taskinfolist << "\",\"";

		}
		StringType strtaskinfo = Taskinfolist.str().substr(0, Taskinfolist.str().length() - 3);

		stringconfig << strtaskinfo;
		stringconfig << "\"}}";

	}
	if (type == "task_result")
	{
		StringStreamType taskstr;
		Tasktype taskresult;
		taskresult.task_result.insert(std::make_pair("0", "完成任务"));
		taskresult.task_result.insert(std::make_pair("1", "放弃任务"));
		taskresult.task_result.insert(std::make_pair("2", "接受任务"));
		boost::ajson::save_to_buff(taskresult, stringconfig);

	}
	if (type == "item_operation")
	{
		StringStreamType str;
		Itemoperation itemoperation;
		itemoperation.item_operation.insert(std::make_pair("1", "掉落"));
		itemoperation.item_operation.insert(std::make_pair("2", "购买"));
		itemoperation.item_operation.insert(std::make_pair("3", "使用"));

		boost::ajson::save_to_buff(itemoperation, stringconfig);

	}
	if (type == "cifu_type")
	{
		StringStreamType str;
		Cifutype cifutype;
		cifutype.cifu_type.insert(std::make_pair("1", "坐骑"));
		cifutype.cifu_type.insert(std::make_pair("2", "弩炮"));
		cifutype.cifu_type.insert(std::make_pair("3", "宠物"));
		cifutype.cifu_type.insert(std::make_pair("4", "骑战"));

		boost::ajson::save_to_buff(cifutype, stringconfig);

	}
	if (type == "dupl_type")
	{
		StringStreamType str;
		Dupltype dupltype;
		dupltype.dupl_type.insert(std::make_pair("1", "单人"));
		dupltype.dupl_type.insert(std::make_pair("2", "组队"));
		boost::ajson::save_to_buff(dupltype, stringconfig);

	}
	if (type == "dupl_result")
	{
		StringStreamType str;
		Duplresult duplresult;
		duplresult.dupl_result.insert(std::make_pair("1", "获胜"));
		duplresult.dupl_result.insert(std::make_pair("2", "失败"));

		boost::ajson::save_to_buff(duplresult, stringconfig);

	}
	if (type == "yb_income")
	{
		StringStreamType str;
		Ybincome ybincome;
		ybincome.yb_income.insert(std::make_pair("1", "出售"));
		ybincome.yb_income.insert(std::make_pair("2", "仓库存取"));
		ybincome.yb_income.insert(std::make_pair("3", "交易"));
		ybincome.yb_income.insert(std::make_pair("4", "礼包"));
		ybincome.yb_income.insert(std::make_pair("5", "任务"));
		ybincome.yb_income.insert(std::make_pair("6", "邮件"));
		ybincome.yb_income.insert(std::make_pair("7", "兑换"));
		ybincome.yb_income.insert(std::make_pair("8", "转账"));
		ybincome.yb_income.insert(std::make_pair("9", "消耗物品"));
		ybincome.yb_income.insert(std::make_pair("10", "副本"));
		ybincome.yb_income.insert(std::make_pair("11", "活跃度"));
		ybincome.yb_income.insert(std::make_pair("12", "退款"));
		ybincome.yb_income.insert(std::make_pair("13", "推广员"));
		ybincome.yb_income.insert(std::make_pair("14", "GM"));
		ybincome.yb_income.insert(std::make_pair("15", "充值"));
		ybincome.yb_income.insert(std::make_pair("16", "拾取"));
		ybincome.yb_income.insert(std::make_pair("17", "VIP"));
		ybincome.yb_income.insert(std::make_pair("18", "活动"));
		ybincome.yb_income.insert(std::make_pair("19", "公共统计"));
		ybincome.yb_income.insert(std::make_pair("20", "其他"));


		boost::ajson::save_to_buff(ybincome, stringconfig);

	}

	if (type == "yb_consume")
	{
		StringStreamType str;
		Ybconsume ybconsume;
		ybconsume.yb_consume.insert(std::make_pair("1", "商店"));
		ybconsume.yb_consume.insert(std::make_pair("2", "仓库存取"));
		ybconsume.yb_consume.insert(std::make_pair("3", "传送"));
		ybconsume.yb_consume.insert(std::make_pair("4", "交易"));
		ybconsume.yb_consume.insert(std::make_pair("5", "修理"));
		ybconsume.yb_consume.insert(std::make_pair("6", "邮件"));
		ybconsume.yb_consume.insert(std::make_pair("7", "拍卖"));
		ybconsume.yb_consume.insert(std::make_pair("8", "帮派"));
		ybconsume.yb_consume.insert(std::make_pair("9", "转载"));
		ybconsume.yb_consume.insert(std::make_pair("10", "商城"));
		ybconsume.yb_consume.insert(std::make_pair("11", "坐骑"));
		ybconsume.yb_consume.insert(std::make_pair("12", "弩炮"));
		ybconsume.yb_consume.insert(std::make_pair("13", "时装"));
		ybconsume.yb_consume.insert(std::make_pair("14", "生产"));
		ybconsume.yb_consume.insert(std::make_pair("15", "强化"));
		ybconsume.yb_consume.insert(std::make_pair("16", "真"));
		ybconsume.yb_consume.insert(std::make_pair("17", "监狱"));
		ybconsume.yb_consume.insert(std::make_pair("18", "技能"));
		ybconsume.yb_consume.insert(std::make_pair("19", "拆除镶嵌"));
		ybconsume.yb_consume.insert(std::make_pair("20", "宠物"));
		ybconsume.yb_consume.insert(std::make_pair("21", "竞技场"));
		ybconsume.yb_consume.insert(std::make_pair("22", "任务"));
		ybconsume.yb_consume.insert(std::make_pair("23", "骑战"));
		ybconsume.yb_consume.insert(std::make_pair("24", "青龙"));
		ybconsume.yb_consume.insert(std::make_pair("25", "白虎"));
		ybconsume.yb_consume.insert(std::make_pair("26", "朱雀"));
		ybconsume.yb_consume.insert(std::make_pair("27", "玄武"));
		ybconsume.yb_consume.insert(std::make_pair("28", "扩充包裹"));
		ybconsume.yb_consume.insert(std::make_pair("29", "宝箱"));
		ybconsume.yb_consume.insert(std::make_pair("30", "武魂"));
		ybconsume.yb_consume.insert(std::make_pair("31", "公共统计"));
		ybconsume.yb_consume.insert(std::make_pair("32", "帮会膜拜"));
		ybconsume.yb_consume.insert(std::make_pair("33", "活动传送"));
		ybconsume.yb_consume.insert(std::make_pair("34", "刷活动任务"));
		ybconsume.yb_consume.insert(std::make_pair("35", "活力"));
		ybconsume.yb_consume.insert(std::make_pair("36", "任务一键完成"));
		ybconsume.yb_consume.insert(std::make_pair("37", "任务吞噬"));
		ybconsume.yb_consume.insert(std::make_pair("38", "进阶日活动"));
		ybconsume.yb_consume.insert(std::make_pair("39", "资源回购"));
		ybconsume.yb_consume.insert(std::make_pair("40", "进阶石升级"));
		ybconsume.yb_consume.insert(std::make_pair("41", "其他"));
		ybconsume.yb_consume.insert(std::make_pair("1000", "角色登录"));
		ybconsume.yb_consume.insert(std::make_pair("1001", "银两祭剑"));
		ybconsume.yb_consume.insert(std::make_pair("1002", "元宝祭剑"));
		ybconsume.yb_consume.insert(std::make_pair("1003", "极限模式"));
		ybconsume.yb_consume.insert(std::make_pair("1004", "葬龙魔穴"));
		ybconsume.yb_consume.insert(std::make_pair("1005", "轻功试炼"));
		ybconsume.yb_consume.insert(std::make_pair("1006", "千里护粮"));
		ybconsume.yb_consume.insert(std::make_pair("1007", "帮会运镖"));
		ybconsume.yb_consume.insert(std::make_pair("1008", "击退五行旗"));
		ybconsume.yb_consume.insert(std::make_pair("1009", "拦截天鹰教"));
		ybconsume.yb_consume.insert(std::make_pair("1100", "日常任务"));
		ybconsume.yb_consume.insert(std::make_pair("1101", "帮会任务"));
		ybconsume.yb_consume.insert(std::make_pair("1102", "比武场"));
		ybconsume.yb_consume.insert(std::make_pair("1103", "在线时长"));
		ybconsume.yb_consume.insert(std::make_pair("1104", "等级"));
		ybconsume.yb_consume.insert(std::make_pair("1105", "老虎机"));
		ybconsume.yb_consume.insert(std::make_pair("1106", "冲榜活动"));
		ybconsume.yb_consume.insert(std::make_pair("1200", "坐骑进阶"));
		ybconsume.yb_consume.insert(std::make_pair("1201", "弩炮进阶"));
		ybconsume.yb_consume.insert(std::make_pair("1202", "宠物进阶"));
		ybconsume.yb_consume.insert(std::make_pair("1203", "骑战进阶"));


		boost::ajson::save_to_buff(ybconsume, stringconfig);

	}
	if (type == "giftmoney_income")
	{
		StringStreamType str;
		Giftmoneyincome giftmoneyincome;
		giftmoneyincome.giftmoney_income.insert(std::make_pair("1", "出售"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("2", "仓库存取"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("3", "交易"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("4", "礼包"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("5", "任务"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("6", "邮件"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("7", "兑换"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("8", "转账"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("9", "消耗物品"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("10", "副本"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("11", "活跃度"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("12", "退款"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("13", "推广员"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("14", "GM"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("15", "充值"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("16", "拾取"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("17", "VIP"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("18", "活动"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("19", "公共统计"));
		giftmoneyincome.giftmoney_income.insert(std::make_pair("20", "其他"));


		boost::ajson::save_to_buff(giftmoneyincome, stringconfig);

	}

	if (type == "giftmoney_consume")
	{
		StringStreamType str;
		Giftmoneyconsume giftmoneyconsume;
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("1", "商店"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("2", "仓库存取"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("3", "传送"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("4", "交易"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("5", "修理"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("6", "邮件"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("7", "拍卖"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("8", "帮派"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("9", "转载"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("10", "商城"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("11", "坐骑"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("12", "弩炮"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("13", "时装"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("14", "生产"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("15", "强化"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("16", "真"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("17", "监狱"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("18", "技能"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("19", "拆除镶嵌"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("20", "宠物"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("21", "竞技场"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("22", "任务"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("23", "骑战"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("24", "青龙"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("25", "白虎"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("26", "朱雀"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("27", "玄武"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("28", "扩充包裹"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("29", "宝箱"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("30", "武魂"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("31", "公共统计"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("32", "帮会膜拜"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("33", "活动传送"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("34", "刷活动任务"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("35", "活力"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("36", "任务一键完成"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("37", "任务吞噬"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("38", "进阶日活动"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("39", "资源回购"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("40", "进阶石升级"));
		giftmoneyconsume.giftmoney_consume.insert(std::make_pair("41", "其他"));


		boost::ajson::save_to_buff(giftmoneyconsume, stringconfig);
	}
	if (type == "jb_operation")
	{
		StringStreamType str;
		Jboperation jboperation;
		jboperation.jb_operation.insert(std::make_pair("1", "出售获得"));
		jboperation.jb_operation.insert(std::make_pair("2", "仓库存取获得"));
		jboperation.jb_operation.insert(std::make_pair("3", "交易获得"));
		jboperation.jb_operation.insert(std::make_pair("4", "礼包获得"));
		jboperation.jb_operation.insert(std::make_pair("5", "任务获得"));
		jboperation.jb_operation.insert(std::make_pair("6", "邮件获得"));
		jboperation.jb_operation.insert(std::make_pair("7", "兑换获得"));
		jboperation.jb_operation.insert(std::make_pair("8", "转账获得"));
		jboperation.jb_operation.insert(std::make_pair("9", "消耗物品获得"));
		jboperation.jb_operation.insert(std::make_pair("10", "副本获得"));
		jboperation.jb_operation.insert(std::make_pair("11", "活跃度获得"));
		jboperation.jb_operation.insert(std::make_pair("12", "退款获得"));
		jboperation.jb_operation.insert(std::make_pair("13", "推广员获得"));
		jboperation.jb_operation.insert(std::make_pair("14", "GM获得"));
		jboperation.jb_operation.insert(std::make_pair("15", "充值获得"));
		jboperation.jb_operation.insert(std::make_pair("16", "拾取获得"));
		jboperation.jb_operation.insert(std::make_pair("17", "VIP获得"));
		jboperation.jb_operation.insert(std::make_pair("18", "活动获得"));
		jboperation.jb_operation.insert(std::make_pair("19", "公共统计获得"));
		jboperation.jb_operation.insert(std::make_pair("20", "其他获得"));
		jboperation.jb_operation.insert(std::make_pair("1001", "商店消耗"));
		jboperation.jb_operation.insert(std::make_pair("1002", "仓库存取消耗"));
		jboperation.jb_operation.insert(std::make_pair("1003", "传送消耗"));
		jboperation.jb_operation.insert(std::make_pair("1004", "交易消耗"));
		jboperation.jb_operation.insert(std::make_pair("1005", "修理消耗"));
		jboperation.jb_operation.insert(std::make_pair("1006", "邮件消耗"));
		jboperation.jb_operation.insert(std::make_pair("1007", "拍卖消耗"));
		jboperation.jb_operation.insert(std::make_pair("1008", "帮派消耗"));
		jboperation.jb_operation.insert(std::make_pair("1009", "转载消耗"));
		jboperation.jb_operation.insert(std::make_pair("1010", "商城消耗"));
		jboperation.jb_operation.insert(std::make_pair("1011", "坐骑消耗"));
		jboperation.jb_operation.insert(std::make_pair("1012", "弩炮消耗"));
		jboperation.jb_operation.insert(std::make_pair("1013", "时装消耗"));
		jboperation.jb_operation.insert(std::make_pair("1014", "生产消耗"));
		jboperation.jb_operation.insert(std::make_pair("1015", "强化消耗"));
		jboperation.jb_operation.insert(std::make_pair("1016", "真消耗"));
		jboperation.jb_operation.insert(std::make_pair("1017", "监狱消耗"));
		jboperation.jb_operation.insert(std::make_pair("1018", "技能消耗"));
		jboperation.jb_operation.insert(std::make_pair("1019", "拆除镶嵌消耗"));
		jboperation.jb_operation.insert(std::make_pair("1020", "宠物消耗"));
		jboperation.jb_operation.insert(std::make_pair("1021", "竞技场消耗"));
		jboperation.jb_operation.insert(std::make_pair("1022", "任务消耗"));
		jboperation.jb_operation.insert(std::make_pair("1023", "骑战消耗"));
		jboperation.jb_operation.insert(std::make_pair("1024", "青龙消耗"));
		jboperation.jb_operation.insert(std::make_pair("1025", "白虎消耗"));
		jboperation.jb_operation.insert(std::make_pair("1026", "朱雀消耗"));
		jboperation.jb_operation.insert(std::make_pair("1027", "玄武消耗"));
		jboperation.jb_operation.insert(std::make_pair("1028", "扩充包裹消耗"));
		jboperation.jb_operation.insert(std::make_pair("1029", "宝箱消耗"));
		jboperation.jb_operation.insert(std::make_pair("1030", "武魂消耗"));
		jboperation.jb_operation.insert(std::make_pair("1031", "公共统计消耗"));
		jboperation.jb_operation.insert(std::make_pair("1032", "帮会膜拜消耗"));
		jboperation.jb_operation.insert(std::make_pair("1033", "活动传送消耗"));
		jboperation.jb_operation.insert(std::make_pair("1034", "刷活动任务消耗"));
		jboperation.jb_operation.insert(std::make_pair("1035", "活力消耗"));
		jboperation.jb_operation.insert(std::make_pair("1036", "任务一键完成消耗"));
		jboperation.jb_operation.insert(std::make_pair("1037", "任务吞噬消耗"));
		jboperation.jb_operation.insert(std::make_pair("1038", "进阶日活动消耗"));
		jboperation.jb_operation.insert(std::make_pair("1039", "资源回购消耗"));
		jboperation.jb_operation.insert(std::make_pair("1040", "进阶石升级消耗"));
		jboperation.jb_operation.insert(std::make_pair("1041", "其他消耗"));

		boost::ajson::save_to_buff(jboperation, stringconfig);
	}
	if (type == "profession")
	{
		StringStreamType str;
		_Profession _profession;
		_profession.profession.insert(std::make_pair("2", "武当"));
		_profession.profession.insert(std::make_pair("3", "昆仑"));
		_profession.profession.insert(std::make_pair("4", "少林"));
		_profession.profession.insert(std::make_pair("5", "崆峒"));
		_profession.profession.insert(std::make_pair("6", "峨嵋"));
		_profession.profession.insert(std::make_pair("7", "华山"));

		boost::ajson::save_to_buff(_profession, stringconfig);
	}
	if (type == "vip_level")
	{
		StringStreamType str;
		Viplevel viplevel;
		viplevel.vip_level.insert(std::make_pair("1", "VIP1"));
		viplevel.vip_level.insert(std::make_pair("2", "VIP2"));
		viplevel.vip_level.insert(std::make_pair("3", "VIP3"));
		viplevel.vip_level.insert(std::make_pair("4", "VIP4"));
		viplevel.vip_level.insert(std::make_pair("5", "VIP5"));
		viplevel.vip_level.insert(std::make_pair("6", "VIP6"));
		viplevel.vip_level.insert(std::make_pair("7", "VIP7"));
		viplevel.vip_level.insert(std::make_pair("8", "VIP8"));
		viplevel.vip_level.insert(std::make_pair("9", "VIP9"));

		boost::ajson::save_to_buff(viplevel, stringconfig);
	}
	if (type == "warpot_type")
	{
		StringStreamType str;
		Scoretype scoretype;
		scoretype.warpot_type.insert(std::make_pair("1", "等级"));
		scoretype.warpot_type.insert(std::make_pair("2", "装备"));
		scoretype.warpot_type.insert(std::make_pair("3", "称号"));
		scoretype.warpot_type.insert(std::make_pair("4", "坐骑"));
		scoretype.warpot_type.insert(std::make_pair("5", "弩炮"));
		scoretype.warpot_type.insert(std::make_pair("6", "宠物"));
		scoretype.warpot_type.insert(std::make_pair("7", "骑战"));
		scoretype.warpot_type.insert(std::make_pair("8", "技能"));
		scoretype.warpot_type.insert(std::make_pair("9", "心法"));
		scoretype.warpot_type.insert(std::make_pair("10", "BUFF"));

		boost::ajson::save_to_buff(scoretype, stringconfig);
	}
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", stringconfig.str());
#endif
}


void SC_ServerDebugInterface::GetMatchIPCharList(const char* charip)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	StringStreamType matchchar;
	uint32_t uip = inet_addr(charip);
	uip = ntohl(uip);
	const object_id_set& idset = playManager->GetIDSetAcrdLoginIP(uip);
	OnlinRole onlinerole;
	mem::vector<OnlinRole> vOnlinRole;
	object_id_set::const_iterator iter = idset.begin();
	for (; iter != idset.end(); iter++)
	{
		LC_ServerPlayer* pkPlayer = _findPlayer(*iter);
		if (!pkPlayer)
		{
			return;
		}
		onlinerole.uid = pkPlayer->GetUserName().c_str();
		StringStreamType stream;
		stream << pkPlayer->GetID();
		onlinerole.charid = stream.str();
		vOnlinRole.push_back(onlinerole);
	}


	boost::ajson::save_to_buff(vOnlinRole, matchchar);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", matchchar.str());
}

void SC_ServerDebugInterface::GetRoleBaseInfo(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}
	StringStreamType charbaseinfo;
	BaseInfo rolebaseinfo;
	rolebaseinfo.Base.charid = pkPlayer->GetInstance();
	rolebaseinfo.Base.userid = pkPlayer->GetUserName();
	rolebaseinfo.Base.charlevel = pkPlayer->GetLevel();
	boost::ajson::save_to_buff(rolebaseinfo, charbaseinfo);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", charbaseinfo.str());
}
void SC_ServerDebugInterface::GetRoleBaseInfoByUID(unique_id_type charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}
	StringStreamType charbaseinfo;
	BaseInfo rolebaseinfo;
	rolebaseinfo.Base.charid = pkPlayer->GetInstance();
	rolebaseinfo.Base.userid = pkPlayer->GetUserName();
	rolebaseinfo.Base.charlevel = pkPlayer->GetLevel();
	boost::ajson::save_to_buff(rolebaseinfo, charbaseinfo);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", charbaseinfo.str());
}

void SC_ServerDebugInterface::GetRoleVehicleInfo(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType charVehicleinfo;
	charVehicleinfo << "{\"坐骑阶数\":";
	charVehicleinfo << pkPlayer->GetVehicleLevel();
	charVehicleinfo << ",\"坐骑技能\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_MOUNT);
	int32_t skillnum = 0;
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& mountskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)mountskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				charVehicleinfo << pkSkillData->_iID;
				charVehicleinfo << "\":\"";
				charVehicleinfo << pkSkillData->_kName;;
				charVehicleinfo << "_";
				charVehicleinfo << pkSkillAttrib->_lCurLevel;
				charVehicleinfo << "重";
				charVehicleinfo << "\",\"";
				skillnum ++;
			}
		}
	}
	if (skillnum != 0)
	{
		StringType strVehicleinfo = charVehicleinfo.str().substr(0, charVehicleinfo.str().length() - 3);
		StringStreamType Vehicleinfo;
		Vehicleinfo << strVehicleinfo;
		Vehicleinfo << "\"}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", Vehicleinfo.str());
	}
	else
	{
		StringType strVehicleinfo = charVehicleinfo.str().substr(0, charVehicleinfo.str().length() - 1);
		StringStreamType Vehicleinfo;
		Vehicleinfo << strVehicleinfo;
		Vehicleinfo << "}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", Vehicleinfo.str());
	}

}
void SC_ServerDebugInterface::GetRoleVehicleInfoByUID(unique_id_type charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType charVehicleinfo;
	charVehicleinfo << "{\"坐骑阶数\":";
	charVehicleinfo << pkPlayer->GetVehicleLevel();
	charVehicleinfo << ",\"坐骑技能\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_MOUNT);
	int32_t skillnum = 0;
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& mountskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)mountskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				charVehicleinfo << pkSkillData->_iID;
				charVehicleinfo << "\":\"";
				charVehicleinfo << pkSkillData->_kName;;
				charVehicleinfo << "_";
				charVehicleinfo << pkSkillAttrib->_lCurLevel;
				charVehicleinfo << "重";
				charVehicleinfo << "\",\"";
				skillnum ++;
			}
		}
	}
	if (skillnum != 0)
	{
		StringType strVehicleinfo = charVehicleinfo.str().substr(0, charVehicleinfo.str().length() - 3);
		StringStreamType Vehicleinfo;
		Vehicleinfo << strVehicleinfo;
		Vehicleinfo << "\"}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", Vehicleinfo.str());
	}
	else
	{
		StringType strVehicleinfo = charVehicleinfo.str().substr(0, charVehicleinfo.str().length() - 1);
		StringStreamType Vehicleinfo;
		Vehicleinfo << strVehicleinfo;
		Vehicleinfo << "}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", Vehicleinfo.str());
	}

}

void SC_ServerDebugInterface::GetRoleArrowInfo(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType chararrowinfo;
	chararrowinfo << "{\"弩炮阶数\":";
	chararrowinfo << pkPlayer->GetBowLevel();
	chararrowinfo << ",\"弩炮技能\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_ARROW);
	int32_t skillnum = 0;
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& arrowskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)arrowskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				chararrowinfo << pkSkillData->_iID;
				chararrowinfo << "\":\"";
				chararrowinfo << pkSkillData->_kName;;
				chararrowinfo << "_";
				chararrowinfo << pkSkillAttrib->_lCurLevel;
				chararrowinfo << "重";
				chararrowinfo << "\",\"";
				skillnum ++;
			}
		}
	}
	if (skillnum != 0)
	{
		StringType strarrowinfo = chararrowinfo.str().substr(0, chararrowinfo.str().length() - 3);
		StringStreamType arrowinfo;
		arrowinfo << strarrowinfo;
		arrowinfo << "\"}}";

		SetGMCommandResult(RE_SUCCESS, "SUCCESS", arrowinfo.str());
	}
	else
	{
		StringType strarrowinfo = chararrowinfo.str().substr(0, chararrowinfo.str().length() - 1);
		StringStreamType arrowinfo;
		arrowinfo << strarrowinfo;
		arrowinfo << "}}";

		SetGMCommandResult(RE_SUCCESS, "SUCCESS", arrowinfo.str());
	}

}
void SC_ServerDebugInterface::GetRoleArrowInfoByUID(unique_id_type charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType chararrowinfo;
	chararrowinfo << "{\"弩炮阶数\":";
	chararrowinfo << pkPlayer->GetBowLevel();
	chararrowinfo << ",\"弩炮技能\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_ARROW);
	int32_t skillnum = 0;
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& arrowskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)arrowskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				chararrowinfo << pkSkillData->_iID;
				chararrowinfo << "\":\"";
				chararrowinfo << pkSkillData->_kName;;
				chararrowinfo << "_";
				chararrowinfo << pkSkillAttrib->_lCurLevel;
				chararrowinfo << "重";
				chararrowinfo << "\",\"";
				skillnum ++;
			}
		}
	}
	if (skillnum != 0)
	{
		StringType strarrowinfo = chararrowinfo.str().substr(0, chararrowinfo.str().length() - 3);
		StringStreamType arrowinfo;
		arrowinfo << strarrowinfo;
		arrowinfo << "\"}}";

		SetGMCommandResult(RE_SUCCESS, "SUCCESS", arrowinfo.str());
	}
	else
	{
		StringType strarrowinfo = chararrowinfo.str().substr(0, chararrowinfo.str().length() - 1);
		StringStreamType arrowinfo;
		arrowinfo << strarrowinfo;
		arrowinfo << "}}";

		SetGMCommandResult(RE_SUCCESS, "SUCCESS", arrowinfo.str());
	}

}

void SC_ServerDebugInterface::GetRoleDevaeyeInfo(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType charDevaeyeinfo;
	charDevaeyeinfo << "{\"宠物阶数\":";
	charDevaeyeinfo << pkPlayer->GetDevaEyeLevel();
	charDevaeyeinfo << ",\"宠物技能\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_DEVAEYE);
	int32_t skillnum = 0;
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& Devaeyeskill = rCategoryData.GetSkillSlots();
		for (size_t i = 0; i < Devaeyeskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry((int)i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				charDevaeyeinfo << pkSkillData->_iID;
				charDevaeyeinfo << "\":\"";
				charDevaeyeinfo << pkSkillData->_kName;;
				charDevaeyeinfo << "_";
				charDevaeyeinfo << pkSkillAttrib->_lCurLevel;
				charDevaeyeinfo << "重";
				charDevaeyeinfo << "\",\"";
				skillnum ++;
			}
		}
	}

	if (skillnum != 0)
	{
		StringType strDevaeyeinfo = charDevaeyeinfo.str().substr(0, charDevaeyeinfo.str().length() - 3);
		StringStreamType Devaeyeinfo;
		Devaeyeinfo << strDevaeyeinfo;
		Devaeyeinfo << "\"}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", Devaeyeinfo.str());
	}
	else
	{
		StringType strDevaeyeinfo = charDevaeyeinfo.str().substr(0, charDevaeyeinfo.str().length() - 1);
		StringStreamType Devaeyeinfo;
		Devaeyeinfo << strDevaeyeinfo;
		Devaeyeinfo << "}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", Devaeyeinfo.str());
	}

}
void SC_ServerDebugInterface::GetRoleDevaeyeInfoByUID(unique_id_type charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType charDevaeyeinfo;
	charDevaeyeinfo << "{\"宠物阶数\":";
	charDevaeyeinfo << pkPlayer->GetDevaEyeLevel();
	charDevaeyeinfo << ",\"宠物技能\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_DEVAEYE);
	int32_t skillnum = 0;
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& Devaeyeskill = rCategoryData.GetSkillSlots();
		for (size_t i = 0; i < Devaeyeskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry((int)i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				charDevaeyeinfo << pkSkillData->_iID;
				charDevaeyeinfo << "\":\"";
				charDevaeyeinfo << pkSkillData->_kName;;
				charDevaeyeinfo << "_";
				charDevaeyeinfo << pkSkillAttrib->_lCurLevel;
				charDevaeyeinfo << "重";
				charDevaeyeinfo << "\",\"";
				skillnum ++;
			}
		}
	}

	if (skillnum != 0)
	{
		StringType strDevaeyeinfo = charDevaeyeinfo.str().substr(0, charDevaeyeinfo.str().length() - 3);
		StringStreamType Devaeyeinfo;
		Devaeyeinfo << strDevaeyeinfo;
		Devaeyeinfo << "\"}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", Devaeyeinfo.str());
	}
	else
	{
		StringType strDevaeyeinfo = charDevaeyeinfo.str().substr(0, charDevaeyeinfo.str().length() - 1);
		StringStreamType Devaeyeinfo;
		Devaeyeinfo << strDevaeyeinfo;
		Devaeyeinfo << "}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", Devaeyeinfo.str());
	}

}

void SC_ServerDebugInterface::GetRoleVehicleFightingInfo(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType charVehicleFightinginfo;
	charVehicleFightinginfo << "{\"骑战阶数\":";
	charVehicleFightinginfo << pkPlayer->GetVehicleFightingLevel();
	charVehicleFightinginfo << ",\"骑战技能\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_VEHICLE_FIGHTING);
	int32_t skillnum = 0;
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& VehicleFightingskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)VehicleFightingskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				charVehicleFightinginfo << pkSkillData->_iID;
				charVehicleFightinginfo << "\":\"";
				charVehicleFightinginfo << pkSkillData->_kName;;
				charVehicleFightinginfo << "_";
				charVehicleFightinginfo << pkSkillAttrib->_lCurLevel;
				charVehicleFightinginfo << "重";
				charVehicleFightinginfo << "\",\"";
				skillnum ++;
			}
		}
	}
	if (skillnum != 0)
	{
		StringType strVehicleFightinginfo = charVehicleFightinginfo.str().substr(0, charVehicleFightinginfo.str().length() - 3);
		StringStreamType VehicleFightinginfo;
		VehicleFightinginfo << strVehicleFightinginfo;
		VehicleFightinginfo << "\"}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", VehicleFightinginfo.str());
	}
	else
	{
		StringType strVehicleFightinginfo = charVehicleFightinginfo.str().substr(0, charVehicleFightinginfo.str().length() - 1);
		StringStreamType VehicleFightinginfo;
		VehicleFightinginfo << strVehicleFightinginfo;
		VehicleFightinginfo << "}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", VehicleFightinginfo.str());
	}

}
void SC_ServerDebugInterface::GetRoleVehicleFightingInfoByUID(unique_id_type charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType charVehicleFightinginfo;
	charVehicleFightinginfo << "{\"骑战阶数\":";
	charVehicleFightinginfo << pkPlayer->GetVehicleFightingLevel();
	charVehicleFightinginfo << ",\"骑战技能\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();

	LC_SkillCategoryMap::const_iterator cit = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_VEHICLE_FIGHTING);
	int32_t skillnum = 0;
	if (cit != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cit->second;
		const LC_SkillIDVector& VehicleFightingskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)VehicleFightingskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				charVehicleFightinginfo << pkSkillData->_iID;
				charVehicleFightinginfo << "\":\"";
				charVehicleFightinginfo << pkSkillData->_kName;;
				charVehicleFightinginfo << "_";
				charVehicleFightinginfo << pkSkillAttrib->_lCurLevel;
				charVehicleFightinginfo << "重";
				charVehicleFightinginfo << "\",\"";
				skillnum ++;
			}
		}
	}
	if (skillnum != 0)
	{
		StringType strVehicleFightinginfo = charVehicleFightinginfo.str().substr(0, charVehicleFightinginfo.str().length() - 3);
		StringStreamType VehicleFightinginfo;
		VehicleFightinginfo << strVehicleFightinginfo;
		VehicleFightinginfo << "\"}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", VehicleFightinginfo.str());
	}
	else
	{
		StringType strVehicleFightinginfo = charVehicleFightinginfo.str().substr(0, charVehicleFightinginfo.str().length() - 1);
		StringStreamType VehicleFightinginfo;
		VehicleFightinginfo << strVehicleFightinginfo;
		VehicleFightinginfo << "}}";
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", VehicleFightinginfo.str());
	}

}

void SC_ServerDebugInterface::GetRoleSectSkillInfo(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType charSectSkillinfo;
	charSectSkillinfo << "{\"类型\":\"门派技能\",\"列表\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();
	LC_SkillCategoryMap::const_iterator cits = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_SECT);
	if (cits != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cits->second;
		const LC_SkillIDVector& roleskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)roleskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}

				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				charSectSkillinfo << pkSkillData->_iID;
				charSectSkillinfo << "\":\"";
				charSectSkillinfo << pkSkillData->_kName;;
				charSectSkillinfo << "_";
				charSectSkillinfo << pkSkillAttrib->_lCurLevel;
				charSectSkillinfo << "重";
				charSectSkillinfo << "\",\"";
			}
		}
	}
	StringType strSectSkillinfo = charSectSkillinfo.str().substr(0, charSectSkillinfo.str().length() - 3);
	StringStreamType SectSkillinfo;
	SectSkillinfo << strSectSkillinfo;
	SectSkillinfo << "\"}}";


	SetGMCommandResult(RE_SUCCESS, "SUCCESS", SectSkillinfo.str());
}
void SC_ServerDebugInterface::GetRoleSectSkillInfoByUID(unique_id_type charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}

	StringStreamType charSectSkillinfo;
	charSectSkillinfo << "{\"类型\":\"门派技能\",\"列表\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();
	LC_SkillCategoryMap::const_iterator cits = kCatagoryMap.find((skill_cat_type)SKILL_TYPE_SECT);
	if (cits != kCatagoryMap.end())
	{
		const LC_SkillCategoryData& rCategoryData = cits->second;
		const LC_SkillIDVector& roleskill = rCategoryData.GetSkillSlots();
		for (int i = 0; i < (int)roleskill.size(); i++)
		{
			const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
			if (pEntry != NULL)
			{
				CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
				if(pkSkillAttrib == NULL)
				{
					continue;
				}
				CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
				if (pkSkillData == NULL)
				{
					continue;
				}
				charSectSkillinfo << pkSkillData->_iID;
				charSectSkillinfo << "\":\"";
				charSectSkillinfo << pkSkillData->_kName;;
				charSectSkillinfo << "_";
				charSectSkillinfo << pkSkillAttrib->_lCurLevel;
				charSectSkillinfo << "重";
				charSectSkillinfo << "\",\"";
			}
		}
	}
	StringType strSectSkillinfo = charSectSkillinfo.str().substr(0, charSectSkillinfo.str().length() - 3);
	StringStreamType SectSkillinfo;
	SectSkillinfo << strSectSkillinfo;
	SectSkillinfo << "\"}}";


	SetGMCommandResult(RE_SUCCESS, "SUCCESS", SectSkillinfo.str());
}

void SC_ServerDebugInterface::GetRoleMentalSkillInfo(object_id_type playerID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	CF_Mental* pkCSVMental = CF_Mental::GetSingletonPtr();
	StringStreamType charMentalSkillinfo;
	charMentalSkillinfo << "{\"类型\":\"心法技能\",\"列表\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();
	for (int i = SKILL_TYPE_MENTAL_BEIGN; i < SKILL_TYPE_MENTAL_END; ++i)
	{
		LC_SkillCategoryMap::const_iterator cits = kCatagoryMap.find((skill_cat_type)i);
		if (cits != kCatagoryMap.end())
		{
			const LC_SkillCategoryData& rCategoryData = cits->second;
			const LC_SkillIDVector& roleskill = rCategoryData.GetSkillSlots();
			mem::vector<int32_t> vecmetalattrskill;
			for (int i = 0; i < (int)roleskill.size(); i++)
			{
				const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
				if (pEntry != NULL)
				{
					CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
					if (pkSkillData == NULL || pkSkillData->_lCategory1 != 1010)
					{
						continue;
					}
					vecmetalattrskill.push_back(pkSkillData->_iID);
				}
			}

			for (int i = 0; i < (int)roleskill.size(); i++)
			{
				const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
				if (pEntry != NULL)
				{
					CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
					if(pkSkillAttrib == NULL)
					{
						continue;
					}

					CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
					if (pkSkillData == NULL)
					{
						continue;
					}
					if (pkSkillData->_lCategory1 != 1011)
					{
						continue;
					}
					charMentalSkillinfo << pkSkillData->_iID;
					charMentalSkillinfo << "\":\"";
					charMentalSkillinfo << pkSkillData->_kName;;
					charMentalSkillinfo << "_";
					charMentalSkillinfo << pkSkillAttrib->_lCurLevel;
					charMentalSkillinfo << "重";
					mem::vector<int32_t>::const_iterator veciter =  vecmetalattrskill.begin();
					for (; veciter != vecmetalattrskill.end(); veciter++)
					{
						CF_SkillExecuteList::DataEntry* pkMentalSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(*veciter);
						if (pkMentalSkillData == NULL)
						{
							continue;
						}
						CF_Mental::DataEntryMapExternIterator pkIter = pkCSVMental->GetIterator();
						CF_Mental::DataEntry* pkEntry = NULL;
						while (pkIter.HasMoreElements())
						{
							pkEntry = pkIter.PeekNextValuePtr();
							if (pkEntry->_lResistanceID == (pkSkillData->_iID - pkSkillAttrib->_lCurLevel + 1) && (*veciter) > pkEntry->_lAttribID && (*veciter) <= pkEntry->_lAttribID + 40)
							{
								charMentalSkillinfo << "_属性";
								charMentalSkillinfo << pkMentalSkillData->_kName;
							}
							pkIter.MoveNext();
						}
					}
					charMentalSkillinfo << "\",\"";
				}
			}
		}
	}

	StringType strMentalSkillinfo = charMentalSkillinfo.str().substr(0, charMentalSkillinfo.str().length() - 3);
	StringStreamType MentalSkillinfo;
	MentalSkillinfo << strMentalSkillinfo;
	MentalSkillinfo << "\"}}";

	SetGMCommandResult(RE_SUCCESS, "SUCCESS", MentalSkillinfo.str());
}
void SC_ServerDebugInterface::GetRoleMentalSkillInfoByUID(unique_id_type charID)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}

	CF_Mental* pkCSVMental = CF_Mental::GetSingletonPtr();

	StringStreamType charMentalSkillinfo;
	charMentalSkillinfo << "{\"类型\":\"心法技能\",\"列表\":{\"";

	LC_SkillAssetData* data = pkPlayer->GetSkillAsset()->GetSkillAssetData();
	const LC_SkillCategoryMap& kCatagoryMap = data->GetSkillCatagoryMap();
	for (int i = SKILL_TYPE_MENTAL_BEIGN; i < SKILL_TYPE_MENTAL_END; ++i)
	{
		LC_SkillCategoryMap::const_iterator cits = kCatagoryMap.find((skill_cat_type)i);
		if (cits != kCatagoryMap.end())
		{
			const LC_SkillCategoryData& rCategoryData = cits->second;
			const LC_SkillIDVector& roleskill = rCategoryData.GetSkillSlots();
			mem::vector<int32_t> vecmetalattrskill;
			for (int i = 0; i < (int)roleskill.size(); i++)
			{
				const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
				if (pEntry != NULL)
				{
					CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
					if (pkSkillData == NULL || pkSkillData->_lCategory1 != 1010)
					{
						continue;
					}
					vecmetalattrskill.push_back(pkSkillData->_iID);
				}
			}

			for (int i = 0; i < (int)roleskill.size(); i++)
			{
				const LC_SkillAssetEntry* pEntry = rCategoryData.GetSkillAssetEntry(i);
				if (pEntry != NULL)
				{
					CF_SkillSpecialAttrib::DataEntry* pkSkillAttrib = SafeGetCSVFileDataEntryPtr<CF_SkillSpecialAttrib>(pEntry->GetTypeID());
					if (pkSkillAttrib == NULL)
					{
						continue;
					}
					CF_SkillExecuteList::DataEntry* pkSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(pEntry->GetTypeID());
					if (pkSkillData == NULL)
					{
						continue;
					}
					if (pkSkillData->_lCategory1 != 1011)
					{
						continue;
					}
					charMentalSkillinfo << pkSkillData->_iID;
					charMentalSkillinfo << "\":\"";
					charMentalSkillinfo << pkSkillData->_kName;;
					charMentalSkillinfo << "_";
					charMentalSkillinfo << pkSkillAttrib->_lCurLevel;
					charMentalSkillinfo << "重";
					mem::vector<int32_t>::const_iterator veciter =  vecmetalattrskill.begin();
					for (; veciter != vecmetalattrskill.end(); veciter++)
					{
						CF_SkillExecuteList::DataEntry* pkMentalSkillData = LC_Helper::SafeGetSkillExecuteDataEntryPtr(*veciter);
						if (pkMentalSkillData == NULL)
						{
							continue;
						}
						CF_Mental::DataEntryMapExternIterator pkIter = pkCSVMental->GetIterator();
						CF_Mental::DataEntry* pkEntry = NULL;
						while (pkIter.HasMoreElements())
						{
							pkEntry = pkIter.PeekNextValuePtr();
							if (pkEntry->_lResistanceID == (pkSkillData->_iID - pkSkillAttrib->_lCurLevel + 1) && (*veciter) > pkEntry->_lAttribID && (*veciter) <= pkEntry->_lAttribID + 40)
							{
								charMentalSkillinfo << "_属性";
								charMentalSkillinfo << pkMentalSkillData->_kName;
							}
							pkIter.MoveNext();
						}
					}
					charMentalSkillinfo << "\",\"";
				}
			}
		}
	}

	StringType strMentalSkillinfo = charMentalSkillinfo.str().substr(0, charMentalSkillinfo.str().length() - 3);
	StringStreamType MentalSkillinfo;
	MentalSkillinfo << strMentalSkillinfo;
	MentalSkillinfo << "\"}}";

	SetGMCommandResult(RE_SUCCESS, "SUCCESS", MentalSkillinfo.str());
}

void SC_ServerDebugInterface::GetRoleInfolist()
{
	StringStreamType strinfolist;
	mem::vector<std::string> roleinfolist;
	roleinfolist.push_back("base");
	roleinfolist.push_back("vehicle");
	roleinfolist.push_back("arrow");
	roleinfolist.push_back("devaeye");
	roleinfolist.push_back("vehiclefighting");
	roleinfolist.push_back("sectskill");
	roleinfolist.push_back("mentalskill");
	boost::ajson::save_to_buff(roleinfolist, strinfolist);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", strinfolist.str());
}

void SC_ServerDebugInterface::GetRoleInfoBytype(unique_id_type charID, const StringType& type)
{
	if (type == "base")
	{
		GetRoleBaseInfoByUID(charID);
	}
	if (type == "vehicle")
	{
		GetRoleVehicleInfoByUID(charID);
	}
	if (type == "arrow")
	{
		GetRoleArrowInfoByUID(charID);
	}
	if (type == "devaeye")
	{
		GetRoleDevaeyeInfoByUID(charID);
	}
	if (type == "vehiclefighting")
	{
		GetRoleVehicleFightingInfoByUID(charID);
	}
	if (type == "sectskill")
	{
		GetRoleSectSkillInfoByUID(charID);
	}
	if (type == "mentalskill")
	{
		GetRoleMentalSkillInfoByUID(charID);
	}
	return;
}

void SC_ServerDebugInterface::GetTaskInfolistBytype(int type)
{
	CF_TaskList* pkCSVTaskList = CF_TaskList::GetSingletonPtr();
	CF_TaskList::DataEntryMapExternIterator pkIter = pkCSVTaskList->GetIterator();
	CF_TaskList::DataEntry* pkEntry = NULL;
	StringStreamType Taskinfolist;
	VTaskInfo tasklist;
	Taskinfolist << "{\"task_list\":{\"";

	while (pkIter.HasMoreElements())
	{
		pkEntry = pkIter.PeekNextValuePtr();
		if (pkEntry->_iTaskTitleType != type)
		{
			pkIter.MoveNext();
			continue;
		}

		Taskinfolist << pkEntry->_iID;
		Taskinfolist << "\":\"";

		Taskinfolist << pkEntry->_kTaskGoal;


		pkIter.MoveNext();
		Taskinfolist << "\",\"";

	}
	StringType strtaskinfo = Taskinfolist.str().substr(0, Taskinfolist.str().length() - 3);
	StringStreamType strtaskinfolist;
	strtaskinfolist << strtaskinfo;
	strtaskinfolist << "\"}}";

	SetGMCommandResult(RE_SUCCESS, "SUCCESS", strtaskinfolist.str());
}

void SC_ServerDebugInterface::SetFCM(object_id_type playerID, int type)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}
	pkPlayer->SetAdult(type > 0);
	pkPlayer->Sync_AnitWallowOnlineTime();
}

void SC_ServerDebugInterface::ClearPack(object_id_type playerID, int type)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}

	pkPlayer->GetPackAsset()->ClearPack(type);
}

void SC_ServerDebugInterface::GetUnionRegUserCount()
{
	GetUnionUserCountByKey("_global_palyer_create_flag");
}

void SC_ServerDebugInterface::GetUnionOnLineUserCount()
{
	GetUnionUserCountByKey("_global_palyer_online_flag");
}

void SC_ServerDebugInterface::GetUnionServerRecharge()
{
	GetUnionUserCountByKey("_global_palyer_recharge_flag");
}

void SC_ServerDebugInterface::GetUnionUserCountByKey(const std::string& key)
{
	int iRet = 0;
	std::string sKeyPrefix = GetGameServerApp()->GetRedisGroupPrefix() + UT_REDIS_KEY_USER_SVRIDS;

	Utility::Redis_Handler* pHandler = GetGameServerApp()->GetRedisHandler();
	UT_REDIS_CMD_BEGIN(iRet, pHandler)
	std::string sField = sKeyPrefix + UT_REDIS_STRINGFY(UT_REDIS_KEY_SEPATATOR) + key;

	redispp::StringReply sSvrIDReply = pkRedisConn->get(sField);

	if (sSvrIDReply.result())
	{
		std::string strRel = sSvrIDReply;
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", strRel.c_str());
	}
	else
	{
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", "[]");
	}
	UT_REDIS_CMD_END(iRet, pHandler)
}

void SC_ServerDebugInterface::ResetPlayerGuildRaid(object_id_type playerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer != NULL)
	{
		uint64_t nGuildID = pkPlayer->GetGuildID();
		if (nGuildID != INVALID_GUILD_ID)
		{
			LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
			if (pkGuildManager != NULL)
			{
				LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
				if (guild != NULL)
				{
					guild->ResetGuildRaid(true, false);
				}
			}
		}
	}
}

void DealPlayerGMRequest(LC_ServerPlayer* pPlayer, int gmType, const StringType& strInfo)
{
	if(pPlayer != NULL && pPlayer->DealSelfGMRequest(gmType, strInfo) == true)
	{
		return;
	}
	switch(gmType)
	{
	case 2:		//清除玩家的Hold和Cross状态
		{
			mem::vector<StringType> strList;
			Utility::tokenize(strInfo, strList, "_");
			size_t nSize = strList.size();
			unique_id_type nCharId = 0;
			uint32_t nOptType = 0;
			__Get_Token_Value(nCharId, strList, nSize, 0, uint64_t, atoll);
			__Get_Token_Value(nOptType, strList, nSize, 1, uint32_t, atoi);

			LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
			if (enterLeaveManager != NULL)
			{
				if(nOptType == 0 || nOptType == 1)
				{
					enterLeaveManager->ClearPlayerHold(nCharId);
				}
				if(nOptType == 0 || nOptType == 2)
				{
					enterLeaveManager->ClearCrossRealmIndex(nCharId);
				}
			}
		}
		break;
	case 3:		//清除玩家Index
		{
			object_id_type nPlayerID = boost::lexical_cast<uint32_t>(strInfo);
			LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
			LC_ServerPlayer* targetPlayer = (LC_ServerPlayer*) pkPlayerManager->FindPlayer(nPlayerID);
			if (NULL == targetPlayer) 
			{
				return;
			}
			if (targetPlayer->GetID() != nPlayerID) 
			{
				pkPlayerManager->ClearPlayerIndex(nPlayerID);
			}
			else 
			{
				LC_ServerPlayerEnterLeaveManager* enterLeaveManager = SERVER_GET_PLAYERENTERLEAVE_MANAGER();
				if (NULL != enterLeaveManager) 
				{
					enterLeaveManager->UserLeaveServer(targetPlayer, LMIOT_KICKOFF);
				}
			}
		}
		break;
	case 4:		//玩家退出队伍
		{
			unique_id_type nCharID = boost::lexical_cast<unique_id_type>(strInfo);
			LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
			LC_ServerPlayer* targetPlayer = (LC_ServerPlayer*) pkPlayerManager->FindPlayer(nCharID);
			if (NULL == targetPlayer) 
			{
				return;
			}
			LC_ServerPlayerGroupManager* pkPlayerGroupManager = (LC_ServerPlayerGroupManager*)SERVER_GET_PLAYER_GROUP_MANAGER();
			if(pkPlayerGroupManager == NULL)
			{
				return;
			}
			LC_ServerPlayerGroup* group = pkPlayerGroupManager->GetPlayerGroup(targetPlayer->GetInstance());
			if(group != NULL)
			{
				bool destory = false;
				pkPlayerGroupManager->leaveGroup(targetPlayer, group, destory);
			}
		}
		break;
	case 5:		//解散玩家队伍
		{
			unique_id_type nCharID = boost::lexical_cast<unique_id_type>(strInfo);
			LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
			LC_ServerPlayer* targetPlayer = (LC_ServerPlayer*) pkPlayerManager->FindPlayer(nCharID);
			if (NULL == targetPlayer) 
			{
				return;
			}
			LC_ServerPlayerGroupManager* pkPlayerGroupManager = (LC_ServerPlayerGroupManager*)SERVER_GET_PLAYER_GROUP_MANAGER();
			if(pkPlayerGroupManager == NULL)
			{
				return;
			}
			LC_ServerPlayerGroup* group = pkPlayerGroupManager->GetPlayerGroup(targetPlayer->GetInstance());
			if(group != NULL)
			{
				pkPlayerGroupManager->Dismiss(group);
			}
		}
		break;
	default:
		break;
	}
}

void SC_ServerDebugInterface::SendGM2TransServer(object_id_type playerID, int serverType, int gmType, const StringType& strInfo)
{
	PS_ServerGMInfo reqInfo;
	reqInfo.m_iType = gmType;
	reqInfo.m_sInfo = TPSTR2STDSTR(strInfo);

	if(serverType != SERVER_TYPE_GAME)
	{
		static UT_ServerCommandParser* pkCommandMgr = (UT_ServerCommandParser*)UT_ServerCommandParser::GetSingletonPtr();
		TS_Server_GM* pkGMTrans = pkCommandMgr->GetCommGMTrans();
		if(pkGMTrans != NULL)
		{
			reqInfo.m_iTransID = pkGMTrans->GetID();
			reqInfo.m_iMsgID = pkGMTrans->get_msgid();
			SetGMCommandResult(RE_IN_TRANS, "TRANS", "");
		}
	}
	_sendGM2Server(playerID, serverType, reqInfo, strInfo);
}

void SC_ServerDebugInterface::SendGM2Server(object_id_type playerID, int serverType, int gmType, const StringType& strInfo)
{
	PS_ServerGMInfo reqInfo;
	reqInfo.m_iType = gmType;
	reqInfo.m_sInfo = TPSTR2STDSTR(strInfo);

	_sendGM2Server(playerID, serverType, reqInfo, strInfo);
}

void SC_ServerDebugInterface::_sendGM2Server(object_id_type playerID, int serverType, PS_ServerGMInfo& reqInfo, const StringType& strInfo)
{
	LC_ServerPlayer* pPlayer = _findPlayer(playerID);

	switch (serverType)
	{
	case SERVER_TYPE_LOGIN:
		{
			MG_SS_LOGIN_GM_INFO req;
			req.m_gmInfo = reqInfo;
			if(pPlayer != NULL)
			{
				SendMsgToLogin(MGPT_GAME_COM_GM_MSG, &req, pPlayer->GetUserID(), pPlayer->GetInstance());
			}
			else
			{
				SendMsgToLogin(MGPT_GAME_COM_GM_MSG, &req);
			}
		}
		break;
	case SERVER_TYPE_GROUP:
		{
			MG_SS_GROUP_GM_INFO req;
			req.m_gmInfo = reqInfo;
			if(pPlayer != NULL)
			{
				SendMsgToGroup(MGPT_GAME_COM_GM_MSG, &req, pPlayer->GetUserID(), pPlayer->GetInstance());
			}
			else
			{
				SendMsgToGroup(MGPT_GAME_COM_GM_MSG, &req);
			}
		}
		break;
	case SERVER_TYPE_SOCIETY:
		{
			MG_SS_GUILD_GM_INFO req;
			req.m_gmInfo = reqInfo;
			if(pPlayer != NULL)
			{
				SendMsgToSociety(MGPT_GAME_COM_GM_MSG, &req, pPlayer->GetUserID(), pPlayer->GetInstance(), pPlayer->GetCitizenship(), GetGateWayServerID(pPlayer));
			}
			else
			{
				SendMsgToSociety(MGPT_GAME_COM_GM_MSG, &req);
			}
		}
		break;
	case SERVER_TYPE_GAME:
		{
			DealPlayerGMRequest(pPlayer, reqInfo.m_iType, strInfo);
		}
		break;
	default:
		break;
	}
}

void SC_ServerDebugInterface::SetInternal(object_id_type playerID, int nStatus)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (pkPlayer != NULL)
	{
		pkPlayer->SetInternal(nStatus > 0);
		pkPlayer->RegisterCharInfo();
		pkPlayer->SetDBSaveMaskFlag(LPDST_CHAR_MAX_COUNT, true);
	}
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "[]");
}

void SC_ServerDebugInterface::SetFreezeAcc(object_id_type userID, uint32_t nFreeze)
{
	MG_GL_Freeze_Player reqMsg;
	reqMsg.m_iTime = nFreeze;
	reqMsg.m_iType = 0;

	SendMsgToLogin(MGPT_GL_SYNC_FREEZE_PLAYER_REQ, &reqMsg, userID);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "[]");
}

void SC_ServerDebugInterface::SetFreezeRol(object_id_type userID, unique_id_type charID, uint32_t nFreeze)
{
	static UT_ServerCommandParser* pkCommandMgr = (UT_ServerCommandParser*)UT_ServerCommandParser::GetSingletonPtr();
	TS_Server_GM* pkGMTrans = pkCommandMgr->GetCommGMTrans();
	if (NULL == pkGMTrans)
	{
		SetGMCommandResult(RE_FAIL, "FAIL", "");
		return;
	}

	MG_GL_Freeze_Player reqMsg;
	reqMsg.m_iTime = nFreeze;
	reqMsg.m_iType = 1;
	reqMsg.m_iTransID = pkGMTrans->GetID();
	reqMsg.m_iMsgID = pkGMTrans->get_msgid();

	SendMsgToLogin(MGPT_GL_SYNC_FREEZE_PLAYER_REQ, &reqMsg, userID, charID);
	if (nFreeze > 0) {
		_kickPlayer(charID);
	}
	
	if (nFreeze >= 7 * ONE_DAY_SECONDS)
	{
		MG_SS_REQ_LeaderAutoChange reqLACMsg;
		SendMsgToSociety(MGPT_GGU_REQ_GUILD_LEADER_AUTOCHG, &reqLACMsg, userID, charID);
	}
	SetGMCommandResult(RE_IN_TRANS, "TRANS", "");
}

void SC_ServerDebugInterface::GetItemListByPage(int nPage, int nItemID, int nCount)
{
	CF_ItemList* pkCSVItemList = CF_ItemList::GetSingletonPtr();
	int nTotal = (int)pkCSVItemList->GetEntryCount();

	StringStreamType Iteminfolist;
	Union_ItemList vitemlist;
	vitemlist.Count = nTotal;
	vitemlist.Page = nPage;

	if (nItemID > 0)
	{
		//仅获取对应ID数据
		CF_ItemList::DataEntry* pkmEntry = pkCSVItemList->GetEntryPtr(nItemID);
		if (pkmEntry != NULL)
		{
			UnionItem item;
			item.ItemID = nItemID;
			item.ItemName = pkmEntry->_kName;
			item.ItemType = pkmEntry->_lCategory1;
			item.DeadlineType = 0;
			vitemlist.ItemList.push_back(item);
			vitemlist.Count = vitemlist.Page = 0;
		}
	}
	else if (nCount > 0)
	{
		//分页
		int nStart = nPage * nCount;
		if (nStart > nTotal)
		{
			nStart = nTotal;
		}
		int nEnd = nStart + nCount;
		int nIndex = 0;

		CF_ItemList::DataEntryMapExternIterator pkIter = pkCSVItemList->GetIterator();
		CF_ItemList::DataEntry* pkmEntry = NULL;

		UnionItem item;
		while (pkIter.HasMoreElements() && nIndex < nTotal && nIndex < nEnd)
		{
			pkmEntry = pkIter.PeekNextValuePtr();
			if (/*pkmEntry == NULL ||*/ nIndex < nStart)
			{
				pkIter.MoveNext();
				nIndex ++;
				continue;
			}

			item.ItemID = pkmEntry->_iID;
			item.ItemName = pkmEntry->_kName;
			item.ItemType = pkmEntry->_lCategory1;
			item.DeadlineType = 0;
			vitemlist.ItemList.push_back(item);
			nIndex ++;

			pkIter.MoveNext();
		}
	}
	boost::ajson::save_to_buff(vitemlist, Iteminfolist);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", Iteminfolist.str());

	StringType sFinal;
	sFinal = CodeConvert::MbsToUtf8(Iteminfolist.str());
	GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, __FUNCTION__, sFinal);
}

void SC_ServerDebugInterface::ReloadCSV(const StringType& rkCSVFile, const StringType& sVersion)
{
	GetGameServerApp()->ReloadCsv(rkCSVFile, sVersion);
}

void SC_ServerDebugInterface::ReloadStoryScript(const StringType& rkScriptFile, const StringType& sVersion)
{
	GameStory::GameStoryCommonManager* pkMgr = GameStoryCommonManager::GetSingletonPtr();
	if (pkMgr)
	{
		pkMgr->LoadStoryConfig(rkScriptFile, sVersion);
	}
}

void SC_ServerDebugInterface::PlatformInfoDirty(unique_id_type charID)
{
	
}

void SC_ServerDebugInterface::SetServerPlatformInfo(object_id_type playerID, const StringType& platform)
{

}

void SC_ServerDebugInterface::PlatformClientMsg(unique_id_type charID, const StringType& pt, const StringType& type, const StringType& content)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)playManager->FindPlayer(charID);
	if (NULL == pkPlayer)
	{
		return;
	}

	MG_PlatformMessage clientMsg;
	clientMsg.m_platform = TPSTR2STDSTR(pt);
	clientMsg.m_type = TPSTR2STDSTR(type);
	clientMsg.m_content = TPSTR2STDSTR(content);

	pkPlayer->SendMsgToClient(MGPT_SS_PLATFORM_MESSAGE, &clientMsg);

	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}

void SC_ServerDebugInterface::PlatformTest(object_id_type playerID, const StringType& platform, const StringType& platformVip, int is_vip, int is_year_vip, int vip_level, int is_high_vip, int is_expand_vip)
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)playManager->FindPlayer(playerID);
	if (NULL == pkPlayer)
	{
		return;
	}
	platform_vip_info ifo = pkPlayer->GetPlatformVipInfo();
	platform_node& rNode = ifo[platformVip];
	rNode.is_vip = is_vip;
	rNode.is_year_vip = is_year_vip;
	rNode.vip_level = vip_level;
	rNode.is_high_vip = is_high_vip;
	rNode.is_expand_vip = is_expand_vip;
	pkPlayer->SetPlatformVipInfoTest(ifo);
}

void SC_ServerDebugInterface::SetLoginPlatformInfo(object_id_type playerID, const StringType& platform)
{
}

void SC_ServerDebugInterface::DetachShield( object_id_type playerID, int stateID, object_id_type targetID )
{
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)playManager->FindPlayer(playerID);
	if (NULL == pkPlayer)
	{
		return;
	}
	
	SC_ServerScriptInterface::GetSingletonPtr()->GetSkillInterface()->DetachTargetState(pkPlayer, targetID, SMT_STATE_ID, stateID,0,0);
}

void SC_ServerDebugInterface::AddForbidWord(const StringType& wordlist)
{
	std::string tmpStr = TPSTR2STDSTR(wordlist);
 /*
	std::wstring wstrWrodList;
	Utility::MultiByteToWideChar(tmpStr, wstrWrodList);
	size_t pos = 0;
	for (std::wstring::size_type i=0; i < wstrWrodList.size(); ++i)
	{
		if (wstrWrodList[i] == L'-')
		{
			wstrWrodList[i] = L' ';
		}
	}
	tmpStr.clear();
	Utility::WideCharToMultiByte(wstrWrodList, tmpStr);
 */

	int ret = 0;
	redispp::IntReply reply;
	std::string sForbidKey = GetGameServerApp()->GetRedisGlobalPrefix() + CHAT_FORBID_WORD_LIST;
	UT_REDIS_CMD_WITH_REPLY(ret, GetGameServerApp()->GetGlobalRedisHandler(), reply, rpush, sForbidKey, tmpStr);
	if (0 == reply)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR, "rpush wordlist failed !");
	}
}

void SC_ServerDebugInterface::DelForbidWord(const StringType& wordlist)
{
	std::string tmpStr = TPSTR2STDSTR(wordlist);
	int ret = 0;
	redispp::IntReply reply;
	std::string sForbidKey = GetGameServerApp()->GetRedisGlobalPrefix() + CHAT_FORBID_WORD_LIST;
	UT_REDIS_CMD_WITH_REPLY(ret, GetGameServerApp()->GetGlobalRedisHandler(), reply, lrem, sForbidKey, 0, tmpStr);
	if (0 == reply)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR, "lrem wordlist failed !");
	}
}

void SC_ServerDebugInterface::ShowForbidWordList(object_id_type playerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
	{
		return;
	}

	mem::list<std::string> wordlist;
	TextFilterChannel::LoadForbidWordListFromRedis(GetGameServerApp()->GetGlobalRedisHandler(), wordlist);
	std::string msg;
	mem::list<std::string>::iterator it = wordlist.begin();
	for (; it != wordlist.end(); ++it)
	{
		msg += " ";
		msg += *it;
	}
	SendToPlayerConsole(playerID, STDSTR2TPSTR(msg));
}

void SC_ServerDebugInterface::ShowChatMonitorInfo(object_id_type playerID, unique_id_type targetCharID)
{
	LC_ServerPlayer* pkTargetPlayer = _findPlayerByUID(targetCharID);
	if (!pkTargetPlayer)
	{
		return;
	}
	LC_ChatChannelController* pkChatController = pkTargetPlayer->GetChatChannelController();
	StringStreamType ss;
	int32_t iRemainTime = pkChatController->GetSilentGagTime() - GET_CURRENT_TIMESTAMP_IN_SECONDS();
	iRemainTime = (iRemainTime < 0 ? 0 : iRemainTime);
	ss << pkTargetPlayer->GetOwnerCharName() << "，监控等级：" << pkChatController->GetChatMonitorLevel()
	   << "，总聊天数量：" << pkChatController->GetChatTotalCount() << "，带屏蔽字聊天数量：" << pkChatController->GetChatForbidCount()
	   << "，监控百分比：" << pkChatController->GetChatForbidRate() * 100 << "%，静默剩余时间：" << iRemainTime;
	SendToPlayerConsole(playerID, ss.str());
}

void SC_ServerDebugInterface::SetChatMonitorLevel(unique_id_type charID, int level)
{
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ChatChannelController* pkChatController = pkPlayer->GetChatChannelController();
	CF_ChatMonitor* pkCSVChatMonitor = CF_ChatMonitor::GetSingletonPtr();
	CF_ChatMonitor::DataEntry* pkEntry = pkCSVChatMonitor->GetEntryPtr(level);
	if (NULL == pkEntry)
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_ERROR, "Invalid monitoring type level[%d]", level);
		return;
	}
	pkChatController->SetChatMonitorLevel(level);
}

void SC_ServerDebugInterface::SetSilentGagTime(unique_id_type charID, int time)
{
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (!pkPlayer)
	{
		return;
	}
	LC_ChatChannelController* pkChatController = pkPlayer->GetChatChannelController();
	if (time == 0)
	{
		pkChatController->SetSilentGagTime(0);
	}
	else
	{
		pkChatController->AddSilentGagTime(time);
	}
}

void SC_ServerDebugInterface::GetGuildList(int32_t page)
{
	static UT_ServerCommandParser* pkCommandMgr = (UT_ServerCommandParser*)UT_ServerCommandParser::GetSingletonPtr();
	TS_Server_GM* pkGMTrans = pkCommandMgr->GetGuildGMTrans();
	if (NULL == pkGMTrans)
	{
		SetGMCommandResult(RE_FAIL, "FAIL", "");
		return;
	}
	
	MG_SS_REQ_GuildList kReqGuild;
	kReqGuild.m_ServerId = GetGameServerID();
	kReqGuild.m_uiIndex = page;
	kReqGuild.m_uiCount = MAX_GUILD_SEARCH_NUM;
	kReqGuild.m_SortType = 2;
	kReqGuild.m_SortCol = 3; //level

	SendMsgToSociety(MGPT_GGU_REQ_GUILDLIST, &kReqGuild, 0, 0, pkGMTrans->GetID());
	SetGMCommandResult(RE_IN_TRANS, "TRANS", "");
}

void SC_ServerDebugInterface::DismissGuild(uint64_t guildid)
{
	LC_ServerGuildManager::GetSingletonPtr()->GMDissmissGuild(guildid);
}

void SC_ServerDebugInterface::RandomGuildName(uint64_t iGuildID)
{
	static UT_ServerCommandParser* pkCommandMgr = (UT_ServerCommandParser*)UT_ServerCommandParser::GetSingletonPtr();
	TS_Server_GM* pkGMTrans = pkCommandMgr->GetRandomGuildNameGMTrans();
	if (NULL == pkGMTrans)
	{
		SetGMCommandResult(RE_FAIL, "FAIL", "");
		return;
	}
	LC_GuildRandomNameManager* pkMgr = LC_GuildRandomNameManager::GetSingletonPtr();
	if (!pkMgr)
	{
		SetGMCommandResult(RE_FAIL, "FAIL", "");
		return;
	}

	StringType sName = pkMgr->GetRandomName();
	if (sName.empty())
	{
		SetGMCommandResult(RE_FAIL, "FAIL", "");
		return;
	}
	MG_SS_REQ_RandomGuildName kReq;
	kReq.m_ServerID = GetGameServerID();
	kReq.m_uiGuildID = iGuildID;
	kReq.m_sRandomName = TPSTR2STDSTR(sName);

	SendMsgToSociety(MGPT_GGU_REQ_RANDOM_GUILD_NAME, &kReq, 0, 0, pkGMTrans->GetID());
	SetGMCommandResult(RE_IN_TRANS, "TRANS", "");
}

void SC_ServerDebugInterface::ActivateFashion(object_id_type playerID, uint32_t iFashionID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}
	LC_FashionBagAsset& pkFashionBag = pkPlayer->GetFashionBagAsset();
	pkFashionBag.ActivateFashion(iFashionID);
}

void SC_ServerDebugInterface::DeactivateFashion(object_id_type playerID, uint32_t iFashionID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
	{
		return;
	}
	LC_FashionBagAsset& pkFashionBag = pkPlayer->GetFashionBagAsset();
	pkFashionBag.DeactivateFashion(iFashionID);
}

void SC_ServerDebugInterface::AddBonusRole(unique_id_type charID)
{
	static GameServerManager* pkServerMgr = GetGameServerApp()->GetServerManager();
	if (!pkServerMgr->isLoaded())
	{
		SetGMCommandResult(RE_FAIL, "FAIL", "");
		return;
	}

	/*
	uint32_t startTime = (uint32_t)pkServerMgr->GetServerStartTime();
	uint32_t conflatTime = (uint32_t)pkServerMgr->GetServerConflateTime();
	uint32_t timestamp = GET_CURRENT_TIMESTAMP_IN_SECONDS();
	if (conflatTime > 0 && conflatTime + 3 * ONE_DAY_SECONDS < timestamp)
	{
		SetGMCommandResult(RE_FAIL, "开服或者合服三天内方可添加福利号!", "");
		return;
	}
	else if (conflatTime == 0 && startTime + 3 * ONE_DAY_SECONDS < timestamp)
	{
		SetGMCommandResult(RE_FAIL, "开服或者合服三天内方可添加福利号!", "");
		return;
	}
	*/
	
	GameServerApp* pkGameServerApp = GetGameServerApp();
	int ret = 0;
	std::string sKey = pkGameServerApp->GetRedisServerPredix() + PLATFORM_BONUS_ROLE;
	std::stringstream ss;
	ss << charID;
	redispp::BoolReply reply;
	UT_REDIS_CMD_WITH_REPLY(ret, pkGameServerApp->GetRedisHandler(), reply, sadd, sKey, ss.str());
	if (!reply.result())
	{
		SetGMCommandResult(RE_FAIL, "Operation failed. Benefit number already exists!", "");
		return;
	}
	pkGameServerApp->AddBonusRole(charID);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}

void SC_ServerDebugInterface::RemoveBonusRole(unique_id_type charID)
{
	GameServerApp* pkGameServerApp = GetGameServerApp();
	int ret = 0;
	std::string sKey = pkGameServerApp->GetRedisServerPredix() + PLATFORM_BONUS_ROLE;
	std::stringstream ss;
	ss << charID;
	redispp::BoolReply reply;
	UT_REDIS_CMD_WITH_REPLY(ret, pkGameServerApp->GetRedisHandler(), reply, srem, sKey, ss.str());
	if (!reply.result())
	{
		SetGMCommandResult(RE_FAIL, "Data removal failed!", "");
		return;
	}
	pkGameServerApp->RemoveBonusRole(charID);
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}

void SC_ServerDebugInterface::ResetBonus()
{
	GameServerApp* pkApp = GetGameServerApp();
	std::string sPoolKey = pkApp->GetRedisServerPredix() + PLATFORM_BONUS_POOL_KEY;
	std::string sBonus = "0";
	ASYNC_REDIS_REQUEST_NCB(pkApp->GetRedisChannel(), &redispp::Connection::set, sPoolKey, sBonus);

	std::string sRoleKey = pkApp->GetRedisServerPredix() + PLATFORM_BONUS_ROLE;
	ASYNC_REDIS_REQUEST_NCB(pkApp->GetRedisChannel(), &redispp::Connection::del, sRoleKey);
	pkApp->ClearBonusRoles();
}

void SC_ServerDebugInterface::ClearAreaKeyMonster(int32_t iStoryID, int32_t iKey, int32_t iAreaIndex)
{
	static GameStory::LC_GameStory_Manager* pkStoryMgr = SERVER_GET_GAMESTORY_MANAGER;
	if (pkStoryMgr)
	{
		pkStoryMgr->ClearAreaKeyMonster(iStoryID, (GameStoryScheduleKey)iKey, iAreaIndex);
	}
}

void SC_ServerDebugInterface::DumpScore( object_id_type playerID )
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL != pkPlayer)
	{
		char buf[256];
		StringType st = buf;

		CF_LevelUp::DataEntry* pkLevelupEntry = SafeGetCSVFileDataEntryPtr<CF_LevelUp>(pkPlayer->GetLevel());
		if (NULL == pkLevelupEntry)
		{
			return ;
		}
		uint64_t sc = pkLevelupEntry->_lCombatScore;
		sprintf_s(buf,256, "Level Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//称号战斗力
		sc = pkPlayer->GetTitleFIghtScore();
		sprintf_s(buf,256, "Title Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//坐骑战斗力
		sc = pkPlayer->GetVehicleCurrentCombatScore();
		sprintf_s(buf,256, "Vehicle Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//弩炮战斗力
		sc = pkPlayer->GetBowCurrentCombatScore();
		sprintf_s(buf,256, "Arrow Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//天眼战斗力
		sc = pkPlayer->GetDevaEyeCurrentCombatScore();
		sprintf_s(buf,256, "Eye Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//骑兵战斗力
		sc = pkPlayer->GetVehicleFightingCurrentCombatScore();
		sprintf_s(buf,256, "VehicleFight Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//神翼战斗力
		sc = pkPlayer->GetMuhunCurrentCombatScore();
		sprintf_s(buf,256, "GodWing Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//法宝战斗力
		sc = pkPlayer->GetFabaoCurrentCombatScore();
		sprintf_s(buf,256, "Fabao Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//灵宝战斗力
		sc = pkPlayer->GetSpiritPetCurrentCombatScore();
		sprintf_s(buf,256, "Spirit Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//灵武战斗力
		sc = pkPlayer->GetSpiritWeaponCurrentCombatScore();
		sprintf_s(buf,256, "Spirit Weapon Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//法器战斗力
		if (NULL != pkPlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_HIDDEN_WEAPON))
		{
			sc = LC_Helper::GetCimeliaTotalScore(pkPlayer, LC_SUB_TP_HIDDEN_WEAPON,pkPlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_HIDDEN_WEAPON)->GetStage());
			sprintf_s(buf,256, "法器战斗力：%ld", sc);
			st = buf;
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 
		}

		//天罡战斗力
		if (NULL != pkPlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_PLOUGH)->GetStage())
		{
			sc = LC_Helper::GetCimeliaTotalScore(pkPlayer, LC_SUB_TP_PLOUGH,pkPlayer->GetSubordinateAsset().GetSubData(LC_SUB_TP_PLOUGH)->GetStage());
			sprintf_s(buf,256, "天罡战斗力：%ld", sc);
			st = buf;
			GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 
		}

		//神兵
		sc = LC_Helper::GetGodWeaponTotalScore(pkPlayer);
		sprintf_s(buf,256, "GodWeapon Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO,  "%s", st.c_str()); 

		//时装
		sc = LC_Helper::GetFashionTotalScore(pkPlayer);
		sprintf_s(buf,256, "Fashion Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//炼魂战斗力
		sc = pkPlayer->GetSoulRefineCombatScore();
		sprintf_s(buf,256, "Refine Soul Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//炼化炉战斗力
		sc = pkPlayer->GetMeltCombatScore();
		sprintf_s(buf,256, "Melt Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//命骨战斗力
		sc = pkPlayer->GetBoneSoulCombatScore();
		sprintf_s(buf,256, "BoneSoul Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//技能战斗力
		sc = LC_Helper::GetSkillTotalScore(pkPlayer);
		sprintf_s(buf,256, "Skill Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//秘笈战斗力
		sc = pkPlayer->GetSkillBookAsset().GetCombatScore();
		sprintf_s(buf,256, "Skill Book Score: %ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str());

		//心法战斗力
		sc = LC_Helper::GetMentalTotalScore(pkPlayer);
		sprintf_s(buf,256, "Innser KF Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//BUFF战斗力
		sc = LC_Helper::GetBufferTotalScore(pkPlayer);
		sprintf_s(buf,256, "BUFF Score：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 

		//全服加成
		sc = LC_Helper::GetMasterBuffScore(pkPlayer);
		sprintf_s(buf,256, "MasterBliss Score ：%ld", sc);
		st = buf;
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "%s", st.c_str()); 
	}
}

void SC_ServerDebugInterface::DumpBuddyScore(object_id_type playerID, int32_t nBuddyId)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (!pkPlayer)
		return;

	BuddyInstance* pBuddy = pkPlayer->GetBuddyInstanceByID(nBuddyId);
	if ( pBuddy == NULL )
		return;
	return;
}

void SC_ServerDebugInterface::SendItemToRole(unique_id_type charID, uint32_t itemid, int itemnum, int bind, const StringType& sTitle, const StringType& sContent, const StringType& sSource, const StringType& sSerial)
{
	GfxWriteFmtLog(LOG_BILL_FILELINE, LOG_SWITCH_BILL, "SendItemToRole", charID, itemid, itemnum, bind, sTitle, sContent, sSource, sSerial);
	LC_ServerPlayerManager* playManager = SERVER_GET_PLAYER_MANAGER();
	if (playManager == NULL)
	{
		return;
	}

	char itemInfo[512] = {0};
	sprintf_s(itemInfo, 511, "%d:%d:%d;", itemid, itemnum, bind);

	StringType utfTitle = Base64Decode(sTitle);
	StringType utfContent = Base64Decode(sContent);

	LC_ServerPlayer* pkPlayer = (LC_ServerPlayer*)playManager->FindPlayer(charID);
	if (NULL == pkPlayer)
	{
		SendMailToOneUtf(charID, utfTitle, utfContent, itemInfo);
	}
	else
	{
		UT_SIMDataList kItems;
		Utility::ParseFmtString2DataIFs(itemInfo, kItems);
		if (!kItems.empty())
		{
			UT_ServerHelper::HandleBonusItems(PACK_TYPE_UNDEFINE, kItems, pkPlayer, SYS_FUNC_TP_GM, 0, utfTitle, utfContent);
		}
	}
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
}

void SC_ServerDebugInterface::Act360PriviTest(object_id_type playerID, const StringType& sUserName, int32_t index, int32_t dailygift)
{

}

void SC_ServerDebugInterface::Set360PriviGMTest(int32_t flag)
{
	GetGameServerApp()->Set360PriviGMTest(flag!=0);
}

void SC_ServerDebugInterface::GainDefenderGift(unique_id_type charID, const StringType& platform)
{

}

void SC_ServerDebugInterface::GetResourceAlertConfig()
{
	LC_ResourceAlertManager* pkAlertMgr = LC_ResourceAlertManager::GetSingletonPtr();
	if (NULL == pkAlertMgr)
	{
		SetGMCommandResult(RE_FAIL, "System error!", "");
		return;
	}
	SetGMCommandResult(RE_SUCCESS, "SUCCESS", pkAlertMgr->GetAlertConfigs());
}

void SC_ServerDebugInterface::UpdateResourceAlertConfig(const StringType& sConfig)
{
	LC_ResourceAlertManager* pkAlertMgr = LC_ResourceAlertManager::GetSingletonPtr();
	if (NULL == pkAlertMgr)
	{
		SetGMCommandResult(RE_FAIL, "System error!", "");
		return;
	}
	StringType sResult = pkAlertMgr->UpdateAlertConfig(sConfig);
	if (sResult.empty())
	{
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", "");
	}
	else
	{
		SetGMCommandResult(RE_FAIL, sResult, "");
	}
}

void SC_ServerDebugInterface::SetPreActive(object_id_type playerID, int32_t active)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
	{
		return;
	}
	GameServerApp* pkApp = GetGameServerApp();
	if (active >= 0)
	{
		int ret = 0;
		std::string sKey = pkApp->GetRedisGroupPrefix() + UT_REDIS_KEY_PREDAY_ACTIVE;
		Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
		std::string sField = boost::lexical_cast<std::string>(GetGameServerID());
		char buf[30];
		sprintf(buf, "%d%07d", GET_CURRENT_TIMESTAMP_IN_SECONDS(), active);
		std::string sValue = buf;
		UT_REDIS_CMD(ret, pkRedisHandler, hset, sKey, sField, sValue);
		if (ret == 0)
		{
			pkApp->SetPreDayActive(active);
		}
	}
	StringStreamType ss;
	ss << "服务器前一天活跃：" << pkApp->GetPreDayActive() << "，前一天三联服总活跃：" << pkApp->GetPreDayCrossActive();
	SendToPlayerConsole(playerID, ss.str());
}

void SC_ServerDebugInterface::LoadUpdateBulletin()
{
	GameServerApp* pkApp = GetGameServerApp();
	pkApp->LoadUpdateBulletin();
}

void SC_ServerDebugInterface::SendFBReward(unique_id_type charID, int32_t iType, int32_t iID)
{
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (NULL == pkPlayer)
	{
		SetGMCommandResult(7, "role doesn't online", "");
		return;
	}
	if (!pkPlayer->GetFacebookRecord().CheckCanDraw(iType, iID))
	{
		SetGMCommandResult(7, "draw times limit", "");
		return;
	}
	pkPlayer->GetFacebookRecord().DrawReward(pkPlayer, iType, iID);
}

void SC_ServerDebugInterface::DebugCommand(object_id_type playerID, const StringType& command, const StringType& params)
{
	if (command.compare("arena") == 0)
	{
		LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
		if (pkPlayer)
		{
			MG_PB<msg_define::arena_signup_req> reqMsg;
			reqMsg.m_value.set_type(LC_ARENA_TP_SOLO);
			reqMsg.m_value.set_charid(pkPlayer->GetInstance());
			//reqMsg.m_value.set_battle(pkPlayer->GetTotalScore());
			reqMsg.m_value.set_battle(pkPlayer->GetControllerCombatScore());
			char_data_define::char_lite_info linfo;
			pkPlayer->PBSaveCharLiteInfo(linfo);
			reqMsg.m_value.set_liteinfo(linfo.SerializeAsString());
			char_data_define::char_base_info binfo;
			pkPlayer->PBSaveCharBaseInfo(binfo);
			reqMsg.m_value.set_baseinfo(binfo.SerializeAsString());
			SendMsgToGlobal(MGPT_REQ_SIGNUPARENA, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
		}
	}
	else if (command.compare("test") == 0)
	{
		LC_GuildLadderManager* pkGuildLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
		if (NULL != pkGuildLadderMgr)
		{
			pkGuildLadderMgr->FillLadderMatchInfo();
		}
		static int iLogicID = 0;

		static LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
		LC_ServerMapLogic_Place* logic = NULL;
		LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
		if (NULL == pkPlayer)
		{
			return;
		}

		if (iLogicID <= 0)
		{
			logic = (LC_ServerMapLogic_Place*)pkWorldManager->CreateServerMapLogic(0, 8005, MT_EIGHTEEN_BRONZE_MAN, true);
			if (NULL == logic)
			{
				return;
			}
			iLogicID = logic->GetMapLogicID();
		}
		
		pkWorldManager->RequestChangePlayerLocation((LC_ServerPlayer*)pkPlayer,
				LMIOT_TRANSFER,
				iLogicID,
				8005, Utility::UT_Vec3Int(25569 ,44000, 1836));
	}
	else if (command.compare("test1") == 0)
	{

		LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
		if (NULL == pkPlayer)
		{
			return;
		}
		unique_id_impl kGuildUID = 1;
		static LC_GuildLadderManager* pkLadderGuildMgr = LC_GuildLadderManager::GetSingletonPtr();
		if (NULL == pkLadderGuildMgr)
		{
			return;
		}
		int32_t iTargetServer = pkLadderGuildMgr->GetTargetServerByGuildUID(kGuildUID);
		if (iTargetServer <= 0)
		{
			return;
		}
		MG_PB<msg_define::guild_ladder_ss_enter_req> kReqMsg;
		msg_define::guild_ladder_ss_enter_req& msg = kReqMsg.m_value;
		Protocol::PS_CharBaseInfoLite kLiteInfo;
		char_data_define::char_base_info kBaseInfo;
		char_data_define::char_full_info kFullInfo;
		pkPlayer->SaveCharPB(kLiteInfo, kBaseInfo, kFullInfo);
		GameServerApp* pkGameServerApp = GetGameServerApp();
		if (NULL == pkGameServerApp)
		{
			return;
		}
		Protocol::MG_PACKET_ENCODE_HANDLER& kPakcetHandler = pkGameServerApp->GetPacketHandler();
		kPakcetHandler.Clear();
		MG_OUTPUT_CDR kLiteInfoStream(kPakcetHandler.GetBuff(), SIZE_MSG_BUFF);
		if (!kLiteInfo.Compress(&kLiteInfoStream))
		{
			return;
		}
		if (!kLiteInfoStream.GoodBit())
		{
			return;
		}
		int len = kLiteInfoStream.GetMsgLength();
		msg.set_lite_info(kPakcetHandler.GetBuff(), kLiteInfoStream.GetMsgLength());
		if (!kBaseInfo.SerializeToString(msg.mutable_full_info()))
		{
			return;
		}
		if (msg.full_info().size() >= 0xC000)
		{
			static std::string _zipbuf;
			uLongf length = ::compressBound((uLong)msg.full_info().size());
			_zipbuf.resize((size_t)length);

			if (Z_OK == compress((Bytef*)(Utility::get_buff_ptr(_zipbuf)), (uLongf*)&length, (Bytef*)msg.full_info().c_str(), (uLong)msg.full_info().size()))
			{
				_zipbuf.resize(length);
				GfxWriteFmtLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "Guild Ladder SaveCharPB ZIP", pkPlayer->GetID(), pkPlayer->GetInstance(), msg.full_info().size(), length);
				kBaseInfo.set_data_flag(1);
				msg.set_full_info(_zipbuf);
			}
		}
		if (!kBaseInfo.SerializeToString(msg.mutable_base_info()))
		{
			return;
		}
		msg.set_object_id(pkPlayer->GetID());
		SendMsgToGame(iTargetServer, MGPT_G2G_GUILD_LADDER_ENTER_REQ, &kReqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstanceUniqueID(), pkPlayer->GetSession()->PeerID());
	}
	else if (command.compare("test2") == 0)
	{
		PS_ServerInfo kSvrInfo;
		kSvrInfo.m_iType = SERVER_TYPE_GAME;
		kSvrInfo.m_iServerID = 9999;
		kSvrInfo.m_szIP = "127.0.0.1";
		kSvrInfo.m_iPort = 1010;
		NW_ServerSession* pkServerSession = GetGameServerApp()->RegisterGameServer(kSvrInfo);
		if (pkServerSession)
		{
			pkServerSession->TryToConnect();
		}
		LC_GuildLadderManager* pkGuildLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
		if (NULL != pkGuildLadderMgr)
		{
			if (pkGuildLadderMgr->GetMatchSize() <= 0)
			{
				pkGuildLadderMgr->FillLadderMatchInfo();
			}			
		}
	}
	else if (command.compare("test3") == 0)
	{
		LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
		if (NULL == pkPlayer)
		{
			return;
		}
		//检查是否可以复活
// 		if (!pkPlayer->GetDead())
// 		{
// 			return;
// 		}
		int		lTargetMapResID = 0;
		UT_Vec3Int	kTargetLocation = pkPlayer->GetCurrentLocation();

		//切换到合适的位置
		lTargetMapResID = pkPlayer->GetMapResID();
		UT_ServerHelper::GetRandomLocationXYZ(kTargetLocation,lTargetMapResID,pkPlayer->GetCitizenship());
	
		LC_ServerWorldManager* pkServerWorldManager = SERVER_GET_WORLD_MANAGER();
		{
			pkServerWorldManager->RequestChangePlayerLocation(pkPlayer,
				LMIOT_SAFE_RESPAWN,
				pkPlayer->GetMapLogicID(),
				lTargetMapResID,
				kTargetLocation);
		}

	}
	else if (command.compare("signup") == 0)
	{
		LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
		if (NULL == pkPlayer)
		{
			return;
		}

		LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
		if (pkGuildManager == NULL)
		{
			return;
		}
		LC_ServerGuildInfo* guild = pkGuildManager->GetGuildInfoByID(pkPlayer->GetGuildID());
		if (guild == NULL)
		{
			return;
		}
		LC_GuildLadderManager* pkLadderMgr = LC_GuildLadderManager::GetSingletonPtr();
		if (NULL == pkLadderMgr)
		{
			return;
		}
		ResultType ret = RE_SUCCESS;
		do {
			if (UT_ServerHelper::IsInCross(pkPlayer))
			{
				ret = RE_ON_CROSS_SERVER;
				break;
			}
			if (guild->GetLevel() < pkLadderMgr->GetSignUpLevelLimit())
			{
				ret = RE_GUILD_LEVEL_NOTENOUGH;
				break;
			}
			if (!pkLadderMgr->IsInSignUpTime())
			{
				ret = RE_GUILD_LADDER_NOT_IN_SIGNUP_TIME;
				break;
			}
			if (!guild->IsElder(pkPlayer->GetInstance()) && !guild->IsViceMaster(pkPlayer->GetInstance()) &&
				!guild->IsMaster(pkPlayer->GetInstance()))
			{
				ret = RE_GUILD_LADDER_AUTH_NOENOUGH;
				break;
			}
			mem::vector<PS_CostEntry> entry;
			PS_CostEntry temp;
			temp.m_ItemType = 0;
			temp.m_ItemCount = pkLadderMgr->GetSignUpNeedMoney();
			entry.push_back(temp);
			ret = guild->CheckCost(entry);
			if (ret != RE_SUCCESS)
			{
				break;
			}
		} while (0);
		if (RE_SUCCESS != ret)
		{
			MG_PB<msg_define::guild_ladder_signup_resp> respMsg;
			msg_define::guild_ladder_signup_resp& resp = respMsg.m_value;
			resp.set_result(ret);
			pkPlayer->SendMsgToClient(MGPT_RLT_GUILD_LADDER_SIGNUP, &respMsg);
			return;
		}
		MG_PB<msg_define::guild_ladder_signup_req> reqMsg;
		msg_define::guild_ladder_signup_req& req = reqMsg.m_value;
		req.set_uid(guild->GetUid());
		req.set_name(TPSTR2STDSTR(guild->GetName()));
		req.set_level(guild->GetLevel());
		req.set_battle(guild->GetActiveBattle());
		SendMsgToGlobal(MGPT_REQ_GUILD_LADDER_SIGNUP, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}

}

struct Sub {
	Sub() : type(-1), level(-1) {}
	StringType HandleAsset(LC_ServerPlayer* pkPlayer)
	{
		if (!IS_VALID_SUBORDINATE_TYPE(type))
		{
			return "type_invalid";
		}

		LC_SubordinateAsset& kSubAsset = pkPlayer->GetSubordinateAsset();
		if (level >= 0)
		{
			kSubAsset.GMSetLevel(type, level, true);
		}
		return "ok";
	}
	int32_t type;
	int32_t level;
};
AJSON(Sub, (type)(level));

struct Item {
	Item() : pack_type(PACK_TYPE_UNDEFINE), pack_index(-1)
		   , type_id(0), reinforce(-1), dst_id(0) {}

	StringType HandleAsset(LC_ServerPlayer* pkPlayer)
	{
		if (NULL == pkPlayer)
		{
			return "player_invalid";
		}
		LC_PackAsset* pkPackAsset = pkPlayer->GetPackAsset();
		if(NULL == pkPackAsset)
		{
			return "pack_invalid";
		}
		LC_BackPackEntry* pkItemEntry = pkPackAsset->GetBackPackEntry(pack_type, pack_index);
		if (NULL == pkItemEntry || !pkItemEntry->GetBackPackValueByType(LPSFET_VALID))
		{
			return "pack_invalid";
		}
		if (type_id != pkItemEntry->GetBackPackValueByType(LPSFET_TYPE_ID))
		{
			return "id_mismatch";
		}

		LC_ItemFactoryBase* pkItemFactory = LC_ItemFactoryBase::GetSingletonPtr();
		LC_ItemBase* pkItemData = pkItemFactory->RequestItem(pkItemEntry->GetBackPackValueByType(LPSFET_TYPE_ID));
		if (NULL == pkItemData)
		{
			return "item_invalid";
		}
		if (reinforce >= 0)
		{
			if (!pkItemData->CanReinforce())
			{
				return "can_not_reinforce";
			}
		}
		if (dst_id > 0)
		{
			if (!IS_EQUIP_ID(dst_id))
			{
				return "dst_id_invalid";
			}
			LC_PackEntryFactory* pkPackEntryFactory = LC_PackEntryFactory::GetSingletonPtr();
			if(NULL == pkPackEntryFactory)
			{
				return "pack_factory_invalid";
			}

			PackOperateInfoList kItemOPs;
			PackOperateInfo objData;
			objData.Reset();
			objData.m_iOPType = OPT_DELETE_ITEM_BY_INDEX;
			objData.m_bOPEntry = false;
			objData.m_iPackType  = pack_type;
			objData.m_iIndex   = pack_index;
			objData.m_lCount   = pkItemEntry->GetBackPackValueByType(LPSFET_OVERLAP_COUNT);
			kItemOPs.push_back(objData);

			LC_BackPackEntryList kEntryList;
			pkPackEntryFactory->CreateItemEntry(dst_id, 1, CIET_NONE, kEntryList, -1, 0, 0, true);
			if (kEntryList.empty())
			{
				return "create_fail";
			}
			objData.Reset();
			objData.m_iOPType = OPT_ADD_ITEM;
			objData.m_bOPEntry = true;
			objData.m_bMerger  = false;
			objData.m_kPackEntry.Clone(kEntryList[0]);
			kItemOPs.push_back(objData);

			if (RE_SUCCESS != LC_Helper::DoItemOPs(pkPlayer, kItemOPs, SYS_FUNC_TP_HANDLE_ASSET, type_id))
			{
				return "item_op_fail";
			}
			LC_OperationPackParam kNewEquipSlot;
			LC_BackPackEntry* pkNewEquipEntry = NULL;
			PackOPRecordSet* record_set = pkPackAsset->GetRecordSet() ;
			for(PackOPRecordSet::PackOPRecords::iterator it = record_set->m_Add_Items.begin(); it != record_set->m_Add_Items.end(); ++it)
			{
				PackEntryOPRecord& rd = *it;
				if (rd.m_Entry && IS_EQUIP_ID(rd.m_Entry->GetBackPackValueByType(LPSFET_TYPE_ID)))
				{
					kNewEquipSlot._type = rd.m_lPackType;
					kNewEquipSlot._index = rd.m_lPackIndex;
					pkNewEquipEntry = pkPackAsset->GetBackPackEntry(kNewEquipSlot._type, kNewEquipSlot._index);
					break;
				}
			}
		}
		return "ok";
	}

	int32_t pack_type;
	int32_t pack_index;
	int32_t type_id;
	int32_t reinforce;
	int32_t dst_id;
};
AJSON(Item, (pack_type)(pack_index)(type_id)(reinforce)(dst_id));

struct PlayerAsset {
	PlayerAsset() : level(0) {}

	ResultType HandleAsset(LC_ServerPlayer* pkPlayer, StringType& msg)
	{
		if (NULL == pkPlayer)
		{
			msg = "role invalid";
			return RE_FAIL;
		}
		if (level > 0)
		{
			pkPlayer->SetLevel(level);
			oss_role_lvup(pkPlayer, SYS_FUNC_TP_HANDLE_ASSET);
			oss_360_log_user_upgrade(pkPlayer, SYS_FUNC_TP_HANDLE_ASSET);
		}
		//StringType result;
		StringStreamType ss;
		ss << "{\"subs\":[";
		size_t iSize = subs.size();
		for (size_t i = 0; i < iSize; i++)
		{
			if (i != 0)
			{
				ss << ',';
			}
			Sub& kSub = subs.at(i);
			ss << "\"" << kSub.HandleAsset(pkPlayer) << "\"";
		}
		ss << "],\"items\":[";
		iSize = items.size();
		for (size_t i = 0; i < iSize; i++)
		{
			if (i != 0)
			{
				ss << ',';
			}
			Item& kItem = items.at(i);
			ss << "\"" << kItem.HandleAsset(pkPlayer) << "\"";
		}
		ss << "]}";
		msg = ss.str();
		return RE_SUCCESS;
	}

	int32_t level;
	std::string mail_title;
	std::string mail_content;
	mem::vector<Sub> subs;
	mem::vector<Item> items;
};
AJSON(PlayerAsset, (level)(mail_title)(mail_content)(subs)(items));

void SC_ServerDebugInterface::HandleAsset(unique_id_type charID, bool bSetResult)
{
	LC_ServerPlayer* pkPlayer = _findPlayerByUID(charID);
	if (NULL == pkPlayer)
	{
		if (bSetResult)
		{
			SetGMCommandResult(RE_SUCCESS, "player offline", "{}");
		}
		return;
	}
	GameServerApp* pkApp = GetGameServerApp();
	if (NULL == pkApp)
	{
		if (bSetResult)
		{
			SetGMCommandResult(RE_FAIL, "system error", "{}");
		}
		return;
	}

	Utility::Redis_Handler* pkRedisHandler = pkApp->GetRedisHandler();
	if (NULL == pkRedisHandler)
	{
		if (bSetResult)
		{
			SetGMCommandResult(RE_FAIL, "db error", "{}");
		}
		return;
	}

	ResultType rslt = RE_SUCCESS;
	StringType msg = "SUCCESS", sMailTitle, sMailContent;
	int ret = 0;
	redispp::StringReply kReply;
	std::string sKey = pkApp->GetRedisServerPredix() + UT_REDIS_KEY_HANDLE_ASSET;
	std::string sField = boost::lexical_cast<std::string>(charID);
	UT_REDIS_CMD_WITH_REPLY(ret, pkRedisHandler, kReply, hget, sKey, sField);
	if (0 == ret && kReply.result())
	{
		do {
			std::string sReply = kReply;
			std::string errorMsg;
			PlayerAsset kAsset;
			if (!boost::ajson::load_from_buff(kAsset, sReply.c_str(), errorMsg))
			{
				rslt = RE_FAIL;
				msg = "json parse failed";
				break;
			}
			UT_REDIS_CMD(ret, pkRedisHandler, hdel, sKey, sField);
			if (0 != ret)
			{
				rslt = RE_FAIL;
				msg = "del failed, for data safe, do nothing";
				break;
			}
			rslt = kAsset.HandleAsset(pkPlayer, msg);
			if (RE_SUCCESS != rslt)
			{
				break;
			}
			sMailTitle = STDSTR2TPSTR(kAsset.mail_title);
			sMailContent = STDSTR2TPSTR(kAsset.mail_content);
		} while (0);
	}
	if (RE_SUCCESS != rslt)
	{
		if (bSetResult)
		{
			SetGMCommandResult(rslt, msg, "{}");
		}
		return;
	}

	if (bSetResult)
	{
		SetGMCommandResult(RE_SUCCESS, "SUCCESS", msg);
	}

	if (!sMailTitle.empty() && !sMailContent.empty())
	{
		LC_BackPackEntryList kEntryList;
		UT_ServerHelper::SendToMail(sMailTitle, sMailContent, charID, pkPlayer->GetOwnerCharName(), 0, kEntryList, "GM");
	}
}

void SC_ServerDebugInterface::BoneSoulTrain(object_id_type lPlayerID, int32_t iType)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		bool bRandHit = false;
		pkPlayer->GetBoneSoulAsset().BoneSoulLevelUpOper(iType, true, bRandHit);
	}
}

void SC_ServerDebugInterface::AddMonopolyCount(object_id_type lPlayerID, int32_t iCount)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL != pkPlayer)
	{
		static ACT_ScheduleDataManager* pkACTDataMgr = ACT_ScheduleDataManager::GetSingletonPtr();
		ACT_ScheduleDataActivityBase* pkActivity = pkACTDataMgr->GetActivity(ACT_SCHEDULE_TP_MONOPOLY);
		Activity::ACT_MonopolyManager* pkActiMonopoly = dynamic_cast<Activity::ACT_MonopolyManager*>(pkActivity);
		if (NULL == pkActiMonopoly)
		{
			return;
		}
		pkActiMonopoly->AddMonopolyCount(pkPlayer, iCount);
	}
}

bool SC_ServerDebugInterface::SetRankTransferFailed(object_id_type lPlayerID, int32_t mapResID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	MG_RLT_SetRankTransferState rltMsg;
	rltMsg.m_nMapResID = mapResID;
	rltMsg.m_nResult = pkPlayer->SetRankTransferFailed(mapResID);
	pkPlayer->SendMsgToClient(MGPT_RLT_SETRANKTRANSFERSTATE, &rltMsg);
	return true;
}

bool SC_ServerDebugInterface::AddRankRedisData(object_id_type lPlayerID, int32_t storyID, int32_t value)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	int rankIndex = pkPlayer->UpdatePlayerDamageData(storyID, value, PlayerDamageUpdateType_GreaterUpdate);
	pkPlayer->DisableCombatRecorder(rankIndex, 10, 10);
	return true;
}

bool SC_ServerDebugInterface::ClearRankRedisData(object_id_type lPlayerID, int32_t storyID)
{
	mem::vector<uint64_t> params;
	params.push_back(storyID);

	UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_BATTLE_INFO, params);
	UT_ServerHelper::RedisClearData_DamageString(UT_REDIS_KEY_DAMAGE_RANK, params);
	return true;
}

bool SC_ServerDebugInterface::GetRankRedisData(object_id_type lPlayerID, int32_t storyID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	int32_t index;
	uint64_t nScore = 0;

	mem::vector<uint64_t> params;
	params.push_back(storyID);

	UT_ServerHelper::RedisRank_ReadDamage(UT_REDIS_KEY_DAMAGE_RANK, params, pkPlayer->GetInstance(), nScore, index);
	return true;
}

bool SC_ServerDebugInterface::TransferMap(object_id_type lPlayerID, int32_t mapResID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if(NULL == pkPlayer)
	{
		return false;
	}

	MG_RLT_Transfer rltMsg;
	rltMsg.m_lPlayerID = pkPlayer->GetInstance();
	rltMsg.m_lMapResID = mapResID;
	rltMsg.m_iLineNum = 0;
	rltMsg.m_iServerID = GetGameServerID();
	rltMsg.m_lMapLogicID = pkPlayer->GetMapLogicID();	
	rltMsg.m_iResult = pkPlayer->TransferMap(TT_TRANSPORT_RAID_MAP, mapResID, mapResID, GetServerID(), false, rltMsg.m_iLineNum);
	//pkPlayer->SendMsgToClient(MGPT_RLT_TRANSFER, &rltMsg);
	return true;
}

bool SC_ServerDebugInterface::ReqPlayerDetail(object_id_type lPlayerID, unique_id_type findPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer->GetPlayerDetailInfo(findPlayerID, 0);
	return true;
}

bool SC_ServerDebugInterface::CommitAnswer(object_id_type lPlayerID, int topicIndex, int choice)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	MG_RLT_CommitAnswer rltMsg;
	rltMsg.m_nTopicIndex = topicIndex;
	rltMsg.m_bResult = pkPlayer->CommitAnswer(topicIndex, choice);
	pkPlayer->SendMsgToClient(MGPT_RLT_COMMITANSWER, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::GetAnswerRankInfo(object_id_type lPlayerID, int32_t minIndex, int maxIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	MG_RLT_AnswerRankInfo rltMsg;
	rltMsg.m_nResult = pkPlayer->GetAnswerRankInfo(minIndex, maxIndex, rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_ANSWERRANKINFO, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::GetRankSubBInfo(object_id_type lPlayerID, int type, int param, int minIndex, int maxIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	MG_RLT_GetRankSubBInfo rltMsg;
	rltMsg.m_nType = type;
	rltMsg.m_nID = param;
	rltMsg.m_nResult = pkPlayer->GetActivityRankInfo(type, param, minIndex, maxIndex, rltMsg);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETRANKSUBBINFO, &rltMsg);

	return true;

}

bool SC_ServerDebugInterface::UpgradeCommanderTitle(object_id_type lPlayerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	MG_RLT_UpgradeCommanderTitle rltMsg;
	rltMsg.m_nResult = pkPlayer->UpgradeCommanderTitle();
	pkPlayer->SendMsgToClient(MGPT_RLT_UPGRADECOMMANDERTITLE, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::GetYiZhongQinXiDamageData(object_id_type lPlayerID, int storyID, uint64_t charID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	UT_ServerHelper::GetPlayerDamageData(pkPlayer, storyID, charID);
	return true;
}

bool SC_ServerDebugInterface::GetYiZhongQinXiDetailDamageData(object_id_type lPlayerID, int storyID, uint32_t time, uint64_t charID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	UT_ServerHelper::GetDetailDamageData(pkPlayer, storyID, time, charID);
	return true;
}

bool SC_ServerDebugInterface::GetChapterRaidStarReward(object_id_type playerID, int32_t chapterID, int32_t rewardIndex)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	MG_RLT_GetChapterRaidStarReward rltMsg;
	rltMsg.m_nChapterID = chapterID;
	rltMsg.m_nRewardIndex = rewardIndex;
	rltMsg.m_nResult = pkPlayer->GetChapterRaidStarReward(chapterID, rewardIndex);
	pkPlayer->SendMsgToClient(MGPT_RLT_GETCHAPTERRAIDSTARREWARD, &rltMsg);

	return true;
}

bool SC_ServerDebugInterface::QuickRaidNew(object_id_type playerID, int32_t mapid, int32_t raidTimes)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
	{
		return false;
	}

	pkPlayer->GetQuickRaidAsset().QuickRaid_New(pkPlayer, mapid, raidTimes);
	return true;
}

bool SC_ServerDebugInterface::TestEquipGradeLevel(object_id_type playerID, int32_t nBuddyId)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return false;

	

	object_id_type lPlayerID = playerID;

	BuddyInstance* pInstance = pkPlayer->GetBuddyInstanceByID(nBuddyId);
	MGPT_RLT_EquipSlotLv equipGradeMsg;
	// 检查 英雄装备 所有槽位是否已具备升阶条件
	// 检查英雄等级是否允许 进阶
	if(pInstance->CheckEquipUpgradeConfig())
	{
		pInstance->EquipUpgrade();
		equipGradeMsg.m_nResult = RE_EQUIP_UPGRADE_SUCCESS;
		pkPlayer->SendMsgToClient(MGPT_REQ_EQUIP_SLOT_LV, &equipGradeMsg);
	}
	else
	{
		equipGradeMsg.m_nResult = RE_EQUIP_UPGRADE_FAIL;
		pkPlayer->SendMsgToClient(MGPT_REQ_EQUIP_SLOT_LV, &equipGradeMsg);
	}


	return true;
}

bool SC_ServerDebugInterface::ControllerEquipSlotReinforce(object_id_type playerID, int32_t slotIndex)
{
	return true;
}

bool SC_ServerDebugInterface::ControllerEquipSlotRefine(object_id_type playerID, int32_t slotIndex)
{
	return true;
}

bool SC_ServerDebugInterface::EquipItemInPackage(object_id_type playerID, int32_t srcType, int32_t srcIndex, int32_t destType, int32_t destIndex, int32_t itemID)
{
	return true;
}

bool SC_ServerDebugInterface::UnEquipItemInEquipSlot(object_id_type playerID, int32_t srcType, int32_t srcIndex, int32_t itemID)
{
	return true;
}

bool SC_ServerDebugInterface::GetOtherOfflineInfo(object_id_type playerID, uint64_t uCharID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return false;

	pkPlayer->GetOtherOfflineInfo(OfflinePlayerFromDBType::OPFDT_PK,uCharID);


	return true;
}

bool SC_ServerDebugInterface::TestGuildFight(object_id_type playerID,uint8_t uType, int32_t nStoryId,uint32_t uPoint)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return false;
	std::string sGuildName = pkPlayer->GetGuildName().c_str();
	std::string sPlayerName = pkPlayer->GetOwnerCharName().c_str();
	if(uType ==  GuildFightRankType::GFRT_PERSONAL)
		UT_ServerHelper::CommitPersonalPointByStageType(nStoryId,pkPlayer->GetInstance(),uPoint,sGuildName,sPlayerName,pkPlayer->GetCommanderTitle());
	else if (uType ==  GuildFightRankType::GFRT_GUILD)
		UT_ServerHelper::CommitGuildPointByStageType(nStoryId,pkPlayer->GetGuildID(), uPoint,sGuildName);
	else if (uType == 2)
	{
		if(uPoint == 0)
		{
			if(nStoryId){
				mem::vector<uint64_t> allIds;
				UT_ServerHelper::GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(allIds, GuildFightRankType::GFRT_PERSONAL,nStoryId);
				std::vector<int> vecId;
				UT_ServerHelper::ReqRankToDbSociety(GameToDbSocietyRankType::GTDSRT_RankClear,allIds,vecId,nStoryId,0 ,0);
				UT_ServerHelper::ClearGuildFightRedisInfo(nStoryId);
			}
			else if(nStoryId == 0)
			{
				for(uint32_t i = 1; i <= 30; ++i)
				{
					mem::vector<uint64_t> allIds;
					UT_ServerHelper::GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(allIds, GuildFightRankType::GFRT_PERSONAL,i);
					std::vector<int> vecId;
					UT_ServerHelper::ReqRankToDbSociety(GameToDbSocietyRankType::GTDSRT_RankClear,allIds,vecId,0,0 ,0);
					UT_ServerHelper::ClearGuildFightRedisInfo(i);
				}

			}
		}
		else
		{
			if(nStoryId){
				mem::vector<uint64_t> allIds;
				allIds.push_back(playerID);
				//UT_ServerHelper::GetAllIdVectorFromRedisRankPonitNameByTypeAndStoryId(allIds, GuildFightRankType::GFRT_PERSONAL,nStoryId);
				std::vector<int> vecId;
				UT_ServerHelper::ReqRankToDbSociety(GameToDbSocietyRankType::GTDSRT_RankClear,allIds,vecId,nStoryId,0 ,0);
				UT_ServerHelper::ClearGuildFightRedisInfo(nStoryId);
			}
			else if(nStoryId == 0)
			{
				mem::vector<uint64_t> allIds;
				allIds.push_back(playerID);
				std::vector<int> vecId;
				for(uint32_t i = 1; i <= 30; ++i)
				{
					UT_ServerHelper::ClearGuildFightRedisInfo(i);
				}
				UT_ServerHelper::ReqRankToDbSociety(GameToDbSocietyRankType::GTDSRT_RankClear,allIds,vecId,0,0 ,0);

			}
		}
		
	}	
	else if (uType == 3)
		UT_ServerHelper::SendGuildFightPersonalRewardByStoryId(nStoryId);
	else if (uType == 4)
		UT_ServerHelper::SendSocietyGetMemberToGuildRankReward(nStoryId);	
	else if (uType == 5)
	{
		MG_RLT_GuildFightRankInfo rltMsg;
		MG_REQ_GuildFightRankInfo reqMsg;
		reqMsg.m_nMin = 0;
		reqMsg.m_nMax = 999;
		reqMsg.m_nType1 = GuildFightRankType::GFRT_PERSONAL;
		reqMsg.m_nStroyId = nStoryId;
		reqMsg.m_nStageInfo.push_back(0);
		reqMsg.m_nStageInfo.push_back(231);
		reqMsg.m_nStageInfo.push_back(232);
		reqMsg.m_nStageInfo.push_back(233);
		reqMsg.m_nStageInfo.push_back(234);
		rltMsg.m_nResult = pkPlayer->GetGuildFightRankInfo(reqMsg, rltMsg);
	}
	else if (uType == 6)
	{
		MG_RLT_GuildFightRankInfo rltMsg;
		MG_REQ_GuildFightRankInfo reqMsg;
		reqMsg.m_nMin = 0;
		reqMsg.m_nMax = 999;
		reqMsg.m_nType1 = GuildFightRankType::GFRT_GUILD;
		reqMsg.m_nStroyId = nStoryId;
		reqMsg.m_nStageInfo.push_back(0);
		reqMsg.m_nStageInfo.push_back(231);
		reqMsg.m_nStageInfo.push_back(232);
		reqMsg.m_nStageInfo.push_back(233);
		reqMsg.m_nStageInfo.push_back(234);
		rltMsg.m_nResult = pkPlayer->GetGuildFightRankInfo(reqMsg, rltMsg);
	}
	else if(uType == 7)
	{
		UT_ServerHelper::UpdateCacheRankOpen();
	}
	else if(uType == 8)
	{
		SingleFightRecord record;
		PS_Buddy buddyInfo;
		pkPlayer->GetAssignedBuddyInstance()->FillBuddyCSInfo(buddyInfo.m_nBuddyData);
		buddyInfo.m_nBuddyIndex = 1;
		buddyInfo.m_nDamageData = 1;
		buddyInfo.m_nHurtData = 1;
		buddyInfo.m_nHealData = 1;

		record.m_nTime = GET_CURRENT_TIMESTAMP_IN_SECONDS();
		record.m_AttackInfo.m_CommanderBaseInfo.m_nCharName = "123";
		record.m_AttackInfo.m_CommanderBaseInfo.m_nControllerLevel = 1;
		record.m_AttackInfo.m_CommanderBaseInfo.m_nBornType = 1;
		//record.m_AttackInfo.m_CommanderBaseInfo.m_nCharName = 1;
		record.m_AttackInfo.m_CommanderBaseInfo.m_nCommanderTitleFrameID = 1;
		record.m_AttackInfo.m_nVecBuddyInfo.push_back(buddyInfo);
		record.m_AttackInfo.m_nVecBuddyInfo.push_back(buddyInfo);

		record.m_DefenceInfo.m_CommanderBaseInfo.m_nCharName = "123";
		record.m_DefenceInfo.m_CommanderBaseInfo.m_nControllerLevel = 1;
		record.m_DefenceInfo.m_CommanderBaseInfo.m_nBornType = 1;
		//record.m_DefenceInfo.m_CommanderBaseInfo.m_nCharName = 1;
		record.m_DefenceInfo.m_CommanderBaseInfo.m_nCommanderTitleFrameID = 1;
		record.m_DefenceInfo.m_nVecBuddyInfo.push_back(buddyInfo);
		record.m_DefenceInfo.m_nVecBuddyInfo.push_back(buddyInfo);
		MG_SS_AddGuildDefendLog msg;
		msg.m_serverID = GSNodeNetwork::Instance().getEntryIdByMergeId(GetGameServerID());
		msg.m_charID = playerID;
		msg.m_logRecord = record;
		SendMsgToSociety(MGPT_SS_ADDGUILDDEFENDLOG, &msg);
		UT_ServerHelper::BroadcastBeFightedToPlayer(pkPlayer->GetInstance());
	}
	else if(uType == 10)
	{
		UT_ServerHelper::ClearRedisData_By_EntryServerID(UT_REDIS_KEY_RANK_START_END_TIME);
	}
	return true;
}

bool  SC_ServerDebugInterface::TestRobScore(object_id_type playerID,uint8_t uType,int nWonNum)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return false;
	LC_ServerPlayerGuild* pPlayerGuild = pkPlayer->GetPlayerGuildRobInfo();
	if(1 == uType)// 购买次数
	{
		MG_Resp_GuildFightRobScore_Opt msg;
		msg.m_uiResult = pPlayerGuild->BuyRobNum(pkPlayer,1);
		pPlayerGuild->PackGuildResMsg(msg);
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_FIGHT_ROB_SCORE, &msg);
	}
	else if(2 == uType) // 领奖
	{
		MG_Resp_GuildFightRobScoreReceive msg;
		msg.m_uiResult = pPlayerGuild->ReceiveRewardByWonNum(pkPlayer,nWonNum);;
		pPlayerGuild->PackGuildRobScoreReceivedInfo(msg.m_ReceivedInfo);
		pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_FIGHT_ROB_SCORE_RECEIVE, &msg);
	}
	else if(3 == uType) // 扣道具次数
	{
		pPlayerGuild->ReduceRobScoreChallengeNumAndItem(pkPlayer);
	}
	else if(4 == uType) // 设置已经 赢的次数
	{
		pPlayerGuild->UpdateRobNum();
	}
	return true;
}

bool SC_ServerDebugInterface::TestGuildMemberPointInfo(object_id_type playerID)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return false;
	uint64_t _nGuild = pkPlayer->GetGuildID();
	LC_ServerGuildManager* pkGuildManager = LC_ServerGuildManager::GetSingletonPtr();
	LC_ServerGuildInfo* pkGuildInfo = pkGuildManager->GetGuildInfoByID(_nGuild);
	if(NULL == pkGuildInfo)
		return false;
	//公会成员信息
	Guild_Member_Map* guildMemberInfos = pkGuildInfo->GetAllMemberInfo();
	
	MG_Resp_GuildFightGuildMemberIdInfo respMsg;
	for(Guild_Member_Map::iterator it = guildMemberInfos->begin(); it!=guildMemberInfos->end(); ++it)
	{
		uint64_t uCharID = it->first;
		respMsg.m_IdVec.push_back(uCharID);
	}
	pkPlayer->SendMsgToClient(MGPT_RESP_GUILD_FIGHT_GUILD_MEMBER_ID_INFO, &respMsg);


	MG_GG_REQ_OfflinePlayerFromDB reqMsg;
	reqMsg.m_eType = 2;
	for(int i = 0; i < respMsg.m_IdVec.size(); ++i)
		reqMsg.m_CharIDs.push_back(respMsg.m_IdVec[i]);
	SendMsgToGameDB(MGPT_GG_REQ_OFFLINE_PLAYER_FROM_DB, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());

	return true;
}

bool SC_ServerDebugInterface::TestCombatScore(object_id_type playerID, uint64_t buddyID, int32_t type, const StringType& value)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return false;

	char *ptr;
	uint64_t score = _strtoui64(TPSTR2STDSTR(value).c_str(), &ptr, 10);

	LC_ActorAttributeMap attrMap;
	attrMap.Clear();
	
	attrMap.SetAttributeValueFunc(ATT_MAX_HP, 100000);
	attrMap.RecomputeAll();
	if(1 == type)
	{
		BuddyInstance* pBuddy = pkPlayer->GetBuddyInstanceByID(buddyID);
		if(NULL != pBuddy)
		{
			pBuddy->SyncBuddyAttributes(type, attrMap, false, score);
		}
	}
	else if(1<type && type<8)
	{
		pkPlayer->SyncCommanderAttributeToClient(type, attrMap, false, score);
	}
	
	else if(type == 17)
	{
		LC_RankManager* rank_Manager = SERVER_GET_RANK_MANAGER();

		MG_GG_REQ_Query_User_Challenge reqMsg;
		reqMsg.m_nRobotSize = 0;
		reqMsg.m_nRandnum = GetGlobalSetting.RandSoluDulePlayerNum;
		reqMsg.m_nRankK = GetGlobalSetting.RankSoluDuleK;
		rank_Manager->FillPlayerData(pkPlayer, reqMsg.m_UserInfo);
		SendMsgToGroup(MGPT_SS_RANK_QUERY_USER_CHALLENGE_RANK_REQ, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}
	else if(type == 18)
	{
		MG_RESP_GetPlayerInfo rltMsg;
		rltMsg.m_CharID = buddyID;
		static LC_ServerPlayerManager* pkPlayManager = SERVER_GET_PLAYER_MANAGER();
		LC_ServerPlayer* pkDestPlayer = (LC_ServerPlayer*)pkPlayManager->FindPlayer(rltMsg.m_CharID);
		if(NULL != pkDestPlayer)
		{
			pkDestPlayer->FillPlayerDetailInfo(rltMsg.m_nUserInfo);
			rltMsg.m_nUserInfo.m_nControllerCombatScore = score;
			pkPlayer->SendMsgToClient(MGPT_RESP_PLAYER_DETAIL, &rltMsg);
		}		
	}
	
	return true;
}


bool SC_ServerDebugInterface::AddAuction(object_id_type playerID, int32_t count)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(playerID);
	if (NULL == pkPlayer)
		return false;

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (NULL == pkPlayer)
		return false;
	
	for(int i = 0; i < count; i++)
	{
		MG_GG_REQ_Auction_Sell kReqAuctionSell;

		int32_t iSequence = LC_SequenceManager::Instance().GetSequenceID();
		kReqAuctionSell.m_iSellCount = GetGlobalSetting.nServerMaxSellCount;
		kReqAuctionSell.m_iServerMaxCount = GetGlobalSetting.nServerMaxSellCount;

		kReqAuctionSell.m_kAuctionSell.m_kPrices = 1;
		kReqAuctionSell.m_kAuctionSell.m_kPriceType =4;
		kReqAuctionSell.m_kAuctionSell.m_kItemEntry.SetItemBaseData_TypeID(11390605);
		kReqAuctionSell.m_kAuctionSell.m_kItemEntry.SetItemBaseData_OverlapCount(1);
		kReqAuctionSell.m_kAuctionSell.m_kItemEntry.SetItemBaseData_Valid(1);
		kReqAuctionSell.m_kAuctionSell.m_uItemId = 11390605;
		kReqAuctionSell.m_kAuctionSell.m_uItenmCount = 1;
		kReqAuctionSell.m_kAuctionSell.m_uiTime = GetGlobalSetting.nAuctionSellTime;
		TPSTR2STDSTR(pkPlayer->GetOwnerCharName(), kReqAuctionSell.m_kAuctionSell.m_szCharName);
		SendMsgToTrade(MGPT_SS_REQ_AUCTION_SELL, &kReqAuctionSell, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	}

	
	return true;
}


bool SC_ServerDebugInterface::SetSkyArenaScore(object_id_type lPlayerID, int nCount)
{
	LC_ServerPlayer* pkPlayer = _findPlayer(lPlayerID);
	if (pkPlayer == NULL)
	{
		return false;
	}

	pkPlayer = pkPlayer->GetOwnerPlayer();
	if (pkPlayer == NULL)
	{
		return false;
	}
	MG_Req_G2G_SummitArenaCheckChallenge reqMsg;
	reqMsg.m_nChallengeId = nCount;
	SendMsgToGroup(MGPT_SS_GM_SETSKYARENASCORE, &reqMsg, pkPlayer->GetUserID(), pkPlayer->GetInstance());
	
	return true;
}
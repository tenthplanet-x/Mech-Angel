#include "LC_ServerMapLogic_SummitWheelPlay.h"
#include "LM_LocaleManager.h"

#include "GameServerApp.h"
#include "LC_ServerWorldManager.h"
#include "UT_ServerHelper.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerNPC.h"
#include "LC_ServerMapLogicFactory.h"
#include "LC_ServerMapLogic_Place.h"

#include "LC_RankManager.h"
#include "CF_RankReward.h"
#include "LC_Dule_Manager.h"
#include "oss_define.h"
#include "SM_StateBase.h"
#include "../Buddy/BuddyInstance.h"
#include "UT_OperateLog.h"
#include "gsnode.h"
#include "MG_GroupProtocol.h"
using namespace OPERATELOG;
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;


LC_ServerMapLogic_SummitWheelPlay::LC_ServerMapLogic_SummitWheelPlay():
m_Config(NULL),m_State(SummitWheel_State_None),m_nEntryPlayerNum(0),
m_ChallengePlayerID(INVALID_ACTOR_ID),m_BeChallengeCharID(0),
m_ChallengePlayer(NULL),m_BeChallengePlayer(NULL),m_TimeStamp(0.0f),
m_solofightuniqueId(0),m_mapID(0),m_CurrChallengeTeamId(0),m_CurrBeChallengeTeamId(0),
m_uCurrIndex(0),m_bChallengeWind(false),m_uniqueID(0)
{
	
}
LC_ServerMapLogic_SummitWheelPlay::~LC_ServerMapLogic_SummitWheelPlay()
{
	//UnInit();
}
bool LC_ServerMapLogic_SummitWheelPlay::Init(LC_ServerMapLogic_Place* place,LC_Dule_Config* config)
{
	if (place==NULL || config==NULL)
		return false;
	if ( false == LC_ServerMapLogic_PlayBase::Init(place,PlayType_SummitWheel))
		return false;
	m_nEntryPlayerNum = 0;
	m_Config=config;
	m_ChallengePlayer=NULL;
	m_BeChallengePlayer=NULL;
	m_TimeStamp=0.0f;
	
	m_vecResult.clear();
	m_State=SummitWheel_State_None;
	m_mapID  = SUMMIT_ARENA_SOLO_DULE_RES_ID;
	return true;
}

void LC_ServerMapLogic_SummitWheelPlay::UnInit()
{
	End();
	// relase
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;
	IF_A_VALID_PLAYER(m_BeChallengePlayer)
	{
		LC_ServerPlayerManager* pkPlayerManager = SERVER_GET_PLAYER_MANAGER();
		m_BeChallengePlayer->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
		for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
		{
			LC_ServerPlayer* pBuddy = vecBuddyPtrPlayer[i];
			if (pBuddy == NULL)
				continue;
			pkPlayerManager->DestroyPlayer(pBuddy);
		}
	}
	m_nEntryPlayerNum = 0;
	m_ChallengePlayer=NULL;
	m_BeChallengePlayer=NULL;
	m_TimeStamp=0;
	
	m_State=SummitWheel_State_None;
	m_Config=NULL;
	LC_ServerMapLogic_PlayBase::UnInit();
}

bool LC_ServerMapLogic_SummitWheelPlay::Start()
{
	if (m_State!=SummitWheel_State_Inited && m_State!=SummitWheel_State_End)
	{
		return false;
	}
	SetWheelNextState(SummitWheel_State_Prepare);
	return true;
}
void LC_ServerMapLogic_SummitWheelPlay::ClearBuddyOp(LC_ServerPlayer* pPlayer)
{
	IF_A_NA_PLAYER(pPlayer)
		return;
	float fCurrentTime = GET_PROCESS_TIME;
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;	
	pPlayer->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
	for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
	{
		LC_ServerPlayer* pBuddy = vecBuddyPtrPlayer[i];
		if (pBuddy == NULL)
			continue;
		pBuddy->TryInterruptSkill( -1, fCurrentTime, SKI_FORCE);
		pBuddy->TryFinishSkill( -1, fCurrentTime, pBuddy->GetCurrentLocation().x, pBuddy->GetCurrentLocation().y );
		pBuddy->RefreshCurrentOPIndex( i, false );
		pBuddy->GetHatredList()->Reset();
		pBuddy->GetHatredList()->Disable();
	}
}
void LC_ServerMapLogic_SummitWheelPlay::SyncFightReportToGroup()
{

}

void LC_ServerMapLogic_SummitWheelPlay::updateNullTeamIDFailed()
{
	for (int i = 0; i < m_vecResult.size(); ++i )
	{
		if( m_vecResult[i].uIndex > m_uCurrIndex )
		{
			int challengeTeamID = m_vecResult[i].uChallengeTeamId;
			int beChallengeTeamID = m_vecResult[i].uBeChallengeTeamId;
			mem::vector<int> challengeBuddyIDVec;
			mem::vector<int> beChallengeBuddyIDVec;
			m_ChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_ChallengePlayer, m_mapID,challengeTeamID, SchemeType_Offensive, challengeBuddyIDVec);
			m_BeChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_BeChallengePlayer, m_mapID,beChallengeTeamID, SchemeType_Defence, beChallengeBuddyIDVec);
			if(challengeBuddyIDVec.size() == 0 && beChallengeBuddyIDVec.size() == 0)
				continue;
			if(challengeBuddyIDVec.size() == 0)
			{
				m_BeChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_BeChallengePlayer, m_mapID, beChallengeTeamID, SchemeType_Defence, m_vecResult[i].vecBeChallengeReliveBuddyID);
			}
			else if(beChallengeBuddyIDVec.size() == 0)
			{
				m_vecResult[i].bChallenge = true;
				m_ChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_ChallengePlayer, m_mapID, challengeTeamID, SchemeType_Offensive, m_vecResult[i].vecChallengeReliveBuddyID);
			}
		}
	}
}

bool LC_ServerMapLogic_SummitWheelPlay::CheckCanNextTeamBattle()
{
	for (int i = 0; i < m_vecResult.size(); ++i )
	{
		if( m_vecResult[i].uIndex > m_uCurrIndex )
		{
			if(m_vecResult[i].uChallengeTeamId != 0 
			&& m_vecResult[i].uBeChallengeTeamId != 0 )
			{
				int challengeTeamID = m_vecResult[i].uChallengeTeamId;
				int beChallengeTeamID = m_vecResult[i].uBeChallengeTeamId;
				mem::vector<int> challengeBuddyIDVec;
				mem::vector<int> beChallengeBuddyIDVec;
				m_ChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_ChallengePlayer, m_mapID,challengeTeamID, SchemeType_Offensive, challengeBuddyIDVec);
				m_BeChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_BeChallengePlayer, m_mapID,beChallengeTeamID, SchemeType_Defence, beChallengeBuddyIDVec);
				if(challengeBuddyIDVec.size() == 0)
				{
					if(beChallengeBuddyIDVec.size() != 0)
						updateRemainTeamFailed(m_uCurrIndex +1 ,true);
					return false;
				}
				if(beChallengeBuddyIDVec.size() == 0)
				{
					if(challengeBuddyIDVec.size() != 0)
						updateRemainTeamFailed(m_uCurrIndex +1,false);
					return false;
				}
				return true;
			}
		}
	}
	return false;
}


void LC_ServerMapLogic_SummitWheelPlay::updateRemainTeamFailed(int currIndex,bool bChallengeFailed)
{
	for (int i = 0; i < m_vecResult.size(); ++i )
	{
		if( m_vecResult[i].uIndex < currIndex )
			continue;
		m_vecResult[i].bIsFightValid =false;
		m_vecResult[i].nFightId = GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
		int challengeTeamID = m_vecResult[i].uChallengeTeamId;
		int beChallengeTeamID = m_vecResult[i].uBeChallengeTeamId;
		mem::vector<int> challengeBuddyIDVec;
		mem::vector<int> beChallengeBuddyIDVec;
		m_ChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_ChallengePlayer, m_mapID,challengeTeamID, SchemeType_Offensive, challengeBuddyIDVec);
		m_BeChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_BeChallengePlayer, m_mapID,beChallengeTeamID, SchemeType_Defence, beChallengeBuddyIDVec);
		if(challengeBuddyIDVec.size() == 0 && beChallengeBuddyIDVec.size() == 0)
			continue;
		if(bChallengeFailed)
		{
			m_BeChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_BeChallengePlayer, m_mapID, beChallengeTeamID, SchemeType_Defence, m_vecResult[i].vecBeChallengeReliveBuddyID);
		}
		else
		{
			m_vecResult[i].bChallenge = true;
			m_ChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_ChallengePlayer, m_mapID, challengeTeamID, SchemeType_Offensive, m_vecResult[i].vecChallengeReliveBuddyID);
		}
	}
}

bool LC_ServerMapLogic_SummitWheelPlay::PrepareNextTeamBattle()
{
	if(false == CheckChallengeBeChallengePtr())
		return false;
	if(false == CheckCanNextTeamBattle())
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  PrepareNextTeamBattle CheckCanNextTeamBattle false ---------- m_uCurrIndex:%d m_uniqueID : %lld \n", m_uCurrIndex,m_uniqueID);
		//  战斗结束 ,准备结算
		updateNullTeamIDFailed();
		SetWheelNextState(SummitWheel_State_BattleEnd);
		return false;
	}
	m_uCurrIndex++;
	InitTeamFightId(m_uCurrIndex);
	m_ChallengePlayer->SwitchBuddyScheme(m_mapID, ++m_CurrChallengeTeamId, SchemeType_Offensive, true, true);
	m_BeChallengePlayer->SwitchBuddyScheme(m_mapID, ++m_CurrBeChallengeTeamId, SchemeType_Defence, true, true);
	InitAllBuddyInfo();
	ChallengePlayerActionForbid(m_ChallengePlayer);
	ChallengePlayerActionForbid(m_BeChallengePlayer);
	return true;
}

void LC_ServerMapLogic_SummitWheelPlay::End()
{
	if (!CheckRun())
		return;
	// 停止战斗行为
	if(false == CheckChallengeBeChallengePtr())
		return;
	
	ChallengePlayerActionUnForbid(m_ChallengePlayer);
	ChallengePlayerActionUnForbid(m_BeChallengePlayer);
	// 恢复SM_Controller
	mem::vector<int32_t> vecBuddyIDsA;
	m_ChallengePlayer->GetActivateBuddyIDs( vecBuddyIDsA);
	m_ChallengePlayer->SetAIControl(false);
	m_ChallengePlayer->SetDenyMove(false);
	for (int i = 0; i < vecBuddyIDsA.size(); i++)
	{
		LC_ServerPlayer* pBuddy = m_ChallengePlayer->GetActivateBuddyByID(vecBuddyIDsA[i]);
		if (pBuddy == NULL)
			continue;
		pBuddy->SetSMController( StateMachine::SCT_SERVER_BUDDY );
	}

	//删除对方
	GetPlace()->AddActor2Faction(m_BeChallengePlayer, FACTION_EVIL);
	m_BeChallengePlayer->GetActorSkillList()->Reset();
	m_BeChallengePlayer->SetSoloDulePlay(false);
	
	GetPlace()->AddActor2Faction(m_ChallengePlayer,FACTION_PLAYER);
	m_ChallengePlayer->GetActorSkillList()->Reset();
	m_ChallengePlayer->SetSoloDulePlay(false);

	SetWheelNextState(SummitWheel_State_End);
}
void LC_ServerMapLogic_SummitWheelPlay::SetWheelNextState(SummitWheel_State eState)
{
	GET_PROCESS_TICK;
	if( m_State >= SummitWheel_State_WaitReward && eState <= SummitWheel_State_WaitReward)
		return;
	

	if(eState ==SummitWheel_State_LoadPlayerInfo)
	{
		m_TimeStamp=GET_PROCESS_TIME;
	}
	else if(eState ==SummitWheel_State_ShowAllInfo)
	{
		int ShowAllInfoTime = 0;//m_Config->ShowAllInfoTime;
		m_TimeStamp=GET_PROCESS_TIME+ShowAllInfoTime;
	}
	else if(eState == SummitWheel_State_PrepareNextTeam)
	{
		m_TimeStamp=GET_PROCESS_TIME+1;
	}
	else if(eState == SummitWheel_State_ShowTeamInfo)
	{
		int ShowTeamInfoTime = m_Config->ShowTeamInfoTime;
		m_TimeStamp=GET_PROCESS_TIME+ShowTeamInfoTime;
	}
	else if(eState == SummitWheel_State_Battle)
	{
		int BattleTime = m_Config->BattleTime;
		m_TimeStamp=GET_PROCESS_TIME+BattleTime;
	}
	else if(eState == SummitWheel_State_BattleEnd)
	{
		m_TimeStamp=GET_PROCESS_TIME;
	}
	else if(eState == SummitWheel_State_WaitReward)
	{
		m_TimeStamp= GET_PROCESS_TIME+10 ;
	}
	else if(eState == SummitWheel_State_End)
	{
		m_TimeStamp=GET_PROCESS_TIME;
	}
	m_State =  eState;
}
bool LC_ServerMapLogic_SummitWheelPlay::Update(float fCurrentTime, float fDeltaTime)
{
	if(m_TimeStamp == 0.0f || m_TimeStamp > fCurrentTime || m_State >= SummitWheel_State_End)
		return true;
	if(m_State ==SummitWheel_State_LoadPlayerInfo)
	{
		// SummonPlayerFinish
		if( m_nEntryPlayerNum == 2 )
		{
			InitAllBuddyInfo();
			//
			SetWheelNextState(SummitWheel_State_ShowAllInfo);
			// 展示全部信息
			//NotifyShowAllInfoTime();
			m_nEntryPlayerNum = 0;
		}
	}
	else if(m_State == SummitWheel_State_ShowAllInfo)
	{
		NotifyShowTeamInfoTime();
		SetWheelNextState(SummitWheel_State_ShowTeamInfo);
	}
	else if(m_State ==SummitWheel_State_PrepareNextTeam)
	{
		// 
		// 打斗结束 发送战报 给group 切换下一队
		if(PrepareNextTeamBattle())
		{
			NotifyShowTeamInfoTime();
			SetWheelNextState(SummitWheel_State_ShowTeamInfo);
		}
	}
	else if(m_State == SummitWheel_State_ShowTeamInfo)
	{
		Battle();
		SetWheelNextState(SummitWheel_State_Battle);
	}
	else if(m_State == SummitWheel_State_Battle)
	{
		// 一轮战斗结束 时间到
		BattleEnd(0,NULL,true);
	}
	else if(m_State == SummitWheel_State_BattleEnd)
	{
		// 判定 本场 胜负
		Settle();
		// 发奖 
		SendReward();
		
	}
	else if(m_State == SummitWheel_State_WaitReward )
	{
		//等待group 10秒后，发奖消息还未收到，定时器补发 假发奖消息退出
		//TODO
		SendErrorReward();
	}
	return true;
}

ResultType LC_ServerMapLogic_SummitWheelPlay::CheckEnter(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player)
		return RE_FAIL;
	return RE_SUCCESS;
}

Utility::UT_Vec3Int	LC_ServerMapLogic_SummitWheelPlay::ComputeEnterLocation(LC_ServerPlayer* player,const Utility::UT_Vec3Int& targetlocation)
{
	return m_Config->Factions[0].Position;
}

void LC_ServerMapLogic_SummitWheelPlay::EventNotify(MapLogicEvent& kEvent)
{
	
}

void LC_ServerMapLogic_SummitWheelPlay::PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent)
{
	//IF_A_NA_PLAYER(player)
	//	return ;
	//if ( kEvent.Type() == PlayerEvent_Enter)
	//{
	//	GetPlace()->AddActor2Faction(player, FACTION_NEUTRAL);
	//	ChallengePlayerActionForbid(player);
	//	if(false == InitTeamInfo(true))
	//	{
	//		//TODO
	//		//初始化失败，退出
	//		return ;
	//	}
	//	// 准备拉取玩家
	//	//Prepare();
	//	m_nEntryPlayerNum++;
	//	SetWheelNextState(SummitWheel_State_LoadPlayerInfo);
	//	
	//	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  PlayerEvent_Enter  m_nEntryPlayerNum %d  ---------- c_uid:%d\n", player->GetUserID(),m_nEntryPlayerNum);

	//}
	//else if (kEvent.Type()==PlayerEvent_Leave || kEvent.Type() ==PlayerEvent_Death)
	//{
	//	// 死亡 事件 在设置血量为0 或负数之前。 所以这里不能立即复活加个状态位跳转 延迟 1秒左右复活
	//	BattleEnd(kEvent.Type(),player);
	//	
	//}
	if (kEvent.Type()==PlayerEvent_Leave || kEvent.Type() ==PlayerEvent_Death)
	{
		// 死亡 事件 在设置血量为0 或负数之前。 所以这里不能立即复活加个状态位跳转 延迟 1秒左右复活
		BattleEnd(kEvent.Type(),player);
		
	}
}
void LC_ServerMapLogic_SummitWheelPlay::SetRoundBattleResult(LC_ServerPlayer* player,bool timeOut)
{
	object_id_type nDeadId;
	if(timeOut)
	{
		UpdateTeamDeadInfo(m_uCurrIndex,m_BeChallengeCharID);
		return;
	}
	bool bOwnerDead = CheckOwnerDead(player, nDeadId);
	// No Controller dead.
	if (!bOwnerDead)
		return;
	if (nDeadId == m_ChallengePlayerID)
	{
		UpdateTeamDeadInfo(m_uCurrIndex,m_BeChallengeCharID);
		/*GetPlace()->SetFactionResult(FACTION_PLAYER,FACTION_RESULT_FAIL);
		GetPlace()->SetFactionResult(FACTION_EVIL,FACTION_RESULT_SUCESS);*/
	}
	else
	{
		UpdateTeamDeadInfo(m_uCurrIndex,player->GetInstance(),true);
		/*GetPlace()->SetFactionResult(FACTION_PLAYER,FACTION_RESULT_SUCESS);
		GetPlace()->SetFactionResult(FACTION_EVIL,FACTION_RESULT_FAIL);*/
	}
};
bool LC_ServerMapLogic_SummitWheelPlay::CheckOwnerDead( LC_ServerPlayer* pPlayer, object_id_type& nDeadPlayerID )
{
	bool bResult = false;
	bResult = pPlayer->CheckOwnerDead();
	if (bResult)
		nDeadPlayerID = pPlayer->GetOwnerPlayerId();
	return bResult;
}

bool LC_ServerMapLogic_SummitWheelPlay::CheckEnd()
{
	return m_State == SummitWheel_State_End;
}

bool LC_ServerMapLogic_SummitWheelPlay::CheckRun()
{
	if (	m_State == SummitWheel_State_Prepare 
		||	m_State == SummitWheel_State_LoadPlayerInfo
		||	m_State == SummitWheel_State_ShowAllInfo 
		||	m_State == SummitWheel_State_ShowTeamInfo 
		||	m_State == SummitWheel_State_PrepareNextTeam
		||	m_State == SummitWheel_State_Battle
		||	m_State == SummitWheel_State_BattleEnd
		||	m_State == SummitWheel_State_WaitReward)
	{
		return true;
	}
	return false;
}
void LC_ServerMapLogic_SummitWheelPlay::Settle(bool bLeave)
{
	// 判定结果
	if(bLeave)
	{
		m_bChallengeWind = false;
		IF_A_NA_PLAYER(m_BeChallengePlayer)
			return;
		updateRemainTeamFailed(m_uCurrIndex -1,true);
		/*for (int i = 0; i < m_vecResult.size(); ++i )
		{
			if( m_vecResult[i].uIndex < m_uCurrIndex)
				continue;

			m_BeChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_BeChallengePlayer, m_mapID, m_vecResult[i].uBeChallengeTeamId, SchemeType_Defence, m_vecResult[i].vecBeChallengeReliveBuddyID);
		}*/
		return;
	}
	int iChallengeWinNum = 0;
	int iBeChallengeWinNum = 0;
	for (int i = 0; i < m_vecResult.size(); ++i )
	{
		int challengeTeamID = m_vecResult[i].uChallengeTeamId;
		int beChallengeTeamID = m_vecResult[i].uBeChallengeTeamId;
		mem::vector<int> challengeBuddyIDVec;
		mem::vector<int> beChallengeBuddyIDVec;
		m_ChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_ChallengePlayer, m_mapID,challengeTeamID, SchemeType_Offensive, challengeBuddyIDVec);
		m_BeChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_BeChallengePlayer, m_mapID,beChallengeTeamID, SchemeType_Defence, beChallengeBuddyIDVec);
		if(challengeBuddyIDVec.size() == 0 && beChallengeBuddyIDVec.size() == 0)		
			 continue;
		if( m_vecResult[i].bChallenge )
			iChallengeWinNum ++;
		else
			iBeChallengeWinNum++;
	}
	if( iChallengeWinNum > iBeChallengeWinNum)
		m_bChallengeWind = true;
}
void LC_ServerMapLogic_SummitWheelPlay::SendReward()
{
	SetWheelNextState(SummitWheel_State_WaitReward);
	//结算
	if(false == CheckChallengeBeChallengePtr())
		return;
	UpLoadSoloDulePlayInfoToGroup(m_ChallengePlayer,m_BeChallengePlayer);
}

void LC_ServerMapLogic_SummitWheelPlay::ChallengePlayerActionForbid(LC_ServerPlayer* pPlayer)
{
	LC_ActionForbid* pa = pPlayer->GetActionForbid();
	pa->AddActionForbid(LAFT_GROUP,LAF_GROUP_MAX_COUNT);
	pa->AddActionForbid(LAFT_ITEM,	LAF_ITEM_MAX_COUNT);
	pa->AddActionForbid(LAFT_SKILL,LAF_SKILL_SKILL_ID,SKILL_RETURN_CITY);
	pa->AddActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT);
	pa->AddActionForbid(LAFT_GAME,LAF_GAME_PLAYER_LEVEL_UP);
	pa->AddActionForbid(LAFT_GAME,LAF_GAME_SKILL_LEVEL_UP);
	pa->AddActionForbid(LAFT_EQUIP,LAF_EQUIP_MAX_COUNT);
	pa->AddActionForbid(LAFT_AUCTION,LAF_AUCTION_MAX_COUNT);
	pa->AddActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT);
	pa->AddActionForbid(LAFT_GUILD,LAF_GUILD_MAX_COUNT);
	pPlayer->SetDenyMove(true);
	
	

	pPlayer->GetStateMap()->RemoveSkillStateInStateMap(GOD_STATE_ID);
	
	pPlayer->SetAIControl(true);
	pPlayer->RefreshSMController();
	pPlayer->GenSkillRandList();

	pPlayer->SetCurrentBuddy(0);
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayer;	
	pPlayer->GetAllBuddyPtrPlayer( vecBuddyPtrPlayer);
	for (int i = 0; i < vecBuddyPtrPlayer.size(); i++)
	{
		LC_ServerPlayer* pBuddy = vecBuddyPtrPlayer[i];
		if (pBuddy == NULL)
			continue;
		pBuddy->SetHP(pBuddy->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP));
		pBuddy->SetDead(false);
		pBuddy->RefreshCurrentOPIndex( 0 );
	}
}

void LC_ServerMapLogic_SummitWheelPlay::ChallengePlayerActionUnForbid(LC_ServerPlayer* pPlayer)
{
	LC_ActionForbid* pa = pPlayer->GetActionForbid();
	pa->RemoveActionForbid(LAFT_GROUP,LAF_GROUP_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_ITEM,	LAF_ITEM_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_SKILL,LAF_SKILL_SKILL_ID,SKILL_RETURN_CITY);
	pa->RemoveActionForbid(LAFT_TRADE,LAF_TRADE_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_GAME,LAF_GAME_PLAYER_LEVEL_UP);
	pa->RemoveActionForbid(LAFT_GAME,LAF_GAME_SKILL_LEVEL_UP);
	pa->RemoveActionForbid(LAFT_EQUIP,LAF_EQUIP_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_AUCTION,LAF_AUCTION_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_MAIL,LAF_MAIL_MAX_COUNT);
	pa->RemoveActionForbid(LAFT_GUILD,LAF_GUILD_MAX_COUNT);

	
	pPlayer->GetStateMap()->StartSavedSkillState();
	pPlayer->SyncSkillStatesToClient(SSC_ALL);
}
void LC_ServerMapLogic_SummitWheelPlay::ReservaDuleInfo(const unique_id_impl challengeCharID,const unique_id_impl bechallengeCharID,uint64_t uniqueID)
{
	m_ChallengeCharID=challengeCharID;
	m_BeChallengeCharID=bechallengeCharID;
	m_uniqueID = uniqueID;
	m_solofightuniqueId =GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
	SetWheelNextState(SummitWheel_State_Inited);
	GetPlace()->SetActive(true);
	Start();
	Prepare();
	
	//IF_A_VALID_PLAYER(challengePlayer)
	//		GetPlace()->NotifyGameEventToClient(challengePlayer,EVENT_TYPE_FIGHT_RECORD_START,m_solofightuniqueId,challengePlayer->GetUserID(),1 );
}

void LC_ServerMapLogic_SummitWheelPlay::Prepare()
{
	LC_ServerPlayer *challengeplayer=GetPlace()->SpawnPlayer(m_ChallengeCharID, m_Config->Factions[0].Position, PLY_DEF_CLONE_BITS, NULL, LC_ServerMapLogic_SummitWheelPlay::SummonPlayerFinish, FACTION_NEUTRAL);
	IF_A_NA_PLAYER(challengeplayer)
	{
		//delete  InitClientTranscribeFightInfo
		uint32_t userId = m_ChallengeCharID.detail();
		if(IS_SOLODULE_ROBOT_PLAYERID(m_ChallengeCharID) )
			 userId = m_ChallengeCharID;
		GameLogic::LC_Helper::deleteFightTranscribePtr(userId);
		return;
	}
	m_ChallengePlayerID = challengeplayer->GetID();
	m_ChallengePlayer=challengeplayer;
	m_ChallengePlayer->GetBuddySchemeMgr().SetBeChallangeMapID(m_mapID);
	m_ChallengePlayer->SetSoloDulePlay(true);
	GameLogic::LC_Helper::InitClientTranscribeFightInfo(GetType(),challengeplayer->GetUserID(),m_solofightuniqueId,1);
	if(IS_SOLODULE_ROBOT_PLAYERID(m_ChallengeCharID) )
	{
		//是机器人，创建数据
		challengeplayer->InitRobotLogicData(m_ChallengeCharID);
	}

	LC_ServerPlayer *bechallengeplayer=GetPlace()->SpawnPlayer(m_BeChallengeCharID, m_Config->Factions[1].Position, PLY_DEF_CLONE_BITS, NULL, LC_ServerMapLogic_SummitWheelPlay::SummonPlayerFinish, FACTION_NEUTRAL);
	IF_A_NA_PLAYER(bechallengeplayer)
	{
		uint32_t userId = m_BeChallengeCharID.detail();
		if(IS_SOLODULE_ROBOT_PLAYERID(m_BeChallengeCharID) )
			userId = m_BeChallengeCharID;
		GameLogic::LC_Helper::deleteFightTranscribePtr(userId);
		return;
	}
	
	
	m_BeChallengePlayer=bechallengeplayer;

	m_BeChallengePlayer->GetBuddySchemeMgr().SetBeChallangeMapID(m_mapID);
	m_BeChallengePlayer->SetSoloDulePlay(true);
	
	
	if(IS_SOLODULE_ROBOT_PLAYERID(m_BeChallengeCharID) )
	{
		//是机器人，创建数据
		bechallengeplayer->InitRobotLogicData(m_BeChallengeCharID);
	}
	return;
}

void LC_ServerMapLogic_SummitWheelPlay::InitAllBuddyInfo()
{
	if(false == CheckChallengeBeChallengePtr())
		return;
	m_ChallengePlayer->GetSkillCoolDownMap()->ClearAllSkillCDNoGod();
	m_BeChallengePlayer->GetSkillCoolDownMap()->ClearAllSkillCD();
	UT_Vec2Float rkDir;
	rkDir.x = Int2Float(m_BeChallengePlayer->GetCurrentLocation().x - m_ChallengePlayer->GetCurrentLocation().x);
	rkDir.y = Int2Float(m_BeChallengePlayer->GetCurrentLocation().y - m_ChallengePlayer->GetCurrentLocation().y);
	rkDir.Unitize();

	Utility::UT_Vec3Int challengePos = m_Config->Factions[0].Position;
	Utility::UT_Vec3Int beChallengePos = m_Config->Factions[1].Position;
	
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayerA;	
	m_ChallengePlayer->GetAllBuddyPtrPlayer( vecBuddyPtrPlayerA);
	for (int i = 0; i < vecBuddyPtrPlayerA.size(); i++)
	{
		LC_ServerPlayer* pBuddy = vecBuddyPtrPlayerA[i];
		if (pBuddy == NULL)
			continue;
		if(i == 0)
		{
			pBuddy->SetCurrentLocation(challengePos);
			pBuddy->SetTargetLocation(challengePos);
		}
		else
		{
			Utility::UT_Vec3Int pos = challengePos;
			pBuddy->ModifyOffsetLocationByIndex(i, pos);
			pBuddy->SetCurrentLocation(pos);
			pBuddy->SetTargetLocation(pos);
		}
		pBuddy->SetForwardDirAngle(135);
		pBuddy->SetAIControlNotify(true);
		pBuddy->SyncEnterFov();
	}

	rkDir.x=0-rkDir.x;
	rkDir.y=0-rkDir.y;
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayerB;	
	m_BeChallengePlayer->GetAllBuddyPtrPlayer( vecBuddyPtrPlayerB);
	for (int i = 0; i < vecBuddyPtrPlayerB.size(); i++)
	{
		LC_ServerPlayer* pBuddy = vecBuddyPtrPlayerB[i];
		if (pBuddy == NULL)
			continue;
		if(i == 0)
		{
			pBuddy->SetCurrentLocation(beChallengePos);
			pBuddy->SetTargetLocation(beChallengePos);
		}
		else
		{
			Utility::UT_Vec3Int pos = beChallengePos;
			pBuddy->ModifyOffsetLocationByIndex(i, pos);
			pBuddy->SetCurrentLocation(pos);
			pBuddy->SetTargetLocation(pos);
		}
		pBuddy->SetForwardDirAngle(315);
		pBuddy->SetAIControlNotify(true);
		pBuddy->SyncEnterFov();
	}
}

void LC_ServerMapLogic_SummitWheelPlay::PackSkyRoundTeamDeadInfo(MG_NOTIFY_SkyRoundShowInfo& notifyMsg)
{
	notifyMsg.m_LeftDeadTeamIdVec.clear();
	notifyMsg.m_RightDeadTeamIdVec.clear();
	for (int i = 0; i <m_vecResult.size(); ++i)
	{
		SingleSummitWheelFightInfo &teamInfo = m_vecResult[i];
		TeamShowInfo leftInfo;
		leftInfo.m_nTeamID = teamInfo.uChallengeTeamId;
		leftInfo.m_nState = 1;
		
		TeamShowInfo rightInfo;
		rightInfo.m_nTeamID = teamInfo.uBeChallengeTeamId;
		rightInfo.m_nState = 1;
		
		if(teamInfo.uWindPlayerId > 0)
		{
			leftInfo.m_nState = teamInfo.bChallenge;
			rightInfo.m_nState = !teamInfo.bChallenge;
		}
		notifyMsg.m_LeftDeadTeamIdVec.push_back(leftInfo);
		notifyMsg.m_RightDeadTeamIdVec.push_back(rightInfo);
	}
}
void LC_ServerMapLogic_SummitWheelPlay::UpdateBuddyDeadInfo(LC_ServerPlayer* pPlayer,mem::vector <int>& vecReliveBuddyID )
{
	vecReliveBuddyID.clear();
	IF_A_NA_PLAYER(pPlayer)
		return ;
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayerB;	
	pPlayer->GetAllBuddyPtrPlayer( vecBuddyPtrPlayerB);
	for (int i = 0; i < vecBuddyPtrPlayerB.size(); i++)
	{
		LC_ServerPlayer*pCheckPlayer = vecBuddyPtrPlayerB[i];
		IF_A_NA_PLAYER(pCheckPlayer)
			continue;
		if( pCheckPlayer->GetDead())
			continue;
		vecReliveBuddyID.push_back(pCheckPlayer->GetBuddyID());
	}
}
void LC_ServerMapLogic_SummitWheelPlay::UpdateTeamDeadInfo(int nCurrIndex,unique_id_impl uWindPlayerId,bool bChallenge )
{
	for (int i = 0; i <m_vecResult.size(); ++i)
	{
		SingleSummitWheelFightInfo &teamInfo = m_vecResult[i];
		if(nCurrIndex == teamInfo.uIndex)
		{
			teamInfo.bChallenge = bChallenge;
			teamInfo.uWindPlayerId = uWindPlayerId;
			UpdateBuddyDeadInfo(m_ChallengePlayer, teamInfo.vecChallengeReliveBuddyID);
			UpdateBuddyDeadInfo(m_BeChallengePlayer, teamInfo.vecBeChallengeReliveBuddyID);
			IF_A_VALID_PLAYER(m_ChallengePlayer)
				GetPlace()->NotifyGameEventToClient(m_ChallengePlayer,EVENT_TYPE_FIGHT_RECORD_END,teamInfo.nFightId,m_ChallengePlayer->GetUserID());
			GameLogic::LC_Helper::FinshTranscribeFightInfo(m_ChallengeCharID.detail(),teamInfo.nFightId,false);
			return;
		}
	}
}
void LC_ServerMapLogic_SummitWheelPlay::InitTeamFightId(int nCurrIndex)
{
	for (int i = 0; i <m_vecResult.size(); ++i)
	{
		SingleSummitWheelFightInfo &teamInfo = m_vecResult[i];
		if(nCurrIndex == teamInfo.uIndex)
		{
			if( nCurrIndex == 1)
				teamInfo.nFightId = m_solofightuniqueId;
			else
			{
				teamInfo.nFightId =GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
				GameLogic::LC_Helper::InitClientTranscribeFightInfo(GetType(),m_ChallengeCharID.detail(),teamInfo.nFightId,nCurrIndex);
				IF_A_VALID_PLAYER(m_ChallengePlayer)
					GetPlace()->NotifyGameEventToClient(m_ChallengePlayer,EVENT_TYPE_FIGHT_RECORD_START,teamInfo.nFightId,m_ChallengePlayer->GetUserID(),nCurrIndex );
			}
			return;
		}
	}
}

void LC_ServerMapLogic_SummitWheelPlay::NotifyShowAllInfoTime()
{
	if(false == CheckChallengeBeChallengePtr())
		return;
	int ShowAllInfoTime = m_Config->ShowAllInfoTime;
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  NotifyShowAllInfoTime  ---------- m_uCurrIndex:%d m_uniqueID: %lld \n", m_uCurrIndex,m_uniqueID);
	MG_NOTIFY_SkyAllShowInfo notifyMsg;
	notifyMsg.m_Time = ShowAllInfoTime;
	m_ChallengePlayer->FillSingleShowInfoByMapId(notifyMsg.m_Left,m_mapID,SchemeType_Offensive);
	m_BeChallengePlayer->FillSingleShowInfoByMapId(notifyMsg.m_Right,m_mapID,SchemeType_Defence);
	m_ChallengePlayer->SendMsgToClient(MGPT_SC_SKY_ALL_SHOW_INFO, &notifyMsg);	
}
void LC_ServerMapLogic_SummitWheelPlay::NotifyShowTeamInfoTime()
{
	if(false == CheckChallengeBeChallengePtr())
		return;
	int ShowTeamInfoTime = m_Config->ShowTeamInfoTime;
	MG_NOTIFY_SkyRoundShowInfo notifyMsg;
	notifyMsg.m_Time = ShowTeamInfoTime;
	notifyMsg.nRound = m_uCurrIndex;
	m_ChallengePlayer->FillPS_SchemeBuddyDataByMapId(notifyMsg.m_Left,m_mapID,SchemeType_Offensive,m_CurrChallengeTeamId);
	m_BeChallengePlayer->FillPS_SchemeBuddyDataByMapId(notifyMsg.m_Right,m_mapID,SchemeType_Defence,m_CurrBeChallengeTeamId);
	PackSkyRoundTeamDeadInfo(notifyMsg);
	m_ChallengePlayer->SendMsgToClient(MGPT_SC_SKY_ROUND_SHOW_INFO, &notifyMsg);	
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  NotifyShowTeamInfoTime  ---------- m_uCurrIndex:%d m_uniqueID:%lld \n", m_uCurrIndex,m_uniqueID);
}
void LC_ServerMapLogic_SummitWheelPlay::PlayerLevave(int opType)
{
	if(opType == QSMOT_None)
	{
		End();
	}
	else if (opType == QSMOT_ReqSettle)
	{
		// 判定 本场 胜负
		Settle(true);
		// 发奖 
		SendReward();
	}
	
}
void LC_ServerMapLogic_SummitWheelPlay::BattleEnd(int endType,LC_ServerPlayer* pPlayer,bool timeOut)
{
	if(false == CheckChallengeBeChallengePtr())
		return;
	// 停止 双方的打斗
	ClearBuddyOp(m_ChallengePlayer);
	ClearBuddyOp(m_BeChallengePlayer);
	LC_ServerMapLogic_Place* pPlace = GetPlace();
	if(pPlace)
	{
		mem::set<int> setA, setB;
		pPlace->ClearAllNPC(setA, setB);
	}
	if(timeOut)
	{
		SetRoundBattleResult(m_ChallengePlayer,timeOut);
		SyncFightReportToGroup();
		SetWheelNextState(SummitWheel_State_PrepareNextTeam);
		return;
	}
	if (endType==PlayerEvent_Leave)
	{
		if(m_State == SummitWheel_State_PrepareNextTeam || m_State == SummitWheel_State_BattleEnd)
			return;
		// 系统内调用的离开不做处理 玩家手动点离开 在协议处 处理
	}
	else if (endType ==PlayerEvent_Death)
	{
		SetRoundBattleResult(pPlayer);
		SyncFightReportToGroup();
		SetWheelNextState(SummitWheel_State_PrepareNextTeam);
	}
}


void LC_ServerMapLogic_SummitWheelPlay::Battle()
{
	if(false == CheckChallengeBeChallengePtr())
		return;
	const unique_id_impl& challengePlayerInstanceID = m_ChallengePlayer->GetInstance();
	const unique_id_impl& beChallengePlayerInstanceID = m_BeChallengePlayer->GetInstance();
	GetPlace()->AddActor2Faction(m_ChallengePlayer,FACTION_PLAYER);
	GetPlace()->AddActor2Faction(m_BeChallengePlayer,FACTION_EVIL);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " Battle   <%u>SoloDulePlay: %s challengePlayer c_cid:%d, c_citizenship:%d, c_uid:%d VS beChallengePlayer c_cid:%d, c_citizenship:%d, c_uid:%d m_uniqueID :%lld \n", 
		UT_ServerHelper::GetServerId(), __FUNCTION__, challengePlayerInstanceID.detail(), challengePlayerInstanceID.catagory(), m_ChallengePlayer->GetUserID(), beChallengePlayerInstanceID.detail(), beChallengePlayerInstanceID.catagory(), m_BeChallengePlayer->GetUserID(),m_uniqueID);

	InitBattle( m_ChallengePlayer, m_BeChallengePlayer);
	int BattleTime = m_Config->BattleTime;
	GetPlace()->NotifyGameEventToClient(m_ChallengePlayer,EVENT_TYPE_SOLODULE_BATTLE,BattleTime);
}

void LC_ServerMapLogic_SummitWheelPlay::InitHatred( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController )
{
	float update_time = GET_PROCESS_TIME;
	update_time += m_Config->BattleTime;
	LC_ServerPlayer* pCurrentA = pPlayerAController->GetOwnerPlayer();
	LC_ServerPlayer* pCurrentB = pPlayerBController->GetOwnerPlayer();
	IF_A_NA_PLAYER(pCurrentA)
		return;
	IF_A_NA_PLAYER(pCurrentB)
		return;
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayerA;	
	pCurrentA->GetAllBuddyPtrPlayer( vecBuddyPtrPlayerA);
	std::vector<LC_ServerPlayer*> vecBuddyPtrPlayerB;	
	pCurrentB->GetAllBuddyPtrPlayer( vecBuddyPtrPlayerB);
	for (int i = 0; i < vecBuddyPtrPlayerB.size(); i++)
	{
		LC_ServerPlayer* pBuddyB = vecBuddyPtrPlayerB[i];
		IF_A_NA_PLAYER(pBuddyB)
			continue;
		uint32_t id =  pBuddyB->GetID();
		for (int k = 0; k < vecBuddyPtrPlayerA.size(); k++)
		{
			LC_ServerPlayer* pBuddyA = vecBuddyPtrPlayerA[k];
			IF_A_NA_PLAYER(pBuddyA)
				continue;
			pBuddyA->GetHatredList()->Reset();
			pBuddyA->GetHatredList()->AddHatredOnly(id,10,update_time);
			pBuddyA->SetDefaultTargetID( id );
			pBuddyA->SetSelectTargetID(id );
			pBuddyA->SetDead(false);
			pBuddyA->SetDying(false);
			
			attr_value_type nMaxHP = pBuddyA->GetFinalAttributeMap()->GetAttribute(ATT_MAX_HP);
			pBuddyA->SetHP(nMaxHP);
			pBuddyA->SetSoloDulePlay(true);
		}
	}

}

//#define			MULTI_THREAT_MODE		1
void LC_ServerMapLogic_SummitWheelPlay::InitBattle( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController )
{
	InitHatred(pPlayerAController,pPlayerBController );
	InitHatred(pPlayerBController,pPlayerAController );
	pPlayerAController->SetDenyMove(false);
	pPlayerBController->SetDenyMove(false);
}

#define MULTI_THREAT_MODE

bool LC_ServerMapLogic_SummitWheelPlay::CheckChallengeBeChallengePtr()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	IF_A_NA_PLAYER(challengePlayer)
		return false;
	IF_A_NA_PLAYER(m_BeChallengePlayer)
		return false;
	m_ChallengePlayer = challengePlayer;
	return true;
}

void LC_ServerMapLogic_SummitWheelPlay::SendErrorReward()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	IF_A_NA_PLAYER(challengePlayer)
		return ;
	MG_CS_SkyArena_Challenge_Result resp;
	resp.m_iResult = RE_SKY_REWARD_TIMEOUT_ERROR;
	challengePlayer->SendMsgToClient(MGPT_SC_SkyArena_ChallengeResult, &resp);

	challengePlayer->SetPlayStateEnd(m_mapID);
}

void LC_ServerMapLogic_SummitWheelPlay::UpLoadSoloDulePlayInfoToGroup(LC_ServerPlayer* challengePlayer,LC_ServerPlayer* bechallengePlayer)
{
	LC_RankManager* rank_manager=SERVER_GET_RANK_MANAGER();
	
	MG_GG_REQ_SoloFightReslut reqMsg;
	rank_manager->FillRankPlayerBaseData(challengePlayer, reqMsg.m_solo_result.m_FightData.m_nLeftUserBaseData);
	rank_manager->FillRankPlayerBaseData(bechallengePlayer, reqMsg.m_solo_result.m_FightData.m_nRightUserBaseData);
	for(uint32_t i = 0; i < m_vecResult.size(); ++i )
	{
		SingleSummitWheelFightInfo & singleInfo = m_vecResult[i];
		BaseBattlefieldReport singleReportInfo;
		// challenge teaminfo
		mem::vector<int> challengeBuddyIDVec;
		challengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(challengePlayer, m_mapID, singleInfo.uChallengeTeamId, SchemeType_Offensive, challengeBuddyIDVec);
		challengePlayer->PackBaseBattlefieldBuddyDataToVec(challengeBuddyIDVec,singleReportInfo.m_nLeftBuddyData.m_nBuddyData);
		singleReportInfo.m_nLeftBuddyData.m_nResult = singleInfo.bChallenge;
		singleReportInfo.m_nLeftBuddyData.m_nGroupID = singleInfo.uChallengeTeamId;
		for(uint32_t j = 0; j < singleInfo.vecChallengeReliveBuddyID.size();++j)
		{
			int reliveBuddyId = singleInfo.vecChallengeReliveBuddyID[j];
			for(uint32_t k = 0; k < singleReportInfo.m_nLeftBuddyData.m_nBuddyData.size();++k)
			{
				BaseBattlefieldBuddyData& buddyInfo = singleReportInfo.m_nLeftBuddyData.m_nBuddyData[k];
				if(buddyInfo.m_BuddyInfo.m_nBuddyData.m_nBuddyID == reliveBuddyId)
				{
					buddyInfo.m_nReliveState =1;
					break;
				}
			}
		}
		// bechallenge teaminfo

		mem::vector<int> bechallengeBuddyIDVec;
		bechallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(bechallengePlayer, m_mapID, singleInfo.uBeChallengeTeamId, SchemeType_Defence, bechallengeBuddyIDVec);
		bechallengePlayer->PackBaseBattlefieldBuddyDataToVec(bechallengeBuddyIDVec,singleReportInfo.m_nRightBuddyData.m_nBuddyData);
		singleReportInfo.m_nRightBuddyData.m_nResult = !singleInfo.bChallenge;
		singleReportInfo.m_nRightBuddyData.m_nGroupID = singleInfo.uBeChallengeTeamId;
		for(uint32_t j = 0; j < singleInfo.vecBeChallengeReliveBuddyID.size();++j)
		{
			int reliveBuddyId = singleInfo.vecBeChallengeReliveBuddyID[j];
			for(uint32_t k = 0; k < singleReportInfo.m_nRightBuddyData.m_nBuddyData.size();++k)
			{
				BaseBattlefieldBuddyData& buddyInfo = singleReportInfo.m_nRightBuddyData.m_nBuddyData[k];
				if(buddyInfo.m_BuddyInfo.m_nBuddyData.m_nBuddyID == reliveBuddyId)
				{
					buddyInfo.m_nReliveState =1;
					break;
				}
			}
		}

		singleReportInfo.m_nRepalyId = singleInfo.nFightId; //本场战斗回放id
		if(singleReportInfo.m_nRepalyId == 0 )
			singleReportInfo.m_nRepalyId =GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
		singleReportInfo.b_IsFightValid = singleInfo.bIsFightValid;
		if(challengeBuddyIDVec.size() != 0 || bechallengeBuddyIDVec.size() != 0)
		{
			if( m_vecResult[i].bChallenge )
				reqMsg.m_solo_result.m_leftWinNum ++;
			else
				reqMsg.m_solo_result.m_rightWinNum ++;
		}
		reqMsg.m_solo_result.m_FightBuddyData.push_back(singleReportInfo);

	}
	
	reqMsg.m_solo_result.m_nResult = m_bChallengeWind?(challengePlayer->GetInstance()):(bechallengePlayer->GetInstance());
	reqMsg.m_solo_result.m_nUniqueId = m_solofightuniqueId;// 总战斗id group 生成 
	reqMsg.m_solo_result.m_nAreanType = Arena_Type_Peak;// 结果 0：Arena_Type_Challenge  1：Arena_Type_Firmament 2:Arena_Type_Peak
	reqMsg.m_solo_result.m_nAgainstId = m_uniqueID;
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  UpLoadSoloDulePlayInfoToGroup  ---------- m_uniqueID:%lld\n", m_uniqueID);
	SendMsgToGroup(MGPT_GG_SoloFightResult, &reqMsg, challengePlayer->GetUserID(), challengePlayer->GetInstance());
	End();
}

void LC_ServerMapLogic_SummitWheelPlay::SummonPlayerFinish(LC_ServerPlayer* player)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* place=(LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(player->GetMapLogicID());
	if (place==NULL)
		return;
	LC_ServerMapLogic_SummitWheelPlay* play=(LC_ServerMapLogic_SummitWheelPlay*)place->GetPlay(PlayType_SummitWheel);
	if (play==NULL)
		return;
	bool bChallengeFlag = false;
	if(play->m_ChallengePlayerID == player->GetID())
		bChallengeFlag = true;
	if(false == play->InitTeamInfo(bChallengeFlag))
	{
		return;
	}
	play->m_nEntryPlayerNum++;
	play->SetWheelNextState(SummitWheel_State_LoadPlayerInfo);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  SummonPlayerFinish  ---------- userID:%d m_nEntryPlayerNum %d \n", player->GetUserID(),play->m_nEntryPlayerNum);
}

bool LC_ServerMapLogic_SummitWheelPlay::InitTeamInfo(bool challengeFlag)
{
	if(challengeFlag)
	{
		//LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
		IF_A_NA_PLAYER(m_ChallengePlayer)
			return false;
		m_vecResult.clear();
		mem::vector<uint32_t> vecChallengeResult;
		m_ChallengePlayer->GetBuddySchemeMgr().GetMapSchemeGroupData(m_ChallengePlayer, m_mapID, SchemeType_Offensive, vecChallengeResult);
		if(vecChallengeResult.size() <= 0 )
			return false;
		for (int i = 0; i < vecChallengeResult.size(); ++i )
		{
			SingleSummitWheelFightInfo singleInfo;
			singleInfo.uIndex = i + 1;
			singleInfo.uChallengeTeamId = vecChallengeResult[i];
			m_vecResult.push_back(singleInfo);
		}
	}
	else
	{
		IF_A_NA_PLAYER(m_BeChallengePlayer)
			return false;
		//LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
		IF_A_NA_PLAYER(m_ChallengePlayer)
			return false;
		/*LC_Dule_Manager* dule_Manager = SERVER_GET_DULE_MANAGER();
		LC_Solo_Duel_Manager* pResult =dule_Manager->GetSoloDuleManager(GameLogic::eSoloDuleType_Sky);
		if(NULL == pResult)
			return false;
		Solo_Duel_Info* pDuelinfo = pResult->GetSoloDuelInfo(m_ChallengePlayer->GetInstance());
		if(NULL == pDuelinfo)
			return false;*/
		m_BeChallengePlayer->SetSkyArenaScoreValue(1000);	
		mem::vector<uint32_t> vecBeChallengeResult;
		m_BeChallengePlayer->GetBuddySchemeMgr().GetMapSchemeGroupData(m_BeChallengePlayer, m_mapID, SchemeType_Defence, vecBeChallengeResult);
		if(vecBeChallengeResult.size() <= 0 )
			return false;
		if(vecBeChallengeResult.size() >= m_vecResult.size() )
		{
			for (int i = 0; i < vecBeChallengeResult.size(); ++i )
			{
				if( i < m_vecResult.size() )
				{
					m_vecResult[i].uBeChallengeTeamId = vecBeChallengeResult[i];
				}
				else
				{
					SingleSummitWheelFightInfo singleInfo;
					singleInfo.uIndex = i + 1;
					singleInfo.uChallengeTeamId = vecBeChallengeResult[i];
					singleInfo.uBeChallengeTeamId = vecBeChallengeResult[i];
					m_vecResult.push_back(singleInfo);
				}
			}
		}
		else
		{
			for (int i = 0; i < m_vecResult.size(); ++i )
			{
				if( i < vecBeChallengeResult.size() )
				{
					m_vecResult[i].uBeChallengeTeamId = vecBeChallengeResult[i];
				}
				else
				{
					m_vecResult[i].uBeChallengeTeamId = m_vecResult[i].uChallengeTeamId;
				}
			}
		}
		
		// 当被挑战者 进入后，初始化当前回合 以及 各自初始化的队伍id
		m_uCurrIndex = 1;
		m_CurrBeChallengeTeamId = 1;
		m_CurrChallengeTeamId	= 1;
		InitTeamFightId(m_uCurrIndex);
	}
	return true;
}

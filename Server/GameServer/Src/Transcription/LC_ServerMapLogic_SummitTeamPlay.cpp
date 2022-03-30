#include "LC_ServerMapLogic_SummitTeamPlay.h"
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
#include "LC_Helper.h"
using namespace OPERATELOG;
using namespace GameLogic;
using namespace CSVFile;
using namespace Protocol;
using namespace Utility;

LC_ServerMapLogic_SummitTeamPlay::LC_ServerMapLogic_SummitTeamPlay():
m_Config(NULL),m_State(SummitTeam_State_None),m_nEntryPlayerNum(0),
m_ChallengePlayerID(INVALID_ACTOR_ID),m_BeChallengeCharID(0),
m_ChallengePlayer(NULL),m_BeChallengePlayer(NULL),m_TimeStamp(0.0f),
m_PlayerBuffInfo(),m_solofightuniqueId(0),m_mapID(0),m_CurrChallengeTeamId(0),m_CurrBeChallengeTeamId(0),
m_uCurrIndex(0),m_bChallengeWind(false)
{
	
}
LC_ServerMapLogic_SummitTeamPlay::~LC_ServerMapLogic_SummitTeamPlay()
{
	//UnInit();
}
bool LC_ServerMapLogic_SummitTeamPlay::Init(LC_ServerMapLogic_Place* place,LC_Dule_Config* config)
{
	if (place==NULL || config==NULL)
		return false;
	if ( false == LC_ServerMapLogic_PlayBase::Init(place,PlayType_SummitTeam))
		return false;
	m_Config=config;
	m_nEntryPlayerNum = 0;
	m_ChallengePlayer=NULL;
	m_BeChallengePlayer=NULL;
	m_TimeStamp=0.0f;
	m_PlayerBuffInfo.clear();
	m_vecResult.clear();
	m_State=SummitTeam_State_None;
	m_mapID  = SUMMIT_TEAM_SOLO_DULE_RES_ID;
	return true;
}

void LC_ServerMapLogic_SummitTeamPlay::UnInit()
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
	m_PlayerBuffInfo.clear();
	m_State=SummitTeam_State_None;
	m_Config=NULL;
	LC_ServerMapLogic_PlayBase::UnInit();
}

bool LC_ServerMapLogic_SummitTeamPlay::Start()
{
	if (m_State!=SummitTeam_State_Inited && m_State!=SummitTeam_State_End)
	{
		return false;
	}
	SetSummitTeamNextState(SummitTeam_State_Prepare);
	return true;
}
void LC_ServerMapLogic_SummitTeamPlay::ClearBuddyOp(LC_ServerPlayer* pPlayer)
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
void LC_ServerMapLogic_SummitTeamPlay::SyncFightReportToGroup()
{

}

void LC_ServerMapLogic_SummitTeamPlay::updateNullTeamIDFailed()
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


bool LC_ServerMapLogic_SummitTeamPlay::CheckCanNextTeamBattle()
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
					updateRemainTeamFailed(m_uCurrIndex,true);
					return false;
				}
				if(beChallengeBuddyIDVec.size() == 0)
				{
					updateRemainTeamFailed(m_uCurrIndex,false);
					return false;
				}
				return true;
			}
		}
	}
	return false;
}
void LC_ServerMapLogic_SummitTeamPlay::updateRemainTeamFailed(int currIndex,bool bChallengeFailed)
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
bool LC_ServerMapLogic_SummitTeamPlay::PrepareNextTeamBattle()
{
	if(false == CheckChallengeBeChallengePtr())
		return false;
	if(false == CheckCanNextTeamBattle())
	{
		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  PrepareNextTeamBattle CheckCanNextTeamBattle false ---------- m_uCurrIndex:%d\n", m_uCurrIndex);
		//  ս������ ,׼������
		SetSummitTeamNextState(SummitTeam_State_BattleEnd);
		return false;
	}
	m_uCurrIndex++;
	//int64_t solofightuniqueId = GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
	InitTeamFightId(m_uCurrIndex);
	m_ChallengePlayer->SwitchBuddyScheme(m_mapID, ++m_CurrChallengeTeamId, SchemeType_Offensive, true, true);
	m_BeChallengePlayer->SwitchBuddyScheme(m_mapID, ++m_CurrBeChallengeTeamId, SchemeType_Defence, true, true);
	InitAllBuddyInfo();
	ChallengePlayerActionForbid(m_ChallengePlayer);
	ChallengePlayerActionForbid(m_BeChallengePlayer);
	return true;
}

void LC_ServerMapLogic_SummitTeamPlay::End()
{
	if (!CheckRun())
		return;
	// ֹͣս����Ϊ
	if(false == CheckChallengeBeChallengePtr())
		return;
	
	ChallengePlayerActionUnForbid(m_ChallengePlayer);
	ChallengePlayerActionUnForbid(m_BeChallengePlayer);
	// �ָ�SM_Controller
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

	//ɾ���Է�
	GetPlace()->AddActor2Faction(m_BeChallengePlayer, FACTION_EVIL);
	m_BeChallengePlayer->GetActorSkillList()->Reset();
	m_BeChallengePlayer->SetSoloDulePlay(false);
	
	GetPlace()->AddActor2Faction(m_ChallengePlayer,FACTION_PLAYER);
	m_ChallengePlayer->GetActorSkillList()->Reset();
	m_ChallengePlayer->SetSoloDulePlay(false);

	SetSummitTeamNextState(SummitTeam_State_End);
}
void LC_ServerMapLogic_SummitTeamPlay::SetSummitTeamNextState(SummitTeam_State eState)
{
	GET_PROCESS_TICK;
	if( m_State >= SummitTeam_State_WaitReward && eState <= SummitTeam_State_WaitReward)
		return;
	

	if(eState ==SummitTeam_State_LoadPlayerInfo)
	{
		m_TimeStamp=GET_PROCESS_TIME;
	}
	else if(eState ==SummitTeam_State_ShowAllInfo)
	{
		int ShowAllInfoTime = m_Config->ShowAllInfoTime;
		m_TimeStamp=GET_PROCESS_TIME+ShowAllInfoTime;
	}
	else if(eState == SummitTeam_State_PrepareNextTeam)
	{
		m_TimeStamp=GET_PROCESS_TIME+1;
	}
	else if(eState == SummitTeam_State_ShowTeamInfo)
	{
		int ShowTeamInfoTime = m_Config->ShowTeamInfoTime;
		m_TimeStamp=GET_PROCESS_TIME+ShowTeamInfoTime;
	}
	else if(eState == SummitTeam_State_Battle)
	{
		int BattleTime = m_Config->BattleTime;
		m_TimeStamp=GET_PROCESS_TIME+BattleTime;
	}
	else if(eState == SummitTeam_State_BattleEnd)
	{
		m_TimeStamp=GET_PROCESS_TIME;
	}
	else if(eState == SummitTeam_State_WaitReward)
	{
		m_TimeStamp= GET_PROCESS_TIME+10 ;
	}
	else if(eState == SummitTeam_State_End)
	{
		m_TimeStamp=GET_PROCESS_TIME;
	}
	m_State =  eState;
}
bool LC_ServerMapLogic_SummitTeamPlay::Update(float fCurrentTime, float fDeltaTime)
{
	if(m_TimeStamp == 0.0f || m_TimeStamp > fCurrentTime || m_State >= SummitTeam_State_End)
		return true;
	if(m_State ==SummitTeam_State_LoadPlayerInfo)
	{
		// SummonPlayerFinish
		// SummonPlayerFinish
		if( m_nEntryPlayerNum == 2 )
		{
			InitAllBuddyInfo();
			//
			SetSummitTeamNextState(SummitTeam_State_ShowAllInfo);
			// չʾȫ����Ϣ
			NotifyShowAllInfoTime();
			m_nEntryPlayerNum = 0;
		}
	}
	else if(m_State == SummitTeam_State_ShowAllInfo)
	{
		NotifyShowTeamInfoTime();
		SetSummitTeamNextState(SummitTeam_State_ShowTeamInfo);
	}
	else if(m_State ==SummitTeam_State_PrepareNextTeam)
	{
		// 
		// �򶷽��� ����ս�� ��group �л���һ��
		if(PrepareNextTeamBattle())
		{
			NotifyShowTeamInfoTime();
			SetSummitTeamNextState(SummitTeam_State_ShowTeamInfo);
		}
	}
	else if(m_State == SummitTeam_State_ShowTeamInfo)
	{
		Battle();
		SetSummitTeamNextState(SummitTeam_State_Battle);
	}
	else if(m_State == SummitTeam_State_Battle)
	{
		// һ��ս������ ʱ�䵽
		BattleEnd(0,NULL,true);
	}
	else if(m_State == SummitTeam_State_BattleEnd)
	{
		// �ж� ���� ʤ��
		Settle();
		// ���� 
		SendReward();
		
	}
	else if(m_State == SummitTeam_State_WaitReward )
	{
		//�ȴ�group 10��󣬷�����Ϣ��δ�յ�����ʱ������ �ٷ�����Ϣ�˳�
		//TODO
		SendErrorReward();
	}
	return true;
}

ResultType LC_ServerMapLogic_SummitTeamPlay::CheckEnter(LC_ServerPlayer* player)
{
	IF_A_NA_PLAYER(player)
		return RE_FAIL;
	return RE_SUCCESS;
}

Utility::UT_Vec3Int	LC_ServerMapLogic_SummitTeamPlay::ComputeEnterLocation(LC_ServerPlayer* player,const Utility::UT_Vec3Int& targetlocation)
{
	return m_Config->Factions[0].Position;
}

void LC_ServerMapLogic_SummitTeamPlay::EventNotify(MapLogicEvent& kEvent)
{
	
}

void LC_ServerMapLogic_SummitTeamPlay::PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent)
{
	IF_A_NA_PLAYER(player)
		return ;
	if ( kEvent.Type() == PlayerEvent_Enter)
	{
		GetPlace()->AddActor2Faction(player, FACTION_NEUTRAL);
		ChallengePlayerActionForbid(player);
		if(false == InitTeamInfo(true))
		{
			//TODO
			//��ʼ��ʧ�ܣ��˳�
			return ;
		}
		// ׼����ȡ���
		Prepare();

		m_nEntryPlayerNum++;
		SetSummitTeamNextState(SummitTeam_State_LoadPlayerInfo);

		GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  LC_ServerMapLogic_SummitTeamPlay PlayerEvent_Enter  m_nEntryPlayerNum %d  ---------- c_uid:%d\n", player->GetUserID(),m_nEntryPlayerNum);

		
	}
	else if (kEvent.Type()==PlayerEvent_Leave || kEvent.Type() ==PlayerEvent_Death)
	{
		// ���� �¼� ������Ѫ��Ϊ0 ����֮ǰ�� �������ﲻ����������Ӹ�״̬λ��ת �ӳ� 1�����Ҹ���
		BattleEnd(kEvent.Type(),player);
		
	}
}
void LC_ServerMapLogic_SummitTeamPlay::SetRoundBattleResult(LC_ServerPlayer* player,bool timeOut)
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
bool LC_ServerMapLogic_SummitTeamPlay::CheckOwnerDead( LC_ServerPlayer* pPlayer, object_id_type& nDeadPlayerID )
{
	bool bResult = false;
	bResult = pPlayer->CheckOwnerDead();
	if (bResult)
		nDeadPlayerID = pPlayer->GetOwnerPlayerId();
	return bResult;
}

bool LC_ServerMapLogic_SummitTeamPlay::CheckEnd()
{
	return m_State == SummitTeam_State_End;
}

bool LC_ServerMapLogic_SummitTeamPlay::CheckRun()
{
	if (	m_State == SummitTeam_State_Prepare 
		||	m_State == SummitTeam_State_LoadPlayerInfo
		||	m_State == SummitTeam_State_ShowAllInfo 
		||	m_State == SummitTeam_State_ShowTeamInfo 
		||	m_State == SummitTeam_State_PrepareNextTeam
		||	m_State == SummitTeam_State_Battle
		||	m_State == SummitTeam_State_BattleEnd
		||	m_State == SummitTeam_State_WaitReward)
	{
		return true;
	}
	return false;
}
void LC_ServerMapLogic_SummitTeamPlay::Settle(bool bLeave)
{
	// �ж����
	if(bLeave)
	{
		m_bChallengeWind = false;
		IF_A_NA_PLAYER(m_BeChallengePlayer)
			return;
		for (int i = 0; i < m_vecResult.size(); ++i )
		{
			if( m_vecResult[i].uIndex <= m_uCurrIndex)
				continue;
			m_BeChallengePlayer->GetBuddySchemeMgr().GetBuddySchemeData(m_BeChallengePlayer, m_mapID, m_vecResult[i].uBeChallengeTeamId, SchemeType_Defence, m_vecResult[i].vecBeChallengeReliveBuddyID);
		}
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
void LC_ServerMapLogic_SummitTeamPlay::SendReward()
{
	SetSummitTeamNextState(SummitTeam_State_WaitReward);
	//����
	if(false == CheckChallengeBeChallengePtr())
		return;
	UpLoadSoloDulePlayInfoToGroup(m_ChallengePlayer,m_BeChallengePlayer);
}

void LC_ServerMapLogic_SummitTeamPlay::ChallengePlayerActionForbid(LC_ServerPlayer* pPlayer)
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
	
	m_PlayerBuffInfo.clear();

	pPlayer->GetStateMap()->RemoveSkillStateInStateMap(GOD_STATE_ID);
	char_data_define::save_skill_state_info info;
	pPlayer->GetStateMap()->Clear_SaveAdditionalStateToPB(info);
	info.SerializeToString(&m_PlayerBuffInfo);
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

void LC_ServerMapLogic_SummitTeamPlay::ChallengePlayerActionUnForbid(LC_ServerPlayer* pPlayer)
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
	char_data_define::save_skill_state_info info;
	info.ParseFromString(m_PlayerBuffInfo);
	pPlayer->GetStateMap()->GetSaveStateFromPB(info,false);
	pPlayer->GetStateMap()->StartSavedSkillState();
	pPlayer->SyncSkillStatesToClient(SSC_ALL);
}

void LC_ServerMapLogic_SummitTeamPlay::ReservaDuleInfo(object_id_type challengeid,const unique_id_impl& bechallengeid)
{
	m_ChallengePlayerID=challengeid;
	m_BeChallengeCharID=bechallengeid;
	//m_solofightuniqueId = solofightuniqueId;
	m_solofightuniqueId = GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	IF_A_VALID_PLAYER(challengePlayer)
		GetPlace()->NotifyGameEventToClient(challengePlayer,EVENT_TYPE_FIGHT_RECORD_START,m_solofightuniqueId,challengePlayer->GetUserID(),1 ,GetType());
}

void LC_ServerMapLogic_SummitTeamPlay::Prepare()
{
	LC_ServerPlayer *bechallengeplayer=GetPlace()->SpawnPlayer(m_BeChallengeCharID, m_Config->Factions[1].Position, PLY_DEF_CLONE_BITS, NULL, LC_ServerMapLogic_SummitTeamPlay::SummonPlayerFinish, FACTION_NEUTRAL);
	IF_A_NA_PLAYER(bechallengeplayer)
		return;
	m_BeChallengePlayer=bechallengeplayer;
	m_BeChallengePlayer->GetBuddySchemeMgr().SetBeChallangeMapID(m_mapID);
	m_BeChallengePlayer->SetSoloDulePlay(true);

	if(IS_SOLODULE_ROBOT_PLAYERID(m_BeChallengeCharID) )
	{
		//�ǻ����ˣ���������
		bechallengeplayer->InitRobotLogicData(m_BeChallengeCharID);
	}
	return;
}

void LC_ServerMapLogic_SummitTeamPlay::InitAllBuddyInfo()
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

void LC_ServerMapLogic_SummitTeamPlay::PackSkyRoundTeamDeadInfo(MG_NOTIFY_SkyRoundShowInfo& notifyMsg)
{
	notifyMsg.m_LeftDeadTeamIdVec.clear();
	notifyMsg.m_RightDeadTeamIdVec.clear();
	for (int i = 0; i <m_vecResult.size(); ++i)
	{
		SingleSummitTeamFightInfo &teamInfo = m_vecResult[i];
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
void LC_ServerMapLogic_SummitTeamPlay::UpdateBuddyDeadInfo(LC_ServerPlayer* pPlayer,mem::vector <int>& vecReliveBuddyID )
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
void LC_ServerMapLogic_SummitTeamPlay::UpdateTeamDeadInfo(int nCurrIndex,unique_id_impl uWindPlayerId,bool bChallenge )
{
	for (int i = 0; i <m_vecResult.size(); ++i)
	{
		SingleSummitTeamFightInfo &teamInfo = m_vecResult[i];
		if(nCurrIndex == teamInfo.uIndex)
		{
			teamInfo.bChallenge = bChallenge;
			teamInfo.uWindPlayerId = uWindPlayerId;
			UpdateBuddyDeadInfo(m_ChallengePlayer, teamInfo.vecChallengeReliveBuddyID);
			UpdateBuddyDeadInfo(m_BeChallengePlayer, teamInfo.vecBeChallengeReliveBuddyID);
			IF_A_VALID_PLAYER(m_ChallengePlayer)
				GetPlace()->NotifyGameEventToClient(m_ChallengePlayer,EVENT_TYPE_FIGHT_RECORD_END,teamInfo.nFightId,m_ChallengePlayer->GetUserID());
			return;
		}
	}
}
void LC_ServerMapLogic_SummitTeamPlay::InitTeamFightId(int nCurrIndex)
{
	for (int i = 0; i <m_vecResult.size(); ++i)
	{
		SingleSummitTeamFightInfo &teamInfo = m_vecResult[i];
		if(nCurrIndex == teamInfo.uIndex)
		{
			if( nCurrIndex == 1)
				teamInfo.nFightId = m_solofightuniqueId;
			else
			{
				teamInfo.nFightId =GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
				IF_A_VALID_PLAYER(m_ChallengePlayer)
					GetPlace()->NotifyGameEventToClient(m_ChallengePlayer,EVENT_TYPE_FIGHT_RECORD_START,teamInfo.nFightId,m_ChallengePlayer->GetUserID(),nCurrIndex,GetType() );
			}
			return;
		}
	}
}

void LC_ServerMapLogic_SummitTeamPlay::NotifyShowAllInfoTime()
{
	if(false == CheckChallengeBeChallengePtr())
		return;
	int ShowAllInfoTime = m_Config->ShowAllInfoTime;
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  NotifyShowAllInfoTime  ---------- m_uCurrIndex:%d\n", m_uCurrIndex);
	MG_NOTIFY_SkyAllShowInfo notifyMsg;
	notifyMsg.m_Time = ShowAllInfoTime;
	m_ChallengePlayer->FillSingleShowInfoByMapId(notifyMsg.m_Left,m_mapID,SchemeType_Offensive);
	m_BeChallengePlayer->FillSingleShowInfoByMapId(notifyMsg.m_Right,m_mapID,SchemeType_Defence);
	m_ChallengePlayer->SendMsgToClient(MGPT_SC_SKY_ALL_SHOW_INFO, &notifyMsg);	
}
void LC_ServerMapLogic_SummitTeamPlay::NotifyShowTeamInfoTime()
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
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------  NotifyShowTeamInfoTime  ---------- m_uCurrIndex:%d\n", m_uCurrIndex);
}
void LC_ServerMapLogic_SummitTeamPlay::PlayerLevave(int opType)
{
	if(opType == QSMOT_None)
	{
		End();
	}
	else if (opType == QSMOT_ReqSettle)
	{
		// �ж� ���� ʤ��
		Settle(true);
		// ���� 
		SendReward();
	}
	
}
void LC_ServerMapLogic_SummitTeamPlay::BattleEnd(int endType,LC_ServerPlayer* pPlayer,bool timeOut)
{
	if(false == CheckChallengeBeChallengePtr())
		return;
	// ֹͣ ˫���Ĵ�
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
		SetSummitTeamNextState(SummitTeam_State_PrepareNextTeam);
		return;
	}
	if (endType==PlayerEvent_Leave)
	{
		if(m_State == SummitTeam_State_PrepareNextTeam || m_State == SummitTeam_State_BattleEnd)
			return;
		// ϵͳ�ڵ��õ��뿪�������� ����ֶ����뿪 ��Э�鴦 ����
	}
	else if (endType ==PlayerEvent_Death)
	{
		SetRoundBattleResult(pPlayer);
		SyncFightReportToGroup();
		SetSummitTeamNextState(SummitTeam_State_PrepareNextTeam);
	}
}


void LC_ServerMapLogic_SummitTeamPlay::Battle()
{
	if(false == CheckChallengeBeChallengePtr())
		return;
	const unique_id_impl& challengePlayerInstanceID = m_ChallengePlayer->GetInstance();
	const unique_id_impl& beChallengePlayerInstanceID = m_BeChallengePlayer->GetInstance();
	GetPlace()->AddActor2Faction(m_ChallengePlayer,FACTION_PLAYER);
	GetPlace()->AddActor2Faction(m_BeChallengePlayer,FACTION_EVIL);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "<%u>SoloDulePlay: %s challengePlayer c_cid:%d, c_citizenship:%d, c_uid:%d VS beChallengePlayer c_cid:%d, c_citizenship:%d, c_uid:%d", 
		UT_ServerHelper::GetServerId(), __FUNCTION__, challengePlayerInstanceID.detail(), challengePlayerInstanceID.catagory(), m_ChallengePlayer->GetUserID(), beChallengePlayerInstanceID.detail(), beChallengePlayerInstanceID.catagory(), m_BeChallengePlayer->GetUserID());
	InitBattle( m_ChallengePlayer, m_BeChallengePlayer);
	int BattleTime = m_Config->BattleTime;
	GetPlace()->NotifyGameEventToClient(m_ChallengePlayer,EVENT_TYPE_SOLODULE_BATTLE,BattleTime);
}

void LC_ServerMapLogic_SummitTeamPlay::InitHatred( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController )
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
void LC_ServerMapLogic_SummitTeamPlay::InitBattle( LC_ServerPlayer* pPlayerAController, LC_ServerPlayer* pPlayerBController )
{
	InitHatred(pPlayerAController,pPlayerBController );
	InitHatred(pPlayerBController,pPlayerAController );
	pPlayerAController->SetDenyMove(false);
	pPlayerBController->SetDenyMove(false);
}

#define MULTI_THREAT_MODE

bool LC_ServerMapLogic_SummitTeamPlay::CheckChallengeBeChallengePtr()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	IF_A_NA_PLAYER(challengePlayer)
		return false;
	IF_A_NA_PLAYER(m_BeChallengePlayer)
		return false;
	m_ChallengePlayer = challengePlayer;
	return true;
}

void LC_ServerMapLogic_SummitTeamPlay::SendErrorReward()
{
	LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
	IF_A_NA_PLAYER(challengePlayer)
		return ;
	MG_CS_SkyArena_Challenge_Result resp;
	resp.m_iResult = RE_SKY_REWARD_TIMEOUT_ERROR;
	challengePlayer->SendMsgToClient(MGPT_SC_SkyArena_ChallengeResult, &resp);

	challengePlayer->SetPlayStateEnd(m_mapID);
}

void LC_ServerMapLogic_SummitTeamPlay::UpLoadSoloDulePlayInfoToGroup(LC_ServerPlayer* challengePlayer,LC_ServerPlayer* bechallengePlayer)
{
	LC_RankManager* rank_manager=SERVER_GET_RANK_MANAGER();
	
	MG_GG_REQ_SoloFightReslut reqMsg;
	rank_manager->FillRankPlayerBaseData(challengePlayer, reqMsg.m_solo_result.m_FightData.m_nLeftUserBaseData);
	rank_manager->FillRankPlayerBaseData(bechallengePlayer, reqMsg.m_solo_result.m_FightData.m_nRightUserBaseData);
	for(uint32_t i = 0; i < m_vecResult.size(); ++i )
	{
		SingleSummitTeamFightInfo & singleInfo = m_vecResult[i];
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
		singleReportInfo.b_IsFightValid = singleInfo.bIsFightValid;//
		singleReportInfo.m_nRepalyId = singleInfo.nFightId; //����ս���ط�id
		if(singleReportInfo.m_nRepalyId == 0 )
			singleReportInfo.m_nRepalyId =GameLogic::LC_Helper::generate_unique_id(0, GetGameServerID());
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
	reqMsg.m_solo_result.m_nUniqueId = m_solofightuniqueId;// ��ս��id group ���� 
	reqMsg.m_solo_result.m_nAreanType = Arena_Type_Peak_Group;// ��� 0��Arena_Type_Challenge  1��Arena_Type_Firmament 2:Arena_Type_Peak
	
	SendMsgToGroup(MGPT_GG_SoloFightResult, &reqMsg, challengePlayer->GetUserID(), challengePlayer->GetInstance());
	
}

void LC_ServerMapLogic_SummitTeamPlay::SummonPlayerFinish(LC_ServerPlayer* player)
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerMapLogic_Place* place=(LC_ServerMapLogic_Place*)pkWorldManager->GetMapLogic(player->GetMapLogicID());
	if (place==NULL)
		return;
	LC_ServerMapLogic_SummitTeamPlay* play=(LC_ServerMapLogic_SummitTeamPlay*)place->GetPlay(PlayType_SummitTeam);
	if (play==NULL)
		return;
	char_data_define::save_skill_state_info info;
	player->GetStateMap()->Clear_SaveAdditionalStateToPB(info);
	if(false == play->InitTeamInfo(false))
	{
		return;
	}
	play->m_nEntryPlayerNum++;
	play->SetSummitTeamNextState(SummitTeam_State_LoadPlayerInfo);
	GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, " ------------ LC_ServerMapLogic_SummitTeamPlay  SummonPlayerFinish  ---------- userID:%d m_nEntryPlayerNum %d \n", player->GetUserID(),play->m_nEntryPlayerNum);

}

bool LC_ServerMapLogic_SummitTeamPlay::InitTeamInfo(bool challengeFlag)
{
	if(challengeFlag)
	{
		LC_ServerPlayer* challengePlayer=GetPlace()->GetActorManger().GetPlayer(m_ChallengePlayerID);
		IF_A_NA_PLAYER(challengePlayer)
			return false;
		m_vecResult.clear();
		mem::vector<uint32_t> vecChallengeResult;
		challengePlayer->GetBuddySchemeMgr().GetMapSchemeGroupData(challengePlayer, m_mapID, SchemeType_Offensive, vecChallengeResult);
		if(vecChallengeResult.size() <= 0 )
			return false;
		for (int i = 0; i < vecChallengeResult.size(); ++i )
		{
			SingleSummitTeamFightInfo singleInfo;
			singleInfo.uIndex = i + 1;
			singleInfo.uChallengeTeamId = vecChallengeResult[i];
			m_vecResult.push_back(singleInfo);
		}
	}
	else
	{
		IF_A_NA_PLAYER(m_BeChallengePlayer)
			return false;
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
					SingleSummitTeamFightInfo singleInfo;
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
		
		// ������ս�� ����󣬳�ʼ����ǰ�غ� �Լ� ���Գ�ʼ���Ķ���id
		m_uCurrIndex = 1;
		m_CurrBeChallengeTeamId = 1;
		m_CurrChallengeTeamId	= 1;
		InitTeamFightId(m_uCurrIndex);
	}
	return true;
}

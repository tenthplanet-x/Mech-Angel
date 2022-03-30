#include "ClientPCH.h"

#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientGamePlayerManager.h"
#include "LC_ClientNPCManager.h"
#include "LC_ClientPlaceHolderManager.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientWorldManager.h"
#include "LC_TooltipManager.h"
#include "LC_ClientPlayerGroupManager.h"
#include "SK_ClientSkillEventManager.h"

#include "MG_MessageBase.h"

#include "IM_Manager.h"

#include "NW_Helper.h"
#include "NW_ClientSessionManager.h"
#include "NW_Game2ClientSession.h"

#include "UT_LogicInfoMessage.h"
#include "LC_ClientGuildManager.h"
#include "LC_ObjectDescription.h"
#include "LC_GameStoryDataDefine.h"
#include "UT_Timer.h"
#include "CF_Helper.h"

using namespace GameLogic;
using namespace Utility;
using namespace Protocol;
using namespace StateMachine;
using namespace Skill;
using namespace Data;
//----------------------------------------------
//float LC_ClientLogicManager::m_fBeginRecord = 0.0f;
LC_ClientLogicManager::LC_ClientLogicManager()
: LC_LogicManagerBase()
, m_lOperationPlayerID(0)
, m_lInputControlActorID(0)
, m_lOperationCharID(INVALID_UNIQUE_ID)
, m_pkNetPlayerManager(NULL)
//, m_pkLocalNpcManager(NULL)
#ifdef _MAP_VIEWER_BUILD
, m_iCreatedViewActorID(-1)
#endif
{
	// 由于gamePlayer和netPlayer都使用了packAsset，
	// 于是先暂时摆在这里调用；
	LC_PackAsset::Init();

	m_pkGamePlayerManager		= T_NEW_D LC_ClientGamePlayerManager;
	m_pkNPCManager				= T_NEW_D LC_ClientNPCManager;
   // m_pkLocalNpcManager			= T_NEW_D LC_ClientLocalNPCManager;
	m_pkNetPlayerManager		= T_NEW_D LC_ClientNetPlayerManager;
	m_pkWorldManager			= T_NEW_D LC_ClientWorldManager;
	m_pkPlaceHolderManager		= T_NEW_D LC_ClientPlaceHolderManager;
	m_pkPlayerGroupManager		= T_NEW_D LC_ClientPlayerGroupManager;
	m_pkSkillEventManager		= T_NEW_D SK_ClientSkillEventManager;

	m_pkTranscriptionManager	= T_NEW_D LC_ClientTranscriptionInfoManager;
	m_pkGameStoryCommonManager	= T_NEW_D GameStoryCommonManager;
	m_pkGameStoyManager			= T_NEW_D LC_ClientGameStoryManager;
	m_pkClientOpenAbilityManager= T_NEW_D LC_ClientOpenAbilityManager;
	m_pkClientRumorManager		= T_NEW_D LC_ClientRumorManager;

	m_pkWorldGuildAssert		= T_NEW_D LC_WorldGuildWarAsset;
}
//--------------------------------------------------------
LC_ClientLogicManager::~LC_ClientLogicManager()
{
	T_SAFE_DELETE(m_pkGamePlayerManager);
	T_SAFE_DELETE(m_pkNPCManager);
	//T_SAFE_DELETE(m_pkLocalNpcManager);
	T_SAFE_DELETE(m_pkNetPlayerManager);
	T_SAFE_DELETE(m_pkWorldManager);
	T_SAFE_DELETE(m_pkPlaceHolderManager);
	T_SAFE_DELETE(m_pkPlayerGroupManager);
	T_SAFE_DELETE(m_pkSkillEventManager);
	T_SAFE_DELETE(m_pkTranscriptionManager);
	m_pkGameStoryCommonManager->Unit();
	T_SAFE_DELETE(m_pkGameStoryCommonManager);
	T_SAFE_DELETE(m_pkGameStoyManager);
	T_SAFE_DELETE(m_pkClientOpenAbilityManager);
	T_SAFE_DELETE(m_pkClientRumorManager);

	T_SAFE_DELETE(m_pkWorldGuildAssert);

	// 由于gamePlayer和netPlayer都使用了packAsset，
	// 于是先暂时摆在这里调用；
	LC_PackAsset::Uninit();
}
//--------------------------------------------------------
void LC_ClientLogicManager::Init()
{
	m_pkWorldManager->Init();
}
//--------------------------------------------------------
void LC_ClientLogicManager::UnInit()
{
	if (m_pkGamePlayerManager)
	{
		LC_ClientGamePlayer* pkGamePlayer = NULL;
		pkGamePlayer = (LC_ClientGamePlayer*)m_pkGamePlayerManager->FindPlayer(m_lOperationPlayerID);
		if(pkGamePlayer)
		{
			pkGamePlayer->GetPackAsset()->Clear();
			//< tx对接
			report2tx_quit(pkGamePlayer->GetID(), pkGamePlayer->GetLevel(), pkGamePlayer->GetOnlineTime());

			m_pkGamePlayerManager->DestroyPlayer(pkGamePlayer);
			_detachMessageHandler(pkGamePlayer);
		}

		m_pkGamePlayerManager->Reset();
	}

	/*if(m_pkMonsterManager)
		m_pkMonsterManager->Reset();*/

	if (m_pkPlaceHolderManager)
		m_pkPlaceHolderManager->Reset();

	if(m_pkNPCManager)
		m_pkNPCManager->Reset();

    //if (m_pkLocalNpcManager)
    //    m_pkLocalNpcManager->Reset();

	if(m_pkNetPlayerManager)
		m_pkNetPlayerManager->Reset();

	if(m_pkPlayerGroupManager)
		m_pkPlayerGroupManager->Reset();

	if(m_pkWorldManager)
		m_pkWorldManager->UnInit();

	if (LC_ClientGuildManager::GetSingletonPtr())
		LC_ClientGuildManager::GetSingletonPtr()->Reset();
}
//--------------------------------------------------------
bool LC_ClientLogicManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	MESSAGE_HANDLE(MGT_TIMER_TICK,_event_TimerTick)
	MESSAGE_HANDLE(MGT_INPUT_KEY_ADD,_event_InputKeyAdd)
	MESSAGE_HANDLE(MGT_INPUT_KEY_2,_event_InputKey2)
	MESSAGE_HANDLE(MGT_INPUT_KEY_3,_event_InputKey3)
	MESSAGE_HANDLE(MSG_UE_SHOW_PLAYER_HEADALL, _eventHeadNPCHead)
	END_MESSAGE_HANDLE
	return true;
}
//--------------------------------------------------------
LC_ClientNetPlayerManager* LC_ClientLogicManager::GetNetPlayerManager()
{
	return m_pkNetPlayerManager;
}


//--------------------------------------------------------
void LC_ClientLogicManager::_eventHeadNPCHead(MG_MessageBase& rkMessage)
{
	LC_ClientGamePlayer* pkGamePlayer = GetOperationPlayer();
	if(!pkGamePlayer)
		return;
	LC_FactionManager* pkFactionManager = LC_FactionManager::GetSingletonPtr();
	if (NULL == pkFactionManager)
	{
		return;
	}

	int m_iActiveLevel = rkMessage.m_lParam1;
	LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
	if (NULL == pkNPCManager)
	{
		return;
	}
	pkNPCManager->_eventHeadNPC(m_iActiveLevel);
}

//--------------------------------------------------------
void LC_ClientLogicManager::_event_TimerTick(MG_MessageBase& rkMessage)
{
	//改到各个子Manager中去计数
	T_PROFILE_EX( "Update::LC_ClientLogicManager",5.0f );
	float fCurrentTime = rkMessage.m_fParam1;
	float fDeltaTime = rkMessage.m_fParam2;

	m_pkGamePlayerManager->Update(fCurrentTime,fDeltaTime);

	m_pkNetPlayerManager->Update(fCurrentTime,fDeltaTime);

	m_pkNPCManager->Update(fCurrentTime,fDeltaTime);

	m_pkWorldManager->Update(fCurrentTime,fDeltaTime);

	m_pkPlaceHolderManager->Update(fCurrentTime,fDeltaTime);

	m_pkSkillEventManager->Update(fCurrentTime, fDeltaTime);

	m_pkTranscriptionManager->Update(fCurrentTime, fDeltaTime);

	m_pkGameStoyManager->Update(fCurrentTime,fDeltaTime);
}
//--------------------------------------------------------
void LC_ClientLogicManager::SetOperationPlayerID( object_id_type lID )
{
	m_lOperationPlayerID = lID;
}
//------------------------------------------------------------------------------
void LC_ClientLogicManager::SetInputControlActorID( object_id_type lID, bool bMsg /*= true*/ )
{
	LC_ClientGamePlayer* pkGamePlayer = NULL;

	if (IS_PLAYER_CLASS_ID(m_lInputControlActorID) && bMsg )
	{
		pkGamePlayer = (LC_ClientGamePlayer*)m_pkGamePlayerManager->FindPlayer(m_lInputControlActorID);
		if(pkGamePlayer)
		{
			_detachMessageHandler(pkGamePlayer);
			pkGamePlayer->GetMovePath().Clear();
		}
	}

	m_lInputControlActorID = lID;
	if (IS_PLAYER_CLASS_ID(m_lInputControlActorID) && bMsg)
	{
		pkGamePlayer = (LC_ClientGamePlayer*)m_pkGamePlayerManager->FindPlayer(m_lInputControlActorID);
		if(pkGamePlayer)
		{
			_attachMessageHandler(pkGamePlayer);
			pkGamePlayer->GetMovePath().Clear();
		}
	}
}
//--------------------------------------------------------
LC_ClientGamePlayer* LC_ClientLogicManager::GetOperationPlayer()
{
	if (!m_pkGamePlayerManager)
		return NULL;
	LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)m_pkGamePlayerManager->FindPlayer(m_lOperationPlayerID);
	return pkGamePlayer;
}
//--------------------------------------------------------
void LC_ClientLogicManager::_event_InputKeyAdd( MG_MessageBase& rkMessage )
{
	MG_PACKET_ENCODE_HANDLER hMsgHandler;
	bool bCreateMsg = false;

	static int              siNPCAdd = 100;
	LC_ClientGamePlayer*    pkGamePlayer = (LC_ClientGamePlayer*)m_pkGamePlayerManager->FindPlayer(m_lOperationPlayerID);

	if (pkGamePlayer)
	{
		UT_Vec3Int kCurrentLocation = pkGamePlayer->GetCurrentLocation();

		// 模拟创建怪物
		/*MG_SyncNPCsInfoInFOV kMsg;
		kMsg.m_lNPCNum										= 1;
		kMsg.m_akNPCInfoEntries[0].m_lID					= MONSTER_ID_MIN + siNPCAdd;
		kMsg.m_akNPCInfoEntries[0].m_lNPCCharType		= 41111007;
		kMsg.m_akNPCInfoEntries[0].m_iCurrentLocationX		= kCurrentLocation.x;
		kMsg.m_akNPCInfoEntries[0].m_iCurrentLocationY		= kCurrentLocation.y;
		bCreateMsg = CreateMsgPacketHandler(hMsgHandler, MGPT_SYNC_NPCS_INFO_IN_FOV, &kMsg);
	
		if(bCreateMsg)
	        pkGamePlayer->GetSession()->OnRecv(hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());*/
		
		++siNPCAdd;
	}

	//pkGamePlayer->NotifySkillEvent(Skill::SET_BE_DAMAGE,0,0,100,0);

	/*SendMessage( CreateMsg_UE_DELTA_EXP( pkGamePlayer->GetID(), 98) );
	SendMessage( CreateMsg_UE_DELTA_HP( pkGamePlayer->GetID(), 98) );
	SendMessage( CreateMsg_UE_DELTA_MP( pkGamePlayer->GetID(), 98) );
	pkGamePlayer->SetHP( pkGamePlayer->GetHP() - 50 );
	pkGamePlayer->SetMP( pkGamePlayer->GetMP() - 60 );

	SendMessage( CreateMsg_UE_SHOW_PLAYER_NAME(pkGamePlayer->GetID()) );*/
}
//---------------------------------------------------------------------------
void LC_ClientLogicManager::_event_InputKey3( MG_MessageBase& rkMessage )
{
#ifdef _MAP_VIEWER_BUILD
	GD_ClientActorModel* pkActor = IM_Manager::GetSingleton().GetActor(m_iCreatedViewActorID);
	if(pkActor)
	{
		UT_ConfigFile kConfig;
		kConfig.Load(GfxMakeFullPath(CLIENT_PREVIEW_MAP_CONFIG_FILE));
		StringType sModName =  kConfig.GetSetting("AnimationName",CLIENT_MAP_CONFIG_SECTION_ACTOR,"");
		float fAngle;
		fAngle = -135.0f;
		NiMatrix3 kRotation;
		kRotation.MakeZRotation( (fAngle /180.0f *NI_PI));

		pkActor->SetRotate(kRotation);
		pkActor->PlayAnimation(sModName,true);
	}
#endif
}
//---------------------------------------------------------------------------
void LC_ClientLogicManager::_event_InputKey2( MG_MessageBase& rkMessage )
{
#ifdef _MAP_VIEWER_BUILD
	static int              siMonsterAdd = 1;
	UT_ConfigFile kConfig;
	kConfig.Load(GfxMakeFullPath(CLIENT_PREVIEW_MAP_CONFIG_FILE));
	
	StringType sCharType = kConfig.GetSetting("CharType",CLIENT_MAP_CONFIG_SECTION_ACTOR,"");
	StringType SecondNum(sCharType[1]);//取第二位的数字

	int nCharType = UT_EngineStringUtility::GetIntFromString(kConfig,CLIENT_MAP_CONFIG_SECTION_ACTOR,"CharType",1);
	LC_ClientGamePlayer*    pkGamePlayer = (LC_ClientGamePlayer*)m_pkGamePlayerManager->FindPlayer(m_lOperationPlayerID);

	if (pkGamePlayer)
	{
		UT_Vec3Int kCurrentLocation = pkGamePlayer->GetCurrentLocation();
		if(SecondNum != "" ) // monster
		{
// 			// 模拟创建怪物
// 			MG_PACKET_ENCODE_HANDLER hMsgHandler;
// 			bool bCreateMsg = false;
// 			MG_SyncMonstersInfoInFOV kMsg;
// 			kMsg.m_lMonsterNum									= 1;
// 			kMsg.m_akMonsterInfoEntries[0].m_lID				= MONSTER_ID_MIN + siMonsterAdd;
// 			kMsg.m_akMonsterInfoEntries[0].m_lMonsterCharType   = nCharType;
// 			kMsg.m_akMonsterInfoEntries[0].m_iCurrentLocationX   = kCurrentLocation.x;
// 			kMsg.m_akMonsterInfoEntries[0].m_iCurrentLocationY   = kCurrentLocation.y;
// 			bCreateMsg = CreateMsgPacketHandler(hMsgHandler,MGPT_SYNC_MONSTERS_INFO_IN_FOV, &kMsg);
// 			if(bCreateMsg)
// 			{
// 				pkGamePlayer->GetSession()->OnRecv( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
// 			}
// 			m_iCreatedViewActorID =  MONSTER_ID_MIN + siMonsterAdd;
// 			++siMonsterAdd;
// 		}
// 		else
// 		{
			MG_PACKET_ENCODE_HANDLER hMsgHandler;
			bool bCreateMsg = false;
			MG_SyncNPCsInfoInFOV  kMsg;
			kMsg.m_lNPCNum								= 1;
			kMsg.m_akNPCInfoEntries[0].m_lID			= NPC_ID_MIN + siMonsterAdd;
			kMsg.m_akNPCInfoEntries[0].m_lNPCCharType   = nCharType;
			kMsg.m_akNPCInfoEntries[0].m_iFactionType	= 1;
			kMsg.m_akNPCInfoEntries[0].m_iCurrentLocationX   = kCurrentLocation.x;
			kMsg.m_akNPCInfoEntries[0].m_iCurrentLocationY   = kCurrentLocation.y;
			kMsg.m_akNPCInfoEntries[0].m_iCurrentLocationZ	 = kCurrentLocation.z;
			
			m_iCreatedViewActorID =  NPC_ID_MIN + siMonsterAdd;
			bCreateMsg = CreateSCMsgPacketHandler(hMsgHandler, MGPT_SYNC_NPCS_INFO_IN_FOV, &kMsg);
			if(bCreateMsg)
			{
				pkGamePlayer->GetSession()->OnRecv( hMsgHandler.GetBuff(), hMsgHandler.GetMsgSize());
			}
			++siMonsterAdd;
		}

	}
#endif
}
//--------------------------------------------------------
//查找主玩家，网络玩家
LC_PlayerBase*   LC_ClientLogicManager::FindPlayer(object_id_type lPlayerID)
{
	LC_PlayerBase* pkPlayerBase = NULL;

	if(m_pkGamePlayerManager)
	{
		pkPlayerBase = static_cast<LC_PlayerBase*>(m_pkGamePlayerManager->FindPlayer(lPlayerID));

		if(pkPlayerBase)
		{
			return pkPlayerBase;
		}
	}

	if(m_pkNetPlayerManager)
	{
		pkPlayerBase = static_cast<LC_PlayerBase*>(m_pkNetPlayerManager->FindPlayer(lPlayerID));

		if(pkPlayerBase)
		{
			return pkPlayerBase;
		}
	}

	return NULL;
}
//--------------------------------------------------------
void LC_ClientLogicManager::UpdatePlayerFightState(LC_PlayerBase* pkPlayer)
{
	LC_PKRecorderBase* pkRecorder = pkPlayer->GetPKRecorder();
	if (NULL == pkRecorder)
	{
		return;
	}

	//更新战斗状态
	bool bFightState = pkRecorder->GetFightState();
	pkPlayer->SetCurrentFightState(bFightState);
}
//--------------------------------------------------------
void LC_ClientLogicManager::UpdateExtraClothing(LC_PlayerBase* pkPlayer, long lCurrentHeadEquipID, long lCurrentBodyEquipID, long lCurrentFootEquipID)
{
	if (NULL == pkPlayer)
	{
		return;
	}
	
	StringType kSuiteExtraCloth = LC_Helper::GetSuiteExtraCloth(lCurrentHeadEquipID, lCurrentBodyEquipID, lCurrentFootEquipID, pkPlayer->GetCharType());
	GD_ClientActorModel* pkVisibleData = IM_Manager::GetSingleton().GetActor(pkPlayer->GetID());
	if(pkVisibleData)
	{
		pkVisibleData->ChangeExtraClothing(kSuiteExtraCloth);
	}
}
//--------------------------------------------------------
void LC_ClientLogicManager::FreezeAnimation(LC_ActorBase* pkActor)
{
	if (NULL == pkActor || (!IS_PLAYER_CLASS_ID(pkActor->GetID()) && !IS_NPC_ID(pkActor->GetID())))
	{
		return;
	}

	//已经冻结
	if (pkActor->GetAnimationFreezed())
	{
		return;
	}

	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	if (NULL == pkIMManager)
	{
		return;
	}

	//冻结角色
	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(pkActor->GetID());
	if (NULL != pkVisibleData)
	{
		pkVisibleData->FreezeAnimation();
		pkActor->SetAnimationFreezed(true);
	}
	
	//冻结坐骑
	if (pkActor->GetRiderCharType() > 0)
	{
		pkVisibleData = pkIMManager->GetRiderActor(pkActor->GetID());
		if (NULL != pkVisibleData)
		{
			pkVisibleData->FreezeAnimation();
		}
	}
}
//--------------------------------------------------------
void LC_ClientLogicManager::UnFreezeAnimation(LC_ActorBase* pkActor)
{
	if (NULL == pkActor || (!IS_PLAYER_CLASS_ID(pkActor->GetID()) && !IS_NPC_ID(pkActor->GetID())))
	{
		return;
	}

	//没有冻结
	if (!pkActor->GetAnimationFreezed())
	{
		return;
	}

	IM_Manager* pkIMManager = IM_Manager::GetSingletonPtr();
	if (NULL == pkIMManager)
	{
		return;
	}

	//解冻角色
	GD_ClientActorModel* pkVisibleData = pkIMManager->GetActor(pkActor->GetID());
	if (NULL != pkVisibleData)
	{
		pkVisibleData->UnfreezeAnimation();
		pkActor->SetAnimationFreezed(false);
	}

	//解冻坐骑
	if (pkActor->GetRiderCharType() > 0)
	{
		pkVisibleData = pkIMManager->GetRiderActor(pkActor->GetID());
		if (NULL != pkVisibleData)
		{
			pkVisibleData->UnfreezeAnimation();
		}
	}
}
//--------------------------------------------------------


int LC_ClientLogicManager::GetFunctionNPCObjectType(LC_ClientNPC* pkNPC)
{
	long lObjectType = -1;
	if (!pkNPC)
		return lObjectType;

	long lState = pkNPC->GetNPCFuncState(NFT_TASK);
	if ( lState == NFST_TASK_CANACCEPT )  //可接任务
	{
		lObjectType = WOT_CAN_ACCEPT_TASK;
	}
	else if ( lState == NFST_TASK_CANFINISH )	//可交任务
	{
		lObjectType = WOT_CAN_FINISH_TASK;
	}
	else if	( lState == NFST_TASK_PROCESSING) //进行中的任务
	{
		lObjectType = WOT_TASK_PROCESSING;
	}
	else if (lState == NFST_COLSE)	//没有任务挂接
	{
		if (pkNPC->GetNPCFuncState(NFT_SHOP) > 0)
			lObjectType = WOT_SHOP;
		else if (pkNPC->GetNPCFuncState(NFT_WAREHOUSE) > 0)
			lObjectType = WOT_WAREHOUSE;
		else if (pkNPC->GetNPCFuncState(NFT_PRODUCTION) > 0)
			lObjectType = WOT_PRODUCTION;
		else if (pkNPC->GetNPCFuncState(NFT_GEMSTONE) > 0)
			lObjectType = WOT_GEMSTONE;
		else if (pkNPC->GetNPCFuncState(NFT_DRUGS_SHOP) > 0)
			lObjectType = WOT_DRUGS_SHOP;
		else if (pkNPC->GetNPCFuncState(NFT_PVP_EXCHANGE) > 0)
			lObjectType = WOT_PVP_EXCHANHR;
		else if (pkNPC->GetNPCFuncState(NFT_PORTAL) > 0)
			lObjectType = WOT_PORTAL;
		else if (pkNPC->GetNPCFuncState(NFT_SIEGE_ENGINES) > 0)
			lObjectType = WOT_SIEGE_ENGINES;
	}

	return lObjectType;
}
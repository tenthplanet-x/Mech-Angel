#include "LC_ClientGameEvent.h"
#include "SC_ClientScriptInterface.h"

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerNotifyDungeonStage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NOTIFY_TRANSCRIPTION_PROGRESS rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SHOW_DUNGEON_STAGE);
	kGameEvent.SetEventParam(EP_SHOW_DUNGEONSTAGE_UI_SHOW, 1);
	kGameEvent.SetEventParam(EP_SHOW_DUNGEONSTAGE_TOTAL_STEP, rltMsg.m_lTotalStep);
	kGameEvent.SetEventParam(EP_SHOW_DUNGEONSTAGE_CUR_STEP, rltMsg.m_lCurStep);
	kGameEvent.SetEventParam(EP_SHOW_DUNGEONSTAGE_FINISH_STATE, rltMsg.m_Finish);
	kGameEvent.SetEventParam(EP_SHOW_DUNGEONSTAGE_USER_DATA, rltMsg.m_UserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncRaidAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RaidAsset rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetRaidAsset()->GetDataFromProtocolStruct(rltMsg.m_kAssetData);
}
//------------------------------------------------------------------------
void HL_FromGameServer::_handlerSyncTransportAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Transport_Asset rltMsg;
	if(!rltMsg.Decode(pMsg,wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return;
	}
	pkGamePlayer->GetTransportAsset()->GetDataFromProtocolStruct(rltMsg.m_kAssetData);
}

//------------------------------------------------------------------------
void HL_FromGameServer::_handleSyncRaidSettle(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_RaidSettle rltMsg;
	if (!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(NULL == pkGamePlayer)
		return;
	
	pkGamePlayer ->PlaySuccessVoice();

	LC_ClientGameEvent kGameEvent;

	LC_ClientGameStoryManager* pkGameStoryManager = CLIENT_GET_GAMESTORY_MANAGER();
	uint16_t storyID = 0;
	uint32_t iMapResID = pkGamePlayer->GetMapResID();
	if (pkGameStoryManager)
	{
		storyID = pkGameStoryManager->GetActiveStoryID(iMapResID);
	}
	if (storyID == 4)  // 天气之门1
		kGameEvent.SetClientEventType(CGET_LANJIETIANYINGJIAO_SETTLE);
	else if(storyID == 5)  // 首领试炼
		kGameEvent.SetClientEventType(CGET_RAID_FAMOUS_SETTLE); 
	else if(LC_Helper::IsCrossRealmMap(iMapResID))  // 组队副本
		kGameEvent.SetClientEventType(CGET_CROSS_REALM_MAP_SETTLE);
	else if((storyID >= 38 && storyID <= 44) || (storyID >= 101 && storyID <= 110))     //进阶副本
		kGameEvent.SetClientEventType(CGET_RAID_JINJIE_SETTLE);
	else if(storyID == 212)     //荣光远征副本
		kGameEvent.SetClientEventType(CGET_RAID_GLORY_SETTLE);
	else
		kGameEvent.SetClientEventType(CGET_RAID_SETTLE);

	GfxWriteLog(LOG_TAG_GAME_LOG, LOG_SWITCH_GAME,"[RaidMap]HL_FromGameServer::_handleSyncRaidSettle storyID=%d iMapResID=%d", storyID, iMapResID);

	LC_ClientGameEventUserData_RaidSettle kUserData(rltMsg,storyID,iMapResID);
	kGameEvent.SetUserData(&kUserData);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

	LC_ClientPlaceHolderManager* pkPlaceManager = CLIENT_GET_PLACEHOLDER_MANAGER();
	if ( pkPlaceManager )
	{	
		int mapID = pkGamePlayer->GetCurrentMapID();
		CF_TranscriptionRule* pkTranscriptionRule = CF_TranscriptionRule::GetSingletonPtr();
		CF_TranscriptionRule::DataEntry* pkData=pkTranscriptionRule->GetEntryPtr(mapID);
		if(pkData)
		{
			int itime = pkData->_CountDownTime;
			pkPlaceManager->SetRaidSettleMaxDelayTime(itime - GetGlobalSetting.RaidSettleTime);
		}
		pkPlaceManager->SetRaidSettle(true);
	}
	MG_MessageCenter::GetSingleton().SendMessage( CreateMsg_UICutScenes_Condition(CUCS_TYPE_Raid, pkGamePlayer->GetCurrentMapID(), CUCS_RAID_BeforeQuit) );
}

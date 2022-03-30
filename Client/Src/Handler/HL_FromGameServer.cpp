#include "ClientPCH.h"
#include "HL_FromGameServer.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientGamePlayerManager.h"
#include "LC_ClientNPC.h"
#include "LC_ClientNPCManager.h"
#include "LC_ClientNetPlayer.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientPlaceHolder.h"
#include "LC_ClientPlaceHolderManager.h"
#include "LC_ClientTaskMap.h"
#include "LC_ClientTaskManager.h"
#include "LC_ClientPlayerGroupManager.h"
#include "LC_ClientPlayerGroup.h"
#include "LC_PlayerFlopManager.h"
#include "LC_ClientMap.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientLogicManager.h"
#include "SK_ClientSkillEventManager.h"
#include "AS_ClientSoundManager.h"
//#include "LC_ClientExchangeManager.h"
#include "LC_ClientArenaManager.h"
#include "LC_NoticeAnalysisMgr.h"
#include "LC_ClientGuildProfitManager.h"
#include "LC_ClientArenaPlayBackManager.h"
#include "IM_Manager.h"
#include "CF_TranscriptionRule.h"


#include "CA_CameraPathAnimation.h"

#include "SE_SceneManager.h"
#include "SM_GameStageMachine.h"

#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"

#include "TT_TextManager.h"

#include "UT_LogicInfoMessage.h"
#include "MG_GameProtocol.h"
#include "MG_Forbid.h"
#include "MG_VehicleProtocol.h"
#include "LC_PlayerHatred.h"

#include "NW_ClientSessionManager.h"
#include "NW_Login2ClientSession.h"
#include "NW_Game2ClientSession.h"
#include "NW_Helper.h"

#include "UI_UIManager.h"
#include "LC_ClientTask.h"

#include "MG_ShortcutKeyProtocol.h"

#include "LC_ArenaAsset.h"

#include "GD_ActorModel.h"

#include "MG_PB.h"
#include "msg.pb.h"

extern "C"
{
	#include <Lzma/Include/Types.h>
	#include <Lzma/Include/Lzma86Dec.h>
    #include <zlib.h>
};


using namespace Camera;
using namespace CSVFile;
using namespace GameLogic;
using namespace Path;
using namespace Protocol;
using namespace StateMachine;
using namespace Utility;
using namespace Skill;
using namespace Data;



////进入心法副本前保存一个特定的效果设置值
//float fShadow_Light_Offeet_Value = 0.0f;


#include "HL_FromGameServer_LoginProtocol.inl"
#include "HL_FromGameServer_GameProtocol.inl"
#include "HL_FromGameServer_SyncProtocol.inl"
#include "HL_FromGameServer_GroupProtocol.inl"
#include "HL_FromGameServer_ShopCityProtocol.inl"
#include "HL_FromGameServer_SkillProtocol.inl"
#include "HL_FromGameServer_TaskProtocol.inl"
#include "HL_FromGameServer_TradeProtocol.inl"
#include "HL_FromGameServer_ChatProtocol.inl"
#include "HL_FromGameServer_ChestProtocol.inl"
#include "HL_FromGameServer_DuelProtocol.inl"
#include "HL_FromGameServer_FrobidProtocol.inl"
#include "HL_FromGameServer_VehicleProtocol.inl"
#include "HL_FromGameServer_MailProtocol.inl"
#include "HL_FromGameServer_AuctionProtocol.inl"
#include "HL_FromGameServer_FriendProtocol.inl"
#include "HL_FromGameServer_SearchProtocol.inl"
#include "HL_FromGameServer_TalentProtocol.inl"
#include "HL_FromGameServer_Profile.inl"
#include "HL_FromGameServer_DebugProtocol.inl"
#include "HL_FromGameServer_ArenaProtocol.inl"
#include "HL_FromGameServer_GuildProtocol.inl"
#include "HL_FromGameServer_CurrencyTradeProtocol.inl"
#include "HL_FromGameServer_GuildHomeProtocol.inl"
#include "HL_FromGameServer_QuanThanhTemple.inl"
#include "HL_FromGameServer_GiftProtocol.inl"
#include "HL_FromGameServer_DungeonStage.inl"
#include "HL_FromGameServer_AssistProtocol.inl"
#include "HL_FromGameServer_BuddyProtocol.inl"
//------------------------------------------------------------------------

static std::string uncompressedMsg;

void HL_FromGameServer::ParsePacket(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	T_PROFILE("ParsePacket::HL_FromGameServer");
	MG_PACKET_DECODE_HANDLER hMsgHandler;

	CS_MG_HEAD kMsgHead;
	if (!hMsgHandler.DecodeCSMsg(pMsg, wSize, kMsgHead))
	{
		Utility::GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR,"FromGameServer hMsgHandler.DecodeCSMsg Fail");
		return;
	}

	if(kMsgHead.m_usMsgType==MGPT_GUILD_FIGHT_TOWER_INFO_NOTIFY)
	{
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"_handlerGuildBattleTowerInfo");
	}

	if (kMsgHead.GetCompressAlgorithm() > 0)
	{
		/*size_t tUnCompressed = 0;
		if (SZ_OK != Lzma86_GetUnpackSize((const Byte*)pMsg, wSize, (UInt64*)&tUnCompressed))
		{
			return;
		}

		char* pUnCompressedBuff = T_ALLOC(char, tUnCompressed, Memory::MEMCATEGORY_GENERAL);
		if (NULL == pUnCompressedBuff)
		{
			T_FREE(pUnCompressedBuff,  Memory::MEMCATEGORY_GENERAL);
			return;
		}

		size_t tCompressed = wSize;
		if (SZ_OK != Lzma86_Decode((Byte*)pUnCompressedBuff, (SizeT *)&tUnCompressed, (const Byte*)pMsg, (SizeT *)&tCompressed))
		{
			T_FREE(pUnCompressedBuff,  Memory::MEMCATEGORY_GENERAL);
			return;
		}*/

//         size_t tUnCompressed = 1024 * 64;
//         char* pUnCompressedBuff = T_ALLOC(char, tUnCompressed, Memory::MEMCATEGORY_GENERAL);
// 		if (NULL == pUnCompressedBuff)
// 		{
// 			T_FREE(pUnCompressedBuff,  Memory::MEMCATEGORY_GENERAL);
// 			return;
// 		}
//         size_t tCompressed = wSize;
//         if (Z_OK != uncompress((Bytef*)pUnCompressedBuff, (uLongf*)&tUnCompressed, (Bytef*)pMsg, tCompressed))
// 		{
// 			T_FREE(pUnCompressedBuff,  Memory::MEMCATEGORY_GENERAL);
// 			return;
// 		}
// 		
// 		pMsg = pUnCompressedBuff;
// 		wSize = (uint16_t)tUnCompressed;
// 		bCompressed = true;
		if(!Protocol::Uncompress(kMsgHead.GetCompressAlgorithm(), pMsg, wSize, uncompressedMsg))
		{
			Utility::GfxWriteLog(LOG_TAG_ERROR, LOG_SWITCH_ERROR,"FromGameServer Protocol::Uncompress Fail");
			return;
		}
		pMsg = (char*)uncompressedMsg.c_str();
		wSize = (uint16_t)uncompressedMsg.size();
	}

	if ( (kMsgHead.m_usMsgType >= MGPT_BEGIN_LOGIN_MESSAGE && kMsgHead.m_usMsgType <= MGPT_END_LOGIN_MESSAGE)
		|| kMsgHead.m_usMsgType == MGPT_RLT_TRANSFER
		)
		Utility::GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM,"[网络][收]FromGameServer :%d", kMsgHead.m_usMsgType);
	
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	if(!pkLogicManager)
		return;
	
	static mem::map<uint16_t, uint32_t> msgCount;
	mem::map<uint16_t, uint32_t>::iterator it = msgCount.find(kMsgHead.m_usMsgType);
	if ( it == msgCount.end() )
		msgCount.insert( make_pair( kMsgHead.m_usMsgType, 1 ) );
	else
		it->second++;

	//消息量统计代码
	/*static mem::map<uint16_t, uint64_t> msgSize;
	mem::map<uint16_t, uint64_t>::iterator it2 = msgSize.find(kMsgHead.m_usMsgType);
	if ( it2 == msgSize.end() )
		msgSize.insert( make_pair( kMsgHead.m_usMsgType, 1 ) );
	else
		it2->second+=wSize;*/

	//战斗回放 录制
	/*if (LC_ClientLogicManager::m_fBeginRecord > 0 && _CheckArenaRecordByType(kMsgHead.m_usMsgType))
	{
		float deltime = GET_PROCESS_TIME - LC_ClientLogicManager::m_fBeginRecord;
		LC_ClientArenaPlayBackManager *pkArenaPlayBackManager = LC_ClientArenaPlayBackManager::GetSingletonPtr();
		pkArenaPlayBackManager->Packet(kMsgHead.m_usMsgType, pMsg, wSize, deltime);

		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI, "战斗回放录制  usMsgType=%d wSize=%d", kMsgHead.m_usMsgType, wSize);
	}*/

	if(GameLogic::LC_Helper::_CheckArenaRecordByType(kMsgHead.m_usMsgType))
	{
		float nowTime = GET_PROCESS_TIME ;
		GameLogic::LC_Helper::clientMakeTranscribeFightMsg(kMsgHead.m_usMsgType, pMsg, wSize, 0,nowTime);
	}
	switch (kMsgHead.m_usMsgType)
	{
	case MGPT_SIGNATURE_PASS:
		_handlerSignaturePass(pkSession,pMsg,wSize);
		break;
	case MGPT_SYSTEM_ANNOUNCE:
		_handlerSystemAnnounce(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_MSG:
		_handlerSystemNotifyMsg(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_YIZHONGQINXI_NOTICE:
		_handlerNotifyYiZhongQinXiNotice(pkSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_CREATE_CHAR:
		_handlerRltCreateChar(pkSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_DELETE_CHAR:
		_handlerRltDeleteChar(pkSession,pMsg,wSize);
		break;
	case MGPT_LC_RLT_RECOVER_CHAR:
		_handlerRltRecoverChar(pkSession,pMsg,wSize);
		break;
	case MGPT_GC_RLT_LOGIN:
		_handlerRltLogin(pkSession,pMsg,wSize);
		break;
	case MGPT_GC_RLT_ENTER_GAME:
		_handlerRltEnterGame(pkSession,pMsg,wSize);
		break;
	case MGPT_GC_RLT_LEAVE_GAME:
		_handlerRltLeaveGame(pkSession,pMsg,wSize);
		break;
	case MGPT_GC_RLT_RESET_GAME:
		_handlerRltResetGame(pkSession,pMsg,wSize);
		break;
	//case MGPT_GC_RLT_JUMPER:
	//	_handlerRltJumpGame(pkSession,pMsg,wSize);
	//	break;
	case MGPT_GC_RLT_WAITING_INFO:
		_handlerRltWaiting(pkSession,pMsg,wSize);
		break;

	case MGPT_RLT_SKILL_UPDATE:
		_handlerRltUpdateSkill(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_LEARNCONTROLLERSKILL:
		_handlerRltLearnControllerSkill(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_RESPAWN:
		_handlerRltRespawn(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NETPLAYER_EQUIP_INFO:
		_handlerRltNetPlayerEquipInfo(pkSession,pMsg,wSize);
		break;
	//case MGPT_RLT_NETPLAYER_ATTRIBUTE:
	//	_handlerRltNetPlayerAttribute(pkSession,pMsg,wSize);
	//	break;

	case MGPT_SC_GetSkyArena_BattleInfo:
		_handlerRltGetSkyArenaBattleInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_NOTIFY_ArenaTimeData:
		_handlerNotifySkyArenaTimeData(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SKYARENA_CHECKBATTLESTAGE_RESP:
		_handlerResp_SkyArenaCheckBattleStage(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_SKYARENA_GETHONOR:
		_handlerNotifySkyArenaGetHonor(pkSession,pMsg,wSize);
		break;

	//是否把当前攻击目标添加到仇人列表
	case MGPT_NOTIFY_ADDTOENEMY:
		_handlerNotifyAddtoenermy(pkSession, pMsg, wSize);
		break;

	case MGPT_SYNC_RAIDENTERMAPRECORD:
		_handlerSyncRaidEnterMapRecord(pkSession, pMsg, wSize);
		break;

	case MGPT_SYNC_IDASSET:
		_handlerSyncIDAsset(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_ADDID:
		_handlerRltAddID(pkSession, pMsg, wSize);
		break;

	case MGPT_RLT_TRANSFER:
		_handlerRltTransfer(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_TRANSFER_TO_LOCATION:
		_handlerRltTransferToLocation(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ENTER_TRANSCRIPTION:
		_handlerRltEnterTranscription(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_QUIT_TRANSCRIPTION:
		_handlerRltQuitTranscription(pkSession,pMsg,wSize);
		break;
	case MG_RLT_RELIVE_BEFOREENTERMAP:
		_handlerRltReliveTranscription(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_CLIENT_CRITICAL_TIPS:
		_handlerReceiveTranscriptionTipsInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_CLIENT_TRANSCRIPTION_PER_DAY:
		_handlerPerDayTranscriptionInfoFromServer(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_UPGRADE:
		_handlerRltNPCUpdate(pkSession,pMsg,wSize);
		break;
	case MGPT_REFRESH_NPC_LEVEL:
		_handlerRefreshNPCLevel(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_CAREER_LEARN:
		_handlerRltQuitCareerSelect(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_CLIENT_SHOW_STORY_TIPS:
		_handlerStoryTipsInfoFromServer(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_CLIENT_NPC_EXECUTE_SKILL:
		_handlerNPCExecuteSkillFromServer(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SEEKTREASURE:
		_handlerReceiveSeekTreasureFromServer(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SET_TRANSFER_POS:
		_handlerRltSetTransferPos(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_TITLE_NETPLAYER_INFO:
		_handlerSyncNetPlayerTitle(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SET_TITLE:
		_handlerRltSetTitle(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ADD_DIY_TITLE:
		_handlerRltAddDIYTitle(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_REMOVE_DIY_TITLE:
		_handlerRltRemoveDIYTitle(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_MODIFY_DIY_TITLE:
		_handlerRltModifyDIYTitle(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_OWNERSHIP:
		_handlerRltNPCOwnerShip(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ANTI_WALLOW:
		_handlerRltAntiWallow(pkSession,pMsg,wSize);
		break;

	case MG_RLT_RESET_SHOPTIMES:
		_handlerRltResetShopTimes(pkSession,pMsg,wSize);
		break;
	case MQ_RLT_GET_SHOPRESETTIMES:
		_handlerRltGetShopResetTimes(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GET_SHOPITEM:
		_handlerRltGetShopItem(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_BUYINVESMENTCARD:
		_handlerRltBuyInvesmentCard(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ADDINVESTMENTCARDREWARD:
		_handlerRltAddInvesmentCardReward(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_REFRESHREWARDTASK:
		_handlerRltRefreshRewardTask(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_FOLLOW_PLAYER:
		_handlerRltFollowPlayer(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GM_TRANSFER:
		_handlerRltGMTransfer(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_LEARNPRODUCTION:
		_handlerRltLearnProduction(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_FORGETPRODUCTION:
		_handlerRltForgetProduction(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_UPGRADEPRODUCTION:
		_handlerRltUpgradeProduction(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_LEARNFORMULA:
		_handlerRltLearnFormula(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ACCEPT_TASK:
		_handlerRltAcceptTask(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_FINISH_TASK:
		_handlerRltFinishTask(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_CANCEL_TASK:
		_handlerRltCancelTask(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SHARE_TASK:
		_handlerRltShareTask(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_CONFIRM_SHARE_TASK:
		_handlerRltConfirmShareTask(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_BILLBOARD_TASK_INFO:
		_handlerRltBillBoardTaskInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_REQ_SHARE_MULTIUSER_TASK:
		_handlerReqShareMultiuserTask(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_JOIN_MULTIUSER_TASK:
		_handlerRltJoinInMultiuserTask(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ASSIGN_TASK:
		_handlerRltTaskAssignInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_DRAW_LIVENESS_REWARD:
		_handlerRltDrawLivenessReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_LOCATION:
		_handlerNPCExecuteSkillTypeLocation(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_TARGET:
		_handlerNPCExecuteSkillTypeTarget(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_EXECUTE_SKILL_TYPE_DIR:
		_handlerNPCExecuteSkillTypeDir(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_INTERRUPT_SKILL:
		_handlerNPCInterruptSkill(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_TARGET:
		_handlerPlayerExecuteSkillTypeTarget(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_DIR:
		_handlerPlayerExecuteSkillTypeDir(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_PLAYER_EXECUTE_SKILL_TYPE_LOCATION:
		_handlerPlayerExecuteSkillTypeLocation(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_PLAYER_INTERRUPT_SKILL:
		_handlerPlayerInterruptSkill(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_NPC_DEAD:
		_handlerRltNPCDead(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_PLAYER_DEAD:
		_handlerRltPlayerDead(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SKILL_EVENT:
		_handlerRltSkillEvent(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SKILL_EVENT_EX:
		_handlerRltSkillEventEx(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCBULLETINFO:
		_handlerRltSkillEventMerged(pkSession, pMsg, wSize);
		break;
	case MGPT_REQ_SKILL_RESPAWN:
		_handlerReqSkillRespawn(pkSession,pMsg,wSize);
		break;
	case MGPT_RESPAWN_BROADCAST_LOCATION:
		_handlerRespawnBroadcastLocation(pkSession,pMsg,wSize);
		break;
	case MGPT_PUNCH_MOVE_PLAYER:
		_handlerPunchMovePlayer(pkSession,pMsg,wSize);
		break;
	case MGPT_PUNCH_MOVE_NPC:
		_handlerPunchMoveNpc(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_DEACTIVE_SKILL_STATE:
		_handlerRltDeactiveSkillState(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_TALKTONPC:
		_handlerRltTalkToNPC(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_REQUEST_INFO:
		_handlerRltRequestInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_PROPERTY:
		_handlerSyncPlayerProperty(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_SOCIETY:
		_handlerSyncPlayerSociety(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_ATTRIBUTE:
		_handlerSyncPlayerAttribute(pkSession,pMsg,wSize);
		break;
	//case MGPT_SYNC_GAME_PLAYER_INFO:
		//_handlerSyncGamePlayerInfo(pkSession,pMsg,wSize);
	//	break;

	case MGPT_SYNC_BASIC_PACK:
		_handlerSyncBasicBackPack(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_TASK_PACK:
		_handlerSyncTaskBackPack(pkSession,pMsg,wSize);
		break;

	//指挥官装备
	case MGPT_SC_SynCommandEquip_Ret:
		_handlerSyncCommandEquip(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_EquipCommand_Ret:
		_handlerSCEquipCommand_Ret(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_IntensifyCommandEquip_Ret:
		_handlerSCIntensifyCommandEquipRet(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_RefineCommandEquip_Ret:
		_handlerSCRefineCommandEquipRet(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SoulCommandEquip_Ret:
		_handlerSCSoulCommandEquipRet(pkSession,pMsg,wSize);
		break;

	//进阶线装备
	case MGPT_RLT_EQUIPSUBITEM:
		_handlerRltEquipSubItem(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_UNEQUIPSUBITEM:
		_handlerRltUnEquipSubItem(pkSession,pMsg,wSize);
		break;

	//英雄纹章
	case MGPT_SYNC_BUDDYEQUIPSLOTPACKDATA:
		_handlerSyncBuddyEquipSlotPackData(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_BUDDYEQUIPSLOTDATA:
		_handlerSyncBuddyEquipSlotData(pkSession,pMsg,wSize);
		break;

	case MGPT_SYNC_RUNE_EQUIP_PACK:
		_handlerSyncRuneBackPack(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCREDPOINTFLAG:
		_handlerSyncRedPointFlag(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_NEW_ITEM_NOTIFY:
		_handlerNewItemNotify(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_ADDRUNE:
		_handlerSyncFirstAddRune(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_RUNERECORD:
		_handlerSyncRuneRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_RUNE_REWARD:
		_handlerRespRuneReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_OPENBAG:
		_handlerSyncOpenBagAsset(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_WAREHOUSE_PACK:
		_handlerSyncWarehouseBackPack(pkSession,pMsg,wSize);
		break;

	
	case MGPT_SYNC_PROCESSING_TASK:
		_handlerSyncProcessingTask(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_FINISHED_TASK:
		_handlerSyncFinishedTask(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_FAIL_TASK_INFO:
		_handlerSyncFailTask(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCLOTTERYRECORD:
		_handlerSyncLotteryRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SyncLottery_Ret:
		_handlerSyncLotteryRet(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_Lottery_Draw_Ret:
		_handlerLotteryDrawRet(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_Lottery_ExtraReward_Ret:
		_handlerLotteryExtranRewardRet(pkSession,pMsg,wSize);
		break;

	case MGPT_SYNC_USEITEMUPGRADESUB:
		_handlerSyncUseItemUpgradeSub(pkSession,pMsg,wSize);
		break;

	case MGPT_SC_SynChargeProfit_Ret:
		_handlerSyncChargeProfit(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_ChargeProfit_Reward_Ret:
		_handlerChargeProfit_RewardRet(pkSession,pMsg,wSize);
		break;


		//每日相关Begin
	case MGPT_RLT_TASK_ONE_KEY_OP:
		_handlerRltTaskOneKeyOp(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_TASK_ASSIGN_ONE_KEY_OP:
		_handlerRltTaskAssignOneKeyOp(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_TASK_PHAGOTROPHY_RESP:
		_handlerRltTaskPhagotrophy(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_REDUCE_TASK_DIFF_STAR:
		_handlerRltReduceTaskDiffStar(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_PROMOTE_TASK_REWARD_STAR:
		_handlerRltPromoteTaskRewardStar(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_TASK_DRAW_ROUND_OFF_REWARD:
		_handlerRltTaskDrawRoundOffReward(pkSession,pMsg,wSize);
		break;
		//每日相关End
	case MGPT_SYNC_SKILL_BACKPACK:
		_handlerSyncSkillBackPack(pkSession,pMsg,wSize);
		break;
	//case MGPT_SYNC_NPCS_INFO_IN_FOV:
	//	_handlerSyncNPCInfoInFOV(pkSession,pMsg,wSize);
	//	break;
	case MGPT_SYNC_NPCS_MOVE_IN_FOV:
		_handlerSyncNPCMoveInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_STATE_IN_FOV:
		_handlerSyncNPCStateFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_ANGLE_IN_FOV:
		_handlerSyncNPCAngleFOV(pkSession,pMsg,wSize);
		break;

	case MGPT_SYNC_NPCS_PROPERT_IN_FOV:
		_handlerSyncNPCPropertyInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_SKILL_STATE_IN_FOV:
		_handlerSyncNPCSkillStateInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_INTERACTION_IN_FOV:
		_handlerSyncNPCInteractionInFOV(pkSession,pMsg,wSize);
		break;
	/*case MGPT_SYNC_NPCS_FACTION_IN_FOV:
		_handlerSyncNPCFactionInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYERS_FACTION_IN_FOV:
		_handlerSyncPlayerFactionInFOV(pkSession,pMsg,wSize);
		break;*/

	/*case MGPT_SYNC_NPCS_INFO_IN_FOV:
		_handlerSyncNPCInfoInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_MOVE_IN_FOV:
		_handlerSyncNPCMoveInFOV(pkSession,pMsg,wSize);
		break;*/
	//case MGPT_SYNC_PLAYERS_INFO_IN_FOV:
	//	_handlerSyncPlayerInfoInFOV(pkSession,pMsg,wSize);
	//	break;
	case MGPT_SYNC_PLAYERS_PROPERTY_IN_FOV:
		_handlerSyncPlayerPropertyInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYERS_PROPERTY_IN_FOV_FAST:
		_handlerSyncPlayerFastPropertyInFOV(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PLAYERS_PROPERTY_IN_FOV_EXT:
		_handlerSyncPlayerExtPropertyInFOV(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PLAYERS_SOCIETY_IN_FOV:
		_handlerSyncPlayerSocietyInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYERS_EQUIP_IN_FOV:
		_handlerSyncPlayerEquipInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYERS_SKILL_STATE_IN_FOV:
		_handlerSyncPlayerSkillStateInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_SUB_INFO_IN_FOV:
		_handlerSyncPlayerSubordinateInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_CHEST_IN_FOV:
		_handlerSyncChestList(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_CHEST_STATE_IN_FOV:
		_handlerSyncChestStateList(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_CASH:
		_handlerSyncPlayerCash(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_YUANBAO:
		_handlerSyncPlayerYuanBao(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_CUSTOM_CASH:
		_handlerSyncPlayerCustomCash(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCLOTTERY_CONSUMEUNBINDYUANBAO:
		_handlerSyncLotteryConsumeUnbindYuanbao(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SHOP_CITY_VIP_OBTAINED_ITEM:
		//-------------------------------------------
		break;
	case MGPT_SYNC_PLAYER_GROUP_MEMBER_LIST:
		_handlerSyncPlayerGroupMemberList(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_GROUP_CANDIDATE_LIST:
		_handlerSyncPlayerGroupCandidateList(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_TRADE_PACK:
		_handlerSyncTradePack(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYERS_SUMMOR_FRIEND_INFO:
		_handlerSyncSummorFriend(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PK_STATE:
		_handlerSyncPKState(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GET_PK_VALUE:
		_handlerSyncPKValue(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_FIGHT_STATE:
		_handlerSyncFightState(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SHORTCUTBAR_INFO:
		_handlerSyncShortcutBar(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SKILL_STATE:
		_handlerSyncSkillState(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_GAME_PLAYER_MOVE:
		_handlerSyncGamePlayerMove(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_PLAYER_JUMP:
		_handlerSyncGamePlayerJump(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_AICONTROL:
		_handlerSyncGamePlayerAIControl(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_AICONTROL_LOCATION:
		_handlerSyncGamePlayerAIControlLocation(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_TITLE_PLAYER_INFO:
		_handlerSyncTitleInfo(pkSession,pMsg,wSize);
		break;
	//case MGPT_SYNC_PRODUCTION_INFO:
	//	_handlerSyncProductionInfo(pkSession,pMsg,wSize);
	//	break;
	case MGPT_SYNC_SKILL_CD:
		_handlerSyncSkillCD(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_SKILL_CD_MODIFY:
		_handlerSyncSkillCDModify(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_ITEM_CD:
		_handlerSyncItemCD(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_TUTORIAL_ASSET:
		_handlerSyncTutorialAsset(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_EXECUTE_SKILL_TYPE_TARGET:
		_handlerSyncPlayerExecuteSkillTypeTarget(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_ENTER_IN_FOV:
		_handlerSyncNpcsEnter(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NPCS_LEAVE_IN_FOV:
		_handlerSyncNpcsLeave(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SHORTCUT_KEY_ASSET:
		_handlerSyncShortcutKey(pkSession,pMsg,wSize);
		break;
	/*case MGPT_SYNC_PLAYER_FACTION:
		_handlerSyncPlayerFaction(pkSession,pMsg,wSize);
		break;*/
	case MGPT_SYNC_ACTION_FORBID_INFO:
		_handlerSyncActionForbidType(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_ARENA_INFO:
		_handlerSyncArenaInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_ACTOR_OPERATION:
		_handlerSyncActorOperationAsset(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_DEFAULT_TRANSFER_POS:
		_handlerSyncDefaultTransferPos(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_USE_ITEM_IN_PACKAGE:
	case MGPT_RLT_PACK_SWITCH_ITEM:
	case MGPT_RLT_PACK_SPLIT_ITEM:
	case MGPT_RLT_TIDY_ITEM_IN_PACKAGE:
	case MGPT_RLT_MERGE_ITEM_IN_PACKAGE:
	case MGPT_RLT_NPCSHOP_BUY:
	case MGPT_RLT_DESTROY_ITEM_IN_PACKAGE:
	case MGPT_RLT_IDENTIFY_ITEM_IN_PACKAGE:
	case MGPT_RLT_EQUIP_VEHICLE_EQCMENT:
	case MGPT_RLT_UNEQUIP_VEHICLE_EQCMENT:
		_handlerItemProtocol(pkSession, kMsgHead.m_usMsgType, pMsg, wSize);
		break;


	case MGPT_RLT_FIX_ITEM_IN_PACKAGE:
		_handlerFixItemProtocol(pkSession, kMsgHead.m_usMsgType, pMsg, wSize);
		break;
	case MGPT_RLT_DO_EXCHANGE:
		_handlerRlExchange(pkSession, kMsgHead.m_usMsgType, pMsg, wSize);
		break;
	case MGPT_RLT_LEAVE_RRISON:
		_handlerRltLeavePrison(pkSession, kMsgHead.m_usMsgType, pMsg, wSize);
		break;
	case MGPT_RLT_IMPRISON:
		_handlerRltImprisoned(pkSession, kMsgHead.m_usMsgType, pMsg, wSize);
		break;
	case MGPT_RLT_REDUCE_PKVALUE:
		_handlerRltReducePkValue(pkSession, kMsgHead.m_usMsgType, pMsg, wSize);
		break;





	case MGPT_RLT_CREATE_GROUP:
		_handlerRltCreateGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_EXIT_GROUP:	
		_handlerRltExitGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_KICK_PLAYER_FROM_GROUP:
		_handlerRltKickPlayerFromGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_DISMISS_GROUP:
		_handlerRltDismissGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_JOIN_GROUP:
		_handlerRltJoinGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_REQ_CONFIRM_GROUP_JOIN:
		_handlerReqConfirmGroupJoin(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_INVITE_TO_GROUP:
		_handlerRltInviteToGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_REQ_CONFIRM_INVITE_TO_GROUP:
		_handlerReqConfirmInviteToGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_CONFIRM_INVITE_TO_GROUP:
		_handlerRltConfirmInviteToGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_REQ_CONFIRM_BEINVITE_TO_GROUP:
		_handlerReqConfirmBeInviteToGroup(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_APPOINTMENT_CHEIF:
		_handlerRltAppointmentCheif(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_GROUP_RAID:
		_handlerNotifyGroupRaid(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_MODIFY_QUALITY:
		_handlerRltModityQuality(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_CHEIF_FORBID_JION:
		_handlerRltForbidJoin(pkSession,pMsg,wSize);
		break;
	case MGPT_REQ_ITEM_REQUEST_TRADE:
		_handlerRequestTradeItem(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_REQUEST_TRADE:
		_handlerRltTradeItemInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_REPLY_TRADE:
		_handlerRelpyTradeItem(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_ADD_ITEM_IN_TRADE:
		_handlerAddItemToTradePack(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_REMOVE_ITEM_IN_TRADE:
		_handlerRemoveItemFormTradePack(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_UPDATE_CASH_IN_TRADE:
		_handlerUpdateTradeCash(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_TRADE_LOCK:
		_handlerLockTrade(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_TRADE_UNLOCK:
		_handlerUnLockTrade(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_TRADE_INTERRUPT:
		_handlerInterruptTrade(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEM_TRADE_FINISH:
		_handlerFinishTrade(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ITEMEHNANCE:
		_handlerEnhanceItemEnd(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ITEMDECOMPOSIT:
		_handlerDecompositeItemEnd(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ITEMUNSOCKET:
		_handlerUnSocketItemEnd(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_UNBIND_ITEM_IN_PACKAGE:
		_handlerItemUnbind(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ITEMREFINE:
		_handlerItemRefine(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ITEMREFINE_APPLY:
		_handlerItemRefineApply(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_EXTERN_BASICBAG:
		_handlerRltExternBasicBag(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_AUTO_OPEN_BAG_SLOT:
		_handlerNotifyAutoOpenBasicBag(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_CHAT_REQUEST:
		_handlerRltChatRequest(pkSession,pMsg,wSize);
		break;
	case MGPT_GC_CHAT_MESSAGE:
		_handlerReceiveChatMsg(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_UPDATE_CHANNEL:
		_handlerRltUpdateChatChannel(pkSession,pMsg,wSize);
		break;


	case MGPT_RLT_CHEST_PICKUP:
		_handlerChestPickUp(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_OP_TEMP_PACK:
		_landlerRltTempPackOP(pkSession,pMsg,wSize);
		break;

	case MGPT_RLT_FORBIDLIST:
		_handlerRltForbidList(pkSession, pMsg, wSize);
		break;

	case MGPT_RLT_SAVECASH_TO_WAREHOUSE:
	case MGPT_RLT_GETCASH_FROM_WAREHOUSE:
	case MGPT_RLT_EXPAND_WAREHOUSE:
		_handlerWarehouseProtocol(pkSession, kMsgHead.m_usMsgType, pMsg, wSize);
		break;
	case MGPT_RLT_SKILL_BOOK_UPGRADE:
		_handlerRltUpgradeMiJi(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_UPGRADE_SUBORDINATE:
		_handlerRltUpgradeCimelia(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SUBORDINATE_OPERATION:
		_handlerRltChangeTalisman(pkSession, pMsg, wSize);
		break;
	case MG_RLT_SETSUBTRANSFORMFLAG:
		_handlerRltSetSubtransFormFlag(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_MAILASSET:
		_handlerRltMailAsset(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_READMAIL:
		_handlerRltReadMail(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETITEMAFFIX:
		_handlerRltItemAffix(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETCASHAFFIX:
		_handlerRltCashAffix(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_WRITE_MAIL:
		_handlerRltWriteMail(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_UNTREAD_MAIL:
		_handlerRltUntreadMail(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DELETE_MAIL:
		_handlerRltDeleteMail(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ADDITEM_MAIL:
		_handlerRltAddItemToMailAffix(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DELITEM_MAIL:
		_handlerRltDelItemFromMailAffix(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_PAYFORMAIL:
		_handlerRltPayForMail(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_IS_EXIT_NEWMAIL:
		_handlerRltIsExistNewMail(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_AUCTION_SELL:
		_handlerRltAuctionSell(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_AUCTION_CANCEL:
		_handlerRltAuctionCancel(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_AUCTION_BUY:
		_handlerRltAuctionBuy(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_AUCTION_SEARCH:
		break;
	case MGPT_RLT_ADD_ITEM_AUCTIONSELL:
		_handlerRltAddItemToAuctionSell(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DEL_ITEM_AUCTIONSELL:
		_handlerRltDelItemFromAuctionSell(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_AUCTION_LIST:
		_handlerRltAuctionList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_AUCTION_GETAWARD:
		_handlerRltAuctionGetAward(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_AUCTION_NOTICE:
		_handlerRltAuctionNotice(pkSession, pMsg, wSize);
		break;

	case MGPT_RLT_CREATE_GUILD:
		_handlerRltCreateGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_ADD2GUILD:
		_handlerNotifyAdd2Guild(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_ADD_MEMBER_INFO:
		_handlerNotifyAddMemberInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_DEL_MEMBER_INFO:
		_handlerNotifyDelMemberInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_GUILD_MEMBER_INFO:
		_handlerNotifyUpdateMemberInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_POSITION_CHANGE:
		_handlerNotifyPositionChange(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_MODIFY_GUILD_DECLARATION:
		_handlerRltModifyGuildDeclaration(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILD_KICKMEMBER:
		_handlerRltKickMemberGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_APPLY_GUILD:
		_handlerRltApplyGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_EXIT_GUILD:
		_handlerRltExitGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILD_ASSET:
		_handlerRltGuildAsset(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DISMISS_GUILD:
		_handlerRltDismissGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILD_SKILL:
		_handlerRltGuildSkill(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DONATE_GUILD_MATERIALS:
		_handlerRltDonateGuildMaterials(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILD_MEMBER_INFO:
		_handlerRltGuildMemberInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_CONFIRM_APPLY_GUILD:
		_handlerRltComirmApplyGuilid(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILD_APPLY_LIST:
		_handlerRltGuildApplyList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_INVITE_TO_GUILD:
		_handlerRltInviteToGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_REQ_CONFIRM_INVITE_TO_GUILD:
		_handlerReqConfirmInviteToGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_FORBID_GUILD_APPLY:
		_handlerRltForbidGuildApply(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_CLEAR_GUILD_APPLYLIST:
		_handlerRltClearGuildApplyList(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_ADD_GUILD_APPLY:
		_handlerNotifyAddGuildApply(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_DEL_GUILD_APPLY:
		_handlerNotifyDelGuildApply(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILD_APPOINT:
		_handlerRltGuildAppoint(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SEARCH_GUILD_RANK:
		_handlerRltUpdateGuildRank(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_POWERGUILD_APPOINT:
		_handlerRltPowerGuildAppoint(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_GUILD_RAID_INFO:
		_handlerNotifyGuildRaidInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_UPGRADE_GUILD:
		_handlerRltUpgradeGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SEARCH_GUILD:
		_handlerRltSearchGuild(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ONLINE_PLAYER_STATE:
		_handlerRltOnlinePlayerState(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GAME_EVENT:
		_handlerRltGameEvent(pkSession, pMsg, wSize);
		break;

	case MGPT_RLT_ADD_FRIEND:
		_handlerRltAddFriend(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DEL_FRIEND:
		_handlerRltDelFriend(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_FRIEND_LIST:
		_handlerRltGetFriendList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ADD_ENEMY:
		_handlerRltAddEnemy(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DEL_ENEMY:
		_handlerRltDelEnemy(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_ENEMY_LIST:
		_handlerRltGetEnemyList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ADD_BLACKLIST:
		_handlerRltAddBlackList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DEL_BLACKLIST:
		_handlerRltDelBlackList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_BLACKLIST:
		_handlerRltGetBlackList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_FRIEND_STATUS:
		_handlerRltGetFriendStatus(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETFRIEND_DETAIL:
		_handlerRltGetFriendDetail(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCMODIFYCHARNAME:
		_handlerRltSyncFriendModifyName(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_PLAYER_DELTADATA:
		_handlerRltPlayerDeltaData(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SOCIETY_DELTADATA:
		_handlerRltGetSocietyDeltaData(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_CHANGE_SOCIETY_INFO:
		_handlerRltChangeSocietyInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_PLAYER_NEARBY:
		_handlerRltGetPlayerNearBy(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SYNC_SUMMOR_POOL:
		_handlerRltSyncSummorPool(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_FRIEND_MSG_BROAD:
		_handlerRltFriendMsgBroad(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_FRIEND_OLINE_NOTICE:
		_handlerRltFriendOnlineNotice(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_FRIEND_OFFLINE_NOTICE:
		_handlerRltFriendOfflineNotice(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ADD_FRIENDNICK:
		_handlerRltAddFiendNick(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SUPPORT_FRIENDNICK:
		_handlerRltSupportFiendNick(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_BEADDFRIEND:
		_handlerRltBeAddFriend(pkSession, pMsg, wSize);
		break;
	case MGPT_REQ_CONFIRM_BEADDFRIEND:
		_handReqConfirmBeAddFriend(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETFRIENDNICK:
		_handlerRltGetFriendNick(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SEARCH_PLAYER:
		_handlerRltSearchPlayer(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SEARCH_PLAYER_DETAILINFO_TITLE:
		_handlerRltSearchPlayerDetailInfoTitle(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SEARCH_PLAYER_DETAILINFO_ARENA:
		_handlerRltSearchPlayerDetailInfoArena(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_CONTROLLER_AND_CORE_COMBAT_SCORE:
		_handlerRltControllerAndCoreCombatScore(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_PLAYER_PK_INFO:
		_handlerRltPlayerPKInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PLAYERS_LOCATION_IN_FOV:
		_handlerSyncPlayersLocationChange(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PLAYERS_ANGLE_IN_FOV:
		_handlerSyncPlayersAngleChange(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PLAYERS_ENTER_IN_FOV:
		_handlerSyncPlayersEnter(pkSession, pMsg, wSize);
		break;
	case MGPT_REFRESHBUDDYTRANSFORMID:
		_handlerRefreshBuddyTransformID(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_BUDDYRUNESTARLEVELINFO:
		_handleSyncbuddyRuneStarLevel(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYERS_LEAVE_IN_FOV:
		_handlerSyncPlayersLeave(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PLAYERS_STATECHANGE_IN_FOV:
		_handlerSyncPlayersStateChange(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PLAYERS_PVECHANGE_IN_FOV:
		_handlerSyncPlayersPVEStateChange(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PLAYERS_HPMPCHANGE_IN_FOV:
		_handlerSyncPlayersHPMPChange(pkSession, pMsg, wSize);
		break;
	case MGPT_SC_PET_SWITCH_ENABLED_INDEX:
		_handlerToyPetSwitchEnabledIndex(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SHAPESHIFT_INFO:
		_handlerSyncShapeShift(pkSession, pMsg, wSize);
		break;

	// Profile相关协议
	case MGPT_RLT_CHAR_PROFILE:
		_handlerRltProfileName(pkSession, pMsg, wSize);
		break;

	////卡片相关协议
	//case MGPT_SYNC_CARD_ASSET:
	//	_handlerSyncCardAsset(pkSession,pMsg,wSize);
	//	break;

	case MGPT_RLT_SYNC_PING:
		_handlerRltSyncPing(kMsgHead,pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_ACT_SUBINFO:
		_handlerSyncACTSubInfoEnd(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_DEBUG_COMMANDLINE:
		_handlerRltDebugCommandLine(pkSession, pMsg, wSize);
		break;

	case MGPT_RLT_DEBUG_ACTOR_AI_INFO:
		_handlerRltDebugActorAIInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_ARENA_OPERTAION:
		_handlerRltArenaOpeartion(pkSession,pMsg,wSize);
		break;
	//竞技场
	case MGPT_RLT_SIGNUPARENA:
		_handlerRltSignArena(pkSession,pMsg,wSize);
		break;
	//case MGPT_RLT_CANCELARENA:
	//	_handlerRltCancelArena(pkSession,pMsg,wSize);
	//	break;
	case MGPT_RLT_MATCHARENA:
		_handlerRltMatchArena(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_LEAVEARENA:
		_handlerRltLeaveArena(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ARENASTATE:
		_handlerRltArenaState(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ARENARESULT:
		_handlerRltArenaResult(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ARENAOPENDOOR:
		_handlerRltArenaOpenDoor(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ARENATIP:
		_handlerRltArenaTip(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GIVEUPARENA:
		_handlerRltGiveUpArena(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ARENA_CONFIRM_CANCEL:
		_handlerRltArenaConfirmCancel(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ARENA_MATCH_AGAIN:
		_handlerRltArenaMatchAgain(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_ARENA_BATTLE_INFO:
		_handlerSyncArenaBattleInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_RANK_QUERY_CHALLENGE_INFO:
		_handlerRankQueryChallengeInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_RANK_CHALLENGE:
		_handlerRltRankChallenge(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_RANK_CHALLENGE:
		_handlerNotifyRankChallenge(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_RANK_BECHALLENGE:
		_handlerNotifyRankBeChallenge(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETRANKYIZHONGQINXI:
		_handlerGetRankYiZhongQinxi(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETGUILDMAPBOSSRANK:
		_handlerGetRankGuildBossActivity(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCSINGLEDAMAGEREWARDRECORD:
		_handlerSyncSingleDamageRewardRecord(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_HUANJINGFLAG:
		_handlerThemeRaidBossFlag(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETTASKEXTRAREWARD:
		_handlerRLT_GetTaskExtraReward(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCTASKEXTRAREWARD:
		_handlerSyncTaskExtraReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETSINGLEDAMAGEREWARD:
		_handlerRLTGetSingleDamageReward(pkSession, pMsg, wSize);
		break;
	case MGPT_SC_ThemeFlagChange_Ret:
		_handlerThemeFlagChange_Ret(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_SET_ATVCHARGE_REDPOINT:
		_handlerNotifySetAtvChargeRedPoint(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_SET_ATVCHARGE_REDPOINT:
		_handlerRespSetAtvChargeRedPoint(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_YIZHONGQINGXI_DAMAGEDATA:
		_handlerRltYizhongqingxiDamageData(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_YIZHONGQINGXI_DETAILDAMAGEDATA:
		_handlerRltYizhongqingxiDetailDamageData(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ANSWERRANKINFO:
		_handlerGetAnswerRankInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_YIJITUXIRANKINFO:
		_handlerGetYijituxiRankInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_RANK_INFO:
		_handlerRespRankInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_CHALLENG_RANK_RESP:
		_handlerChallengeRankInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GETRANK_PLAYERBUDDY_INFO:
		_handlerRespRankPlayerBuddyInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GETRANK_BUDDY_INFO:
		_handlerRespRankBuddyInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GETRANK_PET_INFO:
		_handlerRespRankPetInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GETRANK_SUB_INFO:
		_handlerRespRankSubInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_RANK_TOPN_PLAYER_INFO:
		_handlerRespRankTopnPlayerInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GETBUDDYRANKINDEX:
		_handlerRespBuddyRankIndexEx(pkSession, pMsg, wSize);
		break;
	case MGPT_GG_RESP_CHALLENGGIFTSTATE:
		_handlerRespChallengGiftState(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_BATTLE_INFO:
		_handlerRespBattleInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_ADD_CHALLENGE_TIME:
		_handlerRespAddChallengeTime(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_CLEAR_CHALLENGE_CD:
		_handlerRespClearChallengeCD(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GET_CHALLENGE_CD:
		_handlerRespGetChallengeCD(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DAILY_AWARD:
		_handlerRltDailyAward(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCFINISHMAINTASKID:
		_handlerSyncFinishMainTaskID(pkSession, pMsg, wSize);
		break;
	case MGPT_RANK_QUERY_BASEINFO_RESP:
		_handlerRltRankBaseInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RANK_RANK_DAILY_SETTLE_NOTIFY:
		_handlerRespRankDalySettleNotify(pkSession, pMsg, wSize);
		break;
	case MGPT_SEND_DAILY_CLEAN_RESP:
		_handlerRespDalyCleanData(pkSession, pMsg, wSize);
		break;
	case MGPT_COMMON_STAT_BASIC_OPERATION_RESP:
		_handlerRespCommonStatBaseOpt(pkSession, pMsg, wSize);
		break;
	case MGPT_COMMON_STAT_BASIC_PROFIT_NOTIFY:
		_handlerRespBasicProfitNotify(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_EXECUTE_ACTOR_OPERATION:
		_handleRltExecuteActorOperation(pkSession, pMsg, wSize);
		break;
	case MGPT_SC_EquipAdvance_Ret:
		_handleEquipAdvance_Ret(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SUBEQUIPADVANCE:
		_handleSubEquipAdvance_Ret(pkSession, pMsg, wSize);
		break;
	//case MGPT_RLT_CARD_EXCHANGE:
	//	_handlerRltCardExchange(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_CARD_TAKEOUT:
	//	_handlerRltCardTakeOut(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_ADD_CARD:
	//	_handlerRltAddCard(pkSession,pMsg,wSize);
	//	break;

	//case MGPT_RLT_HOOK_INFO:
	//	_handlerRltHookInfo(pkSession,pMsg,wSize);
	//	break;

	////游戏币元宝交易
	//case MGPT_RLT_CURRENCY_QUICK_BUY:
	//	_handlerRltCurrencyQuickBuy(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_QUICK_SELL:
	//	_handlerRltCurrencyQuickSell(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_COMMISSION_BUY:
	//	_handlerRltCurrencyCommissionBuy(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_COMMISSION_SELL:
	//	_handlerRltCurrencyCommissionSell(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_QUICK_TRADE_LIST:
	//	_handlerRltCurrencyQuickTradeList(pkSession,pMsg,wSize);
	//	break;	
	//case MGPT_RLT_CURRENCY_COMMISSION_ITEM_LIST:
	//	_handlerRltCurrencyCommissionItemList(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_TRADE_RECORD_LIST:
	//	_handlerRltCurrencyTradeRecordList(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_REVOKE_COMMISSION:
	//	_handlerRltCurrencyRevokeCommission(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_YUANBAO_DEPOSIT:
	//	_handlerRltCurrencyYuanBaoDeposit(pkSession, pMsg, wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_YUANBAO_DRAW:
	//	_handlerRltCurrencyYuanBaoDraw(pkSession, pMsg, wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_MONEY_DEPOSIT:
	//	_handlerRltCurrencyMoneyDeposit(pkSession, pMsg, wSize);
	//	break;
	//case MGPT_RLT_CURRENCY_MONEY_DRAW:
	//	_handlerRltCurrencyMoneyDraw(pkSession, pMsg, wSize);
	//	break;
	case MGPT_RLT_MYSTICAL_SHOP_INFO:
		_handlerRltMysticalShopGoodList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_MYSTICAL_SHOP_OPT:
		_handlerRltMysticalShopOp(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_BUY_ITEM:
		_handlerRltShopCityBuyGood(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PENTACLE:
		_handlerSyncGameplayerStarMoney(pkSession, pMsg, wSize);
		break;
	//帮派家园
	case MGPT_RLT_ENTER_GUILD_HOME:
		_handlerRltEnterGuildHome(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_LEAVE_GUILD_HOME:			
		_handlerRltLeaveGuildHome(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_UPGRADE_NPC:					
		_handlerRltUpgradeGuildNPC(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_BUY_FROM_GUILDNPC:
		_handlerRltBuyFromGuildNPC(pkSession, pMsg, wSize);
		break; 
	case MGPT_RLT_DONATE:	
		_handlerRltDonate(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_PUTWAREHOUSE:
		_handlerRltPutWareHouse(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SWAPWAREHOUSE:
		_handlerRltSwapWareHouse(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETGUILDWAREHOUSEINFO:
		_handlerRltGetGuildWareHouse(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_CHANGE_THIEF_RECORD:
		_handlerRltThiefCount(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_PRIZE_OF_KILLTHIEF:
		_handlerRltGetPrizeOfKillThief(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILDHOME_IN_STORY:
		_handlerRltGuildHomeInStory(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_OPEN_KILLTHIEF:
		_handlerRltOpenKillThief(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_KILLTHIEF_RECORDTIME:
		_handlerRltKillThiefRecordTime(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_KILLTHIEF_INFO:
		_handlerRltKillThiefInfo(pkSession, pMsg, wSize);
		break;
//team platform
	case MGPT_RLT_ENTER_SACREDWEAPON:
		_handlerRltEnterSacredWeapon(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_QUIT_SACREDWEAPON:
		_handlerRltQuitSacredWeapon(pkSession,pMsg,wSize);
		break;
	
	//case MGPT_SYNC_GIFT_ASSET:
	//	_handlerSyncGiftAsset(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_GET_GIFT:
	//	_handlerRltGetGift(pkSession,pMsg,wSize);
	//	break;
	//太极阵
	//case MGPT_RLT_CHANGEELEMENT:
	//	_handlerRltChangeElement(pkSession,pMsg,wSize);
	//	break;
	case MGPT_SYNC_SAFE_SYSTEM_DATA:
		//_handlerSyncSafeSystemAsset(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_TRANSCRIPTION_PROGRESS:
		_handlerNotifyDungeonStage(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_RAID_ASSET:
		_handlerSyncRaidAsset(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_TRANSPORT_ASSET:
		_handlerSyncTransportAsset(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_RAID_SETTLE:
		_handleSyncRaidSettle(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_PORTAL:
		_handleSyncPortal(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SET_ASSIST_INFO:
		_handlerRltSetAssistInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_HOOKING_GUIDE_LOCATION:
		_handlerRltGetHookingGuideLocation(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_CONTENT_INFO_SYNC:
		_handlerGameStoryInfoSync(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_BATCH_INFO_SYNC:
		_handlerGameStoryBatchInfoSync(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_ACHIEVEMENT_INFO:
		_handlerSyncAchievementInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCFINISHACHIEVEMENT:
		_handlerSyncFinishAchievement(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCACHIEVEMENTRECORD:
		_handlerSyncAchievementRecord(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ACHIEVEMENT_REWARD:
		_handlerRltAchievementReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ACHIEVEMENTPOINT_REWARD:
		_handlerRltAchievementPointReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETSEVENDAYREWARD:
		_handlerRltSevenDayPointReward(pkSession, pMsg, wSize);
		break;
	/*case MGPT_RLT_ACHIEVEMENT_SEVENDAY_TRIAL_REWARD:
		_handlerRltAchievementReward(pkSession, pMsg, wSize);
		break;*/
	case MGPT_SYNC_VIP_INFO:
		_handleSyncVIPInfo(pkSession, pMsg, wSize);
		break;
	case MG_NOTIFY_VIPLEVEL_CHANGE:
		_handleNotifyVipLevelChange(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DRAW_VIP_TIME_GIFT:
		_handleRltVIPDrawGiftDaily(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DRAW_VIP_ONEOFF_GIFT:
		_handleRltVIPDrawGiftOneOff(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_DRAW_FDEPOSIT_REWARD:
		_handleRltDrawFdepositReward(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_SIGNIN:
		_handleSyncSignInInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_GUILDWAR:
		_handleSyncGuildWarInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_GUILDWAR_CAPAPULT_MSG:
		_handleNotifyGuildWarCatapultMsg(pkSession, pMsg, wSize);
		break;
	
	case MGPT_RESP_SIGNIN:
		_handleRltSignIn(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_RESIGNIN:
		_handleRltReSignIn(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_SIGNINREWARD:
		_handleRespSigninRewward(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_PLAYER_DETAIL:
		_handleRltPlayerDetail(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_POWERGUILD_REWARD:
		_handleRltPowerGuildReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_DOMAINREWARD:
		_handleRltDomainReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GLOBALGUILDWARREWARD:
		_handleRltGlobalGuildReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GUILDRAIDENTER:
		{
			MG_EnterGuildRaid_Resp rltMsg;
			if (!rltMsg.Decode(pMsg, wSize))
			{
				return;
			}
			if (rltMsg.m_Result != RE_SUCCESS)
				SendLogicInfoMessage(rltMsg.m_Result);
		}
		break;
	case MGPT_RESP_GUILDRAIDREWARD:
		_handlerRltGuildRaidReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GUILDWAR_ENTER:
		{
			LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
			if(NULL != pkGamePlayer)
				pkGamePlayer->SetGuildWarType(WarType_GuildWar);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_RUNESCAPE_ENTERED);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, WarType_GuildWar);

			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case MGPT_RLT_GET_OFFLINE_EXP:
		{
			MG_RTL_GetOfflineExp rtlMsg;
			if (!rtlMsg.Decode(pMsg, wSize))
			{
				return;
			}
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(CLET_CLIENT_GET_OFFLINE_EXP);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rtlMsg.m_nExp);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rtlMsg.m_nOffTime);

			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case MGPT_RLT_GAIN_OFFLINE_EXP:
		{
			MG_RTL_GainOfflineExp rtlMsg;
			if (!rtlMsg.Decode(pMsg, wSize))
			{
				return;
			}
			if (rtlMsg.m_result != RE_SUCCESS)
			{
				SendLogicInfoMessage(rtlMsg.m_result);
			}
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_GAIN_OFFLINE_EXP);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, rtlMsg.m_gainType);

			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case MGPT_RESP_GLOBALGUILDWAR_ENTER:
		{
			LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
			if(NULL != pkGamePlayer)
				pkGamePlayer->SetGuildWarType(WarType_CrossRealmGuildWar);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_RUNESCAPE_ENTERED);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, WarType_CrossRealmGuildWar);

			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case MGPT_NOTIFY_POWERGUILD_INFO:
	case MGPT_NOTIFY_GUILDPLAYER_INFO:
	case MGPT_RESP_GUILDWAR_CATAPULT_FIRE:
	case MGPT_CROSSREALM_MAPLIST_RESP:
	case MGPT_CROSSREALM_TEAMLIST_RESP:
	case MGPT_CROSSREALM_CREATETEAM_RESP:
	case MGPT_CROSSREALM_TEAMDETAIL_RESP:
	case MGPT_CROSSREALM_ADD2TEAM_RESP:
	case MGPT_CROSSREALM_LEAVETEAM_RESP:
	case MGPT_CROSSREALM_TEAMINFOEDIT_RESP:
	case MGPT_CROSSREALM_KICKMEMBER_RESP:
	case MGPT_CROSSREALM_READYMEMBER_RESP:
	case MGPT_CROSSREALM_AUTOADD_RESP:
	case MGPT_CROSSREALM_ADD2TEAM_NOTIFY:
	case MGPT_CROSSREALM_PLAYINFOCHANGE_NOTIFY:
	case MGPT_CROSSREALM_TEAMINFOCHANGE_NOTIFY:
	case MGPT_CROSSREALM_LEAVETEAM_NOTIFY:
	case MGPT_CROSSREALM_BEKICK_NOTIFY:
	case MGPT_CROSSREALM_ENTER_RESP:
	case MGPT_CROSSREALM_ENTER_NOTIFY:
	case MGPT_NOTIFY_POWERGUILDEMENY_INFO:
	case MGPT_CROSSREALM_PLAYER_TEAMINFO_RESP:
	case MGPT_NOTIFY_WAR_SETTLE:
	case MGPT_GAMESTORY_NOTIFY_FACTION_SETTLE:
	case MGPT_GAMESTORY_KEY_ECOUNTED_STAT_NOTIFY:
	case MGPT_RESP_TRANSFER_DETAIL:
	case MGPT_FALLIANCE_BOSS_INFO_RSP:
	case MGPT_FALLIANCE_BOSS_INFO_NOTICE:
	case MGPT_FALLIANCE_BOSS_INFO_NOTICE_NOTIFY:
	case MGPT_NOTIFY_CROSSREALM_INVITE:
	case MGPT_CROSSREALM_INVITEPLAYER_RESP:
	case MGPT_NOTIFY_CROSSREALM_REFUSE_INVITE:
	case MGPT_INVITE_ALLPLAYER_CROSSREALMTEAM_RESP:
	case MGPT_RLT_GET_BUY_TIMES:
	case MG_RLT_GET_FD_FREE_REWARD:
	case MG_RLT_GET_FD_DISCOUNT_REWARD:
	case MG_RLT_BUY_GROWTH_PACKAGE:
	case MG_RLT_GET_GROWTH_REWARD:
	case MGPT_NOTIFY_CHALLENGE_ENDLESSTOWERSTATE:
		_handleMsgToLua(kMsgHead.m_usMsgType, pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_CROSSREALMTEAM:
		_handleSyncCorssRealmTeam(pkSession, pMsg, wSize);
		break;
	case MGPT_SS_CROSSREALM_FAST_NOTIFY:
		_handleNotifyCrossRealmTeamFast(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_POWERGUILDPLAYER_BEGIN:
		{
			LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
			if (NULL != pkLogicManager)
				pkLogicManager->UpdateDirtyFlag(DIRTY_FLAG_WORLDPOWERGUILD_ASSET);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_RUNESCAPE_ENTRY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, WarType_GuildWar);

			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case MGPT_NOTIFY_GLOBALPOWERGUILDPLAYER_BEGIN:
		{
			LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
			if (NULL != pkLogicManager)
				pkLogicManager->UpdateDirtyFlag(DIRTY_FLAG_WORLDPOWERGUILD_ASSET);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_RUNESCAPE_ENTRY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, WarType_CrossRealmGuildWar);

			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case MGPT_NOTIFY_DOMAINWARPLAYER_BEGIN:
		{
			LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
			if (NULL != pkLogicManager)
				pkLogicManager->UpdateDirtyFlag(DIRTY_FLAG_WORLDPOWERGUILD_ASSET);

			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_CLIENT_RUNESCAPE_ENTRY);
			kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, WarType_DomainWar);

			UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
		}
		break;
	case MGPT_SYNC_GREAT_KUNGFU://绝学系统 20140513 林培华
		_handleSyncGreatKungFu(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_OPEN_ABILITY_INFO:
		_handleSyncOpenAbilityInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_DOMAIN_INFO:
		_handleSyncDomainInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_GREAT_KUNGFU_OPERATION_RESP://绝学操作
		_handleGreatKungFuOperationResp(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_GM_INFO:
		_handleSyncGMInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_SUB_INFO:
		_handleSyncSubordinateInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_SOCIETY_DYNAMIC_INFO:
		_handlerRltSocietyDynamicInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_OPEN_ABILITY:
		_handlerRltOpenAbility(pkSession, pMsg, wSize);
		break;

	case MGPT_SYNC_COMMON_STAT_INFO:
		_handleSyncCommonStat(pkSession, pMsg, wSize);
		break;

	case MGPT_SYNC_TASK_TYPE_DATA:
		_handlerSyncTaskTypeData(pkSession, pMsg, wSize);
		break;

	case MGPT_SYNC_TASKRANK:
		_handlerSyncTaskRank(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_TASKCHAPTERRAWARD:
		_handlerSyncTaskChapterRecord(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_MAINTASKREACHREWARD:
		_handlerSyncMainTaskReachRecord(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_TAKECHAPTERREWARD:
		_handlerRltTaskChapterReward(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_MAINTASKREACHREWARD:
		_handlerRltTaskReachReward(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCNEXTREFRESHREWARDTASKTIME:
		_handlerSyncNextRefreshRewardTaskTime(pkSession, pMsg, wSize);
		break;
	case MGPT_SC_ADD_TIMER:
		_handerSCAddTimer(pkSession, pMsg, wSize);
		break;

	case MG_RLT_COLLECTION_ACTIVATION:
		_handlerRltCollection(pkSession, pMsg, wSize);
		break;
	case MG_RLT_COLLECTION_REWARD:
		_handlerRltCollectionSuit(pkSession, pMsg, wSize);
		break;
	case MGPT_COMMON_STAT_OPERATION_RESP:
		_handleRltStatOperation(pkSession, pMsg, wSize);
		break;
	case MGPT_ACTIVITY_COMMON_OP_RESP:
		_handleRltActivityCommonOperation(pkSession, pMsg, wSize);
		break;
	
	case MGPT_SYNC_ACT_ACTIVE_REWARD_INFO_B:
		_handleSyncActActiveRewardInfoB(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETRANKSUBBINFO:
		_handleRltGetRankSubBInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCDROPINFO:
		_handleSyncDropInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCREGULARBOSSDROP:
		_handleSyncRegularBossDrop(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCYUANZHENGREVERTINFO:
		_handleSyncGloryPointRecover(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GETSUBBVALUE:
		_handleRLT_GetSubBValue(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNCACTIVITYFINISH:
		_handleSyncActivityFinish(pkSession, pMsg, wSize);
		break;
	case MGPT_ACTIVITY_SCHEDULE_INFO:
		_handleRltActivityScheduleInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_USE_ACTIVATION_COUNT:
		_handleRltUseActivationCount(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_USE_ACTIVATION_CODE:
		_handleRltUseActivationCode(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_BUYCOURAGE_ITEM:
		_handerRltBuyCourageItem(pkSession, pMsg, wSize);
		break;
	case MGPT_TREASURE_CHESTS_OP_RESP:
		_handleRltChestsOperation(pkSession, pMsg, wSize);
		break;
	case MGPT_ACTIVITY_SECRET_TREASURE_OP_RESP:
		_handleRltSecretTreasureOperation(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_SECRET_TREASURE_INFO:
		_handleSyncSecretTreasureInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_MELT_ITEM:
		_handleSmeltBack(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_STARS_RUNE_UPGRADE:
		_handleStarsRuneBack(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_REALTIME_COMBAT_RECORDER:
		_handleRealTimeComBatRecorder(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_RUNE_UPGRADE:
		_handleRltLevelUpRune(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_RUNE_STAR:
		_handleRltStarUpRune(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_RESET_RUNE:
		_handleRltResetRune(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_EQUIP_RUNE:
		_handleRltEquipRune(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ONEKEY_EQUIP_RUNE:
		_handleRltOnekeyEquipRune(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_UNEQUIP_RUNE:
		_handleRltUnEquipRune(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_ONEKEY_UNEQUIP_RUNE:
		_handleRltOnekeyUnEquipRune(pkSession, pMsg, wSize);
		break;

	case MGPT_SYNC_SKILL_BOOK_INFO:
		_handleSyncSkillBookInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_SKILLBOOK_SLOTINFO:
		_handleSyncSkillBookSlotInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_OPEN_NEW_SKILL_BOOK:
		_handleOpenNewSkillBook(pkSession, pMsg, wSize);
		break;
	case MG_RLT_ACTIVATE_SKILLBOOK:
		_handleRltActivateSkillBook(pkSession, pMsg, wSize);
		break;
	case MG_RLT_UNLOCK_SKILLBOOKSLOT:
		_handleRltUnlockSkillBook(pkSession, pMsg, wSize);
		break;
	case MG_RLT_WEAR_SKILLBOOK:
		_handleRltWearSkillBook(pkSession, pMsg, wSize);
		break;

	case MGPT_TREASURE_CHESTS_LOGSNOTIFY:
		_handleRltChestsLogs(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_TREASURE_CHESTS_INFO:
		_handleSyncTreasureChestsInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_ENTER_CROSS_MAP_RESP:
		_handleRespEnterCrossMap(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_OPERATION_RESP:
		_handleRespGameStoryOperation(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_RELATE_TIME_RESP:
		_handleRespSkyArenaTime(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_FACTION_DEFENDER_RESP:
		_handleRespFactionDefender(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_GUILDTRANSPORT:
		_handleGuildTransportAdd(pkSession, pMsg, wSize);
		break;
	case MGPT_NOTIFY_GUILDTRANSPORT_DEL:
	    _handleGuildTransportDel(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GUILDTRANSPORT_CREATE:
		_handleRespGuildTransportCreat(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GUILDTRANSPORT_ENTER:
		_handleRespGuildTransportEnter(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_GUILDREWARD_STATE:
		_handleGuildRewardState(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_GAMESTORY_ASSET:
		_handleSyncGameStoryAsset(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_ADD_GUILDGOOD:
		_handlerRespAddGuildGood(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_GUILD_ITEM_PACK_OPT:
		_handlerRespGuildItemOpt(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_RECRUIT_GUILD_MEMBER:
		_handlerRltRecruitGuildMember(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILD_REWARD:
		_handlerRltGetGuildReward(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_COMMON_SHOP_INFO:
		_hanldeSyncCommonShopInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_BUY_VIGOR:
		_handleRltBuyVigor(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_CLEAR_PK:
		_handleRltClearPKBuff(pkSession, pMsg, wSize);
		break;
	case MGPT_CROSSREALM_TEAM_INFO_NOTIFY:
		_handleCrossRealMapTeamInfoNotify(pkSession, pMsg, wSize);
		break;
	case MGPT_CROSSREALM_TEAM_OPERATION_RESP:
		_handleCrossRealMapTeamOperationResp(pkSession, pMsg, wSize);
		break;
	case MGPT_RESP_ITEM_IN_PACKAGE_OPERATION:
		_handleItemInPackageCommonOperationResp(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_RUMOR_INFO:
		_handlerSyncRumorInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_RANK_INFO_SYNC:
		_handlerSyncGameStoryRankInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_FACTION_SCORE_SYNC:
		_handlerSyncSkyArenaScore(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_KEY_MONSTER_HP_SYNC:
		_handlerSyncSkyArenaMonsterHp(pkSession, pMsg, wSize);
		break;
	case MGPT_SS_PLATFORM_MESSAGE:
		_handlerSyncPlatformMessage(pkSession, pMsg, wSize);
		break;
	case MGPT_ALLIANCE_SERVER_LIST_RESP:
		_handleRltAlliance(pkSession, pMsg, wSize);
		break;
	case MGPT_ALLIANCE_ENTER_CROSS_MAP_RESP:
		_handleRltEnterAlliance(pkSession, pMsg, wSize);
		break;
	case MGPT_FALLIANCE_GUILD_WIN_INFO_RSP:
		_handleRltAllianceWinInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_ALLIANCE_UNIT_INFO_RSP:
		_handleRltAllianceUnitInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SEND_REWARD_GAIN_RESP:
		_handleRltAllianceUnitRwd(pkSession, pMsg, wSize);
		break;
	case MGPT_ALLIANCE_UNIT_GAIN_RSP:
		_handleRltDrawAllianceUnitRwd(pkSession, pMsg, wSize);
		break;
	case MGPT_GAME_NOTICE_MSG:
		_handleGameNoticeMsg(pkSession, pMsg, wSize);
		break;
	case MGPT_DEBUG_RELOAD_NOTIFY:
		_handleDebugReloadNotify(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_SOULREFINE_ASSET_INFO:
		_handleSoulRefineAssetInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GET_FASHION_ASSET_INFO:
		_handleFashionAssetInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_FASHION_SHOW:
		_handleFashionShow(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SHOW_FASHION:
		_handleShowFashion(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SOULREFINE_LVLUP:
		_handleSoulRefineLevelUp(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_SOULREFINE:
		_handleSoulRefine(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_ACT_SCHEDULE_INFO:
		_handleSyncActivityInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_UPDATE_BULLETIN_INFO:
		_handleSyncUpdateBulletinInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_CHANGE_CHAR_NAME:
		_handleChangeCharName(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_CONTROL_INFO:
		_handleSyncPlayerControl(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_RECEIVE_DAILY_ACTIVITY_REWARD:
		_handleRltGamePlayerDailyActivityReward(pkSession, pMsg, wSize);
		break;
	case MGPT_SYNC_DAILY_ACTIVITY:
		_handleRltGamePlayerDailyActivity(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_BUDDY_GROWTH_PACKAGE:
		_handleRltCharacterUpgradGift(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_FD_DAY:
		_handleRltGamePlayerDailyCharge(pkSession,pMsg,wSize);
		break;
	case MG_RLT_REWARD_TIME:
		_handleRltGamePlayerDailyChargeTime(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_FD_CHARGE_REWARD:
		_handleRltGamePlayerDailyChargeRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_COLLECTION_INFO:
		_handleRltGamePlayerCollectionRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_BOSS_FIRST_KILL_INFO:
		_handleRltGamePlayerBossFirstKill(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCKILLBOSSREWARD:
		_handleSyncKillBossReward(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_KILLBOSSREWARD:
		_handleRltKillBossReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCSERVERFIRSTKILLBOSSREWARD:
		_handleSyncServerFirstSkillBossReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCSEVENDAYFINISHTIME:
		_handleSyncSevenDayFinishTime(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCFIRSTDAYLOGINREWARDTIME:
		_handleSyncFirstDayLoginRewardTime(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCCOMMANDERTITLERECORD:
		_handleSyncCommanderTitleRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCPROFILEPICTURERECORD:
		_handleSyncProFilePictureRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCCOLLECTBOSSJP:
		_handleSyncCollectBossJP(pkSession,pMsg,wSize);
		break;

	case MGPT_SYNC_EQUIP_SLOT_LV:
		_handleSyncBuddyEquipLv(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_EQUIP_SLOT_LV:
		_handleRltBuddyEquipLv(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_RECEIVE_DAILY_REWARD:
		_handleRltReceiveGloryDailyReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYC_RECEIVE_DAILY_REWARD:
		_handleSyncReceiveGloryDailyReward(pkSession,pMsg,wSize);
		break;


	case MGPT_RLT_SETEQUIPSUITLEVEL:
		_handleRltSetCommanderEquipSuitLevel(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCEQUIPSUITLEVEL:
		_handleSyncCommanderEquipSuitLevel(pkSession,pMsg,wSize);
		break;
	
	case MGPT_RLT_SUBEQUIPSLOTMOLDINGSPIRIT:
		_handleRltSubEquipModingSpirit(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_BUDDYTRANSFORMOP:
		_handleRltBuddyTransformOP(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SETPROFILEPICTUREFLAG:
		_handleRltSetProfilePictureFlag(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SETCOMMANDERTITLEFLAG:
		_handleRltSetCommanderTitleFlag(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_ACT_ACTIVE_REWARD_INFO:
		_handleRltGamePlayerSuperShopCompete(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BOT_BASE_DATA_SYNC:
		_handleRltSyncBotData(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BOT_ACTIVATE_RESULT:
		_handleRltSyncActiveBotData(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BOT_UPGRADE_RESULT:
		_handleRltSyncUpgradBotData(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BOT_REWARD_RESULT:
		_handleRltBotProfit(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BOT_BONUS_CACHE_SYNC:
		_handleSyncBotProfit(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BOT_BEGIN_TIME_SYNC:
		_handleSyncBotBgeinTime(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_DECOMPOSITION_ITEM:
		_handleRltDeCompositionEquips(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_DECOMPOSITION_SETTING:
		_handleRltDeCompositionEquipsSetting(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SETAUTODECOMPOSITIONITEM:
		_handleRltSetAutoDecompositionItem(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SETCURRENTCOMMANDERTITLEID:
		_handleRltSetCurrentCommanderTitleID(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SETPROFILEPICTURE:
		_handleRltSetProfilePicture(pkSession,pMsg,wSize);
		break;
	case MG_RLT_BIOGRAPHYPART_REWARD:
		_handleRltBiographyReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_BIOGRAPHYPART_REWARD:
		_handleSyncBiographyChapters(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_BIOGRAPHY:
		_handleSyncBiography(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_HOLYDEED:
		_handleSyncHolydeed(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCBOSSDROPINFO:
		_handleSyncBossDropinfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCINVESTMENTCARDRECORD:
		_handleSyncInvestmentCardRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCINVESTMENTCARDREWARDRECORD:
		_handleSyncInvestmentCardRewardRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCREFRESHREWARDTASKTIMES:
		_handleSyncRefreshRewardTaskTimes(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCACCEPTREWARDTASKTIMES:
		_handleSyncOfferTaskAcceptTimes(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SINGLE_COMBAT_RECORDER:
		_handleSyncSingleCombatRecorder(pkSession,pMsg,wSize);
		break;

	case MGPT_NOTIFY_ADDNEW_HOLYDEED:
		_handleSyncAddnewHolydeed(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_TIMELIMITED_ACTIVITY:
		_handleSyncTimeLimitShop(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_TIMELIMITED_ACTIVITY_STATE:
		_handleRltTimeLimitShop(pkSession,pMsg,wSize);
		break;
	//三角色系统
	case MGPT_SC_BUDDY_DATA_SYNC:
		_handlerSyncBuddyData(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_ACTIVATE_BUDDY_DATA_SYNC:
		_handlerSyncActiveBuddyData(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BUDDY_REPLY:
		_handlerBuddyReply(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SET_ACTIVATE_BUDDY_REPLY:
		_handlerSCSetActivateBuddyReply(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BUDDY_BASE_DATA_SYNC:
		_handlerSCBuddyBaseDataSync(pkSession,pMsg,wSize);
		break;
	case MGPT_CREATE_BUDDY_NET_PLAYER:
		_handlerCreateBuddyNetPlayer(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SKY_ALL_SHOW_INFO:
		_handlerSkyAllShowInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SKY_ROUND_SHOW_INFO:
		_handlerSkyRoundShowInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SYNC_BUDDY_ACTIVATE_SCHEME_TYPE:
		_handlerSyncBuddyActivateSchemeType(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BUDDY_SCHEME_REPLY:
		_handlerBuddySchemeReply(pkSession,pMsg,wSize);
		break;

	case MGPT_SYNC_PLAYER_CONTROLLER_PROPERTY:
		_handlerSYNCPlayerControllerProperty(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_PLAYER_CONTROLLERS_PROPERTY_IN_FOV:
		_handlerSYNCPlayerControllersPropertyInFOV(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_BUDDY_STAR_UPGRADE:
		_handlerRLTBuddyStarUpgrade(pkSession,pMsg,wSize);
		break;
	case MGPT_RTL_BUDDY_MANUAL_LEVEL_UP:
		_handlerRltBuddyManualLevelUp(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_ACTIVATE_BUDDY_COUNT:
		_handlerRltActivateBuddyCount(pkSession,pMsg,wSize);
		break;
	case MGPT_REFRESH_PLAYER_BIND_BUDDY:
		_handlerRefreshPlayerBindBuddy(pkSession,pMsg,wSize);
		break;
	case MGPT_REFRESH_PLAYER_STAR_LEVEL:
		_handlerRefreshPlayerStarLevel(pkSession,pMsg,wSize);
		break;
	case MGPT_REFRESH_BUDDY_EQUIPSLOTLEVELCOUNT:
		_handleSyncRefreshBuddyEquipSlotLevel(pkSession,pMsg,wSize);
		break;
	case MGPT_REFRESH_PLAYER_RUNE_LEVEL:
		_handlerRefreshPlayerRuneLevel(pkSession,pMsg,wSize);
		break;

	case MGPT_SC_RESET_SINGLE_BUDDY_RESPAWN_TIMER:
		_handlerResetSingleBuddyRespawnTimer(pkSession,pMsg,wSize);
		break;
	case MGPT_REFRESH_BUDDY_EQUIPSLOT_STARLEVEL:
		_handlerRefreshBuddyEquipSlotStarlevel(pkSession,pMsg,wSize);
		break;
	case MGPT_REFRESHBUDDYEQUIPITEM:
		_handlerRefreshBuddyEquipItem(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCBUDDYRUNESLOTSTATE:
		_handlerActivateBuddyRuneSlot(pkSession,pMsg,wSize);
		break;
	case MGPT_BUDDY_EQUIP_SLOT_STATE:
		_handlerBudyyEquipSlotState(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_RESET_ALL_BUDDY_RESPAWN_TIMER:
		_handlerResetAllBuddyRespawnTimer(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SET_BUDDY_RESPAWN_TIMER:
		_handlerSetBuddyRespawnTimer(pkSession,pMsg,wSize);
		break;

	case MGPT_SYNCRESOURCERETRIEVESTATE:
		_handlerSyncResourceRetrieveState(pkSession,pMsg,wSize);
		break;

	case MGPT_SYNC_COLLECT_PLAYER_EQUIP_SUIT:
		_handlerSyncCollectPlayerEquipSuit(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_COLLECT_PLAYER_EQUIP_SUIT:
		_handlerRespCollectPlayerEquipSuit(pkSession,pMsg,wSize);
		break;

	case MGPT_SYNCRESOURCERETRIEVERRECORD:
		_handlerSyncResourceRetrieveRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETRESOURCERETRIEVERREWARD:
		_handlerRltGetResourceRetrieveReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_BUDDYACTIVATESACHEME:
		_handleSyncBuddyActivateScheme(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_SETBUDDYACTIVATESCHEME:
		_handleRespSetBuddyActivateScheme(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_SETSPECIALMAP_INITBUDDYACTIVATESCHEME:
		_handleRespSetSpecialMapInitBuddyActivateScheme(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BATTLE_LOG_RESP:
		_handleRespBattleLog(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BATTLEFEILD_REPORT_RESP:
		_handleRespBattleFeildReport(pkSession,pMsg,wSize);
		break;

	//灵魂连锁
	case MGPT_SC_BUDDY_CHAIN_COUNT_SYNC:
		_handlerBuddyChainCountSync(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BUDDY_CHAIN_SYNC:
		_handlerBuddyChainSync(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_BUDDY_CHAIN_OPERATION_RESULT:
		_handlerBuddyChainOpResult(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_ADD_BUDDY_CHAIN_SYNC:
		_handlerAddBuddyChainSync(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_DEL_BUDDY_CHAIN_SYNC:
		_handlerDelBuddyChainSync(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SETCHAINFLAG:
		_handlerRltSetChainFlag(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCCHAINFLAG:
		_handlerSyncChainFlag(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCREGULARBOSSGROUPINFO:
		_handlerSyncRegularBossGroupInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCBOSSKILLERINFO:
		_handlerSyncBossKillerInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RTL_ACTIVATE_SKILL:
		_handlerRltActivateSkill(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SKILL_USABLE_COUNT:
		_handlerSkillUsableCount(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SKILL_MAX_USABLE_COUNT:
		_handlerSkillMaxUsableCount(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SINGLE_SKILL_CD:
		_handlerSkillSingleCD(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_LOTTERY_ITEM:
		_handlerDrawCardResp(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SINGLE_SKILL_LOGIC_VALUE:
		_handlerSkillSingleLogicValue(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_EQUIP_NEW_REINFORCE:
		_handlerBuddyEquipReinforceLevel(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_EQUIPSLOT_STAR:
		_handlerBuddyEquipReinforceStar(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_REINFORCE_EQUIPSLOT:
		_handlerBuddyReinforceEquipSlot(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_FRIEND_RECOMMEND_FRIEND:
		_handlerRltRecommendFriend(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ADD_RECOMMEND_FRIEND:
		_handlerRltAddAllRecommendFriends(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_USER_DEAD:
		_handlerRltUserDead(pkSession,pMsg,wSize);
	case MGPT_RLT_REDUCE_CASH:
		_handlerReduceCash(pkSession,pMsg,wSize);
		break;
	case MGPT_CONTROLLER_PLAYER_INFO:
		_handlerNetPlayerActiveBuddy(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_ACTIVATE_STARVEIN:
		_handlerSyncActivateStarVein(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_STARVEIN_GOAL_RECORD:
		_handlerSyncActivateStarVeinGoal(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GET_STARVEIN_REWARD:
		_handlerRltGetStarVeinGoalReward(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ACTIVATE_STARVEIN:
		_handlerRltActivateStarVein(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_SERVER_ERROR_CODE:
		_handlerNotifyServer_ErrorCode(pkSession,pMsg,wSize);
		break;
	
	//玩具
	case MGPT_SC_PET_CAPTURE_RESULT:
		_handlerPetCaptureResult(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_PET_BASE_DATA_SYNC:
		_handlerPetBaseDataSync(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_PET_STATUS_SYNC:
		_handlerPetStatusSync(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_PET_SYNC_SKILLASSET:
		_handlerPetSyncSkillAsset(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_PET_STAR_UPGRADE_REP:
		_handlerPetStarUpgradeResp(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_PET_DECOMPOSE_REP:
		_handlerPetDecomposeResp(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_PET_ENABLE_REP:
		_handlerPetEnableResp(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_PET_DISABLE_REP:
		_handlerPetDisableResp(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_OWNER_MP_INFO:
		_handlerSyncOwnerMPInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NEWVIPINFO:
		_handleSyncVipNewInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ADDREWARD:
		_handleSyncAddReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_NEWVIP_BONUS_CACHE_SYNC:
		_handleSyncVipNewBonus(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_NEWVIP_BEGIN_TIME_SYNC:
		_handleSyncVipNewBonusBeginTime(pkSession,pMsg,wSize);
		break;	
	case MGPT_SC_NEWVIP_REWARD_RESULT:
		_handleNewVipRewardResult(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCTESTDATABASE:
		_handleSyncTestDataBase(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCANSWERINFO:
		_handleSyncAnswerInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETNEARBYPLAYERINFO:
		_handleGetNearbyPlayerInfoAck(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_UPGRADECOMMANDERTITLE:
		_handleTakeCommanderTitleRewardAck(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCSPECIALTRAININGREWARD:
		_handleSyncSpecialTrainingReward(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETSPECIALTRAININGREWARD:
		_handleTakeSpecialTrainingRewardAck(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_QUICKRAID_NEW:
		_handleQuickRaidNewAck(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETCHAPTERRAIDSTARREWARD:
		_handleGetChapterRaidStarRewardAck(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCTOTALRAIDSTARNUM:
		_handleSyncTotalRaidGloryStarNum(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCCHAPTERRAIDSTARNUM:
		_handleSyncChapterRaidGloryStarNum(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCCHAPTERRAIDSTARREWARDRECORD:
		_handleSyncChapterRaidGloryRewardRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCUSEDRAIDCOUNT:
		_handleSyncQuickRaidGloryCount(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETYIJITUXISTAGEREWARD:
		_handleRltGetYiJiTuXiStageReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCYIJITUXIREWARDRECORD:
		_handleSyncYiJiTuXiRewardRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETYIJIXUNBAOSTAGEREWARD:
		_handleRltGetYiJiXunBaoStageReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCYIJIXUNBAOREWARDRECORD:
		_handleSyncYiJiXunBaoRewardRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETROTARYTABLEREWARD:
		_handleRltGetRotaryTableReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCROTARYTABLESKIPANIMATION:
		_handleSyncRotaryTableSkipAnimation(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCROTARYTABLEVAILDTIMES:
		_handleSyncRotaryTableVaildTimes(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCROTARYTABLEREWARDRECORD:
		_handleSyncRotaryTableRewardRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCROTARYTABLEENDTIME:
		_handleSyncRotaryEndTime(pkSession,pMsg,wSize);
		break;
		//极品白拿 
	case MGPT_RLT_BUYNONPAREILITEM: 
		_handlerGetBuyNoPayItem(pkSession,pMsg,wSize); 
	    break ; 
	case MGPT_RLT_GETNONPAREILRETURN:
		_handlderGetBuyNoPayReturn(pkSession,pMsg,wSize) ; 
		break ; 
	case MGPT_SYNCNONPAREILRETURNINFO:
		_handlderSyncBuyNoPayReturnInfo(pkSession,pMsg,wSize); 
		break ; 
	//case MGPT_SYNCNONPAREILRECORD:
	//	_handleSyncNonpareilRecord(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_SYNCNONPAREILRETURNRECORD:
	//	_handleSyncNonpareilReturnRecord(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_SYNCNONPAREILENDTIME:
	//	_handleSyncNonpareilEndTime(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_BUYNONPAREILITEM:
	//	_handleRltBuyNonpareilItem(pkSession,pMsg,wSize);
	//	break;
	//case MGPT_RLT_GETNONPAREILRETURN:
	//	_handleRltGetNonpareilReturn(pkSession,pMsg,wSize);
	//	break;
	case MGPT_RLT_BUYRARESUBDEVAEYETRANSFORM:
		_handleRltBuyRareSubDevaEyeTransform(pkSession,pMsg,wSize);
		break;
	case MGPT_GUILD_FIGHT_TOWER_INFO_NOTIFY:
		_handlerGuildBattleTowerInfo(pkSession, pMsg, wSize);
		break;
	case MGPT_GUILD_FIGHT_TOWER_INFO_NOTIFY_LITE:
		_handlerGuildBattleTowerInfoLife(pkSession, pMsg, wSize);
		break;
	case MGPT_GAMESTORY_KEY_ECOUNTED_STAT_NOTIFY_EX:
		_handlerGuildBattleTowerRankList(pkSession, pMsg, wSize);
		break;
	case MGPT_RLT_GUILDFIGHTRANKINFO:
		_handleRltGuildFightRankInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_FIGHTRECORD:
		_handleRespGuildBattleFightRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTICEBEDEFIGHTED:
		_handlerNoticeBeDefighted(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETREGULARBOSSKILLERINFO:
		_handleRltGetRegularBossKillerInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCBESTGUILDTASKINFO:
		_handleSyncBestGuildTaskInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETBESTGUILDREWARD:
		_handleRltGetBestGuildReward(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_GUILD_FIGHT_ROB_SCORE:
		_handleRltGuildFightRobScore(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_GUILD_FIGHT_ROB_SCORE_RECEIVE :
		_handleRltGuildFightRobScoreReceive(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_GUILD_FIGHT_GUILD_MEMBER_ID_INFO:
		_handleRltGuildFightMemberUID(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_GUILD_FIGHT_PLAYER_INFO:
		_handleRltGuildFightMemberInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_RANK_GUILDOFFLINE_CHALLENGE:
		_handleNotifyRankGuildOfflineChallenge(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_FASTAPPLYGUILD:
		_handleRltQuickApplyGuild(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETPETISLANDSSTAGEREWARD:
		_handleRltGetToyIslandStageReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCPETISLANDSREWARDRECORD:
		_handleSyncToyIslandRewardRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCESCORTNPCSTATE:
		_handlerSyncEscortNPCState(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCGOALRECORD:
		_handlerSyncGoalRecord(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCROBBINGTIMES:
		_handlerSyncRobbingTimes(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCESCORTTIMES:
		_handlerSyncEscortTimes(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCESCORTSTATE:
		_handlerSyncEscortState(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ADDESCORTREWARD:
		_handlerRLTAddEscortReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCFIRSTCHARGEINFO:
		_handlerSyncFirstChargeInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETFIRSTCHARGEREWARD:
		_handlerGetFirstChargeReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCVIPEXTRAREWARDUSEDTIMES:
		_handlerSyncVIPExtraRewardUsedTimes(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETVIPEXTRAREWARD:
		_handlerRLTGetVipExtraReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCMAPNPCINFO:
		_handlerSyncMapNpcInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCNEWACHIEVEMENTSTARNUM:
		_handlerSyncNewAchievementStarNum(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCNEWACHIEVEMENTREWARDRECORD:
		_handlerSyncNewAchievementRewardCord(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETNEWACHIEVEMENTREWARD:
		_handlerRLTGetNewAchievementReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCGUILDPRESENTINFO:
		_handlerSyncGuildProfitPresentInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SENDGUILDPRESENTSTATE:
		_handlerSendGuildProfitState(pkSession,pMsg,wSize);
		break;
	case MGPT_DELETEGUILDPRESENTINFO:
		_handlerDeleteGuildProfitPresentInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_ADDGUILDPRESENTINFO:
		_handlerAddGuildProfitPresentInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCGUILDPRESENTFLAG:
		_handlerSyncGuildProfitShowNameFlag(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETGUILDPRESENTREWARD:
		_handlerRltGetGuildProfitReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCGUILDCRYSTALINFO:
		_handlerSyncGuildProfitCrystalInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETGUILDCRYSTALREWARD:
		_handlerRltGetGuildCrystalReward(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTICENEWACHIEVEMENT:
		_handlerNoticeNewAchievement(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFYGUILDMAPBOSSINFO:
		_handlerNotifyGuildMapBossInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCGUILDMAPFINISHMAP:
		_handlerSyncGuildMapFinishMap(pkSession,pMsg,wSize);
		break;
	case MGPT_UPDATEGUILDMAPBOSSINFO:
		_handlerUpdateGuildMapBossInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCNEWOPENGUILDMAPBOSS:
		_handlerSyncGuildBossActivityOpen(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_ACTIVITYGUILDBOSSMAP:
		_handlerRltGuildBossActivityOpen(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETTREASUREPOS:
		_handlerRltGetTreasurePos(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCREWARD:
		_handlerSyncReward(pkSession,pMsg,wSize);
		break;

	case MGPT_RLT_GETGAMERESTARTREWARD:
		_handlerRltGetGameRestartReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNCGAMERESTARTNOTICE:
		_handlerSyncGameRestartNotice(pkSession,pMsg,wSize);
		break;

	case MGPT_SC_SendEntrustTask_Ret:
		_handleSyncSendEntrustTask_Ret(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_AccpetEntrustTask_Ret:
		_handleAccpetEntrustTask_Ret(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SpeedUpEntrustTask_Ret:
		_handleSpeedUpEntrustTask_Ret(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_GetAwardEntrustTask_Ret:
		_handleGetAwardEntrustTask_Ret(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_UpdateEntrustTask_Ret:
		_handleRefreshEntrustTask_Ret(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_UpdateEntrustTaskNum_Ret:
		_handle_UpdateEntrustTaskNum_Ret(pkSession,pMsg,wSize);
		break;
	//新手帮助
	
	case MGPT_RLT_ADDNOVICETUTORIAL : 
		_handlerRltAddNoviceTutorial(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_GETNOVICETUTORIALREWARD: 
		_handlerRltGetNoviceTutorialReward(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_NOVICETUTORIALINFO: 
		_handlerSyncNoviceTutorialInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RLT_SETMAPBOSSFLAG:
		_handle_SetMapBossFlag_Rlt(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SETMAPBOSSFLAG:
		_handle_MapBossFlagInfo_Sync(pkSession,pMsg,wSize);
		break;

	//苍穹段位奖励 
	case MGPT_SC_GetSkyArena_GradeAward: 
		_handlerResp_SkyArenaGradeAward(pkSession,pMsg,wSize); 
		break;
	case MGPT_SC_GetSkyArena_GradeAwardInfo:
		_handlerResp_SkyArenaMyScore(pkSession,pMsg,wSize) ; 
		break;
	
	//苍穹对战信息
	case MGPT_RESP_GETPLAYERSCHEMEDATA:
		_handlerRespGetPlayerSchemeData(pkSession,pMsg,wSize);
		break;

	case MGPT_RESP_GETRANKTYPEINFO:
		_handlerRespGetRankTypeInfo(pkSession,pMsg,wSize);
		break;

	case MGPT_SC_SkyArena_ChallengeResult:
		_handler_SkyArenaChallengeResult(pkSession,pMsg,wSize);
		break;

	//巅峰竞技场 
	case  MGPT_SC_SUMMIT_ARENA_INFO_RESP:
		_handlerRespSummitArenaInfo(pkSession,pMsg,wSize); 
		break; 
	case  MGPT_NOTIFY_SUMMIT_ARENA_TIME : 
		_handlerRespSummitTimeData(pkSession,pMsg,wSize) ; 
		break;
	case  MGPT_SC_SUMMIT_ARENA_ENTRYLIST_RESP:
		_handerRespSummitArenaGroupStageEntryList(pkSession,pMsg,wSize);
		break ; 
	case  MGPT_SC_SUMMIT_ARENA_GROUPSTAGE_RESP: 
		_handlerRespSummitArenaGroupStage(pkSession,pMsg,wSize) ; 
	  break;
	case  MGPT_SC_SUMMIT_ARENA_MY_INFO_RESP: 
		_handlerRespSummitArenaMyGroupStage(pkSession,pMsg,wSize) ; 
		break;
	case  MGPT_SC_SUMMIT_ARENA_AGAINSTLIST_RESP:
		_handlerRespSummitArenaAgainstList(pkSession,pMsg,wSize); 
		break ; 
	case  MGPT_NOTIFY_SUMMIT_ARENA_REDPOINT: 
		_handlerRespSummitRedPoint(pkSession,pMsg,wSize); 
		break ; 
	case  MGPT_SC_NOTIFY_SUMMITARENA_MY_TOPSTAGEID:
		_handlerRespSummitTopType(pkSession,pMsg,wSize) ;
		break ; 
	case MGPT_SC_NOTIFY_ACTIVATE_SUMMITARENA:
		_handlerRespSummitInvitationValid(pkSession,pMsg,wSize); 
		break; 
	//竞猜
	case MGPT_RESP_GETSUMMITSINGLEDATA:
		_handlerResp_GetSummitSingleData(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_SETSUMMITGUESSINFO:
		_handlerResp_SetSummitGuessInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_GETSUMMITSUPPORTINFO:
		_handlerResp_GetSummitSupportInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_GETSUMMITWINER :
		_handlerResp_GetSummitWiner(pkSession,pMsg,wSize);
		break;
	case MGPT_SYNC_SUMMITDATA :
		_handlerSyncSummitData(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SUMMIT_ARENA_HONOR_RESP :
		_handlerRespSummitArenaHonor(pkSession,pMsg,wSize);
		break;
	case MGPT_SC_SUMMITARENA_MY_HONORTOPID_RESP:
		_handlerRespSummitArenaMyTopHonor(pkSession,pMsg,wSize);
		break;
	case MGPT_NOTIFY_SUMMITARENA_GUESSCLOSE :
		_handlerMgptSummitArenGuessClose(pkSession,pMsg,wSize);
		break;
	//异种降临
	case MGPT_RLT_GETGUILDDAMAGERANK:
		_handlerRltGetGuildDamageRank(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_DAMAGERANKINFO:
		_handlerRespDamageRankInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_GETGUILDDAMAGERANKINFO:
		_handlerRespGuildDamageRankInfo(pkSession,pMsg,wSize);
		break;
	case MGPT_RESP_BONFIREPARTY_BUYLEFTITEMTIMES:
		_handlerBonFirePartyLeftBuyItemTimes(pkSession,pMsg,wSize);
		break; 
	case MGPT_RESP_BONFIREPARTY_BUYLEFTITEM:
		_handlerBonFirePartyBuyInfo(pkSession,pMsg,wSize);
		break; 
	default:
		//T_ASSERT(false;) 
		break;
	}

// 	if (bCompressed)
// 	{
// 		T_FREE(pMsg,  Memory::MEMCATEGORY_GENERAL);
// 	}
}


void HL_FromGameServer::_handlerSyncRedPointFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRedPointFlag msg;
	
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncRedPointFlag解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRedPointFlag(msg.m_nSyncType,msg.m_RedPoint);
}

void HL_FromGameServer::_handlerRltSetSubtransFormFlag(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize)
{
	MG_RLT_SetSubTransformFlag msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SC_UpdateEntrustTaskNum_Ret解析错误");
		return;
	}

	if (msg.m_nResult==RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->UpdateCimeliaGainNew(msg.m_nSubType,msg.m_nTransformID,msg.m_bIsNew);
		pkGamePlayer->AddDirtyFlagRevisionID(DIRTY_FLAG_SUBORDINATE_INFO);
	}
	else
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	
}

//------------------------------------------------------------------------------
void HL_FromGameServer::_handlerWarehouseProtocol(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize )
{
	NW_ClientSessionManager* pkSessionManager		= NW_ClientSessionManager::GetSingletonPtr();
	NW_ClientSessionGroup*	 pkClientSessionGroup	= pkSessionManager->GetSessionGroup(pkSession);
	int eGameStage = pkClientSessionGroup->m_pkGameStageMachine->GetCurrentStateType();

	//如果还未进入游戏，则退出
	if (eGameStage != ST_GAME_STAGE_GAME)
		return;


	ResultType iResult = RE_FAIL; 

	switch(iMsgType)
	{
	case MGPT_RLT_SAVECASH_TO_WAREHOUSE:
		{
			MG_RLT_SaveCashToWarehouse rltMsg;
			if ( rltMsg.Decode(pMsg, wSize) )
			{
				iResult = rltMsg.m_iResult;
			}
		}
		break;
	case MGPT_RLT_GETCASH_FROM_WAREHOUSE:
		{
			MG_RLT_GetCashFromWarehouse rltMsg;
			if ( rltMsg.Decode(pMsg, wSize) )
			{
				iResult = rltMsg.m_iResult;
			}
		}
		break;
	case MGPT_RLT_EXPAND_WAREHOUSE:
		{
			MG_RLT_ExternWarehouse rltMsg;
			if (rltMsg.Decode(pMsg, wSize))
			{
				iResult = rltMsg.m_iResult;
			}
		}
		break;
	}

	SendLogicInfoMessage(iResult);
}




void HL_FromGameServer::_handlerRespChallengGiftState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SS_ChallengGiftState rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_GG_RESP_CHALLENGGIFTSTATE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, rltMsg.m_nRankIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, rltMsg.m_nGiftState);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

//-------------------------------------------------------
void HL_FromGameServer::_handleGreatKungFuOperationResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GreatKungFuOperationRESP resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;
	
	if (resqMsg.m_uiOPRslt != RE_SUCCESS)
	{
		SendLogicInfoMessage(resqMsg.m_uiOPRslt);
	}
};

void HL_FromGameServer::_handlerRltOpenAbility( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize )
{
	MG_RespOpenAbility resqMsg;
	if (!resqMsg.Decode(pMsg,wSize))
		return;
	if (resqMsg.m_Result != RE_SUCCESS)
	{
		SendLogicInfoMessage(resqMsg.m_Result);
	}
	else if(resqMsg.m_iID == OPEN_ABILITY_ID_PET)
	{
		LC_ClientToyPetManager::GetSingletonPtr()->SetActive(true);
	}

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_OPENABILITY);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, resqMsg.m_Result);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, resqMsg.m_iID);

	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerNotifyServer_ErrorCode(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_ServerErrorCode msg;
	if(!msg.Decode(pMsg, wSize))
	{
		return;
	}

	SendLogicInfoMessage(msg.m_nErrorCode);
}

void HL_FromGameServer::_handleGameNoticeMsg(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_GameNotice msg;
	if(!msg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( pkGamePlayer)
	{
		//篝火晚会购买倍率公告 只有在篝火地图的玩家才显示
		if( msg.m_nID == 1202
			&& !IS_GUILDPARTY_MAP(pkGamePlayer->GetMapLogicType()))
			return;
		if(msg.m_nID==1202){
			StringStreamType sStream("") ;
			sStream<<msg.m_strArgs[0];
			StringType & name=sStream.str(); 
			pkGamePlayer->ReqBonFirePartyLeftBuyItemTimes();
			pkGamePlayer->SetBuyerInfo(msg.m_numArgs); 
			pkGamePlayer->SetBuyerName(name); 
			LC_GameEvent kGameEvent;
			kGameEvent.SetEventType(GLET_GET_BONFIRE_PARTY_BUYPERSONINFO);
			UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
			pkUIManager->HandlerGameEvent(kGameEvent);
			return ; 
		}
		LC_NoticeAnalysisMgr *pkNoticeMgr = LC_NoticeAnalysisMgr::GetSingletonPtr();
		if(pkNoticeMgr)
		{
			pkNoticeMgr->ShowNoticeMessage(pkGamePlayer, msg);
		}
	}
}


void HL_FromGameServer::_handleDebugReloadNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_DebugReloadNotify msg;
	if(!msg.Decode(pMsg, wSize))
	{
		return;
	}

	ClientConfigReloadManager *pkConfigReloadManager = (ClientConfigReloadManager*)ClientConfigReloadManager::GetSingletonPtr();
	if (pkConfigReloadManager)
	{
		TStringVector kReloadFileList;
		Protocol::PS_FileInfoList::const_iterator it = msg.m_kCSVFiles.begin();

		for (;it != msg.m_kCSVFiles.end(); ++it)
		{
			kReloadFileList.push_back(UT_BaseStringUtility::SplitFileName(STDSTR2TPSTR(it->m_sName)).second);
		}
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "_handleDebugReloadNotify, ResVersion=%s, ReloadFileCount=%d", msg.m_strResVersion.c_str(), kReloadFileList.size());
		pkConfigReloadManager->SetResVersion(STDSTR2TPSTR(msg.m_strResVersion), kReloadFileList, msg.m_uiTimestamp);
	}
}

void HL_FromGameServer::_handleSyncVipNewInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Sync_NewVipInfo msg;
	if(!msg.Decode(pMsg, wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if(!pkGamePlayer)
	{
		return;
	}
	pkGamePlayer->UpdateVipNewRewardRecord(msg.m_nVipRewardRecord);
	bool bLevelUp=pkGamePlayer->UpdateVipNewLevel(msg.m_nNewVipLevel);
	pkGamePlayer->UpdateVipNewOpen(msg.m_bState);

	
	LC_GameEvent kGameEvent;
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	if(bLevelUp)
	{
		kGameEvent.SetEventType(GLET_CLIENT_VIP_NEW_LEVELUP);
		pkUIManager->HandlerGameEvent(kGameEvent);
		pkGamePlayer->ReqAcceptVipNewLevelTasks(msg.m_nNewVipLevel);
	}
	kGameEvent.SetEventType(GLET_CLIENT_VIP_NEW_UPDATE_INFO);	
	pkUIManager->HandlerGameEvent(kGameEvent);

}

void HL_FromGameServer::_handleSyncAddReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddReward msg;
	if(!msg.Decode(pMsg, wSize))
	{
		return;
	}
	
	if(msg.m_nResult == RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		if(!pkGamePlayer)
		{
			return;
		}
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_VIP_NEW_UPDATE_INFO);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}	
	else
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handleSyncVipNewBonus(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{

	GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "_handleSyncVipNewBonus");
	MG_SC_NewVIPBonusCacheSync msg;
	if(!msg.Decode(pMsg, wSize))
		return;
	for (int i=0;i<msg.m_nCount;i++)
	{
		const NewVIPBonusData& pData=msg.m_Data[i];
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "_handleSyncVipNewBonus ItemId:%d  itemCount:%d",pData.m_nItemId,pData.m_nCount);
	}
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		GfxWriteLog(LOG_TAG_SYSTEM, LOG_SWITCH_SYSTEM, "_handleSyncVipNewBonus NoPkGamePlayer");
		return ;
	}
	pkGamePlayer->RltSyncVipNewProfitData(msg);
}

void HL_FromGameServer::_handleSyncVipNewBonusBeginTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_NewVIPSync_BeginTime rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	MG_SC_BotDataSync_BeginTime botMsg;
	botMsg.m_nBeginTime=rltMsg.m_nBeginTime;
	pkGamePlayer->RltSyncBotBeginTime(botMsg);
}

void HL_FromGameServer::_handleNewVipRewardResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_NewVIPRewardResult rltMsg;
	if(!rltMsg.Decode(pMsg, wSize))
		return;
	
	if(rltMsg.m_nError != RE_SUCCESS)
	{
		SendLogicInfoMessage(rltMsg.m_nError);
	}
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->ReqGetBotProfit();

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_VIP_NEW_GET_BONUS_SUCCESS);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handleGetNearbyPlayerInfoAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetNearbyPlayer msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->UpdateNearbyPlayer(msg.m_nNearbyPlayer);
}

void HL_FromGameServer::_handleTakeCommanderTitleRewardAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_UpgradeCommanderTitle msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	if( NULL == pkGamePlayer )
	{
		return ;
	}
	pkGamePlayer->SetCommanderTitleLevel(msg.m_nCurrentCommanderTitle);

	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	LC_GameEvent kGameEvent;
	if (msg.m_nResult==RE_SUCCESS)
	{
		kGameEvent.SetEventType(GLET_CLIENT_COMMANDER_TITLE_TAKEREWARD);
	}
	else
	{
		kGameEvent.SetEventType(GLET_CLIENT_COMMANDERTITLE_TAKEREWARDFAIL);
		SendLogicInfoMessage(msg.m_nResult);
	}
	if(pkUIManager)
	{
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handleSyncSpecialTrainingReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSpecialTrainingReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	std::vector<int32_t>& IDs=msg.m_nSpecialTrainingIDs;
	int iLevel=0;
	for (int i=0;i<IDs.size();i++)
	{
		if (iLevel<IDs[i])
		{
			iLevel=IDs[i];
		}
	}
	iLevel++;
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateSpecialTrainingLevel(iLevel);
	pkGamePlayer->CheckCommanderTitleOpen();

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SPECIAL_TRAINING_LEVEL_UPDATED);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleTakeSpecialTrainingRewardAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetSpecialTrainingReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult!=RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handleQuickRaidNewAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_QuickRaid_New msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult!=RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		SendLogicInfoMessage(RE_QUICKRAID_SUCCESS);
	}
}

void HL_FromGameServer::_handleGetChapterRaidStarRewardAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetChapterRaidStarReward msg;

	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult!=RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{

	}
}

void HL_FromGameServer::_handleSyncTotalRaidGloryStarNum(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncTotalRaidStarNum msg;

	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRaidGloryTotalStarNum(msg.m_nTotalStarNum);
}

void HL_FromGameServer::_handleSyncChapterRaidGloryStarNum(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncChapterRaidStarNum msg;

	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRaidGloryChapterStarNum(msg.m_nChapterID,msg.m_nChapterStarNum);
}

void HL_FromGameServer::_handleSyncChapterRaidGloryRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncChapterRaidStarRewardRecord msg;

	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateGloryChapterRewardRecord(msg.m_nChapterID,msg.m_nRewardIndex);
}

void HL_FromGameServer::_handleSyncQuickRaidGloryCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncUsedRaidCount msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateQuickRaidGloryCount(msg.m_nUsedRaidTimesRecords);
}

void HL_FromGameServer::_handleRltReceiveGloryDailyReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MGPT_RLT_Daily_Reward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	if (msg.m_nResult == RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->SetGloryDailRewardDrawed(msg.m_DailyReward);

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_GLORY_DAILYREWARD);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handleSyncReceiveGloryDailyReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MGPT_Syc_Daily_Reward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetGloryDailRewardDrawed(msg.m_DailyReward);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_GLORY_DAILYREWARD);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltGetToyIslandStageReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetPetIslandsStageReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult == RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->UpdateToyIslandReward(msg.m_nID);
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_YIJIRAID_NEW_REWARD);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.m_nResult);
	}

}

void HL_FromGameServer::_handleSyncToyIslandRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncPetIslandsRewardRecord msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateToyIslandReward(msg.m_nRewardIDRecord);
}

void HL_FromGameServer::_handleRltGetYiJiTuXiStageReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetYiJiTuXiStageReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult == RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->UpdateYijiTuxiReward(msg.m_nID);
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_YIJIRAID_NEW_REWARD);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	
}

void HL_FromGameServer::_handleSyncYiJiTuXiRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncYiJiTuXiRewardRecord msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateYijiTuxiReward(msg.m_nRewardIDRecord);
}

void HL_FromGameServer::_handleRltGetYiJiXunBaoStageReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetYiJiXunBaoStageReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult == RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->UpdateYijiXunbaoReward(msg.m_nID);
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_YIJIRAID_NEW_REWARD);
		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handleSyncYiJiXunBaoRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncYiJiXunBaoRewardRecord msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateYijiXunbaoReward(msg.m_nRewardIDRecord);
}

void HL_FromGameServer::_handleRltGetRotaryTableReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetRotaryTableReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_RLT_GETROTARYTABLEREWARD result = %d,m_nRewardIndex = %d",msg.m_nResult,msg.m_nRewardIndex);
	if (msg.m_nResult==RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->UpdateRotaryRewardIdx(msg.m_nRewardIndex);
		
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_ROTARY_START);

		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handleSyncRotaryTableSkipAnimation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRotaryTableSkipAnimation msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRotarySkipAnimSetting(msg.m_bSkipAnimation);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_CLIENT_ROTARY_SKIPANIM_SETTING);

	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleSyncRotaryTableVaildTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRotaryTableVaildTimes msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRotaryTableVaildTimes(msg.m_nVaildTimes);
}

void HL_FromGameServer::_handleSyncRotaryTableRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRotaryTableRewardRecord msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRotaryRewardRecord(msg.m_nRewardIndexs);
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SYNCROTARYTABLEREWARDRECORD");
}

void HL_FromGameServer::_handleSyncRotaryEndTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRotaryTableEndTime msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRotaryEndTime(msg.m_nRotaryTableEndTime);
	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SYNCROTARYTABLEENDTIME m_nRotaryTableEndTime = %d",msg.m_nRotaryTableEndTime);
}

//void HL_FromGameServer::_handleSyncNonpareilRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
//{	
//	MG_SyncNonpareilRecord msg;
//	if (!msg.Decode(pMsg,wSize))
//	{
//		return;
//	}
//	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//	pkGamePlayer->UpdateBuyNonpareilRecord(msg);
//}
//
//void HL_FromGameServer::_handleSyncNonpareilReturnRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
//{
//	MG_SyncNonpareilReturnRecord msg;
//	if (!msg.Decode(pMsg,wSize))
//	{
//		return;
//	}
//	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//	pkGamePlayer->UpdateNonpareilReturnRecord(msg.m_nNonpareilReturnRecord);
//}
//
//void HL_FromGameServer::_handleRltBuyNonpareilItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
//{
//	MG_RLT_BuyNonpareilItem msg;
//	if (!msg.Decode(pMsg,wSize))
//	{
//		return;
//	}
//	if (msg.m_nResult==RE_SUCCESS)
//	{
//	}
//	else
//	{
//		SendLogicInfoMessage(msg.m_nResult);
//	}
//}
//
//void HL_FromGameServer::_handleRltGetNonpareilReturn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
//{
//	MG_RLT_GetNonpareilReturn msg;
//	if (!msg.Decode(pMsg,wSize))
//	{
//		return;
//	}
//	if (msg.m_nResult==RE_SUCCESS)
//	{
//	}
//	else
//	{
//		SendLogicInfoMessage(msg.m_nResult);
//	}
//}
//
//void HL_FromGameServer::_handleSyncNonpareilEndTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
//{
//	MG_SyncNonpareilEndTime msg;
//	if (!msg.Decode(pMsg,wSize))
//	{
//		return;
//	}
//	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
//	pkGamePlayer->UpdateBuyNonpareilEndTime(msg.m_nNonpareilEndTime1,msg.m_nNonpareilEndTime2);
//	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MGPT_SYNCNONPAREILENDTIME m_nNonpareilEndTime1 = %d , m_nNonpareilEndTime2 = %d",msg.m_nNonpareilEndTime1,msg.m_nNonpareilEndTime2);
//}

void HL_FromGameServer::_handleRltBuyRareSubDevaEyeTransform(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_BuyRareSubDevaEyeTransform msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult == RE_SUCCESS)
	{
		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_BUY_RARESUB);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nShopID);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
		SendLogicInfoMessage(msg.m_nResult);
}

void HL_FromGameServer::_handleSyncBestGuildTaskInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBestGuildTaskInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateBestGuildTaskInfo(msg.m_nBestGuildTaskCount,msg.m_nBestGuildRewardFlag);
}

void HL_FromGameServer::_handleRltGetBestGuildReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetBestGuildReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handleRltGuildFightRobScore(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_GuildFightRobScore_Opt msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_uiResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_uiResult);
		return;
	}

	StringStreamType sStream;
	const std::vector<int32_t>& receivedVec = msg.m_ReceivedInfo.m_ReceivedVec;
	sStream << msg.m_ReceivedInfo.m_WonNum << '#' << receivedVec.size() << '#';

	std::vector<int32_t>::const_iterator iter = receivedVec.begin();
	for( ; iter!=receivedVec.end(); ++iter)
	{
		sStream << *iter << '#';
	}

	const GuildRobScoreInfo& robScoreInfo = msg.m_RobScoreInfo;
	sStream << robScoreInfo.m_FreeNum << '#' << robScoreInfo.m_PayingNum << '#' << robScoreInfo.m_PaidNum;

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_GUILDBATTLE_SYNC_ROBSCORE);

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltGuildFightRobScoreReceive(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_GuildFightRobScoreReceive msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_uiResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_uiResult);
		return;
	}

	StringStreamType sStream;
	const std::vector<int32_t>& receivedVec = msg.m_ReceivedInfo.m_ReceivedVec;
	sStream << msg.m_ReceivedInfo.m_WonNum << '#' << receivedVec.size() << '#';

	std::vector<int32_t>::const_iterator iter = receivedVec.begin();
	for( ; iter!=receivedVec.end(); ++iter)
	{
		sStream << *iter << '#';
	}

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_GUILDBATTLE_SYNC_ROBSCORE);

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltGuildFightMemberUID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_GuildFightGuildMemberIdInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	
	StringStreamType sStream;
	sStream << msg.m_nGuild << '@';

	std::vector<uint64_t>::const_iterator iter = msg.m_IdVec.begin();
	for(; iter != msg.m_IdVec.end(); ++iter)
	{
		sStream << *iter << '#';
	}

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_BESTGUILD_GUILD_MEMBER_UIDS);

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltGuildFightMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_GuildFightPlayerInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if(msg.m_uiResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_uiResult);
		return;
	}

	StringStreamType sStream;
	sStream << (int)msg.m_End << '|';

	std::vector<PS_Guild_Fight_User_Info>::const_iterator iter = msg.m_PlayerInfo.begin();
	for( ; iter!=msg.m_PlayerInfo.end(); ++iter)
	{
		const Ps_Controller_Base_Info& kInfo = iter->m_ControllerInfo;
		sStream << kInfo.m_CharID << '#' << kInfo.m_nBornType << '#' << kInfo.m_nCommanderTitleID << '#' << kInfo.m_sCharName << '#';
		sStream << kInfo.m_nCommanderTitleFrameID << '#' << kInfo.m_nLevel << '#' << kInfo.m_nTotalPoint << '#' << kInfo.m_nRobbedNum << '#';

		const std::vector<PS_Buddy_Info_BuddyCS>& buddyVec = iter->m_nBuddyInfo;
		sStream << buddyVec.size() << '#';
		std::vector<PS_Buddy_Info_BuddyCS>::const_iterator buddyItr = buddyVec.begin();
		for( ; buddyItr!=buddyVec.end(); ++buddyItr)
		{
			const PS_Buddy_Info_BuddyCS& buddyInfo = *buddyItr;
			sStream << buddyInfo.m_nBuddyID << '@' << buddyInfo.m_nBuddyLevel << '@' << buddyInfo.m_nBuddyStarLevel << '@';
			sStream << (int)buddyInfo.m_bActivateAllSkill << '@' << buddyInfo.m_nAllEquipSlotReinforceLevel << '@';
			sStream << buddyInfo.m_nBuddyCombatScore << '@' << buddyInfo.m_nBuddyTransformID << '@';

			sStream << buddyInfo.m_nBuddyRuneInfo.size() << '@';
			std::map<int32_t,PS_RuneItemInfo_BuddyCS>::const_iterator runeItr = buddyInfo.m_nBuddyRuneInfo.begin();
			for( ; runeItr!=buddyInfo.m_nBuddyRuneInfo.end(); ++runeItr)
			{
				const PS_RuneItemInfo_BuddyCS& runeInfo = runeItr->second;
				sStream << runeItr->first << '_' << runeInfo.m_nRuneItemID << '_' << runeInfo.m_nRuneStarLevel << '@';
			}

			sStream << buddyInfo.m_nEquipSlotInfo.size() << '@';
			std::map<int32_t,PS_EquipItemInfo_BuddyCS>::const_iterator equipItr = buddyInfo.m_nEquipSlotInfo.begin();
			for( ; equipItr!=buddyInfo.m_nEquipSlotInfo.end(); ++equipItr)
			{
				sStream << equipItr->first << '_' << equipItr->second.m_nEquipItemID << '@';
			}
			sStream << '#';
		}
		sStream << '|';
	}

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::CGET_BESTGUILD_GUILD_MEMBER_INFO);

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleNotifyRankGuildOfflineChallenge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NOTIFY_GuildOfflineChallenge msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	GfxWriteLog(LOG_TAG_GAME_LOG, LOG_SWITCH_GAME,"[RaidMap]HL_FromGameServer::_handleNotifyRankGuildOfflineChallenge Success=%d AccountPoint=%d CurrentGuildRankIndex=%d CurrentRankIndex=%d LastGuildRankIndex=%d LastRankIndex=%d", 
		msg.m_nSuccess, msg.m_nAccountPoint, msg.m_nCurrentGuildRankIndex, msg.m_nCurrentRankIndex, msg.m_nLastGuildRankIndex, msg.m_nLastRankIndex);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_NOTIFY_GUILD_OFFLINE_CHALLENGE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nSuccess);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nAccountPoint);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_nCurrentGuildRankIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, msg.m_nCurrentRankIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_5, msg.m_nLastGuildRankIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_6, msg.m_nLastRankIndex);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRltQuickApplyGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_FastApplyGuild msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handlerRLTAddEscortReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_AddEscortReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
}


//enum EscortType 1.截取成功 2.被劫，护送失败 3.护送成功
void HL_FromGameServer::_handlerSyncEscortState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncEscortState msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	
	StringStreamType sStream;
	sStream << msg.m_nEscortType << "#" << msg.m_nNpcID << "#"<<msg.m_nPlayer;
	LC_ClientGameEvent kCGameEvent;
	kCGameEvent.SetClientEventType(GLET_NOTIFY_ESCORT_RESULT);
	LC_ClientGameEventUserData_String kUserData(sStream.str());
	kCGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kCGameEvent);
}

void HL_FromGameServer::_handlerSyncRobbingTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncRobbingTimes msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRobbingTimes(msg.m_nUsedRobbingTimes);
}

void HL_FromGameServer::_handlerSyncEscortTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncEscortTimes msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateEscortTimes(msg.m_nUsedEscortTimes);
}

void HL_FromGameServer::_handlerSyncEscortNPCState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncEscortNpcState msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateEscortState(msg.m_nEscortNpcState);
}

void HL_FromGameServer::_handlerSyncGoalRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncGoalRecord msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateBestGuildGoalRecord(msg.m_nGoalInfoRecord);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_SYNC_BESTGUILDGOALINFO);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerSyncFirstChargeInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncFirstChargeInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateFirstChargeTime(msg.m_nFirstChargeTime);
	pkGamePlayer->UpdateFirstChargeRewardRecord(msg.m_nFirstChargeRewardState);
}

void HL_FromGameServer::_handlerGetFirstChargeReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetFirstChargeReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->UpdateFirstChargeRewardRecord(msg.m_nID);

		LC_GameEvent GameEvent;
		GameEvent.SetEventType(GLET_SYNC_FIRSTCHARGEREWARD);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}
}

void HL_FromGameServer::_handlerSyncVIPExtraRewardUsedTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncVIPExtraRewardUsedTimes msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateVipQuickHookTime(msg.m_nUsedTimes);
}

void HL_FromGameServer::_handlerRLTGetVipExtraReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetVIPExtraReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->UpdateVipQuickHookReward(msg.m_nItemInfo);
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GLET_RLT_GETVIPEXTRAREWARD);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerSyncNewAchievementStarNum(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncNewAchievementStarNum msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncNewAchievementStarNum解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetNewAchievementStarNum(msg.m_nNewAchievementStarNum);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_SYNC_NEWACHIEVEMENTSTARNUM);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerSyncNewAchievementRewardCord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncNewAchievementRewardRecord msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncNewAchievementRewardRecord解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetNewAchievementRewardCord(msg.m_nID);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_SYNC_NEWACHIEVEMENTREWARD);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerNoticeNewAchievement(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NoticeNewAchievement msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_NoticeNewAchievement解析错误");
		return;
	}

	for (int i=0;i<msg.m_nAchievementSet.size();i++)
	{
		LC_GameEvent GameEvent;
		GameEvent.SetEventType(GLET_NOTICE_NEWACHIEVEMENT);
		GameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nAchievementSet[i]);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}

}

void HL_FromGameServer::_handlerSyncGuildProfitPresentInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncGuildPresentInfo msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncGuildPresentInfo解析错误");
		return;
	}

	LC_ClientGuildProfitManager *pkGuildProfitManager = LC_ClientGuildProfitManager::GetSingletonPtr();
	pkGuildProfitManager->SyncProfitRecord(msg.m_nGuildPresentInfo);
}

void HL_FromGameServer::_handlerSendGuildProfitState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SendGuildPresentState msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SendGuildPresentState解析错误");
		return;
	}
	LC_ClientGuildProfitManager *pkGuildProfitManager = LC_ClientGuildProfitManager::GetSingletonPtr();
	if (msg.m_nState == SendState_Start)
	{
		pkGuildProfitManager->Reset();
	}
	else if (msg.m_nState == SendState_End)
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"公会礼物 _handlerSendGuildProfitState 发送完毕");
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		unique_id_type lCharID = pkLogicManager->GetOperationCharID();
		pkGuildProfitManager->LoadFile(lCharID);
		pkGuildProfitManager->SortProfitRecord(true, true);

		LC_GameEvent GameEvent;
		GameEvent.SetEventType(GLET_GUILDPROFIT_PRESENTINFO);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}
}

void HL_FromGameServer::_handlerDeleteGuildProfitPresentInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_DeleteGuildPresentInfo msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_DeleteGuildPresentInfo解析错误");
		return;
	}

	LC_ClientGuildProfitManager *pkGuildProfitManager = LC_ClientGuildProfitManager::GetSingletonPtr();
	bool bNormalChange = pkGuildProfitManager->DelProfitRecord(msg.m_ID);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDPROFIT_PRESENTINFOCHANGE);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_1, bNormalChange);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerAddGuildProfitPresentInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_AddGuildPresentInfo msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_AddGuildPresentInfo解析错误");
		return;
	}

	LC_ClientGuildProfitManager *pkGuildProfitManager = LC_ClientGuildProfitManager::GetSingletonPtr();
	bool bNormalChange = pkGuildProfitManager->AddProfitRecord(msg);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDPROFIT_PRESENTINFOCHANGE);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_1, bNormalChange);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerSyncGuildProfitShowNameFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncGuildPresentFlag msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncGuildPresentFlag解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetGuildProfitShowName(msg.m_nShowGuildPresentName);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDPROFIT_SHOWNAMESETTING);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerRltGetGuildProfitReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetGuildPresentReward msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_GetGuildPresentReward解析错误");
		return;
	}

	SendLogicInfoMessage(msg.m_nResult);
	if (msg.m_nResult != RE_SUCCESS && msg.m_nResult != RE_GUILDPRESENT_ERROR_14)
	{
		return;
	}

	LC_ClientGuildProfitManager *pkGuildProfitManager = LC_ClientGuildProfitManager::GetSingletonPtr();
	bool bNormalChange = pkGuildProfitManager->AddProfitRecord(msg.m_nIDRewardInfo);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDPROFIT_PRESENTINFOCHANGE);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_1, bNormalChange);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_2, 1);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);

	pkGuildProfitManager->SaveFile();
}

void HL_FromGameServer::_handlerSyncGuildProfitCrystalInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncGuildCrystalInfo msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncGuildCrystalInfo解析错误");
		return;
	}

	GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"公会礼物  _handlerSyncGuildProfitCrystalInfo， 礼物等级=%d, 礼物等级经验=%d, 水晶等级=%d, 水晶等级经验=%d", msg.m_nLevel1, msg.m_nGuildPresentExp1Num, msg.m_nLevel2, msg.m_nGuildPresentExp2Num);
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetGuildProfitLevel(msg.m_nLevel1);
	pkGamePlayer->SetGuildProfitLevelExp(msg.m_nGuildPresentExp1Num);
	pkGamePlayer->SetGuildProfitCrystalLevel(msg.m_nLevel2);
	pkGamePlayer->SetGuildProfitCrystalLevelExp(msg.m_nGuildPresentExp2Num);
	pkGamePlayer->SyncGuildProfitCrystalInfo(msg.m_nGuildCrystalInfo);
}

void HL_FromGameServer::_handlerRltGetGuildCrystalReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetGuildCrystalReward msg;
	if (!msg.Decode(pMsg, wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_GetGuildCrystalReward解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handleSyncGloryPointRecover(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncYuanZhengRevertInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncYuanZhengRevertInfo解析错误");
		return;
	}

	
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateRaidGloryPointRecoverTimestamp(msg.m_nNextRevertTime,msg.m_nEndRevertTime);
	
}

void HL_FromGameServer::_handleRLT_GetSubBValue(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetSubBValue msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_GetSubBValue解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	} 
	else
	{
		LC_ClientActivityManager::GetSingletonPtr()->UpdateSubBValue(msg.m_nSubBValue);
	}
	
}

void HL_FromGameServer::_handleSyncSendEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_SendEntrustTask_Ret msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SC_SendEntrustTask_Ret解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateEntrustTaskInfo(msg.m_AccpetedTasks,msg.m_Tasks,msg.m_Num);
}

void HL_FromGameServer::_handleAccpetEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_AccpetEntrustTask_Ret msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SC_AccpetEntrustTask_Ret解析错误");
		return;
	}
	if (msg.result==RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->OnAcceptEntrustTask(msg.m_Task);

		StringStreamType sStream("");
		sStream<<msg.m_Task.m_Index;
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GLET_NOTIFY_ACCEPT_ENTRUST_TASK);

		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.result);
	}
}

void HL_FromGameServer::_handleSpeedUpEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_SpeedUpEntrustTask_Ret msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SC_SpeedUpEntrustTask_Ret解析错误");
		return;
	}
	if (msg.result==RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->OnFinishedEntrustTask(msg.m_Task);

		StringStreamType sStream("");
		sStream<<msg.m_Task.m_Index;
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GLET_NOTIFY_SPEED_UP_ENTRUST_TASK);

		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.result);
	}
	
}

void HL_FromGameServer::_handleGetAwardEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_GetAwardEntrustTask_Ret msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SC_GetAwardEntrustTask_Ret解析错误");
		return;
	}
	if (msg.result==RE_SUCCESS)
	{
		StringStreamType sStream("");
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		sStream<<pkGamePlayer->GetAcceptEntrustTaskRewards(msg.m_Indexs);

		pkGamePlayer->OnTakeEntrustTaskReward(msg.m_Indexs);

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GLET_NOTIFY_TAKE_ENTRUST_TASK_REWARD);

		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.result);
	}
}

void HL_FromGameServer::_handle_UpdateEntrustTaskNum_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_UpdateEntrustTaskNum_Ret msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SC_UpdateEntrustTaskNum_Ret解析错误");
		return;
	}
	StringStreamType sStream("");
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateEntrustTaskRefreshNum(msg.m_Num);

	LC_ClientGameEvent kGameEvent;
	kGameEvent.SetClientEventType(GameLogic::GLET_NOTIFY_ENTRUST_TASK_NEW_DAY);

	LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
	kGameEvent.SetUserData(&kUserData);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRefreshEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_UpdateEntrustTask_Ret msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SC_UpdateEntrustTask_Ret解析错误");
		return;
	}
	if (msg.result==RE_SUCCESS)
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->OnRefreshEntrustTask(msg.m_Tasks,msg.m_Num);

		StringStreamType sStream("");
		for (int32_t i=0;i<msg.m_Tasks.size();i++)
		{
			sStream<<msg.m_Tasks[i]<<'#';
		}
		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GLET_NOTIFY_REFRESH_ENTRUST_TASK);

		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
	else
	{
		SendLogicInfoMessage(msg.result);
	}
}

void HL_FromGameServer::_handlerRLTGetNewAchievementReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetNewAchievementReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_GetNewAchievementReward解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
}

void HL_FromGameServer::_handlerNotifyGuildMapBossInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_NotifyGuildMapBossInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_NotifyGuildMapBossInfo解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SyncGuildBossActivityInfo(msg.m_nGuildMapBossInfo);
	pkGamePlayer->SyncGuildBossActivityFinishMap(msg.m_nFinishMapIDInfo);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_SYNC_GUILDBOSSACTIVITY);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerSyncGuildMapFinishMap(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncGuildMapFinishMap msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncGuildMapFinishMap解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateGuildBossActivityFinishMap(msg.m_nFinishMapID);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDBOSSACTIVITY_FINISHMAP);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerUpdateGuildMapBossInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_UpdateGuildMapBossInfo msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_UpdateGuildMapBossInfo解析错误");
		return;
	}
	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateGuildBossActivityInfo(msg.m_nMapID, msg.m_nBossState);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDBOSSACTIVITY_BOSSSTATECHANGE);
	GameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nMapID);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerSyncGuildBossActivityOpen(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncNewOpenGuildMapBoss msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncNewOpenGuildMapBoss解析错误");
		return;
	}

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDBOSSACTIVITY_OPEN);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerRltGuildBossActivityOpen(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_ActivityGuildBossMap msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_ActivityGuildBossMap解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->ReSetGuildBossActivityRankList();

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_CLIENT_RLT_RANK_INFO);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, MGPT_RLT_GETGUILDMAPBOSSRANK);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nMapID);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, 0);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_4, 0);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_5, -1);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerSyncSingleDamageRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncSingleDamageRewardRecord msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncSingleDamageRewardRecord解析错误");
		return;
	}


	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateThemeACTRaidBossRewardInfo(msg.m_nSingleDamageRewardRecord);

	LC_GameEvent GameEvent;
	GameEvent.SetEventType(GLET_GUILDBOSSACTIVITY_TOTALREWARDSTATE);
	UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(GameEvent);
}

void HL_FromGameServer::_handlerRLTGetSingleDamageReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetSingleDamageReward msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_GetSingleDamageReward解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->AddThemeACTRaidBossRewardInfo(msg.m_nMapID,msg.m_nRewardIndex);

		LC_GameEvent GameEvent;
		GameEvent.SetEventType(GLET_GUILDBOSSACTIVITY_TOTALREWARDSTATE);
		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(GameEvent);
	}

	
}

void HL_FromGameServer::_handlerThemeFlagChange_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SC_ThemeFlagChange_Ret msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SC_ThemeFlagChange_Ret解析错误");
		return;
	}
	if (msg.m_Vec.size()>0)
	{
		StringStreamType sStream;
		for (int i=0;i<msg.m_Vec.size();i++)
		{
			sStream<<msg.m_Vec[i]<<';';
		}

		LC_ClientGameEvent kGameEvent;
		kGameEvent.SetClientEventType(GameLogic::GLET_RLT_THEME_ACT_AD_SHOW);

		LC_ClientGameEventUserData_String kUserData = LC_ClientGameEventUserData_String(sStream.str());
		kGameEvent.SetUserData(&kUserData);

		UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
	}
}

void HL_FromGameServer::_handlerThemeRaidBossFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Sync_HuanJingFlag msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_Sync_HuanJingFlag解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateThemeACTRaidBossFlags(msg.m_nInvaildID);
}

void HL_FromGameServer::_handlerRLT_GetTaskExtraReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_GetTaskExtraReward msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_GetTaskExtraReward解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
	}
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
		pkGamePlayer->ClearTaskExtraReward();
	}
}

void HL_FromGameServer::_handlerSyncTaskExtraReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncTaskExtraReward msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncTaskExtraReward解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateTaskExtraReward((SyncType)msg.m_nSyncType,(SyncOP)msg.m_nSyncOP,msg.m_nRewardInfo);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_ON_SYNC_TASK_EXTRA_REWARD);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleSyncBuddyActivateScheme(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncBuddyActivateScheme msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncBuddyActivateScheme解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetMapFullSchemeData(msg);
}

void HL_FromGameServer::_handleRespSetBuddyActivateScheme(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_SetBuddyActivateScheme msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_SetBuddyActivateScheme解析错误");
		return;
	}

	if (msg.m_nResultType != RE_BUDDY_SCHEME_ERROR_OK)
	{
		SendLogicInfoMessage(msg.m_nResultType);
		return;
	}

	if (msg.m_nSchemeType == SchemeType_Defence)
	{
		SendLogicInfoMessage(RE_BUDDY_SCHEME_ERROR_OK);
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetMapFullSchemeData(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BUDDYSCHEME_CHANGE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nMapID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nSchemeType);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRespSetSpecialMapInitBuddyActivateScheme(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RESP_SetSpecialMapInitBuddyActivateScheme msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RESP_SetSpecialMapInitBuddyActivateScheme解析错误");
		return;
	}

	if (msg.m_nResultType != RE_BUDDY_SCHEME_ERROR_OK)
	{
		SendLogicInfoMessage(msg.m_nResultType);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetMapFullSchemeData(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BUDDYSCHEME_CHANGE);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nMapID);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nSchemeType);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRespBattleLog(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_BattlefieldLog_List msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_Resp_BattlefieldLog_List解析错误");
		return;
	}

	if (msg.m_nResult != BATTLELOG_OPERATE_SUCESS)
	{
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateBattleRecord(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BATTLERECORD_LIST);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nBattleType);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handleRespBattleFeildReport(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_BattlefieldReport msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_Resp_BattlefieldReport解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateBattleDetail(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_BATTLERECORD_DETAIL);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nBattleType);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_nClientIndex);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_nCharId);
	kGameEvent.SetUint64Param(msg.m_nIndex);


	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSyncMapNpcInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncMapNpcInfo msg;

	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncMapNpcInfo解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetMapNpcInfo(msg);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GELT_SYNC_MAP_NPC_COUNT);
	kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResMapID);

	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}


void HL_FromGameServer::_handle_SetMapBossFlag_Rlt(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_RLT_SetMapBossFlag  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_RLT_SetMapBossFlag解析错误");
		return;
	}

		LC_GameEvent kGameEvent;
		kGameEvent.SetEventType(GLET_RLT_SETMAPBOSSFLAG);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_1, msg.m_nResult);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_2, msg.m_serverPlayerBossFlagData.m_nCharTypeID);
		kGameEvent.SetEventParam(EVENT_BASE_PARAM_3, msg.m_serverPlayerBossFlagData.m_bFlag);

		UI_UIManager *pkUIManager = UI_UIManager::GetSingletonPtr();
		pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handle_MapBossFlagInfo_Sync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncMapBossFlagInfo  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SYNC_SETMAPBOSSFLAG解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetBossFlagList(msg.m_vServerPlayerBossFlagInfo);
	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_SYNC_SETMAPBOSSFLAG);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);

}

void HL_FromGameServer::_handlerNotifySetAtvChargeRedPoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Notify_SetAtvCharge_RedPoint  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_Notify_SetAtvCharge_RedPoint解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	for (int i = 0; i < msg.m_nScheduleIds.size(); ++i)
		pkGamePlayer->UpdateAtvChargeRedPoint(msg.m_nScheduleIds[i]);
}

void HL_FromGameServer::_handlerRespSetAtvChargeRedPoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_Resp_SetAtvCharge_RedPoint  msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_Resp_SetAtvCharge_RedPoint解析错误");
		return;
	}

	if (msg.m_nResult != RE_SUCCESS)
	{
		SendLogicInfoMessage(msg.m_nResult);
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->UpdateAtvChargeRedPoint(msg.m_nScheduleId);

	LC_GameEvent kGameEvent;
	kGameEvent.SetEventType(GLET_ATVCHARGE_REDPOINT);
	UI_UIManager* pkUIManager = UI_UIManager::GetSingletonPtr();
	pkUIManager->HandlerGameEvent(kGameEvent);
}

void HL_FromGameServer::_handlerSyncFinishMainTaskID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize)
{
	MG_SyncFinishMainTaskID msg;
	if (!msg.Decode(pMsg,wSize))
	{
		GfxWriteLog(LOG_TAG_UI, LOG_SWITCH_UI,"MG_SyncFinishMainTaskID解析错误");
		return;
	}

	LC_ClientGamePlayer* pkGamePlayer = pkSession->GetPlayer();
	pkGamePlayer->SetFinishMainTaskID(msg.m_nFinishMainTaskID);
}


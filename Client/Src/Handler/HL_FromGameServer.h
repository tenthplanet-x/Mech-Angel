#ifndef _HL_FROM_GAMESERVER_H
#define _HL_FROM_GAMESERVER_H
//处理GameServer发来的消息
class NW_Game2ClientSession;
class HL_FromGameServer
{
public:
	HL_FromGameServer(){};
	~HL_FromGameServer(){};
	static void ParsePacket(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

public:
	//Begin Login Protocol	
	static void _handlerSignaturePass(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltCreateChar(NW_Game2ClientSession *pkClientSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDeleteChar(NW_Game2ClientSession *pkClientSession, char* pMsg, uint16_t wSize);
	static void _handlerRltRecoverChar(NW_Game2ClientSession* pkClientSession,char* pMsg, uint16_t wSize);
	static void _handlerRltLogin(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );	
	static void _handlerRltEnterGame(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltLeaveGame(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltResetGame(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltJumpGame(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltWaiting(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize);
	//End Login Protocol
	
	//Begin Game Protocol
	static void _handlerSystemAnnounce(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSystemNotifyMsg(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltUpdateSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltLearnControllerSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltRespawn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltNetPlayerEquipInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltNetPlayerAttribute(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltTransfer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltTransferToLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltRequestInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncAchievementInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncFinishAchievement(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncAchievementRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltAchievementReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltAchievementPointReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltSevenDayPointReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//Transcription Protocal
	static void _handlerRltEnterTranscription(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltQuitTranscription(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltReliveTranscription(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerReceiveTranscriptionTipsInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltNPCUpdate(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRefreshNPCLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltQuitCareerSelect(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltNPCChangeFactionType(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerNPCUpgradeInfoFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerPerDayTranscriptionInfoFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerGameStoryInfoSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerGameStoryBatchInfoSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerStoryTipsInfoFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerNPCExecuteSkillFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerReceiveSeekTreasureFromServer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//Transcription Protocal End

	static void _handlerRltSetTransferPos(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncNetPlayerTitle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltSetTitle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltAddDIYTitle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltRemoveDIYTitle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltModifyDIYTitle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncShapeShift(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltNPCOwnerShip(NW_Game2ClientSession* pkSession,char* pMsg, uint16_t wSize);
	
	static void _handlerRltAntiWallow(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltFollowPlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGMTransfer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _landlerRltTempPackOP(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//End Game Protocol

	//Begin Task Protocol
	static void _handlerRltAcceptTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltFinishTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltCancelTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltShareTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltConfirmShareTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerRltBillBoardTaskInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerReqShareMultiuserTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerRltJoinInMultiuserTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerRltTaskAssignInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltDrawLivenessReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//End Task Protocol
	
	//Begin Skill Protocol
	static void _handlerNPCExecuteSkillTypeLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );	
	static void _handlerNPCExecuteSkillTypeTarget(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerNPCExecuteSkillTypeDir(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerNPCInterruptSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerPlayerExecuteSkillTypeTarget(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerPlayerExecuteSkillTypeDir(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerPlayerExecuteSkillTypeLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerPlayerInterruptSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerRltNPCDead(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerRltPlayerDead(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerRltSkillEvent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void	_handlerRltSkillEventEx(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void	_handlerRltSkillEventMerged(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerReqSkillRespawn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerRespawnBroadcastLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerPunchMovePlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerPunchMoveNpc(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDeactiveSkillState(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize);
	static void _handlerRltTalkToNPC(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize);
	//End Skill Protocol

	//Begin Sync Protocol
	static void _handlerSyncPlayerProperty(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayerSociety(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerAttribute(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncGamePlayerInfo(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncGamePlayerAIControl(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncGamePlayerAIControlLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//static void _handlerSyncBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncBasicBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncTaskBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncCommandEquip(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncRuneBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncFirstAddRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncRuneRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRespRuneReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerNewItemNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncRedPointFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSyncOpenBagAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncWarehouseBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncSkillBackPack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncSkillCD(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncSkillCDModify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncItemCD(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncTutorialAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncTalent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncSupremacy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncBerserk(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncShortcutKey(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//static void _handlerSyncPlayerFaction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncActionForbidType(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	/*static void _handlerSyncMonsterInfoInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncMonsterMoveInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncMonsterPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncMonsterSkillStateInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );*/
	//static void _handlerSyncNPCInfoInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncNPCMoveInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncNPCStateFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncNPCAngleFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncNPCPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncNPCSkillStateInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncNPCInteractionInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//static void _handlerSyncNPCFactionInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//static void _handlerSyncPlayerFactionInFOV( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//static void _handlerSyncPlayerInfoInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayerFastPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayerExtPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerSocietyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayerEquipInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerSkillStateInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayerSubordinateInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);

	static void _handlerSyncPlayerList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncChestList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncChestStateList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerCash(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerYuanBao(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerCustomCash(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncLotteryConsumeUnbindYuanbao(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncProcessingTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncFinishedTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncFailTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncLotteryRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncLotteryRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerLotteryDrawRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerLotteryExtranRewardRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSyncUseItemUpgradeSub(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSyncChargeProfit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerChargeProfit_RewardRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//每日任务相关Begin
	static void _handlerRltTaskOneKeyOp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltTaskAssignOneKeyOp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltReduceTaskDiffStar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltPromoteTaskRewardStar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltTaskDrawRoundOffReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
    //每日任务相关End
	static void _handlerRltTaskPhagotrophy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerGroupMemberList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPlayerGroupCandidateList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPKState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncPKValue(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncFightState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncShortcutBar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncTitleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncDefaultTransferPos(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncSummorFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSyncSkillState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncGamePlayerMove(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncGamePlayerJump(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayerExecuteSkillTypeTarget(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);

	static void _handlerSyncArenaInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncActorOperationAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncArenaBattleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handleSyncVIPInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleNotifyVipLevelChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltVIPDrawGiftDaily(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltVIPDrawGiftOneOff(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltDrawFdepositReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handleSyncSignInInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleSyncGuildWarInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleNotifyGuildWarCatapultMsg(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltSignIn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltReSignIn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRespSigninRewward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltPlayerDetail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltPowerGuildReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltGlobalGuildReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRltDomainReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleMsgToLua(uint16_t iMsgType, NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleSyncCorssRealmTeam(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleNotifyCrossRealmTeamFast(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltResetShopTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetShopResetTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetShopItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltBuyInvesmentCard(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAddInvesmentCardReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltRefreshRewardTask(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//End Sync Protocol

	static void _handlerRltCreateGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );	
	static void _handlerRltExitGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltKickPlayerFromGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltDismissGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltJoinGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerReqConfirmGroupJoin(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltInviteToGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerReqConfirmInviteToGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltConfirmInviteToGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerReqConfirmBeInviteToGroup(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltAppointmentCheif(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltTranscriptionKickPlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltAgreeTranscriptionKickPlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyGroupRaid(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltModityQuality(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltForbidJoin(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltGuildRaidReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRespAddGuildGood(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRespGuildItemOpt(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltRecruitGuildMember(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltGetGuildReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
			
	static void _handlerRltShopCityBuyGood(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltShopCityGoodList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltMysticalShopGoodList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltMysticalShopOp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerSyncGameplayerStarMoney(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRequestTradeItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltTradeItemInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRelpyTradeItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerAddItemToTradePack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRemoveItemFormTradePack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerUpdateTradeCash(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerLockTrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerUnLockTrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerInterruptTrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerFinishTrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncTradePack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerEnhanceItemEnd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerDecompositeItemEnd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerUnSocketItemEnd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerItemUnbind(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerItemRefine(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerItemRefineApply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltExternBasicBag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyAutoOpenBasicBag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltChatRequest(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerReceiveChatMsg(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltUpdateChatChannel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerNotifyYiZhongQinXiNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	
	//掉落包裹相关
	static void _handlerChestShowContent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerChestPickUp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerChestItemRoll(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerChestOPResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//仇人
	static void _handlerNotifyAddtoenermy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//进入地图次数同步
	static void _handlerSyncRaidEnterMapRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncIDAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAddID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//屏蔽相关
	static void _handlerRltForbidList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerRltUpgradeMiJi(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize);

	//----------Begin Vehicle Protocol--------//
	static void _handlerRltUpgradeCimelia(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize);
	static void _handlerRltRemoveVehicle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltSummonVehicle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltDismissVehicle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltActiveVehicle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltChangeTalisman(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize);

	static void _handlerRltSetSubtransFormFlag(NW_Game2ClientSession *pkSession, char *pMsg, uint16_t wSize);
	//----------End Vehicle Protocol----------//

	//-------Begin ItemEnhance Protocol--------------//
	static void _handlerRltLearnProduction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltForgetProduction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltUpgradeProduction(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltLearnFormula(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	
	static void _handlerItemProtocol(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize );
	static void _handlerWarehouseProtocol(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize );
	static void _handlerFixItemProtocol(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize );


	// -------------begin mail Protocol -------------//
	static void _handlerRltMailAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltReadMail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltWriteMail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltItemAffix(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCashAffix(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltUntreadMail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDeleteMail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAddItemToMailAffix(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDelItemFromMailAffix(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltPayForMail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltIsExistNewMail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	// -------------end  mail protocol ---------------//

	// -------------begin Auction Protocol -------------//
	static void _handlerRltAddItemToAuctionSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDelItemFromAuctionSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAuctionSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAuctionCancel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAuctionBuy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAuctionList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAuctionGetAward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAuctionNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	// -------------end  Auction protocol ---------------//

	// -------------begin Guild Protocol -------------//
	static void _handlerRltGuildAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGuildMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCreateGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltApplyGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltComirmApplyGuilid(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGuildApplyList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltExitGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltModifyGuildDeclaration(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyAdd2Guild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyAddMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyDelMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyUpdateMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyPositionChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltKickMemberGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDismissGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltInviteList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltInviteToGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerReqConfirmInviteToGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);	
	static void _handlerRltGuildSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDonateGuildMaterials(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSearchGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltForbidGuildApply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltClearGuildApplyList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyAddGuildApply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyDelGuildApply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGuildAppoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltUpdateGuildRank(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltUpgradeGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltLearnGuildSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltPowerGuildAppoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyGuildRaidInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltOnlinePlayerState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	// -------------end Guild Protocol -------------//


	// -------------begine Game event Protocol -------------//
	static void _handlerRltGameEvent(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	// -------------end Game event Protocol -------------//

	// -------------begine Friend Protocol -------------//
	static void _handlerRltAddFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDelFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetFriendList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltAddEnemy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDelEnemy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetEnemyList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltAddBlackList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDelBlackList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetBlackList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltGetFriendStatus(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetFriendDetail(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSyncFriendModifyName(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltPlayerDeltaData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetSocietyDeltaData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltFriendMsgBroad(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltFriendOnlineNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltFriendOfflineNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltAddFiendNick(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSupportFiendNick(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetFriendNick(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltBeAddFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handReqConfirmBeAddFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	static void _handlerRltChangeSocietyInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSyncSummorPool(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltGetPlayerNearBy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSocietyDynamicInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSingleFriendInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	// -------------end    Friend Protocol -------------//

	// -------------begine Search Player Protocol -------------//
	static void _handlerRltSearchPlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSearchPlayerDetailInfoTitle(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSearchPlayerDetailInfoArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	// -------------end    Search Player Protocol -------------//
	static void _handlerSyncPlayersLocationChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayersAngleChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayersEnter(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handleSyncbuddyRuneStarLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncPlayersLeave(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayersStateChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayersPVEStateChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncPlayersHPMPChange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerToyPetSwitchEnabledIndex(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRefreshBuddyTransformID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerSyncNpcsEnter(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerSyncNpcsLeave(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltRecommendFriend(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltAddAllRecommendFriends(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//-------Begin Profile Protocol--------------//
	static void _handlerRltProfileName(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------end Profile Protocol--------------//

	//-------Begin Card Protocol--------------//
	//static void _handlerSyncCardAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//static void _handlerRltCardExchange(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//static void _handlerRltCardTakeOut(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//static void _handlerRltAddCard(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------end Card Protocol--------------//

	//-------Begin Card Protocol--------------//
	static void _handlerRltSaveShortcutKey(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------end Card Protocol--------------//

	static void _handlerRltSyncPing(Protocol::CS_MG_HEAD& rkHeader,NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//debug信息
	static void _handlerRltDebugCommandLine(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDebugActorAIInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	// 收藏相关
	static void _handlerRltCollection(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCollectionSuit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//开服竞技榜结束后数据
	static void _handlerSyncACTSubInfoEnd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//竞技场
	static void _handlerRltArenaOpeartion(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSignArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCancelArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltMatchArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltEnterArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltLeaveArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltArenaState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltArenaResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltArenaOpenDoor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltArenaCloseDoor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltArenaTip(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize);
	static void _handlerRltGiveUpArena(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltArenaMatchAgain(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize);
	static void _handlerRltArenaConfirmCancel(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize);

	static void _handlerRltControllerAndCoreCombatScore(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize);
	static void _handlerRltPlayerPKInfo(NW_Game2ClientSession* pkSession,char* pMsg,uint16_t wSize);
	//新竞技场
	static void _handlerRankQueryChallengeInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltRankChallenge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyRankChallenge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifyRankBeChallenge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerGetRankYiZhongQinxi(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerGetAnswerRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerGetYijituxiRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerChallengeRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespRankPlayerBuddyInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespRankBuddyInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespRankPetInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespRankSubInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespRankTopnPlayerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespBuddyRankIndexEx(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespChallengGiftState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespBattleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespAddChallengeTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespClearChallengeCD(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespGetChallengeCD(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDailyAward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncFinishMainTaskID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltRankBaseInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltYizhongqingxiDamageData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltYizhongqingxiDetailDamageData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRespDalyCleanData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRespRankDalySettleNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespBasicProfitNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRespCommonStatBaseOpt(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltExecuteActorOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleEquipAdvance_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSubEquipAdvance_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);


	//帮派家园
	static void _handlerRltEnterGuildHome(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltLeaveGuildHome(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltUpgradeGuildNPC(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltBuyFromGuildNPC(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltDonate(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltPutWareHouse(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSwapWareHouse(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetGuildWareHouse(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltThiefCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetPrizeOfKillThief(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGuildHomeInStory(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltOpenKillThief(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltKillThiefRecordTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltKillThiefInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);


	//---------Exchange------------------
	static void _handlerRlExchange(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize );
	//挂机
	static void _handlerRltHookInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//----------Prison-------------------
	static void _handlerRltLeavePrison(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize );
	static void _handlerRltImprisoned(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize );
	static void  _handlerRltReducePkValue(NW_Game2ClientSession *pkSession, uint32_t iMsgType, char* pMsg, uint16_t wSize );


	//-------Begin Currency Trade Protocol--------------//
	static void _handlerRltCurrencyQuickBuy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyQuickSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyCommissionBuy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyCommissionSell(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyQuickTradeList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyCommissionItemList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyTradeRecordList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyRevokeCommission(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyYuanBaoDeposit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyYuanBaoDraw(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyMoneyDeposit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltCurrencyMoneyDraw(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------end Currency Trade Protocol--------------//

	//-------Begin QuanThanhTemple Protocol--------------//
//	static void _handlerRltChangeElement(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------End QuanThanhTemple Protocol--------------//

	//-------Begin Gift Protocol--------------//
	//static void _handlerSyncGiftAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//static void _handlerRltGetGift(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------end Gift Protocol--------------//

	static void _handlerRltEnterSacredWeapon(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltQuitSacredWeapon(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------begin raid --------------//
	//Dungeon Stage
	static void _handlerNotifyDungeonStage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncRaidAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncTransportAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//Raid Settle
	static void _handleSyncRaidSettle(NW_Game2ClientSession* pkSession, char* pMsg, uint16_t wSize);
	//-------end raid--------------//

	//-------Begin Portal Protocol--------------//
	static void _handleSyncPortal(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//-------end Portal Protocol--------------//

	//-------Begin Assist Protocol--------------//
	static void _handlerRltSetAssistInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltGetHookingGuideLocation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//-------end Assist Protocol--------------//
	
	//-------Begin Great Kungfu ProtocolMGPT_GREAT_KUNGFU_OPERATION_RESP
	static void _handleSyncGreatKungFu(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleGreatKungFuOperationResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------Begin OpenAbility Protocol
	static void _handleSyncOpenAbilityInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltOpenAbility(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//--End OpenAbility Protocol
	//-------Begin GM Protocol
	static void _handleSyncGMInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------End GM Protocol

	//-------Begin Subordinate Protocol
	static void _handleSyncSubordinateInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//-------End Subordinate Protocol

	static void _handleSyncDomainInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);


	static void _handleSyncCommonStat(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerSyncTaskTypeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//新主线任务相关
	static void _handlerSyncTaskRank(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncTaskChapterRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltTaskChapterReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltTaskReachReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncMainTaskReachRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncNextRefreshRewardTaskTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltStatOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltActivityCommonOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltActivityScheduleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltUseActivationCode(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltUseActivationCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handerRltBuyCourageItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltGetRankSubBInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncSubInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncSubbValue(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncDropInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncRegularBossDrop(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncActivityFinish(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handerSCAddTimer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltChestsOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltChestsLogs(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleSyncTreasureChestsInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltSecretTreasureOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltChessMatrixOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleSyncSecretTreasureInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleSmeltBack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);	
	
	static void _handleStarsRuneBack(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);	

	static void _handleRealTimeComBatRecorder(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleSyncSkillBookInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleSyncSkillBookSlotInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleOpenNewSkillBook(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltActivateSkillBook(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltUnlockSkillBook(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltWearSkillBook(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRespEnterCrossMap(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRespGameStoryOperation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRespSkyArenaTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRespFactionDefender(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleGuildTransportAdd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	static void _handleRespGuildTransportCreat(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	static void _handleRespGuildTransportEnter(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleSyncGameStoryAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	static void _handleGuildTransportDel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleGuildRewardState( NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _hanldeSyncCommonShopInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltBuyVigor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltClearPKBuff(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleCrossRealMapTeamInfoNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleCrossRealMapTeamOperationResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleItemInPackageCommonOperationResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerSyncRumorInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	
	static void _handlerSyncGameStoryRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSyncPlatformMessage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSyncSkyArenaScore(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSyncSkyArenaMonsterHp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerNotifyServer_ErrorCode(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	
	//边境地图
	static void _handleRltAlliance(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltEnterAlliance(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltAllianceWinInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltAllianceUnitInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltAllianceUnitRwd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltDrawAllianceUnitRwd(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//公告消息
	static void _handleGameNoticeMsg(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//炼魂同步
	static void _handleSoulRefineAssetInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSoulRefine(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSoulRefineLevelUp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//时装同步
	static void _handleFashionAssetInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleFashionShow(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleShowFashion(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//热加载更新信息
	static void _handleDebugReloadNotify(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//活动消息同步
	static void _handleSyncActivityInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//玩家改名
	static void _handleChangeCharName(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//更新公告同步
	static void _handleSyncUpdateBulletinInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//同步玩家控制角色信息
	static void	_handleSyncPlayerControl(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	//玩家今日活跃度
	static void _handleRltGamePlayerDailyActivity(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGamePlayerDailyActivityReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//玩家角色成长礼包
	static void _handleRltCharacterUpgradGift(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//首充奖励七日
	static void _handleRltGamePlayerDailyCharge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGamePlayerDailyChargeTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGamePlayerDailyChargeRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//收藏相关
	static void _handleRltGamePlayerCollectionRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//Boss首杀相关
	static void _handleRltGamePlayerBossFirstKill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncKillBossReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltKillBossReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncServerFirstSkillBossReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncSevenDayFinishTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncFirstDayLoginRewardTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncCommanderTitleRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncProFilePictureRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltSetProfilePictureFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltSetCommanderTitleFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncCollectBossJP(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//竞技榜相关
	static void _handleRltGamePlayerSuperShopCompete(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncActActiveRewardInfoB(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//挂机机器人
	static void _handleRltSyncBotData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltSyncActiveBotData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltSyncUpgradBotData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltBotProfit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncBotProfit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncBotBgeinTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//分解相关
	static void _handleRltDeCompositionEquips(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltDeCompositionEquipsSetting(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltSetAutoDecompositionItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	//传记
	static void _handleRltBiographyReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncBiographyChapters(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncBiography(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//限时礼包
	static void _handleSyncTimeLimitShop(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltTimeLimitShop(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//三角色
	static void _handlerSyncBuddyData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerSyncActiveBuddyData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerBuddyReply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSCSetActivateBuddyReply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerSCBuddyBaseDataSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerCreateBuddyNetPlayer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void	_handlerSyncSingleBuddySchemeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerSyncMultiBuddySchemeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerSyncBuddyActivateSchemeType(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void	_handlerBuddySchemeReply(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSYNCPlayerControllerProperty(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSYNCPlayerControllersPropertyInFOV(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerRLTBuddyStarUpgrade(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltBuddyManualLevelUp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltActivateBuddyCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRefreshPlayerBindBuddy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	static void _handlerRefreshPlayerStarLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncRefreshBuddyEquipSlotLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRefreshPlayerRuneLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerResetSingleBuddyRespawnTimer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRefreshBuddyEquipSlotStarlevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRefreshBuddyEquipItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerActivateBuddyRuneSlot(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerBudyyEquipSlotState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerResetAllBuddyRespawnTimer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSetBuddyRespawnTimer(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerBuddyChainCountSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerBuddyChainSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerBuddyChainOpResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerAddBuddyChainSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerDelBuddyChainSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltSetChainFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncChainFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltSetCurrentCommanderTitleID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltSetProfilePicture(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltActivateSkill(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSkillUsableCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSkillMaxUsableCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSkillSingleCD(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSkillSingleLogicValue(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncOwnerMPInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//装备强化、升星、附魔
	static void _handlerBuddyEquipReinforceLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerBuddyEquipReinforceStar(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerBuddyReinforceEquipSlot(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRltUserDead(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerReduceCash(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNetPlayerActiveBuddy(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize, bool bArenaPlayBack = false);
	//核芯
	static void _handlerSyncActivateStarVein(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncActivateStarVeinGoal(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetStarVeinGoalReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltActivateStarVein(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//玩具
	static void _handlerPetCaptureResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerPetBaseDataSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerPetStarUpgradeResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerPetDecomposeResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerPetStatusSync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerPetSyncSkillAsset(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerPetEnableResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerPetDisableResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//抽卡
	static void _handlerDrawCardResp(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerSyncResourceRetrieveState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//套装收集
	static void _handlerSyncCollectPlayerEquipSuit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespCollectPlayerEquipSuit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//资源找回
	static void _handlerSyncResourceRetrieveRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRltGetResourceRetrieveReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//星魂
	static void _handleRltEquipRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltOnekeyEquipRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltUnEquipRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltOnekeyUnEquipRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltLevelUpRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltStarUpRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltResetRune(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//圣契
	static void _handleSyncHolydeed(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncAddnewHolydeed(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//投资卡
	static void _handleSyncInvestmentCardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncInvestmentCardRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//悬赏任务
	static void _handleSyncRefreshRewardTaskTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncOfferTaskAcceptTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//异种突袭
	static void _handleSyncSingleCombatRecorder(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//新VIP
	static void _handleSyncVipNewInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncAddReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncVipNewBonus(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncVipNewBonusBeginTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleNewVipRewardResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleSyncBossDropinfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncTestDataBase(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncAnswerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	//本地图角色回复
	static void _handleGetNearbyPlayerInfoAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleTakeCommanderTitleRewardAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleSyncSpecialTrainingReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleTakeSpecialTrainingRewardAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//英雄装备
	static void _handleSyncBuddyEquipLv(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltBuddyEquipLv(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//指挥官装备
	static void _handleRltSetCommanderEquipSuitLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncCommanderEquipSuitLevel(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//进阶线装备
	static void _handleRltSubEquipModingSpirit(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//荣光远征
	static void _handleQuickRaidNewAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleGetChapterRaidStarRewardAck(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncTotalRaidGloryStarNum(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncChapterRaidGloryStarNum(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncChapterRaidGloryRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncQuickRaidGloryCount(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltReceiveGloryDailyReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncReceiveGloryDailyReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//遗迹突袭寻宝玩具岛
	static void _handleRltGetYiJiTuXiStageReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncYiJiTuXiRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGetYiJiXunBaoStageReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncYiJiXunBaoRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGetToyIslandStageReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncToyIslandRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	
	//幸运转盘
	static void _handleRltGetRotaryTableReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncRotaryTableSkipAnimation(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncRotaryTableVaildTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncRotaryTableRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncRotaryEndTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//极品白拿
	/*static void _handleSyncNonpareilRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncNonpareilReturnRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltBuyNonpareilItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGetNonpareilReturn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncNonpareilEndTime(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);*/

	static void _handleRltBuyRareSubDevaEyeTransform(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//公会战相关
	static void _handlerGuildBattleTowerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerGuildBattleTowerInfoLife(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerGuildBattleTowerRankList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGuildFightRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncBestMapItemInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSyncBestGuildTaskInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGetBestGuildReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGuildFightRobScore(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGuildFightRobScoreReceive(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGuildFightMemberUID(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltGuildFightMemberInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleNotifyRankGuildOfflineChallenge(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRespGuildBattleFightRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNoticeBeDefighted(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handleRltGetRegularBossKillerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRltQuickApplyGuild(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncRegularBossGroupInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncBossKillerInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//物资护送
	static void	_handlerRLTAddEscortReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);//领奖返回
	static void	_handlerSyncEscortState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncRobbingTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncEscortTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncEscortNPCState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void	_handlerSyncGoalRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void	_handlerSyncFirstChargeInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);//
	static void	_handlerGetFirstChargeReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);//领奖返回

	static void	_handlerSyncVIPExtraRewardUsedTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerRLTGetVipExtraReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);//领奖返回

	static void	_handlerSyncNewAchievementStarNum(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncNewAchievementRewardCord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerRLTGetNewAchievementReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerNoticeNewAchievement(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void	_handlerRltGetTreasurePos(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void	_handlerRltGetGameRestartReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncGameRestartNotice(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void	_handleRltBuddyTransformOP(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//公会礼物
	static void	_handlerSyncGuildProfitPresentInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSendGuildProfitState(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerDeleteGuildProfitPresentInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerAddGuildProfitPresentInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncGuildProfitShowNameFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerRltGetGuildProfitReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncGuildProfitCrystalInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerRltGetGuildCrystalReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//远征点恢复
	static void _handleSyncGloryPointRecover(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//开服争霸榜
	static void _handleRLT_GetSubBValue(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//委派任务
	static void _handleSyncSendEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleAccpetEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleSpeedUpEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleGetAwardEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handleRefreshEntrustTask_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handle_UpdateEntrustTaskNum_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	//公会秘境
	static void	_handlerNotifyGuildMapBossInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncGuildMapFinishMap(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerUpdateGuildMapBossInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerSyncGuildBossActivityOpen(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerRltGuildBossActivityOpen(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerRltGuildBossActivityTotalReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerGetRankGuildBossActivity(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	//主题活动
	static void	_handlerSyncSingleDamageRewardRecord(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerRLTGetSingleDamageReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void	_handlerThemeFlagChange_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);//活动期间第一次登陆弹广告的通知
	static void	_handlerThemeRaidBossFlag(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);//活动期间幻境试炼每日红点

	//新装备协议
	//指挥官装备
	static void _handlerSCEquipCommand_Ret(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSCIntensifyCommandEquipRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSCRefineCommandEquipRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSCSoulCommandEquipRet(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//进阶线装备
	static void _handlerRltEquipSubItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRltUnEquipSubItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	//英雄纹章
	static void _handlerSyncBuddyEquipSlotPackData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncBuddyEquipSlotData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//主线任务奖励特权双倍领取
	static void _handlerRLT_GetTaskExtraReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncTaskExtraReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//阵容配置
	static void _handleSyncBuddyActivateScheme(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRespSetBuddyActivateScheme(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRespSetSpecialMapInitBuddyActivateScheme(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//战斗记录
	static void _handleRespBattleLog(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handleRespBattleFeildReport(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSyncMapNpcInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//新手帮助 
	static void _handlerRltAddNoviceTutorial(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize ); 
	static void _handlerRltGetNoviceTutorialReward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSyncNoviceTutorialInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//黄金首领
	static void _handle_SetMapBossFlag_Rlt(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handle_MapBossFlagInfo_Sync(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//苍穹战场
	static void _handlerRltGetSkyArenaBattleInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerNotifySkyArenaTimeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerResp_SkyArenaCheckBattleStage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerNotifySkyArenaGetHonor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);

	static void _handlerRespGetPlayerSchemeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerRespGetRankTypeInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	static void _handlerSkyAllShowInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );
	static void _handlerSkyRoundShowInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize );

	//苍穹奖励
	static void _handlerResp_SkyArenaGradeAward(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ;  
	static void _handlerResp_SkyArenaMyScore(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	
	//苍穹结算
	static void _handler_SkyArenaChallengeResult(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	//竞猜相关
	static void _handlerResp_GetSummitSingleData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerResp_SetSummitGuessInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerResp_GetSummitWiner(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerResp_GetSummitSupportInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerSyncSummitData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerMgptSummitArenGuessClose(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 

	//巅峰竞技场 
	static void _handlerRespSummitTimeData(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerRespSummitArenaGroupStage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerRespSummitArenaMyGroupStage(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerRespSummitArenaAgainstList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerRespSummitRedPoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerRespSummitArenaInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handerRespSummitArenaGroupStageEntryList(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerRespSummitArenaHonor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerRespSummitArenaMyTopHonor(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlerRespSummitTopType(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize); 
	static void _handlerNotifySetAtvChargeRedPoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespSetAtvChargeRedPoint(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);
	static void _handlerRespSummitInvitationValid(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize); 
	//极品白拿新版 
	static void _handlerGetBuyNoPayItem(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize) ; 
	static void _handlderSyncBuyNoPayReturnInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize); 
	static void _handlderGetBuyNoPayReturn(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize);  

	//异种降临
	static void _handlerRltGetGuildDamageRank(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize); 
	static void _handlerRespDamageRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize); 
	static void _handlerRespGuildDamageRankInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize); 


	//篝火晚会 
	static void _handlerBonFirePartyLeftBuyItemTimes(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize); 
	static void _handlerBonFirePartyBuyInfo(NW_Game2ClientSession *pkSession, char* pMsg, uint16_t wSize); 
};

#endif

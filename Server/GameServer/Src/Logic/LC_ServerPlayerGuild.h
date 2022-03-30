//玩家个人 身上 的公会信息

#pragma once
#include "MG_CS_Guild.h"
#include "MM_MemoryAllocatorConfig.h"
#include "char_data.pb.h"
using namespace Protocol;

class LC_ServerPlayer;

class LC_ServerPlayerGuild
{
public:
	LC_ServerPlayerGuild();
	~LC_ServerPlayerGuild();
public:
	void ReUse();
	// 挑战前 判断 次数， 道具是否足够
	ResultType PreIsAllowChallenge(LC_ServerPlayer*pkPlayer);
	// 挑战后，扣除道具次数
	bool ReduceRobScoreChallengeNumAndItem(LC_ServerPlayer*pkPlayer);
	
	void UpdateRobNum(){ m_WonNum++;}

	void PackGuildRobScoreReceivedInfo(GuildRobScoreReceivedInfo& info);
	void PackGuildRobScoreInfo(GuildRobScoreInfo& info);
	void PackGuildResMsg(MG_Resp_GuildFightRobScore_Opt &msg);
	ResultType BuyRobNum(LC_ServerPlayer*pkPlayer,int nBuyNum);
	void PlayerGuildDaily(LC_ServerPlayer*pkPlayer);

	ResultType ReceiveRewardByWonNum(LC_ServerPlayer*pkPlayer,int nWonNum);
	void SaveGuildInfo(LC_ServerPlayer*pkPlayer, char_data_define::DbGuildFightInfo& info);
	void LoadGuildInfo(LC_ServerPlayer* pkPlayer, const::char_data_define::DbGuildFightInfo& info);
	bool PushReceivedData(LC_ServerPlayer* pkPlayer, int32_t nData );
private:

	int32_t								m_FreeNum;   // 免费次数 每天0 点刷新
	int32_t								m_PayingNum; // 已购买次数 用于 计算下次购买需要消耗的钻石 0点 重置为0
	int32_t								m_PaidNum;	 // 历史已购次数 （先消耗免费次数 ）
	int32_t								m_WonNum;	 //已经赢的次数
	mem::vector<int32_t>				m_ReceivedVec;// 已经 领取的奖励（3.6.9 次可以领奖）
 
};

//��Ҹ��� ���� �Ĺ�����Ϣ

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
	// ��սǰ �ж� ������ �����Ƿ��㹻
	ResultType PreIsAllowChallenge(LC_ServerPlayer*pkPlayer);
	// ��ս�󣬿۳����ߴ���
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

	int32_t								m_FreeNum;   // ��Ѵ��� ÿ��0 ��ˢ��
	int32_t								m_PayingNum; // �ѹ������ ���� �����´ι�����Ҫ���ĵ���ʯ 0�� ����Ϊ0
	int32_t								m_PaidNum;	 // ��ʷ�ѹ����� ����������Ѵ��� ��
	int32_t								m_WonNum;	 //�Ѿ�Ӯ�Ĵ���
	mem::vector<int32_t>				m_ReceivedVec;// �Ѿ� ��ȡ�Ľ�����3.6.9 �ο����콱��
 
};

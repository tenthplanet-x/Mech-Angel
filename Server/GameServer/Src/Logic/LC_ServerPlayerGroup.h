//服务器组队
#ifndef _LC_SERVER_PLAYER_GROUP_H
#define _LC_SERVER_PLAYER_GROUP_H

#include "LC_ServerPlayer.h"

class LC_ServerMapLogic_Place;
class LC_ServerPlayerGroup : public GameLogic::LC_PlayerGroupBase
{
public:
	LC_ServerPlayerGroup();
	virtual ~LC_ServerPlayerGroup();
	
	virtual void			ReUse();
	virtual void			Update(float fCurrentTime);
	virtual void			CheckAllMember(float fCurrentTime);

	GameLogic::LC_PlayerGroupEntry*	GetCheif();
	ResultType				Create(LC_ServerPlayer* player);
	ResultType				DelPlayer(const unique_id_impl& lCharID);
	ResultType				Leave(LC_ServerPlayer* player);
	ResultType				Kick(const unique_id_impl& lCharID);
	virtual void			Dismiss();

	bool					UpdatePlayerState(const unique_id_impl& lCharID,int online);
	virtual ResultType		AddPlayerToMemberList(GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType		AddPlayerToCandidateList(GameLogic::LC_PlayerBase* pkPlayer);
	virtual ResultType		RemoveCandidateInfo(const unique_id_impl& lCharID);
	void					BroadCast(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL);
	void					SendToCheif(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL);
	virtual void			SyncRaidPortal();
	bool					CheckGroupRaidExclude(LC_ServerPlayer* addplayer,LC_ServerMapLogic_Place* & raidportal,bool& addto);
	void					PackageGroupMemberList(Protocol::MG_SyncPlayerGroupMemberList& rkSyncMsg);
	bool					GetNeedRecycle();
    StringType&				GetGUID() { return m_GUID; }
    void					MakeGUID();
	void					CheifSyncGroupCandidateListToClient(const unique_id_impl& lCharID);

	bool					IsNewCreate() { return m_bNewCreate; }
	int						GetGuildMapId() { return m_nGildMapId; }
	void					SetGuildMapId(int nID) { m_nGildMapId = nID ; }
protected:
	void					_startRecycleCountDown(int cooldown);
	void					_stopRecycleCountDown();
	void					_updateGroupMemberList();
	bool					_updateEntryInfo(GameLogic::LC_PlayerGroupEntry& rkEntry);
	void					_syncGroupCandidateListToClient();	
	ResultType				_checkGuildType(LC_ServerPlayer* addplayer);
private:
	float					m_fLastSyncTime;
	int						m_lRecycleCountDown;
	bool					m_bStartRecycleCountDown;
	StringType				m_GUID;

	bool					m_bNewCreate;
	int						m_nGildMapId;
};

#endif
#pragma once

#include "LC_Define.h"
#include "MM_BaseObject.h"

class LC_ServerPlayer;
class LC_ServerPlayerEnterLeaveManager;



class LC_ServerPlayerQueueManager : public Memory::MM_BaseObject
{
public:
	LC_ServerPlayerQueueManager();
	~LC_ServerPlayerQueueManager(void);

// 	void	Update( float fCurrentTime,float fDeltaTime);
	void	CheckWaitingQueue(float fCurrentTime,float fDeltaTime);

	void	AddWaitingQueue(user_id_type userID, const unique_id_impl& charID);
	int		IsAlreadyInWaitingQueue(user_id_type userID, const unique_id_impl& charID);
	size_t	GetWaitingQueueLenth();

	void	PlayerEnterWorld(LC_ServerPlayer* player);
	void	PlayerWaitingInfo(LC_ServerPlayer* player,int waitingCount,int type);
	void	SetEnterLeaverManager(LC_ServerPlayerEnterLeaveManager* manager);
private:
	void	CheckNewbee(mem::vector<LC_ServerPlayer*>& newbeeList, unique_id_set& enterPlayerList);
	void	CheckNormal(mem::vector<LC_ServerPlayer*>& normalList, unique_id_set& enterPlayerList);
	
	int		FindNormalPlayerCount();


	struct PlayerEnterData
	{
		PlayerEnterData()
		{
			userID = 0;
			charID = 0;
			lastActiveTime = 0;
		}

		user_id_type userID;
		unique_id_impl charID;
		float lastActiveTime;
	};
	mem::list<PlayerEnterData>	m_waitingQueue;

	float m_lastCheckTime;
	LC_ServerPlayerEnterLeaveManager* m_enterLeaveManager;
};

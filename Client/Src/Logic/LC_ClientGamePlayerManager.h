#ifndef _LC_CLIENT_GAME_PLAYER_MANAGER_H
#define _LC_CLIENT_GAME_PLAYER_MANAGER_H

//客户端专用本地玩家管理器
namespace Utility
{
	class UT_TimeManager;
};

class LC_ClientGamePlayer;
class LC_ClientGamePlayerManager : public GameLogic::LC_PlayerManagerBase
{
public:
	LC_ClientGamePlayerManager();
	virtual ~LC_ClientGamePlayerManager();
	
	virtual void	Update(float fCurrentTime,float fDeltaTime);
	virtual void	Reset();

	virtual LC_PlayerBase* CreatePlayer(object_id_type objectID, const unique_id_impl& charID, int32_t lCharType, uint32_t bits=PLY_DEF_INSTANCE_BITS);
	virtual void	DestroyPlayer(GameLogic::LC_PlayerBase* pkPlayer);

protected:
	void								_event_TimerTick(MG_MessageBase& rkMessage);


	virtual GameLogic::LC_PlayerBase*   _allocPlayer();
	virtual void						_freePlayer(GameLogic::LC_PlayerBase* pkPlayer);

	Utility::UT_MemoryPool<LC_ClientGamePlayer>*		m_pkClientGamePlayerPool;
};

#endif
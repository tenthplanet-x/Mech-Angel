#ifndef _LC_SERVERMAPLOGIC_GUILD_LADDER_PLAY_H
#define _LC_SERVERMAPLOGIC_GUILD_LADDER_PLAY_H

#include "LC_Dule_Config.h"
#include "LC_ServerMapLogic_PlayBase.h"
#include "LC_Arena_Manager.h"
using namespace GameLogic;
using namespace DungeonConfig;

class LC_ServerMapLogic_GuildLadderPlay : public LC_ServerMapLogic_PlayBase
{
public:
	DECL_RTTI_NAME(LC_ServerMapLogic_GuildLadderPlay)
	LC_ServerMapLogic_GuildLadderPlay();
	virtual ~LC_ServerMapLogic_GuildLadderPlay();
	bool Init(LC_ServerMapLogic_Place* place);

	virtual bool Start();
	virtual void End();
	virtual bool Update(float fCurrentTime, float fDeltaTime);
	virtual void EventNotify(MapLogicEvent& kEvent);
	void PlayerEventNotify(LC_ServerPlayer* player, MapLogicEvent& kEvent);

	virtual bool CheckEnd();
	virtual bool CheckRun();
};



#endif
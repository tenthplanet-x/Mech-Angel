#ifndef LC_GAMESTORY_TRANSCRIPT_H
#define LC_GAMESTORY_TRANSCRIPT_H

#include "LC_GameEvent.h"
#include "LC_ServerPlayer.h"
#include "LC_ActorBase.h"
#include "LC_TranscriptionDataDefine.h"
#include "LC_TranscriptionDataType.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_GameStoryBase.h"

namespace GameStory
{


class LC_GameStory_Transcript : public LC_Server_GameStory_Base
{
public:
	LC_GameStory_Transcript(int logicID, GameStoryRecord *pkRecord);
	virtual			~LC_GameStory_Transcript();

	virtual void	Init();
	virtual void	UnInit();

	virtual void	ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase*  pkInjured);
	virtual void	DoTranscriptionEntered(LC_ServerMapLogic_Place* pkPlace, LC_ServerPlayer *pkPlayer);
	virtual void	DoTranscriptionFinished(LC_ServerMapLogic_Place* pkPlace, GSTranscriptResult &trans);

private:
};
}
#endif
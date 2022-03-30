#ifndef _LC_GAMESTORY_WILDBOSS_H
#define _LC_GAMESTORY_WILDBOSS_H
//“∞Õ‚À¢π÷ªÓ∂Ø
#include "LC_GameEvent.h"
#include "LC_ServerPlayer.h"
#include "LC_ActorBase.h"
#include "LC_TranscriptionDataDefine.h"
#include "LC_TranscriptionDataType.h"
#include "LC_GameStoryDataDefine.h"
#include "LC_GameStoryBase.h"

namespace GameStory
{
	class LC_Server_GameStory_WildBoss : public LC_Server_GameStory_Base
	{
	public:
		LC_Server_GameStory_WildBoss(int32_t logicID, GameStoryRecord *pkRecord);
		virtual ~LC_Server_GameStory_WildBoss();

		//virtual void	ActivateStory(int iTimeStart,int iTimeEnd);
		//virtual void	DeactivateStory();

		virtual void	Init();
		virtual void	UnInit();

		virtual void ActorBeKilled(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured);
		virtual void ActorBeDamaged(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkInjured, int32_t iMapLogicID, attr_value_type dmg);
	protected:

	private:
		virtual void	_onActived(void);
		virtual void	_onDeactived(void);

	protected:
		bool			m_bTipsRemaindFiveMinutesFlag;
		bool			m_bTipsRemaindOneMinutesFlag;
	};

}
#endif
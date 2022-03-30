//GameServer端技能事件管理器

#ifndef _SK_SERVER_SKILL_EVENT_MANAGER_H
#define _SK_SERVER_SKILL_EVENT_MANAGER_H
#include "UT_MemoryPool.h"
#include "SK_SkillEventManagerBase.h"
#include "LC_ActorBase.h"
namespace Skill
{
	class SK_ServerSkillEventManager : public SK_SkillEventManagerBase
	{
	public:
		enum E_ProcessState
		{
			E_ProcessState_Init = 0,
			E_ProcessState_Chain,
			E_ProcessState_Main,
			E_ProcessState_Done,
		};
	public:
		SK_ServerSkillEventManager();
		virtual ~SK_ServerSkillEventManager();

		virtual void		   Update(float fCurrentTime, float fDeltaTime);
		virtual SK_SkillEvent* AllocEvent();
		virtual void		   AddEvent(SK_SkillEvent* pkEvent);
		virtual void		   ProcImmediateEvent(SK_SkillEvent* pkEvent);
		void				   SendToClient(GameLogic::LC_ActorBase* pkActor, SK_SkillEvent* pkEvent, SendSkillEventType eSendType);
		void				   SendMsgToClient(GameLogic::LC_ActorBase* pkActor, uint16_t usMsgType, Protocol::MG_BODY* pkMsg, SendSkillEventType eSendType);
		StringType				GetRTInfo(void);

	private:
		void _procEventList(float fCurrentTime, SK_SkillEventList& rkEventList, int nMaxSize);
		void _clearEventList(SK_SkillEventList& rkEventList);
		//事件处理
		void _procEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procCostEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procRecoverEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procDamageEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procAllDamageEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);//一次generateDamage所造成的所有伤害（统计用，目标身上buff等条件，造成的伤害变化无法统计）
		void _procExecuteSkillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procInterruptEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procAttachStateEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procDetachStateEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBulletEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procKillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procEnterGuardEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBroadcastEnemyEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procAttachGfxEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procRetreateEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procAttachSoundEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procAttachDisplayChestEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procPlayerEnterMapEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procDetachGfxEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent)  ;

		void _procClientEyeSkillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);

		void _procSummorDamage(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procImmMoveEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		
		void _procBulletToTarget(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBulletToLocation(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);

		//辅助函数
		object_id_vector _bulletLockTarget(GameLogic::LC_ActorBase* pkAttacker, SK_BulletEvent* pkBulletEvent, const Utility::UT_Vec3Int& rkCircleCenter, int iCircleRadius, int lActorFilterFuncID, int iMaxLockCount, bool bShotBack = false, bool bRepeatStrike = false);
		void _broadcastBulletChangeTargetLocation(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, const Utility::UT_Vec3Int& rkTargetLocation);
		void _broadcastBulletChangeTargetID(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, long lTargetID);
		void _broadcastBulletPenetrate(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation);
		void _broadcastBulletExplosive(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, long lTargetID, SK_BulletEvent::SK_BulletExplosiveReason enmReason);

		//通知双方身上状态（状态可能对事件进行修改）
		void _notifyState(GameLogic::LC_ActorBase* pkSrcActor, GameLogic::LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent);
		//通知双方状态控制器（不对事件进行修改）
		void _notifyController(GameLogic::LC_ActorBase* pkSrcActor, GameLogic::LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent);
		//计算后通知状态
		void _settleState(GameLogic::LC_ActorBase* pkSrcActor, GameLogic::LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent);

		Utility::UT_MemoryPool<SK_SkillEvent> m_kEventPool;
		SK_SkillEventList		m_kMainEventList;
		SK_SkillEventList		m_kChainEventList;

		int						m_iAllocEventID;
		int						m_nProcessingEvent;

		//////////////////////////////////////////////////////////////////////////
		// AcitveBuddy子弹的相关请求，包括穿刺、爆炸、改变目的地
	public:
		bool			ActiveBuddyReqBulletPenetrate(GameLogic::LC_ActorBase* pkActor, const MG_REQ_Bullet_Penetrate & req);
		bool			ActiveBuddyReqBulletExplosive(GameLogic::LC_ActorBase* pkActor, const MG_REQ_Bullet_Explosive & req);
		bool			ActiveBuddyReqBulletChangeTargetLocation(GameLogic::LC_ActorBase* pkActor, const MG_REQ_Bullet_ChangeTargetLocation & req);
		SK_SkillEvent*  FindEvent(int eventID);

	private:
		SK_SkillEventList		m_kActiveBuddyBulletReqEvents;

		void            _procActiveBuddyBulletReqEvent(float fCurrentTime, SK_SkillEvent * pkEvent, bool& bDestroyEvent);
		void			_updateActiveBuddyBulletReqEvent(float fCurrentTime);
		int64_t			_BULLET_UUID(long lEventID, long lLocalID);		
		//////////////////////////////////////////////////////////////////////////
	};

#define SERVER_GET_SKILL_EVENT_MANAGER (SK_ServerSkillEventManager*)SK_ServerSkillEventManager::GetSingletonPtr

}

#endif

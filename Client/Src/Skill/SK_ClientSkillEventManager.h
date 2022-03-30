//用于管理客户端的技能事件
#ifndef _SK_CLIENT_SKILL_EVENT_MANAGER_H
#define _SK_CLIENT_SKILL_EVENT_MANAGER_H

#include "SK_SkillEventManagerBase.h"

namespace Skill
{
	class SK_ClientSkillEventManager : public SK_SkillEventManagerBase
	{
	public:
		SK_ClientSkillEventManager();
		virtual ~SK_ClientSkillEventManager();

		virtual void				Update(float fCurrentTime, float fDeltaTime);
		virtual SK_SkillEvent*		AllocEvent();
		virtual void				AddEvent(SK_SkillEvent* pkEvent);
		virtual void				ProcImmediateEvent(SK_SkillEvent* pkEvent);
		virtual SK_SkillEvent*		FindBulletEvent(long lEventID, long lBulletLocalID, object_id_type lShooterID);
		
		//处理网络消息
		void	RecvMessageFromServer(const msg_define::skill_event& kpbSkillEvent, bool bArenaPlayBack = false);
		// 处理服务器合并的子弹消息，‘服务器子弹’，怪物、buddy，不包括服务器转发的NetPlayer的子弹
		void	RecvShotBulletMergedFromServer(const Protocol::MG_SyncBulletInfo & rkMsg, bool bArenaPlayBack = false);
		//处理客户端消息
		void	RecvMessageFromClient(SK_SkillEvent* pkEvent);

		void	DeleteAllBulletEvent();

	private:
		void _recvMessageFromServer(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);

		//接收服务器发送的技能事件
		void _recvRecoverMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvDamageMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvAttachStateMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvDetachStateMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvBulletShotMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvBulletChangeTargetLocationMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvBulletPenetrateMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvBulletExplosiveMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvKillMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvAttachGfxMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvAttachSoundMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvAttachDisplayChestMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvDetachGfxMessage(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		void _recvImmunePunch(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		//处理延迟伤害
		void _recvSummorDamageEvent(SK_SkillEvent* pkEvent);
		//处理客户端天眼放技能时间
		void _procClientEyeSkillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);

		//处理技能事件
		void _procEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBulletEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBulletToTarget(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBulletToLocation(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);

		//辅助函数
		void _bulletExplosive(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		object_id_vector _bulletLockTarget(GameLogic::LC_ActorBase* pkAttacker, SK_BulletEvent* pkBulletEvent, const Utility::UT_Vec3Int& rkCircleCenter, int iCircleRadius, int lActorFilterFuncID, int iMaxLockCount, bool bShotBack = false, bool bRepeatStrike = false);
		void _requestBulletChangeTargetLocation(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, const Utility::UT_Vec3Int& rkTargetLocation, long lTargetID, long lNextTargetID);
		void _requestBulletPenetrate(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, long lTargetID, long lNextTargetID);
		void _requestBulletExplosive(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, long lTargetID, SK_BulletEvent::SK_BulletExplosiveReason enmReason);
		void _modifyBulletZValue(SK_SkillEvent* pkEvent);

		//处理客户端打击点事件
		void _procClientAttackEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);

		//处理客户端打击点事件
		void _procSummorEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);

		//通知双方身上状态（状态可能对事件进行修改）
		void _notifyState(GameLogic::LC_ActorBase* pkSrcActor, GameLogic::LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent);

		//播放技能打击声音
		void _playHitSound(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkTarget, CSVFile::CF_SkillExecuteList::DataEntry* pkSkillData);
		//主角被击播放屏幕特效
		void _playBeAttackScreenGfx(GameLogic::LC_ActorBase* pkAttacker);
		//创建客户端表现掉落物
		void _createDisplayHolder(const UT_Vec3Int& kLocation, int lItemTypeID, int FlyToID, int flyType);

		//事件管理接口
		//注意：客户端的技能事件管理不提供外部接口
		SK_SkillEvent* _allocEvent();
		void		   _addEvent(SK_SkillEvent* pkEvent);
		void		   _removeEvent(SK_SkillEvent* pkEvent);
		void		   _freeEvent(SK_SkillEvent* pkEvent);

		SK_SkillEvent* _findBulletEvent(long lEventID, long lBulletLocalID, object_id_type lShooterID);

		// 事件对象池和列表
		Utility::UT_MemoryPool<SK_SkillEvent> m_kEventPool;
		SK_SkillEventList m_kMainEventList;
	};

#define CLIENT_GET_SKILL_EVENT_MANAGER (SK_ClientSkillEventManager*)SK_ClientSkillEventManager::GetSingletonPtr
}

#endif
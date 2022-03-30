//���ڹ���ͻ��˵ļ����¼�
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
		
		//����������Ϣ
		void	RecvMessageFromServer(const msg_define::skill_event& kpbSkillEvent, bool bArenaPlayBack = false);
		// ����������ϲ����ӵ���Ϣ�����������ӵ��������buddy��������������ת����NetPlayer���ӵ�
		void	RecvShotBulletMergedFromServer(const Protocol::MG_SyncBulletInfo & rkMsg, bool bArenaPlayBack = false);
		//����ͻ�����Ϣ
		void	RecvMessageFromClient(SK_SkillEvent* pkEvent);

		void	DeleteAllBulletEvent();

	private:
		void _recvMessageFromServer(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);

		//���շ��������͵ļ����¼�
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
		//�����ӳ��˺�
		void _recvSummorDamageEvent(SK_SkillEvent* pkEvent);
		//����ͻ������۷ż���ʱ��
		void _procClientEyeSkillEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);

		//�������¼�
		void _procEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBulletEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBulletToTarget(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);
		void _procBulletToLocation(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);

		//��������
		void _bulletExplosive(SK_SkillEvent* pkEvent, bool bArenaPlayBack = false);
		object_id_vector _bulletLockTarget(GameLogic::LC_ActorBase* pkAttacker, SK_BulletEvent* pkBulletEvent, const Utility::UT_Vec3Int& rkCircleCenter, int iCircleRadius, int lActorFilterFuncID, int iMaxLockCount, bool bShotBack = false, bool bRepeatStrike = false);
		void _requestBulletChangeTargetLocation(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, const Utility::UT_Vec3Int& rkTargetLocation, long lTargetID, long lNextTargetID);
		void _requestBulletPenetrate(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, long lTargetID, long lNextTargetID);
		void _requestBulletExplosive(GameLogic::LC_ActorBase* pkBroadcastCenter, SK_BulletEvent* pkEvent, const Utility::UT_Vec3Int& rkBulletLocation, long lTargetID, SK_BulletEvent::SK_BulletExplosiveReason enmReason);
		void _modifyBulletZValue(SK_SkillEvent* pkEvent);

		//����ͻ��˴�����¼�
		void _procClientAttackEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);

		//����ͻ��˴�����¼�
		void _procSummorEvent(float fCurrentTime, SK_SkillEvent* pkEvent, bool& bDestroyEvent);

		//֪ͨ˫������״̬��״̬���ܶ��¼������޸ģ�
		void _notifyState(GameLogic::LC_ActorBase* pkSrcActor, GameLogic::LC_ActorBase* pkDstActor, SK_SkillEvent* pkEvent);

		//���ż��ܴ������
		void _playHitSound(GameLogic::LC_ActorBase* pkAttacker, GameLogic::LC_ActorBase* pkTarget, CSVFile::CF_SkillExecuteList::DataEntry* pkSkillData);
		//���Ǳ���������Ļ��Ч
		void _playBeAttackScreenGfx(GameLogic::LC_ActorBase* pkAttacker);
		//�����ͻ��˱��ֵ�����
		void _createDisplayHolder(const UT_Vec3Int& kLocation, int lItemTypeID, int FlyToID, int flyType);

		//�¼�����ӿ�
		//ע�⣺�ͻ��˵ļ����¼������ṩ�ⲿ�ӿ�
		SK_SkillEvent* _allocEvent();
		void		   _addEvent(SK_SkillEvent* pkEvent);
		void		   _removeEvent(SK_SkillEvent* pkEvent);
		void		   _freeEvent(SK_SkillEvent* pkEvent);

		SK_SkillEvent* _findBulletEvent(long lEventID, long lBulletLocalID, object_id_type lShooterID);

		// �¼�����غ��б�
		Utility::UT_MemoryPool<SK_SkillEvent> m_kEventPool;
		SK_SkillEventList m_kMainEventList;
	};

#define CLIENT_GET_SKILL_EVENT_MANAGER (SK_ClientSkillEventManager*)SK_ClientSkillEventManager::GetSingletonPtr
}

#endif
//���������ܽű��ӿ�
#ifndef _SC_SERVER_SKILL_INTERFACE_H
#define _SC_SERVER_SKILL_INTERFACE_H

#include "SC_SkillInterface.h"

class SC_ServerSkillInterface : public Script::SC_SkillInterface
{
public:
	SC_ServerSkillInterface();
	virtual ~SC_ServerSkillInterface();
	DECL_RTTI_NAME(SC_ServerSkillInterface)

	unique_id_type			ToNowID(unique_id_type sid);

	virtual bool			IsInCross(GameLogic::LC_Attacker* pkScriptCaller);

	object_id_type			GetLCLockedID(GameLogic::LC_Attacker* pkScriptCaller);

	virtual void			FaceToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	virtual void			BackToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	virtual void			SetTargetLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	virtual void			SetLockedLocationByDir(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iFaceDistance);

	//��������ΪĿ���, bLockedTarget�Ƿ��ǵ�ǰ����Ŀ��
	virtual void				SetLockedLocationByMouseLocation(GameLogic::LC_Attacker* pkScriptCaller, bool bLockedTarget);

	// CombatEvent
	virtual void RegisterCombatEventData(GameLogic::LC_Attacker* pkScriptCaller,  int eEventType, int nKey, int nValue );
	virtual void UnregisterCombatEventData(GameLogic::LC_Attacker* pkScriptCaller,  int eEventType, int nKey );

	virtual void RegisterCombatEventPassiveCrit(GameLogic::LC_Attacker* pkScriptCaller,  GameLogic::LC_Attacker* pkAttacker, int nValue );
	virtual void UnregisterCombatEventPassiveCrit(GameLogic::LC_Attacker* pkScriptCaller,  GameLogic::LC_Attacker* pkAttacker );

	//trick ֱ�Ӵ�����ֵ��ȥ�ߣ�û�취��
	virtual void			SetTargetLocationValue(GameLogic::LC_Attacker* pkScriptCaller, int posx, int posy, int posz);

	virtual bool          ModifyVIPGrowthValue(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int64_t iDeltaValue);
	virtual void  		   ModifyTargetAttributeByValue(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iAttributeType, int64_t iDeltaValue, bool bRevert);
	virtual void		   ModifyTargetAttributeByPercent(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,double fDeltaPercent,bool bRevert);
	//��������������
	virtual void			SetTargetVehicleType(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lVehicleType);

	virtual void			NotifyCastSkillToAchievement(GameLogic::LC_Attacker* pkScriptCaller, int achievementType);

	virtual void			NotifyEventToGoal(GameLogic::LC_Attacker* pkScriptCaller, int goalType, int param);

	virtual void			NotifyCastSkillToGatherTaskNpc(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//��ȡ���޸ļ����¼������ӿ�
	//virtual object_id_type GetEventSrcID(Skill::SK_SkillEvent* pkEvent);
	//virtual object_id_type GetEventDstID(Skill::SK_SkillEvent* pkEvent);
	//virtual object_id_type GetEventLogicID(Skill::SK_SkillEvent* pkEvent);
	//virtual int GetEventParam(Skill::SK_SkillEvent* pkEvent, int iParamIndex);
	virtual void			SetEventParam(Skill::SK_SkillEvent* pkEvent, int iParamIndex, int64_t lParam);


	//��Ŀ����Χ��ɢ�ӵ�
	/*
	��	   iTargetIndex :	    Ŀ��˵��
	   lMinRadius :			����뾶
	   lMaxRadius :			��Զ�뾶
	   lSkillTypeID :		�ӵ������ļ���
	   iBulletFuncID :		�ӵ�����ID
	   iPenetrateCount :	�ӵ���͸����
	   iDummyType :			��Ч�����Ĳ�λ
	*/
	virtual void			ShotBulletAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType);
	//��Ŀ�귢ɢ�ӵ�
	/*
	�� iTargetIndex :	    Ŀ��˵��
	   lSkillTypeID :		�ӵ������ļ���
	   iBulletFuncID :		�ӵ�����ID
	   iPenetrateCount :	�ӵ���͸����
	   iDummyType :			��Ч�����Ĳ�λ
	*/
	virtual void			ShotBulletToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType);
	virtual void			ShotBulletToTargetAngularDeflection(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, float fAngle = 0);
	virtual void			ShotBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);
	virtual void			ShotMultiBulletToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt);
	virtual void			ShotMultiBulletToTargetFromLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);
	virtual void			ShotMultiBulletToTargetFromMultiLocation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType, int iScatteringAngle, int iScatteringCnt, int iLocationIndex, float fDeltaAngle, int64_t lBulletID);	
			bool			ActiveBuddyReqBulletShot(GameLogic::LC_Attacker* pkScriptCaller, const MG_REQ_Bullet_Shot & req);

			void			ModifyTargetControllerAttributeByValue(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,int64_t iDeltaValue,bool bRevert, bool computerCombatScore=false, int32_t reasonType=SPART_None);
	virtual void			ShotBulletToTargetFromState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType);
	//��Ŀ�귢�䰵��
	/*
	�� iTargetIndex :	    Ŀ��˵��
	   lSkillTypeID :		�ӵ������ļ���
	   iBulletFuncID :		�ӵ�����ID
	   iPenetrateCount :	�ӵ���͸����
	   iDummyType :			��Ч�����Ĳ�λ
	   iMoveType :			�ӵ����ͣ�BT_SHOOT��BT_CHAIN��BT_CAST��BT_MISSILE��
	   fMoveSpeed :			1��������ǲ�Ϊ0���������ʱ�䣻2���������ӵ��ٶ�
	   fPitchAngle :		�����ǣ�ֻ��BT_CAST��BT_MISSILE�ӵ�����Ч��
	   fRollAngle :			����ǣ�ֻ��BT_MISSILE�ӵ�����Ч��
	   fTurningPercentage :	�յ���ֱ���ϵİٷֱ�λ�ã�ֻ��BT_MISSILE�ӵ�����Ч��
	*/
	struct ShotBulletArg
	{
		Skill::SK_SkillExecute	*	pkSkillExecute;
		const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet;	// SkillBulletFunc.csv ����
		int				lSkillTypeID;		// �ӵ���������
		int				iBulletFuncID;		// �����е��ӵ�����
		int				iPenetrateCount;	// ���̴���
		int				iDummyType;			// ����dummy

		float			fStartTime;			// ����ʱ��
		float			fMoveTime;
		UT_Vec3Int		kShotLocation;		// ����λ��
		UT_Vec3Int		kDestLocation;		// Ŀ��λ��
		bool			bClientBullet;		// �ͻ����ӵ�
		float			fPitchAngle;		// ��������

		// volatile��ÿ���ӵ���һ��
		UT_Vec3Float	kShotDir;			// ��ǰ�ӵ����������
		int				iClientLocalID;		// ��ǰ�ӵ���ClientLocalID
		float			fCollidePosParamT;	// ��ǰ�ӵ�����ײ����ֵ��ֱ���ͣ��������ӵ�Ĭ��1.0001f��˵����Ҫ����ײ���
		int				iCollideSurfaceMaterial;	// ��ײ������������

		ShotBulletArg() { ::memset(this, 0, sizeof(ShotBulletArg));}
	};

	//���㼼���˺�
	/*
	iDamageTargetIndex :	�˺�Ŀ��
	lSkillTypeID :			��������
	iDamageRandomIndex :	�˺�������
	iAttackRateRandomIndex :��������
	iCriticalRateRandomIndex :��������
	lDamageSourceActorID :	���������ߵ�ID
	iHPCost				 :	���μ��ܵ�HP���ģ����ڼ����˺����ò�����ʱ����
	iMPCost				 :	���μ��ܵ�MP���ģ����ڼ����˺����������0���������Զ����㼼��MP���ģ�Ĭ��MP���ĺͶ������ģ�
	*/
	virtual void			GenerateSkillDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost);

	//���㼼���˺�
	/*
	iDamageTargetIndex :	�˺�Ŀ��
	lSkillTypeID :			��������
	iDamageRandomIndex :	�˺�������
	iAttackRateRandomIndex :��������
	iCriticalRateRandomIndex :��������
	lDamageSourceActorID :	���������ߵ�ID
	iHPCost				 :	���μ��ܵ�HP���ģ����ڼ����˺����ò�����ʱ����
	iMPCost				 :	���μ��ܵ�MP���ģ����ڼ����˺����������0���������Զ����㼼��MP���ģ�Ĭ��MP���ĺͶ������ģ�
	*/
	virtual void			GenerateSkillDamageMainTarget(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int iPercent);

	virtual void			GenerateSkillDamageMainTargetMulTiple(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple , int iPercent);
	//���԰��ձ�������ɼ����˺�
	//
	virtual void			GenerateSkillDamageMultiple(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent);

	virtual void			GenerateSkillDamageMultipleWithPunchFlag(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent, bool bDoPunch);

	//���㼼���˺�
	virtual void			GenerateSkillDamageNoScore(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost);

	//��������˺�
	/*
	iDamageTargetIndex :	Ŀ��˵��
	lDamageLogicTypeID :	�����˺��ļ��ܱ��
	lDamageType :			�˺�������
	iDamage :				�˺�����
	lDamageSourceActorID :	�����˺��ߵı��
	*/
	virtual void			GenerateDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID);

	//��������˺�(����˺����㹥���ߵ�ATT_DMG_RATE����)
	/*
	iDamageTargetIndex :	Ŀ��˵��
	lDamageLogicTypeID :	�����˺��ļ��ܱ��
	lDamageType :			�˺�������
	iDamage :				�˺�����
	lDamageSourceActorID :	�����˺��ߵı��
	*/
	virtual void			GenerateDamage2(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID);

	//���㾫Ӣ�ֵļ����˺�(�����⹫ʽ�����������
	/*
		iDamageTargetIndex :	Ŀ��˵��
		lDamageLogicTypeID :	�����˺��ļ��ܱ��
		lDamageType :			�˺�������
		iDamage :				�˺�����
		lDamageSourceActorID :	�����˺��ߵı��
	*/
	virtual void			GenerateEliteDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID);
	virtual void			DetachStateAndGenerateDamage(GameLogic::LC_Attacker* pActor, int targetIndex, int sourceIndex, int stateID, int skillID, int damage, int damageType, bool pure);

	//����HP��������˺�
	virtual void			GenerateDamageByHPPercent(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, int iHPType, double fHPPercent, object_id_type lDamageSourceActorID);
	//�������Ա�������˺�
	virtual void			GenerateDamageByAttributePercent(GameLogic::LC_Attacker* pCaller, int iTargetIndex, int iSkillID, int iDamageType, int iAttributeType, float fAttrPercent, int iSourcerIndex, bool bCountDef);
	//����ֵ��ʽ����Ŀ���HP
	virtual void			CostTargetAbsHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue);
	//�ٷֱȷ�ʽ����Ŀ��Ļ���HP���ȼ���Ӧ�����HP��
	virtual void			CostTargetDefaultPercentHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent);
	//�ٷֱȷ�ʽ����Ŀ��Ķ���HP������װ��������HP��
	virtual void			CostTargetAdditionPercentHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent);
	//����ֵ��ʽ����Ŀ���MP
	virtual void			CostTargetAbsMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue);
	//�ٷֱȷ�ʽ����Ŀ��Ļ���MP���ȼ���Ӧ�����MP��
	virtual void			CostTargetDefaultPercentMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent);
	//�ٷֱȷ�ʽ����Ŀ��Ķ���MP������װ��������MP��
	virtual void			CostTargetAdditionPercentMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent);

	//���ļ�����Դ�������ͼ��ܵ�HP,MP�ڽű��п۳�
	virtual void			CostTargetSkillResource(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSkillTypeID);

	//�����
	virtual int			ActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int nBuddyId, int lSkillTypeID);

	//��ȡ���ܼ���״̬
	virtual int			CheckActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID);

	//ѧϰ����
	virtual void			LearnSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID);

	//���ü��ܿ��ô���
	virtual void			SetSkillUsableCount(GameLogic::LC_Attacker* pAttacker,int targetIndex, int skillID, int usableCount);

	//ѧϰ�䷽
	virtual void			LearnFormula(GameLogic::LC_Attacker* pkScriptCaller, int lFoumulaID);

	//�ͻ�������ִ�м���
	virtual void			NotifyClientEyeSkill(GameLogic::LC_Attacker* pkScriptCaller, int skillID);

	//��Ŀ���һ����Ч
	virtual void			AttachGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic,
										bool bApplyDirection, object_id_type lSrcActorID, int sendType, int iLCGfx, int iBindMode);
	virtual void			AttachTargetGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic);
	virtual void			AttachTargetGfxSingleSend(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic);
	virtual void			AttachTargetGfxEx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic,
										bool bApplyDirection, object_id_type lSrcActorID, int sendType);
	void					AttachTargetGfxImp(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic,
										bool bApplyDirection, object_id_type lSrcActorID, int sendType, int iLCGfx = 0, int iBindMode = 0);
	virtual void			AttachTargetLCGfx(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, bool bStayStatic, bool bApplyDirection, object_id_type lSrcActorID, int sendType, int iLCGfx);
	virtual void			AttachTargetDisplayChest(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iItemTypeID, object_id_type lSrcActorID, int dummyType, int flyType);

	//��Ŀ��ȡ��һ����Ч
	virtual void			DetachTargetGfxServer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, int broadCastType);

	//��Ŀ�������
	virtual void			AttachTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, object_id_type lSrcActorID, object_id_type lLogicID, 
										int iSoundChannel, int iSoundType, int iBroadCastType);

	//����Ŀ��
	virtual void			RespawnTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t iHP, double fHPPercent, int64_t iMP, double fMPPercent);


	//�жϹ����Ƿ�����
	virtual bool			IsAttackHit(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iAttackRateRandomIndex);

	//��ǰ�����Ƿ񱩻� ��ͨ��������ʹ��
	virtual bool			IsAttackCritical(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iAttackRateRandomIndex, int iCriticalRateRandomIndex);

	//����˫����ֵ,��������˺�ֵ
	/*
		iDamageTargetIndex :	Ŀ��˵��
		lSkillTypeID :			�����˺��ļ���ID
		iDamageRandomIndex :	�˺�������
		iHPCost			   :	���μ��ܵ�HP���ģ����ڼ����˺����ò�����ʱ����
		iMPCost			   :	���μ��ܵ�MP���ģ����ڼ����˺����������0���������Զ����㼼��MP���ģ�Ĭ��MP���ĺͶ������ģ�
	*/
	virtual int64_t				ComputeDamageBase(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iCriticalRateRandomIndex, int iHPCost, int iMPCost);


	//�����Ʒ��������
	virtual void			AddItemToPack(GameLogic::LC_Attacker* pkScriptCaller, int iItemID, int iItemNum, int iReason);

	virtual void			AddItemToPackByDrop(GameLogic::LC_Attacker* pkScriptCaller, int targetIndex);

	//�Ƴ���������Ʒ
	virtual bool			DeleteItemInPack(GameLogic::LC_Attacker*  pkScriptCaller, int iItemID, int iItemNum, int iReason);

	//����NPC
	virtual int				ActiveSpawnPoint(GameLogic::LC_Attacker* pkScriptCaller, int lSpawnPointID);


	//��������
	virtual void			LearnVehicle(GameLogic::LC_Attacker* pkScripterCaller, int lVehicleSkillID);

	//��Ŀ�����״̬
	/*
	��  iTargetIndex :	    Ŀ��˵��
	   lStateTypeID :		״̬����ID
	   fPara0 - fPara4:    ״̬��ǰ5���ɱ����
	   */
	virtual int				AttachSkillState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID, int lStateLifeTime, 
								float fPara0 = 0.0f, float fPara1 = 0.0f, float fPara2 = 0.0f, float fPara3 = 0.0f, float fPara4 = 0.0f);

		//��Ŀ���ָ�ӹ����״̬
		/*
		��  iTargetIndex :	    Ŀ��˵��
		    lStateTypeID :		״̬����ID
			fPara0 - fPara4:    ״̬��ǰ5���ɱ����
			NOTE: Ŀǰ���API����Ҫ��Ŀ����Player����ΪPlayer����ָ�ӹ�
		*/
	virtual int					AttachSkillStateToController(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID, int lStateLifeTime, 
		float fPara0 = 0.0f, float fPara1 = 0.0f, float fPara2 = 0.0f, float fPara3 = 0.0f, float fPara4 = 0.0f);

	//�����е�Ŀ�����״̬
	/*
	��  iTargetIndex :	    Ŀ��˵��
	   lStateTypeID :		״̬����ID
	   fPara0 - fPara4:    ״̬��ǰ5���ɱ����
	   */
	virtual void			AttachHitedTargetSkillState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID,	int lStateLifeTime, 
								float fPara0 = 0.0f, float fPara1 = 0.0f, float fPara2 = 0.0f, float fPara3 = 0.0f, float fPara4 = 0.0f);


	//��ɢĿ��״̬
	/*
		�����ɢ����״̬���򷵻���ɢ״̬���ص�����
		�����ɢһ��״̬��������ɢ״̬�ĸ���
	*/
	virtual int				DetachTargetState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel, int iMaxStateNum);
	virtual int				DetachTargetStateToController(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel, int iMaxStateNum);
	virtual void			NotifyClientDetachTargetState(int attachID, int dstID, int stateID);

	//���ݻ������ͣ���ȡĿ�����ϵ�״̬
	virtual int				GetTargetStateCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel);

	//���ݻ������ͣ���ȡĿ������״̬���ص���
	virtual int				GetTargetStateOverlapCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel);

	virtual void			DirtySkillState(GameLogic::LC_Attacker* pkScriptCaller, int iStateID);
	//ת��״̬
	virtual int				TranslateState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDestIndex, int iBuffType, int iKind, int iLevel);

	//��Ŀ����Ӽ���CD
	virtual void			SetTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, float fCDTime);
	virtual void			ResetTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,float fCDTime);
	virtual void			SetTargetSkillCDOnce(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, float fCDTime);

	//��Ŀ����Ӽ�������CD
	virtual void			SetTargetSkillTypeCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDType, int iTypeID, float fCDTime);

	//��Ŀ���������CD
	virtual void			ClearTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID);

	//��Ŀ�������������CD
	virtual void			ClearTargetSkillTypeCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDType, int iTypeID);

	//��Ŀ���������CD
	virtual void			ClearTargetGlobalCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//�޸ļ���CD
	virtual void			ModifyTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDModifyFuncID, bool bRevertModify);

	//�õ����ܵ�ǰCD
	virtual float			GetTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID);


	//�����������е���������
	virtual int				GetShortBarGodSkill(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int ShortBarIndex);

	//���Ŀ�굱ǰ����
	virtual void			InterruptTargetCurrentSkill(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);


	//��һس�
	virtual void			ReturnToCity(GameLogic::LC_Attacker* pkScriptCaller, bool needadd=false);

	virtual bool			CanReturnToCity(GameLogic::LC_Attacker* pkScriptCaller);

	//�Ƿ��ǿ��
	bool					ServerPlayerCross(GameLogic::LC_Attacker* pkScriptCaller);

	//�ر�����
	//virtual void ClosePlaceHolder(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//����NPC
	virtual object_id_type	SpawnNPCNearObject(GameLogic::LC_Attacker* pkScriptCaller, object_id_type nObjectId, int lRadius, int lCharTypeID, int iFictionType, int lSurvialTime);

	//����NPC
	virtual object_id_type	SpawnNPCByPointID(GameLogic::LC_Attacker* pkScriptCaller, int lSpawnPointID, int lCharTypeID, int lSurvivalTime);

	//����NPC
	virtual object_id_type	SpawnNPCNearTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lRadius, int lCharTypeID, int iFictionType, int lSurvialTime);

	//����NPC
	virtual object_id_type	SpawnNPCAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lCharTypeID, int iFictionType, int lSurvialTime);

	//����NPC
	virtual object_id_type	SpawnNPCForwardTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int forwardDis, float angle, int lCharTypeID, int iFictionType, int lSurvialTime);

	//����npc�ٻ���id
	virtual void			SetNPCSummonOwnerID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, object_id_type lSummonOwnerID);
	//����npc�ٻ���id �Ұ��ٻ��������Ը�ֵ��npc
	virtual void			SetNPCFromSummonOwnerAttr(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, object_id_type lSummonOwnerID);
	
	//�����ڳ�״̬
	virtual void SetEscortNpcState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int EscortNpcState);

	//����npc�Ѿ��ٻ���ID
	virtual void			SetNPCSummorID(GameLogic::LC_Attacker* pkScriptCaller, object_id_type lSummonID);

	//�������
	virtual void			FinishTask(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID);

	//��������
	virtual void			CancelTask(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID);

	//��������ʱ�䴥����
	virtual void			DestineTaskTimeEvent(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int lTime, bool bNoticeClient);

	//��������ʧ��״̬
	virtual void			SetTaskFail(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iState);

	//��ͼ����Чǽ�ŵ�״̬����
	virtual void			ControlMapLogicDoorState(GameLogic::LC_Attacker* pkScriptCaller,  int iDoorIndex, int iState);

	//�ָ�Ŀ��HP
	/*
		iTargetIndex :		Ŀ��˵��
		lSourceTypeID :		�����ָ�HP�¼��˵�ID
		lRecoverHP :		HP�ָ�����ֵ
		fHPRecoverPercent : HP�ָ��ٷֱ�
	*/
	virtual void			RecoverTargetHP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, attr_value_type lRecoverHP, float fHPRecoverPercent, object_id_type lSrcActorID);

	//�ָ�Ŀ��MP
	/*
		iTargetIndex :		Ŀ��˵��
		lSourceTypeID :		�����ָ�MP�¼��˵�ID
		lRecoverMP :		MP�ָ�����ֵ
		fMPRecoverPercent :	MP�ָ��ٷֱ�
	*/
	virtual void			RecoverTargetMP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, attr_value_type lRecoverMP, float fMPRecoverPercent, object_id_type lSrcActorID);
	//����Ŀ��HP
	/*
		iTargetIndex :		Ŀ��˵��
	*/
	virtual void			SetTargetHP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t lHP);

	virtual void			AddEscortTimes(object_id_type playerID);
	virtual void			UpdateEscortRecord(GameLogic::LC_Attacker* pkScriptCaller);

	//Ŀ����ҷ�ʽ��SetTargetHP��ͬ��ȫ�ֲ��ң����ǽ��ڵ�ǰ��ͼ��
	virtual void			KillNpcAnyhow(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//���ĶԷ���Ʒ
	/*
	iTargetIndex :		Ŀ��˵��
	lSourceTypeID :		����������Ʒ�¼��˵�ID
	lItemTypeID :		��Ʒ����
	lItemCount :		��Ʒ����
	*/
	virtual bool			CostTargetItem(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int lItemTypeID, int lItemCount);

	virtual int				RandomTreasureEvent(GameLogic::LC_Attacker* pkScriptCaller, int itemID);
	virtual int				GetTreasureEventType(GameLogic::LC_Attacker* pkScriptCaller, int eventIndex);
	virtual int				CastTreasureEventAddItem(GameLogic::LC_Attacker* pkScriptCaller, int eventIndex);
	virtual int				CastTreasureEventSwapnNPC(GameLogic::LC_Attacker* pkScriptCaller, int eventIndex);
	virtual int				CastTreasureEventTransferMap(GameLogic::LC_Attacker* pkScriptCaller, int eventIndex);

	virtual int				CostItemByID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, object_id_type lSrcID, object_id_type lLogicID, int lItemID, int iItemCount, int iAction, int lPara0, int lPara1, int lPara2, int lPara3, int lPara4);
	virtual int				CostItemByIDAddSkillState(GameLogic::LC_Attacker* pkScriptCaller, int iCostTargetIndex, int iAddStateTargetIndex, object_id_type lSrcID, object_id_type lLogicID, int lItemID, int iItemCount, int lSkillStateID, int lStateTime, float fPara0, float fPara1, float fPara2, float fPara3, float fPara4);
	virtual int				CostItemByIDAddSkillStateToController(GameLogic::LC_Attacker* pkScriptCaller, int iCostTargetIndex, int iAddStateTargetIndex, object_id_type lSrcID, object_id_type lLogicID, int lItemID, int iItemCount, int lSkillStateID, int lStateTime, float fPara0, float fPara1, float fPara2, float fPara3, float fPara4);


	//��������״̬
	virtual void			SetTaskFinishState(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iIndex, int lState);

	//��������׷������
	virtual void			SetTaskTraceIndex(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iIndex);

	//��ʾ��Ϸ�����Ϣ
	virtual void			ShowStoryMessage(int lMsgID, const StringType& rkMsgKey, int iTargetType, int iTargetID, int iShowType, int lParam);

	//��ʾ��Ϸ�����Ϣ
	virtual void			ShowStoryMessageByString(int lMsgID, const StringType& rkMsgKey, int iTargetType, int iTargetID, int iShowType, int lParam, const StringType& strParam);

	//����ְҵ
	virtual void			SetCareerType(GameLogic::LC_ActorBase* pkActor, int iCareerType);

	//virtual void			NotifyCareerSlectionSucceed(GameLogic::LC_ActorBase* pkMachineOwner,bool flag);

	//����ӿ� Begin

	//����ŭ��ֵ
	virtual void			CostTargetBerserk(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue);
	//�ظ�Ŀ��ŭ��
	virtual void			RecoverTargetBerserk(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int lRecoverValue);
	//���ûظ�ŭ����ʱ����
	virtual void			SetBerserkTimer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fTimer);
	//�ع�ŭ����ʱ����
	virtual void			ResetBerserkTimer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//����ɱ�ֻظ�ŭ���ĸ���
	virtual void			SetBerserkProbability(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fProbability);
	//�ع�ɱ�ֻظ�ŭ���ĸ���
	virtual	void			ResetBerserkProbability(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//����ŭ�����ֵ
	virtual void			SetMaxBerserkPoint(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iMaxBerserk);

	//����ӿ� End

	//��Ӫ�ӿ� Begin
	//����npc��Ӫ
	virtual void			SetFactionID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iFactionID);

	//��Ӫ�ӿ� End


	//AI�ӿ�Begin

	virtual bool			ChangeActorAIFunc(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iAIFuncID);

	//�������ⷽʽ���������
	//��޶���Ĭ�ϰ��ճ��ֵ�ɴ�С���򣬻�ȡָ��λ�õĳ��ֵ
	virtual int				GetSortHatredByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred);
	//��ȡ��޶�����ָ��λ��Ŀ��ĳ��ֵ
	virtual attr_value_type	GetHatredValueByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred);
	//��ȡ��޶�����ָ��ID�ĳ��ֵ
	virtual attr_value_type	GetHatredValueByID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID);
	//��ȡ��޵ĸ���
	virtual int				GetHatredCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//���ó�޶�����ָ��λ��Ŀ��ĳ��ֵ
	virtual void			SetHatredValueByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, int lHatredValue, bool bExcludeCurrentHatred);
	//���ó�޶�����ָ��IDĿ��ĳ��ֵ
	virtual void			SetHatredValueByID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lHatredValue);
	//��ӳ��Ŀ��
	virtual void			AddHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lHatredValue);
	//ɾ����޶�����ָ��λ�õ�Ŀ��
	virtual void			RemoveHatredByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred);
	//ɾ����޶�����ָ��ID��Ŀ��
	virtual void			RemoveHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID);
	//��ճ��
	virtual void			ClearHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//�ڼ��ܶ��������Ŀ��ͼ���
	virtual void			AddHatredSkill(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lSkillID, int iPriority, float fSkillLifeTime);
	//��ռ��ܶ���
	virtual void			ClearHatredSkillList(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//�����������-��¡���ٻ���������ù������Ե�
	virtual void			ProcessCloneMonsterAfterSpawn(GameLogic::LC_Attacker* pkScriptCaller, int iSourceIndex, int iTargetIndex);

	//��ȡ�ض����Ե�Ŀ��
	virtual object_id_type	SelectLockedTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//��npcָ��һ��Ŀ�꣬�ڴ��ڼ��޶��в�������
	virtual void			AddAdditionHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, float fDurationTime);

	//���������Ŀ��
	virtual void			ClearAdditionHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//AI�ӿ�End

	//������븱��
	virtual bool			ReqEnterTranscription(GameLogic::LC_Attacker* pkScriptCaller, int iTranscriptionType);

	//�����˳�����
	virtual bool			ReqQuitTranscription(GameLogic::LC_Attacker* pkScriptCaller, int32_t iDelay/*=0����*/);

	virtual void			AddGuildPresent(GameLogic::LC_Attacker* pkScriptCaller, int32_t itemID, int32_t way, int32_t count);

	//���ϵͳ�ƺŽӿ�
	virtual void			AddSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID);
	//����
	virtual void			UseItemAddlevel(GameLogic::LC_ActorBase* pkScriptCaller, int ilevel);
	//��������
	virtual int				CheckUseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID);
	virtual int				UseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID);
	//�ӽ�������
	virtual int				UseItemAddSubordinateAttr(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iAttributeType, int32_t iVal, int32_t iPercent);
	virtual int				UseItemCommitAddSubordinateAttr(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t uiValCount, int32_t uiPerCount);
	//�ӽ���ף����
	virtual int				UseItemAddSubordinateBlessPoint(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iVal, int32_t iPercent);
	virtual	int				CheckUseItemOpenSubordinate(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iLevel);
	virtual	int				UseItemOpenSubordinate(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iLevel);
	//�����ߵȼ�������
	virtual int				UseItemAddSubLevel(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iItemID, int32_t iSubType,
											   int32_t iLevelLimit, int32_t iLevel, const StringType& sRewards);
	virtual	int				UseItemAddSubLevelTo(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iItemID, int32_t iSubType,
												 int32_t iLevelLimit, const StringType& sRewards);

	virtual void			UseItemAddActScore(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iType, int32_t iVal, const StringType sCatagory);
	virtual	void			UseItemAddSnowManScore(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iScore, int32_t iAchieve, const StringType sCatagory);

	virtual ResultType		AddGemReinforceExp(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iType, int32_t iExp);

	//�Ƴ�ϵͳ�ƺŽӿ�
	virtual void			RemoveSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID);

	//����ϵͳ�ƺŽӿ�
	virtual void			SetSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID);

	//��Ŀ���е
	virtual bool			DisarmTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iRandomIndex, int lUnDisarmStateID, float fDisarmDuration);

	//���Ŀ��Ľ�е״̬
	virtual void			UnDisarmTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//֪ͨ�����ͷ��¼�
	virtual void			NoticeSkillExecuteEvent(object_id_type lPlayerID, int lSkillID, object_id_type lTargetID);

	//֪ͨ��ʼ����NPC�¼�
	virtual void			NoticeConvoyNPCEvent(object_id_type lPlayerID, object_id_type lNPCID, int lTaskID);

	//֪ͨ������Ϸ�¼�
	virtual void NoticeTaskGameEvent(object_id_type lPlayerID, int lTaskID, int lTaskEventType, int lParam1, int lParam2);

	//��Ŀ�����Ӿ���
	virtual void			AddExp(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t iExp);

	//�������Ӿ���
	virtual void			RecoverExp(GameLogic::LC_Attacker* pkScriptCaller, float multiply);

	//�ظ�Ŀ������
	virtual void			RecoverSkillPoint(GameLogic::LC_Attacker* pkScriptCaller, float multiply);

	//��������ӳɣ���ɫ��Ч
	virtual int				GetSitRecoverExpIncrement(GameLogic::LC_Attacker* pkScriptCaller, float multiply);


	//��Ŀ��������������ֵ��
	virtual void			AddTargetSkillPoint(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, attr_value_type iSkillPoint);
	//��Ŀ�����ӵȼ��������
	virtual void			AddTargetSkillPointLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fParam1);

	//��Ŀ�����ӵȼ���صľ���
	virtual void			AddTargetCashLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fParam1);

	virtual void			AddCash(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, MoneyType iCash, int iCashType, int iSrcType, int iSrcID);

	//��Ŀ����ϵȼ���ؾ���(Ŀǰ���׽�����)
	virtual void			AddTargetExpRelatedLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float param1);

	//Ŀ�����ָ����SkillFlag������Ŀ�꼼��
	virtual void			CheckSkillFlagAndInterrupt(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iSkillFlagIndex, object_id_type lLogicID);

	//��ȡ��ǰϵͳʱ��
	virtual int				GetCurrentSystemTime();

	//��Ӵ���
	virtual	void			GroupTranstfer(GameLogic::LC_Attacker* pkScriptCaller, int iResMapID, int lX, int lY);

	//��ȡ�����Ŀ��ID
	virtual void			AutoFitLevelReInit(GameLogic::LC_Attacker* pkScriptCaller, int iLevel);

	//�����Ϊ����
	virtual void			AddActionForbid(GameLogic::LC_Attacker* pkScriptCaller, int iForbidType, int lParam1 = 0, int lParam2 = 0);

	//�Ƴ���Ϊ����
	virtual void			RemoveActionForbid(GameLogic::LC_Attacker* pkScriptCaller, int iForbidType, int lParam1 = 0, int lParam2 = 0);

	//��������ʱ��
	virtual void			SetSurvivalTime(GameLogic::LC_Attacker* pkScriptCaller, int iSurvivalTime);
	virtual void			SetTargetSurvivalTime(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iSurvivalTime);
	virtual int				IsSuvivalDead(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//��ȡAI����ֵ
	virtual int				GetAIFuncValue(GameLogic::LC_Attacker* pkScriptCaller, int iAIFuncType);

	//���ñ���
	virtual void			SetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller, int lCharTypeID);

	//��ȡ������Ϣ
	virtual int				GetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller);

	virtual void			AddTotalHookTime(GameLogic::LC_Attacker* pkScriptCaller, int addHookTime);
	virtual void			NotifyHookInfoUpdate(GameLogic::LC_Attacker* pkScriptCaller);

	//NPC��ӹ���
	virtual void			AddOwnerID(GameLogic::LC_Attacker* pkScriptCaller, object_id_type lOwnerID, bool bConsiderGroupMember);

	//NPC�̶�����
	virtual bool			SetNPCFixedOwnerListFlag(GameLogic::LC_Attacker* pkScriptCaller, bool bFlag);

	////ʹ����Ʒ�����츳��
	//virtual bool			AddTalentPointByItem(GameLogic::LC_Attacker* pkScriptCaller, int lItemID);
	////����츳
	//virtual bool			ClearAllTalentByItem(GameLogic::LC_Attacker* pkScriptCaller, int lItemID);

	//���ᶯ��
	virtual void			FreezeAnimation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//�ⶳ����
	virtual void			UnFreezeAnimation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//Notify NPC Execute Skill
	virtual void			NPCExecuteSkill(GameLogic::LC_ActorBase* pkActor, int lSkillID);

	virtual void			NPCExecuteSkillEx(GameLogic::LC_ActorBase* pkActor, int lSkillID, object_id_type lTargetLocked);

	virtual void			ServerPlayerExecuteSkill(GameLogic::LC_ActorBase* pkActor, int lSkillID);

	virtual void			RltPlayerExecutePairSit(GameLogic::LC_ActorBase* pkActor);


	virtual	void			ShowRichUI(GameLogic::LC_Attacker* pkScriptCaller, int iTargetType, int iRichUIID, int bShow);

	//�������5������Buff���
	virtual void			SetCourage(GameLogic::LC_Attacker* pkCaller , bool Value);

	//��ȡNPC�������Ա����
	virtual int				GetGroupNpcSize(GameLogic::LC_Attacker* pkCaller);

	//����������ȡ���ڳ�ԱID
	virtual object_id_type	GetGroupNpcIDByIndex(GameLogic::LC_Attacker* pkCaller, int index);

	//���ݰ뾶�����λ
	virtual bool			SetNextPosInCircle(GameLogic::LC_Attacker* pkCaller, int iTargetIndex,/*int posx,int posy,*/ int innerRadius, int outRadius, float step, bool clockwise);

	//˲��NPC����������Ҫ����Ŀ�����
	virtual void			TransferToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius);
	virtual void			TransferBackToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius);
	//˲�����
	virtual void			TransferTargetToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex0, int iTargetIndex1, bool bUseSafeRadius);
	//˲������
	virtual void			TransferToXYZ(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int x, int y, int z);
	//virtual void			SetPostTransferLocation(GameLogic::LC_Attacker* pkScriptCaller);

	//����Ŀ��
	virtual void			PunchBackTarget(GameLogic::LC_ActorBase* pkActor, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime);
	virtual void			PunchBackTargetToPoint(GameLogic::LC_ActorBase* pkActor, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime);
	virtual void			PunchBackTargetBySide(GameLogic::LC_ActorBase* pkActor, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime);

	//�û��˴���ǿ��λ��
	virtual void			DoTransferToPos(GameLogic::LC_ActorBase* pkActor, int iTargetIndex, int iTransferTarget, float fPunchTime);
	virtual void			DoTransferToCircle(GameLogic::LC_ActorBase* pkActor, int iTargetIndex, int iTransferTarget, float fPunchTime, int dis = 0);

	virtual void			UpdateAIcontrolPos(GameLogic::LC_ActorBase* pkCaller);
	virtual void			UpdateAIcontrolDir(GameLogic::LC_ActorBase* pkCaller);
	virtual bool			CheckGuildAssert(GameLogic::LC_ActorBase* pkCaller, int asserttype, int assertcount);
	virtual bool			CheckGivenPower(GameLogic::LC_ActorBase* pkCaller);
	virtual void			GivenPowerGuild(GameLogic::LC_ActorBase* pkCaller);
	virtual int				CheckControlCatapult(GameLogic::LC_ActorBase* pkCaller);
	virtual void			ControlCatapult(GameLogic::LC_ActorBase* pkCaller);
	virtual bool			CheckGuildLadderOccupation(GameLogic::LC_ActorBase* pkCaller);
	virtual void			GuildLadderOccupation(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex);
	virtual	int				GetGuildLadderFaction(GameLogic::LC_ActorBase* pkCaller);

	virtual void			SetTargetPlayerAIControl(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex, bool bFlag);

	//��ͻ��˵���ҷ���һ���Լ�ƴ���ַ�����Ϣ���ͻ��˻����msgType������Ӧ����ʾ
	virtual void			SendNotifyStrToClientPlayer(GameLogic::LC_ActorBase* pkCaller, const StringType& str, int msgType, int level);
	//��ͻ��˵���ҷ���һ��SystemError.csv�ж�ӦID���ַ�����Ϣ���ͻ��˻����msgType������Ӧ����ʾ
	virtual void			SendNotifyErrorIDStrToClientPlayer(GameLogic::LC_ActorBase* pkCaller, int systemErrorID, int level);

	//����ս�Զ���þ���ͳ�ƽӿ�
	virtual void			AddGuildWarAutoExp(GameLogic::LC_ActorBase* pkCaller, attr_value_type iExpValue);

	void					GuildWarCapapultMsg(GameLogic::LC_ActorBase* pkCaller, GameLogic::LC_ActorBase* pkTarget, int damage);
	void					GuildWarCapapultAllMsg(GameLogic::LC_ActorBase* pkCaller, int count, int damage);

	//����Ŀ��attributemap��ĳ������
	virtual bool			LockAttribute(GameLogic::LC_ActorBase* pkCaller, int iAttributeType, bool bLock, int iValue);

	//�ж��ڳ���ΧԲ��������û�ж�Ӧ�Ľ�����
	virtual int				LockRobberInCircle(GameLogic::LC_Attacker* pkScriptCaller, int iSrcIndex, int lActorFilterFuncID, bool bExclusive, 
								int iCircleRadius, int iMaxLockCount, int iRandomIndex, object_id_type lFileterPlayerID);
	//���÷�����MapLogic_place���йֽӿ�ˢ��
	virtual object_id_type	MapLogicSpawnNPC(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex, int minRadius, int maxRadius, int lCharTypeID);
	//���÷�����MapLogic_place���йֽӿ�ˢ��
	virtual object_id_type	MapLogicSpawnNPCXYZ(GameLogic::LC_ActorBase* pkCaller, int x, int y, int z, int lCharTypeID);
	//���÷�����MapLogic_place���йֽӿ�ˢ��Ӣ��
	virtual object_id_type	MapLogicSpawnEliteNPC(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex, int minRadius, int maxRadius, int lCharTypeID, int rule1, int rule2, int rule3);
	//���ù���������ʱ��
	virtual void			SetNPCSurvivalTime(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex, float dTime);

	//�õ���ǰ��ͼ���Լ�SummorID�Ĺ���
	virtual int				GetCurrentMapSummorCount(GameLogic::LC_ActorBase* pkCaller);

	//��ḱ��Boss����ʱ֪ͨ(�㲥)
	virtual void			NotifyTranscriptionData(GameLogic::LC_ActorBase* pkCaller, int kType, int param1, int param2);

	//��ḱ��Boss����ʱ֪ͨ(����)
	virtual void			NotifySingleTranscriptionData(GameLogic::LC_ActorBase* pkCaller, GameLogic::LC_ActorBase* pkPlayer, int kType, int param1, int param2);

	//�������
	virtual void			DismissTargetVehicle(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex);

	virtual void			NotifyClientShowTips(const StringType& strContent, uint16_t nFlag = MG_SERVER_ROUTER_FLAG_SINGLE);
	virtual void			NotifyClientShowTipsInMap(const StringType& strContent, int32_t iMapLogicID, int32_t iMapResID);
	virtual void			NotifyClientShowTipsInGuild(uint64_t iGuildID, int iType, const StringType& strContent);

	virtual void			GodKill(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetType, int lSkillTypeID);

	virtual void			AddUnbindYuanbao(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetType, MoneyType lYuanbao, int iSrcType, const StringType& orderid);
	virtual void			AddYuanZhengPoint(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, MoneyType iCash, int iCashType, int iSrcType, int iSrcID);

	virtual int				ActiveSubTransform(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetType, int transFormID, int setAddExpFlag);
	virtual int				ActiveBuddyTransform(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetType, int transFormID, int buddyID);

	virtual bool			OpenFDeposit(GameLogic::LC_ActorBase* pkScriptCaller);
	virtual bool			ItemUseAddBossDropTimes(GameLogic::LC_ActorBase* pkScriptCaller, int type, int times, int consumeCount);
	virtual bool			ItemUseActivateCommanderTitleID(GameLogic::LC_ActorBase* pkScriptCaller, int id);

	virtual void			AddOssRecharge(unique_id_type charID, int lYuanbao, int iSrcType, const StringType& orderid);

	//�õ���ǰ�����ٻ�������ٻ���Ŀ
	virtual int				GetSummorIDCount(GameLogic::LC_ActorBase* pkCaller);
	//�õ�������ǰ�����ٻ����CharTypeID
	virtual object_id_type	GetSummorIDByIndex(GameLogic::LC_ActorBase* pkCaller, int iIndex);

	//�õ���������еĽ�����Ŀ
	virtual int				GetEscortStealCount(GameLogic::LC_ActorBase* pkCaller);

	//����������ϵ�Attacker
	virtual void			SetNPCAttacker(GameLogic::LC_ActorBase* pkCaller, int attackerID);

	virtual void			CopyHitedInfo(GameLogic::LC_Attacker* pkCaller, GameLogic::LC_Attacker* dstActor);

	virtual void			ClearHitedInfo(GameLogic::LC_ActorBase* pkCaller);

	//transferID ��Ӧ	TransferArea.csv���е�Index
	virtual void			CreatePortal(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex, int iMapResID, int doorCharTypeID, int transferID);
	virtual void			CreatePortalXYZ(GameLogic::LC_ActorBase* pkCaller, int x, int y, int z, int iMapResID, int doorCharTypeID, int transferID);

	virtual int				GetWorldLevel(int32_t iCitizenship);
	virtual int				GetGroupWorldLevel();

	virtual void			SetDecompositionItem(GameLogic::LC_ActorBase* pkScriptCaller, bool flag);
	virtual void			VipEnjoy(GameLogic::LC_ActorBase* pkScriptCaller, int kType, bool flag);
	
	virtual int64_t			RecalculateCombatScore(GameLogic::LC_ActorBase* pkScriptCaller, int kType,int64_t oldScore, bool flag);

	ResultType				DoConsumeItems(GameLogic::LC_PlayerBase* pkPlayer, const StringType& items, int32_t reason);
	ResultType				DoPayShops(GameLogic::LC_PlayerBase* pkPlayer, const StringType& shops, int32_t srctype, int32_t srcid);

	void					SetMapLockEnter(int32_t iMapLogicID, int32_t iResID, bool flag);
	void					ClearStoryBatchSchedule(int32_t iStoryID, int32_t iKey);

	void					SetWatchTowerState(int storyID, int32_t towerMapID, int32_t bossMapID, int towerID, int bossID, int state, int vaildTime, int endTimerID);
	void					SetTowerLocation(int storyID, int32_t mapID, int32_t towerID, int x, int y, int z);
	
	bool					CurrentMapIsRecorderPkValue(GameLogic::LC_Attacker* pkCaller);
	void					SendKingFlagGainInfo(int nServerID, uint64_t nTime);
	void					SendFactionResult(int32_t iStatus);
	void					AddCouragePoint(GameLogic::LC_ActorBase* pkCaller, attr_value_type iPoint);

	void					AddBreakRewardItem(GameLogic::LC_ActorBase* pkCaller, int iItemID, int iStateID, float per, int iMax, int dis);
	double					GetBossShieldPercent();
	double					GetCrossBossShieldPercent();

	void					ActivateFashion(GameLogic::LC_Attacker* pkCaller, int lID);

	void					SetShieldBreak(GameLogic::LC_ActorBase* pkCaller);

	void					DelaySkillEvent(GameLogic::LC_ActorBase* pkCaller, int skillID, int duration, int startTime, int perTime);

	virtual void			AddPlayerBoneSoul(GameLogic::LC_ActorBase* caller, int skillID);
	virtual int				GetPlayerBoneSoul(GameLogic::LC_ActorBase* caller,int iIndex);
	virtual void			RemovePlayerBoneSoul(GameLogic::LC_ActorBase* caller, int skillID);

	int32_t					ChooseFactionByGlobalPower(GameLogic::LC_ActorBase* pkCaller);
	int32_t					GetFactionDefServer();
	int32_t					GetFactionFailBuffOverlap(GameLogic::LC_ActorBase* pkCaller);
	int32_t					AddRelateTimeBuff(GameLogic::LC_ActorBase* pkCaller, int32_t iStoryID, int32_t iBuffID);

	void					AddGSFactionScore(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured, int32_t iType, int32_t iVal);
	void					AddGSActorScore(GameLogic::LC_ActorBase* pkKiller, GameLogic::LC_ActorBase* pkInjured, int32_t iVal);

	void					GFxWriteGAMELog(const char * str);
	bool					NotifyClientSete_Damage(uint32_t SrcActorID,uint32_t DstActorID,uint32_t LogicID,uint32_t lSkillDamageType,uint32_t lDamageResult,uint32_t iFinalDamage,uint32_t MapResID,uint32_t MapLogicID);
private:
	void					_handleCostEvent(object_id_type lSrcActorID, object_id_type lDstActorID, object_id_type lLogicID, Skill::SK_CostEvent::SK_CostType eCostType, int iCostValue, int lItemID, int iItemCount);
	void					_handleDetachStateEvent(object_id_type lSrcActorID, object_id_type lDstActorID, object_id_type lLogicID, int lStateType, int iDetachCount);

	void					_implShotBullet(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, ShotBulletArg & arg);
	void					_implShotBulletToTarget(GameLogic::LC_ActorBase* pkAttacker, object_id_type lTargetID, const Utility::UT_Vec3Int& rkTargetLocation, const ShotBulletArg & arg);
	void					_implshotBulletToLocation(GameLogic::LC_ActorBase* pkAttacker, const Utility::UT_Vec3Int& rkTargetLocation, const ShotBulletArg & arg);

	void					_implShotMultiBullet(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, mem::vector<ShotBulletArg>& argVec);
	void					_implShotMultiBulletToTarget(GameLogic::LC_ActorBase* pkAttacker, object_id_type lTargetID, const mem::vector<Utility::UT_Vec3Int>& rkTargetLocation, const mem::vector<ShotBulletArg>& argVec);
	void					_implShotMultiBulletToLocation(GameLogic::LC_ActorBase* pkAttacker, const mem::vector<Utility::UT_Vec3Int>& rkTargetLocation, const mem::vector<ShotBulletArg>& argVec);

	//������=������������-�Է���ܣ�/�Է��ȼ�+�����ʸ��ӣ�
	bool					_judgeNormalAttack(attr_value_type iDog, attr_value_type iDogRate, int64_t iAttackerLevel, int64_t iTargetLevel, attr_value_type iAttackRate, double fRandom);
	double					_compureAttackRate(attr_value_type iDog, attr_value_type iDogRate, int64_t iAttackerLevel, int64_t iTargetLevel, attr_value_type iAttackRate);

	//������=����������-�Է����ԣ�/�Է��ȼ�+�����ʸ��ӣ�%
	bool					_judgeCriticalAttack(attr_value_type iCritical, attr_value_type iCriticalRate, attr_value_type iCriticalDef, int64_t iAttackerLevel, int64_t iTargetLevel, double fCriticalRateAdd, double fRandom, LC_ActorBase* pSrc, LC_ActorBase* pDst);
	double					_computeCriticalRate(attr_value_type iCritical, attr_value_type iCriticalRate, attr_value_type iCriticalDef, int64_t iAttackerLevel, int64_t iTargetLevel, double fCriticalRateAdd, LC_ActorBase* pSrc, LC_ActorBase* pDst);

	void					_generateSkillDamageMultiple( GameLogic::LC_Attacker* pkScriptCaller ,  int iDamageTargetIndex, int lSkillTypeID,  int iDamageRandomIndex , int iAttackRateRandomIndex, int iCriticalRateRandomIndex,  object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent, int bUseScore, bool bDoPunch = true);
	//�����˺�=�����˺���-�Է���������*����Ƶ��+ ��Ϣ����*�˺�ϵ��-�Է���������
	/*
		�µļ��㹫ʽ��

		AΪ��������BΪ������

		�����˺�={����ϵ��*max��(A����-B����)��A����*0.35��+�����˺��̶�ֵ}*(1+A�˺��ӳ�-B�˺�����)+ս���˺�*����ϵ��

		ս���˺�= power��Bս��/Aս����0.5��/100*B�������ֵ
	*/
	double					_computeDamageBase(double iDamage, attr_value_type iDef, int32_t iMPCost, double fDamageRate, double fOverAttackRate, double appendRate);
	//�ٷֱ�����
	double					_computeMinAck(object_id_type lTargetID, object_id_type lAttackerID);

	void					_doDmagePunch( int64_t iFinalDamage, LC_ActorBase* pkTarget, CF_SkillExecuteList::DataEntry* pkSkillData, object_id_type lTargetID, LC_ActorBase* pkAttacker );
	//ս���˺�= (power��Bս��/Aս����0.5��/100*B�������ֵ)
	int64_t					_computeScoreDamgeBase(uint64_t iAScore, uint64_t iBScore, attr_value_type bMaxHP, double skillaset);
	//�ӵ����ߵ�Ŀ��������ҵ�ָ����Ŀ��
	virtual void	_findTarget(GameLogic::LC_Attacker* pkCaller, int iTargetIndex, GameLogic::LC_ActorPtrList& kResult);

	//�ӵ����ߵ�Ŀ��������ҵ�ָ����Ŀ��
	virtual double	GetBonFirPartyMapBuffStateMutableParameter(GameLogic::LC_Attacker* pkCaller);
};
#endif

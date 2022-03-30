//服务器技能脚本接口
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

	//设置鼠标点为目标点, bLockedTarget是否考虑当前锁定目标
	virtual void				SetLockedLocationByMouseLocation(GameLogic::LC_Attacker* pkScriptCaller, bool bLockedTarget);

	// CombatEvent
	virtual void RegisterCombatEventData(GameLogic::LC_Attacker* pkScriptCaller,  int eEventType, int nKey, int nValue );
	virtual void UnregisterCombatEventData(GameLogic::LC_Attacker* pkScriptCaller,  int eEventType, int nKey );

	virtual void RegisterCombatEventPassiveCrit(GameLogic::LC_Attacker* pkScriptCaller,  GameLogic::LC_Attacker* pkAttacker, int nValue );
	virtual void UnregisterCombatEventPassiveCrit(GameLogic::LC_Attacker* pkScriptCaller,  GameLogic::LC_Attacker* pkAttacker );

	//trick 直接传三个值进去走，没办法了
	virtual void			SetTargetLocationValue(GameLogic::LC_Attacker* pkScriptCaller, int posx, int posy, int posz);

	virtual bool          ModifyVIPGrowthValue(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int64_t iDeltaValue);
	virtual void  		   ModifyTargetAttributeByValue(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iAttributeType, int64_t iDeltaValue, bool bRevert);
	virtual void		   ModifyTargetAttributeByPercent(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int iAttributeType,double fDeltaPercent,bool bRevert);
	//设置玩家骑乘类型
	virtual void			SetTargetVehicleType(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lVehicleType);

	virtual void			NotifyCastSkillToAchievement(GameLogic::LC_Attacker* pkScriptCaller, int achievementType);

	virtual void			NotifyEventToGoal(GameLogic::LC_Attacker* pkScriptCaller, int goalType, int param);

	virtual void			NotifyCastSkillToGatherTaskNpc(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//获取和修改技能事件参数接口
	//virtual object_id_type GetEventSrcID(Skill::SK_SkillEvent* pkEvent);
	//virtual object_id_type GetEventDstID(Skill::SK_SkillEvent* pkEvent);
	//virtual object_id_type GetEventLogicID(Skill::SK_SkillEvent* pkEvent);
	//virtual int GetEventParam(Skill::SK_SkillEvent* pkEvent, int iParamIndex);
	virtual void			SetEventParam(Skill::SK_SkillEvent* pkEvent, int iParamIndex, int64_t lParam);


	//向目标周围发散子弹
	/*
	　	   iTargetIndex :	    目标说明
	   lMinRadius :			最近半径
	   lMaxRadius :			最远半径
	   lSkillTypeID :		子弹产生的技能
	   iBulletFuncID :		子弹功能ID
	   iPenetrateCount :	子弹穿透次数
	   iDummyType :			特效产生的部位
	*/
	virtual void			ShotBulletAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lSkillTypeID, int iBulletFuncID, int iPenetrateCount, int iDummyType);
	//向目标发散子弹
	/*
	　 iTargetIndex :	    目标说明
	   lSkillTypeID :		子弹产生的技能
	   iBulletFuncID :		子弹功能ID
	   iPenetrateCount :	子弹穿透次数
	   iDummyType :			特效产生的部位
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
	//向目标发射暗器
	/*
	　 iTargetIndex :	    目标说明
	   lSkillTypeID :		子弹产生的技能
	   iBulletFuncID :		子弹功能ID
	   iPenetrateCount :	子弹穿透次数
	   iDummyType :			特效产生的部位
	   iMoveType :			子弹类型（BT_SHOOT、BT_CHAIN、BT_CAST、BT_MISSILE）
	   fMoveSpeed :			1、如果仰角不为0，则填飞行时间；2、否则填子弹速度
	   fPitchAngle :		俯仰角（只有BT_CAST、BT_MISSILE子弹才有效）
	   fRollAngle :			侧倾角（只有BT_MISSILE子弹才有效）
	   fTurningPercentage :	拐点在直线上的百分比位置（只有BT_MISSILE子弹才有效）
	*/
	struct ShotBulletArg
	{
		Skill::SK_SkillExecute	*	pkSkillExecute;
		const CSVFile::CF_SkillBulletFunc::DataEntry *	pkBullet;	// SkillBulletFunc.csv 配置
		int				lSkillTypeID;		// 子弹所属技能
		int				iBulletFuncID;		// 技能中的子弹索引
		int				iPenetrateCount;	// 穿刺次数
		int				iDummyType;			// 发射dummy

		float			fStartTime;			// 发射时间
		float			fMoveTime;
		UT_Vec3Int		kShotLocation;		// 发射位置
		UT_Vec3Int		kDestLocation;		// 目标位置
		bool			bClientBullet;		// 客户端子弹
		float			fPitchAngle;		// 发射仰角

		// volatile，每个子弹不一样
		UT_Vec3Float	kShotDir;			// 当前子弹的射击方向
		int				iClientLocalID;		// 当前子弹的ClientLocalID
		float			fCollidePosParamT;	// 当前子弹的碰撞参数值，直线型，服务器子弹默认1.0001f，说明需要做碰撞检测
		int				iCollideSurfaceMaterial;	// 碰撞体表面材质索引

		ShotBulletArg() { ::memset(this, 0, sizeof(ShotBulletArg));}
	};

	//计算技能伤害
	/*
	iDamageTargetIndex :	伤害目标
	lSkillTypeID :			技能类型
	iDamageRandomIndex :	伤害力骰子
	iAttackRateRandomIndex :命中骰子
	iCriticalRateRandomIndex :暴击骰子
	lDamageSourceActorID :	发动技能者的ID
	iHPCost				 :	本次技能的HP消耗，用于计算伤害，该参数暂时无用
	iMPCost				 :	本次技能的MP消耗，用于计算伤害，如果传入0，函数内自动计算技能MP消耗（默认MP消耗和额外消耗）
	*/
	virtual void			GenerateSkillDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost);

	//计算技能伤害
	/*
	iDamageTargetIndex :	伤害目标
	lSkillTypeID :			技能类型
	iDamageRandomIndex :	伤害力骰子
	iAttackRateRandomIndex :命中骰子
	iCriticalRateRandomIndex :暴击骰子
	lDamageSourceActorID :	发动技能者的ID
	iHPCost				 :	本次技能的HP消耗，用于计算伤害，该参数暂时无用
	iMPCost				 :	本次技能的MP消耗，用于计算伤害，如果传入0，函数内自动计算技能MP消耗（默认MP消耗和额外消耗）
	*/
	virtual void			GenerateSkillDamageMainTarget(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int iPercent);

	virtual void			GenerateSkillDamageMainTargetMulTiple(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple , int iPercent);
	//可以按照倍数来造成技能伤害
	//
	virtual void			GenerateSkillDamageMultiple(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent);

	virtual void			GenerateSkillDamageMultipleWithPunchFlag(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent, bool bDoPunch);

	//计算技能伤害
	virtual void			GenerateSkillDamageNoScore(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex,	int lSkillTypeID, int iDamageRandomIndex, int iAttackRateRandomIndex, int iCriticalRateRandomIndex, object_id_type lDamageSourceActorID, int iHPCost, int iMPCost);

	//计算绝对伤害
	/*
	iDamageTargetIndex :	目标说明
	lDamageLogicTypeID :	产生伤害的技能编号
	lDamageType :			伤害的类型
	iDamage :				伤害点数
	lDamageSourceActorID :	发起伤害者的编号
	*/
	virtual void			GenerateDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID);

	//计算绝对伤害(这个伤害计算攻击者的ATT_DMG_RATE属性)
	/*
	iDamageTargetIndex :	目标说明
	lDamageLogicTypeID :	产生伤害的技能编号
	lDamageType :			伤害的类型
	iDamage :				伤害点数
	lDamageSourceActorID :	发起伤害者的编号
	*/
	virtual void			GenerateDamage2(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID);

	//计算精英怪的技能伤害(用特殊公式计算带防御力
	/*
		iDamageTargetIndex :	目标说明
		lDamageLogicTypeID :	产生伤害的技能编号
		lDamageType :			伤害的类型
		iDamage :				伤害点数
		lDamageSourceActorID :	发起伤害者的编号
	*/
	virtual void			GenerateEliteDamage(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, attr_value_type iDamage, object_id_type lDamageSourceActorID);
	virtual void			DetachStateAndGenerateDamage(GameLogic::LC_Attacker* pActor, int targetIndex, int sourceIndex, int stateID, int skillID, int damage, int damageType, bool pure);

	//按照HP比率造成伤害
	virtual void			GenerateDamageByHPPercent(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lDamageLogicTypeID, int lDamageType, int iHPType, double fHPPercent, object_id_type lDamageSourceActorID);
	//按照属性比率造成伤害
	virtual void			GenerateDamageByAttributePercent(GameLogic::LC_Attacker* pCaller, int iTargetIndex, int iSkillID, int iDamageType, int iAttributeType, float fAttrPercent, int iSourcerIndex, bool bCountDef);
	//绝对值方式消耗目标的HP
	virtual void			CostTargetAbsHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue);
	//百分比方式消耗目标的基本HP（等级对应的最大HP）
	virtual void			CostTargetDefaultPercentHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent);
	//百分比方式消耗目标的额外HP（包括装备后的最大HP）
	virtual void			CostTargetAdditionPercentHP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent);
	//绝对值方式消耗目标的MP
	virtual void			CostTargetAbsMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue);
	//百分比方式消耗目标的基本MP（等级对应的最大MP）
	virtual void			CostTargetDefaultPercentMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent);
	//百分比方式消耗目标的额外MP（包括装备后的最大MP）
	virtual void			CostTargetAdditionPercentMP(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, float fCostPercent);

	//消耗技能资源，引导型技能的HP,MP在脚本中扣除
	virtual void			CostTargetSkillResource(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSkillTypeID);

	//激活技能
	virtual int			ActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int nBuddyId, int lSkillTypeID);

	//获取技能激活状态
	virtual int			CheckActivateSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID);

	//学习技能
	virtual void			LearnSkill(GameLogic::LC_Attacker* pkScriptCaller, int lSkillTypeID);

	//设置技能可用次数
	virtual void			SetSkillUsableCount(GameLogic::LC_Attacker* pAttacker,int targetIndex, int skillID, int usableCount);

	//学习配方
	virtual void			LearnFormula(GameLogic::LC_Attacker* pkScriptCaller, int lFoumulaID);

	//客户端天眼执行技能
	virtual void			NotifyClientEyeSkill(GameLogic::LC_Attacker* pkScriptCaller, int skillID);

	//给目标绑定一个特效
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

	//给目标取消一个特效
	virtual void			DetachTargetGfxServer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDummyType, const StringType& strFxFileName, int broadCastType);

	//给目标绑定声音
	virtual void			AttachTargetSound(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, const StringType& kFileName, object_id_type lSrcActorID, object_id_type lLogicID, 
										int iSoundChannel, int iSoundType, int iBroadCastType);

	//复活目标
	virtual void			RespawnTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t iHP, double fHPPercent, int64_t iMP, double fMPPercent);


	//判断攻击是否命中
	virtual bool			IsAttackHit(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iAttackRateRandomIndex);

	//提前计算是否暴击 普通攻击技能使用
	virtual bool			IsAttackCritical(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iAttackRateRandomIndex, int iCriticalRateRandomIndex);

	//根据双方数值,计算基本伤害值
	/*
		iDamageTargetIndex :	目标说明
		lSkillTypeID :			产生伤害的技能ID
		iDamageRandomIndex :	伤害力骰子
		iHPCost			   :	本次技能的HP消耗，用于计算伤害，该参数暂时无用
		iMPCost			   :	本次技能的MP消耗，用于计算伤害，如果传入0，函数内自动计算技能MP消耗（默认MP消耗和额外消耗）
	*/
	virtual int64_t				ComputeDamageBase(GameLogic::LC_Attacker* pkScriptCaller, int iDamageTargetIndex, int lSkillTypeID, int iDamageRandomIndex, int iCriticalRateRandomIndex, int iHPCost, int iMPCost);


	//添加物品到背包中
	virtual void			AddItemToPack(GameLogic::LC_Attacker* pkScriptCaller, int iItemID, int iItemNum, int iReason);

	virtual void			AddItemToPackByDrop(GameLogic::LC_Attacker* pkScriptCaller, int targetIndex);

	//移除背包中物品
	virtual bool			DeleteItemInPack(GameLogic::LC_Attacker*  pkScriptCaller, int iItemID, int iItemNum, int iReason);

	//出生NPC
	virtual int				ActiveSpawnPoint(GameLogic::LC_Attacker* pkScriptCaller, int lSpawnPointID);


	//创建坐骑
	virtual void			LearnVehicle(GameLogic::LC_Attacker* pkScripterCaller, int lVehicleSkillID);

	//给目标添加状态
	/*
	　  iTargetIndex :	    目标说明
	   lStateTypeID :		状态类型ID
	   fPara0 - fPara4:    状态的前5个可变参数
	   */
	virtual int				AttachSkillState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID, int lStateLifeTime, 
								float fPara0 = 0.0f, float fPara1 = 0.0f, float fPara2 = 0.0f, float fPara3 = 0.0f, float fPara4 = 0.0f);

		//给目标的指挥官添加状态
		/*
		　  iTargetIndex :	    目标说明
		    lStateTypeID :		状态类型ID
			fPara0 - fPara4:    状态的前5个可变参数
			NOTE: 目前这个API必须要求目标是Player，因为Player才有指挥官
		*/
	virtual int					AttachSkillStateToController(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID, int lStateLifeTime, 
		float fPara0 = 0.0f, float fPara1 = 0.0f, float fPara2 = 0.0f, float fPara3 = 0.0f, float fPara4 = 0.0f);

	//给击中的目标添加状态
	/*
	　  iTargetIndex :	    目标说明
	   lStateTypeID :		状态类型ID
	   fPara0 - fPara4:    状态的前5个可变参数
	   */
	virtual void			AttachHitedTargetSkillState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lStateTypeID,	int lStateLifeTime, 
								float fPara0 = 0.0f, float fPara1 = 0.0f, float fPara2 = 0.0f, float fPara3 = 0.0f, float fPara4 = 0.0f);


	//驱散目标状态
	/*
		如果驱散单个状态，则返回驱散状态的重叠数量
		如果驱散一批状态，返回驱散状态的个数
	*/
	virtual int				DetachTargetState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel, int iMaxStateNum);
	virtual int				DetachTargetStateToController(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel, int iMaxStateNum);
	virtual void			NotifyClientDetachTargetState(int attachID, int dstID, int stateID);

	//根据互斥类型，获取目标身上的状态
	virtual int				GetTargetStateCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel);

	//根据互斥类型，获取目标身上状态的重叠数
	virtual int				GetTargetStateOverlapCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iBuffType, int iKind, int iLevel);

	virtual void			DirtySkillState(GameLogic::LC_Attacker* pkScriptCaller, int iStateID);
	//转移状态
	virtual int				TranslateState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iDestIndex, int iBuffType, int iKind, int iLevel);

	//给目标添加技能CD
	virtual void			SetTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, float fCDTime);
	virtual void			ResetTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller,int iTargetIndex,int lSkillTypeID,float fCDTime);
	virtual void			SetTargetSkillCDOnce(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, float fCDTime);

	//给目标添加技能类型CD
	virtual void			SetTargetSkillTypeCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDType, int iTypeID, float fCDTime);

	//给目标清除技能CD
	virtual void			ClearTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID);

	//给目标清除技能类型CD
	virtual void			ClearTargetSkillTypeCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDType, int iTypeID);

	//给目标清除公共CD
	virtual void			ClearTargetGlobalCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//修改技能CD
	virtual void			ModifyTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iCDModifyFuncID, bool bRevertModify);

	//得到技能当前CD
	virtual float			GetTargetSkillCD(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID);


	//神器技能栏中的神器技能
	virtual int				GetShortBarGodSkill(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int ShortBarIndex);

	//打断目标当前技能
	virtual void			InterruptTargetCurrentSkill(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);


	//玩家回城
	virtual void			ReturnToCity(GameLogic::LC_Attacker* pkScriptCaller, bool needadd=false);

	virtual bool			CanReturnToCity(GameLogic::LC_Attacker* pkScriptCaller);

	//是否是跨服
	bool					ServerPlayerCross(GameLogic::LC_Attacker* pkScriptCaller);

	//关闭箱子
	//virtual void ClosePlaceHolder(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//出生NPC
	virtual object_id_type	SpawnNPCNearObject(GameLogic::LC_Attacker* pkScriptCaller, object_id_type nObjectId, int lRadius, int lCharTypeID, int iFictionType, int lSurvialTime);

	//出生NPC
	virtual object_id_type	SpawnNPCByPointID(GameLogic::LC_Attacker* pkScriptCaller, int lSpawnPointID, int lCharTypeID, int lSurvivalTime);

	//出生NPC
	virtual object_id_type	SpawnNPCNearTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lRadius, int lCharTypeID, int iFictionType, int lSurvialTime);

	//出生NPC
	virtual object_id_type	SpawnNPCAroundTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lMinRadius, int lMaxRadius, int lCharTypeID, int iFictionType, int lSurvialTime);

	//出生NPC
	virtual object_id_type	SpawnNPCForwardTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int forwardDis, float angle, int lCharTypeID, int iFictionType, int lSurvialTime);

	//设置npc召唤主id
	virtual void			SetNPCSummonOwnerID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, object_id_type lSummonOwnerID);
	//设置npc召唤主id 且把召唤主的属性赋值给npc
	virtual void			SetNPCFromSummonOwnerAttr(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, object_id_type lSummonOwnerID);
	
	//设置镖车状态
	virtual void SetEscortNpcState(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int EscortNpcState);

	//设置npc已经召唤的ID
	virtual void			SetNPCSummorID(GameLogic::LC_Attacker* pkScriptCaller, object_id_type lSummonID);

	//完成任务
	virtual void			FinishTask(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID);

	//放弃任务
	virtual void			CancelTask(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID);

	//设置任务时间触发器
	virtual void			DestineTaskTimeEvent(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int lTime, bool bNoticeClient);

	//设置任务失败状态
	virtual void			SetTaskFail(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iState);

	//地图中特效墙门的状态控制
	virtual void			ControlMapLogicDoorState(GameLogic::LC_Attacker* pkScriptCaller,  int iDoorIndex, int iState);

	//恢复目标HP
	/*
		iTargetIndex :		目标说明
		lSourceTypeID :		产生恢复HP事件人的ID
		lRecoverHP :		HP恢复绝对值
		fHPRecoverPercent : HP恢复百分比
	*/
	virtual void			RecoverTargetHP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, attr_value_type lRecoverHP, float fHPRecoverPercent, object_id_type lSrcActorID);

	//恢复目标MP
	/*
		iTargetIndex :		目标说明
		lSourceTypeID :		产生恢复MP事件人的ID
		lRecoverMP :		MP恢复绝对值
		fMPRecoverPercent :	MP恢复百分比
	*/
	virtual void			RecoverTargetMP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, attr_value_type lRecoverMP, float fMPRecoverPercent, object_id_type lSrcActorID);
	//设置目标HP
	/*
		iTargetIndex :		目标说明
	*/
	virtual void			SetTargetHP(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t lHP);

	virtual void			AddEscortTimes(object_id_type playerID);
	virtual void			UpdateEscortRecord(GameLogic::LC_Attacker* pkScriptCaller);

	//目标查找方式与SetTargetHP不同，全局查找，不是仅在当前地图找
	virtual void			KillNpcAnyhow(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//消耗对方物品
	/*
	iTargetIndex :		目标说明
	lSourceTypeID :		产生消耗物品事件人的ID
	lItemTypeID :		物品类型
	lItemCount :		物品数量
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


	//设置任务状态
	virtual void			SetTaskFinishState(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iIndex, int lState);

	//设置任务追踪索引
	virtual void			SetTaskTraceIndex(GameLogic::LC_Attacker* pkScriptCaller, int lTaskID, int iIndex);

	//显示游戏情节消息
	virtual void			ShowStoryMessage(int lMsgID, const StringType& rkMsgKey, int iTargetType, int iTargetID, int iShowType, int lParam);

	//显示游戏情节消息
	virtual void			ShowStoryMessageByString(int lMsgID, const StringType& rkMsgKey, int iTargetType, int iTargetID, int iShowType, int lParam, const StringType& strParam);

	//设置职业
	virtual void			SetCareerType(GameLogic::LC_ActorBase* pkActor, int iCareerType);

	//virtual void			NotifyCareerSlectionSucceed(GameLogic::LC_ActorBase* pkMachineOwner,bool flag);

	//至尊接口 Begin

	//消耗怒气值
	virtual void			CostTargetBerserk(GameLogic::LC_ActorBase* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int iCostValue);
	//回复目标怒气
	virtual void			RecoverTargetBerserk(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSourceTypeID, int lRecoverValue);
	//设置回复怒气的时间间隔
	virtual void			SetBerserkTimer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fTimer);
	//回滚怒气的时间间隔
	virtual void			ResetBerserkTimer(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//设置杀怪回复怒气的概率
	virtual void			SetBerserkProbability(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fProbability);
	//回滚杀怪回复怒气的概率
	virtual	void			ResetBerserkProbability(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//设置怒气最大值
	virtual void			SetMaxBerserkPoint(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iMaxBerserk);

	//至尊接口 End

	//阵营接口 Begin
	//设置npc阵营
	virtual void			SetFactionID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iFactionID);

	//阵营接口 End


	//AI接口Begin

	virtual bool			ChangeActorAIFunc(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iAIFuncID);

	//按照特殊方式给仇恨排序
	//仇恨队列默认按照仇恨值由大到小排序，获取指定位置的仇恨值
	virtual int				GetSortHatredByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred);
	//获取仇恨队列中指定位置目标的仇恨值
	virtual attr_value_type	GetHatredValueByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred);
	//获取仇恨队列中指定ID的仇恨值
	virtual attr_value_type	GetHatredValueByID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID);
	//获取仇恨的个数
	virtual int				GetHatredCount(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//设置仇恨队列中指定位置目标的仇恨值
	virtual void			SetHatredValueByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, int lHatredValue, bool bExcludeCurrentHatred);
	//设置仇恨队列中指定ID目标的仇恨值
	virtual void			SetHatredValueByID(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lHatredValue);
	//添加仇恨目标
	virtual void			AddHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lHatredValue);
	//删除仇恨队列中指定位置的目标
	virtual void			RemoveHatredByIndex(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iHatredIndex, bool bExcludeCurrentHatred);
	//删除仇恨队列中指定ID的目标
	virtual void			RemoveHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID);
	//清空仇恨
	virtual void			ClearHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//在技能队列中添加目标和技能
	virtual void			AddHatredSkill(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, int lSkillID, int iPriority, float fSkillLifeTime);
	//清空技能队列
	virtual void			ClearHatredSkillList(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//怪物随机技能-克隆，召唤怪物后设置怪物属性等
	virtual void			ProcessCloneMonsterAfterSpawn(GameLogic::LC_Attacker* pkScriptCaller, int iSourceIndex, int iTargetIndex);

	//获取特定属性的目标
	virtual object_id_type	SelectLockedTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//给npc指定一个目标，在此期间仇恨队列不起作用
	virtual void			AddAdditionHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lHatredID, float fDurationTime);

	//清除特殊仇恨目标
	virtual void			ClearAdditionHatred(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//AI接口End

	//请求进入副本
	virtual bool			ReqEnterTranscription(GameLogic::LC_Attacker* pkScriptCaller, int iTranscriptionType);

	//请求退出副本
	virtual bool			ReqQuitTranscription(GameLogic::LC_Attacker* pkScriptCaller, int32_t iDelay/*=0秒数*/);

	virtual void			AddGuildPresent(GameLogic::LC_Attacker* pkScriptCaller, int32_t itemID, int32_t way, int32_t count);

	//添加系统称号接口
	virtual void			AddSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID);
	//升级
	virtual void			UseItemAddlevel(GameLogic::LC_ActorBase* pkScriptCaller, int ilevel);
	//开启秘笈
	virtual int				CheckUseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID);
	virtual int				UseItemOpenSkillBook(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iID);
	//加进阶属性
	virtual int				UseItemAddSubordinateAttr(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iAttributeType, int32_t iVal, int32_t iPercent);
	virtual int				UseItemCommitAddSubordinateAttr(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t uiValCount, int32_t uiPerCount);
	//加进阶祝福点
	virtual int				UseItemAddSubordinateBlessPoint(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iVal, int32_t iPercent);
	virtual	int				CheckUseItemOpenSubordinate(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iLevel);
	virtual	int				UseItemOpenSubordinate(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iSubType, int32_t iLevel);
	//进阶线等级提升丹
	virtual int				UseItemAddSubLevel(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iItemID, int32_t iSubType,
											   int32_t iLevelLimit, int32_t iLevel, const StringType& sRewards);
	virtual	int				UseItemAddSubLevelTo(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iItemID, int32_t iSubType,
												 int32_t iLevelLimit, const StringType& sRewards);

	virtual void			UseItemAddActScore(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iType, int32_t iVal, const StringType sCatagory);
	virtual	void			UseItemAddSnowManScore(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iScore, int32_t iAchieve, const StringType sCatagory);

	virtual ResultType		AddGemReinforceExp(GameLogic::LC_ActorBase* pkScriptCaller, int32_t iType, int32_t iExp);

	//移除系统称号接口
	virtual void			RemoveSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID);

	//设置系统称号接口
	virtual void			SetSystemTitle(GameLogic::LC_Attacker* pkScriptCaller, int lID);

	//让目标缴械
	virtual bool			DisarmTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int lSkillTypeID, int iRandomIndex, int lUnDisarmStateID, float fDisarmDuration);

	//解除目标的缴械状态
	virtual void			UnDisarmTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//通知技能释放事件
	virtual void			NoticeSkillExecuteEvent(object_id_type lPlayerID, int lSkillID, object_id_type lTargetID);

	//通知开始护送NPC事件
	virtual void			NoticeConvoyNPCEvent(object_id_type lPlayerID, object_id_type lNPCID, int lTaskID);

	//通知任务游戏事件
	virtual void NoticeTaskGameEvent(object_id_type lPlayerID, int lTaskID, int lTaskEventType, int lParam1, int lParam2);

	//给目标增加经验
	virtual void			AddExp(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int64_t iExp);

	//打坐增加经验
	virtual void			RecoverExp(GameLogic::LC_Attacker* pkScriptCaller, float multiply);

	//回复目标真气
	virtual void			RecoverSkillPoint(GameLogic::LC_Attacker* pkScriptCaller, float multiply);

	//打坐经验加成，角色有效
	virtual int				GetSitRecoverExpIncrement(GameLogic::LC_Attacker* pkScriptCaller, float multiply);


	//给目标增加真气（定值）
	virtual void			AddTargetSkillPoint(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, attr_value_type iSkillPoint);
	//给目标增加等级相关真气
	virtual void			AddTargetSkillPointLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fParam1);

	//给目标增加等级相关的经验
	virtual void			AddTargetCashLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float fParam1);

	virtual void			AddCash(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, MoneyType iCash, int iCashType, int iSrcType, int iSrcID);

	//给目标加上等级相关经验(目前进阶奖励用)
	virtual void			AddTargetExpRelatedLevel(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, float param1);

	//目标存在指定的SkillFlag，则打断目标技能
	virtual void			CheckSkillFlagAndInterrupt(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iSkillFlagIndex, object_id_type lLogicID);

	//获取当前系统时间
	virtual int				GetCurrentSystemTime();

	//组队传送
	virtual	void			GroupTranstfer(GameLogic::LC_Attacker* pkScriptCaller, int iResMapID, int lX, int lY);

	//获取对象的目标ID
	virtual void			AutoFitLevelReInit(GameLogic::LC_Attacker* pkScriptCaller, int iLevel);

	//添加行为屏蔽
	virtual void			AddActionForbid(GameLogic::LC_Attacker* pkScriptCaller, int iForbidType, int lParam1 = 0, int lParam2 = 0);

	//移除行为屏蔽
	virtual void			RemoveActionForbid(GameLogic::LC_Attacker* pkScriptCaller, int iForbidType, int lParam1 = 0, int lParam2 = 0);

	//设置生存时间
	virtual void			SetSurvivalTime(GameLogic::LC_Attacker* pkScriptCaller, int iSurvivalTime);
	virtual void			SetTargetSurvivalTime(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int iSurvivalTime);
	virtual int				IsSuvivalDead(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//获取AI功能值
	virtual int				GetAIFuncValue(GameLogic::LC_Attacker* pkScriptCaller, int iAIFuncType);

	//设置变身
	virtual void			SetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller, int lCharTypeID);

	//获取变身信息
	virtual int				GetShapeShiftInfo(GameLogic::LC_Attacker* pkScriptCaller);

	virtual void			AddTotalHookTime(GameLogic::LC_Attacker* pkScriptCaller, int addHookTime);
	virtual void			NotifyHookInfoUpdate(GameLogic::LC_Attacker* pkScriptCaller);

	//NPC添加归属
	virtual void			AddOwnerID(GameLogic::LC_Attacker* pkScriptCaller, object_id_type lOwnerID, bool bConsiderGroupMember);

	//NPC固定归属
	virtual bool			SetNPCFixedOwnerListFlag(GameLogic::LC_Attacker* pkScriptCaller, bool bFlag);

	////使用物品增加天赋点
	//virtual bool			AddTalentPointByItem(GameLogic::LC_Attacker* pkScriptCaller, int lItemID);
	////清空天赋
	//virtual bool			ClearAllTalentByItem(GameLogic::LC_Attacker* pkScriptCaller, int lItemID);

	//冻结动作
	virtual void			FreezeAnimation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);
	//解冻动作
	virtual void			UnFreezeAnimation(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex);

	//Notify NPC Execute Skill
	virtual void			NPCExecuteSkill(GameLogic::LC_ActorBase* pkActor, int lSkillID);

	virtual void			NPCExecuteSkillEx(GameLogic::LC_ActorBase* pkActor, int lSkillID, object_id_type lTargetLocked);

	virtual void			ServerPlayerExecuteSkill(GameLogic::LC_ActorBase* pkActor, int lSkillID);

	virtual void			RltPlayerExecutePairSit(GameLogic::LC_ActorBase* pkActor);


	virtual	void			ShowRichUI(GameLogic::LC_Attacker* pkScriptCaller, int iTargetType, int iRichUIID, int bShow);

	//设置玩家5层勇气Buff标记
	virtual void			SetCourage(GameLogic::LC_Attacker* pkCaller , bool Value);

	//获取NPC所在组成员数量
	virtual int				GetGroupNpcSize(GameLogic::LC_Attacker* pkCaller);

	//根据索引获取组内成员ID
	virtual object_id_type	GetGroupNpcIDByIndex(GameLogic::LC_Attacker* pkCaller, int index);

	//根据半径随机走位
	virtual bool			SetNextPosInCircle(GameLogic::LC_Attacker* pkCaller, int iTargetIndex,/*int posx,int posy,*/ int innerRadius, int outRadius, float step, bool clockwise);

	//瞬移NPC（服务器需要检查点的可行性
	virtual void			TransferToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius);
	virtual void			TransferBackToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, bool bUseSafeRadius);
	//瞬移玩家
	virtual void			TransferTargetToTarget(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex0, int iTargetIndex1, bool bUseSafeRadius);
	//瞬移坐标
	virtual void			TransferToXYZ(GameLogic::LC_Attacker* pkScriptCaller, int iTargetIndex, int x, int y, int z);
	//virtual void			SetPostTransferLocation(GameLogic::LC_Attacker* pkScriptCaller);

	//击退目标
	virtual void			PunchBackTarget(GameLogic::LC_ActorBase* pkActor, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime);
	virtual void			PunchBackTargetToPoint(GameLogic::LC_ActorBase* pkActor, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime);
	virtual void			PunchBackTargetBySide(GameLogic::LC_ActorBase* pkActor, int iTargetIndex, int iCenterTarget, int iPunchDistance, float fPunchTime);

	//用击退代替强制位移
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

	//向客户端的玩家发送一条自己拼的字符串消息，客户端会根据msgType进行相应的显示
	virtual void			SendNotifyStrToClientPlayer(GameLogic::LC_ActorBase* pkCaller, const StringType& str, int msgType, int level);
	//向客户端的玩家发送一条SystemError.csv中对应ID的字符串消息，客户端会根据msgType进行相应的显示
	virtual void			SendNotifyErrorIDStrToClientPlayer(GameLogic::LC_ActorBase* pkCaller, int systemErrorID, int level);

	//公会战自动获得经验统计接口
	virtual void			AddGuildWarAutoExp(GameLogic::LC_ActorBase* pkCaller, attr_value_type iExpValue);

	void					GuildWarCapapultMsg(GameLogic::LC_ActorBase* pkCaller, GameLogic::LC_ActorBase* pkTarget, int damage);
	void					GuildWarCapapultAllMsg(GameLogic::LC_ActorBase* pkCaller, int count, int damage);

	//锁定目标attributemap中某项属性
	virtual bool			LockAttribute(GameLogic::LC_ActorBase* pkCaller, int iAttributeType, bool bLock, int iValue);

	//判断镖车周围圆形区域有没有对应的劫镖人
	virtual int				LockRobberInCircle(GameLogic::LC_Attacker* pkScriptCaller, int iSrcIndex, int lActorFilterFuncID, bool bExclusive, 
								int iCircleRadius, int iMaxLockCount, int iRandomIndex, object_id_type lFileterPlayerID);
	//调用服务器MapLogic_place的招怪接口刷怪
	virtual object_id_type	MapLogicSpawnNPC(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex, int minRadius, int maxRadius, int lCharTypeID);
	//调用服务器MapLogic_place的招怪接口刷怪
	virtual object_id_type	MapLogicSpawnNPCXYZ(GameLogic::LC_ActorBase* pkCaller, int x, int y, int z, int lCharTypeID);
	//调用服务器MapLogic_place的招怪接口刷精英怪
	virtual object_id_type	MapLogicSpawnEliteNPC(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex, int minRadius, int maxRadius, int lCharTypeID, int rule1, int rule2, int rule3);
	//设置怪物做多存活时间
	virtual void			SetNPCSurvivalTime(GameLogic::LC_ActorBase* pkCaller, int iTargetIndex, float dTime);

	//得到当前地图中自己SummorID的怪物
	virtual int				GetCurrentMapSummorCount(GameLogic::LC_ActorBase* pkCaller);

	//帮会副本Boss倒计时通知(广播)
	virtual void			NotifyTranscriptionData(GameLogic::LC_ActorBase* pkCaller, int kType, int param1, int param2);

	//帮会副本Boss倒计时通知(单播)
	virtual void			NotifySingleTranscriptionData(GameLogic::LC_ActorBase* pkCaller, GameLogic::LC_ActorBase* pkPlayer, int kType, int param1, int param2);

	//解除坐骑
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

	//得到当前怪物召唤物的总召唤数目
	virtual int				GetSummorIDCount(GameLogic::LC_ActorBase* pkCaller);
	//得到索引当前怪物召唤物的CharTypeID
	virtual object_id_type	GetSummorIDByIndex(GameLogic::LC_ActorBase* pkCaller, int iIndex);

	//得到帮会运镖中的劫镖数目
	virtual int				GetEscortStealCount(GameLogic::LC_ActorBase* pkCaller);

	//清除怪物身上的Attacker
	virtual void			SetNPCAttacker(GameLogic::LC_ActorBase* pkCaller, int attackerID);

	virtual void			CopyHitedInfo(GameLogic::LC_Attacker* pkCaller, GameLogic::LC_Attacker* dstActor);

	virtual void			ClearHitedInfo(GameLogic::LC_ActorBase* pkCaller);

	//transferID 对应	TransferArea.csv表中的Index
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

	//命中率=（（自身命中-对方躲避）/对方等级+命中率附加）
	bool					_judgeNormalAttack(attr_value_type iDog, attr_value_type iDogRate, int64_t iAttackerLevel, int64_t iTargetLevel, attr_value_type iAttackRate, double fRandom);
	double					_compureAttackRate(attr_value_type iDog, attr_value_type iDogRate, int64_t iAttackerLevel, int64_t iTargetLevel, attr_value_type iAttackRate);

	//暴击率=（（自身暴击-对方韧性）/对方等级+暴击率附加）%
	bool					_judgeCriticalAttack(attr_value_type iCritical, attr_value_type iCriticalRate, attr_value_type iCriticalDef, int64_t iAttackerLevel, int64_t iTargetLevel, double fCriticalRateAdd, double fRandom, LC_ActorBase* pSrc, LC_ActorBase* pDst);
	double					_computeCriticalRate(attr_value_type iCritical, attr_value_type iCriticalRate, attr_value_type iCriticalDef, int64_t iAttackerLevel, int64_t iTargetLevel, double fCriticalRateAdd, LC_ActorBase* pSrc, LC_ActorBase* pDst);

	void					_generateSkillDamageMultiple( GameLogic::LC_Attacker* pkScriptCaller ,  int iDamageTargetIndex, int lSkillTypeID,  int iDamageRandomIndex , int iAttackRateRandomIndex, int iCriticalRateRandomIndex,  object_id_type lDamageSourceActorID, int iHPCost, int iMPCost, int Multiple, int iPercent, int bUseScore, bool bDoPunch = true);
	//基本伤害=（（伤害力-对方防御力）*攻击频率+ 内息消耗*伤害系数-对方防御力）
	/*
		新的计算公式：

		A为攻击方，B为防御方

		基础伤害={技能系数*max【(A攻击-B防御)，A攻击*0.35】+技能伤害固定值}*(1+A伤害加成-B伤害减免)+战力伤害*技能系数

		战力伤害= power（B战力/A战力，0.5）/100*B最大生命值
	*/
	double					_computeDamageBase(double iDamage, attr_value_type iDef, int32_t iMPCost, double fDamageRate, double fOverAttackRate, double appendRate);
	//百分比区别
	double					_computeMinAck(object_id_type lTargetID, object_id_type lAttackerID);

	void					_doDmagePunch( int64_t iFinalDamage, LC_ActorBase* pkTarget, CF_SkillExecuteList::DataEntry* pkSkillData, object_id_type lTargetID, LC_ActorBase* pkAttacker );
	//战力伤害= (power（B战力/A战力，0.5）/100*B最大生命值)
	int64_t					_computeScoreDamgeBase(uint64_t iAScore, uint64_t iBScore, attr_value_type bMaxHP, double skillaset);
	//从调用者的目标队列中找到指定的目标
	virtual void	_findTarget(GameLogic::LC_Attacker* pkCaller, int iTargetIndex, GameLogic::LC_ActorPtrList& kResult);

	//从调用者的目标队列中找到指定的目标
	virtual double	GetBonFirPartyMapBuffStateMutableParameter(GameLogic::LC_Attacker* pkCaller);
};
#endif

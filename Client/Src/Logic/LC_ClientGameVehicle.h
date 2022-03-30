//客户端骑乘类,召唤出来的坐骑
#pragma  once


#include "MG_MessageHandlerInterface.h"
#include "ClientDefine.h"
#include "GD_AnimModelBase.h"

class LC_ClientGameVehicle : public GameLogic::LC_VehicleBase, 
						     public MG_MessageHandlerInterface
{
public:
	LC_ClientGameVehicle();
	~LC_ClientGameVehicle();

	virtual void			Update(float fCurrentTime, float fDeltaTime);
	virtual bool			NotifyStateEnd(int iEndState,float fCurrentTime,float fDeltaTime);

	virtual bool			RecvMessage(MG_MessageBase& rkMessage);

	virtual void			CreateVisibleData();
	virtual void			DestoryVisibleData();

	virtual void			PlayAnimation(const StringType& rkAnim,bool bForce = false,float fSequency = 1.0f,bool bAutoAdjustFrequency = false);
    virtual void			PlayIdleAnimation(void);

	void					SetSynFlag(bool bSynFlag) { m_bSynFlag = bSynFlag; }
	bool					GetSynFlag() { return m_bSynFlag; }

	float					GetLastSceneMouseOverTime() { return m_fLastSceneMouseOverTime;}
	void					SetLastSceneMouseOverTime(float fSceneMouseOverTime) { m_fLastSceneMouseOverTime = fSceneMouseOverTime;}

	float					GetLastPickTerrainTime() { return m_fLastPickTerrainTime;}
	void					SetLastPickTerrainTime(float fPickTerrainTime) { m_fLastPickTerrainTime = fPickTerrainTime;}
	
	bool					GetMoveStride(){return m_bMoveStride;}
	void					SetMoveStride(bool bMoveStride){m_bMoveStride = bMoveStride;}

	//鼠标类型
	void					SetMouseLogicModeType(MouseLogicModeType eType){m_eMouseLogicModeType = eType;}
	MouseLogicModeType		GetMouseLogicModeType(){return m_eMouseLogicModeType;}
	void					SetLastMouseOverActorID(long lActorID) { m_lLastMoveOverActorID = lActorID;}
	long					GetLastMouseOverActorID()	{ return m_lLastMoveOverActorID;}
	
	//Begin 客户端路径接口
	Path::PF_Path&		    GetMovePath(){return m_kMovePath;};
	void					SetMoveLineTargetLocation(const UT_Vec3Int& kLocation){m_kMoveLineTargetLocation = kLocation;};
	UT_Vec3Int				GetMoveLineTargetLocation(){return m_kMoveLineTargetLocation;};
	//End 路径接口

	//Begin 状态控制接口
	virtual StateMachine::StateType FindMoveState();
	//End	状态控制接口
	
	virtual void            SetTargetLocation( const Utility::UT_Vec3Int& rkTarget );

    Utility::UT_Vec3Int		GetVisibleSeatLocation(void);

	//---------------  Begin 技能 -------------------//
	//检查技能是否能够使用，以及触发物品是否有效
	virtual ResultType		CheckExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,long lItemID);	

	//准备执行技能
	virtual ResultType		PrepareExecuteSkill(Skill::SK_SkillExecute* pkSkill,long lItemID);

	//执行技能，并设置状态机，但不执行Update
	virtual ResultType		ExecuteSkill(float fCurrentTime,Skill::SK_SkillExecute* pkSkill,long lItemID);
	//---------------  End 技能 -------------------//

protected:
	float					m_fLastSceneMouseOverTime;
	float					m_fLastPickTerrainTime;
	float					m_fLastSyncTime;

	bool					m_bMoveStride;
	bool					m_bSynFlag;	

	MouseLogicModeType		m_eMouseLogicModeType;
	long					m_lLastMoveOverActorID;
	
	//Begin 路径数据
	Path::PF_Path						 m_kMovePath;
	Utility::UT_Vec3Int					 m_kMoveLineTargetLocation;
	//End 路径数据
};
 
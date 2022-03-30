#pragma once
#include "MC_UniCastDataDirtyFlag.h"
#include "MC_BroadCastDataDirtyFlag.h"
#include "LC_DataChangeObserver.h"

class LC_ServerPlayer;

class MC_DataDirtyFlag : public GameLogic::LC_DataChangeObserver
{
public:
	MC_DataDirtyFlag(void);
	~MC_DataDirtyFlag(void);
	
	MC_UniCastDataDirtyFlag& GetUniCastFlag();
	MC_BroadCastDataDirtyFlag& GetBroadCastFlag();	

	void InitFlagForAllBuddy();

	void SetPlayer(LC_ServerPlayer* player);

	virtual void Update(GameLogic::DirtyFlag flag);

	void SetMoveSpeed();
	void SetStateType();
	void SetPVEState();
	void SetHPMP();
	void SetLocation();
	void SetPlayerEnterOrLeave();
	void SetInteractionValue();

private:
	MC_UniCastDataDirtyFlag		m_uniCastDataDirtyFlag;
	MC_BroadCastDataDirtyFlag	m_broadCastDataDirtyFlag;
};

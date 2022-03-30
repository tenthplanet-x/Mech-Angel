#pragma once
#include "MC_BaseDataDirtyFlag.h"

class MC_UniCastDataDirtyFlag : public MC_BaseDataDirtyFlag
{
public:
	MC_UniCastDataDirtyFlag(void);
	~MC_UniCastDataDirtyFlag(void);

	void SetPlayerEnterOrLeave();
	void SetNpcEnterOrLeave();
	void SetSysnAll();

	void ClearPlayerEnterOrLeave();
	void ClearNpcEnterOrLeave();
	void ClearSysnAll();

	bool IsPlayerEnter()const;
	bool IsPlayerLeave()const;
	bool IsPlayerEnterOrLeave();
	bool IsNpcEnterOrLeave();
	bool IsSysnAll();
private:
	bool m_playerEnterOrLeave;
	bool m_npcEnterOrLeave;
	bool m_sysnAll;
};

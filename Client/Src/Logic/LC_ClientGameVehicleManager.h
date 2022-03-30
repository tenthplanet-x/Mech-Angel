//客户端玩家座骑管理类
#pragma once
#include "LC_VehicleManagerBase.h"

class LC_ClientGameVehicle;
class LC_ClientGameVehicleManager : public GameLogic::LC_VehicleManagerBase
{
public:
	LC_ClientGameVehicleManager();
	virtual ~LC_ClientGameVehicleManager();

	virtual void Update(float fCurrentTime, float fDeltaTime);
	virtual void Reset();

protected:

	virtual GameLogic::LC_VehicleBase*	_allocVehicle();
	virtual	void						_freeVehicle(GameLogic::LC_VehicleBase* pkVehicle);

	Utility::UT_MemoryPool<LC_ClientGameVehicle>*	m_pkClientVehiclePool;
};
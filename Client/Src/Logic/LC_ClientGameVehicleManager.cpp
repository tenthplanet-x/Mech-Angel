#include "ClientPCH.h"

#include "LC_ClientGamePlayer.h"
#include "LC_ClientGamePlayerManager.h"
#include "LC_ClientGameVehicle.h"
#include "LC_ClientGameVehicleManager.h"
#include "LC_ClientMap.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientLogicManager.h"

using namespace GameLogic;
using namespace Protocol;
using namespace Utility;
using namespace StateMachine;

LC_ClientGameVehicleManager::LC_ClientGameVehicleManager()
: LC_VehicleManagerBase()
{
	m_pkClientVehiclePool = T_NEW_D Utility::UT_MemoryPool<LC_ClientGameVehicle>;
	m_pkClientVehiclePool->Init(1, 10);
}
//------------------------------------------------------------------------------
LC_ClientGameVehicleManager::~LC_ClientGameVehicleManager()
{
	T_SAFE_DELETE(m_pkClientVehiclePool);
}
//------------------------------------------------------------------------------
void LC_ClientGameVehicleManager::Update(float fCurrentTime, float fDeltaTime)
{
	LC_VehicleIDPtrMapIter IterBe = m_kVehicleIDPtrMap.begin();
	LC_VehicleIDPtrMapIter IterEd = m_kVehicleIDPtrMap.end();

	for( ; IterBe != IterEd; IterBe++)
	{
		LC_VehicleBase* pkVehicle = (*IterBe).second;
		pkVehicle->Update(fCurrentTime,fDeltaTime);
	}
}
//------------------------------------------------------------------------
void LC_ClientGameVehicleManager::Reset()
{
	LC_VehicleIDPtrMapIter IterBe = m_kVehicleIDPtrMap.begin();
	LC_VehicleIDPtrMapIter IterEd = m_kVehicleIDPtrMap.end();

	for( ; IterBe != IterEd; IterBe++)
	{
		LC_VehicleBase* pkVehicle = (*IterBe).second;
		_freeVehicle(pkVehicle);
	}

	m_kVehicleIDPtrMap.clear();
}
//------------------------------------------------------------------------------
LC_VehicleBase*	LC_ClientGameVehicleManager::_allocVehicle()
{
	return m_pkClientVehiclePool->Alloc();
}
//------------------------------------------------------------------------------
void LC_ClientGameVehicleManager::_freeVehicle(GameLogic::LC_VehicleBase* pkVehicle)
{
	m_pkClientVehiclePool->Free((LC_ClientGameVehicle*)pkVehicle);
}
//------------------------------------------------------------------------------
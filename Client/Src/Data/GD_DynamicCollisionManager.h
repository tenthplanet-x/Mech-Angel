#pragma once

#include "MG_MessageHandlerInterface.h"


class GD_DynamicCollisionManager : public Memory::MM_BaseObject
                                 , public MG_MessageHandlerInterface
                                 , public Utility::UT_Singleton<GD_DynamicCollisionManager>
{
public:
	GD_DynamicCollisionManager(void);
	virtual ~GD_DynamicCollisionManager(void);

    void OnMapReloaded(bool bMapChanged);

    bool EnableDynamicCollision(long lID);
    void DisableDynamicCollision(long lID, bool bUnregisterDCShape = false);

    virtual bool RecvMessage(MG_MessageBase& rkMessage);
	
private:
    void _eventTimerTick(MG_MessageBase& rkMessage);

private:
    typedef mem::set<long> DynamicCollisionEnabledNpcIDSet;

    DynamicCollisionEnabledNpcIDSet m_kDCEnablingNpcIDs;
    DynamicCollisionEnabledNpcIDSet m_kDCRegisterdNpcIDs;
    DynamicCollisionEnabledNpcIDSet m_kDCEnabledNpcIDs;
};
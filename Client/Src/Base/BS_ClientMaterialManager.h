#ifndef _BS_CLIENT_MATERIAL_MANAGER_H
#define _BS_CLIENT_MATERIAL_MANAGER_H

#include "GD_MaterialManager.h"
#include "MG_MessageHandlerInterface.h"

class BS_ClientMaterialManager : public Data::GD_MaterialManager,
								 public MG_MessageHandlerInterface
{
public:
	BS_ClientMaterialManager();
	virtual ~BS_ClientMaterialManager();

	virtual bool RecvMessage(MG_MessageBase& rkMessage);

protected:
	virtual	void _preCreateProtoType();
    virtual	void _preReleaseProtoType(void);

	void _eventTimerTick(MG_MessageBase& rkMessage);
	void _eventTimerTick2(MG_MessageBase& rkMessage);
	
private:
	float m_fLastPurgeTime;
};

#endif
#pragma once

#include "LC_ServerMapLogic_Place.h"

class LC_ServerMapLogic_NewBee : public LC_ServerMapLogic_Place
{
public:
	LC_ServerMapLogic_NewBee(void);
	~LC_ServerMapLogic_NewBee(void);

	virtual void				Init();
	virtual void				Update(float fCurrentTime,float fDeltaTime);

protected:

	float						m_fLastCheckTime;
};

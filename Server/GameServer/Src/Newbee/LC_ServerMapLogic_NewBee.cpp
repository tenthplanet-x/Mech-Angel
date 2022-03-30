

#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_NewBee.h"

//------------------------------------------------------------------------------
LC_ServerMapLogic_NewBee::LC_ServerMapLogic_NewBee(void)
: LC_ServerMapLogic_Place()
{
}
//------------------------------------------------------------------------------
LC_ServerMapLogic_NewBee::~LC_ServerMapLogic_NewBee(void)
{

}
//------------------------------------------------------------------------------
void LC_ServerMapLogic_NewBee::Init()
{
	LC_ServerMapLogic_Place::Init();
	SetMapLogicType(MT_NEWBEE);
}
//-----------------------------------------------------------------------------
void LC_ServerMapLogic_NewBee::Update( float fCurrentTime,float fDeltaTime )
{
	LC_ServerMapLogic_Place::Update(fCurrentTime,fDeltaTime);
}




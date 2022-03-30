#include "UT_GMLogic.h"
#include <boost/function.hpp>

using namespace Utility;

UT_GMLogic::UT_GMLogic()
{

}

UT_GMLogic::~UT_GMLogic()
{

}

void UT_GMLogic::Init()
{

}

void UT_GMLogic::UnInit()
{

}

void UT_GMLogic::Update( float fCurrent, float fDelta )
{
	Update_SendMail( fCurrent, fDelta );
	Update_BroadcastMsg( fCurrent, fDelta );
	Update_KickUser( fCurrent, fDelta );
}

void UT_GMLogic::Update_SendMail(float fCurrent, float fDelta)
{

}

void UT_GMLogic::Update_BroadcastMsg(float fCurrent, float fDelta)
{

}

void UT_GMLogic::Update_KickUser(float fCurrent, float fDelta)
{

}


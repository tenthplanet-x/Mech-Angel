#include "ClientPCH.h"
#include "BS_ClientModelContentManager.h"
#include "BL_AsyncOperationManager.h"
bool BS_ClientModelContentManager::_doGetMainThreadPermission( void )
{
	if( BL_AsyncOperationManager::GetSingletonPtr() )
	{
		return false;
	}
	else
	{
		return true;
	}
}
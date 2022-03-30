#include "CF_ServerCSVManager.h"
#ifdef ACTIVATION_CODE
#include "CSV_Activation_Code_Helper.h"
#else
#include "CSV_Server_Helper.h"
#include "LC_ServerChestCSVManager.h"
#include "ConfigManager.h"
#endif

//-------------------------------------------------------------------
CF_ServerCSVManager::CF_ServerCSVManager()
{
}
//-------------------------------------------------------------------
CF_ServerCSVManager::~CF_ServerCSVManager()
{

}
//-------------------------------------------------------------------
bool CF_ServerCSVManager::Init()
{
#ifdef ACTIVATION_CODE
	if (Init_Activation_Code_CSV_File() == false)
	{
		return false;
	}
#else
	if(Init_Server_CSV_File() == false)
	{
		return false;
	}
	LC_ServerChestCSVManager * obj = LC_ServerChestCSVManager::GetSingletonPtr();
	if(obj == NULL)
	{
		obj = T_NEW_D LC_ServerChestCSVManager;
	}
	obj->Init();
#endif
	return true;
}
//-------------------------------------------------------------------
void CF_ServerCSVManager::UnInit()
{
#ifdef ACTIVATION_CODE
	UnInit_Activation_Code_CSV_File();
#else
	UnInit_Server_CSV_File();
	LC_ServerChestCSVManager * obj = LC_ServerChestCSVManager::GetSingletonPtr();
	T_SAFE_DELETE(obj);
#endif
}

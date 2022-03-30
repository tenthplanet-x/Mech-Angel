
#include "SM_ServerControllerFactory.h"
#include "SM_ServerNPCBaseController.h"
#include "SM_ServerPlayerController.h"
#include "SM_ServerBuddyController.h"
#include "CF_MonsterAIFunc.h"

using namespace StateMachine;
//----------------------------------------------------------
SM_ServerControllerFactory::SM_ServerControllerFactory()
{
}
//----------------------------------------------------------
SM_ServerControllerFactory::~SM_ServerControllerFactory()
{
}
//----------------------------------------------------------
bool SM_ServerControllerFactory::Init()
{
	SM_ControllerFactory::Init();

	SM_ControllerBase* pkController = T_NEW_D SM_ServerNPCBaseController;
	pkController->SetControllerType(SCT_SERVER_NPC);
	m_kCtrlTypePtrMap[SCT_SERVER_NPC] = pkController;

	pkController = T_NEW_D SM_ServerPlayerController;
	pkController->SetControllerType(SCT_SERVER_PLAYER);
	m_kCtrlTypePtrMap[SCT_SERVER_PLAYER] = pkController;

	pkController = T_NEW_D SM_ServerBuddyController;
	pkController->SetControllerType(SCT_SERVER_BUDDY);
	m_kCtrlTypePtrMap[SCT_SERVER_BUDDY] = pkController;

	/*
	*加载所有的AI控制脚本
	*/
	CSVFile::CF_MonsterAIFunc* pkAIList = CSVFile::CF_MonsterAIFunc::GetSingletonPtr();
	CSVFile::CF_MonsterAIFunc::DataEntryMapExternIterator iter = pkAIList->GetIterator(); 
	while (iter.HasMoreElements())
	{
		CSVFile::CF_MonsterAIFunc::DataEntry* pkAIData = iter.PeekNextValuePtr();
		const StringType & kScriptName = pkAIData->_kAIControlScriptPath;
		//如果存在脚本文件名（"xx"是csv文件使用中的字符串占位符）
		if (/*NULL != pkAIData &&*/ kScriptName != "" && kScriptName != "xx")
		{
			SM_ScriptCtrlNamePtrMapIter iter = m_kScriptCtrlNamePtrMap.find(kScriptName);	//防止重入
			if(iter == m_kScriptCtrlNamePtrMap.end())
			{
				SM_ServerNPCBaseController *pkServerController = T_NEW_D SM_ServerNPCBaseController;
				if (pkServerController->InitScriptData(kScriptName) )	//如果加载脚本成功
				{
					m_kScriptCtrlNamePtrMap[kScriptName] = pkServerController;
				}
				else
					T_SAFE_DELETE(pkServerController);
			}
		}

		iter.MoveNext();
	}

	return true;
}

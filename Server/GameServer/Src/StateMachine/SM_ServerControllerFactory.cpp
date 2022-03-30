
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
	*�������е�AI���ƽű�
	*/
	CSVFile::CF_MonsterAIFunc* pkAIList = CSVFile::CF_MonsterAIFunc::GetSingletonPtr();
	CSVFile::CF_MonsterAIFunc::DataEntryMapExternIterator iter = pkAIList->GetIterator(); 
	while (iter.HasMoreElements())
	{
		CSVFile::CF_MonsterAIFunc::DataEntry* pkAIData = iter.PeekNextValuePtr();
		const StringType & kScriptName = pkAIData->_kAIControlScriptPath;
		//������ڽű��ļ�����"xx"��csv�ļ�ʹ���е��ַ���ռλ����
		if (/*NULL != pkAIData &&*/ kScriptName != "" && kScriptName != "xx")
		{
			SM_ScriptCtrlNamePtrMapIter iter = m_kScriptCtrlNamePtrMap.find(kScriptName);	//��ֹ����
			if(iter == m_kScriptCtrlNamePtrMap.end())
			{
				SM_ServerNPCBaseController *pkServerController = T_NEW_D SM_ServerNPCBaseController;
				if (pkServerController->InitScriptData(kScriptName) )	//������ؽű��ɹ�
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

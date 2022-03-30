#include "ClientPCH.h"
#include "LC_ClientDialogManager.h"
#include "LC_ClientDialogInfo.h"
LC_ClientDialogManager::LC_ClientDialogManager()
{
	m_kCharDialogInfoMap.clear();
}

LC_ClientDialogManager::~LC_ClientDialogManager()
{
// 	CharDialogInfoMapIter BegIter = m_kCharDialogInfoMap.begin();
// 	for (;BegIter != m_kCharDialogInfoMap.end();)
// 	{
// 		LC_ClientDialogInfo* pkInfo = (LC_ClientDialogInfo*)(BegIter->second);
// 		BegIter = m_kCharDialogInfoMap.erase(BegIter);
// 		T_SAFE_DELETE(pkInfo);
// 	}
	CharDialogInfoListIter Iter = m_kCharDialogList.begin();
	for (;Iter !=  m_kCharDialogList.end() ;)
	{
		LC_ClientDialogInfo* pkInfo = (LC_ClientDialogInfo*)(*Iter);
		T_SAFE_DELETE(pkInfo);
		Iter = m_kCharDialogList.erase(Iter);
	}

}

LC_ClientDialogInfo* LC_ClientDialogManager::AllocateNPCDialogInfo()
{
	LC_ClientDialogInfo* pkInfo = T_NEW_D LC_ClientDialogInfo;
	m_kCharDialogList.push_back(pkInfo);
	return pkInfo;
}

LC_ClientDialogInfo* LC_ClientDialogManager::GetNPCDialogInfo( long lNPCID )
{
	CharDialogInfoMapIter Iter = m_kCharDialogInfoMap.find(lNPCID);
	if (m_kCharDialogInfoMap.end() != Iter)
	{
		LC_ClientDialogInfo* pkInfo = (LC_ClientDialogInfo*)Iter->second;
		return pkInfo;
	}
	return NULL;
}

void LC_ClientDialogManager::AddNPCDialogInfo( long lNPCID,LC_ClientDialogInfo* pkDialogInfo )
{
	if (pkDialogInfo == NULL)
		return;

	CharDialogInfoMapIter Iter = m_kCharDialogInfoMap.find(lNPCID);
	if (m_kCharDialogInfoMap.end() == Iter)
		m_kCharDialogInfoMap[lNPCID] = pkDialogInfo;
}

bool LC_ClientDialogManager::IsExistAccpetTaskDialog( long lNPCID,long lTaskID )
{
	LC_ClientDialogInfo* pkNPCDialogInfo	= GetNPCDialogInfo(lNPCID);
	if (pkNPCDialogInfo == NULL)
		return false;

	int iSize = pkNPCDialogInfo->GetDialogSetCount();
	for (int iDialogSetIndex = 0;iDialogSetIndex < iSize;iDialogSetIndex++)
	{
		LC_DialogSet* pkDialogSet = pkNPCDialogInfo->GetDialogSet(iDialogSetIndex);
		if (pkDialogSet == NULL)
			continue;

		if (pkDialogSet->GetParamType() != LC_DialogSet::DST_TASK)
			continue;
	
		if (lTaskID != pkDialogSet->GetParam())
			continue;
		
		int iNodeNum = pkDialogSet->GetDialogNodeCount();
		for (int iDialogNodeIndex = 0;iDialogNodeIndex < iNodeNum;iDialogNodeIndex++)
		{
			LC_DialogNodeData* pkNode = pkDialogSet->GetDialogNodeData(iDialogNodeIndex);
			if (pkNode == NULL)
				continue;
			
			int iDialogEvent = pkNode->GetDialogEvent();
			if (iDialogEvent&LC_DialogNodeData::DIALOG_EVENT_SHOW_TASK_ACCEPT_VIEW)
				return true;
		}
	}

	return false;
}

bool LC_ClientDialogManager::IsExistFinishTaskDialog( long lNPCID,long lTaskID )
{
	return false;
}
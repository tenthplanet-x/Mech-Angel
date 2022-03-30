#include "ClientPCH.h"
#include "LC_ClientDialogInfo.h"

LC_DialogNodeData::LC_DialogNodeData()
{
	Reset();
}

LC_DialogNodeData::~LC_DialogNodeData()
{

}

void LC_DialogNodeData::Reset()
{
	m_iDialogEvent		= DIALOG_EVENT_NONE;
	m_iDialogEventParam = 0;
	m_iDialogShowLimit	= DIALOG_SHOWLIMIT_ALWAYS;
	m_strDialogTitle	= "";
	m_strDialogContent	= "";

	memset(m_iDialogSelectIndex,0,sizeof(m_iDialogSelectIndex));
}

int LC_DialogNodeData::GetDialogSelectIndex( int index ) 
{
	if (index < 0 || index>= MAX_DAILOG_SELECT_ITEM_COUNT)
		return 0;

	return m_iDialogSelectIndex[index];
}

void LC_DialogNodeData::SetDialogSelectIndex( int index, int iSelectIndex )
{
	if (index < 0 || index>= MAX_DAILOG_SELECT_ITEM_COUNT)
		return;

	m_iDialogSelectIndex[index]	= iSelectIndex;
}

LC_DialogSet::LC_DialogSet()
{

}

LC_DialogSet::~LC_DialogSet()
{

}

LC_DialogNodeData* LC_DialogSet::GetDialogNodeData( int iIndex ) 
{
	if (iIndex < 0 || iIndex>= (int)m_kDialogList.size())
		return NULL;

	return	&m_kDialogList[iIndex];
}

void LC_DialogSet::AddDialogNodeData( LC_DialogNodeData& rDialogNode )
{
	m_kDialogList.push_back(rDialogNode);
}

int LC_DialogSet::GetDialogNodeCount()
{
	return (int)m_kDialogList.size();
}

LC_ClientDialogInfo::LC_ClientDialogInfo()
{
	m_kDialogInfo.clear();
}

LC_ClientDialogInfo::~LC_ClientDialogInfo()
{
	CharDialogSetListIter iter = m_kDialogInfo.begin();
	for (; iter != m_kDialogInfo.end(); )
	{
		LC_DialogSet* pkSet = (LC_DialogSet*)(*iter);
		iter = m_kDialogInfo.erase(iter);
		T_SAFE_DELETE(pkSet)
	}
}

LC_DialogSet* LC_ClientDialogInfo::AllocateDialogSet()
{
	LC_DialogSet* pkDialogSet = T_NEW_D LC_DialogSet;
	//m_kAllocateDialogSet.push_back(pkDialogSet);
	return pkDialogSet;
}

void LC_ClientDialogInfo::AddDialogSet( LC_DialogSet* pkDialogSet )
{
	m_kDialogInfo.push_back(pkDialogSet);
}

LC_DialogSet* LC_ClientDialogInfo::GetDialogSet( int iIndex )
{
	if (iIndex < 0 || iIndex>= (int)m_kDialogInfo.size())
		return NULL;

	CharDialogSetListIter iter = m_kDialogInfo.begin();
	for (int i = 0; i < m_kDialogInfo.size();i++ )
	{
		if (i == iIndex)
			return (LC_DialogSet*)(*iter);
		++iter;
	}
	return	NULL;
}

int LC_ClientDialogInfo::GetDialogSetCount()
{
	return (int)m_kDialogInfo.size();
}


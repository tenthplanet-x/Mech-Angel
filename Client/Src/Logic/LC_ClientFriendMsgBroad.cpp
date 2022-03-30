#include "ClientPCH.h"

#include "LC_ClientFriendMsgBroad.h"
#include "CF_FriendMsgBroadContent.h"

using namespace CSVFile;

LC_ClientFriendMsgBroad::LC_ClientFriendMsgBroad()
{

}

LC_ClientFriendMsgBroad::~LC_ClientFriendMsgBroad()
{

}

void LC_ClientFriendMsgBroad::ReUse()
{
	m_kFriendIDMsgBroadMap.clear();
}

void LC_ClientFriendMsgBroad::AddMsg(long lFriendID, int iMsgType)
{
	CF_FriendMsgBroadContent* pkFriendMsgBroadContent = CF_FriendMsgBroadContent::GetSingletonPtr();
	if (NULL == pkFriendMsgBroadContent)
	{
		return;
	}

	CF_FriendMsgBroadContent::DataEntry* pkDataEntry = pkFriendMsgBroadContent->GetEntryPtr(iMsgType);
	if (NULL == pkDataEntry)
	{
		return;
	}

	LC_ClientFriendMsgBroadStruct kInfo;

	SYSTEMTIME kTime;
	GetLocalTime(&kTime);

	char szTime[16] = {0};
	sprintf_s(szTime, sizeof(szTime), "%2d:%2d", kTime.wHour, kTime.wMinute);

	kInfo.SetTime(szTime);
	kInfo.SetTitle(pkDataEntry->_strTitle);
	kInfo.SetContent(pkDataEntry->_strContent);

	mapFriendIDMsgBroad::iterator it = m_kFriendIDMsgBroadMap.find(lFriendID);

	if ( it != m_kFriendIDMsgBroadMap.end())
	{
		vecClientFriendMsgBroad& rFriendMsgBroad = it->second;
		rFriendMsgBroad.push_back(kInfo);
	}
	else
	{
		vecClientFriendMsgBroad rFriendMsgBroad;
		m_kFriendIDMsgBroadMap[lFriendID] = rFriendMsgBroad;
		m_kFriendIDMsgBroadMap[lFriendID].push_back(kInfo);
	}
}

void LC_ClientFriendMsgBroad::Delete(long lFriendID)
{
	mapFriendIDMsgBroad::iterator it = m_kFriendIDMsgBroadMap.find(lFriendID);
	if (it != m_kFriendIDMsgBroadMap.end())
	{
		m_kFriendIDMsgBroadMap.erase(it);
	}
}

void LC_ClientFriendMsgBroad::DeleteMsg(long lFriendID, int iIndex)
{
	mapFriendIDMsgBroad::iterator it = m_kFriendIDMsgBroadMap.find(lFriendID);

	if (it != m_kFriendIDMsgBroadMap.end())
	{
		vecClientFriendMsgBroad& rFriendMsgBroad = it->second;
		if (iIndex < rFriendMsgBroad.size())
		{
			rFriendMsgBroad.erase(rFriendMsgBroad.begin() + iIndex);
		}

		if (rFriendMsgBroad.size() == 0)
		{
			m_kFriendIDMsgBroadMap.erase(it);
		}
	}
}

int LC_ClientFriendMsgBroad::GetMsgCount(long lFriendID)
{
	int iMsgCount = 0;

	mapFriendIDMsgBroad::iterator it = m_kFriendIDMsgBroadMap.find(lFriendID);

	if (it != m_kFriendIDMsgBroadMap.end())
	{
		vecClientFriendMsgBroad& rFriendMsgBroad = it->second;

		iMsgCount = static_cast<int>(rFriendMsgBroad.size());
	}

	return iMsgCount;
}

LC_ClientFriendMsgBroadStruct* LC_ClientFriendMsgBroad::GetMsg(long lFriendID, int iIndex)
{
	LC_ClientFriendMsgBroadStruct* pkInfo = NULL;

	mapFriendIDMsgBroad::iterator it = m_kFriendIDMsgBroadMap.find(lFriendID);

	if (it != m_kFriendIDMsgBroadMap.end())
	{
		vecClientFriendMsgBroad& rFriendMsgBroad = it->second;
		if (iIndex < rFriendMsgBroad.size())
		{
			pkInfo = &( rFriendMsgBroad.at(iIndex) );
		}
	}

	return pkInfo;
}


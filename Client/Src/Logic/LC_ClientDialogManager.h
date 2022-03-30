#ifndef _LC_CLIENT_DIALOG_MANAGER_H
#define _LC_CLIENT_DIALOG_MANAGER_H

class LC_ClientDialogInfo;
class LC_ClientDialogManager : public Memory::MM_BaseObject,
							   public Utility::UT_Singleton<LC_ClientDialogManager>
{
public:
	LC_ClientDialogManager();
	~LC_ClientDialogManager();
	DECL_RTTI_NAME(LC_ClientDialogManager);

	LC_ClientDialogInfo* AllocateNPCDialogInfo();
	LC_ClientDialogInfo* GetNPCDialogInfo(long lNPCID);
	void	AddNPCDialogInfo(long lNPCID,LC_ClientDialogInfo* pkDialogInfo);
	bool    IsExistAccpetTaskDialog(long lNPCID,long lTaskID);
	bool    IsExistFinishTaskDialog(long lNPCID,long lTaskID);

protected:
	typedef mem::map<long,LC_ClientDialogInfo*>		CharDialogInfoMap;
	typedef mem::list<LC_ClientDialogInfo*>			CharDialogInfoList;
	typedef CharDialogInfoMap::iterator				CharDialogInfoMapIter;
	typedef CharDialogInfoList::iterator			CharDialogInfoListIter;
	CharDialogInfoMap		m_kCharDialogInfoMap;
	CharDialogInfoList      m_kCharDialogList;
};
#endif
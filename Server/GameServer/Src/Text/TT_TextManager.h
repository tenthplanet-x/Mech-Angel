#ifndef _TT_TEXT_MANAGER_H
#define _TT_TEXT_MANAGER_H
#include "MM_BaseObject.h"
#include "UT_Singleton.h"
#include "UT_Lock.h"

class TT_TextManager : public Memory::MM_BaseObject,
						public Utility::UT_Singleton<TT_TextManager>
{
public:
	TT_TextManager();
	virtual ~TT_TextManager();
	
	void	Update(float fCurrentTime);

	void	AddServerAnnoceMsg(const StringType& rkMsg);
protected:
	Utility::UT_Lock	m_kLock;
	TStringList			m_kServerAnnoceMsgList;
};

#endif
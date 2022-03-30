#ifndef _LC_WILDPRINCES_MANAGER_H_ 
#define _LC_WILDPRINCES_MANAGER_H_

#include "SystemError.h"
#include "MM_BaseObject.h"
#include "UT_Singleton.h"

class CWildPrincesManager : public Memory::MM_BaseObject, public Utility::UT_Singleton<CWildPrincesManager>
{
public:
	typedef mem::map<uint32_t, mem::vector<uint32_t> > MapIdAndBossIdMap;
	typedef mem::map<uint32_t, MapIdAndBossIdMap> TypeIDAndMapInfoMap;

	CWildPrincesManager();
	~CWildPrincesManager();

	void Init();
	void Unit();

	ResultType CheckMapBossIDVaild(uint32_t type, uint32_t mapID, uint32_t bossID);
protected:
private:
	TypeIDAndMapInfoMap m_WildPrincesMapInfo;
};


#endif

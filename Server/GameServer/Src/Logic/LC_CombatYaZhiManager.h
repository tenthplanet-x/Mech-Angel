#ifndef _LC_COMBATYAZHIMANAGER_H_
#define _LC_COMBATYAZHIMANAGER_H_

#include "LC_Define.h"

struct YaZhiData
{
	YaZhiData(float pvp, float pve) : YaZhi_PVP(pvp), YaZhi_PVE(pve){}
	float YaZhi_PVP;
	float YaZhi_PVE;
};

class CYaZhiManager
{
public:
	CYaZhiManager();
	~CYaZhiManager();

	void Load_CombatYaZhi();
	float FetchCombatYaZhiData(int64_t srcCombatScore, int64_t destCombatScore, int yazhiType);
protected:
private:
	mem::map<float, YaZhiData> m_combatYaZhiInfoMap;
};

#endif

//服务器地图级逻辑对象创建消耗管理
#ifndef _LC_SERVER_MAP_LOGIC_FACTORY_H
#define _LC_SERVER_MAP_LOGIC_FACTORY_H

#include "UT_MemoryPool.h"
#include "LC_MapLogicFactoryBase.h"
#include "LC_ServerMapLogic_NewBee.h"
#include "LC_ServerMapLogic_BatchPlay.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_ServerMapLogic_RankPlace.h"
#include "LC_ServerMapLogic_WatchTowerPlace.h"
#include "LC_ServerMapLogic_GuildMapPlace.h"
#include "LC_ServerMap.h"
#include "LC_ServerSector.h"

template <int mapsize> 
class LC_ServerMap_Template;
namespace DungeonConfig {struct Logic_ConfigData_Base;};

class LC_ServerMapLogicFactory : public GameLogic::LC_MapLogicFactoryBase
{
public:
	LC_ServerMapLogicFactory();
	~LC_ServerMapLogicFactory();

	virtual void Init();
	virtual void UnInit();
	virtual void Update(float fCurrentTime);

	static DungeonConfig::Logic_ConfigData_Base* ReadConfig(int iLogicPlayType,const StringType& configFileName);

	virtual LC_ServerMapLogic_Place* RequestMapLogic(int iMapLogicType);
	virtual void ReleaseMapLogic(LC_ServerMapLogic_Place* pkMapLogic);

	LC_ServerMap* RequestMap(GameLogic::LC_MapSizeType mapsize);
	void ReleaseMap(LC_ServerMap* pkServerMap);

	LC_ServerMapLogic_Batch* RequestBatch();
	void ReleaseBatch(LC_ServerMapLogic_Batch* batch);

	LC_ServerSector * RequestSector();
	void ReleaseSector(LC_ServerSector * s);

protected:
	
	Utility::UT_MemoryPool<LC_ServerMapLogic_Place>*								m_pkMapLogicPlacePool;
	Utility::UT_MemoryPool<LC_ServerMapLogic_NewBee>*								m_pkMapLogicNewBeePool;
	Utility::UT_MemoryPool<LC_ServerMapLogic_Batch>*								m_pkMapLogicBatchPool;
	Utility::UT_MemoryPool<LC_ServerMapLogic_RankPlace>*							m_pkMapLogicRankPlacePool;
	Utility::UT_MemoryPool<LC_ServerMapLogic_WatchTowerPlace>*						m_pkMapLogicWatchTowerPlacePool;
	Utility::UT_MemoryPool<LC_ServerMapLogic_GuildMapPlace>*						m_pkMapLogicGuildMapPlacePool;
	Utility::UT_MemoryPool<LC_ServerMapLogic_GuildStoryMapPlace>*						m_pkMapLogicGuildStoryMapPlacePool;

	//地图容器
	Utility::UT_MemoryPool<LC_ServerMap_Template<0> >*								m_pkMap_1_1_Pool;
	Utility::UT_MemoryPool<LC_ServerMap_Template<1> >*								m_pkMap_4_4_Pool;
	Utility::UT_MemoryPool<LC_ServerMap_Template<2> >*								m_pkMap_8_8_Pool;
	Utility::UT_MemoryPool<LC_ServerMap_Template<3> >*								m_pkMap_16_16_Pool;
	Utility::UT_MemoryPool<LC_ServerMap_Template<4> >*								m_pkMap_32_32_Pool;
	Utility::UT_MemoryPool<LC_ServerMap_Template<5> >*								m_pkMap_48_48_Pool;
	Utility::UT_MemoryPool<LC_ServerSector>*										m_pkServerSector_Pool;
};
#endif
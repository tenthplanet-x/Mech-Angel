#ifndef _LC_SERVER_WORLD_MANAGER_H
#define _LC_SERVER_WORLD_MANAGER_H

class LC_ServerMap;
class LC_ServerPlayer;
class LC_ServerMapLogic_TranscriptionBase;

#include "LC_ServerSector.h"
#include "LC_ServerMapLogic_Place.h"
#include "LC_WorldManagerBase.h"

class LC_ServerWorldManager : public GameLogic::LC_WorldManagerBase
{
public:
	LC_ServerWorldManager();
	~LC_ServerWorldManager();

	virtual bool Init();
	virtual bool UnInit();
	virtual void Update(float fCurrentTime, float fDeltaTime);

	LC_ServerMapLogic_Place* GetMapLogic(int32_t lMapLogicID);
	inline int32_t GetMapLogicsize(void) { return (int32_t)m_kLogicID2Logicmap.size(); }
	LC_ServerMapLogicPlaceVector* GetMapLogicByRes(int32_t iMapResID);
	LC_ServerMapLogic_Place* GetMapLogicByResAndIndex(int32_t iMapResID, int iTributaryIndex);
	LC_ServerMapLogic_Place* FindProperMapLogicByRes(int32_t iMapResID, bool bEnableTributary);
	bool	DelayMapLogicAutoRelease(int32_t lMapLogicID);		/// 延迟MapLogic自动销毁时间

	void ReportOrigMapLineInfo();
	void SetGlobalMapLogicId_Lite( int32_t nServerNodeId, int32_t nOrigLogicId, int32_t nMapResId, int32_t nGlobalLine );

	bool IsNewbee(LC_ServerPlayer* player);
	LC_ServerMapLogic_Place* FindFreeNewbeeMapLogic(int32_t iMapResID, bool bEnableTributary);
	void BroadCast(int32_t iMsgType, Protocol::MG_BODY* pkMsgBody, bool bGlobal = false, int32_t nServerId = 0, uint16_t nRouterFlag = MG_SERVER_ROUTER_FLAG_SINGLE);

	void UpdateTributaryRecords(int resMapID, int index);

	ActiveSectorClusterVector& SetUpActiveSector(void);
	inline size_t ActiveSectorClusSize(void) const { return m_uiActiveSectorClusSize; }

	ResultType CheckLeaveServerWorld(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, float fOfflineTime);		//是否可以离开
	ResultType RequestEnterServerWorld(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, MapType eTargetMapLogicType,
									   int lTargetMapLogicID, int iTargetMapResID, const Utility::UT_Vec3Int&	rkTargetLocation);	//进入世界
	ResultType RequestChangePlayerLocation(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, int lTargetMapLogicID,
										   int iTargetMapResID, const Utility::UT_Vec3Int&	rkTargetLocation, int lineNum = 0, bool bCertainSameServer = false);	//改变玩家的位置, bCertainSameServer 确定是同服务器内传送
	ResultType RequestLeaveServerWorld(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType);	//退出世界
	// _ActiveBuddyIds;    //出战buddyID限制    出战buddyID限制（没有限制则不填）
	bool CheckAllowToFightBuddyId(int buddyId,Utility::UT_SIMDataList&_ActiveBuddyIds);
	void QuitTranscriptionOp(LC_ServerPlayer* pkPlayer); // quit 副本 相关结算信息
	LC_ServerMapLogic_Place* CreateServerMapLogic(int lMapLogicID, int lMapResID, int lMapLogicType, int32_t lineNum=0, bool start = true);		//创建地图逻辑对象
	void ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent);
	void ForAllPlayer(GameLogic::LC_ActorOperator* oper);
	void ForAllNPC(GameLogic::LC_ActorOperator* oper);
	void GetLogicMapIDListFromResID(GameLogic::LC_IDList& kLogicList, int32_t iResID);
	inline bool IsTownMap(int32_t iMapResID) const {return m_townmaps.find(iMapResID) != m_townmaps.end();	}
	bool IsTributaryMap(LC_ServerMapLogic_Place* pkPlace) const;
	bool HasTributaryMap(int lMapResID) const;
	LC_ServerMapLogic_Place* ForceCreateTributaryMap(int lMapResID);
	LC_ServerMapLogic_Place* FindProperTributaryMap(int32_t iMapResID);
	int32_t		CalcNewTributaryMapLineNo(LC_ServerMapLogicPlaceVector* pkMapLogicVec);		/// 获取新分线地图的分线编号（本地编号）

	bool RestartMapLogicByRes( int32_t nMapResId );

	bool FindGuildMapBossLogicMap(int32_t mapID, uint64_t guildID, LC_ServerMapLogicPlaceVector& logicPlaceVec);

protected:
	ResultType _requestChangePlayerLocationByRecover(LC_ServerPlayer* pkPlayer);
	ResultType _requestChangePlayerLocationByOffLine(LC_ServerPlayer* pkPlayer);
	ResultType _requestChangePlayerLocationByLogic(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, int lTargetMapLogicType, int lTargetMapLogicID, int iTargetMapResID, const Utility::UT_Vec3Int& rkTargetLocation, int lineNum, bool bCertainSameServer);
	ResultType _requestChangePlayerLocationByFirstEnterRaid(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, int lTargetMapLogicType, int lTargetMapLogicID, int iTargetMapResID, const Utility::UT_Vec3Int& rkTargetLocation);
	ResultType _requestChangePlayerLocationToDefault(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType);

	LC_ServerMapLogic_Place* _findProperMapLogic(LC_ServerPlayer* pkPlayer, GameLogic::LC_MapInOutType eInOutType, int lMapLogicType, int lMapLogicID, int lMapResID, int lineNum=0);
	LC_ServerMapLogic_Place* _createServerMapLogic(int lMapLogicID, int lMapLogicType, int lMapResID, CF_WorldMapList::DataEntry* pkData, bool start, int lineNum=0);
	void _releaseServerMapLogic(LC_ServerMapLogic_Place* pkMapLogic);

	LC_ServerMap* _createServerMap(GameLogic::LC_MapSizeType eSizeType, bool bGlobalMode = false);
	void _freeServerMap(LC_ServerMap* pkServerMap);

	ResultType _addMapLogic(LC_ServerMapLogic_Place* pkPlace);
	ResultType _removeMapLogic(int32_t iMapLogicID);

	bool _checkWarPlayRun(LC_ServerMapLogic_Place* pkMapLogic);
public:
	bool _checkAllowGlobalMapLine( CF_WorldMapList::DataEntry* pData, int32_t nMapLogicType );
	void _unregisterGlobalMapLine(LC_ServerMapLogic_Place* pData);
	void UpdateAllMapLineTologin();
protected:

	ActiveSectorClusterVector		m_kActiveSectorClusVector;
	size_t m_uiActiveSectorClusSize;

	LC_ServerMapLogicIDPtrMap		m_kLogicID2Logicmap; // 实例id 映射 value  LC_ServerMapLogic_Place
	LC_ServerMapMapLogicIndices		m_kLogicMapIndices;	 // 模板id 管理vector LC_ServerMapLogic_Place
	LC_ServerMapTributaryRecords	m_kTributaryRecords;

	bool	m_bInitializing;						// 正在初始化状态
	int32_t m_iMapLogicAllocID;
	
	GameLogic::LC_IDSet m_kNewBeeResIDs;      // 新手分流 地图id 集合
	GameLogic::LC_IDSet m_kTributaryMapResIDs;  // 可以分流的地图id 集合（）

	mem::set<int> m_townmaps;
	Utility::UT_Timer m_kUpdateTimer;
	Utility::UT_Timer m_kTribuMapTimer;

	// map-line mapping
	typedef mem::map< uint64_t, int32_t > MAP_LINE_TYPE;
	typedef MAP_LINE_TYPE::iterator MAP_LINE_TYPE_IT;
	MAP_LINE_TYPE m_mapLocalResLineData;			// 初始化时的所有地图分线信息，等启动完一次性发给LoginSvr

	//key: { mapResId }
	//value: 
	//	 {
	//		 vector
	//			 [
	//		 {
	//			 globalLineIndex 
	//		 }
	//			 ]
	//	 }
};

#endif


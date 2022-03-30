#ifndef _LC_SERVER_MAP_H
#define _LC_SERVER_MAP_H

#include "LC_MapBase.h"
#include "LC_ServerPlayer.h"
#include "LC_ServerPlaceHolder.h"
#include "LC_ServerMap_Trigger.h"
#include "LC_ServerSector.h"

class LC_ServerMap : public GameLogic::LC_MapBase
{
public:
	LC_ServerMap(GameLogic::LC_MapSizeType eMapSizeType);
	virtual ~LC_ServerMap();

	virtual void    Init();
	virtual void    UnInit();

	virtual void	Update(float fCurrentTime,float fDeltaTime);
	virtual void	ChangeSector(GameLogic::LC_LogicObject* pkLogicObject );
	virtual bool	RemoveObject(GameLogic::LC_LogicObject* pkObject);
	
	void			BroadCastToArea(SectorIndex dwSectorIndex, uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter=NULL);
	void			BroadCastToServer(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter=NULL);
	void			BroadCastToAll(uint16_t usMsgType, Protocol::MG_BODY* pkMsgBody = NULL, GameLogic::LC_PlayerOperator* pkFilter=NULL);

	void			GetAreaPlayerList(SectorIndex dwSectorIndex, LC_PlayerPtrList& playerList);
	void			GetAreaPlayerList(SectorIndex dwSectorIndex, mem::vector<const GameLogic::LC_PlayerIDPtrMap*>& playersVec);

	void			FillActivePlayerSector(LC_ServerSectorVector& activePlayerSectors);
	//activeNPCSectors 是个传入传出参数，保证activeNPCSectors的开始部分与activePlayerSectors一置
	void			FillActiveNpcSector(LC_ServerSectorVector& activeNPCSectors);
	void			FillActiveSector(ActiveSectorCluster& activeSectorClus);

	void			ProcessGameEvent(GameLogic::LC_GameEvent* pkEvent);

	virtual	void	GetAllPlaceHolder(PlaceHolderListType& pkList);	

	//触发器
	LC_ServerMap_Trigger* AddCircleTrigger(Utility::UT_Circle& circle,LC_ServerMapLogic_Place* maplogic,LC_ServerMap_Trigger::TriggerCallBack callback=LC_ServerMap_Trigger::TriggerCallBack());
	LC_ServerMap_Trigger* AddRectTrigger(Utility::UT_OBB& obb,LC_ServerMapLogic_Place* maplogic,LC_ServerMap_Trigger::TriggerCallBack callback=LC_ServerMap_Trigger::TriggerCallBack());
	LC_ServerMap_Trigger* GetTrigger(int id);
	void DelTrigger(int id);

	void SetGlobalMode( bool bMode );

	/*void AddTrigger2Sector(LC_ServerMap_Trigger* trigger);
	void DelTriggerInSector(LC_ServerMap_Trigger* trigger);
	void GetSectorTrigger(int sectorindex,mem::vector<LC_ServerMap_Trigger*>& result);
	int GetAllTriggerSectorPlayerCount(LC_ServerMap_Trigger* trigger);*/

protected:
	GameLogic::LC_SectorBase*	_findDirtySector(GameLogic::SectorIndex iIndex);
	void						_removeDirtySector(GameLogic::SectorIndex iIndex);
	void						_addDirtySector(GameLogic::LC_SectorBase* pkSector);
	void						_updateDirtySector();

	typedef mem::map<GameLogic::SectorIndex,GameLogic::LC_SectorBase*>	LC_DirtySectorIndexPtrMap;
	typedef LC_DirtySectorIndexPtrMap::iterator							LC_DirtySectorIndexPtrMapIter;

	LC_DirtySectorIndexPtrMap											m_kDirtySectorIndexPtrMap;
	float																m_fLastUpdateTime;
	float																m_fTriggerLastUpdateTime;

private:
	//触发器
	mem::map<int,LC_ServerMap_Trigger*>									m_AllTrigger;
	mem::set<int>														m_DelTrigger;
	int																	m_TriggerIdBegin;
	bool m_bGlobalMode;
};

template<int mapsize>
class LC_ServerMap_Template:public LC_ServerMap
{
public:
	LC_ServerMap_Template():LC_ServerMap((GameLogic::LC_MapSizeType)mapsize)
	{}
};

#endif
#ifndef _LC_SERVER_CHEST_MANAGER_H
#define _LC_SERVER_CHEST_MANAGER_H

#include "LC_PlaceHolderManagerBase.h"
#include "MG_PlaceHolderProtocol.h"
#include "LC_ServerPlaceHolder.h"
#include "UT_MemoryPool.h"
#include "UT_TimeManager.h"

class LC_ServerPlayer;
class LC_ItemFlopInfoList;
class LC_ItemFlopInfo;

typedef mem::vector<UT_Vec3Int> PosListType;


class LC_ServerPlaceHolderManager : public GameLogic::LC_PlaceHolderManagerBase , public UT_Singleton<LC_ServerPlaceHolderManager>
{
public:
	LC_ServerPlaceHolderManager();
	~LC_ServerPlaceHolderManager();


	virtual void			Update(float fCurrentTime,float fDeltaTime);
	void					CreatePlaceHolderByItemFlopInfoList(LC_ItemFlopInfoList& rkInfoList,bool bPosition_Fixed	=	false);
	void					CreatePlaceHolderByItemFlopInfoList(LC_ItemFlopInfoList& rkInfoList,mem::list<object_id_type>& kPlaceHolderIDList,bool bPosition_Fixed	=	false);
	
	int32_t				AddPlaceHolderList(int res_map_id , PlaceHolderListType& list , const UT_Vec3Int& pos, int32_t randIndex);
	
	void					Init();


protected:
	bool										_createPlaceHolderByItemFlopInfo(LC_ItemFlopInfo* pkFlopInfo,bool bPosition_Fixed	=	false);
	bool										_createPlaceHolderByItemFlopInfo(LC_ItemFlopInfo* pkFlopInfo,	mem::list<object_id_type>& kPlaceHolderIDList,bool bPosition_Fixed	=	false);

	LC_ServerPlaceHolder*								_findPlaceHolder(object_id_type lPlaceHolderID);
	virtual GameLogic::LC_PlaceHolderBase*			_allocPlaceHolder();
	virtual void								_freePlaceHolder(GameLogic::LC_PlaceHolderBase* pkPlaceHolder);


	Utility::UT_MemoryPool<LC_ServerPlaceHolder>		m_kServerPlaceHolderPool;
	PosListType	m_DropPlaceHolderPosList_;

	Utility::UT_Timer	m_kUpdateTimer;
};
#endif
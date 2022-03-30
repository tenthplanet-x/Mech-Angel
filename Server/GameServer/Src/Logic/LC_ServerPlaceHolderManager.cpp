#include "LC_ServerPlayer.h"
#include "LC_ServerPlaceHolder.h"
#include "LC_ServerPlaceHolderManager.h"
#include "LC_ServerItemFlopManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerLogicManager.h"
#include "GameServerApp.h"
#include "PF_ServerPathManager.h"
#include "UT_MathBase.h"
#include "LC_LogicIDManager.h"
#include "GlobalSettings.h"
using namespace GameLogic;
using namespace Utility;
using namespace Protocol;
//--------------------------------------------------------



LC_ServerPlaceHolderManager::LC_ServerPlaceHolderManager()
	: LC_PlaceHolderManagerBase()
{
	const Utility::MemPoolConfigEntry & plc = GetGlobalSetting.poolPlaceHolder;
	m_kServerPlaceHolderPool.Init(plc.InitialCount, plc.IncCount, "LC_ServerPlaceHolderManager.m_kServerPlaceHolderPool");
	m_kServerPlaceHolderPool.EnableAutoDec(plc.AutoDec, plc.AutoDecInterval, plc.AutoDecCount, plc.AutoDecMinCount);
	Init();

	m_kUpdateTimer.SetTimer(GET_INTERVAL(CMN_INTER_SLOW));
}
//--------------------------------------------------------
LC_ServerPlaceHolderManager::~LC_ServerPlaceHolderManager()
{
}
//--------------------------------------------------------
void LC_ServerPlaceHolderManager::Update(float fCurrentTime, float fDeltaTime)
{
	if (!m_kUpdateTimer.IsExpired())
	{
		return;
	}

	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();

	//更新宝箱，删除过期宝箱
	LC_PlaceHolderIDPtrMapIter IterBe = m_kPlaceHolderIDPtrMap.begin();
	LC_PlaceHolderIDPtrMapIter IterEd = m_kPlaceHolderIDPtrMap.end();

	for (; IterBe != IterEd;)
	{
		LC_PlaceHolderBase* pkPlaceHolder = (*IterBe).second;
		if (pkPlaceHolder->IsLifeEnd(fCurrentTime))
		{
			m_kPlaceHolderIDPtrMap.erase(IterBe++);

			pkWorldManager->RemoveObjectFromMap(pkPlaceHolder);
			_freePlaceHolder(pkPlaceHolder);
		}
		else
		{
			pkPlaceHolder->Update(fCurrentTime, fDeltaTime);
			++IterBe;
		}
	}


	//< 更新对象池
	m_kServerPlaceHolderPool.Update(fCurrentTime);
}
//--------------------------------------------------------
void LC_ServerPlaceHolderManager::CreatePlaceHolderByItemFlopInfoList(LC_ItemFlopInfoList& rkInfoList, bool bPosition_Fixed)
{
	int iFlopInfoCount = rkInfoList.GetItemFlopInfoCount();
	for (int iPlaceHolderIndex = 0; iPlaceHolderIndex < iFlopInfoCount; iPlaceHolderIndex++)
	{
		LC_ItemFlopInfo* pkFlopInfo = rkInfoList.GetItemFlopInfoByIndex(iPlaceHolderIndex);
		if (!pkFlopInfo)
			continue;
		if (pkFlopInfo->GetBackPackEntryList().size() <= 0)
			continue;

		pkFlopInfo->TidyContent();

		_createPlaceHolderByItemFlopInfo(pkFlopInfo, bPosition_Fixed);
	}
}
//--------------------------------------------------------
void LC_ServerPlaceHolderManager::CreatePlaceHolderByItemFlopInfoList(LC_ItemFlopInfoList& rkInfoList, mem::list<object_id_type>& kPlaceHolderIDList, bool bPosition_Fixed)
{
	int iFlopInfoCount = rkInfoList.GetItemFlopInfoCount();
	for (int iPlaceHolderIndex = 0; iPlaceHolderIndex < iFlopInfoCount; iPlaceHolderIndex++)
	{
		LC_ItemFlopInfo* pkFlopInfo = rkInfoList.GetItemFlopInfoByIndex(iPlaceHolderIndex);
		if (!pkFlopInfo)
			continue;
		if (pkFlopInfo->GetBackPackEntryList().size() <= 0)
			continue;

		pkFlopInfo->TidyContent();

		_createPlaceHolderByItemFlopInfo(pkFlopInfo, kPlaceHolderIDList, bPosition_Fixed);
	}
}
//--------------------------------------------------------
LC_ServerPlaceHolder* LC_ServerPlaceHolderManager::_findPlaceHolder(object_id_type lPlaceHolderID)
{
	LC_ServerPlaceHolder* pkPlaceHolder = (LC_ServerPlaceHolder*)FindPlaceHolder(lPlaceHolderID);
	if (!pkPlaceHolder)
		return NULL;
	else
		return pkPlaceHolder;
}
//--------------------------------------------------------
LC_PlaceHolderBase* LC_ServerPlaceHolderManager::_allocPlaceHolder()
{
	return m_kServerPlaceHolderPool.Alloc();
}
//--------------------------------------------------------
void LC_ServerPlaceHolderManager::_freePlaceHolder(LC_PlaceHolderBase* pkPlaceHolder)
{
	LC_LogicIDManager::GetSingletonPtr()->ReleaseLogicID(LOT_PLACEHOLDER, pkPlaceHolder->GetID());

	m_kServerPlaceHolderPool.Free((LC_ServerPlaceHolder*)pkPlaceHolder);
}
//--------------------------------------------------------
bool LC_ServerPlaceHolderManager::_createPlaceHolderByItemFlopInfo(LC_ItemFlopInfo* pkFlopInfo, bool bPosition_Fixed)
{
	LC_LogicIDManager* pkLogicManager		= LC_LogicIDManager::GetSingletonPtr();
	if (NULL == pkLogicManager)
		return false;

	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();

	object_id_type lPlaceHolderID = LC_LogicIDManager::GetSingletonPtr()->RequestLogicID(LOT_PLACEHOLDER);
	LC_ServerPlaceHolder* pkPlaceHolder	= (LC_ServerPlaceHolder*)CreatePlaceHolder(lPlaceHolderID);
	if (NULL == pkPlaceHolder)
	{
		return false;
	}

	float fCurrentTime = GET_PROCESS_TIME;
	pkPlaceHolder->SetLifeStartTime(fCurrentTime);
	pkPlaceHolder->SetLifeEndTime(fCurrentTime + pkFlopInfo->GetLifeTime());

	//设置所有者

	pkPlaceHolder->SetSpawnZRotAngle(pkFlopInfo->GetZRotAngle());
	pkPlaceHolder->SetFactionID(3);

	pkWorldManager->AddObjectToMap(pkFlopInfo->GetMapID(), pkPlaceHolder);

	return true;

}

//----------------------------------------------------------------------------------
bool LC_ServerPlaceHolderManager::_createPlaceHolderByItemFlopInfo(LC_ItemFlopInfo* pkFlopInfo,	mem::list<object_id_type>& kPlaceHolderIDList, bool bPosition_Fixed)
{
	LC_LogicIDManager* pkLogicManager		= LC_LogicIDManager::GetSingletonPtr();
	if (NULL == pkLogicManager)
	{
		return false;
	}
	object_id_type lPlaceHolderID = LC_LogicIDManager::GetSingletonPtr()->RequestLogicID(LOT_PLACEHOLDER);
	LC_ServerPlaceHolder* pkPlaceHolder = (LC_ServerPlaceHolder*)CreatePlaceHolder(lPlaceHolderID);
	if (NULL == pkPlaceHolder)
	{
		return false;
	}

	float fCurrentTime = GET_PROCESS_TIME;
	pkPlaceHolder->SetLifeStartTime(fCurrentTime);
	pkPlaceHolder->SetLifeEndTime(fCurrentTime + pkFlopInfo->GetLifeTime());

	return true;
}

int32_t	LC_ServerPlaceHolderManager::AddPlaceHolderList(int res_map_id , PlaceHolderListType& list , const UT_Vec3Int& pos,  int32_t randIndex)
{
	int32_t newRandIndex = randIndex;
	LC_ServerWorldManager* pkWorldManager	= SERVER_GET_WORLD_MANAGER();	
	PF_ServerPathManager* pathmanager = PF_ServerPathManager::GetSingletonPtr();
	if (NULL == pathmanager)
	{
		return newRandIndex;
	}

	UT_Vec3Int new_pos;

	size_t count = list.size();
	for(int i=0; i<count; ++i)
	{
		int tmp = 0;
		LC_ServerPlaceHolder* ph = list[i];
		UT_Vec3Int location;

		//找10次，找不到合法位置，就给重叠的位置
		do
		{
			randIndex += 1;
			tmp += 1;
			Utility::UT_Vec3Int ret = Utility::UT_MathBase::MakeSpiralMatrixPosition(randIndex);
			int delta_x = GetGlobalSetting.nDropDistance * ret.x;
			int delta_y = GetGlobalSetting.nDropDistance * ret.y;
			UT_Vec3Int index(pos.x + delta_x, pos.y + delta_y, pos.z);
			new_pos = index;
		} while(!pathmanager->IsPosValid(res_map_id, new_pos) && tmp<10);

		if(pathmanager->IsPosValid(res_map_id, new_pos))
		{
			location = new_pos;
		}
		else
		{
			if(10 == tmp)
			{
				GfxWriteLog(LOG_INFO_FILELINE, LOG_SWITCH_INFO, "AddPlaceHolderList times>10, not find vaild pos! serverid:%d,", GetGameServerID());
			}
			location = pos;			
		}

		ph->SetFromLocationPos(pos);
		ph->SetCurrentLocation(location);
		pkWorldManager->AddObjectToMap(ph->GetMapID(), ph);
	}

	newRandIndex = randIndex;

	return newRandIndex;
}

const int PlaceHolderDelta = 65;

void LC_ServerPlaceHolderManager::Init()
{
	m_DropPlaceHolderPosList_.push_back(UT_Vec3Int(0, 0, 0));
	for (int i = 1; i <= 130; ++i)
	{
		UT_Vec3Int pos = UT_MathBase::MakeSpiralMatrixPosition(i);
		pos.x *= PlaceHolderDelta;
		pos.y *= PlaceHolderDelta;
		m_DropPlaceHolderPosList_.push_back(pos);
	}
}
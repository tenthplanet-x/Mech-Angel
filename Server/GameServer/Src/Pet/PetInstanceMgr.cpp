#include "PetInstanceMgr.h"
#include "IdDefine.h"
#include "PetInstance.h"
#include "LC_ServerLogicManager.h"
#include "LC_ServerWorldManager.h"
#include "LC_ServerPlayer.h"
//#include "CF_Pet.h"
#include "CF_PetType.h"
#include "CF_PetUpgrade.h"
#include "MG_PetProtocol.h"
#include "../Buddy/BuddyHelper.h"
#include "CF_ItemGenMonsterDrop.h"
#include "UT_OperateLog.h"
using namespace OPERATELOG;

PetInstanceMgr::PetInstanceMgr()
	: m_nOwnerPlayerId(INVALID_ACTOR_ID)
	, m_nEnabledIndex(-1)
	, m_nMaxCombatScoreIndex(-1)
	, m_nMaxCombatScorePetID(0)
	, m_nActivateState(false)
{
	ReUse();
}

PetInstanceMgr::~PetInstanceMgr()
{

}

void PetInstanceMgr::Init(object_id_type nOwnerPlayerId)
{
	m_nOwnerPlayerId = nOwnerPlayerId;
}

void PetInstanceMgr::Clear()
{

}

ResultType PetInstanceMgr::Activate(int petID)
{
	bool tag = IsActivated();
	if(tag)
	{
		return RE_PET_ALREADY_ACTIVATE;
	}

	if(0 != petID)
	{
		int res = CapturePet(petID);
		if(res)
		{
			m_nActivateState = true;
		}
	}
	else
	{
		m_nActivateState = true;
	}
	

	return RE_SUCCESS;	
}

bool PetInstanceMgr::IsActivated()
{
	return m_nActivateState;
}

int PetInstanceMgr::CapturePet(int32_t nPetResId, bool bLottery)
{
	int nResult = false;
	MG_SC_PetCaptureResult result;
	result.m_nRetCode = RE_SUCCESS;
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
	{
		result.m_nRetCode = RE_PET_INVALID_PLAYER;
		goto Exit0;
	}

	PetInstance* pInstance = AddPet(nPetResId);
	if (pInstance == NULL)
	{
		if ( m_bDecompose )
		{
			// 这里可以定义类似“已分解”的错误码
			result.m_nRetCode = RE_FAIL;
		}
		else
		{
			if ( !bLottery)
			{
				result.m_nRetCode = RE_PET_CAPTURE_ERROR;
			}
			else
			{
				result.m_nRetCode = RE_FAIL;
			}
		}
		m_bDecompose = false;
		goto Exit0;
	}
	
	result.m_Data.nResId = pInstance->GetTemplateId();
	result.m_Data.nStarLevel = pInstance->GetStarLevel();
	result.m_Data.nIndex = pInstance->GetIndex();
	if (pOwnerPlayer->GetLotteryStat())
		result.m_nMode = ProductMode::eProductMode_Lottery;
	else
		result.m_nMode = ProductMode::eProductMode_Normal;

	{
		//顺序不能换
		pOwnerPlayer->GetCommanderAttrMgr().RefreshPetAttributeMap(pOwnerPlayer, false, SPART_Pet_1);
		TryRefreshMaxCombatScoreByTemplate(pInstance->GetIndex());
		pOwnerPlayer->UpdateRankUserValueByRankType(Rank_Type_Pet);
	}

	AchievementEvent kEvent;
	kEvent.SetNum(1);
	pOwnerPlayer->UpdateAchievementRecord(LC_ACHIEVEMENT_TYPE_PET_COUNT, kEvent);

	nResult = true;
Exit0:
		pOwnerPlayer->SendMsgToClient(MGPT_SC_PET_CAPTURE_RESULT, &result);
	if (nResult)
		pInstance->SyncAllDataToClient();

	return nResult;
}

int PetInstanceMgr::EnablePet(int16_t nIndex)
{
	MG_SC_PetOp_EnableRep rep;
	rep.m_nIndex = nIndex;
	int nResult = false;

	PetInstance* pInstance = GetPetInstance(nIndex);
	if (pInstance == NULL)
	{
		rep.m_nError = RE_PET_INVALID_INDEX	;
		goto Exit0;
	}

	LC_ServerPlayer* pOwner = GetOwnerPlayer();
	if ( pOwner == NULL )
	{
		rep.m_nError = RE_PET_INVALID_PLAYER;
		goto Exit0;
	}

	if ( nIndex == m_nEnabledIndex )
	{
		rep.m_nError = RE_PET_ALREADY_ENABLED;
		goto Exit0;
	}
	DisablePet( true);
	m_nEnabledIndex = nIndex;
	PetInstance* pNewInstance = GetEnabledPetInstance();
	if (pNewInstance == NULL)
	{
		rep.m_nError = RE_PET_INVALID_INSTANCE;
		goto Exit0;
	}

	nResult = true;
	rep.m_nError = RE_SUCCESS;
Exit0:
	pOwner->SendMsgToClient(MGPT_SC_PET_ENABLE_REP, &rep);
	return nResult;
}

int PetInstanceMgr::DisablePet(bool bAuto)
{
	int nResult = false;
	MG_SC_PetOp_DisableRep rep;
	rep.m_nIndex = m_nEnabledIndex;

	LC_ServerPlayer* pOwner = GetOwnerPlayer();
	if ( pOwner == NULL )
	{
		rep.m_nError = RE_PET_INVALID_PLAYER;
		goto Exit0;
	}

	if (m_nEnabledIndex == -1)
	{
		rep.m_nError = RE_PET_ENABLE_NONE;
		goto Exit0;
	}

	PetInstance* pInstance = GetPetInstance( m_nEnabledIndex );
	if (pInstance == NULL)
	{
		rep.m_nError = RE_PET_INVALID_INDEX;
		goto Exit0;
	}

	// DoDisable
	m_nEnabledIndex = -1;
	// broadcast.

	nResult = true;
	rep.m_nError = RE_SUCCESS;
Exit0:
	if (!bAuto)
	{
		pOwner->SendMsgToClient(MGPT_SC_PET_DISABLE_REP, &rep);
	}
	return nResult;
}

int PetInstanceMgr::Decompose(int16_t nIndex)
{
	int nResult = false;
	MG_SC_PetDecompose result;
	result.m_nErrorCode = RE_PET_INVALID_INSTANCE;
	result.m_nIndex = nIndex;

	if (nIndex == m_nEnabledIndex)
	{
		result.m_nErrorCode = RE_PET_ALREADY_ENABLED;
		goto Exit0;
	}

	LC_ServerPlayer* pOwner = GetOwnerPlayer();
	if ( pOwner == NULL )
	{
		result.m_nErrorCode = RE_PET_INVALID_PLAYER;
		goto Exit0;
	}

	//
	int nPackEmpty = pOwner->GetPackAsset()->GetPackEmptyEntryCount(PACK_TYPE_BASIC);
	if ( nPackEmpty <= 0 )
	{
		result.m_nErrorCode = RE_ITEM_PACKAGE_FULL;
		goto Exit0;
	}
	

	PetInstance* pInstance = GetPetInstance(nIndex);
	if (pInstance == NULL)
	{
		result.m_nErrorCode = RE_PET_INVALID_INSTANCE;
		goto Exit0;
	}

	bool bRet = CheckDecompose(nIndex);
	if (bRet == false)
	{
		result.m_nErrorCode = RE_PET_DENY_DECOMPOSE;
		goto Exit0;
	}

	int nRetCode = DecomposePetInstance(nIndex, pInstance);
	if (nRetCode == false)
	{
		result.m_nErrorCode = RE_PET_ERROR_DECOMPOSE;
		goto Exit0;
	}

	result.m_nErrorCode = RE_SUCCESS;
	nResult = true;
Exit0:
	pOwner->SendMsgToClient(MGPT_SC_PET_DECOMPOSE_REP, &result);
	return nResult;
}

int PetInstanceMgr::DropDecomposeItem(int nDropID)
{
	LC_ServerPlayer* pOwner = GetOwnerPlayer();
	if (pOwner == NULL)
		return false;

	pOwner->AddDropByDisassemble(nDropID);

	return true;
}

int PetInstanceMgr::DecomposePetInstance(int16_t nIndex, PetInstance* pInstance)
{
	CF_Pet::DataEntry* pData = pInstance->GetConfig();
	if (pData == NULL)
		return false;

	//pInstance->GetConfig()->_DropID  pData->_DropID
	int32_t nDropId = pInstance->GetCurrentDecomposeID();
	int nResult = DropDecomposeItem(nDropId);

	m_ppPetInstance.erase(nIndex);
	T_DELETE_D(pInstance);

	return true;
}

const mem::map<int16_t, PetInstance*>& PetInstanceMgr::GetRawData()
{
	return m_ppPetInstance;
}

PetInstance* PetInstanceMgr::GetMaxCombatScoreInstance()
{
	PetInstance* pResult = NULL;
	pResult = GetPetInstance(m_nMaxCombatScoreIndex);
	return pResult;
}

bool PetInstanceMgr::TryRefreshMaxCombatScoreByTemplate(int16_t nInputIndex)
{
	PetInstance* pNewInstance = GetPetInstance(nInputIndex);
	if ( pNewInstance == NULL )
		return false;

	int32_t nInputTemplateId = pNewInstance->GetTemplateId();
	uint64_t nInputMaxScore = pNewInstance->GetCombatScore();

	if (m_nMaxCombatScoreIndex == -1)
	{
		SetPetMaxCombatScoreInfo(nInputTemplateId, nInputIndex);
	}
	else
	{
		PetInstance* pMaxInstance = GetPetInstance(m_nMaxCombatScoreIndex);
		if (pMaxInstance == NULL || pMaxInstance->GetCombatScore() < nInputMaxScore)
		{
			SetPetMaxCombatScoreInfo(nInputTemplateId, nInputIndex);
		}
	}

	return true;
}

bool PetInstanceMgr::CheckDecompose(int16_t nIndex)
{
	bool bResult = false;
	PetInstance* pInput = GetPetInstance(nIndex);
	if (pInput == NULL)
		return bResult;

	mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.begin();
	for ( ; it != m_ppPetInstance.end(); ++it)
	{
		PetInstance* pInstance = it->second;
		// nullptr checking
		if (pInstance == NULL)
			continue;

		// self checking
		if (pInstance->GetIndex() == nIndex)
			continue;

		// Must be same res id
		if (pInstance->GetTemplateId() != pInput->GetTemplateId())
			continue;

		// Combat score and index
		if (pInstance->GetCombatScore() > pInput->GetCombatScore())
		{
			bResult = true;
			break;
		}

		if (pInstance->GetIndex() < pInput->GetIndex() && pInstance->GetCombatScore() >= pInput->GetCombatScore())
		{
			bResult = true;
			break;
		}
	}

	return bResult;
}

int PetInstanceMgr::UpgradeStarLevel(int16_t nIndex, bool bForce)
{
	int32_t nCurLevel = 0;
	Utility::UT_SIMDataList kItemReq;
	Utility::UT_SIMDataList::iterator it;
	int32_t nResult = false;
	MG_RLT_PetStarUpgrade data;
	data.m_nErrorCode = RE_SUCCESS;
	LC_ServerPlayer* pOwner = GetOwnerPlayer();
	vector<int> nParams; 
	nParams.clear();
	if (pOwner == NULL)
	{
		data.m_nErrorCode = RE_PET_INVALID_PLAYER;
		goto Exit0;
	}

	PetInstance* pInstance = GetPetInstance(nIndex);
	if (pInstance == NULL)
	{
		data.m_nErrorCode = RE_PET_INVALID_INSTANCE;
		goto Exit0;
	}

	CF_Pet::DataEntry* pResData = pInstance->GetConfig();
	if (pResData == NULL)
	{
		data.m_nErrorCode = RE_PET_INVALID_RES;
		goto Exit0;
	}

	nCurLevel = pInstance->GetStarLevel();
	CSVFile::CF_PetUpgrade::DataEntry* pCfg = const_cast<CSVFile::CF_PetUpgrade::DataEntry*>(BuddyHelper::GetPetUpgradeData(pResData->_cID, nCurLevel));
	CSVFile::CF_PetUpgrade::DataEntry* pNextCfg = const_cast<CSVFile::CF_PetUpgrade::DataEntry*>(BuddyHelper::GetPetUpgradeData(pResData->_cID, nCurLevel + 1 ));
	if (pCfg == NULL || pNextCfg == NULL)
	{
		data.m_nErrorCode = RE_PET_INVALID_STAR_LEVEL;
		goto Exit0;
	}

	if (bForce == false)
	{
		Utility::ParseFmtString2DataIFs(pCfg->_needitem, kItemReq);
		it = kItemReq.begin();
		// Item Checking..
		for (; it != kItemReq.end(); ++it)
		{
			UT_SIMDataInfo info = *it;
			int32_t nItemId = info.GetSkillID();
			int32_t nItemCount = info.GetSkillEnableLevel();
			int nFoundCount = pOwner->GetPackAsset()->GetItemCountInPack(PACK_TYPE_BASIC, nItemId);
			if ( nFoundCount < nItemCount )
			{
				data.m_nErrorCode = RE_BUDDY_ERROR_STAR_NO_RESOURCE;
				goto Exit0;
			}
		}
	}

	// Money Checking
	if ( pOwner->GetUnBindCash().GetMoney() < pCfg->_needgold && bForce == false)
	{
		data.m_nErrorCode = RE_UNBIND_CASH_NOT_ENOUGH;
		goto Exit0;
	}
	//埋点日志
	
	nParams.push_back(nIndex);
	nParams.push_back(nCurLevel);
	nParams.push_back(nCurLevel+1);
	int nLogCode = LC_Helper::SerializeOperateLogCode(SYSTEM_LOGTYPE, WANJU_OPERATE, OPERATE_LOGTYPE2);
	if (bForce == false)
	{
		// Item Reducing
		it = kItemReq.begin();
		for (; it != kItemReq.end(); ++it)
		{
			UT_SIMDataInfo info = *it;
			int32_t nItemId = info.GetSkillID();
			int32_t nItemCount = info.GetSkillEnableLevel();
			pOwner->DeleteItem(nItemId, nItemCount, nLogCode, nParams);
		}
	}

	// Mondy Reducing
	if (bForce == false)
		pOwner->ReduceCash(pCfg->_needgold, CT_UNBIND_CASH, SYS_FUNC_TP_BUDDY_STAR_LEVEL, pResData->_cID, nLogCode, nParams);
	// just a impl.
	pInstance->DoUpgradeStar();
	data.m_Data.nIndex = pInstance->GetIndex();
	data.m_Data.nResId = pInstance->GetTemplateId();
	data.m_Data.nStarLevel = pInstance->GetStarLevel();

Exit0:
	pOwner->SendMsgToClient(MGPT_SC_PET_STAR_UPGRADE_REP, &data);
	nResult = true;
	return nResult;
}

int16_t PetInstanceMgr::GetMaxCombatScoreIndexByTemplate(int32_t nResId)
{
	mem::map<int16_t, PetInstance*>::iterator findIt = m_ppPetInstance.find(m_nMaxCombatScoreIndex);
	if(findIt == m_ppPetInstance.end())
	{
		return 0;
	}

	PetInstance* pkPet = findIt->second;
	if(NULL == pkPet)
	{
		return 0;
	}

	nResId = m_nMaxCombatScorePetID;
	return pkPet->GetCombatScore();
}

void PetInstanceMgr::SetPetMaxCombatScoreInfo(int petID, int petIndex)
{
	m_nMaxCombatScoreIndex = petIndex;
	m_nMaxCombatScorePetID = petID;
}

uint64_t PetInstanceMgr::GetTotalCombatScore()
{
	uint64_t nResult = 0;
	mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.begin();
	for ( ;it != m_ppPetInstance.end(); ++it )
	{
		PetInstance* pInstance = it->second;
		if ( pInstance == NULL )
			continue;

		nResult += pInstance->GetCombatScore();
	}
	return nResult;
}

int PetInstanceMgr::GetCount()
{
	return m_ppPetInstance.size();
}

int32_t PetInstanceMgr::GetEnabledPetInstanceResId()
{
	PetInstance* pInstance = GetEnabledPetInstance();
	if (pInstance == NULL)
		return INVALID_ACTOR_ID;

	return pInstance->GetTemplateId();
}

int16_t PetInstanceMgr::GetEnabledPetInstanceIndex()
{
	return m_nEnabledIndex;
}

PetInstance* PetInstanceMgr::GetEnabledPetInstance()
{
	PetInstance* pResult = NULL;
	mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.find(m_nEnabledIndex);
	if (it == m_ppPetInstance.end())
		return pResult;

	pResult = it->second;
	return pResult;
}

PetInstance* PetInstanceMgr::GetPetInstance( int16_t nIndex )
{
	PetInstance* pResult = NULL;
	mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.find(nIndex);
	if (it == m_ppPetInstance.end())
		return pResult;

	pResult = it->second;
	return pResult;
}


void PetInstanceMgr::ReUse()
{
	m_nOwnerPlayerId = INVALID_ACTOR_ID;
	m_nEnabledIndex = -1;
	m_nMaxCombatScoreIndex = -1;
	m_nActivateState = false;
	m_setPetIds.clear();
	m_bDecompose = false;
	for (int i = 0; i < MAX_PET_COUNT; i++)
		m_bIndexArray[i] = false;
	ReUse_PetInstance();
}

void PetInstanceMgr::ReUse_PetInstance()
{
	// avoid mem-leak. release all instance.
	mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.begin();
	for ( ; it != m_ppPetInstance.end(); ++it )
	{
		PetInstance* pData = it->second;
		T_DELETE_D (pData);
	}
	m_ppPetInstance.clear();
}

PetInstance* PetInstanceMgr::AddPet( int32_t nPetResId )
{
	PetInstance* pResult = NULL;
	CF_Pet* pConfig = CF_Pet::GetSingletonPtr();

	CF_Pet::DataEntry* pData = pConfig->GetEntryPtr(nPetResId);
	if (pData == NULL)
	{
		goto Exit0;
	}

	pResult = AddPet_Impl(pData);

Exit0:
	return pResult;
}

PetInstance* PetInstanceMgr::AddPet_Impl(CSVFile::CF_Pet::DataEntry* pResData, int nIndex)
{
	PetInstance* pInstance = NULL;
	if (nIndex == -1)
		nIndex = TryAllocateIndex();
	if (nIndex == -1)
		return pInstance;
	pInstance = CreatePetInstance(  pResData, nIndex );
	if (pInstance == NULL)
		return pInstance;

	AllocateIndex(nIndex);

	m_ppPetInstance.insert(std::make_pair(nIndex, pInstance));
	m_setPetIds.insert(pResData->_cID);

	return pInstance;
}

int PetInstanceMgr::RemovePet( PetInstance* pInstance)
{
	return true;
}

int PetInstanceMgr::GenPetTypeData()
{
	return 0;
}

int16_t PetInstanceMgr::TryAllocateIndex()
{
	int16_t nResult = -1;
	for (int16_t i = 0; i < MAX_PET_COUNT; i++)
	{
		if (m_bIndexArray[i] == false)
		{
			nResult = i;
			break;
		}
	}
	return nResult;
}

int16_t PetInstanceMgr::AllocateIndex(int16_t nIndex)
{
	if (nIndex < 0)
		return false;

	if (nIndex >= MAX_PET_COUNT)
		return false;

	if (m_bIndexArray[nIndex])
		return false;

	m_bIndexArray[nIndex] = true;
	return true;
}

int16_t PetInstanceMgr::DeallocateIndex(int16_t nIndex)
{
	if (nIndex < 0)
		return false;

	if (nIndex >= MAX_PET_COUNT)
		return false;

	if (!m_bIndexArray[nIndex])
		return false;

	m_bIndexArray[nIndex] = false;
	return true;
}

void PetInstanceMgr::SyncAllPetData()
{
	SyncPetBaseData();
	SyncPetFullData();
}

void PetInstanceMgr::SyncPetBaseData()
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
		return;

	MG_SC_PetBaseDataSync sync;
	sync.m_nActivateState = m_nActivateState;
	sync.m_nEnabledIndex = m_nEnabledIndex;
	sync.m_nCount = 0;
	mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.begin();
	for ( ; it != m_ppPetInstance.end() ; ++it )
	{
		PetInstance* pInstance = it->second;
		if (pInstance == NULL)
			continue;

		sync.m_Data[sync.m_nCount].nIndex = pInstance->m_nIndex;
		sync.m_Data[sync.m_nCount].nResId = pInstance->m_pConfig->_cID;
		sync.m_Data[sync.m_nCount].nStarLevel = pInstance->m_Data.m_Data_Base.m_nStarLevel;

		sync.m_nCount++;
	}

	pOwnerPlayer->SendMsgToClient(MGPT_SC_PET_BASE_DATA_SYNC, &sync);
}

void PetInstanceMgr::SyncPetFullData()
{
	LC_ServerPlayer* pOwnerPlayer = GetOwnerPlayer();
	if (pOwnerPlayer == NULL)
		return;

	mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.begin();
	for ( ; it != m_ppPetInstance.end() ; ++it )
	{
		PetInstance* pInstance = it->second;
		if (pInstance == NULL)
			continue;

		pInstance->SyncAllDataToClient();
	}
}


object_id_type PetInstanceMgr::GetOwnerPlayerId()
{
	return m_nOwnerPlayerId;
}

LC_ServerPlayer* PetInstanceMgr::GetOwnerPlayer()
{
	LC_ServerWorldManager* pkWorldManager = SERVER_GET_WORLD_MANAGER();
	LC_ServerPlayer* pPlayer = (LC_ServerPlayer*)pkWorldManager->FindObject(m_nOwnerPlayerId);
	return pPlayer;
}

bool PetInstanceMgr::FindPetInstanceByTemplateId( CSVFile::CF_Pet::DataEntry* pResData )
{
	if ( pResData == NULL )
		return false;

	//mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.find( pResData->_cID );	
	//if ( it == m_ppPetInstance.end() )
	//	return false;
	mem::set<int32_t>::iterator it = m_setPetIds.find(pResData->_cID);
	if (it == m_setPetIds.end())
		return false;

	return true;
}

PetInstance* PetInstanceMgr::CreatePetInstance( CSVFile::CF_Pet::DataEntry* pResData, int16_t nIndex )
{
	PetInstance* pInstance = NULL;
	if (FindPetInstanceByTemplateId( pResData ))
	{
		// 分解
		mem::vector<int32_t> vecDropId;
		Utility::ParseFmtString2DataIFs( pResData->_DropID, vecDropId);
		if ( pResData->_InitLevel >= 0 && pResData->_InitLevel < vecDropId.size() )
		{
			int32_t nDropId = vecDropId[ pResData->_InitLevel ];
			DropDecomposeItem(nDropId);
		}
		m_bDecompose = true;
		return pInstance;
	}

	pInstance = T_NEW_D PetInstance(m_nOwnerPlayerId, nIndex, this);

	pInstance->InitTemplateData(pResData, true);

	return pInstance;
}

void PetInstanceMgr::SaveAll(char_data_define::pet_detail_data & detail_data)
{
	detail_data.set_activatestate(m_nActivateState);
	detail_data.set_enabled_index(m_nEnabledIndex);
	mem::map<int16_t, PetInstance*>::iterator it = m_ppPetInstance.begin();
	for ( ; it != m_ppPetInstance.end(); ++it )
	{
		PetInstance* pInstance = it->second;
		pInstance->Save(*detail_data.add_data() );
	}
}

void PetInstanceMgr::LoadAll(const char_data_define::pet_detail_data & detail_data)
{	
	m_nActivateState = detail_data.activatestate();
	m_nEnabledIndex = detail_data.enabled_index();
	for ( int i = 0; i < detail_data.data_size(); i++)
	{
		const char_data_define::pet_single_instance_data& single = detail_data.data(i);
		PetInstance* pInstance = LoadSingleInstance(single);
		if (pInstance == NULL)
			continue;

		TryRefreshMaxCombatScoreByTemplate(pInstance->GetIndex());

		m_setPetIds.insert(single.resid());
	}
}

PetInstance* PetInstanceMgr::LoadSingleInstance(const char_data_define::pet_single_instance_data& data)
{
	CF_Pet* pConfig = CF_Pet::GetSingletonPtr();

	int32_t nPetResId = data.resid();
	CF_Pet::DataEntry* pResData = pConfig->GetEntryPtr(nPetResId);
	if (pResData == NULL)
	{
		return NULL;
	}

	int nIndex = data.index();

	PetInstance* pInstnace = AddPet_Impl( pResData, nIndex );
	if (pInstnace == NULL)
		return NULL;

	pInstnace->Load(data);

	return pInstnace;
}


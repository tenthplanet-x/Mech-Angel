#include "ClientPCH.h"
#include "MG_MessageBase.h"
#include "MG_MessageCenter.h"
#include "RG_ClientPointLightManager.h"

using namespace Render;

RG_ClientPointLightManager::RG_ClientPointLightManager(void)
{
}

RG_ClientPointLightManager::~RG_ClientPointLightManager(void)
{
}

bool RG_ClientPointLightManager::RecvMessage(MG_MessageBase& rkMessage)
{
	MG_MessageHandlerInterface::RecvMessage(rkMessage);

	BEGIN_MESSAGE_HANDLE
	    MESSAGE_HANDLE(MGT_TIMER_TICK, _eventTimerTick)
	END_MESSAGE_HANDLE

	return true;
}

void RG_ClientPointLightManager::_eventTimerTick(MG_MessageBase& rkMessage)
{
    float fCurrentTime = rkMessage.m_fParam1;

    Update(fCurrentTime);
}

void RG_ClientPointLightManager::CheckInScenePointLightInfo(int iID, 
                                                            const NiColor& kPLColor, 
                                                            const NiPoint3& kPLPosition, 
                                                            const NiPoint3& kPLParam, 
                                                            int eICTypeWithFOType, 
                                                            const PLIntensityCtlrParams& kICParams)
{

    CheckedInPointLightInfoMap::iterator itr = m_kCheckedInPLsInfo.find(iID);
    if (itr == m_kCheckedInPLsInfo.end())
    {
        CheckedInPointLightInfo* pkCheckedInPLInfo = T_NEW_E(CheckedInPointLightInfo, Memory::MEMCATEGORY_GENERAL);
        pkCheckedInPLInfo->kPLColor             = kPLColor;
        pkCheckedInPLInfo->kPLPosition          = kPLPosition;
        pkCheckedInPLInfo->kPLParam             = kPLParam;
        pkCheckedInPLInfo->eICTypeWithFOType    = eICTypeWithFOType;
        pkCheckedInPLInfo->kICParams            = kICParams;
        pkCheckedInPLInfo->nRef                 = 1;

        m_kCheckedInPLsInfo.insert(std::make_pair(iID, pkCheckedInPLInfo));
    }
    else
    {
        ++(itr->second->nRef);
    }
}

void RG_ClientPointLightManager::CreateCheckedInScenePointLights(void)
{
    for (CheckedInPointLightInfoMap::iterator itr = m_kCheckedInPLsInfo.begin(); itr != m_kCheckedInPLsInfo.end(); ++itr)
    {
        CheckedInPointLightInfo* pkCheckedInPLInfo = itr->second;

        for (unsigned int i = 0; i < pkCheckedInPLInfo->nRef; ++i)
        {
            CreateScenePointLight(itr->first, 
                pkCheckedInPLInfo->kPLColor, pkCheckedInPLInfo->kPLPosition, pkCheckedInPLInfo->kPLParam, 
                _getFOType(pkCheckedInPLInfo->eICTypeWithFOType), 
                _getICType(pkCheckedInPLInfo->eICTypeWithFOType), pkCheckedInPLInfo->kICParams);
        }

        T_DELETE_E(itr->second, CheckedInPointLightInfo, Memory::MEMCATEGORY_GENERAL);
    }
    m_kCheckedInPLsInfo.clear();
}

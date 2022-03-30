//�ͻ��˵��Ч��������

#pragma once

#include "RG_PointLightManager.h"
#include "MG_MessageHandlerInterface.h"

class RG_ClientPointLightManager : public Render::RG_PointLightManager
                                 , public MG_MessageHandlerInterface
{
private:
    struct CheckedInPointLightInfo 
    {
        int iID;
        NiColor kPLColor;
        NiPoint3 kPLPosition;
        NiPoint3 kPLParam;
        int eICTypeWithFOType;
        Render::PLIntensityCtlrParams kICParams;
        unsigned int nRef;
    };

    typedef mem::map<int, CheckedInPointLightInfo*> CheckedInPointLightInfoMap;

public:
    static RG_ClientPointLightManager& GetSingleton(void)
    {
        return *(static_cast<RG_ClientPointLightManager*>(RG_PointLightManager::GetSingletonPtr()));
    }
    static RG_ClientPointLightManager* GetSingletonPtr(void)
    {
        return static_cast<RG_ClientPointLightManager*>(RG_PointLightManager::GetSingletonPtr());
    }

	RG_ClientPointLightManager(void);
	virtual ~RG_ClientPointLightManager(void);

    virtual bool RecvMessage(MG_MessageBase& rkMessage);

    // �ں�̨�̵߳Ǽ���Ҫ�����ĳ��������Ϣ
    void CheckInScenePointLightInfo(int iID, 
        const NiColor& kPLColor, const NiPoint3& kPLPosition, const NiPoint3& kPLParam, 
        int eICTypeWithFOType, const Render::PLIntensityCtlrParams& kICParams);
    // �ص����̴߳��������ѵǼǵĳ������
    void CreateCheckedInScenePointLights(void);

protected:

private:
    void _eventTimerTick(MG_MessageBase& rkMessage);

private:
    UT_Lock m_kCheckInPLLock;

    CheckedInPointLightInfoMap m_kCheckedInPLsInfo;
};
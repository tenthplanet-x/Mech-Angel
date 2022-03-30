#pragma once

#include "GD_BulletManager.h"
#include "MG_MessageHandlerInterface.h"

//==============================================================================
// �ͻ����ӵ�����������Ч������
//==============================================================================

class GD_ClientBulletManager : public Data::GD_BulletManager
                             , public MG_MessageHandlerInterface
{
public:
    static GD_ClientBulletManager& GetSingleton(void)
    {
        return *(static_cast<GD_ClientBulletManager*>(GD_BulletManager::GetSingletonPtr()));
    }
    static GD_ClientBulletManager* GetSingletonPtr(void)
    {
        return static_cast<GD_ClientBulletManager*>(GD_BulletManager::GetSingletonPtr());
    }

    GD_ClientBulletManager(void);
    virtual ~GD_ClientBulletManager(void);

    virtual bool RecvMessage(MG_MessageBase& rkMessage);

private:
	virtual Data::GD_Bullet* _allocBullet(const Data::GD_BulletCreateArg & kArg);

    void _eventTimerTick(MG_MessageBase& rkMessage);
};

//==============================================================================
// �ͻ����ӵ�
//==============================================================================

class GD_ClientBullet : public Data::GD_Bullet
{
public:
	explicit GD_ClientBullet(const Data::GD_BulletCreateArg & kArg);
    virtual ~GD_ClientBullet(void);

protected:
    virtual Data::GD_ModelBase* _doGetModelById(long lId);

	virtual bool _refetchTargetCurrentLocation(Target& kTarget);	//����Ŀ���λ��

};

//==============================================================================
// �ͻ�������������𴴽����ӵ�������������ʼid
// ԭ�������ӵ����Ƿ��������ɵģ�������Ҫ��GamePlayer�Լ����ӵ��ŵ��ͻ���������
//==============================================================================
class GD_ClientSelfInitiatedBulletID
{
public:
	typedef unsigned int BulletID;
	static BulletID	AllocIDs(unsigned int n);
	static BulletID Current() { return ms_BulletStartID; }

private:
	static BulletID	ms_BulletStartID;
};

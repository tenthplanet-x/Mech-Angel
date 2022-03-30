#pragma once

#include "GD_BulletManager.h"
#include "MG_MessageHandlerInterface.h"

//==============================================================================
// 客户端子弹及连接型特效管理器
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
// 客户端子弹
//==============================================================================

class GD_ClientBullet : public Data::GD_Bullet
{
public:
	explicit GD_ClientBullet(const Data::GD_BulletCreateArg & kArg);
    virtual ~GD_ClientBullet(void);

protected:
    virtual Data::GD_ModelBase* _doGetModelById(long lId);

	virtual bool _refetchTargetCurrentLocation(Target& kTarget);	//更新目标点位置

};

//==============================================================================
// 客户端向服务器发起创建的子弹，这里存的是起始id
// 原先所有子弹都是服务器生成的，现在需要把GamePlayer自己的子弹放到客户端来生成
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

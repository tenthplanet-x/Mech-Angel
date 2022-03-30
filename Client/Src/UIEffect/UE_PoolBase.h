#pragma once
#include "UE_UiEffectBase.h"
#include "UT_PoolSet.h"
class UE_PoolBase : public Utility::UT_PoolSet<UE_UiEffectBase>
{
public:
	UE_PoolBase(int nPoolSize = 50 );
	~UE_PoolBase();
	UE_UiEffectBase* GetNewInstance(UI_EFFECT_TYPE eType);
protected:
	UE_UiEffectBase* Allocate();
	void Deallocate( UE_UiEffectBase* pkObj );
};
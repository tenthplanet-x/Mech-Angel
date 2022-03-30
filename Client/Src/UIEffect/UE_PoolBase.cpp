#include "ClientPCH.h"
#include "UE_PoolBase.h"

UE_UiEffectBase* UE_PoolBase::GetNewInstance( UI_EFFECT_TYPE eType )
{
	UE_UiEffectBase* pkNewObj = GetPoolable();
	//pkNewObj
	return pkNewObj;
	//return pkNewObj;
}

UE_UiEffectBase* UE_PoolBase::Allocate()
{
	UE_UiEffectBase *pkNewObj = T_NEW_D UE_UiEffectBase();
	return pkNewObj;
}
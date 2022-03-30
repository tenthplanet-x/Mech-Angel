#pragma once

#include "UT_PoolSet.h"
#include "UE_UiEffectHead.h"
#include "MG_MessageHandlerInterface.h"
#include "MG_MessageCenter.h"
#include "UT_Win32Helper.h"
#include "UE_PlayerName.h"

template <class T> 
class UE_UiEffectPoolBase : public Utility::UT_PoolSet<T>
{
public:
	UE_UiEffectPoolBase(UI_EFFECT_TYPE eType);

	virtual ~UE_UiEffectPoolBase();

	void Update(float fCurrentTime);

	T* GetUiEffect(long lTarget);
	void HideUiEffect();
	void ShowUiEffect();
	void GetVisibleArrary(NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray);
	void RequestRedraw();
	void RedrawPlaceHolderNameRect();
	void ResetTime();
protected:
	virtual T* Allocate();
	virtual void Deallocate(T* p );
private:
	UI_EFFECT_TYPE		m_eType;
	unsigned int        m_nAllocateCount;
};

template <class T>
void UE_UiEffectPoolBase<T>::RequestRedraw()
{
	size_t nActiveNum = mActivePoolables.size();
	if (nActiveNum == 0)
	{
		return;
	}
	for(ActivePoolableList::iterator iter = mActivePoolables.begin() ; iter != mActivePoolables.end(); ++iter)
	{
		T* pkActive = (*iter);
		pkActive->RequestRedraw();
	}
}
template <class T>
void UE_UiEffectPoolBase<T>::ResetTime()
{
	size_t nActiveNum = mActivePoolables.size();
	if (nActiveNum == 0)
		return;
	for (ActivePoolableList::iterator iter = mActivePoolables.begin(); iter != mActivePoolables.end(); ++iter)
	{
		T* pkActive = (*iter);
		pkActive->ResetTime();
	}
}
template <class T>
void UE_UiEffectPoolBase<T>::RedrawPlaceHolderNameRect()
{
	size_t nActiveNum = mActivePoolables.size();
	if (nActiveNum == 0)
	{
		return;
	}
	for(ActivePoolableList::iterator iter = mActivePoolables.begin() ; iter != mActivePoolables.end(); ++iter)
	{
		T* pkActive = (*iter);
		long lTargetID = pkActive->GetTargetID();
		if(IS_CHEST_ID(lTargetID))
			((UE_PlayerName*)(pkActive))->RedrawPlaceHolderNameRect();
	}
}
template <class T>
UE_UiEffectPoolBase<T>::UE_UiEffectPoolBase( UI_EFFECT_TYPE eType )
:m_eType(eType)
{
	T_ASSERT(m_eType >= 0 && m_eType < UE_MAX_NUM);
	m_nAllocateCount = 0;
}

template <class T>
UE_UiEffectPoolBase<T>::~UE_UiEffectPoolBase()
{
	DeletePool();
}

template <class T>
void UE_UiEffectPoolBase<T>::Update( float fCurrentTime )
{
	size_t nActiveNum = mActivePoolables.size();
	if (nActiveNum == 0)
	{
		return;
	}
	for(ActivePoolableList::iterator iter = mActivePoolables.begin() ; iter != mActivePoolables.end(); )
	{
		T* pkActive = (*iter);

		if ( pkActive->Update(fCurrentTime) == false )
		{
			//GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING , 
			//	"erase ui effect for %d" , pkActive->GetTargetID() );
			//Deallocate( pkActive , true );
			mFreePoolables.push_back (*iter);
			//在遍历一个Map的时候删除其元素，需要把Iter手动更新！
#ifndef _USE_STL_PORT
			iter = mActivePoolables.erase( iter );	
#else
			mActivePoolables.erase(iter++);
#endif
		}		
		else
		{
			++iter;
		}
	}
}

template <class T>
void UE_UiEffectPoolBase<T>::GetVisibleArrary( NiCullingProcess& rkCuller, NiVisibleArray& rkVisibleArray )
{
	NiCameraPtr spPerCamera = CA_CameraManager::GetSingleton().GetNiCamera();

	ActivePoolableList::iterator iter;
	int i=0;
	TRAV( mActivePoolables , iter )
	{
		T* pkActive = (*iter);
		NiNodePtr spNiRoot = pkActive->GetRoot();
		{
			NiCullScene(spPerCamera, spNiRoot, rkCuller, rkVisibleArray,false);
		}
	}
}

template <class T>
T* UE_UiEffectPoolBase<T>::Allocate()
{
	T* pkNewObj =  T_NEW_D T(m_eType);
	pkNewObj->Init();
	return pkNewObj;
}

template <class T>
void UE_UiEffectPoolBase<T>::Deallocate( T* p )
{
	T_DELETE_D p;
}



template <class T>
T* UE_UiEffectPoolBase<T>::GetUiEffect( long lTarget )
{
	ActivePoolableList::iterator iter;
	TRAV( mActivePoolables , iter )
	{
		T* pkActive = (*iter);

		if ( pkActive->GetTargetID() == lTarget)
		{
			return pkActive;				
		}		
	}
	return NULL;
}

template <class T>
void UE_UiEffectPoolBase<T>::HideUiEffect()
{
	ActivePoolableList::iterator iter;
	TRAV( mActivePoolables , iter )
	{
		T* pkActive = (*iter);
		pkActive->Hide();
	}
}

template <class T>
void UE_UiEffectPoolBase<T>::ShowUiEffect()
{
	ActivePoolableList::iterator iter;
	TRAV( mActivePoolables , iter )
	{
		T* pkActive = (*iter);
		pkActive->Show();
	}
}
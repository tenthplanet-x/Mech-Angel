//UI效果基类，仅满足当前需求

#pragma once

#include "UE_UiEffectHead.h"
#include "RS_ScreenElements.h"
#include "IG_Image.h"
namespace Data
{
    class GD_ModelContent;
}

namespace GameLogic
{
	class LC_ActorBase;
}

class UE_UiEffectBase : public Memory::MM_BaseObject
{
public:
	UE_UiEffectBase(UI_EFFECT_TYPE eType);
	virtual ~UE_UiEffectBase();

	virtual bool Init() = 0;
	virtual void SetPendingParam(long lParam1, long lParam2) {}	
	virtual bool Update(float fCurrentTime);
	virtual void Show();
	virtual void Hide() {}
	virtual void RequestRedraw(){}
	virtual void ResetTime(bool kFlag = false){};
public:
	long		GetTargetID() const { return m_lTargetID; }
	NiNodePtr	GetRoot() const { return m_spEffectRoot; }

protected:	
	virtual GameLogic::LC_ActorBase* _getTargetPosition(Utility::UT_Vec3Int& rkPosition) const;	
	void _findPlayTime(NiObjectNET* pkObjectNET);
	bool _findActorHeight();
	bool _findActorShowEffectTime();
	bool _IsForever() const { return m_fPlayTime < 0.001f; }
	bool _targetLoaded();		//目标角色是否完成后台加载？	
	bool _targetDestroyed();	//目标角色是否已经删除？

	GameLogic::LC_ActorBase* _getTargetActor();

protected:
	UI_EFFECT_TYPE			m_eType;
	object_id_type			m_lTargetID;
	long					m_lActorHeight;
	float					m_fDelayShowTime;
	float					m_fStartShowTime;
	float					m_fLastChangeTime;
	long					m_lShowPosIndex;
	float					m_fPlayTime;
	bool					m_bShow;
	bool					m_bVisible;
	float					m_fDelayInterval;
	float					m_fInterpolatorBeginTime;
	float					m_fInterpolatorEndTime;

    Data::GD_ModelContent*  m_pkModelContent;
	NiNodePtr				m_spEffectRoot;
	NiNodePtr				m_spPropertyNode;
	bool                    m_bStartPlay;
	NiPoint3				m_kShowStartPos;//转换到屏幕坐标用
	
	NiInterpolatorPtr		m_spInterpolator;
	NiQuatTransform			m_kUpdateValue;
	NiMaterialPropertyPtr	m_spMaterialProperty;
	float					m_fAlpha;
	float					m_fScale;
	NiPoint3				m_kDeltaPos;
	typedef std::pair<GUISys::IG_ImagePtr,GUISys::QuadInfo> QuadType;
	typedef mem::vector<QuadType> QuadArray;
	QuadArray m_kQuadArray;
};

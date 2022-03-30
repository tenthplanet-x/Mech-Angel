//其实是HP条，和MP条

#pragma once

#include "UE_UiEffectBase.h"

namespace Data
{
    class GD_Material;
}

class UE_PlayerBlood : public UE_UiEffectBase					 				
{
public:
	enum { BLOOD_VERTEX_NUM = 4 };

	UE_PlayerBlood(UI_EFFECT_TYPE eType);
	~UE_PlayerBlood();

	virtual bool Init();
	virtual void SetPendingParam(long lParam1, long lParam2);
	virtual void Show();
	virtual void Hide();
	virtual bool Update(float fCurrentTime);
	void		 SetMouseType(bool bValue){m_bMouseType = bValue;}
	bool		 GetMouseType(){return m_bMouseType;}

private:
	bool _updateInfo();

private:
	bool m_bMouseType;
};
#pragma once

class RG_AvatarRenderValidator : public NiRenderClickValidator
{
public:
	RG_AvatarRenderValidator(void){};
	virtual ~RG_AvatarRenderValidator(void){};

	/*virtual */bool ValidateClick(NiRenderClick* pkRenderClick,
		unsigned int uiFrameID);
};
NiSmartPointer(RG_AvatarRenderValidator);

class RG_AvatarRenderClick : public NiRenderClick 
{
public:
	RG_AvatarRenderClick(void);
	virtual ~RG_AvatarRenderClick(void);

	/*virtual */unsigned int GetNumObjectsDrawn() const { return m_uiObjects; }
	/*virtual */float GetCullTime() const { return 0.0f; }
	/*virtual */float GetRenderTime() const { return m_fRenderTime; }

	void SetBgColorInfo(bool bSpecial);
	void SetIndex(int idx) { m_iIndex = idx; };
	int GetIndex() { return m_iIndex; }
protected:

	/*virtual */void PerformRendering(unsigned int uiFrameID);
	static bool _callbackPreProc(NiRenderClick* pkRenderClick, void* pvCallbackData);
	static bool _callbackPostProc(NiRenderClick* pkRenderClick, void* pvCallbackData);

private:

	float m_fRenderTime;
	unsigned int m_uiObjects;
	int m_iIndex;		//avatarRenderclickIndex RENDER_CLICK_ORIGIN = 1, RENDER_CLICK_SMALL = 2

	RG_AvatarRenderValidatorPtr m_spRenderValidator;
};
NiSmartPointer(RG_AvatarRenderClick);

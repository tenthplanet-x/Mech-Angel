#pragma once

//////////////////////////////////////////////////////////////////////////
/// 仅仅是增加了PROFILE和RenderEvent的 NiDefaultClickRenderStep

class RG_ClientRenderStep : public NiDefaultClickRenderStep
{
public:
	RG_ClientRenderStep(const char* name);
	virtual ~RG_ClientRenderStep(void);

protected:
	virtual void PerformRendering(void);

protected:
    const char*		m_pMbcsName;
	std::wstring	m_kWideCharName;
};

//////////////////////////////////////////////////////////////////////////
/// 仅仅是增加了PROFILE和RenderEvent的 NiViewRenderClick

class RG_ClientRenderClick : public NiViewRenderClick
{
public:
	RG_ClientRenderClick(const char* name);
	virtual ~RG_ClientRenderClick(void);

protected:
	virtual void PerformRendering(unsigned int uiFrameID);

protected:
	const char*		m_pMbcsName;
	std::wstring	m_kWideCharName;
};

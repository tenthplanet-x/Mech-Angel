#pragma once

#include "NiScreenElements.h"

class RG_UERenderClick: public NiRenderClick
{
public:
	RG_UERenderClick(void);
	virtual ~RG_UERenderClick(void);

	// Functions for reporting statistics about the most recent frame.
	virtual unsigned int GetNumObjectsDrawn() const { return m_uiObjects; }
	// This render click does not perform any culling, so the cull time is
	// always zero.
	virtual float GetCullTime() const { return 0.0f; }
	virtual float GetRenderTime() const { return m_fRenderTime; }

protected:
	virtual void PerformRendering(unsigned int uiFrameID);

protected:
	float           m_fRenderTime;
	unsigned int    m_uiObjects;
};
NiSmartPointer(RG_UERenderClick);

#ifndef _RG_MINIMAP_RENDER_STEP_H
#define _RG_MINIMAP_RENDER_STEP_H

#include "NiRenderStep.h"

class RG_MiniMapRenderStep : public NiRenderStep
{
public:
	RG_MiniMapRenderStep();
	virtual ~RG_MiniMapRenderStep();

	virtual bool SetOutputRenderTargetGroup(
		NiRenderTargetGroup* pkOutputRenderTargetGroup);
	virtual NiRenderTargetGroup* GetOutputRenderTargetGroup();

	virtual unsigned int GetNumObjectsDrawn() const{return 0;};
	virtual float GetCullTime() const {return 0;};
	virtual float GetRenderTime() const {return 0;};


protected:
	virtual void PerformRendering();
};

NiSmartPointer(RG_MiniMapRenderStep);


#endif
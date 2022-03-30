//Set Camera Data or PV

#pragma once

class RG_UiEffectRenderView : public NiRenderView
{
public:
	RG_UiEffectRenderView();
	virtual ~RG_UiEffectRenderView();

	virtual void SetCameraData(const NiRect<float>& kViewport);

protected:
	virtual void CalculatePVGeometry();
};

NiSmartPointer(RG_UiEffectRenderView);
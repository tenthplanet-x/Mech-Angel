//�� instancing ��ʽ��Ⱦ�ݵ� click
#pragma once

#include "NiViewRenderClick.h"

class RG_GrassRenderClick : public NiViewRenderClick
{
public:
    RG_GrassRenderClick(void);
    virtual ~RG_GrassRenderClick(void);

protected:
    virtual void PerformRendering(unsigned int uiFrameID);
};
NiSmartPointer(RG_GrassRenderClick);

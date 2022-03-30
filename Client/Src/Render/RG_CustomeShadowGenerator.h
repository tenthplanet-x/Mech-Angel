#pragma once
#include "NiDefaultShadowClickGenerator.h"

class NIMAIN_ENTRY RG_CustomeShadowGenerator : public NiShadowGenerator
{
    NiDeclareRTTI;

public:
    RG_CustomeShadowGenerator();
    RG_CustomeShadowGenerator(NiDynamicEffect* pkDynEffect);
    virtual ~RG_CustomeShadowGenerator();

    void GetCustomCasterGeometryList(NiGeometryList& kCasterList, NiCamera* pkShadowCamera);
private:	
	NiCullingProcess m_CullingProcess;
};

class RG_CustomeShadowClickGenerator : public NiDefaultShadowClickGenerator
{
	NiDeclareRTTI;
public:
	RG_CustomeShadowClickGenerator();
	virtual ~RG_CustomeShadowClickGenerator();

	void SetName( const char* strName );

	virtual bool GenerateRenderClicks(
		const NiTPointerList<NiShadowGeneratorPtr>& kGenerators);

protected:
    virtual bool HandleDirectionalLight(NiShadowGenerator* pkGenerator, 
        NiDirectionalLight* pkDirLight, bool bRegenerateViews,
        bool bUpdateShadowMaps);

};

NiSmartPointer(RG_CustomeShadowClickGenerator);
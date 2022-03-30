#include "ClientPCH.h"
#include "RG_CustomeShadowGenerator.h"

#include "FX_ClientFxManager.h"
#include "IM_Manager.h"
#include "SE_SceneManager.h"

NiImplementRTTI(RG_CustomeShadowGenerator, NiShadowGenerator);

RG_CustomeShadowGenerator::RG_CustomeShadowGenerator() : m_CullingProcess(0)
{
}

RG_CustomeShadowGenerator::RG_CustomeShadowGenerator(NiDynamicEffect* pkDynEffect)
: NiShadowGenerator(pkDynEffect), m_CullingProcess(0)
{
}

RG_CustomeShadowGenerator::~RG_CustomeShadowGenerator()
{
}

void RG_CustomeShadowGenerator::GetCustomCasterGeometryList(NiGeometryList& kCasterList, NiCamera* pkShadowCamera)
{
    kCasterList.RemoveAll();

    if (!m_pkDynEffect)
    {
        return;
    }

	
    SE_SceneManager::GetSingleton().GetShadowCasters(pkShadowCamera, m_CullingProcess, kCasterList);
    IM_Manager::GetSingleton().GetShadowCasters(kCasterList);
    FX_ClientFxManager::GetSingleton().GetFxModelVisibleArray(kCasterList);
    //// Flatten the affected node list of the shadow generator's light into a 
    //// set of only NiGeometry objects.
    //const NiNodeList& kAffectedNodeList = m_pkDynEffect->GetAffectedNodeList();
    //NiTListIterator kIter = kAffectedNodeList.GetHeadPos();
    //while (kIter)
    //{
    //    NiNode* pkNode = kAffectedNodeList.GetNext(kIter);
    //    AdditiveListHelper(pkNode, &kCasterList);
    //}

    //// Now remove all NiGeometry objects from the casters list that are 
    //// included in the light's unaffected node list.
    //const NiNodeList& kUnaffectedNodeList = 
    //    m_pkDynEffect->GetUnaffectedNodeList();
    //kIter = kUnaffectedNodeList.GetHeadPos();
    //while (kIter)
    //{
    //    NiNode* pkNode = kUnaffectedNodeList.GetNext(kIter);
    //    SubtractiveListHelper(pkNode, &kCasterList);
    //}

    //// Now remove all NiGeometry objects from the casters list that are 
    //// included in the generators unaffected caster list.
    //kIter = m_kUnaffectedCasterList.GetHeadPos();
    //while (kIter)
    //{
    //    NiNode* pkNode = m_kUnaffectedCasterList.GetNext(kIter);
    //    SubtractiveListHelper(pkNode, &kCasterList);
    //}
}

//---------------------------------------------------------------------------

NiImplementRTTI(RG_CustomeShadowClickGenerator, NiDefaultShadowClickGenerator);

RG_CustomeShadowClickGenerator::RG_CustomeShadowClickGenerator()
: NiDefaultShadowClickGenerator()
{
    kVSMTechniqueName = "NiVSMShadowTechnique";
}

RG_CustomeShadowClickGenerator::~RG_CustomeShadowClickGenerator()
{
}

void RG_CustomeShadowClickGenerator::SetName( const char* strName )
{
    *( const_cast<NiFixedString*>( &m_kName ) ) = NiFixedString(strName);
}

bool RG_CustomeShadowClickGenerator::GenerateRenderClicks(
	const NiTPointerList<NiShadowGeneratorPtr>& kGenerators)
{
	m_uiActivePostProcessClicks = 0;
	NiShadowManager::ReorganizeActiveShadowGenerators();

	NiTListIterator kIter = kGenerators.GetHeadPos();
	while (kIter)
	{
		NiShadowGenerator* pkGenerator = kGenerators.GetNext(kIter);

		if (!pkGenerator)
			continue;

		bool bRegenerateRenderClicks = false;
		bool bUpdateShadowMaps = false;
		NiDynamicEffect* pkDynEffect = 
			pkGenerator->GetAssignedDynamicEffect();

		if (pkGenerator->GetActive() && pkDynEffect->GetSwitch())
		{
			if (pkGenerator->GetStatic())
			{
				if (pkGenerator->GetRenderViewsDirty() )
					bRegenerateRenderClicks = true;
				if (pkGenerator->AreShadowMapsDirty())
					bUpdateShadowMaps = true;
			}
			else
			{
				bUpdateShadowMaps = true;
				//if (pkGenerator->GetRenderViewsDirty() )
					bRegenerateRenderClicks = true;
			}

			if (bRegenerateRenderClicks || bUpdateShadowMaps)
			{
				// Render Clicks need to be regenerated. So regenerate the
				// render clicks and add them to the list of clicks to be
				// rendered so their shadow maps are up to date.
				bool bResults = false; 
				switch (pkDynEffect->GetEffectType())
				{
				case NiDynamicEffect::SPOT_LIGHT:
				case NiDynamicEffect::SHADOWSPOT_LIGHT:
					bResults = HandleSpotLight(pkGenerator, 
						(NiSpotLight*)pkDynEffect, 
						bRegenerateRenderClicks, bUpdateShadowMaps);
					break;
				case NiDynamicEffect::POINT_LIGHT:
				case NiDynamicEffect::SHADOWPOINT_LIGHT:
					bResults = HandlePointLight(pkGenerator, 
						(NiPointLight*)pkDynEffect, 
						bRegenerateRenderClicks, bUpdateShadowMaps);
					break;
				case NiDynamicEffect::DIR_LIGHT:
				case NiDynamicEffect::SHADOWDIR_LIGHT:
					bResults = HandleDirectionalLight(pkGenerator,
						(NiDirectionalLight*)pkDynEffect, 
						bRegenerateRenderClicks, bUpdateShadowMaps);
					break;
				default:
					// Unsupported dynamic effect; do nothing.
					NIASSERT("Unsupported NiDynamicEffect type");
					break;
				}

				if (!bResults)
				{
					// Failed to correctly produce the shadow render clicks 
					// for the generator, so de-activate the generator.
					pkGenerator->SetActive(false, true);
				}

			}

		}
	}

	return true;
}

bool RG_CustomeShadowClickGenerator::HandleDirectionalLight(NiShadowGenerator* pkGenerator, 
                                                            NiDirectionalLight* pkDirLight, 
                                                            bool bRegenerateViews, 
                                                            bool bUpdateShadowMaps)
{
    NIASSERT(pkDirLight);

    // Get a shadow map for the generator.
    NiShadowMap* pkShadowMap = ObtainAndAssignShadowMap(pkGenerator, 0);

    if (!pkShadowMap)
        return false;

    NIASSERT(pkShadowMap->GetTextureType() == NiShadowMap::TT_SINGLE);

    // Get the render click for the shadow map.
    NiShadowRenderClick* pkShadowClick =
        pkShadowMap->GetRenderClick();
    NIASSERT(pkShadowClick);

    // Get the render view from the render click.
    NIASSERT(pkShadowClick->GetRenderViews().GetSize() > 0);
    Ni3DRenderView* pkShadowView = NiDynamicCast(Ni3DRenderView,
        pkShadowClick->GetRenderViews().GetHead());
    NIASSERT(pkShadowView);

    NiCamera* pkShadowCamera = pkShadowView->GetCamera();
    NIASSERT(pkShadowCamera);

    if (bRegenerateViews)
    {
        // Clear all scenes from the render view.
        pkShadowView->GetScenes().RemoveAll();

        // Iterate through affected nodes list, adding all affected
        // nodes to the render view and merging the bounds of all the affected 
        // nodes into a single bound. The view frustum will then be computed 
        // from this bound.

        NiPoint3 kCameraPos;

        float fNearClippingDist = pkGenerator->GetNearClippingDist();
        float fFarClippingDist = pkGenerator->GetFarClippingDist();
        float fDirLightFrustumWidth = pkGenerator->GetDirLightFrusturmWidth();



		fDirLightFrustumWidth += 200;
        kCameraPos = pkDirLight->GetWorldTranslate();

        NiFrustum kFrustum(-fDirLightFrustumWidth, fDirLightFrustumWidth, 
            fDirLightFrustumWidth, -fDirLightFrustumWidth, 
            fNearClippingDist, fFarClippingDist/*+10000*/, true);
		kCameraPos -= 2000 * pkDirLight->GetWorldDirection();
        pkShadowCamera->SetViewFrustum(kFrustum);
        pkShadowCamera->SetTranslate(kCameraPos);

		((RG_CustomeShadowGenerator*)pkGenerator)->GetCustomCasterGeometryList(m_kGeometryList, pkShadowCamera);
		NiTListIterator kCasterNodeIter =  m_kGeometryList.GetHeadPos();
		while (kCasterNodeIter)
		{
			NiGeometry* pkGeometry = m_kGeometryList.GetNext(kCasterNodeIter);
			pkShadowView->AppendScene((NiAVObject*)pkGeometry);
		}
    }
    else
    {
        pkShadowCamera->SetTranslate(pkDirLight->GetWorldTranslate());
    }

    pkShadowCamera->SetRotate(pkDirLight->GetWorldRotate());
    pkShadowCamera->SetScale(pkDirLight->GetWorldScale());
    pkShadowCamera->Update(0.0f);

    pkShadowClick->SetProcessor(NiShadowManager::GetMaterialSwapProcessor(
        NiStandardMaterial::LIGHT_DIR));

    if (bUpdateShadowMaps)
    {
        AddRenderClick(pkShadowClick);

        NiShadowTechnique* pkTechnique = pkGenerator->GetShadowTechnique();
        pkTechnique->PrepareShadowMap(pkShadowMap);
        pkTechnique->AppendPostProcessRenderClicks(pkShadowMap, this);
    }

    pkGenerator->SetRenderViewsDirty(false);
    return true;
}

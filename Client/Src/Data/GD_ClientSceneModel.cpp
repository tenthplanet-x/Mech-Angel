#include "ClientPCH.h"
#include "GD_ClientSceneModel.h"
#include "MTF_WorldTile.h"
#include "RG_ClientPointLightManager.h"
#include "SE_SceneManager.h"
#include "UT_RenderHelper.h"
#include "SoftParticlesManager.h"
#include "IM_Manager.h"
#include "PF_GroundMeshConverter.h"

using namespace FileSystem;

static inline bool IsSceneFxModel( NiAVObject* pAVObject )
{
	if(NiDynamicCast(NiNode,pAVObject))
	{
		int nCount = ((NiNode*)pAVObject)->GetArrayCount();
		for(int x = 0; x < nCount; ++x)
		{
			if(((NiNode*)pAVObject)->GetAt(x))
			{
				if( IsSceneFxModel(((NiNode*)pAVObject)->GetAt(x) ) )
					return true;
			}
		}
	}
	else
	{
		if(NiDynamicCast(NiParticleSystem,pAVObject))
		{
			return true;
		}

		if(NiDynamicCast(NiGeometry,pAVObject))
		{
			NiGeometry* pkGeom = (NiGeometry*) pAVObject;
			NiTexturingProperty* pkTex = (NiTexturingProperty*)pkGeom->GetProperty( NiProperty::TEXTURING );
			if ( pkTex != NULL )
			{
				if( pkTex->GetControllers() != NULL )
				{
					return true;
				}
			}
			NiNode* pkParent = pkGeom->GetParent();
			if ( pkParent && NiDynamicCast( NiBillboardNode , pkParent ) )
			{
				return true;
			}
			return false;
		}
	}
	return false;
	
}

GD_ClientSceneModel::GD_ClientSceneModel(void)
: m_fLastUpdateTime(0.0f)
{
	m_bPickFlag = false;
	for ( int i = 0 ; i < MAX_DYNAMIC_POINT_LIGHT ; i++ )
	{
		m_vPLID[i] = -1;
	}
	m_nPointLightID = 0;
	m_bIsClickable = false;
	m_pkPathPickRoot = NULL;
	m_bTransparent = false;
	m_bSceneFx = false;
	m_bWalkable = false;
	m_bRenderReflect = false;
	m_bSceneModelFx = false;
	m_bSceneParticleSystem = false;
}

GD_ClientSceneModel::~GD_ClientSceneModel(void)
{
}

void GD_ClientSceneModel::GetModelType(bool& bSkinned , bool& bGlow)
{
	if (!_isInitedWithLoaded() || m_pkModelContent == NULL)
	{
		return;
	}
	bool bUseUV = false;
	bool bUV2 = false;
	GfxGetModelType(m_spNiRootNode, bSkinned, bGlow,bUseUV,bUV2);
}

bool GD_ClientSceneModel::SetTargetForNiPick(NiPick* pkPick)
{
	if (!_isInitedWithLoaded() || m_pkModelContent == NULL)
	{
		return false;
	}

	pkPick->SetTarget(m_spNiRootNode);

	return true;
}
bool GD_ClientSceneModel::SetTargetForPathPick( NiPick* pkPick )
{
	if (!_isInitedWithLoaded() || m_pkModelContent == NULL)
	{
		return false;
	}
	if ( !m_bIsClickable || m_pkPathPickRoot == NULL )
	{
		return false;
	}
	//如果设置PathPickRoot为目标点有些桥就不能被Pick
	//暂时只能这样解决，否则很多桥都不能点
	pkPick->SetTarget(m_spNiRootNode);

	return true;
}

bool GD_ClientSceneModel::_doLoad( const StringType& strModFileName, int iActiveModelContentIndex )
{
	//在后台线程调用
	if(NiDX9Renderer::GetRenderer())
		NiDX9Renderer::GetRenderer()->SetMipmapSkipLevel( g_settings.LOD_SCENE_MIPMAP_SKIPLEVEL );

	if ( !GD_SceneModel::_doLoad( strModFileName , iActiveModelContentIndex ) )
	{
		if(NiDX9Renderer::GetRenderer())
			NiDX9Renderer::GetRenderer()->SetMipmapSkipLevel( 0 );

		return false;
	}
	if(NiDX9Renderer::GetRenderer())
		NiDX9Renderer::GetRenderer()->SetMipmapSkipLevel( 0 );


	Utility::GfxGetAABB( m_spNiRootNode , m_kWorldBound , false );
	m_kWorldBound.Update();
	m_kWorldBound.UpdateCorner();

	// 隐藏子弹碰撞数据
	this->ShowBulletCollision(false);

	bool bPick = m_bPickFlag;
	//有寻路片的物件不一定需要可点击，比如牌坊和门
	if ( HasPathMesh() && bPick )
	{
		m_bIsClickable = true;
	}
	//但是如果有寻路片，就必须要隐藏掉
    m_pkPathPickRoot =  m_spNiRootNode->GetObjectByName( "PICK_MESH" );
	int nPos = UT_BaseStringUtility::FindSubString(m_pkModelContent->GetPathFileName(), SCENE_MODEL_GFX);
	m_bSceneModelFx = IsSceneFxModel( m_spNiRootNode );
	m_bSceneFx = (nPos >= 0) ;

#ifndef _SHIPPING
	Utility::GfxSetDebugName( m_spNiRootNode , GetResourceName().c_str() );
#endif

	return true;
}

bool GD_ClientSceneModel::IsSoftParticle() const
{
	return m_pkModelData && m_pkModelData->m_bSoftParticle;
}

void GD_ClientSceneModel::_doInitAfterLoad(void)
{
	GD_SceneModel::_doInitAfterLoad();

	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	if ( pkSceneManager )
	{
		if ( g_settings.LOD_MODEL_SHADER > 0 || GfxGetWorldConfig().CFG_BAKING_TYPE != Terrain::TS_WorldConfig::NORMAL )
		{
			RG_ClientPointLightManager::GetSingleton().ApplyPointLightsToModel(MAX_DYNAMIC_POINT_LIGHT,
			m_vPLID, true, m_spNiRootNode);
		}
		pkSceneManager->AttachToLightEffect( m_spNiRootNode , m_bSceneFx );
		m_spNiRootNode->UpdateEffects();

		if ( !m_spLightMapTexList.empty() && GfxGetWorldConfig().CFG_BAKING_TYPE != Terrain::TS_WorldConfig::NORMAL )
		{
			if ( m_spLightMapTexList.size() == 1 )
			{
				GfxSetDarkTexture( m_spNiRootNode , m_spLightMapTexList[0] );
			}
			else
			{
				NiVisibleArray kVisibleArray;
				GfxGetGeometrySet( m_spNiRootNode , kVisibleArray );
				if ( kVisibleArray.GetCount() != m_spLightMapTexList.size() )
				{
					GfxWriteLog( LOG_ERROR_FILELINE , LOG_SWITCH_ERROR ,
						"found light map mismatch for model %s , light map array is %d , mesh is %d " ,
						GetResourceName().c_str() , m_spLightMapTexList.size() , kVisibleArray.GetCount() );
				}
				else
				{
					for ( size_t i = 0 ; i < m_spLightMapTexList.size() ; i++ )
					{
						NiTexturingPropertyPtr spTexturingProperty = NiDynamicCast(NiTexturingProperty, kVisibleArray.GetAt(i).GetProperty(NiProperty::TEXTURING));
						if(spTexturingProperty)
						{
							NiTexturingProperty::Map* pkDarkMap = spTexturingProperty->GetDarkMap();
							if ( pkDarkMap == NULL )
							{
								pkDarkMap = NiNew NiTexturingProperty::Map;
								spTexturingProperty->SetDarkMap( pkDarkMap );
							}
							pkDarkMap->SetTexture( m_spLightMapTexList[i] );
						}
					}
				}
			}

			bool bSkinned = false, bGlow = false, bUseUV = false, bUV2 = false;
			NiShaderPtr spSceneShader;
			GfxGetModelType( m_spNiRootNode , bSkinned, bGlow, bUseUV,bUV2);
			if ( !bUV2 )
			{
				if (bSkinned)
				{
					spSceneShader = GfxGetWorldConfig().GetSceneObjectSkinnedShader();
				}
				else
				{
					spSceneShader = GfxGetWorldConfig().GetSceneObjectNoSkinnedShader();
				}
			}
			else
			{
				spSceneShader = GfxGetWorldConfig().GetSceneObjectNoSkinnedIndoorShader(  );
			}
			NiShaderPtr spStaticGlowShader = GfxGetWorldConfig().GetSceneObjectNoSkinnedGlowShader();
			if (bUV2)
			{
				spStaticGlowShader = GfxGetWorldConfig().GetSceneObjectNoSkinnedIndoorGlowShader();
			}
			NiShaderPtr spSkinGlowShader = GfxGetWorldConfig().GetSceneObjectSkinnedGlowShader();
			if (bUV2)
			{
				spSkinGlowShader = GfxGetWorldConfig().GetSceneObjectSkinnedIndoorGlowShader();
			}

			if (bSkinned)
			{
				NiShaderPtr spSkinShader;
				spSkinShader = GfxGetWorldConfig().GetSceneObjectSkinnedIndoorShader();

				GfxSetShaderEx(m_spNiRootNode, spSceneShader, spSkinShader, spStaticGlowShader, spSkinGlowShader , false, bSkinned);
			}
			else
			{

				GfxSetShader(m_spNiRootNode, spSceneShader, spStaticGlowShader, false , bSkinned);
			}

			GfxSetParticleSystemShader(m_spNiRootNode);
		}
	}

	m_kWorldBound.Transform( m_spNiRootNode->GetWorldTransform() );

	if (IsSoftParticle() && SoftParticlesManager::GetInstance())
	{
		SoftParticlesManager::GetInstance()->RecursiveFindAndAddSoftParticles(m_spNiRootNode);
	}

	if ( IsSceneEffectModel() )
		SetVisibility(IM_Manager::GetSingletonPtr()->GetShieldVisibility(0,IM_Manager::SHIELD_SCENE_FX));
}
void GD_ClientSceneModel::_doPreRelease(void)
{
	SE_SceneManager* pkSceneManager = SE_SceneManager::GetSingletonPtr();
	if ( pkSceneManager )
	{
		pkSceneManager->DetachToLightEffect(m_spNiRootNode , m_bSceneFx );
		//m_spNiRootNode->UpdateEffects();
	}

	if (IsSoftParticle() && SoftParticlesManager::GetInstance())
	{
		SoftParticlesManager::GetInstance()->RecursiveFindAndRemoveSoftParticles(m_spNiRootNode);
	}

    GD_SceneModel::_doPreRelease();
}

void GD_ClientSceneModel::SetExtraModelInfo( const ModelInfo& rkModelInfo )
{
	//在主线程调用
    m_bPickFlag = rkModelInfo.bPick;
	//可以行走和穿透的物件都有这个标记。。。。
    m_bWalkable = rkModelInfo.bWalkable;
	m_bRenderReflect = rkModelInfo.bRenderReflect;
	m_spLightMapTexList.clear();
	for ( size_t i = 0 ; i < rkModelInfo.spLightMapTexList.size() ; i++ )
	{
		m_spLightMapTexList.push_back( rkModelInfo.spLightMapTexList[i] );
	}
	//m_spLightMapTexList = rkModelInfo.spLightMapTexList;

	for ( int i = 0 ; i < MAX_DYNAMIC_POINT_LIGHT ; i++ )
	{
		m_vPLID[i] = rkModelInfo.kPLID[i];
	}
}

void GD_ClientSceneModel::AddToVisibleArray( NiVisibleArray& rkVisibleSet )
{
	GfxAddGeometryToSet( m_spNiRootNode , rkVisibleSet );
}

void GD_ClientSceneModel::EnableTransparent( bool bEnable )
{
	if (!_isInitedWithLoaded() || m_pkModelContent == NULL)
	{
		return;
	}
	if ( m_bSceneFx || m_bSceneModelFx  )
	{
		return;
	}
	if ( bEnable != m_bTransparent )
	{
		m_bTransparent = bEnable;
		m_pkModelContent->EnableZWrite( !m_bTransparent );
		if ( g_settings.LOD_MODEL_SHADER == 0 )
		{
			float fAlpha = GetAlpha();
			if ( fAlpha < 1.0f )
			{
				m_pkModelContent->EnableAlphaBlend( true );
			}
			else
			{
				m_pkModelContent->EnableAlphaBlend( false );
			}
		}
	}

}

void GD_ClientSceneModel::UpdateAlphaBlend()
{
	if (!_isInitedWithLoaded() || m_pkModelContent == NULL)
	{
		return;
	}
	
	float fAlpha = GetAlpha();
	bool trans = ( fAlpha < 1.0f );
	if (trans != m_bTransparent)
	{
		m_bTransparent = trans;
	//	if ( g_settings.LOD_MODEL_SHADER == 0  )
		{
			if ( fAlpha < 1.0f )
			{
				m_pkModelContent->EnableAlphaBlend( true );
			}
			else
			{
				m_pkModelContent->EnableAlphaBlend( false );
			}
		}
	}


}

bool GD_ClientSceneModel::_doIsUpdatable(float fTime)
{
	if ( !g_settings.LOD_SCENE_ANIM_UPDATE )
	{
		return false;
	}

	if ( m_bSceneFx || m_bSceneModelFx )
	{
		// 避免由于 pre-cull 跳过了实际 cull 操作导致视野外的模型仍然被 update 的问题
		if ( IM_Manager::GetSingletonPtr()->IsPendingShowOrHideShield(IM_Manager::SHIELD_SCENE_FX) )
			SetVisibility(IM_Manager::GetSingletonPtr()->GetShieldVisibility(0,IM_Manager::SHIELD_SCENE_FX));
	}

	if (IsSoftParticle())
	{
		return true;
	}

    if (GD_SceneModel::_doIsUpdatable(fTime)
        || (IsSceneEffectModel() && (fTime - m_fLastUpdateTime >= 0.25f)))
    {
        m_fLastUpdateTime = fTime;

        return true;
    }

    return false;
}

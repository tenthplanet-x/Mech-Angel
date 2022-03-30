//---------------------------------------------------------------------------------------------
#include "ClientPCH.h"
#include "UI_AvatarRender.h"
#include "UI_AvatarManager.h"
#include "IG_ImageManager.h"
#include "IG_Material.h"
#include "IG_MaterialManager.h"
#include "FX_ClientFxManager.h"
#include "RG_RenderManager.h"
#include "RG_AvatarRenderClick.h"
#include "RG_Dx9MsaaRenderedTex.h"
#include "SE_SceneManager.h"
#include "GD_ClientActorModel.h"

#include "NiCamera.h"

using namespace GUISys;
using namespace Render;
//---------------------------------------------------------------------------------------------
#define RENDER_TEXTURE_WIDTH				2048
#define RENDER_TEXTURE_HEIGHT				2048
#define RENDER_SMALL_TEXTURE_WIDTH			1024
#define RENDER_SMALL_TEXTURE_HEIGHT			2048
#define RENDER_DEPTHBUFFER_TEXTURE_WIDTH	256
#define RENDER_DEPTHBUFFER_TEXTURE_HEIGHT	512

#define RENDER_TEXTURE_NAME					"AvatarTexture"
#define RENDER_MATERIAL_NAME				"AvatarMaterial"
#define RENDER_SMALL_TEXTURE_NAME			"AvatarSmallTexture"
#define RENDER_SMALL_MATERIAL_NAME			"AvatarSmallMaterial"
#define RENDER_DEPTHBUFFER_TEXTURE_NAME		"AvatarDepthBufferTexture"
#define RENDER_DEPTHBUFFER_MATERIAL_NAME	"AvatarDepthBufferMaterial"
//---------------------------------------------------------------------------------------------
// UI_AvatarRender::ShaderParams
//---------------------------------------------------------------------------------------------
UI_AvatarRender::ShaderParams::ShaderParams()
				:m_iCurrParamIndex(-1)
				,m_kFogColorRestore(NiColor::BLACK)
				,m_vDepthFogParamRestore(NiColorA::BLACK)
				,m_vHeightFogParamRestore(NiColorA::BLACK)
				,m_fCharacterRimLightPowerRestore(0.0f)
				,m_fCharacterIllumBiasRestore(0.0f)
				,m_fCharacterAmbientPowerRestore(0.0f)
				,m_fCharacterSunPowerRestore(0.0f)
				,m_fCharacterMatEmmisiveRestore(0.0f)
				,m_fCharacterSelfIlluminationRestore(0.0f)
				,m_fCharacterSpecularPowerRestore(0.0f)
				,m_fCharacterSpecularMultiplierRestore(0.0f)
				,m_vCharacterAmbientColorRestore(NiColor::BLACK)
				,m_vCharacterRimLightColorRestore(NiColor::BLACK)
				,m_vCharacterDiffuseLightColorRestore(NiColor::BLACK)
				,m_vCharacterLightWorldDirRestore(NiPoint3(0.0f, 0.0f, 0.0f))
{
	m_kFogColor = NiColor(0.015686275f, 0.49019608f, 0.70980394f);
	//m_vDepthFogParam = NiColorA(800.f, 2200.0f, 1.500f, 0.00062f);
	m_vDepthFogParam = NiColorA(0.f, 0.0f, 0.0f, 0.0f);
	m_vHeightFogParam = NiColorA(0.0f, 0.0f, 0.0f, 0.0f);

	const StringType SECTION_NAME[] = {"Actor", "WorldMap", "TreasureChests"};
	UT_ConfigFile kConfig;
	int iSection = kConfig.Load("Data\\Effect\\ui_avatar_light_setting.ini");
	if (iSection > 0)
	{
		for (int i = 0; i < 3; i++)
		{
			m_fCharacterAmbientPower[i] = 
				UT_EngineStringUtility::GetFloatFromString(kConfig, SECTION_NAME[i], "g_fCharacterAmbientPower", 0.2f);
			m_fCharacterSunPower[i] = 
				UT_EngineStringUtility::GetFloatFromString(kConfig, SECTION_NAME[i], "g_fCharacterSunPower", 1.0f);
			m_fCharacterMatEmmisive[i] = 
				UT_EngineStringUtility::GetFloatFromString(kConfig, SECTION_NAME[i], "g_fCharacterMatEmmisive", 0.38f);
			m_fCharacterSelfIllumination[i] = 
				UT_EngineStringUtility::GetFloatFromString(kConfig, SECTION_NAME[i], "g_fCharacterSelfIllumination", 0.05f);
			m_fCharacterSpecularPower[i] = 
				UT_EngineStringUtility::GetFloatFromString(kConfig, SECTION_NAME[i], "g_fCharacterSpecularPower", 8.0f);
			m_fCharacterSpecularMultiplier[i] = 
				UT_EngineStringUtility::GetFloatFromString(kConfig, SECTION_NAME[i], "g_fCharacterSpecularMultiplier", 4.0f);
			m_vCharacterDiffuseLightColor[i] = 
				UT_EngineStringUtility::GetNiColorFromString(kConfig, SECTION_NAME[i], "g_vCharacterDiffuseLightColor");
			m_vCharacterAmbientColor[i] = 
				UT_EngineStringUtility::GetNiColorFromString(kConfig, SECTION_NAME[i], "g_vCharacterAmbientColor");
			m_vCharacterLightWorldDir[i] = 
				UT_EngineStringUtility::GetNiPoint3SplitFromString(kConfig, SECTION_NAME[i], "g_vCharacterLightWorldDir");
			m_vCharacterRimLightColor[i] = 
				UT_EngineStringUtility::GetNiColorFromString(kConfig, SECTION_NAME[i], "g_vCharacterRimLightColor");
			m_fCharacterRimLightPower[i] = 
				UT_EngineStringUtility::GetFloatFromString(kConfig, SECTION_NAME[i], "g_fCharacterRimLightPower");
			m_fCharacterIllumBias[i] = 
				UT_EngineStringUtility::GetFloatFromString(kConfig, SECTION_NAME[i], "g_fCharacterIllumBias");
		}
	}
}


void UI_AvatarRender::ShaderParams::SetupGlobalParams()
{
	NiShaderFactory::UpdateGlobalShaderConstant("g_vDepthFogParam",
		sizeof(NiColorA), &m_vDepthFogParam);

	NiShaderFactory::UpdateGlobalShaderConstant("g_vHeightFogParam",
		sizeof(NiColorA), &m_vHeightFogParam);
}

void UI_AvatarRender::ShaderParams::SetupLocalParams( size_t iParamIndex )
{
	if ( iParamIndex < 0 || iParamIndex > 2 )
		iParamIndex = 0;

	if ( m_iCurrParamIndex != iParamIndex )
	{
		SE_SceneManager::GetSingleton().UpdateCharacterShaderGlobalParams(
			m_fCharacterAmbientPower[iParamIndex],		m_fCharacterSunPower[iParamIndex], 
			m_fCharacterMatEmmisive[iParamIndex],		m_fCharacterSelfIllumination[iParamIndex], 
			m_fCharacterSpecularPower[iParamIndex],		m_fCharacterSpecularMultiplier[iParamIndex], 
			m_vCharacterDiffuseLightColor[iParamIndex], m_vCharacterAmbientColor[iParamIndex], 
			m_vCharacterLightWorldDir[iParamIndex],		m_vCharacterRimLightColor[iParamIndex], 
			m_fCharacterRimLightPower[iParamIndex],		m_fCharacterIllumBias[iParamIndex]);

		m_iCurrParamIndex = iParamIndex;
	}
}

void UI_AvatarRender::ShaderParams::PreRender()
{
	//@ Restore fog param
	{
		const void* gsEntityData = NULL;
		unsigned int gsEntryDataSize = 0;

		NiShaderFactory::RetrieveGlobalShaderConstant("g_vDepthFogParam", gsEntryDataSize, gsEntityData);
		if ( gsEntryDataSize >= sizeof(NiColorA)) m_vDepthFogParamRestore = *((NiColorA*)gsEntityData);

		NiShaderFactory::RetrieveGlobalShaderConstant("g_vHeightFogParam",gsEntryDataSize, gsEntityData);
		if ( gsEntryDataSize >= sizeof(NiColorA)) m_vHeightFogParamRestore = *((NiColorA*)gsEntityData);
	}

	//@ Restore scene illumination param
	SE_SceneManager::GetSingleton().GetCharacterShaderGlobalParams(
		m_fCharacterAmbientPowerRestore,	m_fCharacterSunPowerRestore, 
		m_fCharacterMatEmmisiveRestore,		m_fCharacterSelfIlluminationRestore, 
		m_fCharacterSpecularPowerRestore,	m_fCharacterSpecularMultiplierRestore, 
		m_vCharacterDiffuseLightColorRestore, m_vCharacterAmbientColorRestore, 
		m_vCharacterLightWorldDirRestore,	m_vCharacterRimLightColorRestore, 
		m_fCharacterRimLightPowerRestore,	m_fCharacterIllumBiasRestore);

	m_iCurrParamIndex = -1;

	//@ 设置全局参数
	SetupGlobalParams();
}

void UI_AvatarRender::ShaderParams::PostRender()
{
	//@ Recover fog param
	{
		NiShaderFactory::UpdateGlobalShaderConstant("g_vDepthFogParam",
			sizeof(NiColorA), &m_vDepthFogParamRestore);

		NiShaderFactory::UpdateGlobalShaderConstant("g_vHeightFogParam",
			sizeof(NiColorA), &m_vHeightFogParamRestore);
	}

	//@ Recover scene illumination param
	if ( m_iCurrParamIndex >= 0 )
	{
		SE_SceneManager::GetSingleton().UpdateCharacterShaderGlobalParams(
			m_fCharacterAmbientPowerRestore,	m_fCharacterSunPowerRestore, 
			m_fCharacterMatEmmisiveRestore,		m_fCharacterSelfIlluminationRestore, 
			m_fCharacterSpecularPowerRestore,	m_fCharacterSpecularMultiplierRestore, 
			m_vCharacterDiffuseLightColorRestore, m_vCharacterAmbientColorRestore, 
			m_vCharacterLightWorldDirRestore,	m_vCharacterRimLightColorRestore, 
			m_fCharacterRimLightPowerRestore,	m_fCharacterIllumBiasRestore);

		m_iCurrParamIndex = -1;
	}
}
//---------------------------------------------------------------------------------------------
// UI_AvatarRender::ObjectGroup
//---------------------------------------------------------------------------------------------

UI_AvatarRender::ObjectGroup::ObjectGroup(const StringType& strGroupName)
							:m_bActived(false)
							,m_bMainQueueDirty(false)
							,m_bBackGroundQueueDirty(false)
							,m_eUpdateState(0)
							,m_fWaitingTimeForDiscard(0.0f)
							,m_strGroupName(strGroupName)
{
	
}

void UI_AvatarRender::ObjectGroup::Update(UI_AvatarRender* render, float fTime, float fTimeDelta)
{
	//@ backGroundQueue
	{
		bool& objectQueueDirty = m_bBackGroundQueueDirty;
		ObjectQueue& objectQueue = m_vBackGroundQueue;
		AvatarElementKeyId keyId = Invalid_AvatarElementKeyId;

		size_t queueOffset = 0;
		size_t queueSize = objectQueue.size();
		while ( queueOffset < queueSize )
		{
			keyId = objectQueue[queueOffset++];

			if ( keyId )
			{
				render->onConfigCamera(keyId);
				render->onUpdateObject(keyId, fTime, fTimeDelta);
			}
		}
	}
	//@ mainQueue
	{
		bool& objectQueueDirty = m_bMainQueueDirty;
		ObjectQueue& objectQueue = m_vMainQueue;
		AvatarElementKeyId keyId = Invalid_AvatarElementKeyId;

		size_t queueOffset = 0;
		size_t queueSize = objectQueue.size();
		while ( queueOffset < queueSize )
		{
			keyId = objectQueue[queueOffset++];

			if ( keyId )
			{
				render->onConfigCamera(keyId);
				render->onUpdateObject(keyId, fTime, fTimeDelta);
			}
		}
	}
}

void UI_AvatarRender::ObjectGroup::Render(UI_AvatarRender* render, RENDER_CLICK_INDEX rdClickIdx)
{
	//@ backGroundQueue
	{
		bool& objectQueueDirty = m_bBackGroundQueueDirty;
		ObjectQueue& objectQueue = m_vBackGroundQueue;
		AvatarElementKeyId keyId = Invalid_AvatarElementKeyId;

		if ( objectQueueDirty )
		{
			render->onClearGroupDirty(objectQueue);
			objectQueueDirty = false;
		}

		int queueSize = objectQueue.size();
		int queueOffset = queueSize - 1;
		while ( queueOffset >= 0 )
		{
			keyId = objectQueue[queueOffset--];
			if ( keyId != Invalid_AvatarElementKeyId && render->IsShouldRender(keyId, rdClickIdx))
			{
				render->onConfigCamera(keyId);
				render->onConfigShader(keyId);
				render->onRenderSolidPass(keyId);
			}
		}

		queueOffset = 0;
		while ( queueOffset < queueSize )
		{
			keyId = objectQueue[queueOffset++];
			if ( keyId != Invalid_AvatarElementKeyId && render->IsShouldRender(keyId, rdClickIdx))
			{
				render->onConfigCamera(keyId);
				render->onConfigShader(keyId);
				render->onRenderTransparentPass(keyId, rdClickIdx);
			}
		}
	}
	//@ mainQueue
	{
		bool& objectQueueDirty = m_bMainQueueDirty;
		ObjectQueue& objectQueue = m_vMainQueue;
		AvatarElementKeyId keyId = Invalid_AvatarElementKeyId;

		if ( objectQueueDirty )
		{
			render->onClearGroupDirty(objectQueue);
			objectQueueDirty = false;
		}

		int queueSize = objectQueue.size();
		int queueOffset = queueSize - 1;
		while ( queueOffset >= 0 )
		{
			keyId = objectQueue[queueOffset--];
			if ( keyId != Invalid_AvatarElementKeyId && render->IsShouldRender(keyId, rdClickIdx))
			{
				render->onConfigCamera(keyId);
				render->onConfigShader(keyId);
				render->onRenderSolidPass(keyId);
			}
		}

		queueOffset = 0;
		while ( queueOffset < queueSize )
		{
			keyId = objectQueue[queueOffset++];
			if ( keyId != Invalid_AvatarElementKeyId && render->IsShouldRender(keyId, rdClickIdx))
			{
				render->onConfigCamera(keyId);
				render->onConfigShader(keyId);
				render->onRenderTransparentPass(keyId, rdClickIdx);
			}
		}
	}
}

void UI_AvatarRender::ObjectGroup::ConfigObjectsShader(UI_AvatarRender* render, RENDER_CLICK_INDEX rdClickIdx, bool bPre)
{
	AvatarElementKeyId keyId = Invalid_AvatarElementKeyId;

	{
		ObjectQueue& objectQueue = m_vMainQueue;

		size_t queueOffset = 0;
		size_t queueSize = objectQueue.size();
		while ( queueOffset < queueSize )
		{
			keyId = objectQueue[queueOffset++];
			render->onConfigObjectShader(keyId, rdClickIdx, bPre);
		}
	}
	{
		ObjectQueue& objectQueue = m_vBackGroundQueue;

		size_t queueOffset = 0;
		size_t queueSize = objectQueue.size();
		while ( queueOffset < queueSize )
		{
			keyId = objectQueue[queueOffset++];
			render->onConfigObjectShader(keyId, rdClickIdx, bPre);
		}
	}
}

bool UI_AvatarRender::ObjectGroup::Active(UI_AvatarRender* render)
{
	//@ 增加引用计数
	bool isCaseSwitched = false;
	if ( !m_bActived )
	{
		m_bActived = true;
		isCaseSwitched = true;
	}

	//@ 通知render
	if ( isCaseSwitched )
	{
		{
			ObjectQueue& objectQueue = m_vMainQueue;

			size_t queueOffset = 0;
			size_t queueSize = objectQueue.size();
			while ( queueOffset < queueSize )
			{
				render->onActiveObject(objectQueue[queueOffset++]);
			}
		}
		{
			ObjectQueue& objectQueue = m_vBackGroundQueue;

			size_t queueOffset = 0;
			size_t queueSize = objectQueue.size();
			while ( queueOffset < queueSize )
			{
				render->onActiveObject(objectQueue[queueOffset++]);
			}
		}
	}

	return isCaseSwitched;
}

bool UI_AvatarRender::ObjectGroup::Deactive(UI_AvatarRender* render)
{
	//@ 释放引用计数
	bool isCaseSwitched = false;
	if ( m_bActived )
	{
		m_bActived = false;;
		isCaseSwitched = true;
	}

	//@ 通知render
	if ( isCaseSwitched )
	{
		{
			ObjectQueue& objectQueue = m_vMainQueue;

			size_t queueOffset = 0;
			size_t objectSize = objectQueue.size();
			while ( queueOffset < objectSize )
			{
				render->onDeactiveObject(objectQueue[queueOffset++]);
			}
		}
		//@ 通知backGroundQueue
		{
			ObjectQueue& objectQueue = m_vBackGroundQueue;

			size_t queueOffset = 0;
			size_t queueSize = objectQueue.size();
			while ( queueOffset < queueSize )
			{
				render->onDeactiveObject(objectQueue[queueOffset++]);
			}
		}
	}

	return isCaseSwitched;
}


void UI_AvatarRender::ObjectGroup::AddObject(AvatarElementKeyId keyId, bool isAddBackGround/* = false*/)
{
	//@ 向objectGroup中加入所需要的元素
	bool& objectQueueDirty = (!isAddBackGround) ? m_bMainQueueDirty : m_bBackGroundQueueDirty;
	ObjectQueue& objectQueue = (!isAddBackGround) ? m_vMainQueue : m_vBackGroundQueue;

	size_t queueOffset = 0;
	size_t queueSize = objectQueue.size();
	while ( queueOffset < queueSize )
	{
		// 判断重复元素;
		if ( objectQueue[queueOffset++] == keyId )
		{
			T_ASSERT2(true, "有重复的avatar元素被加入到绘制队列中！");
			return;
		}
	}

	queueOffset = 0;
	while ( queueOffset < queueSize )
	{
		// 检查可复用元素;
		if ( objectQueue[queueOffset] == Invalid_AvatarElementKeyId )
		{
			objectQueue[queueOffset] = keyId;
			objectQueueDirty = true;
			return;
		}
		queueOffset++;
	}
	// 没有可复用元素，增加新元素;
	objectQueue.push_back(keyId);
	objectQueueDirty = true;
}

void UI_AvatarRender::ObjectGroup::RemoveObject(AvatarElementKeyId keyId)
{
	//@ mainElements
	{
		bool& objectQueueDirty = m_bMainQueueDirty;
		ObjectQueue& objectQueue = m_vMainQueue;

		size_t queueOffset = 0;
		size_t queueSize = objectQueue.size();
		while ( queueOffset < queueSize )
		{
			if ( objectQueue[queueOffset] == keyId )
			{
				objectQueue[queueOffset] = Invalid_AvatarElementKeyId;
				objectQueueDirty = true;
				return;
			}
			queueOffset++;
		}
	}
	//@ backGroundElements
	{
		bool& objectQueueDirty = m_bBackGroundQueueDirty;
		ObjectQueue& objectQueue = m_vBackGroundQueue;

		size_t queueOffset = 0;
		size_t queueSize = objectQueue.size();
		while ( queueOffset < queueSize )
		{
			if ( objectQueue[queueOffset] == keyId )
			{
				objectQueue[queueOffset] = Invalid_AvatarElementKeyId;
				objectQueueDirty = true;
				return;
			}
			queueOffset++;
		}
	}
}

void UI_AvatarRender::ObjectGroup::RemoveAllObject()
{
	//@ mainQueue
	{
		bool& objectQueueDirty = m_bMainQueueDirty;
		ObjectQueue& objectQueue = m_vMainQueue;

		size_t queueOffset = 0;
		size_t queueSize = objectQueue.size();
		while ( queueOffset < queueSize )
		{
			objectQueue[queueOffset++] = Invalid_AvatarElementKeyId;
		}
	}
	//@ backGroundQueue
	{
		bool& objectQueueDirty = m_bBackGroundQueueDirty;
		ObjectQueue& objectQueue = m_vBackGroundQueue;

		size_t queueOffset = 0;
		size_t queueSize = objectQueue.size();
		while ( queueOffset < queueSize )
		{
			objectQueue[queueOffset++] = Invalid_AvatarElementKeyId;
		}
	}
}

//---------------------------------------------------------------------------------------------
// UI_AvatarRender::ObjectDepthSorter
//---------------------------------------------------------------------------------------------
UI_AvatarRender::ObjectDepthSorter::ObjectDepthSorter( UI_AvatarManager* pkManager )
									:m_pkManager(pkManager)
									,m_iNumDepthItems(0)
									,m_iMaxDepthItems(0)
									,m_pfDepthItems(NULL)
{
}

UI_AvatarRender::ObjectDepthSorter::~ObjectDepthSorter()
{
	if ( m_pfDepthItems ) 
		T_FREE(m_pfDepthItems, Memory::MEMCATEGORY_GENERAL);
}

void UI_AvatarRender::ObjectDepthSorter::Sort(ObjectGroup::ObjectQueue& objectQueue)
{
	if ( objectQueue.size() > 1 )
	{
		//@ 获取深度信息
		collectDepthItems(objectQueue);

		//@ 根据深度信息对objectQueue排序 
		sortDepthItems(0, m_iNumDepthItems - 1, objectQueue);
	}	
}

void UI_AvatarRender::ObjectDepthSorter::collectDepthItems(ObjectGroup::ObjectQueue& objectQueue)
{
	size_t iNumCurrentItems = objectQueue.size(); 
	if ( iNumCurrentItems > m_iMaxDepthItems )
	{
		if ( m_pfDepthItems ) 
			T_FREE(m_pfDepthItems, Memory::MEMCATEGORY_GENERAL);

		m_pfDepthItems = T_ALLOC(float, iNumCurrentItems, Memory::MEMCATEGORY_GENERAL);
		memset(m_pfDepthItems, 0, sizeof(float)* iNumCurrentItems);

		m_iMaxDepthItems = iNumCurrentItems;
	}
	m_iNumDepthItems = iNumCurrentItems;

	AvatarElementPtr pkAvatarElement = NULL;
	for ( size_t i = 0; i < m_iNumDepthItems; ++i )
	{
		pkAvatarElement = m_pkManager->GetAvatar(objectQueue[i]);
		if ( pkAvatarElement && pkAvatarElement->pkActorModel )
			m_pfDepthItems[i] = pkAvatarElement->pkActorModel->GetTranslate().y;
	}
}

void UI_AvatarRender::ObjectDepthSorter::sortDepthItems(int left, int right, ObjectGroup::ObjectQueue& objectQueue)
{
	if (right > left)
	{
		int i = left - 1;
		int j = right + 1;
		float fPivot = chooseDepthSortPivot(left, right);

		while (true)
		{
			do 
			{
				j--;
			} while (fPivot > m_pfDepthItems[j]);

			do
			{
				i++;
			} while (m_pfDepthItems[i] > fPivot);

			if (i < j)
			{
				AvatarElementKeyId keyIdTemp = objectQueue[i];
				objectQueue[i] = objectQueue[j];
				objectQueue[j] = keyIdTemp;

				float fTempDepth = m_pfDepthItems[i];
				m_pfDepthItems[i] = m_pfDepthItems[j];
				m_pfDepthItems[j] = fTempDepth;
			}
			else
			{
				break;
			}
		}

		if (j == right)
		{
			sortDepthItems(left, j - 1, objectQueue);
		}
		else
		{
			sortDepthItems(left, j, objectQueue);
			sortDepthItems(j + 1, right, objectQueue);
		}
	}
}

float UI_AvatarRender::ObjectDepthSorter::chooseDepthSortPivot(int left, int right)  const
{
	int middle = (left + right) >> 1;

	const float fDepth_l = m_pfDepthItems[left];
	const float fDepth_r = m_pfDepthItems[right];
	const float fDepth_m = m_pfDepthItems[middle];

	if (fDepth_l > fDepth_m)
	{
		if (fDepth_m > fDepth_r)
		{
			return fDepth_m;
		}
		else
		{
			if (fDepth_l > fDepth_r)
			{
				return fDepth_r;
			}
			else
			{
				return fDepth_l;
			}
		}
	}
	else
	{
		if (fDepth_l > fDepth_r)
		{
			return fDepth_l;
		}
		else
		{
			if (fDepth_m > fDepth_r)
			{
				return fDepth_r;
			}
			else
			{
				return fDepth_m;
			}
		}
	}
}

//---------------------------------------------------------------------------------------------
// UI_AvatarRender
//---------------------------------------------------------------------------------------------
UI_AvatarRender::UI_AvatarRender(UI_AvatarManager* parentManager)
				:m_pkManager(parentManager)
				,m_spRenderer(NULL)
				,m_spCamera(NULL)
				,m_kVisibleSet(16, 8)
				,m_kCuller(&m_kVisibleSet)
				,m_pkObjectDepthSorter(NULL)
				,m_spRenderTarget(NULL)
				,m_spSmallRenderTarget(NULL)
				,m_spNoDepthBufferRenderTarget(NULL)
				,m_spGuiMaterial(NULL)
				,m_spGuiMaterialSmall(NULL)
				,m_spGuiMaterialDepthBuffer(NULL)
				,m_bInitSuccess(false)
				,m_fObjectGroupExpiredLife(10.0f)
				,m_fObjectGroupExpiredLifeScale(1.0f)
				,m_fObjectGroupExpiredRealLife(10.0f)
{
	m_spRenderer = (NiDX9Renderer*)NiRenderer::GetRenderer();

	m_spCamera = _createCamera();

	if ( _createRenderTexture(m_kRenderTexture, RENDER_TEXTURE_NAME, RENDER_TEXTURE_WIDTH, RENDER_TEXTURE_HEIGHT) )
	{
		m_spRenderTarget = _createRenderTarget(m_kRenderTexture);
		m_spGuiMaterial = _createGUIMaterial(m_kRenderTexture, RENDER_MATERIAL_NAME);

	}

	if ( _createRenderTexture(m_kSmallRenderTexture, RENDER_SMALL_TEXTURE_NAME, RENDER_SMALL_TEXTURE_WIDTH, RENDER_SMALL_TEXTURE_HEIGHT) )
	{
		m_spSmallRenderTarget = _createRenderTarget(m_kSmallRenderTexture);
		m_spGuiMaterialSmall = _createGUIMaterial(m_kSmallRenderTexture, RENDER_SMALL_MATERIAL_NAME);
	}

	if ( _createRenderTexture(m_kNoDepthBufferRenderTexture, RENDER_DEPTHBUFFER_TEXTURE_NAME, RENDER_DEPTHBUFFER_TEXTURE_WIDTH, RENDER_DEPTHBUFFER_TEXTURE_HEIGHT, false) )
	{
		m_spNoDepthBufferRenderTarget = _createRenderTarget(m_kNoDepthBufferRenderTexture);
		m_spGuiMaterialDepthBuffer = _createGUIMaterial(m_kNoDepthBufferRenderTexture, RENDER_DEPTHBUFFER_MATERIAL_NAME);
	}

	m_pkObjectDepthSorter = _createDepthSorter();

	m_bInitSuccess = ( m_spRenderer && m_spCamera && 
		m_spGuiMaterial && m_spGuiMaterialSmall && m_spRenderTarget && m_spSmallRenderTarget && m_spNoDepthBufferRenderTarget && m_pkObjectDepthSorter );

	SetObjectGroupExpiredLifeScale(m_fObjectGroupExpiredLifeScale);
}

UI_AvatarRender::~UI_AvatarRender()
{
	_destroyAllObjectGroup();

	_destroyCamera(m_spCamera);

	_destroyAllGUIImage();
	_destroyGUIMaterial(m_spGuiMaterial);
	_destroyGUIMaterial(m_spGuiMaterialSmall);
	_destroyGUIMaterial(m_spGuiMaterialDepthBuffer);

	_destroyRenderTarget(m_spRenderTarget);
	_destroyRenderTarget(m_spSmallRenderTarget);
	_destroyRenderTarget(m_spNoDepthBufferRenderTarget);
	_destroyRenderTexture(m_kRenderTexture);
	_destroyRenderTexture(m_kSmallRenderTexture);
	_destroyRenderTexture(m_kNoDepthBufferRenderTexture);

	_destroyDepthSorter(m_pkObjectDepthSorter);
}

void UI_AvatarRender::HandleReset(void)
{
	/*
	ref: RG_Dx9MsaaRenderedTex
	AvatarTexture在设备丢失时
		如果原先是MSAA、现不是，则只重新创建RT，不创建Texture；因此只要重新设置RT即可
		如果原先不是MSAA、现在是，则保持原RT和Texture不变；等下次重启才会变成MSAA
		这是AvatarTexture不同于其它PostProcessTexture的地方，由于被多个地方引用，如果要全部重新创建，则牵涉流程较多且复杂
	*/
	/// 清除引用计数
	m_spRenderTarget = NULL;
	m_spSmallRenderTarget = NULL;
	m_spNoDepthBufferRenderTarget = NULL;

	/// 重设新RT的引用计数
	m_spRenderTarget = _createRenderTarget(m_kRenderTexture);
	m_spSmallRenderTarget = _createRenderTarget(m_kSmallRenderTexture);
	m_spNoDepthBufferRenderTarget = _createRenderTarget(m_kNoDepthBufferRenderTexture);
}


void UI_AvatarRender::ActiveObjects(const StringType& strGroupName)
{
	//@ 创建或者获取visibleGroup;
	ObjectGroup* visibleGroup = _retrieveObjectGroup(strGroupName);
	if ( visibleGroup == NULL )
		visibleGroup = _createObjectGroup(strGroupName);

	if ( visibleGroup && 
		visibleGroup->Active(this) &&
		!visibleGroup->IsEmpty() )
	{
		_removeFromObjectGroupExpiredList(visibleGroup);
		_addToObjectGroupUpdateList(visibleGroup);
	}
}

void UI_AvatarRender::DeactiveObjects(const StringType& strGroupName)
{
	//@ 创建或者获取visibleGroup;
	ObjectGroup* visibleGroup = _retrieveObjectGroup(strGroupName);
	if ( visibleGroup == NULL )
		return;

	if ( visibleGroup->Deactive(this) )
	{
		_removeFromObjectGroupUpdateList(visibleGroup);
		_addToObjectGroupExpiredList(visibleGroup);
	}
}

bool UI_AvatarRender::IsObjectsActived(const StringType& strGroupName)
{
	bool isActived = false;

	ObjectGroup* visibleGroup = _retrieveObjectGroup(strGroupName);
	if ( visibleGroup )
	{
		isActived = visibleGroup->IsActive();
	}
	
	return isActived;
}

void UI_AvatarRender::AddObject(AvatarElementKeyId keyId, const StringType& strGroupName, bool isAddBackGround/* = false*/)
{
	//@ 创建或者获取renderGroup;
	ObjectGroup* visibleGroup = _retrieveObjectGroup(strGroupName);
	if ( !visibleGroup )
		visibleGroup = _createObjectGroup(strGroupName);

	T_ASSERT2( visibleGroup != NULL, "获取avatar绘制队列失败！" );
	if ( visibleGroup == NULL )
		return;

	//@ 插入到具体队列
	bool isEmpty = visibleGroup->IsEmpty();
	visibleGroup->AddObject(keyId, isAddBackGround);

	//@ 是否需要重新放回到更新队列
	if ( isEmpty )
	{
		_removeFromObjectGroupExpiredList(visibleGroup);
		_addToObjectGroupUpdateList(visibleGroup);
	}
}

void UI_AvatarRender::RemoveObject(AvatarElementKeyId keyId, const StringType& strGroupName)
{
	//@ 获取renderGroup;
	ObjectGroup* visibleGroup = _retrieveObjectGroup(strGroupName);
	if ( visibleGroup == NULL )
		return;

	//@ 从具体队列移除
	visibleGroup->RemoveObject(keyId);

	//@ 检查可否从更新队列中移除 
	if ( visibleGroup->IsEmpty() )
	{
		_removeFromObjectGroupExpiredList(visibleGroup);
		_addToObjectGroupExpiredList(visibleGroup);
	}
}

void UI_AvatarRender::RemoveAllObjects()
{
	_destroyAllObjectGroup();
}

void UI_AvatarRender::UpdateObjects(float fTime, float fDeltaTime)
{
	//@ 更新discardList
	_updateObjectGroupExpiredList(fDeltaTime);

	//@ 更新updateList
	if ( IsAnyObjectActived() )
	{
		//@ 设置fxManager参数配置
		FX_ClientFxManager& ClientFxManager = FX_ClientFxManager::GetSingleton();
		const NiCameraPtr spOldFxCamera = ClientFxManager.GetCamera();
		ClientFxManager.SetCamera(m_spCamera);

		//@ 更新
		ObjectGroupList::iterator groupIter = m_kObjectGroupUpdateList.begin();
		ObjectGroupList::iterator groupIterEnd = m_kObjectGroupUpdateList.end();
		while ( groupIter != groupIterEnd )
			(*(groupIter++))->Update(this, fTime, fDeltaTime);

		//@ 恢复fxManager参数配置
		ClientFxManager.SetCamera(spOldFxCamera);
	}
}

void UI_AvatarRender::RenderObjects(RG_AvatarRenderClick* renderClick)
{ 
	//@ 设置fxManager参数配置
	FX_ClientFxManager& ClientFxManager = FX_ClientFxManager::GetSingleton();
	const NiCameraPtr spOldFxCamera = ClientFxManager.GetCamera();
	ClientFxManager.SetCamera(m_spCamera);

	//@ Shader参数配置
	m_kShaderParams.PreRender();

	//@ 提交批次
	ObjectGroupList::iterator groupIter = m_kObjectGroupUpdateList.begin();
	ObjectGroupList::iterator groupIterEnd = m_kObjectGroupUpdateList.end();

	while ( groupIter != groupIterEnd )
		(*(groupIter++))->Render(this, (RENDER_CLICK_INDEX)renderClick->GetIndex());

	//@ 提交贴图
	m_kRenderTexture.Update();
	m_kSmallRenderTexture.Update();
	m_kNoDepthBufferRenderTexture.Update();

	//@ 恢复shader参数配置
	m_kShaderParams.PostRender();

	//@ 恢复fxManager参数配置
	ClientFxManager.SetCamera(spOldFxCamera);
}

void UI_AvatarRender::ConfigObjectsShader(RENDER_CLICK_INDEX rdClickIdx, bool bPre)
{
	ObjectGroupList::iterator groupIter = m_kObjectGroupUpdateList.begin();
	ObjectGroupList::iterator groupIterEnd = m_kObjectGroupUpdateList.end();

	while ( groupIter != groupIterEnd )
		(*(groupIter++))->ConfigObjectsShader(this, rdClickIdx, bPre);
}

void UI_AvatarRender::RemoveAllExpiredObjectGroup()
{
	ObjectGroup* pkObjectGroup = NULL;

	ObjectGroupList::iterator iter = 
		m_kObjectGroupExpiredList.begin();
	ObjectGroupList::iterator iend = 
		m_kObjectGroupExpiredList.end();

	while( iter != iend )
	{	
		pkObjectGroup = (*iter++);
		_destroyExpiredObjectGroup(pkObjectGroup);
	}

	m_kObjectGroupExpiredList.clear();
}

void UI_AvatarRender::RemoveExpiredObjectGroup( int iRemoveCount )
{
	if ( iRemoveCount < 1 )
		return;

	int iCurrentCount = 0;
	ObjectGroup* pkObjectGroup = NULL;

	ObjectGroupList::iterator iter = 
		m_kObjectGroupExpiredList.begin();

	while( !m_kObjectGroupExpiredList.empty() && 
		iCurrentCount < iRemoveCount )
	{	
		pkObjectGroup = (*iter);
		_destroyExpiredObjectGroup(pkObjectGroup);

		iter = m_kObjectGroupExpiredList.erase(iter);
		++iCurrentCount;
	}

}

UI_AvatarRender::ObjectGroup* UI_AvatarRender::_createObjectGroup(const StringType& strGroupName)
{
	ObjectGroup* pkObjectGroup = NULL;

	ObjectGroups::iterator groupIter = m_kObjectGroups.find(strGroupName);
	if ( groupIter != m_kObjectGroups.end() )
	{
		T_ASSERT2( false, "重复创建avatar绘制队列！" );
		return NULL;
	}

	pkObjectGroup = T_NEW_D ObjectGroup(strGroupName);

	ObjectGroups::_Pairib result = m_kObjectGroups.insert(
		ObjectGroups::value_type(strGroupName, pkObjectGroup));

	T_ASSERT2( result.second == true, "加入新的avatar绘制队列失败！" );
	if ( !result.second )
	{
		T_SAFE_DELETE(pkObjectGroup);
		pkObjectGroup = NULL;
	}

	return pkObjectGroup;
}

UI_AvatarRender::ObjectGroup* UI_AvatarRender::_retrieveObjectGroup(const StringType& strGroupName)
{
	ObjectGroup* pkObjectGroup = NULL;

	ObjectGroups::iterator groupIter = m_kObjectGroups.find(strGroupName);
	if ( groupIter != m_kObjectGroups.end() )
		pkObjectGroup = (groupIter->second);

	return pkObjectGroup;
}

void UI_AvatarRender::_destroyAllObjectGroup()
{
	_clearObjectGroupUpdateList();
	_clearObjectGroupExpiredList();

	mem::map<StringType, ObjectGroup*>::iterator iter = m_kObjectGroups.begin();
	mem::map<StringType, ObjectGroup*>::iterator iend = m_kObjectGroups.end();
	for ( ; iter != iend; ++iter )
		T_SAFE_DELETE(iter->second);

	m_kObjectGroups.clear();
}


void UI_AvatarRender::_destroyExpiredObjectGroup( ObjectGroup* pkObjectGroup )
{
	if ( !pkObjectGroup )
		return;

	onDestroyExpiredGroup(pkObjectGroup->GetMainQueue(), 
		pkObjectGroup->GetBackGroundQueue());

	ObjectGroups::iterator iter = m_kObjectGroups.find(pkObjectGroup->GetName());
	if ( iter != m_kObjectGroups.end() )
	{
		m_kObjectGroups.erase(iter);
		T_SAFE_DELETE(pkObjectGroup);
	}
}

void UI_AvatarRender::_addToObjectGroupUpdateList(ObjectGroup* pkObjectGroup)
{
	if ( pkObjectGroup &&
		!pkObjectGroup->IsInUpdateList() &&
		!pkObjectGroup->IsInExpiredList() )
	{
		pkObjectGroup->SetInUpdateList(true);
		m_kObjectGroupUpdateList.push_back(pkObjectGroup);
	}
}

void UI_AvatarRender::_removeFromObjectGroupUpdateList(ObjectGroup* pkObjectGroup)
{
	if ( pkObjectGroup &&
		pkObjectGroup->IsInUpdateList() )
	{
		ObjectGroupList::iterator iter = 
			m_kObjectGroupUpdateList.begin();
		ObjectGroupList::iterator iend = 
			m_kObjectGroupUpdateList.end();

		while ( iter != iend )
		{
			if ( (*iter) == pkObjectGroup )
			{
				pkObjectGroup->SetInUpdateList(false);
				m_kObjectGroupUpdateList.erase(iter);
				break;
			}
			++iter;
		}
	}
}

void UI_AvatarRender::_clearObjectGroupUpdateList()
{
	ObjectGroupList::iterator iter = m_kObjectGroupUpdateList.begin();
	ObjectGroupList::iterator iend = m_kObjectGroupUpdateList.end();
	while ( iter != iend )
		(*iter++)->SetInUpdateList(false);

	m_kObjectGroupUpdateList.clear();
}

void UI_AvatarRender::_addToObjectGroupExpiredList(ObjectGroup* pkObjectGroup)
{
	if ( pkObjectGroup &&
		!pkObjectGroup->IsActive() &&
		!pkObjectGroup->IsInUpdateList() &&
		!pkObjectGroup->IsInExpiredList() )
	{
		pkObjectGroup->SetInExpiredList(true);
		pkObjectGroup->ResetTimeForDiscard();
		m_kObjectGroupExpiredList.push_back(pkObjectGroup);
	}
}

void UI_AvatarRender::_removeFromObjectGroupExpiredList(ObjectGroup* pkObjectGroup)
{
	if ( pkObjectGroup && 
		pkObjectGroup->IsInExpiredList() )
	{
		ObjectGroupList::iterator iter = 
			m_kObjectGroupExpiredList.begin();
		ObjectGroupList::iterator iend = 
			m_kObjectGroupExpiredList.end();

		while ( iter != iend )
		{
			if ( (*iter) == pkObjectGroup )
			{
				pkObjectGroup->SetInExpiredList(false);
				pkObjectGroup->ResetTimeForDiscard();
				m_kObjectGroupExpiredList.erase(iter);
				break;
			}
			++iter;
		}
	}
}


void UI_AvatarRender::_clearObjectGroupExpiredList()
{
	ObjectGroupList::iterator iter = m_kObjectGroupExpiredList.begin();
	ObjectGroupList::iterator iend = m_kObjectGroupExpiredList.end();
	while ( iter != iend )
		(*iter++)->SetInExpiredList(false);

	m_kObjectGroupExpiredList.clear();
}

void UI_AvatarRender::_updateObjectGroupExpiredList(float fTimeDelta)
{
	ObjectGroup* pkObjectGroup = NULL;

	ObjectGroupList::iterator iter = 
		m_kObjectGroupExpiredList.begin();
	ObjectGroupList::iterator iend = 
		m_kObjectGroupExpiredList.end();

	while( iter != iend )
	{	
		pkObjectGroup = (*iter);
		pkObjectGroup->WaitingForDiscard(fTimeDelta);

		if ( pkObjectGroup->GetTimeForDiscard() > m_fObjectGroupExpiredRealLife )
		{
			iter = m_kObjectGroupExpiredList.erase(iter);
			_destroyExpiredObjectGroup(pkObjectGroup);
		}
		else
		{
			++iter;
		}
	}
}

NiCameraPtr UI_AvatarRender::_createCamera()
{
	NiCamera* pkCamera = NiNew NiCamera();
	return pkCamera;
}

void UI_AvatarRender::_destroyCamera(NiCameraPtr& spCamera)
{
	if ( spCamera )
	{
		spCamera = NULL;
	}
}

UI_AvatarRender::ObjectDepthSorter* UI_AvatarRender::_createDepthSorter()
{
	return T_NEW_D ObjectDepthSorter(m_pkManager);
}

void UI_AvatarRender::_destroyDepthSorter(UI_AvatarRender::ObjectDepthSorter*& pkSorter)
{
	T_SAFE_DELETE(pkSorter);
}

void UI_AvatarRender::CreateImage(const StringType& sTextureName, int iX, int iY, int iWidth, int iHeight, bool bSmall)
{
	if(bSmall)
		_createGUIImage(m_spGuiMaterialSmall, sTextureName, iX, iY, iWidth, iHeight);
	else
		_createGUIImage(m_spGuiMaterial, sTextureName, iX, iY, iWidth, iHeight);
}

void UI_AvatarRender::DestroyAllImages()
{
	_destroyAllGUIImage();
}

void UI_AvatarRender::DestroyMaterial()
{
	_destroyGUIMaterial(m_spGuiMaterial);
	_destroyGUIMaterial(m_spGuiMaterialSmall);
	_destroyGUIMaterial(m_spGuiMaterialDepthBuffer);
}

void UI_AvatarRender::_createGUIImage(GUI_MaterialPtr& spMaterial, const StringType& sTextureName, int iX, int iY, int iWidth, int iHeight)
{
	if ( !spMaterial )
		return;

	IG_Image* pkImage = IG_ImageManager::GetSingleton().CreateImage(
		spMaterial,sTextureName, GUIRect( GUIPoint2((float)iX, (float)iY), GUISize((float)iWidth, (float)iHeight) ));

	if (pkImage)
		m_kGuiImageSet.push_back(pkImage);
}

void UI_AvatarRender::_destroyAllGUIImage()
{
	IG_ImageManager* imageManager = IG_ImageManager::GetSingletonPtr();
	if ( !imageManager )
		return;

	size_t imgSize = m_kGuiImageSet.size();
	for ( size_t i = 0; i < imgSize; ++i )
		imageManager->DestroyImage(m_kGuiImageSet[i]->GetName());

	m_kGuiImageSet.clear();
}

GUI_MaterialPtr UI_AvatarRender::_createGUIMaterial(RG_RenderTexture& renderTexture, const StringType& materialName)
{
	GUI_MaterialPtr spMaterial = NULL;

	NiTexturePtr spTexture = NiSmartPointerCast(NiTexture, renderTexture.GetRenderedTexture());
	if( spTexture )
	{
		spMaterial = IG_MaterialManager::GetSingleton().CreateMaterial(materialName, spTexture);
	}

	T_ASSERT( spMaterial != NULL );
	if ( !spMaterial )
	{
		GfxWriteLog(LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL, 
			"Failed to create UI avatar material [%s]!", materialName.c_str());
	}

	return spMaterial;
}

void UI_AvatarRender::_destroyGUIMaterial(GUI_MaterialPtr& spIGMaterial)
{
	if ( spIGMaterial )
	{
		spIGMaterial = NULL;
	}
}

bool UI_AvatarRender::IsRenderTextureMsaaEnable()
{
	return m_kRenderTexture.IsMsaaEnable();
}

unsigned int UI_AvatarRender::GetRenderTargetWidth()
{
	return RENDER_TEXTURE_WIDTH;
}

unsigned int UI_AvatarRender::GetRenderTargetHeight()
{
	return RENDER_TEXTURE_HEIGHT;
}

unsigned int UI_AvatarRender::GetSmallRenderTargetWidth()
{
	return RENDER_SMALL_TEXTURE_WIDTH;
}

unsigned int UI_AvatarRender::GetSmallRenderTargetHeight()
{
	return RENDER_SMALL_TEXTURE_HEIGHT;
}

void UI_AvatarRender::ResetRenderTargetGroup(RG_AvatarRenderClick* renderClick, RENDER_CLICK_INDEX rdClickIdx)
{
		switch(rdClickIdx)
		{
		case RENDER_CLICK_ORIGIN:
			renderClick->SetRenderTargetGroup(m_spRenderTarget);
			renderClick->SetActive(m_spRenderTarget!=NULL);
			break;
		case RENDER_CLICK_SMALL:
			renderClick->SetRenderTargetGroup(m_spSmallRenderTarget);
			renderClick->SetActive(m_spSmallRenderTarget!=NULL);
			break;
		case RENDER_CLICK_NODEPTHBUFFER:
			renderClick->SetRenderTargetGroup(m_spNoDepthBufferRenderTarget);
			renderClick->SetActive(m_spNoDepthBufferRenderTarget!=NULL);
			break;
		default:
			T_ASSERT_CRITICAL_S(false);
		}
}


bool UI_AvatarRender::_createRenderTexture(RG_RenderTexture& renderTexture, const char * sTextureName, int iTextureWidth, int iTextureHeight, bool bDepthBuffer)
{
	NiTexture::FormatPrefs::PixelLayout eLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	bool bUseAlpha = true;

	if (!renderTexture.Create( sTextureName, 
		iTextureWidth, 
		iTextureHeight, 
		eLayout, bUseAlpha, false, bDepthBuffer, true ))
	{
		GfxWriteLog(LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL, 
			"Failed to create/recreate UI avatar MSAA render texture [%s]!", sTextureName);

		return false;
	}

	return true;
}

void UI_AvatarRender::_destroyRenderTexture(RG_RenderTexture& renderTexture)
{

}

NiRenderTargetGroupPtr UI_AvatarRender::_createRenderTarget(RG_RenderTexture& renderTexture)
{
	NiRenderTargetGroupPtr spRenderTarget;

	spRenderTarget = renderTexture.GetMsaaRenderTarget();
	T_ASSERT2(spRenderTarget != NULL, "在AvatarRender中创建renderTarget失败！")
	if (!spRenderTarget)
	{
		GfxWriteLog(LOG_TAG_CLIENT_CRITICAL, LOG_SWITCH_CLIENT_CRITICAL, 
			"Failed to get UI avatar MSAA render target!");
	}

	return spRenderTarget;
}

void UI_AvatarRender::_destroyRenderTarget(NiRenderTargetGroupPtr& spRenderTarget)
{
	if ( !spRenderTarget )
		return;

	// 释放1个引用计数
	RG_RenderManager::GetSingleton().ClearAvatarRenderTargetGroup();

	// 还有一个引用计数在RenderTexture中
	T_ASSERT2( spRenderTarget && spRenderTarget->GetRefCount() == 2, 
		"在AvatarRender中释放renderTarget时发现资源泄漏！" )

	spRenderTarget = NULL;
}

void UI_AvatarRender::onActiveObject(AvatarElementKeyId keyId)
{
	if ( m_pkManager )
		m_pkManager->renderCallBack_Active(keyId);
}

void UI_AvatarRender::onDeactiveObject(AvatarElementKeyId keyId)
{
	if ( m_pkManager )
		m_pkManager->renderCallBack_Deactive(keyId);
}

void UI_AvatarRender::onUpdateObject(AvatarElementKeyId keyId, float time, float timedelta)
{
	if ( m_pkManager )
		m_pkManager->renderCallBack_Update(keyId, time, timedelta);
}

void UI_AvatarRender::onConfigObjectShader( AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx, bool bPre)
{
	if ( m_pkManager )
		m_pkManager->renderCallBack_ConfigObjectShader(keyId, rdClickIdx, bPre);
}


void UI_AvatarRender::onClearGroupDirty(UI_AvatarRender::ObjectGroup::ObjectQueue &objectQueue)
{
	if ( m_pkObjectDepthSorter )
		m_pkObjectDepthSorter->Sort(objectQueue);
}

void UI_AvatarRender::onDestroyExpiredGroup( const UI_AvatarRender::ObjectGroup::ObjectQueue &objectQueue1, const UI_AvatarRender::ObjectGroup::ObjectQueue &objectQueue2 )
{
	if ( m_pkManager )
	{
		AvatarElementKeyIdList rkKeyIdList;
		rkKeyIdList.resize(objectQueue1.size() + objectQueue2.size());

		size_t iCount1 = objectQueue1.size();
		for ( size_t i = 0; i < iCount1; ++i )
			rkKeyIdList[i] = objectQueue1[i];
		size_t iCount2 = objectQueue2.size();
		for ( size_t i = 0; i < iCount2; ++i )
			rkKeyIdList[i+iCount1] = objectQueue2[i];

		m_pkManager->renderCallBack_RenderGroupExpired(rkKeyIdList);
	}
		
}

void UI_AvatarRender::onConfigCamera(AvatarElementKeyId keyId)
{
	if ( m_pkManager )
	{
		m_pkManager->renderCallBack_ConfigCamera(m_spCamera, keyId);
	}

	//由于外部直接修改了world坐标，所以这里可以不用update()
	//m_spCamera->Update(0.0f);
}


void UI_AvatarRender::onConfigShader(AvatarElementKeyId keyId)
{
	int iShaderParamIndex = 0;

	if ( m_pkManager )
		m_pkManager->renderCallBack_ConfigShader(iShaderParamIndex, keyId);

	m_kShaderParams.SetupLocalParams(iShaderParamIndex);
}


void UI_AvatarRender::onRenderSolidPass(AvatarElementKeyId keyId)
{
	if ( m_pkManager )
		m_pkManager->renderCallBack_RenderSolidPass(m_spCamera, m_kCuller, keyId);
}

void UI_AvatarRender::onRenderTransparentPass(AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx)
{
	if ( m_pkManager )
		m_pkManager->renderCallBack_RenderTransparentPass(m_spCamera, m_kCuller, keyId, rdClickIdx);
}

bool UI_AvatarRender::IsShouldRender(AvatarElementKeyId keyId, RENDER_CLICK_INDEX rdClickIdx)
{
	if ( m_pkManager )
		return m_pkManager->IsShouldRender(keyId, rdClickIdx);

	return false;
}

bool UI_AvatarRender::WorldPtToScreenPt(const NiPoint3& kPos, float& fX, float& fY)
{
	return m_spCamera->WorldPtToScreenPt(kPos, fX, fY);
}





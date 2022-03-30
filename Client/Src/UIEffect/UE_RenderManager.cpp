#include "ClientPCH.h"
#include "UE_RenderManager.h"
#include "CA_CameraManager.h"

#include "RS_ScreenElements.h"
#include "FT_FontManager.h"
#include "FT_Font.h"
#include "CR_HelpFunction.h"
#include "GD_ModelContent.h"
#include "GD_ModelContentManager.h"
#include "UE_UiEffectHead.h"
#include "UT_CutSceneManager.h"
#include "IG_Material.h"
#include "UE_PlayerName.h"
#include "LC_TitleManager.h"
#include "CL_RichTextParse.h"
#include "IG_ImageManager.h"
#include "YunYingPlatform.h"
#include "UE_UiEffectManager.h"

using namespace GUISys;
using namespace Data;
using namespace Utility;

const float UE_RenderManager::HPHEIGHT = 16.0f;
const float UE_RenderManager::HPWIDTH  = 128.0f;

#define INTERVALTIME 0.2f

UE_RenderManager::UE_RenderManager():m_fTransXFactor(1.0f),m_fTransYFactor(1.0f),m_iFont(1),m_fBloodHeight(0)
{
	m_pPlayerInfoBuffer = T_NEW_D GUISys::RS_GeometryBuffer();
	
	NiScreenElementsData* pkData = NiNew NiScreenElementsData(false, true, 
        1, MAX_BUFFER_QUADS / 4, 1, MAX_BUFFER_QUADS, 1, MAX_BUFFER_QUADS / 2);
	m_spElementInstancing = NiNew RS_ScreenElements(pkData);
	m_spElementInstancing->Init(MAX_BUFFER_QUADS / 4);

	pkData = NiNew NiScreenElementsData(false, true, 
		1, MAX_BUFFER_QUADS / 4, 1, MAX_BUFFER_QUADS, 1, MAX_BUFFER_QUADS / 2);
	m_spFontInstancing = NiNew RS_MemoryImageElement(pkData);
	m_spFontInstancing->Init(MAX_BUFFER_QUADS / 4);

	UpdateOffset();

	m_kPlayerNames.reserve( 250 );
	m_kPlayerAnnotations.reserve( 200 );
	m_kTitleFlashs.reserve(200);	
}

UE_RenderManager::~UE_RenderManager()
{
	_ClearGfxImageReferences();

	T_DELETE_D m_pPlayerInfoBuffer;
}

bool UE_RenderManager::AddPlayerBloodInfo(int iLevel, const NiPoint3 &kPos,const float HpDeepPercent, const float HpLightPercent, BloodType iType, int DamageElementType, bool isPlayer, bool isSkillActivateAll, int starlevel,const mem::vector<int> &runelevel,bool runeshow,const mem::vector<int> &equiplevel,bool equipshow,int equipRLevel)
{
	T_PROFILE("AddPlayerBloodInfo");

	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;
	m_pPlayerInfoBuffer->BeginRender(MODE_NONE);
	float fBloodWidth = 0;
	float fBloodHeight = 0;
	if(BloodType_Big_Begin < iType && iType < BloodType_Big_End)
	{
		fBloodWidth = m_fBloodWidth;
		fBloodHeight = m_fBloodHeight;
	}
	else if(BloodType_Small_Begin < iType && iType < BloodType_Small_End)
	{
		fBloodWidth = m_fBloodWidthSmall;
		fBloodHeight = m_fBloodHeightSmall;
	}
	else
	{
		T_ASSERT(false);
	}

	float HpDeepLen = HpDeepPercent * fBloodWidth;
	float HpLightLen = HpLightPercent * fBloodWidth;

	NiPoint3 kScreenPos = ConvertToScreenPos(kPos);
	kScreenPos.x = (int) (kScreenPos.x + 0.5f);
	kScreenPos.y = (int) (kScreenPos.y + 0.5f);
	GUIRect hprect, hprect2,hpbkimgrect, hpboundrect, hpbkrect, smallBoundRect, kNameArea,peaklevelrect,bossboundrect,bossbloodrect, bossNameArea, starrect, runerect, equiprect, equipRrect, equipRArea;
	NiPoint3 renderpos;
	renderpos.x = kScreenPos.x + m_kBloodOffset.x;
	renderpos.y = kScreenPos.y + m_kBloodOffset.y;

	float fHeight = (int)( fBloodHeight );

	hprect.SetPosition(renderpos.x,renderpos.y);
	hprect.SetWidth(HpDeepLen);
	hprect.SetHeight(fHeight);

	hprect2.SetPosition(renderpos.x,renderpos.y);
	hprect2.SetWidth(HpLightLen);
	hprect2.SetHeight(fHeight);

	bossbloodrect.SetPosition(kScreenPos.x + m_kBossBloodOffset.x, kScreenPos.y + m_kBossBloodOffset.y);

	bossbloodrect.SetWidth(HpDeepLen);
	bossbloodrect.SetHeight(fHeight);

	const Utility::UT_GlobalSettings & g_set = g_settings;

	hpbkimgrect.SetPosition(kScreenPos.x + m_kBloodbkOffset.x,  kScreenPos.y + m_kBloodbkOffset.y);
	hpbkimgrect.SetWidth(g_set.UE_PLAYER_BLOOD_BKIMAGE_WIDTH); 
	hpbkimgrect.SetHeight(g_set.UE_PLAYER_BLOOD_BKIMAGE_HEIGHT);

	hpboundrect.SetPosition(kScreenPos.x + m_kBoundOffset.x,  kScreenPos.y + m_kBoundOffset.y);
	hpboundrect.SetWidth(g_set.UE_PLAYER_BOUND_WIDTH); 
	hpboundrect.SetHeight(g_set.UE_PLAYER_BOUND_HEIGHT);
	
	bossboundrect.SetPosition(kScreenPos.x + m_kBossBoundOffset.x, kScreenPos.y + m_kBossBoundOffset.y);
	bossboundrect.SetWidth(g_set.UE_PLAYER_BOSS_BOUND_WIDTH);
	bossboundrect.SetHeight(g_set.UE_PLAYER_BOSS_BOUND_HEIGHT);
	
	hpbkrect.SetPosition(kScreenPos.x + m_kBloodBkOffset.x, kScreenPos.y + m_kBloodBkOffset.y);
	hpbkrect.SetWidth(g_set.UE_PLAYER_BLOOD_BK_WIDTH);
	hpbkrect.SetHeight(g_set.UE_PLAYER_BLOOD_BK_HEIGHT);

	smallBoundRect.SetPosition(kScreenPos.x + m_kSmallBoundOffset.x, kScreenPos.y + m_kSmallBoundOffset.y);
	smallBoundRect.SetWidth(g_set.UE_PLAYER_SMALL_BOUND_WIDTH);
	smallBoundRect.SetHeight(g_set.UE_PLAYER_SMALL_BOUND_HEIGHT);

	peaklevelrect.SetPosition(renderpos.x - 37, renderpos.y - 20);
	peaklevelrect.SetWidth(g_set.UE_PLAYER_BOUND_WIDTH); 
	peaklevelrect.SetHeight(g_set.UE_PLAYER_BOUND_HEIGHT);

	kNameArea.SetPosition(kScreenPos.x + m_kBoundOffset.x,  kScreenPos.y + m_kBoundOffset.y -1);
	kNameArea.SetWidth(m_fLevelWidth);
	kNameArea.SetHeight(g_set.UE_PLAYER_BOUND_HEIGHT);

	equipRArea.SetPosition(kScreenPos.x + m_kEquipReinforceOffset.x - 3, kScreenPos.y + m_kEquipReinforceOffset.y);
	equipRArea.SetWidth(g_set.UE_PLAYER_EQUIPBK_WIDTH);
	equipRArea.SetHeight(g_set.UE_PLAYER_EQUIPBK_HEIGHT);

	bossNameArea = bossboundrect;
	bossNameArea.SetWidth(m_fBossLevelWidth);
	
	const GUIColor & bkcolor = GUIColor::GWHITE;
	GUISys::IG_ImagePtr pkImageDeep = NULL;
	GUISys::IG_ImagePtr pkImageLight = NULL;
	switch (iType)
	{
	case BloodType_RED_Big_deep:
		{
			pkImageDeep = m_pPRedImgDeep;
			pkImageLight = m_pPRedImgLight;
		}
		break;
	case BloodType_GREEN_Big_deep:
		{
			pkImageDeep = m_pPGreenImgDeep;
			pkImageLight = m_pPGreenImgLight;
		}
		break;
	case BloodType_BLUE_Big_deep:
		{
			pkImageDeep = m_pPBlueImgDeep;
			pkImageLight = m_pPBlueImgLight;
		}
		break;
	default:
		break;
	}
	
	if(pkImageDeep && HpDeepPercent != -1)
	{
		GUISys::IG_ImagePtr	 bloodBKImage = m_pPBloodBKImg;
		_updateSourceTextureArea(bloodBKImage, bloodBKImage->GetOriginSourceTextureArea());
		bloodBKImage->Draw(m_pPlayerInfoBuffer, hpbkimgrect, 1, NULL, bkcolor);

		if(BloodType_Big_Begin < iType && iType < BloodType_Big_End)
		{
			GUISys::IG_ImagePtr	 bloodBackImage = isSkillActivateAll ? m_pHpBackAdvVec[DamageElementType-1] : m_pHpBackVec[DamageElementType-1];
			if (bloodBackImage != NULL)
			{
				_updateSourceTextureArea(bloodBackImage, bloodBackImage->GetOriginSourceTextureArea());
				bloodBackImage->Draw(m_pPlayerInfoBuffer, hpboundrect, 0, NULL, bkcolor);
			}

			int color = (starlevel-1) / 5 + 1;
			int num = starlevel % 5;
			if (num == 0 && starlevel != 0)
				num = 5;
			
			GUISys::IG_ImagePtr	 bloodstarImage = m_pBuddyStarIconVec[color - 1];
			if (bloodstarImage != NULL)
			{
				for (int idx = 0;idx<=num -1;idx++)
				{
					float star_x = kScreenPos.x + m_kStarOffset.x + ((g_set.UE_PLAYER_STARBOUND_WIDTH - (-7*(num-1)+g_set.UE_PLAYER_STAR_WIDTH * num))/2) + g_set.UE_PLAYER_STAR_WIDTH*idx - 7*idx;
					starrect.SetPosition(star_x,  kScreenPos.y + m_kStarOffset.y);
					starrect.SetWidth(g_set.UE_PLAYER_STAR_WIDTH); 
					starrect.SetHeight(g_set.UE_PLAYER_STAR_HEIGHT);
					_updateSourceTextureArea(bloodstarImage, bloodstarImage->GetOriginSourceTextureArea());
					bloodstarImage->Draw(m_pPlayerInfoBuffer, starrect, 0, NULL, bkcolor);
				}
			}
			
			if (runeshow)
			{
				for(int idx = 0;idx<4;idx++)
				{
					GUISys::IG_ImagePtr	 bloodruneImage = m_pRuneStarIconVec[runelevel[idx]];
					if (bloodruneImage != NULL)
					{
						float rune_x = kScreenPos.x + m_kRuneOffset.x + g_set.UE_PLAYER_EQUIP_WIDTH*(idx-1);
						runerect.SetPosition(rune_x, kScreenPos.y + m_kRuneOffset.y);
						runerect.SetWidth(g_set.UE_PLAYER_RUNE_WIDTH);
						runerect.SetHeight(g_set.UE_PLAYER_RUNE_HEIGHT);
						_updateSourceTextureArea(bloodruneImage, bloodruneImage->GetOriginSourceTextureArea());
						bloodruneImage->Draw(m_pPlayerInfoBuffer, runerect, 0, NULL, bkcolor);
					}
				}
			}

			if (equipshow)
			{
				for(int idx = 0;idx<4;idx++)
				{
					GUISys::IG_ImagePtr	 bloodequipImage = m_pEquipStarIconVec[equiplevel[idx]];
					if (bloodequipImage != NULL)
					{
						float equip_x = kScreenPos.x + m_kEquipOffset.x + g_set.UE_PLAYER_EQUIP_WIDTH*(idx-1);
						equiprect.SetPosition(equip_x, kScreenPos.y + m_kEquipOffset.y);
						equiprect.SetWidth(g_set.UE_PLAYER_EQUIP_WIDTH);
						equiprect.SetHeight(g_set.UE_PLAYER_EQUIP_HEIGHT);
						_updateSourceTextureArea(bloodequipImage, bloodequipImage->GetOriginSourceTextureArea());
						bloodequipImage->Draw(m_pPlayerInfoBuffer, equiprect, 0, NULL, bkcolor);
					}
				}
			}

			FT_Font* pFont = FT_FontManager::GetSingleton().GetFont(m_iNumberFont,FONT_MODE_NONE);
			if ( NULL != pFont )
			{
				pFont->DrawText(m_pPlayerInfoBuffer, Utility::Int2String(iLevel), kNameArea, 0, NULL, dtCenter + dtVCenter, GUIColor::GWHITE, FONT_MODE_NONE);
			}

			if(equipRLevel > 0)
			{
				GUISys::IG_ImagePtr	 bloodEquipBkImage = m_pEquipBkImage;
				if (bloodEquipBkImage != NULL)
				{
					equipRrect.SetPosition(kScreenPos.x + m_kEquipReinforceOffset.x, kScreenPos.y + m_kEquipReinforceOffset.y);
					equipRrect.SetWidth(g_set.UE_PLAYER_EQUIPBK_WIDTH);
					equipRrect.SetHeight(g_set.UE_PLAYER_EQUIPBK_HEIGHT);
					_updateSourceTextureArea(bloodEquipBkImage, bloodEquipBkImage->GetOriginSourceTextureArea());
					bloodEquipBkImage->Draw(m_pPlayerInfoBuffer, equipRrect, 0, NULL, bkcolor);
				}

				FT_Font* eFont = FT_FontManager::GetSingleton().GetFont(m_iEquipReinforceFont,FONT_MODE_NONE);
				eFont->DrawText(m_pPlayerInfoBuffer, StringType('+' + Int2String(equipRLevel)), equipRArea, 0, NULL, dtRight + dtVCenter, GUIColor::GGREEN, FONT_MODE_NONE);
			}
		}

		GUIRect rect = pkImageDeep->GetOriginSourceTextureArea();
		rect.SetWidth(HpDeepLen);
		_updateSourceTextureArea(pkImageDeep, rect);
		pkImageDeep->Draw(m_pPlayerInfoBuffer,hprect,-25.0f,NULL, bkcolor);
	}

	m_pPlayerInfoBuffer->EndRender();
	return true;
}

bool UE_RenderManager::AddMonsterBloodInfo(int iLevel, const NiPoint3 &kPos,const float HpDeepPercent, const float HpLightPercent, BloodType iType, bool isRaidBoss, int iDropTimes)
{
	T_PROFILE("AddMonsterBloodInfo");

	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;
	m_pPlayerInfoBuffer->BeginRender(MODE_NONE);
	float fBloodWidth = 0;
	float fBloodHeight = 0;
	if(BloodType_Big_Begin < iType && iType < BloodType_Big_End)
	{
		fBloodWidth = m_fBloodWidth;
		fBloodHeight = m_fBloodHeight;
		if (iType == BloodType_RED_Big_deep) // BOSS
		{
			fBloodWidth = m_fBossBloodWidth;
			fBloodHeight = m_fBossBloodHeight;
		}
	}
	else if(BloodType_Small_Begin < iType && iType < BloodType_Small_End)
	{
		fBloodWidth = m_fBloodWidthSmall;
		fBloodHeight = m_fBloodHeightSmall;
	}
	else
	{
		T_ASSERT(false);
	}

	float HpDeepLen = HpDeepPercent * fBloodWidth;
	float HpLightLen = HpLightPercent * fBloodWidth;

	NiPoint3 kScreenPos = ConvertToScreenPos(kPos);
	kScreenPos.x = (int) (kScreenPos.x + 0.5f);
	kScreenPos.y = (int) (kScreenPos.y + 0.5f);
	GUIRect hprect, smallBoundRect, kNameArea,bossboundrect,bossbloodrect, bossNameArea, dropArea, dropArea2, dropIconRect;
	NiPoint3 renderpos;
	renderpos.x = kScreenPos.x + m_kNpcBloodOffset.x;
	renderpos.y = kScreenPos.y + m_kNpcBloodOffset.y;
	float fHeight = (int)( fBloodHeight );

	hprect.SetPosition(renderpos.x,renderpos.y);
	hprect.SetWidth(HpDeepLen);
	hprect.SetHeight(fHeight);

	bossbloodrect.SetPosition(kScreenPos.x + m_kBossBloodOffset.x, kScreenPos.y + m_kBossBloodOffset.y);
	bossbloodrect.SetWidth(HpDeepLen);
	bossbloodrect.SetHeight(fHeight);
	
	bossboundrect.SetPosition(kScreenPos.x + m_kBossBoundOffset.x, kScreenPos.y + m_kBossBoundOffset.y);
	bossboundrect.SetWidth(g_settings.UE_PLAYER_BOSS_BOUND_WIDTH);
	bossboundrect.SetHeight(g_settings.UE_PLAYER_BOSS_BOUND_HEIGHT);

	smallBoundRect.SetPosition(kScreenPos.x + m_kSmallBoundOffset.x, kScreenPos.y + m_kSmallBoundOffset.y);
	smallBoundRect.SetWidth(g_settings.UE_PLAYER_SMALL_BOUND_WIDTH);
	smallBoundRect.SetHeight(g_settings.UE_PLAYER_SMALL_BOUND_HEIGHT);

	kNameArea.SetPosition(kScreenPos.x + m_kBoundOffset.x,  kScreenPos.y + m_kBoundOffset.y -1);
	kNameArea.SetWidth(m_fLevelWidth);
	kNameArea.SetHeight(g_settings.UE_PLAYER_BOUND_HEIGHT);

	bossNameArea = bossboundrect;
	bossNameArea.SetWidth(m_fBossLevelWidth);
	
	const GUIColor & bkcolor = GUIColor::GWHITE;
	GUISys::IG_ImagePtr pkImageDeep = NULL;
	GUISys::IG_ImagePtr pkImageLight = NULL;
	switch (iType)
	{
	case BloodType_RED_Big_deep:
		{
			pkImageDeep = m_pMRedImgBigDeep;
			pkImageLight = m_pMRedImgBigLight;
		}
		break;
	case BloodType_RED_Small_deep:
		{
			pkImageDeep = m_pMRedImgSmallDeep;
			pkImageLight = m_pMRedImgSmallLight;
		}
		break;
	default:
		break;
	}
	
	if(pkImageDeep && HpDeepPercent != -1)
	{
		switch (iType)
		{
		case BloodType_RED_Big_deep:
			{
				_updateSourceTextureArea(m_pBoundBossImg, m_pBoundBossImg->GetOriginSourceTextureArea());
				m_pBoundBossImg->Draw(m_pPlayerInfoBuffer, bossboundrect, 0, NULL, bkcolor);

				FT_Font* pFont = FT_FontManager::GetSingleton().GetFont(m_iNumberFont,FONT_MODE_STROKER);
				if ( NULL != pFont )
					pFont->DrawText(m_pPlayerInfoBuffer, Utility::Int2String(iLevel), bossNameArea, 0, NULL, dtCenter + dtVCenter, GUIColor::GWHITE, FONT_MODE_STROKER);

				GUIRect rect = pkImageDeep->GetOriginSourceTextureArea();
				rect.SetWidth(HpDeepLen);
				_updateSourceTextureArea(pkImageDeep, rect);
				pkImageDeep->Draw(m_pPlayerInfoBuffer,bossbloodrect,-25.0f,NULL, bkcolor);

				if (isRaidBoss)
				{
					GUISys::IG_ImagePtr	 dropIcon = m_pWildBossDropImage;
					if (dropIcon != NULL)
					{
						dropIconRect.SetPosition(kScreenPos.x + m_kwildBossDropoffset.x, kScreenPos.y + m_kwildBossDropoffset.y);
						dropIconRect.SetWidth(g_settings.UE_WILDBOSS_DROPICON_WIDTH);
						dropIconRect.SetHeight(g_settings.UE_WILDBOSS_DROPICON_HEIGHT);
						_updateSourceTextureArea(dropIcon, dropIcon->GetOriginSourceTextureArea());
						dropIcon->Draw(m_pPlayerInfoBuffer, dropIconRect, 0, NULL, bkcolor);
					}

					dropArea.SetPosition(kScreenPos.x + m_kwildBossDropoffset.x + g_settings.UE_WILDBOSS_DROPICON_WIDTH, kScreenPos.y + m_kwildBossDropoffset.y + 5.0f);
					dropArea.SetWidth(14.0f);
					dropArea.SetHeight(14.0f);
					FT_Font* lFont = FT_FontManager::GetSingleton().GetFont(m_iFont,FONT_MODE_NONE);
					if (iDropTimes > 0)
						lFont->DrawText(m_pPlayerInfoBuffer, Utility::Int2String(iDropTimes), dropArea, 0, NULL, dtRight + dtVCenter, GUIColor::GWHITE, FONT_MODE_SHADOW);
					else
						lFont->DrawText(m_pPlayerInfoBuffer, Utility::Int2String(iDropTimes), dropArea, 0, NULL, dtRight + dtVCenter, GUIColor::GRED, FONT_MODE_SHADOW);

					dropArea2.SetPosition(kScreenPos.x + m_kwildBossDropoffset.x + g_settings.UE_WILDBOSS_DROPICON_WIDTH + 15.0f, kScreenPos.y + m_kwildBossDropoffset.y + 5.0f);
					dropArea2.SetWidth(30.0f);
					dropArea2.SetHeight(14.0f);
					FT_Font* rFont = FT_FontManager::GetSingleton().GetFont(m_iFont,FONT_MODE_NONE);
					rFont->DrawText(m_pPlayerInfoBuffer, StringType('/' + Int2String(GetGlobalSetting.nKillNpcDropMaxTimes)), dropArea2, 0, NULL, dtLeft + dtVCenter, GUIColor::GWHITE, FONT_MODE_SHADOW);
				}
			}
			break;
		case BloodType_RED_Small_deep:
			{
				m_pBoundMonsterImg->Draw(m_pPlayerInfoBuffer, smallBoundRect, 0, NULL, bkcolor);

				GUIRect rect = pkImageDeep->GetOriginSourceTextureArea();
				rect.SetWidth(HpDeepLen);
				_updateSourceTextureArea(pkImageDeep, rect);
				pkImageDeep->Draw(m_pPlayerInfoBuffer,hprect,-25.0f,NULL, bkcolor);
			}
			break;
		default:
			break;
		}
	}

	m_pPlayerInfoBuffer->EndRender();
	return true;
}


bool UE_RenderManager::AddMonsterSkillCastInfo(float Percent,const StringType& SkillName,const NiPoint3 &kPos)
{
	if (Percent<0)
	{
		return false;
	}
	T_PROFILE("AddMonsterSkillCastInfo");
	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;

	m_pPlayerInfoBuffer->BeginRender(MODE_NONE);
	NiPoint3 kScreenPos = ConvertToScreenPos(kPos);
	kScreenPos.x = (int) (kScreenPos.x + 0.5f);
	kScreenPos.y = (int) (kScreenPos.y + 0.5f);
	GUISys::IG_ImagePtr pkImageDeep = m_pNPCSkillPercentImg;
	GUIRect rect = pkImageDeep->GetOriginSourceTextureArea();
	rect.SetPosition(kScreenPos.x-56,kScreenPos.y-10);
	rect.SetWidth(m_fBossBloodWidth*1.1);
	rect.SetHeight(m_fBossBloodHeight);
	//_updateSourceTextureArea(pkImageDeep, rect);
	if (pkImageDeep)
	{
		const GUIColor & bkcolor = GUIColor::GWHITE;
		pkImageDeep->Draw(m_pPlayerInfoBuffer,rect,-24.0f,NULL, bkcolor);
	}

	FT_Font* pFont = FT_FontManager::GetSingleton().GetFont(m_iFont,FONT_MODE_STROKER);
	if (pFont)
	{
		pFont->DrawText(m_pPlayerInfoBuffer, SkillName, rect, 0, NULL, dtCenter + dtVCenter, GUIColor::GWHITE, FONT_MODE_STROKER);
	}
	GUISys::IG_ImagePtr pkImageLight = m_pNPCSkillPercentBKImg;
	rect.SetPosition(kScreenPos.x-56,kScreenPos.y-10);
	rect.SetWidth(m_fBossBloodWidth*1.1*(1-Percent));
	rect.SetHeight(m_fBossBloodHeight);
	//_updateSourceTextureArea(pkImageLight, rect);
	if (pkImageLight)
	{
		const GUIColor & bkcolor = GUIColor::GWHITE;
		pkImageLight->Draw(m_pPlayerInfoBuffer,rect,-25.0f,NULL, bkcolor);
	}


	m_pPlayerInfoBuffer->EndRender();
	return true;
}

bool UE_RenderManager::AddNPCTaskInfo(const NiPoint3 &kPos, long lTaskState)
{
	T_PROFILE("AddNPCTaskInfo");

	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;
	m_pPlayerInfoBuffer->BeginRender(MODE_NONE);

	IG_ImagePtr spIconBG = NULL;
	switch (lTaskState)
	{
		case NFST_TASK_CANFINISH:
			spIconBG = m_spTaskFinishIcon;
			break;
		case NFST_TASK_CANACCEPT:
			spIconBG = m_spTaskAcceptIcon2;
			break;
		case NFST_TASK_PROCESSING:
			spIconBG = m_spTaskUnFinishIcon;
			break;
		case NFST_TASK_CANT_ACCEPT:
			spIconBG = m_spTaskNoneIcon;
			break;
	}

	NiPoint3 kScreenPos = ConvertToScreenPos(kPos);
	GUIRect taskrect;
	
	taskrect.SetPosition(kScreenPos.x + m_kNpcTaskOffset.x ,kScreenPos.y + m_kNpcTaskOffset.y);
	taskrect.SetWidth(m_fNPCTaskWidth);
	taskrect.SetHeight(m_fNPCTaskHeight);

	if(spIconBG)
		spIconBG->Draw(m_pPlayerInfoBuffer, taskrect, 1, NULL, GUIColor(1,1,1,1));

	return true;
}


NiPoint3 UE_RenderManager::ConvertToScreenPos( const NiPoint3 &kPos )
{
	NiPoint3 ScreenPos = NiPoint3::ZERO;
	{
		NiCameraPtr spCamera = CA_CameraManager::GetSingletonPtr()->GetNiCamera();

		if (spCamera->WorldPtToScreenPt3(kPos,ScreenPos.x,ScreenPos.y,ScreenPos.z))
		{
			const NiRenderTargetGroup* pkRTGroup = NiRenderer::GetRenderer()->GetDefaultRenderTargetGroup();

			ScreenPos.x = ScreenPos.x * (float)pkRTGroup->GetWidth(0);
			ScreenPos.y = (1 - ScreenPos.y) * (float)pkRTGroup->GetHeight(0);
		}
	}
	return ScreenPos;
}

unsigned int UE_RenderManager::RenderUEScreen() 
{
	unsigned int nDrawObjects = 0;
	if(!UT_CutSceneManager::GetSingleton().ShowUE())
	{
		return nDrawObjects;
	}

	nDrawObjects += _renderGeometry(m_pPlayerInfoBuffer);

	if(!UT_CutSceneManager::GetSingleton().ShowUECost())
		return nDrawObjects;

	//m_pPlayerInfoBuffer->UpdateHandleList();

	//_renderUEFX();

	return nDrawObjects;
}

void UE_RenderManager::ClearAllBuffer()
{
	m_pPlayerInfoBuffer->Reset();
	m_kPlayerNames.clear();
	m_kPlayerAnnotations.clear();
	m_kTitleFlashs.clear();
}

void UE_RenderManager::UpdateOffset()
{
	//g_settings.Load(true);//重新载入config
	m_kBloodbkOffset = g_settings.UE_PLAYER_BLOODBK_OFFSET;
	m_kBloodOffset = g_settings.UE_PLAYER_BLOOD_OFFSET;
	m_kNpcBloodOffset = g_settings.UE_NPC_BLOOD_OFFSET;
	m_kBoundOffset = g_settings.UE_PLAYER_BOUND_OFFSET;
	m_kStarOffset = g_settings.UE_PLAYER_STAR_OFFSET;
	m_kRuneOffset = g_settings.UE_PLAYER_RUNE_OFFSET;
	m_kEquipOffset = g_settings.UE_PLAYER_EQUIP_OFFSET;
	m_kEquipReinforceOffset = g_settings.UE_PLAYER_EQUIPREINFORCE_OFFSET;
	m_kBloodBkOffset = g_settings.UE_PLAYER_BLOOD_BK_OFFSET;
	m_fBossLevelWidth = g_settings.UE_PLAYER_BOSS_LEVEL_WIDTH;
	m_kNameOffset  = g_settings.UE_PLAYER_NAME_OFFSET;
	m_kMonsterNameOffset  = g_settings.UE_MONSTER_NAME_OFFSET;
	m_kSmallBoundOffset = g_settings.UE_PLAYER_SMALL_BOUND_OFFSET;
	m_kBossBoundOffset = g_settings.UE_PLAYER_BOSS_BOUND_OFFSET;
	m_kBossBloodOffset = g_settings.UE_PLAYER_BOSS_BLOOD_OFFSET;
	m_kwildBossDropoffset = g_settings.UE_WILDBOSS_DROP_OFFSET;
	m_kHolyFlamOffset = g_settings.UE_PLAYER_HOLYFLAM_OFFSET;
	m_kNpcTaskOffset = g_settings.UE_NPC_TASK_OFFSET;
	m_kStarsOffset = g_settings.UE_STARS_OFFSET;
	m_fLevelWidth = g_settings.UE_PLAYER_LEVEL_WIDTH;
	m_fBloodWidth  = g_settings.UE_PLAYER_BLOOD_WIDTH;
	m_fBloodHeight = g_settings.UE_PLAYER_BLOOD_HEIGHT;
	m_fBossBloodWidth = g_settings.UE_PLAYER_BOSS_BLOOD_WIDTH;
	m_fBossBloodHeight = g_settings.UE_PLAYER_BOSS_BLOOD_HEIGHT;
	m_fBloodWidthSmall  = g_settings.UE_PLAYER_BLOOD_WIDTH_SMALL;
	m_fBloodHeightSmall = g_settings.UE_PLAYER_BLOOD_HEIGHT_SMALL;
	m_fNameIconOffset = g_settings.UE_PLAYER_NAME_ICON_OFFSET;
	m_fTransXFactor = g_settings.UE_TRANS_X_FACTOR;
	m_fTransYFactor =  g_settings.UE_TRANS_Y_FACTOR;
	m_fTransXOffset =  g_settings.UE_TRANS_X_OFFSET;
	m_iFont			=  g_settings.UE_FONT;
	m_iNumberFont			=  g_settings.UE_NUMBER_FONT;
	m_iEquipReinforceFont   =  g_settings.UE_EQUIPR_FONT;
	m_iguildFont	=  g_settings.UE_GUILDFONT;
	m_fNPCTaskWidth		=  g_settings.UE_NPC_TASK_WIDTH;
	m_fNPCTaskHeight	=  g_settings.UE_NPC_TASK_HEIGHT;
}

bool UE_RenderManager::AddNameInfo(RS_GeometryBuffer* pkBuffer ,
								   const NiPoint3 &renderpos, 
								   const StringType &sName, 
								   const StringType &sTitle, 
								   const StringType &sDebug, 
								   const NiColor &kNameColor, 
								   const NiColor &kTitleColor, 
								   const NiColor &kDebugColor, 
								   float fNameAlpha,
								   bool bIsPlayer,
								   int iStar,
								   int iTitleLevel)
{
	T_ASSERT(pkBuffer);
	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;

	FT_Font* pFont = FT_FontManager::GetSingleton().GetFont(m_iFont,FONT_MODE_SHADOW);//UE 人名不要描边
	if ( pFont == NULL )
	{
		return false;
	}
	//pkBuffer->BeginRender(MODE_NOALIGNTEXT);
	T_PROFILE("UE_RenderManager::AddNameInfo");

	GUIRect kNameArea;
	GUIRect kRealNameArea;
	kNameArea.SetPosition(renderpos.x,renderpos.y);
	if(bIsPlayer)
		kNameArea.SetWidth(m_fBloodWidth+2.0f);
	else
		kNameArea.SetWidth(m_fBloodWidthSmall+2.0f);

	kNameArea.SetHeight(m_kBloodOffset.z+22.0f);
	kRealNameArea = kNameArea;
	_RenderTextName(pkBuffer, sName, GUIColor(kNameColor.r,kNameColor.g,kNameColor.b,fNameAlpha), kNameArea, pFont);

	//头衔Icon 紧挨着名字
	if(iTitleLevel > 0)
	{
		m_pDailyActivityIcon = m_pCommanderTitleLevelIcon[iTitleLevel - 1];
		if(m_pDailyActivityIcon)
		{
			GUIPoint2 pos;
			pos.x = kRealNameArea.m_left + kRealNameArea.GetWidth() * 0.5f + pFont->GetTextExtent(sName) * 0.5f;
			pos.y = kRealNameArea.m_top - m_pDailyActivityIcon->GetHeight() * 0.5f + pFont->GetFontHeight() * 0.5f;

			m_pDailyActivityIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
		}
	}

	pkBuffer->UpdateTitleFontIndex();

	if ( iStar > 0 && iStar <= 4)
	{
		IG_ImagePtr pNpcStarIcon = m_pNPCStarIcon[iStar - 1];
		if(pNpcStarIcon != NULL)
		{
			float m_fRichTextOffset = (- pFont->GetTextExtent(sName, 1.0f))/ 2.0f;
			int iWidth = kRealNameArea.GetWidth();
			int iBeginPosX = renderpos.x +  m_fRichTextOffset + (- pNpcStarIcon->GetWidth() + 35.0f ) ;
			int iBeginPosY = kRealNameArea.GetPosition().y - (pNpcStarIcon->GetHeight())/2  + 10.0f;
			GUIPoint2 pos(iBeginPosX,iBeginPosY);

			pNpcStarIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
		}
	}

	kNameArea.Offset(0,m_kNameOffset.z);
	if (!sName.empty())
	{
		kRealNameArea.Offset(0,m_kNameOffset.z);
	}

	_RenderTextName(pkBuffer, sTitle, GUIColor(kTitleColor.r,kTitleColor.g,kTitleColor.b,1), kNameArea, pFont);
	kNameArea.Offset(0,m_kNameOffset.z);
	if (!sTitle.empty())
	{
		kRealNameArea.Offset(0,m_kNameOffset.z);
	}

	_RenderTextName(pkBuffer, sDebug, GUIColor(kDebugColor.r,kDebugColor.g,kDebugColor.b,1), kNameArea, pFont);
	kNameArea.Offset(0,m_kNameOffset.z);
	if (!sDebug.empty())
	{
		kRealNameArea.Offset(0,m_kNameOffset.z);
	}


	pkBuffer->EndRender();
	return true;
}

bool UE_RenderManager::AddGuildInfo(RS_GeometryBuffer* pkBuffer ,
								   const NiPoint3 &renderpos, 
								   const StringType &sName, 
								   const StringType &sTitle, 
								   const StringType &sDebug, 
								   const NiColor &kNameColor, 
								   const NiColor &kTitleColor, 
								   const NiColor &kDebugColor, 
								   float fNameAlpha,
								   bool bIsPlayer,
								   int iStar)
{
	T_ASSERT(pkBuffer);
	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;

	FT_Font* pFont = FT_FontManager::GetSingleton().GetFont(m_iguildFont,FONT_MODE_SHADOW);//UE 人名不要描边
	if ( pFont == NULL )
	{
		return false;
	}
	//pkBuffer->BeginRender(MODE_NOALIGNTEXT);
	T_PROFILE("UE_RenderManager::AddNameInfo");

	GUIRect kNameArea;
	GUIRect kRealNameArea;
	kNameArea.SetPosition(renderpos.x,renderpos.y);
	if(bIsPlayer)
		kNameArea.SetWidth(m_fBloodWidth+2.0f);
	else
		kNameArea.SetWidth(m_fBloodWidthSmall+2.0f);

	kNameArea.SetHeight(m_kBloodOffset.z+22.0f);
	kRealNameArea = kNameArea;
	_RenderTextName(pkBuffer, sName, GUIColor(kNameColor.r,kNameColor.g,kNameColor.b,fNameAlpha), kNameArea, pFont);

	pkBuffer->UpdateTitleFontIndex();

	kNameArea.Offset(0,m_kNameOffset.z);
	if (!sName.empty())
	{
		kRealNameArea.Offset(0,m_kNameOffset.z);
	}

	_RenderTextName(pkBuffer, sTitle, GUIColor(kTitleColor.r,kTitleColor.g,kTitleColor.b,1), kNameArea, pFont);
	kNameArea.Offset(0,m_kNameOffset.z);
	if (!sTitle.empty())
	{
		kRealNameArea.Offset(0,m_kNameOffset.z);
	}

	_RenderTextName(pkBuffer, sDebug, GUIColor(kDebugColor.r,kDebugColor.g,kDebugColor.b,1), kNameArea, pFont);
	kNameArea.Offset(0,m_kNameOffset.z);
	if (!sDebug.empty())
	{
		kRealNameArea.Offset(0,m_kNameOffset.z);
	}

	/*if (m_pNPCStarIcon && iStar > 0)
	{
		int iWidth = kRealNameArea.GetWidth();
		int iBeginPosX = kRealNameArea.GetPosition().x + (iWidth - m_pNPCStarIcon->GetWidth() * iStar) / 2.0f;
		int iBeginPosY = kRealNameArea.GetPosition().y - 2.0f;
		for (int i = 1; i <= iStar; ++i)
		{
			GUIPoint2 pos(iBeginPosX,iBeginPosY);
			m_pNPCStarIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
			iBeginPosX += m_pNPCStarIcon->GetWidth();
		}
	}*/
	pkBuffer->EndRender();
	return true;
}

bool UE_RenderManager::AddAnnotate( GUISys::RS_GeometryBuffer* pkBuffer, const NiPoint3 &renderpos, int iAnnotateMask, const NiPoint3& kScreenPos, float deltaHeight, int iTitleID,bool bShowTitle, const std::vector<int>& iParamVec, int& tttop,GUIRect	&mTitleFlashRect,GUIRect &mGuildTitleFlashRect)
{
	T_ASSERT(pkBuffer);
	T_ASSERT(iParamVec.size() >= UE_PlayerName::ATBIT_MAX);
	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;

	pkBuffer->BeginRender(MODE_NONE);

	float fWidth = ( m_fBloodWidth+2.0f ) / 2.0f ;
	float fIconOffset = 0.0f;
	float fTop = renderpos.y;
	
	if ((iAnnotateMask & UE_PlayerName::AT_VIP) && (iParamVec[UE_PlayerName::ATBIT_VIP] > 0))
	{
		m_pVipIcon = m_pVipLevelIcon[iParamVec[UE_PlayerName::ATBIT_VIP]];
		if(m_pVipIcon)
		{
			GUIPoint2 pos(renderpos.x + fWidth, renderpos.y );
			fIconOffset -= m_pVipIcon->GetWidth();
			pos.x += fIconOffset;
			pos.y -= m_pVipIcon->GetHeight() * 0.5f - (m_kBloodOffset.z + 2.0f) * 0.5f;
			
			m_pVipIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
		}
	}

	IG_ImagePtr spGroupIcon = NULL;
	if(iAnnotateMask & UE_PlayerName::AT_OTHER_GROUP)
	{
		spGroupIcon = m_pOtherGroupMember;
	}
	else if(iAnnotateMask & UE_PlayerName::AT_GROUP)
	{
		if(iAnnotateMask & UE_PlayerName::AT_TEAM_LEAD)
			spGroupIcon = m_pGroupLeader;
		else
			spGroupIcon = m_pGroupMember;
	}

	if(spGroupIcon)
	{
		GUIPoint2 pos(renderpos.x + fWidth, renderpos.y );//为了配合字体绘制，居中后偏移，HARK代码
		fIconOffset -= spGroupIcon->GetWidth();
		pos.x += fIconOffset;
		pos.y -= spGroupIcon->GetHeight() * 0.5f - (m_kBloodOffset.z + 2) * 0.5f;
		
		spGroupIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
	}

	if ((iAnnotateMask & UE_PlayerName::AT_VILLAIN) && m_pVillainIcon)
	{
		GUIPoint2 pos(renderpos.x + fWidth, renderpos.y );
		fIconOffset -= m_pVillainIcon->GetWidth();
		pos.x += fIconOffset;
		pos.y -= m_pVillainIcon->GetHeight() * 0.5f - (m_kBloodOffset.z + 2.0f) * 0.5f;

		m_pVillainIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
	}

	if(iAnnotateMask & UE_PlayerName::AT_PLATFORM_VIP_YEAR)
	{
		if(!m_pPlatformVipYearIcon)
			m_pPlatformVipYearIcon = _SafeGfxGetImageFromAtlas(getPlatformVipTypeYearImage().c_str());

		if(m_pPlatformVipYearIcon)
		{
			GUIPoint2 pos(renderpos.x + fWidth, renderpos.y );
			fIconOffset -= m_pPlatformVipYearIcon->GetWidth();
			pos.x += fIconOffset;
			pos.y -= m_pPlatformVipYearIcon->GetHeight() * 0.5f - (m_kBloodOffset.z + 4.0f) * 0.5f;

			m_pPlatformVipYearIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
		}
	}	

	if((iAnnotateMask & UE_PlayerName::AT_PLATFORM_VIP) && (iParamVec[UE_PlayerName::ATBIT_PLATFORM_VIP] > 0))
	{
		m_pPlatformVipIcon = _SafeGfxGetImageFromAtlas(getPlatformVipTypeImage(iParamVec[UE_PlayerName::ATBIT_PLATFORM_VIP], iParamVec[UE_PlayerName::ATBIT_PLATFORM_VIP_HIGH]>0).c_str());
		if(m_pPlatformVipIcon)
		{
			GUIPoint2 pos(renderpos.x + fWidth, renderpos.y );
			fIconOffset -= m_pPlatformVipIcon->GetWidth();
			pos.x += fIconOffset;
			pos.y -= m_pPlatformVipIcon->GetHeight() * 0.5f - (m_kBloodOffset.z + 4.0f) * 0.5f;

			m_pPlatformVipIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
		}
	}

	if((iAnnotateMask & UE_PlayerName::AT_GM_TITLE_INFO) && (iParamVec[UE_PlayerName::ATBIT_GM_TITLE_INFO] < MAX_GM_RPIORITY) && (iParamVec[UE_PlayerName::ATBIT_GM_TITLE_INFO] > 0))
	{
		if(m_pGmTitleIcon)
		{
			GUIPoint2 pos(renderpos.x + fWidth, renderpos.y + deltaHeight);
			fIconOffset -= m_pGmTitleIcon->GetWidth();
			pos.x += fIconOffset;
			pos.y -= m_pGmTitleIcon->GetHeight() * 0.5f - (m_kBloodOffset.z + 4.0f) * 0.5f;

			m_pGmTitleIcon->Draw(pkBuffer, pos, 0, NULL, GUIColor::GWHITE);
		}
	}
	if (bShowTitle && iTitleID > 0 && LC_TitleManager::GetSingletonPtr())
	{
		m_pTitleIcon = _SafeGfxGetImageFromAtlas(LC_TitleManager::GetSingleton().GetTitleRes(iTitleID).c_str());
		if(NULL != m_pTitleIcon)
		{
			float wid = m_pTitleIcon->GetWidth();
			float hei = m_pTitleIcon->GetHeight();
			GUIRect kTitlePos(kScreenPos.x + (m_fBloodWidth - wid)*0.5f, fTop - hei,0,0);
			kTitlePos.SetWidth(wid);
			kTitlePos.SetHeight(hei);

			m_pTitleFlash = _SafeGfxGetImageFromAtlas(LC_TitleManager::GetSingleton().GetTitleFlashRes(iTitleID).c_str());
			if (NULL != m_pTitleFlash )
			{
				wid = m_pTitleFlash->GetWidth();
				hei = m_pTitleFlash->GetHeight();
				GUIRect kTitlePos(kScreenPos.x + (m_fBloodWidth - wid)*0.5f, fTop -(hei*0.5f + m_pTitleIcon->GetHeight()*0.5f),0,0);
				kTitlePos.SetWidth(wid);
				kTitlePos.SetHeight(hei);
				mTitleFlashRect = kTitlePos;
			}
			if (!LC_TitleManager::GetSingleton().IsTitleOnlyFlash(iTitleID))//武林游戏称号只显示特效
			{
				m_pTitleIcon->Draw(pkBuffer, kTitlePos, 0, NULL, GUIColor(1,1,1,1));
			}			

			fTop += -kTitlePos.GetHeight();
		}
	}

	if((iAnnotateMask & UE_PlayerName::AT_ENEMY))
	{
		if(NULL != m_pEnemyIcon) 
		{
			GUIRect kEnemyPos(kScreenPos.x - m_pEnemyIcon->GetWidth()*0.5f, fTop - m_pEnemyIcon->GetHeight(),0,0);
			kEnemyPos.SetWidth(m_pEnemyIcon->GetWidth());
			kEnemyPos.SetHeight(m_pEnemyIcon->GetHeight());

			m_pEnemyIcon->Draw(pkBuffer, kEnemyPos, 0, NULL, GUIColor(1,1,1,1));

			fTop += -kEnemyPos.GetHeight();
		}
	}

	if ((iAnnotateMask & UE_PlayerName::AT_DOMAIN_HOLDER) && m_pDomainIcon)
	{
		GUIRect kHolyFlamPos(renderpos.x + fWidth, fTop,0,0);
		kHolyFlamPos.m_left = kScreenPos.x - m_kHolyFlamOffset.x * 0.5f;
		kHolyFlamPos.m_right = kHolyFlamPos.m_left + m_kHolyFlamOffset.x;
		kHolyFlamPos.m_top += m_kHolyFlamOffset.z;
		kHolyFlamPos.m_bottom = kHolyFlamPos.m_top + m_kHolyFlamOffset.y;
		m_pDomainIcon->Draw(pkBuffer, kHolyFlamPos, 0, NULL, GUIColor(1,1,1,1));

		fTop += m_kHolyFlamOffset.z;
	}

	if ((iAnnotateMask & UE_PlayerName::AT_COOLDOWN))
	{
		StringStreamType sStream("");
		sStream << iParamVec[UE_PlayerName::ATBIT_COOLDOWN];

		FT_Font* pFont = FT_FontManager::GetSingleton().GetFont(FONT_TYPE_Number, FONT_MODE_SHADOW);//UE 人名不要描边
		if ( NULL != pFont )
		{
			GUIRect rect(kScreenPos.x - m_fBloodWidth*0.5f + g_settings.UE_COOLDOWN_OFFSET.x, fTop + g_settings.UE_COOLDOWN_OFFSET.y, 0, 0);
			rect.SetWidth(m_fBloodWidth);
			rect.SetHeight(pFont->GetFontHeight(g_settings.UE_COOLDOWN_OFFSET.z));

			pFont->DrawText(pkBuffer, sStream.str(), rect, 0, NULL, DT_CENTER + DT_VCENTER, GUIColor::GWHITE, FONT_MODE_SHADOW, g_settings.UE_COOLDOWN_OFFSET.z, g_settings.UE_COOLDOWN_OFFSET.z);
		}
	}

	tttop = fTop;

	pkBuffer->EndRender();
	return true;
}

bool UE_RenderManager::AddAnnotateEx( GUISys::RS_GeometryBuffer* pkBuffer, int iTitleID, bool bShowOtherTitle, int iParamGuildWar, GUIRect &mTitleFlashRect,GUIRect &mGuildTitleFlashRect, UE_PlayerName* pkTargetName )
{
	T_ASSERT ( pkBuffer && pkTargetName );
	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;

	pkBuffer->BeginRender(MODE_NONE);	

	GUISys::IG_Image* titleFlash = NULL;
	if (bShowOtherTitle && iTitleID > 0)
	{	
		titleFlash = _SafeGfxGetImageFromAtlas(LC_TitleManager::GetSingleton().GetTitleFlashRes(iTitleID).c_str());
		if (titleFlash != NULL)
		{
			int index = pkTargetName->GetFlashIndex(GET_PROCESS_TIME,titleFlash->GetCount(),120);
			titleFlash->Draw(index,pkBuffer, mTitleFlashRect, 0, NULL, GUIColor(1,1,1,1));
		}
	}

	pkBuffer->EndRender();

	return true;
}

bool UE_RenderManager::AddNameInfoWithBG(GUISys::RS_GeometryBuffer* pkBuffer,
										 const StringType &sName,
										 const StringType &sTitle,
										 const StringType &sDebug,
										 const NiColor &kNameColor,
										 const NiColor &kTitleColor,
										 const NiColor &kDebugColor,
										 float fNameAlpha,
										 int iNameFrameState,
										 const GUIRect &kNameArea)
{
	T_ASSERT (pkBuffer);
	if (UE_UiEffectManager::m_iActiveLevel >= 1)
		return false;

	FT_Font* pFont = FT_FontManager::GetSingleton().GetFont(m_iFont,FONT_MODE_SHADOW);//UE 人名不要描边
	if ( pFont == NULL )
	{
		return false;
	}
	//pkBuffer->BeginRender(MODE_NOALIGNTEXT);
	T_PROFILE("UE_RenderManager::AddNameInfoWithBG");

	GUIRect namerect;
	namerect.SetPosition(kNameArea.m_left, kNameArea.m_top + 1.0f);
	namerect.SetWidth(m_fBloodWidth + 2.0f);
	namerect.SetHeight(m_kBloodOffset.z + 2.0f);
	namerect.SetPosition(kNameArea.m_left + (kNameArea.GetWidth() - namerect.GetWidth())* 0.5f, kNameArea.m_top + 2.0f);
	
	IG_ImagePtr spIconBG = NULL;
	if (UE_PlayerName::NFS_NORMAL == iNameFrameState
	|| UE_PlayerName::NFS_HOVER == iNameFrameState)
	{
		spIconBG = m_pNameBGIconNormal;
	}
	
	if(spIconBG)
		spIconBG->Draw(pkBuffer, kNameArea, 1, NULL, GUIColor(1,1,1,1));

	_RenderTextName(pkBuffer,sName,GUIColor(kNameColor.r,kNameColor.g,kNameColor.b,fNameAlpha),namerect,pFont);
	
	pkBuffer->UpdateTitleFontIndex();

	namerect.Offset(0,m_kNameOffset.z);
	_RenderTextName(pkBuffer,sTitle,GUIColor(kTitleColor.r,kTitleColor.g,kTitleColor.b,1),namerect,pFont);

	pkBuffer->EndRender();
	return true;
}


bool UE_RenderManager::AddInteractiveBtn(GUISys::RS_GeometryBuffer* pkBuffer, int iFrameState, GUISys::GUIRect &kRect)
{
	T_PROFILE("UE_RenderManager::AddInteractiveBtn");

	T_ASSERT (pkBuffer);
	
	if(NULL != m_pNPCToyIcon) 
	{
		kRect.m_left -= m_pNPCToyIcon->GetWidth()*0.5f;
		kRect.m_top -= m_pNPCToyIcon->GetHeight();
		kRect.SetWidth(m_pNPCToyIcon->GetWidth());
		kRect.SetHeight(m_pNPCToyIcon->GetHeight());
		m_pNPCToyIcon->Draw(iFrameState, pkBuffer, kRect, 0, NULL, GUIColor(1,1,1,1));
	}

	return true;
}

NiTimeControllerPtr UE_RenderManager::_getNiTimeController( NiNodePtr spNode )
{
	NiAVObject *pAVObject = spNode->GetObjectByName("Dummy01");
	NiTimeController* spController = pAVObject->GetControllers();
	return spController;
}

void UE_RenderManager::_renderUEFX()
{
	for(int i =0; i < m_kScreenElementsArray.size(); i++)
	{
		RS_ScreenElements* pScreenElements = m_kScreenElementsArray[i];
		pScreenElements->RenderAllBuffer();
		pScreenElements->Clear();
	}
	m_kScreenElementsArray.clear();
}

void UE_RenderManager::Update( float fTime )
{
	//UpdateOffset();
	//UpdateOffset();
}

void UE_RenderManager::LoadImgsets()
{
	//m_pRedImg = GFxGetImageFromAtlas("v2/v2_processba_生命值");
	//m_pBlueImg = GFxGetImageFromAtlas("v2/v2_processba_内息值");
	//m_pBoundImg = GFxGetImageFromAtlas("v2/v2_image_血条底");

	/*
		UE_RenderManager管理的image创建规则：
		只有在loadImageSet中加载的image, 且保证在整个渲染流程中不会被覆盖的image, 才可以直接使用GFxGetImageFromAtlas赋值；
		否则必须调用_SafeGetImageFromAtlas；

		author: xuyadong
	*/
	m_pPBloodBKImg = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_PBLOODBKIMG));
	m_pBoundImg = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_BOUNDIMG));
	m_pBoundBkImg = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_BOUNDBKIMG));
	m_pBoundBossImg = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_BOUNDBOSSIMG));
	m_pBoundMonsterImg = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_BOUNDMONSTERIMG));
	m_pMRedImgBigDeep = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_MREDIMGBIGDEEP));
	m_pMRedImgBigLight = NULL;
	m_pMRedImgSmallDeep = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_MREDIMGSMALLDEEP));
	m_pMRedImgSmallLight = NULL;
	m_pPGreenImgDeep = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_PGREENIMGDEEP));
	m_pPGreenImgLight = NULL;
	m_pPBlueImgDeep = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_PBLUEIMGDEEP));
	m_pPBlueImgLight = NULL;
	m_pPRedImgDeep = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_PREDIMGDEEP));
	m_pPRedImgLight = NULL;

	m_pGroupLeader = NULL;//GFxGetImageFromAtlas("Common1/Common1_image_队长角色图标");
	m_pGroupMember = NULL;//GFxGetImageFromAtlas("Common1/Common1_image_队员角色图标");
	m_pOtherGroupMember = NULL;
	m_pNameBGIconNormal = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_NAMEBGICONNORMAL));
	m_pNameBGIconHover = NULL;
	m_pDomainIcon = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_DOMAINICON));
	m_pVillainIcon = NULL;//GFxGetImageFromAtlas("Common1/Common1_image_大恶人头");
	m_pEnemyIcon = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_ENEMYICON));
	m_pNPCToyIcon = GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_NPCTOYICON));

	m_spTaskAcceptIcon = NULL;//GFxGetImageFromAtlas("图标_任务/可接任务.dds");
	m_spTaskAcceptIcon2 = NULL;//GFxGetImageFromAtlas("图标_任务/可接任务2.dds");
	m_spTaskUnFinishIcon = NULL;//GFxGetImageFromAtlas("图标_任务/未完成任务.dds");
	m_spTaskUnFinishIcon2 = NULL;//GFxGetImageFromAtlas("图标_任务/未完成任务2.dds");
	m_spTaskNoneIcon = NULL;//GFxGetImageFromAtlas("图标_任务/可接任务.dds");
	m_spTaskFinishIcon = NULL;//GFxGetImageFromAtlas("图标_任务/完成任务.dds");
	m_spTaskFinishIcon2 = NULL;//GFxGetImageFromAtlas("图标_任务/没有任务.dds");
	
	m_pNPCSkillPercentBKImg=GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_NPCSKILLPERCENTBKIMG));
	m_pNPCSkillPercentImg=GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_NPCSKILLPERCENTIMG));
	m_pEquipBkImage=GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_EQUIPBKIMAGE));
	m_pWildBossDropImage=GFxGetImageFromAtlas(GET_MSG(CLIEN_PIC_WILDBOSSDROPIMAGE));

	m_pGmTitleIcon = NULL;

	char img[256] = {0};

	//HeadInfo/Attribute_image_星星1 到  HeadInfo/Attribute_image_星星5
	m_pBuddyStarIconVec.resize(5);
	for(int i=0; i<5; ++i)
	{
		Utility::StringAppendDigital(img, GET_MSG(CLIEN_PIC_BUDDYSTARICON), i + 1);
		m_pBuddyStarIconVec[i] = GFxGetImageFromAtlas(img);
	}

	//HeadInfo/Attribute_image_星魂_0 到 HeadInfo/Attribute_image_星魂_5
	m_pRuneStarIconVec.resize(6);
	for(int i=0; i<6; ++i)
	{
		Utility::StringAppendDigital(img, GET_MSG(CLIEN_PIC_RUNESTARICON), i);
		m_pRuneStarIconVec[i] = GFxGetImageFromAtlas(img);
	}

	//HeadInfo/Attribute_image_装备_0 到 HeadInfo/Attribute_image_装备_5
	m_pEquipStarIconVec.resize(6);
	for(int i=0; i<6; ++i)
	{
		Utility::StringAppendDigital(img, GET_MSG(CLIEN_PIC_EQUIPSTARICON), i);
		m_pEquipStarIconVec[i] = GFxGetImageFromAtlas(img);
	}

	m_pVipLevelIcon.resize(26);
	for (int i=0;i<26;++i)
	{
		Utility::StringAppendDigital(img, GET_MSG(CLIEN_PIC_VIPLEVELICON), i);
		m_pVipLevelIcon[i] = GFxGetImageFromAtlas(img);
	}

	m_pCommanderTitleLevelIcon.resize(26);
	for (int i=0;i<26;i++)
	{
		Utility::StringAppendDigital(img, GET_MSG(CLIEN_PIC_COMMANDERTITLELEVELICON), i);
		m_pCommanderTitleLevelIcon[i] = GFxGetImageFromAtlas(img);
	}

	//HeadInfo/Common1_image_头顶血条底板_1 到  HeadInfo/Common1_image_头顶血条底板_4
	//HeadInfo/Common1_image_头顶血条底板进阶_1 到  HeadInfo/Common1_image_头顶血条底板进阶_4
	//HeadInfo/Common_image_系别1 到  HeadInfo/Common_image_系别4
	m_pHpBackVec.resize(4);
	m_pHpBackAdvVec.resize(4);
	m_pNPCStarIcon.resize(4);
	for(int i=0; i<4; ++i)
	{
		Utility::StringAppendDigital(img, GET_MSG(CLIEN_PIC_HPBACK), i + 1);
		m_pHpBackVec[i] = GFxGetImageFromAtlas(img);

		Utility::StringAppendDigital(img, GET_MSG(CLIEN_PIC_HPBACKADV), i + 1);
		m_pHpBackAdvVec[i] = GFxGetImageFromAtlas(img);

		Utility::StringAppendDigital(img, GET_MSG(CLIEN_PIC_NPCSTARICON), i + 1);
		m_pNPCStarIcon[i] = GFxGetImageFromAtlas(img);
	}
}

unsigned int UE_RenderManager::_renderGeometry(RS_GeometryBuffer *pGeometryBuffer) 
{
	unsigned int nDrawObjects = 0;

	const IG_Material* pCurImageSet = NULL;
	RS_GeometryBuffer::QuadVector::iterator iter;
	{
		RS_GeometryBuffer::QuadVector & qv = pGeometryBuffer->m_QuadImgList;
		if (qv.size() > 0)
		{
			std::sort(qv.begin(), qv.end(), QuadInfo::ue_sort_cmp_z_material);
			TRAV( qv , iter )
			{
				nDrawObjects += _submitQuad( iter, pCurImageSet);
			}
		}
	}
	{
		RS_GeometryBuffer::QuadVector & qv = m_kPlayerNames;
		if (qv.size() > 0)
		{
			std::sort(qv.begin(), qv.end(), QuadInfo::ue_sort_cmp_z_material);
			TRAV( qv , iter )
			{
				nDrawObjects += _submitQuad( iter, pCurImageSet);
			}
		}
	}
	{
		RS_GeometryBuffer::QuadVector & qv = m_kPlayerAnnotations;
		if (qv.size() > 0)
		{
			std::sort(qv.begin(), qv.end(), QuadInfo::ue_sort_cmp_z_material);
			TRAV( qv , iter )
			{
				nDrawObjects += _submitQuad( iter, pCurImageSet);
			}
		}
	}
	{
		RS_GeometryBuffer::QuadVector & qv = m_kTitleFlashs;
		if (qv.size() > 0)
		{
			std::sort(qv.begin(), qv.end(), QuadInfo::ue_sort_cmp_z_material);
			TRAV( qv , iter )
			{
				nDrawObjects += _submitQuad( iter, pCurImageSet);
			}
		}
	}

	nDrawObjects += m_spElementInstancing->RenderAllBuffer();
	m_spElementInstancing->Clear();
	
	nDrawObjects += m_spFontInstancing->RenderAllBuffer();
	m_spFontInstancing->Clear();

	return nDrawObjects;
}

NiPoint2 UE_RenderManager::To_Screen_Pos( const NiPoint3 &kTransPos )
{
	NiRenderer* pkRenderer = NiRenderer::GetRenderer();
	const NiRenderTargetGroup* pkRTGroup = pkRenderer->GetDefaultRenderTargetGroup();
	GUIRect kScreenBounds(0, 0,(float)pkRTGroup->GetWidth(0),(float)pkRTGroup->GetHeight(0));
	NiPoint2 screenPos;
	screenPos.x = kTransPos.x / 1024.0f * m_fTransXFactor;
	screenPos.y = kTransPos.z / 728.0f  * m_fTransYFactor;
	screenPos.x =  screenPos.x * kScreenBounds.GetWidth() + m_fTransXOffset;
	screenPos.y =  screenPos.y * kScreenBounds.GetHeight();
	return screenPos;
}

void UE_RenderManager::AddScreenElements( RS_ScreenElements* pElements )
{
	m_kScreenElementsArray.push_back(pElements);
}

void UE_RenderManager::SubmitPlayerNames( GUISys::RS_GeometryBuffer* pkBuffer )
{
	RS_GeometryBuffer::QuadVector::iterator iter =  pkBuffer->m_QuadImgList.begin();
	TRAV( pkBuffer->m_QuadImgList , iter )
	{
		GUISys::QuadInfo *quad = *iter;
		m_kPlayerNames.push_back( quad );
	}
}

void UE_RenderManager::SubmitPlayerAnnotations( GUISys::RS_GeometryBuffer* pkBuffer )
{
	RS_GeometryBuffer::QuadVector::iterator iter =  pkBuffer->m_QuadImgList.begin();
	TRAV( pkBuffer->m_QuadImgList , iter )
	{
		GUISys::QuadInfo *quad = *iter;
		m_kPlayerAnnotations.push_back( quad );
	}
}

void UE_RenderManager::SubmitTitleFlash( GUISys::RS_GeometryBuffer* pkBuffer )
{
	RS_GeometryBuffer::QuadVector::iterator iter =  pkBuffer->m_QuadImgList.begin();
	TRAV( pkBuffer->m_QuadImgList , iter )
	{
		GUISys::QuadInfo *quad = *iter;
		m_kTitleFlashs.push_back( quad );
	}
}

unsigned int UE_RenderManager::_submitQuad( RS_GeometryBuffer::QuadVector::iterator& iter, 
								    const IG_Material* &pCurImageSet) const
{
	unsigned int nDrawObjects = 0;

	QuadInfo* quad = *iter;
	RS_ScreenElementsPtr spInstancing = quad->pImage ? m_spFontInstancing : m_spElementInstancing;

	quad->dest_rect = quad->draw_rect;
	if(pCurImageSet != quad->pMaterial )//如果Texture不同,则提交数据
	{
		nDrawObjects += spInstancing->RenderAllBuffer();
		spInstancing->ClearGeometery();
	
		spInstancing->SetBaseMaterial( quad->pMaterial,quad->filtertype);
		spInstancing->SetSecondMaterial(NULL);
		spInstancing->SetShaderMode(RS_ScreenElementShader::SM_Normal);
	
		pCurImageSet = quad->pMaterial;
	}
	if( false == spInstancing->AddQuad(quad) )
	{
		nDrawObjects += spInstancing->RenderAllBuffer();
		spInstancing->ClearGeometery();
		spInstancing->AddQuad(quad);
	}
	return nDrawObjects;
}

void UE_RenderManager::_updateSourceTextureArea(GUISys::IG_ImagePtr pkImg, const GUISys::GUIRect& rect)
{
	NiTexturePtr spNiTexture = pkImg->GetTexture();
	if(spNiTexture)
	{
		GUISys::GUIRect ImgSetRect(GUISys::GUIPoint2(0,0),GUISys::GUISize(spNiTexture->GetWidth(),spNiTexture->GetHeight()));
		GUISys::GUIRect DestRect = GUISys::GUIRect::GetIntersectRect(rect,ImgSetRect);

		pkImg->SetSourceTextureArea(DestRect);
	}

}

void UE_RenderManager::_RenderTextName(GUISys::RS_GeometryBuffer* pkBuffer, const StringType &sName, GUIColor &kNameColor, const GUISys::GUIRect &kNameArea, FT_Font* pFont)
{
	/*StringType BufferText(sName);
	StringStreamType sStream;
	sStream << "<color val =" << kNameColor.GetARGB() << ">";
	BufferText.insert(0,sStream.str());
	BufferText.append("</color>");
	CL_RichText *m_pRichCaption = getRichTextParse->ParseText(BufferText,FONT_MODE_SHADOW,false,m_iFont);
	if (m_pRichCaption)
	{
		if(!m_pRichCaption->IsFormated())
		{
			m_pRichCaption->Format(kNameArea.GetSize(), false);
		}
		m_pRichCaption->SetRenderMode(FONT_MODE_SHADOW);
		m_pRichCaption->DrawRichText(pkBuffer,kNameArea,0,NULL,dtCenter, 1, 1);
		CL_RichTextManager::GetSingleton().DestroyRichText(m_pRichCaption->GetIndex());
		m_pRichCaption = NULL;
	}
	else
	{*/
	T_ASSERT_CRITICAL_S(pFont != NULL);
	
	if(sName.empty())
		return;

	pFont->DrawText(pkBuffer,sName,kNameArea,0,NULL,dtCenter,
			kNameColor, FONT_MODE_SHADOW);//描边绘制
	//}
}


void UE_RenderManager::_ClearGfxImageReferences()
{
	m_kImageMaterialReferenceTempMap.clear();
}

GUISys::IG_Image* UE_RenderManager::_SafeGfxGetImageFromAtlas(const char * pName)
{
	if (pName[0] == '\0')
		return NULL;

	GUISys::IG_Image* pkImage = GUISys::GFxGetImageFromAtlas(pName);
	if ( pkImage )
	{
		m_kImageMaterialReferenceTempMap.insert(std::make_pair(pkImage, pkImage));
	}

	return pkImage;
}
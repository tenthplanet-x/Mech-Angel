
#include "ClientPCH.h"
#include "LM_LocaleManager.h"
#include "CA_CameraManager.h"
#include "GD_MaterialManager.h"
#include "GD_ModelContent.h"
#include "UI_UIManager.h"
#include "LC_ClientNetPlayerManager.h"
#include "LC_ClientLogicManager.h"
#include "LC_ClientGamePlayer.h"
#include "LC_ClientWorldManager.h"
#include "LC_ClientNPCManager.h"
#include "LC_ClientArenaManager.h"
#include "UE_PlayerName.h"
#include "UE_RenderManager.h"
#include "IM_Manager.h"
#include "GD_ClientActorModel.h"
#include "LC_ClientNPC.h"
#include "UT_CutSceneManager.h"
#include "LC_ClientTaskMap.h"
#include "IG_Material.h"
#include "FT_FontManager.h"
#include "FT_Font.h"
#include "ClientTextDefine.h"
#include "LC_ClientPlaceHolder.h"
#include "UE_UiEffectManager.h"
#include "LC_TooltipBase.h"
#include "LC_ClientNetPlayer.h"
#include "NW_ClientSessionManager.h"
#include "LC_GuildRaidDataManager.h"
#include "UT_EngineMath.h"
#include "LC_TitleManager.h"
#include "CR_HelpFunction.h"
#include "CL_RichTextParse.h"

using namespace Data;
using namespace Utility;
using namespace GameLogic;
using namespace CSVFile;
using namespace GUISys;
using namespace StateMachine;

//------------------------------------------------------------------------------
UE_PlayerName::UE_PlayerName(UI_EFFECT_TYPE eType)
: UE_UiEffectBase(eType),
	m_bNeedRedraw(false),
	m_pkActor(NULL),
	m_pkPlayer(NULL),
	m_nNameFrameState(NFS_NORMAL),
	m_bShowInteractiveBtn(false),
	m_bMouseType(false),
	m_bShowOtherTitle(true)
{
	m_pkFontBuffer		= T_NEW_D RS_GeometryBuffer;
	m_pkAnnotateBuffer	= T_NEW_D RS_GeometryBuffer;
	m_pkTitleFlashBuffer= T_NEW_D RS_GeometryBuffer;
	m_fRichTextOffset	= 0;
	m_fPlaceHolderCurLifeTime	= GET_PROCESS_TIME + g_settings.UE_PLACEHOLDER_NAME_LIFE_TIME;
	m_nTitleID = 0;
	m_nParamVec.clear();
	m_iLastFlashPic = -1;
	m_iLastFlashPicTime = 0;
	m_iGuildLastFlashPic = -1;
	m_iGuildLastFlashPicTime =0;

	m_placeHolderTime = 0;
}
//------------------------------------------------------------------------------
UE_PlayerName::~UE_PlayerName()
{
	UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
	pkUIEffectMgr->DeletePlaceHolderName(m_lTargetID);
	T_SAFE_DELETE( m_pkFontBuffer );
	T_SAFE_DELETE( m_pkAnnotateBuffer );
	T_SAFE_DELETE( m_pkTitleFlashBuffer);
}
//------------------------------------------------------------------------------
bool UE_PlayerName::Init()
{
	m_fRichTextOffset = 0;
	m_nTitleID = 0;
	m_nParamVec.clear();
	m_kNameTop = 0;
	m_kGuildColor = NiColor(1.0f, 163.0f/255.0f, 0.0f);		//FFA300
	return true;
}
//------------------------------------------------------------------------------
void UE_PlayerName::Show(float lifeTime /*= 0.0f*/)
{
	_findActorHeight();
	m_bVisible = true;
	SetPlaceHoldeTime(lifeTime);
	ResetTime();
}
//------------------------------------------------------------------------------
void UE_PlayerName::Hide()
{
	m_lTargetID = INVALID_ACTOR_ID;
	m_bVisible = false;
	m_nTitleID = 0;
	m_nParamVec.clear();

	UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
	pkUIEffectMgr->DeletePlaceHolderName(m_lTargetID);
}
//------------------------------------------------------------------------------
bool UE_PlayerName::Update(float fCurrentTime)
{
	if(!UT_CutSceneManager::GetSingleton().ShowPlayerName())
		return true;

	if ( !UE_UiEffectBase::Update( fCurrentTime ) )
	{
		return false;
	}

	
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	//LC_ClientNetPlayerManager* pkNetPlayerManager = CLIENT_GET_NETPLAYER_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if (!pkGamePlayer)
		return true;

	m_pkPlayer = pkLogicManager->FindPlayer(m_lTargetID);
	bool bNetPlayer = pkGamePlayer != m_pkPlayer;

	m_pkActor = (LC_ActorBase*)(pkWorldManager->FindObject(m_lTargetID));
	if (!m_pkActor)
		return true;

	if (m_pkActor->GetType() == LOT_PLACEHOLDER)
	{
		LC_PlaceHolderBase* pkPlaceHolderBaseModel = (LC_PlaceHolderBase*)m_pkActor;
		if (pkPlaceHolderBaseModel->GetReward().m_eType != GameLogic::LC_Reward::RT_EFFECT)
		{
			if(m_fPlaceHolderCurLifeTime < 0)
				return true;
			if (m_fPlaceHolderCurLifeTime < fCurrentTime)
			{
				m_fPlaceHolderCurLifeTime = -1;
				UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
				pkUIEffectMgr->DeletePlaceHolderName(m_lTargetID);
				return true;
			}
		}
	}

	if(!m_bVisible)
	{
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
		pkUIEffectMgr->DeletePlaceHolderName(m_lTargetID);
		return true;
	}

	/**
	����һ������������ֺͳƺ���Ⱦ��Ч��
	��ǰ�ı�����������ÿһ��Update��ʱ��
	������������Ű棬������������ܴ��
	��û�б�Ҫ����Ϊ���ֵ�һ��Show��֮��
	��ֻ����ɫ����Alpha��λ�ûᷢ���ı�
	���Լ���һ������Quad��Buffer��һ�����ͼ��Quad��
	Buffer�������ֿ���������ĺô��ǻ��Ƶ�ʱ��
	��������Ҳ����Ȼ�ļ��������л�������DP
	*/

	//_findActorHeight();

	UT_Vec3Int kPosition;
	if (m_pkActor->GetDead())
	{
		GD_ClientActorModel* pkModel = IM_Manager::GetSingleton().GetActor(m_lTargetID);
		if(pkModel)
			kPosition = UT_EngineMath::ConvertNiPoint3ToVec3Int(pkModel->GetFxDummyNodePosition(GD_ModelBase::FDT_HEART));
		else
			kPosition = m_pkActor->GetVisibleLocation();
	}
	else
	{
			kPosition = m_pkActor->GetVisibleLocation();
			_findActorHeight();
			kPosition.z += m_lActorHeight;

		/*if ( IS_PLAYER_CLASS_ID( m_pkActor->GetID()) 
			&& (StateMachine::IS_PLAYER_JUMP_STATE(m_pkActor->GetCurrentStateType()))
			&& (m_pkActor->GetRiderCharType() != 0) )
		{
			kPosition.z  += UE_PLAY_NAME_BOLLD_CORRECT;
		}*/

		if ( m_pkActor->GetStateExtendData(GOD_STATE_ID, GOD_STATE_ITEMTYPE) == GOD_ITEM_4 )
		{
			kPosition.z += 40;
		}
	}	

	m_kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kPosition);

	NiPoint3 kScreenPos = UE_RenderManager::GetSingleton().ConvertToScreenPos(m_kPos);
	NiPoint3 renderpos;
	
	bool bNpc = IS_NPC_ID( m_lTargetID ) || IS_LOCAL_NPC_ID(m_lTargetID);
	bool bToyPet = false;

	if (IS_CHEST_ID(m_lTargetID))
	{
		renderpos.x = kScreenPos.x + g_settings.UE_PLACEHOLDER_NAME_OFFSET.x;
		renderpos.y = kScreenPos.y + g_settings.UE_PLACEHOLDER_NAME_OFFSET.y;
	}
	else if (bNpc)
	{
		renderpos.x = kScreenPos.x + g_settings.UE_MONSTER_NAME_OFFSET.x;
		renderpos.y = kScreenPos.y + g_settings.UE_MONSTER_NAME_OFFSET.y;
	}
	else
	{
		renderpos.x = kScreenPos.x + g_settings.UE_PLAYER_NAME_OFFSET.x;
		renderpos.y = kScreenPos.y + g_settings.UE_PLAYER_NAME_OFFSET.y;
	}

	//����float����ֵ��ԭ�����ֿ��ܻᶶ������ֱ��ȡ����������ֶ�
	renderpos.x = (int)(renderpos.x + 0.5f);
	renderpos.y = (int)(renderpos.y + 0.5f);

	//�������Ļ�⣬��ô����ʱ����Ⱦ�ˣ����´θ����ټ�飬���Է���true
	if ( renderpos.x < 0 || renderpos.y < 0 )
	{
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
		pkUIEffectMgr->DeletePlaceHolderName(m_lTargetID);
		return true;
	}
	
	bool bShowName = true;
	int nNewAnnotateMask = 0;
	int iParam = 0;
	StringType kCurrentGuildName;
	std::vector<int> iParamVec;
	iParamVec.resize(ATBIT_MAX);
	//gm��ʼ��Ȩ��
	iParamVec[ATBIT_GM_TITLE_INFO] = MAX_GM_RPIORITY;

	m_bCanAttack = false;

	//�Ƿ����״̬
	CF_ActorFilterFunc::DataEntry* pkData = CF_ActorFilterFunc::GetSingletonPtr()->GetEntryPtr(2);
	m_bCanAttack = (LC_Helper::IsValidTarget(pkGamePlayer,m_lTargetID,pkData) == RE_SKILL_CHECK_VALID_TARGET);
	
	int iTitleLevel = 0;

	if(m_pkPlayer)
	{
		//bool bPlatformYear = false;
		//bool bPlatformHigh = false;
		if ( bNetPlayer )
		{
			//�������
			if ( m_bCanAttack )
				bShowName = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_ENEMY_NAME );
			else if ( ((LC_ClientNetPlayer*)m_pkPlayer)->GetOwnerObjectID() == pkGamePlayer->GetMainControllerID() ) // �ҵ�buddy
			{
				bShowName  = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_PLAYER_NAME );
			}
			else
				bShowName = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_FRIEND_NAME );
			/*
			bPlatformYear = ((LC_ClientNetPlayer*)m_pkPlayer)->IsPlatformYearVip(VipType);
			bPlatformHigh = ((LC_ClientNetPlayer*)m_pkPlayer)->IsPlatformHighVip(VipType);
			iParamVec[ATBIT_PLATFORM_VIP] = ((LC_ClientNetPlayer*)m_pkPlayer)->GetPlatformVipLevel(VipType);
			iParamVec[ATBIT_PLATFORM_VIP_YEAR] = bPlatformYear ? 1 : 0;
			iParamVec[ATBIT_PLATFORM_VIP_HIGH] = bPlatformHigh ? 1 : 0;*/
		}
		else
		{
			//�Լ�
			bShowName  = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_PLAYER_NAME );
			/*bPlatformYear = ((LC_ClientGamePlayer*)m_pkPlayer)->IsPlatformYearVip(VipType);
			bPlatformHigh = ((LC_ClientGamePlayer*)m_pkPlayer)->IsPlatformHighVip(VipType);
			iParamVec[ATBIT_PLATFORM_VIP] = ((LC_ClientGamePlayer*)m_pkPlayer)->GetPlatformVipLevel(VipType);
			iParamVec[ATBIT_PLATFORM_VIP_YEAR] = bPlatformYear ? 1 : 0;
			iParamVec[ATBIT_PLATFORM_VIP_HIGH] = bPlatformHigh ? 1 : 0;*/
		}

		////����Ҫ����
		//ʥ����
		if (m_pkPlayer->IsSkillStateExist(GuildWarTmpStateID))
		{
			nNewAnnotateMask |= AT_HOLY_FLAM_HOLDER;
		}
		//������
		if (m_pkPlayer->IsSkillStateExist(GuildWarTmpStateID_T))
		{
			nNewAnnotateMask |= AT_VORPAL_BLADE_HOLDER;
		}
		//����
		bool bIsMain = bNetPlayer && ((LC_ClientNetPlayer*)m_pkPlayer)->IsMainNetPlayer();
		bIsMain = bIsMain || !bNetPlayer;
		if (m_pkPlayer->IsSkillStateExist(DomainWarTmpState_ID) 
			&& IS_PLAYER_CLASS_ID(m_pkPlayer->GetID())
			&& bIsMain)
		{
			nNewAnnotateMask |= AT_DOMAIN_HOLDER;
		}
		//�����
		if(m_pkPlayer->IsSkillStateExist(DAEREN))
		{
			nNewAnnotateMask |= AT_VILLAIN;
		}
		//����ʱ
		iParamVec[ATBIT_COOLDOWN] = m_pkPlayer->GetStateRemainTime(UpgradeSuccessState);
		if (iParamVec[ATBIT_COOLDOWN] > 0)
		{
			nNewAnnotateMask |= AT_COOLDOWN;
		}

		if (bShowName)
		{
			if (bNetPlayer)
			{
				if (((LC_ClientNetPlayer*)m_pkPlayer)->IsMainNetPlayer() && m_pkPlayer->GetPlayerGroupCheifFlag() )
				{
					nNewAnnotateMask |= AT_TEAM_LEAD;
				}
			}
			else
			{
				if( m_pkPlayer->GetPlayerGroupCheifFlag() )
				{
					nNewAnnotateMask |= AT_TEAM_LEAD;
				}
			}
			
			if ( m_pkPlayer->GetPlayerGroupID() != 0 )
			{
				if(bNetPlayer)
				{
					if ( ((LC_ClientNetPlayer*)m_pkPlayer)->IsMainNetPlayer() )
					{
						if (m_pkPlayer->GetPlayerGroupID() != pkGamePlayer->GetPlayerGroupID() )
						{
							nNewAnnotateMask |= AT_OTHER_GROUP;
						}
						else if (m_pkPlayer->GetPlayerGroupID() == pkGamePlayer->GetPlayerGroupID() )
						{
							nNewAnnotateMask |= AT_GROUP;
						}
					}
					
				}
				else
					nNewAnnotateMask |= AT_GROUP;
			}
			
			if (bNetPlayer)
			{
				LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)m_pkPlayer;
				if ( pkNetPlayer->IsMainNetPlayer())
				{
					iTitleLevel = pkNetPlayer->GetCommanderTitleLevel();
				}
			} 
			else
			{
				if (pkGamePlayer->GetCommanderTitleOpen() && pkGamePlayer->GetCommanderTitleLevel() > 0)
				{
					iTitleLevel = pkGamePlayer->GetCommanderTitleLevel();
				}
			}
			//����VIP������Ҫ������VIPͷ������ʾVIP��־
			if (bNetPlayer)
			{
				if ( ((LC_ClientNetPlayer*)m_pkPlayer)->IsMainNetPlayer() && ((LC_ClientNetPlayer*)m_pkPlayer)->GetCommanderNewVipLevel() > 0)
				{
					iParamVec[ATBIT_VIP] = ((LC_ClientNetPlayer*)m_pkPlayer)->GetCommanderNewVipLevel();
					nNewAnnotateMask |= AT_VIP;
				}
			}
			else
			{
				if (pkGamePlayer->GetVipNewLevel() > 0)
				{
					iParamVec[ATBIT_VIP] = pkGamePlayer->GetVipNewLevel();
					nNewAnnotateMask |= AT_VIP;
				}
			}

			/*if(bPlatformYear)
			{
				nNewAnnotateMask |= AT_PLATFORM_VIP_YEAR;
			}
			if (bPlatformHigh)
			{
				nNewAnnotateMask |= AT_PLATFORM_VIP_HIGH;
			}*/
			if(iParamVec[ATBIT_PLATFORM_VIP] > 0)
			{
				nNewAnnotateMask |= AT_PLATFORM_VIP;
			}

			if (m_pkPlayer->GetGMPriority() < MAX_GM_RPIORITY && m_pkPlayer->GetGMPriority() > 0)
			{
				iParamVec[ATBIT_GM_TITLE_INFO] = m_pkPlayer->GetGMPriority();
				nNewAnnotateMask |= AT_GM_TITLE_INFO;
			}

			LC_ClientSocietyAsset *pkSocietyAsset = pkGamePlayer->GetSocietyAsset();
			LC_SocietyBasicInfo* pkSocietyInfo = pkSocietyAsset->GetEnemy(m_pkPlayer->GetInstance());
			if(pkSocietyInfo)
			{
				SocietyDegreeDetail kDegree;
				pkSocietyInfo->ParseDegree(kDegree);
				if(kDegree._peer > kDegree._local)
				{
					nNewAnnotateMask |= AT_ENEMY;
				}
			}
		}
		

		if ( !bShowName )
		{
			m_pkFontBuffer->SetAlpha( 0.0f , true );
		}
		else
		{
			m_pkFontBuffer->SetAlpha( 1.0f , true );
		}
		UT_Vec3Float kNameColor = _computeActorNameColor(pkGamePlayer, m_pkActor, m_pkPlayer, bNetPlayer && m_pkPlayer->GetPlayerGroupID() != 0 && m_pkPlayer->GetPlayerGroupID() == pkGamePlayer->GetPlayerGroupID());
		float		 fNameAlpha = 1.0f;
		if ( kNameColor.x != m_kCharColor.r || kNameColor.y != m_kCharColor.g || kNameColor.z != m_kCharColor.b )
		{
			m_kCharColor = NiColor(kNameColor.x,kNameColor.y,kNameColor.z);
			m_bNeedRedraw = true;
			//m_pkFontBuffer->SetColor( m_kCharColor , true );
		}
		UT_Vec3Float kTitleColor = _computeActorNameColor(pkGamePlayer, m_pkActor, m_pkPlayer);
		if ( kTitleColor.x != m_kTitleColor.r || kTitleColor.y != m_kTitleColor.g || kTitleColor.z != m_kTitleColor.b )
		{
			m_kTitleColor = NiColor(kTitleColor.x,kTitleColor.y,kTitleColor.z);
			//m_pkFontBuffer->SetColor( m_kTitleColor , false );
			m_bNeedRedraw = true;
		}

		StringType kCurrentCharName = "";
		//new ��ʾ����ɫ����
		if(!bNetPlayer)
		{
			kCurrentCharName = pkGamePlayer->GetCommanderName();
			kCurrentGuildName = m_pkPlayer->GetGuildName();
		}
		else // Buddy������������������
		{
			if (pkGamePlayer->IsBuddy(m_lTargetID))
			{
				kCurrentCharName = m_pkPlayer->GetCharName();
			}
			else if(((LC_ClientNetPlayer*)m_pkPlayer)->IsMainNetPlayer())
			{
				kCurrentCharName = ((LC_ClientNetPlayer*)m_pkPlayer)->GetCommanderName();
				kCurrentGuildName = ((LC_ClientNetPlayer*)m_pkPlayer)->GetCommanderGuildName();
			}
			else
			{	
				kCurrentCharName = ((LC_ClientNetPlayer*)m_pkPlayer)->GetCommanderName() + GET_MSG(LC_CLIENTNPCMANAGER_CPP_001) + m_pkPlayer->GetCharName();
				/*object_id_type commanderID = ((LC_ClientNetPlayer*)m_pkPlayer)->GetCommanderMainID();
				LC_ClientNetPlayer* pkPlayer = (LC_ClientNetPlayer*)pkNetPlayerManager->FindPlayer(commanderID);
				kCurrentCharName = pkPlayer->GetCommanderName();
				kCurrentCharName = kCurrentCharName + "��" + m_pkPlayer->GetCharName();*/
			}
		}
		
		StringType kGuildTitleName;
		int iGuildTitleParam = GWTI_NONE;//2��ʾ�������� 1��ʾ��������
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		if (NULL != pkLogicManager)
		{
			LC_WorldGuildWarAsset* kGuildWarAsset = pkLogicManager->GetGuildWarAssert();
			if ( kGuildWarAsset )
			{
				Protocol::PS_CrossRealm_PowerGuildInfo* pkGlobalGuildInfo = kGuildWarAsset->GetGlobalPowerGuildInfo(kGuildWarAsset->GetGroupId());
				Protocol::PS_PowerGuildInfo* pkPowerGuildInfo = kGuildWarAsset->GetPowerGuildInfo(m_pkPlayer->GetServerID());
				if (pkGlobalGuildInfo && INVALID_GUILD_ID != pkGlobalGuildInfo->m_GuildId && pkGlobalGuildInfo->m_GuildId == m_pkPlayer->GetGuildID())
				{
					kGuildTitleName = CLIENT_TEXT_GLOBAL_GUILD_WAR_GUILD_TITLE;
				}
				else if (pkPowerGuildInfo && INVALID_GUILD_ID != pkPowerGuildInfo->m_GuildId && pkPowerGuildInfo->m_GuildId == m_pkPlayer->GetGuildID())
				{
					kGuildTitleName = CLIENT_TEXT_GUILD_WAR_GUILD_TITLE;
				}

				if (pkGlobalGuildInfo && INVALID_ACTOR_ID != pkGlobalGuildInfo->m_CharID && pkGlobalGuildInfo->m_CharID == m_pkPlayer->GetInstance())
				{
					iGuildTitleParam = GWTI_GLOBAL_GUILD_WAR_TITLE;
					nNewAnnotateMask |= AT_GUILD_WAR_TITLE_INFO;
				}
				else if (pkPowerGuildInfo && INVALID_ACTOR_ID != pkPowerGuildInfo->m_AllMember[0].m_CharID && pkPowerGuildInfo->m_AllMember[0].m_CharID == m_pkPlayer->GetInstance())
				{
					iGuildTitleParam = GWTI_GUILD_WAR_TITLE;
					nNewAnnotateMask |= AT_GUILD_WAR_TITLE_INFO;
				}
			}
		}
		iParamVec[ATBIT_GUILD_WAR_TITLE_INFO] = iGuildTitleParam;
		/*if(!kCurrentGuildName.empty())
		{
			
			char szTemp[256] = {0};
			sprintf_s(szTemp, sizeof(szTemp), "[%s]%s", kCurrentGuildName.c_str(), kCurrentCharName.c_str() );
			kCurrentCharName = szTemp;
		}*/
		if(!kGuildTitleName.empty())
		{
			char szTemp[256] = {0};
			sprintf_s(szTemp, sizeof(szTemp), "[%s]%s", kGuildTitleName.c_str(), kCurrentCharName.c_str() );
			kCurrentCharName = szTemp;
		}
		if (pkGamePlayer->GetGuildWarType() == WarType_CrossRealmGuildWar)
		{
			NW_ClientSessionManager* pkSessionManager = NW_ClientSessionManager::GetSingletonPtr();
			const Protocol::PS_ServerInfo* pkServerInfo		  = pkSessionManager->GetGameServerInfo(m_pkPlayer->GetServerID());
			if(NULL != pkServerInfo)
			{
				char szTemp[256] = {0};
				sprintf_s(szTemp, sizeof(szTemp), "[%s]%s", pkServerInfo->m_szName.c_str(), kCurrentCharName.c_str() );
				kCurrentCharName = szTemp;
			}			
		}
#ifndef _SHIPPING	// ��ʾ�������
		if (!bNetPlayer && true)
		{
			char szTemp[256] = {0};
			const UT_Vec3Int & pos = m_pkPlayer->GetCurrentLocation();
			sprintf_s(szTemp, sizeof(szTemp), "[%d, %d, %d]ID:%u", pos.x, pos.y, pos.z,pkGamePlayer->GetMainControllerID());
			/*kCurrentCharName = m_pkPlayer->GetCharName() + szTemp;*/
			kCurrentCharName += + szTemp;
		}
#endif
	
		//��������
		//��������
		if(m_kCharName != kCurrentCharName)
		{
			m_kCharName = kCurrentCharName;
			m_bNeedRedraw = true;
		}
		if (m_kGuildName != kCurrentGuildName)
		{
			m_kGuildName = kCurrentGuildName;
			m_bNeedRedraw = true;
		}

	}
	else
	{
		if ( bNpc)
		{
			if ( m_bCanAttack )
			{
				//Monster
				bShowName = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_MONSTER_NAME );
				if(!bShowName)
				{
					bShowName = GetMouseType();
				}
			}
			else
			{
				//Npc
				bShowName = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_NPC_NAME );
			}

			if (IS_LOCAL_NPC_ID(m_lTargetID))
			{
				bShowName = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_LOCAL_NPC_NAME );
			}

			//npc��Ӫ�ı��Ժ�ˢ��npc����
			LC_ClientNPCManager* pkNPCManager = CLIENT_GET_NPC_MANAGER();
			if (NULL != pkNPCManager)
			{
				LC_ClientNPC* pkNPC = (LC_ClientNPC*)pkNPCManager->FindNPC(m_lTargetID);
				bShowName = bShowName && (NULL != pkNPC) && pkNPC->IsShowName();

				if (NULL != pkNPC)
				{
					bToyPet = pkNPC->GetNPCFuncState(NET_TOYPET) > 0;
					if (bToyPet) // �����ʾ�ɲ�׽
					{
						nNewAnnotateMask |= AT_TOYPET;
					}
					//��������Ӫ�������ΪĿ���ʱ��Ҫ�л���ɫ
					if (pkNPC->GetFactionID() == 8)
					{
						pkNPC->SetNeedRefreshNPCName(IS_PLAYER_CLASS_ID(pkNPC->GetLockedTargetID()));
					}
					if(pkNPC->GetNeedRefreshNPCName())
					{
						pkNPC->SetNeedRefreshNPCName(false);

						UT_Vec3Float kNameColor = LC_Helper::ComputeActorNameColor(pkGamePlayer, m_pkActor);
						float		 fNameAlpha = 1.0f;
						if ( kNameColor.x != m_kCharColor.r || kNameColor.y != m_kCharColor.g || kNameColor.z != m_kCharColor.b )
						{
							m_kCharColor = NiColor(kNameColor.x,kNameColor.y,kNameColor.z);
							m_bNeedRedraw = true;
							//m_pkFontBuffer->SetColor( m_kCharColor , true );
						}
					}
				}
			}
		}
	}

	bool bUpdateAnnotatePos = true;
	if(nNewAnnotateMask == m_nAnnotationMask && !std::equal(m_nParamVec.begin(),m_nParamVec.end(),iParamVec.begin()))
	{
		m_bNeedRedraw = true;
	}

	if (bNetPlayer)
	{
		m_bShowOtherTitle = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_FRIEND_TITLE )
							&& IM_Manager::GetSingletonPtr()->GetShieldVisibility(0, IM_Manager::SHIELD_PLAYER_TITLE);
		if (m_bShowOtherTitle != m_bShowTitle)
		{
			m_bNeedRedraw = true;
			m_bShowTitle = m_bShowOtherTitle;
		}
	}
	else
	{
		m_bShowOtherTitle = g_settings.GetBoolSetting( UT_GlobalSettings::GS_REVEAL_PLAYER_TITLE );
		if (m_bShowOtherTitle != m_bShowTitle)
		{
			m_bNeedRedraw = true;
			m_bShowTitle = m_bShowOtherTitle;
		}
	}



	int ttoo = renderpos.y; 
	GUISys::GUIRect kInteractiveBtnArea(0,0,132,29);	//��Ӧ������ťͼƬ�Ĵ�С
	m_nParamVec.assign(iParamVec.begin(),iParamVec.end());
	if ( nNewAnnotateMask != m_nAnnotationMask || m_bNeedRedraw || UE_UiEffectManager::m_bActiveLevelChange)
	{
		FT_Font* pFont = FT_FontManager::GetSingleton().GetFont(g_settings.UE_FONT,FONT_MODE_NONE);
		if ( pFont == NULL )
		{
			return false;
		}
		float deltaHeight = 0.0f;
		NiPoint3 kDrawPos =renderpos;
		//Ϊ�����������ƣ����к�ƫ�ƣ�HARK����
		m_fRichTextOffset = (- pFont->GetTextExtent(m_kCharName, 1.0f))/ 2.0f;
		kDrawPos.x = renderpos.x +  m_fRichTextOffset;
		
		//��һ�θ���ͼ��֮������ȥ����λ�ã���������ۼƵ�ƫ�Ƴ���
		bUpdateAnnotatePos = false;
		m_nAnnotationMask = nNewAnnotateMask;
		
		m_pkAnnotateBuffer->Reset();
		
		if(!m_kTitleName.empty())
		{
			deltaHeight = pFont->GetFontHeight();
			kDrawPos.y -= deltaHeight;
		}
		if (!kCurrentGuildName.empty())
		{
			deltaHeight = pFont->GetFontHeight();
			kDrawPos.y -= (deltaHeight + 3);
		}
		
		UE_RenderManager::GetSingleton().AddAnnotate( m_pkAnnotateBuffer, 
			kDrawPos, 
			nNewAnnotateMask, 
			renderpos, 
			deltaHeight, 
			m_nTitleID,
			m_bShowOtherTitle, 
			iParamVec, 
			ttoo,
			m_kTitleFlashRect,
			m_kGuildTitleFlashRect);
		
		if (bToyPet)
		{
			kInteractiveBtnArea.SetPosition(kScreenPos.x, kDrawPos.y);
			UE_RenderManager::GetSingleton().AddInteractiveBtn(m_pkAnnotateBuffer, m_nNameFrameState, kInteractiveBtnArea);

			LC_ClientNPC* pkNPC = (LC_ClientNPC*)(CLIENT_GET_NPC_MANAGER()->FindNPC(m_lTargetID));
			if(pkNPC != NULL)
				pkNPC->SetInteractiveBtnArea(kInteractiveBtnArea);
		}

		m_kNameTop = ttoo - renderpos.y; 
	}

	NiPoint3 kDelta = renderpos - m_kScreenPos;
	GUIRect kNameArea;
	if (IS_CHEST_ID(m_lTargetID))
	{
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
		if ( !((LC_ClientPlaceHolder*)m_pkActor)->IsShowName() )
		{
			//�����»��������û���߶�ʱ����Ʒ����Ӧ����ʧ��û����ʧ
			m_bNeedRedraw = true;

		}
		else 
		{	((LC_ClientPlaceHolder*)m_pkActor)->GetNameArea(kNameArea);

			if ( abs( kDelta.x ) > 0.0001f || abs( kDelta.y ) > 0.0001f )
			{
				kNameArea.Offset(GUISys::GUIPoint2(kDelta.x, kDelta.y));
				pkUIEffectMgr->InsertPlaceHolderNameRect(m_lTargetID, GUISys::GUIPoint2(kNameArea.m_left, kNameArea.m_top), kNameArea);
				((LC_ClientPlaceHolder*)m_pkActor)->SetNameArea(kNameArea);
			}
		}
	}

	if ( abs( kDelta.x ) > 0.0001f || abs( kDelta.y ) > 0.0001f )
	{
		//m_pkFontBuffer->SetOffset(kDelta);

		if ( bUpdateAnnotatePos )
		{		
			m_kTitleFlashRect.Offset(kDelta.x,kDelta.y);
			m_kGuildTitleFlashRect.Offset(kDelta.x,kDelta.y);
			m_pkAnnotateBuffer->SetOffset( kDelta );

			if (bToyPet)
			{
				kInteractiveBtnArea.Offset(kDelta.x, kDelta.y);
				LC_ClientNPC* pkNPC = (LC_ClientNPC*)(CLIENT_GET_NPC_MANAGER()->FindNPC(m_lTargetID));
				if(pkNPC != NULL)
				{
					GUIRect& rect = pkNPC->GetInteractiveBtnArea();
					rect.Offset(kDelta.x,kDelta.y);
				}
			}
		}
		
		m_kScreenPos = renderpos;
		
		

		//kNameArea.Offset(GUISys::GUIPoint2(-kDelta.x, -kDelta.y));
		//((LC_ClientPlaceHolder*)m_pkActor)->SetNameArea(kNameArea);

		m_bNeedRedraw = true;
	}
	
	m_pkTitleFlashBuffer->Reset();

	int iParamGuildWar = iParamVec[UE_PlayerName::ATBIT_GUILD_WAR_TITLE_INFO];
	UE_RenderManager::GetSingleton().AddAnnotateEx( m_pkTitleFlashBuffer, 
		m_nTitleID,
		m_bShowOtherTitle, 
		iParamGuildWar,
		m_kTitleFlashRect,
		m_kGuildTitleFlashRect, this );



	//*********************************************//

	if(m_bNeedRedraw)
	{
		m_pkFontBuffer->Reset();

		if (IS_CHEST_ID(m_lTargetID))
		{
			UE_RenderManager::GetSingleton().AddNameInfoWithBG( m_pkFontBuffer,
				m_kCharName,m_kTitleName,m_kDebugName,
				m_kCharColor,m_kTitleColor,m_kDebugColor, 1.0f, m_nNameFrameState, kNameArea);
			((LC_ClientPlaceHolder*)m_pkActor)->SetNameArea(kNameArea);
		}
		else if (IS_NPC_ID(m_lTargetID))
		{
			UE_RenderManager::GetSingleton().AddNameInfo( m_pkFontBuffer,
				renderpos,m_kCharName,m_kTitleName,m_kDebugName,
				m_kCharColor,m_kTitleColor,m_kDebugColor, 1.0f, IS_GENERALIZED_PLAYER_ID(m_lTargetID),((LC_NPCBase*)m_pkActor)->GetDamageElementType());
		}
		else
		{
			if (!kCurrentGuildName.empty())
			{
				StringType guildName = "��" + kCurrentGuildName + "��" ;
				UE_RenderManager::GetSingleton().AddGuildInfo( m_pkFontBuffer,
					renderpos,guildName,m_kTitleName,m_kDebugName,
					m_kGuildColor,m_kTitleColor,m_kDebugColor, 1.0f, IS_GENERALIZED_PLAYER_ID(m_lTargetID),0);

				NiPoint3 kDrawPos = renderpos;
				kDrawPos.y -= 17.0f;
				UE_RenderManager::GetSingleton().AddNameInfo( m_pkFontBuffer,
					kDrawPos,m_kCharName,m_kTitleName,m_kDebugName,
					m_kCharColor,m_kTitleColor,m_kDebugColor, 1.0f, IS_GENERALIZED_PLAYER_ID(m_lTargetID),0,iTitleLevel);
			}
			else
			{
				UE_RenderManager::GetSingleton().AddNameInfo( m_pkFontBuffer,
					renderpos,m_kCharName,m_kTitleName,m_kDebugName,
					m_kCharColor,m_kTitleColor,m_kDebugColor, 1.0f, IS_GENERALIZED_PLAYER_ID(m_lTargetID),0,iTitleLevel);
			}
		
			
		}
		m_bNeedRedraw = false;
	}

//origin
	//if ( bShowName )
	//	UE_RenderManager::GetSingleton().SubmitPlayerNames( m_pkFontBuffer );
//new
	if ( bShowName )
	{
		if (m_pkPlayer == NULL || !m_pkPlayer->GetDead())
			UE_RenderManager::GetSingleton().SubmitPlayerNames( m_pkFontBuffer );
	}
//--new
	
	if ( m_pkPlayer == NULL || !m_pkPlayer->GetDead())
	{
		UE_RenderManager::GetSingleton().SubmitPlayerAnnotations( m_pkAnnotateBuffer );
		UE_RenderManager::GetSingleton().SubmitTitleFlash(m_pkTitleFlashBuffer);
	}
	return true;
}

int UE_PlayerName::GetFlashIndex(float fTime,int iCount,int iInterval)
{
	int iCurrentTime	= static_cast<int>( fTime * 1000.0f);

	if(m_iLastFlashPic < 0)
	{
		m_iLastFlashPicTime = iCurrentTime;
		m_iLastFlashPic = 0;
	}
	else
	{

		if(iCurrentTime - m_iLastFlashPicTime >= iInterval)
		{
			m_iLastFlashPicTime = iCurrentTime;
			m_iLastFlashPic = m_iLastFlashPic + 1;
		}
	}

	//Loop ����
	if(m_iLastFlashPic >= iCount)
	{
		if(iCount == 1)
			m_iLastFlashPic = m_iLastFlashPic % 2;
		else
			m_iLastFlashPic = 0;
	}
	return m_iLastFlashPic;
}


int UE_PlayerName::GetGuildFlashIndex(float fTime,int iCount,int iInterval)
{
	int iCurrentTime	= static_cast<int>( fTime * 1000.0f);

	if(m_iGuildLastFlashPic < 0)
	{
		m_iGuildLastFlashPicTime = iCurrentTime;
		m_iGuildLastFlashPic = 0;
	}
	else
	{

		if(iCurrentTime - m_iGuildLastFlashPicTime >= iInterval)
		{
			m_iGuildLastFlashPicTime = iCurrentTime;
			m_iGuildLastFlashPic = m_iGuildLastFlashPic + 1;
		}
	}

	//Loop ����
	if(m_iGuildLastFlashPic >= iCount)
	{
		if(iCount == 1)
			m_iGuildLastFlashPic = m_iGuildLastFlashPic % 2;
		else
			m_iGuildLastFlashPic = 0;
	}
	return m_iGuildLastFlashPic;
}

void UE_PlayerName::RedrawPlaceHolderNameRect(void)
{
	GUISys::GUIRect kNameRect;
	bool bIsFind = false;
	if(!m_pkActor)
	{
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
		pkUIEffectMgr->DeletePlaceHolderName(m_lTargetID);
		return ;
	}
	UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
	bIsFind = pkUIEffectMgr->GetPlaceHolderNameRect(m_lTargetID, kNameRect);
	if(!bIsFind)
		((LC_ClientPlaceHolder*)m_pkActor)->GetNameArea(kNameRect);
	else
	{
		((LC_ClientPlaceHolder*)m_pkActor)->SetNameArea(kNameRect);
	}
	if(kNameRect.m_left < 0 || kNameRect.m_top < 0)
		return;

	m_pkFontBuffer->Reset();
	UE_RenderManager::GetSingleton().AddNameInfoWithBG( m_pkFontBuffer,
		m_kCharName,m_kTitleName,m_kDebugName,
		m_kCharColor,m_kTitleColor,m_kDebugColor, 1.0f, m_nNameFrameState, kNameRect);
}
//------------------------------------------------------------------------
void UE_PlayerName::ChangePlayerTitle()
{
	if (m_pkActor == NULL)
		return; 

	//BUG #25231 ��B UG�����ɾͳƺš����鿴�Ѿ�ʹ�óƺŵ����ʱ���ƺ��������ʾ��Ӧ�ƺŵļ����֣�����Ҫ��ʾ�⼸����
	if (!IS_PLAYER_CLASS_ID(m_pkActor->GetID()))
	{
		m_kTitleName = m_pkActor->GetCharTitle();
	}
	else
	{
		m_kTitleName = "";
	}
	m_kTitleName = ((m_kTitleName == "NULL") ? "": m_kTitleName);
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(pkGamePlayer == NULL)
		return;

	LC_ClientTaskMap* pkTaskMap = (LC_ClientTaskMap*)pkGamePlayer->GetTaskMap();
	if (pkTaskMap&&pkTaskMap->IsConvoyNPC(m_lTargetID))
	{
		m_kTitleName = GET_MSG(UE_PLAYERNAME_CPP_001);
	}

	if(IS_PLAYER_CLASS_ID(m_lTargetID))
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)m_pkActor;
		/*m_nTitleID = pkPlayer->GetCurrentSystemTitleID();*/
		if (pkPlayer == pkGamePlayer)
			m_nTitleID = pkGamePlayer->GetCurrentSystemTitleID();
		else if (pkGamePlayer->IsBuddy(m_lTargetID))
			m_nTitleID = 0;
		else
		{
			pkPlayer = (LC_ClientNetPlayer*)m_pkActor;
			m_nTitleID = pkPlayer->IsMainNetPlayer()? pkPlayer->GetCommanderTitleID(): 0;
		}
	}
	m_bNeedRedraw = true;
}
//------------------------------------------------------------------------------
void UE_PlayerName::SetPendingParam( long Param1 , long Param2 , float fParam3 /*= 0.0f*/)
{
	m_lTargetID = Param1;
	m_nTitleID = 0;
	m_kGuildName.clear();
	m_kTitleName.clear();
	m_nParamVec.clear();
	Show(fParam3);

	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkGamePlayer)
		return;

	m_pkActor = (LC_ActorBase*)(pkWorldManager->FindObject(m_lTargetID));
	if(!m_pkActor)
	{
		GfxWriteLog( LOG_BACKLOADING_FILELINE, LOG_SWITCH_BACKLOADING ,  
			"Can't find actor [%d] for ui effect init, maybe not loaded yet!",	m_lTargetID);
		return;
	}

	bool bNetPlayer = false;
	if(IS_PLAYER_CLASS_ID(m_lTargetID))
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)m_pkActor;
		/*m_nTitleID = pkPlayer->GetCurrentSystemTitleID();*/
		if (pkPlayer == pkGamePlayer)
			m_nTitleID = pkGamePlayer->GetCurrentSystemTitleID();
		else if (pkGamePlayer->IsBuddy(m_lTargetID))
			m_nTitleID = 0;
		else
		{
			pkPlayer = (LC_ClientNetPlayer*)m_pkActor;
			m_nTitleID = pkPlayer->IsMainNetPlayer()? pkPlayer->GetCommanderTitleID(): 0;
			bNetPlayer = true;
		}
	}

	UT_Vec3Int kPosition;
	if (m_pkActor->GetDead())
	{
		GD_ClientActorModel* pkModel = IM_Manager::GetSingleton().GetActor(m_lTargetID);
		if(pkModel)
			kPosition = UT_EngineMath::ConvertNiPoint3ToVec3Int(pkModel->GetFxDummyNodePosition(GD_ModelBase::FDT_HEART));
		else
			kPosition = m_pkActor->GetVisibleLocation();
	}
	else
	{
			kPosition = m_pkActor->GetVisibleLocation();
			_findActorHeight();
			kPosition.z += m_lActorHeight;
	}	

	m_kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kPosition);

	NiPoint3 kScreenPos = UE_RenderManager::GetSingleton().ConvertToScreenPos(m_kPos);
	NiPoint3 renderpos;
	renderpos.x = ((kScreenPos.x + g_settings.UE_PLAYER_NAME_OFFSET.x)* 100) / 100.0f;
	renderpos.y = ((kScreenPos.y + g_settings.UE_PLAYER_NAME_OFFSET.y)* 100) / 100.0f;

	if (IS_CHEST_ID(m_lTargetID))
	{
		renderpos.x = ((kScreenPos.x + g_settings.UE_PLACEHOLDER_NAME_OFFSET.x)* 100) / 100.0f;
		renderpos.y = ((kScreenPos.y + g_settings.UE_PLACEHOLDER_NAME_OFFSET.y)* 100) / 100.0f;
	}

	if ( renderpos.x < 0 )
	{
		renderpos.x = 0;
	}
	if ( renderpos.y < 0 )
	{
		renderpos.y = 0;
	}

	m_kScreenPos = renderpos;

	GUIRect kNameArea;
	if (IS_CHEST_ID(m_lTargetID))
	{
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();		
		pkUIEffectMgr->InsertPlaceHolderNameRect(m_lTargetID, GUISys::GUIPoint2(renderpos.x, renderpos.y), kNameArea);
		((LC_ClientPlaceHolder*)m_pkActor)->SetNameArea(kNameArea);
	}

	//BUG #25231 ��B UG�����ɾͳƺš����鿴�Ѿ�ʹ�óƺŵ����ʱ���ƺ��������ʾ��Ӧ�ƺŵļ����֣�����Ҫ��ʾ�⼸����
	if (!IS_PLAYER_CLASS_ID(m_pkActor->GetID()))
	{
		m_kTitleName = m_pkActor->GetCharTitle();
	}
	else
	{
		m_kTitleName = "";
	}

	m_pkPlayer = pkLogicManager->FindPlayer(m_lTargetID);
	UT_Vec3Float kNameColor = _computeActorNameColor(pkGamePlayer, m_pkActor, m_pkPlayer);
	CalCharName(pkGamePlayer, pkWorldManager, pkLogicManager, kNameColor);

	//�Ƿ����״̬
//	CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
//	CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
	m_nAnnotationMask = 0;

	float		 fNameAlpha = 1.0f;

	m_kCharColor = NiColor(kNameColor.x,kNameColor.y,kNameColor.z);

	UT_Vec3Float kTitleColor = LC_Helper::ComputeActorTitleColor(pkGamePlayer, m_pkActor);

	m_kTitleColor = NiColor(kTitleColor.x,kTitleColor.y,kTitleColor.z);

	m_pkFontBuffer->Reset();
	m_pkAnnotateBuffer->Reset();
	m_pkTitleFlashBuffer->Reset();

	if (IS_CHEST_ID(m_lTargetID))
	{
		UE_RenderManager::GetSingleton().AddNameInfoWithBG( m_pkFontBuffer,
			m_kCharName,m_kTitleName,m_kDebugName,
			m_kCharColor,m_kTitleColor,m_kDebugColor, 1.0f, m_nNameFrameState, kNameArea);
		((LC_ClientPlaceHolder*)m_pkActor)->SetNameArea(kNameArea);
	}
	else if (IS_NPC_ID(m_lTargetID))
	{
		UE_RenderManager::GetSingleton().AddNameInfo( m_pkFontBuffer,
			renderpos,m_kCharName,m_kTitleName,m_kDebugName,
			m_kCharColor,m_kTitleColor,m_kDebugColor, 1.0f, IS_GENERALIZED_PLAYER_ID(m_lTargetID),((LC_NPCBase*)m_pkActor)->GetStarLevel()+1);
	}
	else
	{
		int iTitleLevel = 0;
		if (bNetPlayer)
		{
			LC_ClientNetPlayer* pkNetPlayer = (LC_ClientNetPlayer*)m_pkActor;
			if(pkNetPlayer->IsMainNetPlayer())
			{
				iTitleLevel = pkNetPlayer->GetCommanderTitleLevel();
			}
		}
		else
		{
			iTitleLevel = pkGamePlayer->GetCommanderTitleLevel();
		}

		UE_RenderManager::GetSingleton().AddNameInfo( m_pkFontBuffer,
			renderpos,m_kCharName,m_kTitleName,m_kDebugName,
			m_kCharColor,m_kTitleColor,m_kDebugColor, 1.0f, IS_GENERALIZED_PLAYER_ID(m_lTargetID),0, iTitleLevel);
	}

	if ( Param2 == 1)
	{
		m_fPlaceHolderCurLifeTime = -1;
		UE_UiEffectManager* pkUIEffectMgr = UE_UiEffectManager::GetSingletonPtr();
		pkUIEffectMgr->DeletePlaceHolderName(m_lTargetID);
	}
	m_bNeedRedraw = true;
}

void UE_PlayerName::ChangeNameFrameState(int iState)
{
	if (iState != m_nNameFrameState)
	{
		m_bNeedRedraw = true;
	}
	m_nNameFrameState = iState;
}

void UE_PlayerName::SetInteractiveBtnVisible(bool bVisible)
{
	if (bVisible != m_bShowInteractiveBtn)
	{
		m_bNeedRedraw = true;
	}
	m_bShowInteractiveBtn = bVisible;
}

UT_Vec3Float UE_PlayerName::_computeActorNameColor(GameLogic::LC_PlayerBase* pkWatcher, GameLogic::LC_ActorBase* pkTarget, GameLogic::LC_PlayerBase* pkPlayer, bool isInTeam)
{
	if(IS_CHEST_ID(m_lTargetID))
	{
		LC_ClientPlaceHolder* pkPlaceHolder = (LC_ClientPlaceHolder*)pkTarget;
		long lItemTypeID = pkPlaceHolder->GetItemTypeID();
		if (0 == lItemTypeID)
		{
			return UT_Vec3Float(1.0f,1.0f,1.0f);
		}
		else
		{
			CSVFile::CF_ItemList::DataEntry* pkData =  CSVFile::CF_ItemList::GetSingleton().GetEntryPtr(lItemTypeID);
			if(!pkData)
			{
				GfxWriteLog(LOG_ERROR_FILELINE,LOG_SWITCH_ERROR,"�Ҳ���ItemList[%i]CSV����",lItemTypeID);
				return UT_Vec3Float(0.98f,1.0f,0.62f);
			}
			else
			{
				if(IS_ITEM_ID(lItemTypeID))
				{
					return Int2RGB(GetGradeColor(pkData->_lGrade));
				}
				return UT_Vec3Float(0.98f,1.0f,0.62f);
			}
		}
	}		
	else
	{
		LC_ClientGamePlayer* pkGamePlayer = (LC_ClientGamePlayer*)pkWatcher;
		int iPowerGuildID = pkGamePlayer->GetPowerGuildID();
		if (pkGamePlayer->GetMapLogicType() != MT_GUILDWAR && pkGamePlayer->GetGuildWarType() != WarType_DomainWar)
			iPowerGuildID = -1;
		if (IS_PLAYER_CLASS_ID(pkTarget->GetID()))
		{
			return ComputeActorNameColor(pkGamePlayer,pkTarget, iPowerGuildID, pkPlayer, isInTeam);
		}
		else
			return LC_Helper::ComputeActorNameColor(pkWatcher, pkTarget, iPowerGuildID);
	}
}

UT_Vec3Float UE_PlayerName::ComputeActorNameColor(LC_ClientGamePlayer* pkWatcher, LC_ActorBase* pkTarget, int iPowerGuildID, LC_PlayerBase* pkPlayer, bool isInTeam)
{
	//�۲��߱���Ϊplayer ��������ɫ����������GPNP��ID����仯
	if (NULL == pkWatcher || NULL == pkTarget || !IS_PLAYER_CLASS_ID(pkWatcher->GetMainControllerID()))
	{
		return UT_Vec3Float(0.98f, 1.0f, 0.62f);  // 
	}

	if (iPowerGuildID == -1 )
	{
		UT_Vec3Float col(0.035f, 0.83f, 0.28f);
		// ��Ϊ�л���������Ҫ��buddy��id����������commander����Ϊcommander�ǲ����
		if( pkWatcher->GetID() != pkTarget->GetID() ) 
		{
			
			LC_ClientNetPlayer* pkPlayerTarget = (LC_ClientNetPlayer*)pkTarget;
			if (pkPlayer == NULL )
			{
				return col;
			}
			//Buddy����ɫ������һ��  ����buddyID�仯����������id������Ϊ���ֵı�׼
			if (!pkPlayerTarget->IsMainNetPlayer()) // Buddy
			{
				object_id_type id = pkPlayerTarget->GetOwnerObjectID();
				object_id_type id6 = pkWatcher->GetMainControllerID();
				col = Int2RGB(0xFFFFFFFF); 
				if (id == id6)  // ���ҵ�buddy
				{
					col = Int2RGB(0xffadff2f);
					return col;
				}
				else  // ���˵�buddy
				{
					if (isInTeam)
					{
						return Int2RGB(0xff8a2be2); //ͬ���������ʾ��ɫ
					}
					switch ( pkWatcher->GetPKRecorder()->GetPKMode() )
					{
					case PKM_ALL:
					case PKM_SERVER:
					case PKM_SOCIETY:
						{

							CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
							CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
							ResultType ret = LC_Helper::IsValidTarget(pkWatcher, pkTarget->GetID(), pkData);
							if ( RE_SKILL_CHECK_VALID_TARGET == ret ) 
							{
								col = Int2RGB(0xFFFFA300);	//��ɫ
							}
							else
							{
								col = Int2RGB(0xFFFFFFFF);	//��ɫ
							}
						}
						break;
					case PKM_PEACE:
					case PKM_GOODEVIL:
						{
							switch (pkPlayerTarget->GetPKRecorder()->GetPKStateType())
							{
							case PST_GRAY:
							case PST_RED:
								{
									col = Int2RGB(0xffff0000);
								}
								break;
							case PST_YELLOW:
								{
									col = Int2RGB(0xffffff00);
								}
								break;
							default:
								{
									col = Int2RGB(0xFFFFFFFF);	// �Ȱ�ͼ�޸�Ϊ��ɫ��
								}
								break;
							}
						}
						break;
					default:
						break;
					}
				}
			}
			else   // net����
			{
				if (isInTeam)
				{
					return Int2RGB(0xff8a2be2); //ͬ���������ʾ��ɫ
				}
				switch ( pkWatcher->GetPKRecorder()->GetPKMode() )
				{
				case PKM_ALL:
				case PKM_SERVER:
				case PKM_SOCIETY:
					{

						CF_ActorFilterFunc* pkCSVFilterFunc = CF_ActorFilterFunc::GetSingletonPtr();
						CF_ActorFilterFunc::DataEntry* pkData = pkCSVFilterFunc->GetEntryPtr(2);
						ResultType ret = LC_Helper::IsValidTarget(pkWatcher, pkTarget->GetID(), pkData);
						if ( RE_SKILL_CHECK_VALID_TARGET == ret ) 
						{
							col = Int2RGB(0xFFFFA300);	//��ɫ
						}
						else
						{
							col = Int2RGB(0xFFFFFFFF);	//��ɫ
						}
					}
					break;
				case PKM_PEACE:
				case PKM_GOODEVIL:
					{
						switch (pkPlayerTarget->GetPKRecorder()->GetPKStateType())
						{
						case PST_GRAY:
							{
								col = Int2RGB(0xff9d9d9d);
							}
							break;
						case PST_YELLOW:
							{
								col = Int2RGB(0xffffff00);
							}
							break;
						case PST_RED:
							{
								col = Int2RGB(0xffdc143c);  //��ɫ
							}
							break;
						default:
							{
								col = Int2RGB(0xFFFFFFFF);	//�Ȱ�ͼ�޸�Ϊ��ɫ��
							}
							break;
						}
					}
					break;
				default:
					break;
				}
			}
		}
		else // �Լ�
		{
			col = Int2RGB(0xffadff2f);
		}

		return col;
	}
	else
	{
		LC_PlayerBase* pkPlayer = (LC_PlayerBase*)pkTarget;
		guild_id_type iGuildID = pkPlayer->GetGuildID();
		guild_id_type iWatcherGuildID = pkWatcher->GetGuildID();
		if (pkPlayer->GetID() == pkWatcher->GetID())
		{
			return Int2RGB(0xFF5788FE);	//��ɫ
		}
		//�ط�
		if (iWatcherGuildID == iPowerGuildID && iPowerGuildID != INVALID_GUILD_ID)
		{
			if (iWatcherGuildID == iGuildID)
			{
				return Int2RGB(0xFF5788FE);	//��ɫ
			}
			else
			{
				return Int2RGB(0xFFDE1F00);	//��ɫ
			}
		}
		//����
		else if (iGuildID == iPowerGuildID && iPowerGuildID != INVALID_GUILD_ID)
		{
			return Int2RGB(0xFFDE1F00);	//��ɫ
		}
		else if (iWatcherGuildID == iGuildID && iWatcherGuildID != INVALID_GUILD_ID)
		{
			return Int2RGB(0xFF5788FE);	//��ɫ
		}
		else
		{
			return  Int2RGB(0xFFFFA300);	//��ɫ
		}
	}

	return UT_Vec3Float(1.0f, 1.0f, 1.0f);

}



void UE_PlayerName::RequestRedraw()
{
	m_bNeedRedraw = true;
	m_pkFontBuffer->Reset();
}

bool UE_PlayerName::GetLocalNameRect(long lTargetID, GUISys::GUIRect& kRetNameRect)
{
	LC_ClientWorldManager* pkWorldManager = CLIENT_GET_WORLD_MANAGER();
	GameLogic::LC_ActorBase* pkActor = (LC_ActorBase*)(pkWorldManager->FindObject(lTargetID));
	if(!pkActor)
		return false;
	LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
	LC_ClientGamePlayer* pkGamePlayer = pkLogicManager->GetOperationPlayer();
	if(NULL == pkGamePlayer)
		return false;
	
	UT_Vec3Int kPosition;
	if (pkActor->GetDead())
	{
		GD_ClientActorModel* pkModel = IM_Manager::GetSingleton().GetActor(lTargetID);
		if(pkModel)
			kPosition = UT_EngineMath::ConvertNiPoint3ToVec3Int(pkModel->GetFxDummyNodePosition(GD_ModelBase::FDT_HEART));
		else
			kPosition = pkActor->GetVisibleLocation();
	}
	else
	{
		kPosition = pkActor->GetVisibleLocation();
	}	

	_findActorHeight();
	kPosition.z += m_lActorHeight;
	NiPoint3 kPos = UT_EngineMath::ConvertVec3IntToNiPoint3(kPosition);

	NiPoint3 kScreenPos = UE_RenderManager::GetSingleton().ConvertToScreenPos(kPos);
	NiPoint3 renderpos;
	renderpos.x = ((kScreenPos.x + g_settings.UE_PLAYER_NAME_OFFSET.x)* 100) / 100.0f;
	renderpos.y = ((kScreenPos.y + g_settings.UE_PLAYER_NAME_OFFSET.y)* 100) / 100.0f;

	if (IS_CHEST_ID(m_lTargetID))
	{
		renderpos.x = ((kScreenPos.x + g_settings.UE_PLACEHOLDER_NAME_OFFSET.x)* 100) / 100.0f;
		renderpos.y = ((kScreenPos.y + g_settings.UE_PLACEHOLDER_NAME_OFFSET.y)* 100) / 100.0f;
	}

	if ( renderpos.x < 0 )
	{
		renderpos.x = 0;
	}
	if ( renderpos.y < 0 )
	{
		renderpos.y = 0;
	}

	kRetNameRect.SetPosition(renderpos.x, renderpos.y);
	kRetNameRect.SetWidth(g_settings.UE_NAME_BG_WIDTH);
	kRetNameRect.SetHeight(20);

	return true;
}

void UE_PlayerName::SetNameRect(const GUISys::GUIRect& kNameRect)
{
	if(IS_CHEST_ID(m_lTargetID) && m_pkActor)
		((LC_ClientPlaceHolder*)m_pkActor)->SetNameArea(kNameRect);
}

UT_Vec3Int UE_PlayerName::GetScreentPos()
{
	return UT_Vec3Int(m_kScreenPos.x, m_kScreenPos.y + m_kNameTop ,0);
}

void UE_PlayerName::CalCharName( LC_ClientGamePlayer* pkGamePlayer, LC_ClientWorldManager* pkWorldManager, LC_ClientLogicManager* pkLogicManager, UT_Vec3Float& kNameColor )
{
	if ( pkGamePlayer == NULL ||  pkWorldManager == NULL || pkLogicManager == NULL)
	{
		return ;
	}

	m_pkActor = (LC_ActorBase*)(pkWorldManager->FindObject(m_lTargetID));
	if ( m_pkActor == NULL )
		return ;

	m_kCharName = m_pkActor->GetCharName();
	if( IS_CHEST_ID(m_lTargetID) )
	{	
		LC_ClientPlaceHolder* phb = (LC_ClientPlaceHolder*)m_pkActor;
		CF_ItemList::DataEntry* pk = CF_ItemList::GetSingletonPtr()->GetEntryPtr(phb->GetItemTypeID());

		//��Ԫ�ڻ��������Ʒ����ʾ���ҡ�
		if ( phb->IsOwner(pkGamePlayer->GetMainControllerID()) && pk && pk->_lGrade >= ITEM_GRADE_GOLDEN && 10000614 != phb->GetItemTypeID())
			m_kCharName = m_kCharName + GET_MSG(UE_PLAYERNAME_CPP_007);
	}

	// ���ɲ���
	if ( m_pkPlayer )
	{
		m_kGuildName = m_pkPlayer->GetGuildName();
		if (m_kGuildName.size() > 0)
		{
			char szTemp[256] = {0};
			sprintf_s(szTemp, sizeof(szTemp), "[%s]%s", m_kGuildName.c_str(), m_kCharName.c_str() );
			m_kCharName = szTemp;
		}
	}

	m_kCharName = ((m_kCharName == "NULL") ? "": m_kCharName);
	m_kTitleName = ((m_kTitleName == "NULL") ? "": m_kTitleName);	

	//--------------------------------��ʱ----------------------------------------//
	LC_ClientTaskMap* pkTaskMap = (LC_ClientTaskMap*)pkGamePlayer->GetTaskMap();
	if (pkTaskMap&&pkTaskMap->IsConvoyNPC(m_lTargetID))
	{
		m_kTitleName = GET_MSG(UE_PLAYERNAME_CPP_001);
	}

	//--------------------------------��ʱ----------------------------------------//

	//--------------------------------��ʱ1----------------------------------------//
	if ( m_pkActor->GetFollowedTargetID() > 0 && IS_LOCAL_NPC_ID(m_pkActor->GetID()) )
	{
		LC_ClientLogicManager* pkLogicManager = CLIENT_GET_LOGIC_MANAGER();
		GameLogic::LC_PlayerBase* pkSummonOwnerPlayer = pkLogicManager->FindPlayer(m_pkActor->GetFollowedTargetID());
		if ( pkSummonOwnerPlayer )
			m_kCharName = pkSummonOwnerPlayer->GetCommanderName() + GET_MSG(LC_CLIENTNPCMANAGER_CPP_001) + m_kCharName;
	}
	//--------------------------------��ʱ1----------------------------------------//
	
}

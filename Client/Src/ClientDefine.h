
#ifndef _CLIENT_DEFINE_H
#define _CLIENT_DEFINE_H 

#include "LM_LocaleManager.h"

typedef int32_t	time_type;


enum VideoType
{
	VIDEOTYPE_GAME_INTRO	= 0,		//��Ϸ������Ƶ
	VIDEOTYPE_CHAR_DEMO		= 1,		//������ɫ������ʾ
	VIDEOTYPE_SKILL			= 2,		//������ʾ
	VIDEOTYPE_SKILL_TOOLTIP	= 3,		//����Tips��ʾ

	VIDEOTYPE_COUNT,
};

//YES/NO�Ի�����Ҫ���߼�����
enum MessageDialogType
{
	MDT_INVALID						= 0,
	MDT_LEAVE_GAME_NOW				= 1,		//�����˳���Ϸ�� Parm1 = ����λ��
	MDT_CONFIRM_CHAR_OPERATION		= 2,		//ȷ�Ͻ�ɫ���� Param1=�������ͣ�Parame2=��ɫID
	MDT_CONFIRM_INVITE_TO_GROUP		= 3,		//ȷ��������ӡ�Param1 = ����Ŀ��ID Param2 = ����ID Param3 = ������Ŀ��ID
	MDT_CONFIRM_SKILL_RESPAWN		= 4,		//ȷ�ϱ����Param1 = ������ID	
	MDT_CONFIRM_FORGET_PRODUCTION	= 5,		//ȷ��������������	Param1 = ��������
	//MDT_CONFIRM_COMPOSITION			= 10,		//ȷ�Ϻϳ�����Ʒ
	MDT_CONFIRM_BEADDFRIEND			= 11,		//ȷ�ϱ���Ϊ����
	MDT_CONFIRM_USE_UNBINDCRASH_REDUCEPK =14, //�÷ǰ󶨽����pkֵ
	MDT_DIALOG_CONFIRM_TRANSFER_MAP	= 15,		//��ͼ����ȷ�Ͽ�
	MDT_DIALOG_CONFIRM_LOW_SCORE	= 16,		//��λ����ս��������ʾ
	MDT_DIALOG_CONFIRM_RECHARGE		= 17		//��ʾ��ֵ
};


//��¼��������UI�����ַ����õķָ�������ڷָ���Ϣ
#define LOGIN_UI_STRING_MARKER "+"
//��������ͼID
#define MAPID_ARENA 2039
//��������ս��ͼid
#define MAPID_ARENAPLAYBACK 2041
#define MAPID_SKYARENA 2042
//ָ�ӹ�ͷ��uiopenlevel��key
#define COMMANDERTITLE_UIOPENLEVEL_KEY 900043
//��һ���������
#define HOLIDAYACTIVITY_TYPE_WUYI 1

//ʹ�ÿ�ݼ���һ����Ⱥ�����л�Ŀ��
enum ShortCutTargetType
{
	SCTT_ENEMY		= 1,//�ڹ���͵ж�������л�Ŀ��
	SCTT_FRIEND		= 2,//���ҷ����л�Ŀ��
};

//��ɫ�������
enum CharManageActionType
{
	CMAT_DELETE_CHAR = 1,//ɾ����ɫ
	CMAT_RECOVER_CHAR= 2,//�ָ���ɫ
};

#define SHORT_CUT_ENEMY_COUNT 5		//ʹ�ÿ�ݼ���һ�������ĵж�Ŀ���н����л�
#define SHORT_CUT_FRIEND_COUNT 5	//ʹ�ÿ�ݼ���һ���������ҷ�Ŀ���н����л�

//����UI������ʱ�Ҽ�����
enum BagUiRClickFuncType
{
	BAGUI_RCLICK_NONE			= 0, 	//�޹���
	BAGUI_RCLICK_DRAG			= 1, 	//ճ����Ʒ
	BAGUI_RCLICK_USE			= 2, 	//ֱ��ʹ��
	BAGUI_RCLICK_ACTIVE_OP		= 3, 	//�������
	BAGUI_RCLICK_EQUIP			= 4, 	//�ƶ���װ��UI(װ����Ʒ)
	BAGUI_RCLICK_SHOP			= 5, 	//�ƶ����̵�UI
	BAGUI_RCLICK_SHORTCUT		= 6, 	//�ƶ��������UI
	BAGUI_RCLICK_TRANSFER		= 7, 	//����UI
	BAGUI_RCLICK_ACCEPTTASK		= 8, 	//��������UI
	BAGUI_RCLICK_RESET_TALENT	= 9, 	//�����츳
	BAGUI_RCLICK_IDENTIFY_WORD	= 10,	//��������
	BAGUI_RCLICK_MOUNTS			= 11,	//�������
	BAGUI_RCLICK_UI_INTERACTION	= 12,	//UI��������
	BAGUI_RCLICK_UI_OPENUI		= 13,	//�򿪶�ӦUI
};

#define TOOLTIP_WINDOW_WIDTH 250.0		//Tooltip���ڿ��

enum GreatKungFuAttribute
{
	GREATKUNGFU_HP				= 0,
	GREATKUNGFU_HP_RECOVER		= 1,
	GREATKUNGFU_ATTACK			= 2,
	GREATKUNGFU_DEFENCE			= 3,
	GREATKUNGFU_DOGGE_LEVEL		= 4,
	GREATKUNGFU_CRI_RATE		= 5,
	GREATKUNGFU_SCORE			= 6,
	GREATKUNGFU_SIGNINNUMBER	= 7,
	GREATKUNGFU_TIMEDOWN		= 8,
	GREATKUNGFU_SKILLID			= 9,
};

enum OpenLevelArrtibute
{
	OPENLEVEL_HP					= 0,
	OPENLEVEL_HP_RECOVER			= 1,
	OPENLEVEL_ATTACK				= 2,
	OPENLEVEL_DEFENCE				= 3,
	OPENLEVEL_CRI_RATE				= 4,
	OPENLEVEL_DOGGE_LEVEL			= 5,
	OPENLEVEL_ATK_RATE				= 6,
	OPENLEVEL_CRI_DEF				= 7,
};

enum CharTypeShowFlag	//��ʾ��־λ����
{
	CTSF_BLOOD		= 0x0001,
	CTSF_NAME		= 0x0002,
	CTSF_SELECTFX	= 0x0004,
};

enum UICutScenesType
{
	CUCS_TYPE_None		= 0,
	CUCS_TYPE_Raid		= 1,
	CUCS_TYPE_Task		= 2,
	CUCS_TYPE_BOSS		= 3		//boss����
};
enum RaidState	//ö��ֵ�ͱ������Ӧ����Ҫ����޸ġ�
{
	CUCS_RAID_Enter			= 0,
	CUCS_RAID_BeforeQuit	= 1,
};
enum TaskState
{
	CUCS_TASK_Accept		= 0,	//����ȡ�����ʱ��
	CUCS_TASK_Processing	= 1,	//�����������
	CUCS_TASK_Finish		= 2,	//����������ʱ��
	CUCS_TASK_Finished		= 3,	//����������ɹ�
	CUCS_TASK_UnFinished	= 4,	//�����δ��ɹ�
};
enum BossState
{
	CUCS_BOSS_Appear		= 0,	//Boss����
	CUCS_BOSS_Die			= 1,	//Boss����
};

//������װ����
enum SpecialKungfu
{
	SKF_GUILDWAR			= 1,
	SKF_CROSSGUILDWAR		= 2,
};

//������װ��λ����
enum SpecialKungfuDress
{
	SKFD_HEAD			= 0,
	SKFD_BODY			= 1,
	SKFD_FOOT			= 2,
};

void SetupMainWindow(HWND hWnd);
void GetDesktopCenterParamter(int& nWidth, int& nHeight, int& nOffsetX, int& nOffsetY);

#define		PLAYER_JUMP_END_ANIM_BEGIN_HEIGHT	150	//��ʼ����Ծ��������ʱ�ĸ߶�

#define		SHORTCUT_INDEX_ITEM 5	//�������ŵ���(ҩˮ)�ĸ�������

#define		DAYLY_TASK_OPEN_LEVEL	21 //�ճ��������ȼ�

#define		DAILY_RECHARGE_REWARD_NUMBER	1//ÿ���׳影������

#define		PROMOTER_REMAIN_LEVEL	15 //�ƹ�Աϵͳ���ѵȼ�

//������ڣ������Զ����׼��
static float AUTO_UPGRADE_INTERVAL = 0.2f;

#define		TRANSFER_LOADING_DISTANCE	3000	//������������loading������ֱ���л�λ�ã�ʵ�ʸ����μ��ط�Χ�й�
#define		VEHICLE_PATH_TO_DISTANCE	2500	//�������Ѱ·���������
#define		DIRECTION_SHOW_DISTANCE		1000
#define		DIRECTION_SHOW_DISTANCE_GAMESTORY	1600
#define		AUTO_AVENUE_PATH_DISTANCE	1800

#define		OPEN_ABILITY_MAX_VIEW		7		//�����������������ʾ����

#define		UpgradeSuccessState	32200001 //���ڣ����ۡ�������׳ɹ���þ����õ�State

//CF_WorldObjectDesc _Type
#define LINK_TYPE_NPC_LOCATION		 1	//NPC����
#define LINK_TYPE_ITEM_LOCATION		 2	//��Ʒ����
#define LINK_TYPE_MONSTER_LOCATION	 3	//������
#define LINK_TYPE_COLLECT_LOCATION	 4	//�ɼ�����
#define LINK_TYPE_ITEM_TIP			 5	//��Ʒtips
#define LINK_TYPE_SKILL_TIP			 6	//����tips
#define LINK_TYPE_ITEM_DETAILED_TIP	 7	//��Ʒ��ϸtips
#define LINK_TYPE_OPEN_UI			 8	//��UI
#define LINK_TYPE_SIMPLEITEM_TIP	 9	//����Ʒtips
#define LINK_TYPE_HOLYFLAM			 10	//ʥ����tips
#define LINK_TYPE_TRAVEL_BOOTS		 11	//��Ь
#define LINK_TYPE_ACHIEVEMENT		 12	//�ɾ�
#define LINK_TYPE_MAP_TRANSFER		 13	//ֱ�Ӵ��͵�Ŀ���ͼ
#define LINK_TYPE_PLAYER_LOCATION	 14	//�������
#define LINK_TYPE_QUIT_TRANSCRIPTION		 15	//�˳�����Transcription
#define LINK_TYPE_TRAVEL_BOOTS_CODINATE		 16	//��Ь(����)
#define LINK_TYPE_TRAVEL_DAILYRECHARGE		 17 //ÿ���׳�
#define LINK_TYPE_TRAVEL_DIVINE				 18 //ռ��
#define LINK_TYPE_GOLDEN_HOOK				 19 //�ƽ�һ���
#define LINK_TYPE_GAMESTORY					20
#define LINK_TYPE_MAP_TRANSFER_HOOK			21	//ֱ�Ӵ��ͣ�ս��
#define LINK_TYPE_PARALLEL_SPACE			25	//��λ����

enum LinkParmType
{
	LINK_PARAM_TRAVEL_BOOTS			= 1,//��Ь0x1
	LINK_PARAM_TRANSFER_NPC			= 4,//�ʹ�����NPC�Ի�0x4
	LINK_PARAM_MAIN_MAP				= 8,//Ĭ��Ѱ·���߷�Ь�ɵ�������
	LINK_PARAM_MAP_POSITION			= 16,//Ĭ�ϻ�õ�KEYNAMEΪ��ͼ���ֺ�λ��
	LINK_PARAM_NO_GAIN_JUDGE		= 32,//�Ƿ���Ҫ�ж�������
};

//��ɫѰ·��Ŀ����Ϊ������WorldObjectDesc.csv
enum	PathPendingActionType
{
	PPAT_NA = 0,
	PPAT_TALK_TO_NPC,	//��̸
	PPAT_PICK_ITEM,		//ʰȡ��Ʒ
	PPAT_ATTACK_NPC,	//���
	PPAT_COLLECT_ITEM,	//�ɼ���Ʒ
	PPAT_TRANSFER,	//����
};

///ǩ����������AVATAR����
enum	SignInRewardAvatarType
{
	SIRAT_SKILL					= 0, //����
	SIRAT_VEHICLE				= 1, //����
	SIRAT_ARROW					= 2, //����
	SIRAT_DEVA_EYE				= 3, //����
	SIRAT_VEHICLE_FIGHTING		= 4, //��ս
	SIRAT_MUHUN					= 5, //���
	SIRAT_MENTAL				= 6, //�ķ�
	SIRAT_TREASURE				= 7, //�ر�
	SIRAT_BOSSHALL				= 8, //�������
	SIRAT_SPIRITPET				= 9, //���
	SIRAT_SPIRITWEAPON			= 10, //����
	SIRAT_TALISMAN				= 11, //����
	SIRAT_HIDDENWEAPON			= 12, //����
	SIRAT_GODWEAPON				= 13, //���
	SIRAT_PLOUGH				= 14, //���
	SIRAT_FAN					= 15, //����
	SIRAT_SUPERBOW				= 16, //��
	SIRAT_HOOK					= 17, //�乳
	SIRAT_SPIRITFAN				= 18, //����
	SIRAT_BUDDY					= 19, //��ɫ
	SIRAT_STIGMAS				= 20, //ʥ��
	SIRAT_TOY						= 21,//���
	SIRAT_END,	
};

///��������ս������ȡ
enum DomainRewardType
{
	DOMAIN_REWARD_TYPE_CANT		= 1, //������
	DOMAIN_REWARD_TYPE_DRAWED	= 2, //����ȡ
	DOMAIN_REWARD_TYPE_HAVE		= 3, //������
};

const char* SUB_TP_NAME(int index);

static int SUB_TP_CHARID[] = {1001, 2001, 3001, 6001, 0, 0, 0, 0, 5001};

//lua��ȡ��������
enum DataStringType
{
	DataStringType_GuildRaid_Begin				= 1,		//����boss ��ʼ
	DataStringType_GuildRaid_BOSS_Detail		= 2,		//boss����
	DataStringType_GuildRaid_Info				= 3,		//��ǰ��ͼ������Ϣ
	DataStringType_GuildRaid_Reward				= 4,		//�����б�
	DataStringType_GuildRaid_Boss_TIP			= 5,		//boss tooltips
	DataStringType_GuildRaid_Boss_State_List	= 6,		//boss״̬�б�
	DataStringType_GuildRaid_Boss_CharTypeList	= 7,		//boss�б�
	DataStringType_GuildRaid_Boss_Progress		= 8,		//��õ�ǰ����
	DataStringType_GuildRaid_Boss_Title			= 9,		//title
	DataStringType_GuildRaid_Boss_EndTime		= 10,		//����ʱ��
	DataStringType_GuildRaid_End				= 20,		//����boss ����

	DataStringType_WildPrinces_Begin			= 21,		//Ұ��boss ��ʼ
	DataStringType_WildPrinces_BossList			= 22,		//Ұ��boss�
	DataStringType_WildPrinces_Boss_Info		= 23,		//Ұ��boss��ϸ��Ϣ
	DataStringType_WildPrinces_MapStateList		= 24,		//��ͼ״̬
	DataStringType_WildPrinces_Boss_Pos			= 25,		//bossλ��
	DataStringType_WildPrinces_TimeStamp		= 26,		//�´�ˢ��ʱ��
	DataStringType_WildPrinces_End				= 30,		//Ұ��boss ����

	DataStringType_COURAGE_SHOP_LIST  			= 32,		//�����һ��̵�

	DataStringType_UIOpenLevel					= 35,		//�״��ͼ��ఴť������Ϣ
	DataStringType_TrialShopList				= 36,		//��������
	DataStringType_TaskListRewardList			= 37,		//������
	DataStringType_PurchaseBackReward			= 38,		//���ؽ���

	DataStringType_Achievement_Begin			= 40,		//�ɾ�ϵͳ ��ʼ
	DataStringType_Achievement_TitleList		= 50,		//�ƺ��б�
	DataStringType_Achievement_TitleCount		= 51,		//�б�ƺ�����
	DataStringType_Achievement_CurrentTitle		= 52,		//��ǰ�ƺ�
	DataStringType_Achievement_TitleStateList	= 53,		//�ƺ�״̬
	DataStringType_Achievement_TitleTips		= 54,		//�ƺ�tips
	DataStringType_Achievement_End				= 60,		//�ɾ�ϵͳ ����
	DataStringType_CommonStatRewardList_Confirm	= 61,		//CommonStat������ʾ��ȷ������
	DataStringType_CommonStatRewardList_Random	= 62,		//CommonStat������ʾ���������
	
	DataStringType_OpenLevel_Detail				= 64,		//OPENLEVEL����һЩϸ��
	DataStringType_SlotMachineShop_ItemList		= 65,		//�ϻ����̵���Ʒ�б�
	DataStringType_ShotActivity_Begin			= 70,		//�����ʼ
	DataStringType_ShotActivity_EntryInfo		= 71,		//����ӻ��Ϣ
	DataStringType_ShotActivity_RankTypeID		= 72,		//�����ID��Ϣ
	DataStringType_ShotActivity_BaseEntryInfo	= 73,		//����ӻ������Ϣ
	DataStringType_ShotActivity_MyRank			= 74,		//������ҵ�����
	DataStringType_ShotActivity_DirtyFlagRv		= 75,		//�����Revision
	DataStringType_ShotActivity_RankInfoByIndex	= 76,		//��������а���Ϣ
	DataStringType_ShotActivity_SetBeginIndex	= 77,		//���ò�ѯBEGININDEX
	DataStringType_ShotActivity_GetTotalSize	= 78,		//������а�SIZE��Ϣ
	DataStringType_ShotActivity_GetValidDays	= 79,		//��û��Чʱ��
	DataStringType_ShotActivity_DecreaseCount	= 80,		//�����������
	DataStringType_CommonStatRewardList_MainTask= 81,		//�ͽ�Boss��������ʾ
	DataStringType_FunctionPreview				= 82,		//����Ԥ��
	DataStringType_ShopList_EntryInfo			= 83,		

	DataStringType_GameStoryRaidList_BossInfo	= 84,		//���̽���Boss��Ϣ

	DataStringType_WorldMapList_LimitInfo		= 85,		//��ͼ�������������Ϣ
	
	DataStringType_SendFlower_PanelInfo			= 87,		//�ʻ������Ϣ
	DataStringType_PopularReward_Info			= 88,

	DataStringType_DisassembleUse_Info			= 90,		//�ɲ�����������Ϣ
	DataStringType_OffLineExp_Info				= 91,		//���߾���������Ϣ
	DataStringType_NewBeeMap_Info				= 92,		//��ͼ���������Ϣ��ȡ

	DataStringType_DiscountShopList				= 97,		//�ۿ��̵��б�

	DataStringType_ExchangeShop					= 99,		//��װ�һ��̵�
	DataStringType_Subordinage_Profit			= 100,		//���׷���

	DataStringType_LimitGiftBag					= 101,		//�������
	
	DataStringType_CharAttribute				= 111,		//ͨ�����Լӳ�
	
	DataStringType_SubordinateSpecialAttribInfo	= 116,
	DataStringType_SubordinateExpireForbid		= 117,
	DataStringType_MulSelectGift_Info			= 118,		//�࿪�����Ϣ

	DataStringType_RaidExpInfo                  = 133,      //���鸱�������Ϣ
	DataStringType_RaidSingleSole               = 134,      //���׸��������Ϣ
	DataStringType_RailyActivity                = 135,      //��Ծ�������Ϣ
	DataStringType_RailySignReward              = 136,      //ÿ��ǩ�������Ϣ
	DataStringType_RailyCharge	                = 137,      //�׳����������Ϣ
	DataStringType_BuddyUpgradCost				= 138,      //��ɫ�ɳ������Ϣ
	DataStringType_BuddyUpgradData				= 139,      //��ɫ�ɳ������Ϣ
	
	DataStringType_OffLineRwdRatio_Info			= 141,		//���߽���������Ϣ
	DataStringType_Collection					= 142,		//�ղ������Ϣ
	DataStringType_BossFirstKill				= 143,		//��ɱboss�����Ϣ
	DataStringType_EndlessTower					= 144,		//�޾�֮�������Ϣ
	DataStringType_Biography					= 145,		//���������Ϣ
	DataStringType_BuddyBiography				= 146,		//С�������Ϣ
	DataStringType_Holydeed                     = 147,      //ʥ�������Ϣ
	DataStringType_SevenDaysExperiment			= 148,		//�������������Ϣ
	DataStringType_EquipUpgrade					= 149,      //װ�����������Ϣ
	DataStringType_BattlePassCard				= 150,		//��ѡ�񻶼������Ϣ
	DataStringType_BattlePassCardRewardShow		= 151,      //��ѡ�񻶼���һ�������Ϣ
	DataStringType_BossInvade					= 152,		//������Ϯ�����Ϣ
	DataStringType_BossInvadeRankReward			= 153,		//������Ϯ�������������Ϣ
	DataStringType_VipNew						= 154,		//��VIP�����Ϣ
	DataStringType_VipNewTask					= 155,		//��VIP������Ϣ
	DataStringType_RaidBossSpeciesStoryID		= 156,		//������Ϯ�id
	DataStringType_RecommendHeroByID			= 157,		//�Ƽ�Ӣ����Ϣ
	
	DataStringType_ACTShopAAvatarInfo			= 160,		//ActShopA�� ģ������
	DataStringType_RaidBossPersonal				= 161,		//�������������Ϣ
	DataStringType_AnswerActivityPointReward    = 162,      //�������ֽ��������Ϣ
	DataStringType_AnswerActivityRankRewards	= 163,		//�����������������Ϣ
	DataStringType_CommanderTitleRewards		= 164,		//ָ�ӹ�ͷ�ν��������Ϣ
	DataStringType_SpecialTrainingRewardInfo	= 165,		//��ѵ���������Ϣ
	DataStringType_CharTypePosition				= 166,		//Chartype��ͷλ�������Ϣ
	DataStringType_PersonalHeadSetting			= 167,		//ͷ��������Ϣ
	DataStringType_GloryExpeditionReward		= 168,		//�ٹ�Զ��
	DataStringType_GloryExpeditionMissionInfo	= 169,		//�ٹ�Զ���ؿ���Ϣ
	DataStringType_RecommendRuneInfo			= 170,		//�����Ƽ���Ϣ
	DataStringType_GloryExpeditionMissionIndex	= 171,		//�ٹ�Զ���ؿ���Ϣ
	DataStringType_GloryExpeditionRaidInfo		= 172,		//�ٹ�Զ���ؿ���Ϣ
	DataStringType_RaidGloryNextMissionId		= 173,		//�ٹ�Զ����һ��
	DataStringType_YijiTuxiInfo					= 174,		//�ż�ͻϮ
	DataStringType_YijiXunbaoInfo				= 175,		//�ż�Ѱ��
	DataStringType_RotaryTableReward			= 176,		//����ת�̽���
	DataStringType_CombatCompareInfo			= 177,		//ս���Ա�
	DataStringType_NonpareilItemInfo			= 178,		//��Ʒ���ý�����Ϣ
	DataStringType_BestGuildRankReward			= 179,      //��ǿ�������н���
	DataStringType_BestGuildInfo				= 180,		//��ǿ����bestguild����Ϣ
	DataStringType_RaidBossPublic				= 181,		//�ƽ��ؾ�������Ϣ
	DataStringType_ToyIslandRewardInfo			= 182,		//��ߵ���׽����������Ϣ
	DataStringType_EscortRewards				= 183,		//���ʻ��ͽ���
	DataStringType_BestGuildGoalInfo			= 184,		//��ǿ����������Ϣ
	DataStringType_EscortRewardsById			= 185,		//���ʻ��ͽ���
	DataStringType_NewAchievementRewardInfo		= 186,		//�³ɾ�ϵͳ��ʾ��Ϣ
	DataStringType_YijituxiRankRewards			= 187,		//�ż�ͻϮ�������������Ϣ
	DataStringType_EntrustTaskInfo				= 188,		//ί��������Ϣ
	DataStringType_GuildBossActivityInfo		= 189,		//����boss��Ϣ

	DataStringType_RaidSingleSub				= 190,      //�����������������Ϣ
	DataStringType_GuildBossBefallStoryID		= 191,		//���ֽ���storyID
	DataStringType_GuildBossBefallReward		= 192,		//���ֽ��ٽ�����Ϣ
	DataStringType_GuildBossBefallInfo			= 193,		//���ֽ���Boss��Ϣ
	DataStringType_AnswerActivityGuildRankRewards	= 194,		//���������������������Ϣ
};

//��Ʒ��������
enum ItemUseType
{
	ITEM_USE_TYPE_NULL							= 0,		
	ITEM_USE_TYPE_LEVEL_LOW						= 1,		//�ȼ�̫��
	ITEM_USE_TYPE_WRONG_COMMUNITY				= 2,		//���ɲ���
	ITEM_USE_TYPE_SUB_LEVEL_LOW					= 3,		//�����ߵȼ�̫��
	ITEM_USE_TYPE_CANUSE						= 4,		//��ʹ��
};

static float SMALLTRUMPT_VIEW_TIME_UPPER				= 20.0f;		//���Ž���ʾʱ������
static float SUPERTRUMPT_VIEW_TIME_UPPER				= 30.0f;		//��Ž���ʾʱ������

enum GuildChannelType
{
	GUILD_DECLARATION		= 0,   //�������
	GUILD_NOTIFICATION		= 1,	//��ṫ��
};

enum GuildWorshipType
{
	WORSHIP_TYPE_FREE			= 0,	//���Ĥ��
	WORSHIP_TYPE_JINGBI			= 1,	//��ǮĤ��
	WORSHIP_TYPE_YUANBAO		= 2,	//Ԫ��Ĥ��
};

enum ProductionType
{
	PRODUCTION_TYPE_EQUIP		= 1,
	PRODUCTION_TYPE_GEM			= 2,
};

enum TopRankingError
{
	TOPRANKINGERROR_ERROR_NUM	= 1,	//��ҫ֮�۴�������1,��������
	TOPRANKINGERROR_ERROR_ZONE	= 2,	//��ҫ֮�۴�������2,����������
};

#define CHANGE_LOCKED_DISTANCE	1600
#define LOCKED_DISTANCE_OVER	2100

#define QINGGONG_TRIAL_MAPID  2601

#define UE_PLAY_NAME_BOLLD_CORRECT 105

static int TUTORIAL_1ST_REQ_ENTER_GAME		=	0;
static int TUTORIAL_1ST_ENTER_SCN			=	156;

#define SKILL_RLT_DELAY		1.0f

const char* CASH_TYPE_NAME(int index);

const char* ITEM_GRADE_CHAR(int index);
const char* ITEM_GRADE_TEXT(int index);
const char* RUNE_ITEM_GRADE_CHAR(int index);

#define MOVE_STEP_END_FLAG 0.999f

#define UI_AVATAR_PLATFORM_CHARTYPE						100000
#define UI_AVATAR_SECRET_REMAIN_PLATFORM_CHARTYPE		100003

#define LINK_TYPE_OPEN_UI_FUNC				"OpenUIByLinkID"	//��LinkUI��lua function name

//////////////////////////////////////////////////////////////////////////
//< cmdline arg to client
#define CMD_ARG_SID							"sid"				//< ������id
#define CMD_ARG_SIDX						"server_idx"		//< ������index
#define CMD_ARG_UID							"uid"				//< �û�id���ǽ�ɫid
#define CMD_ARG_UID32						"uid32"				//< uidӳ���uint32�����ⲿӳ�����
#define CMD_ARG_PLATFORM					"platform"			//< ƽ̨��
#define CMD_ARG_IP_PORT						"ip_port"			//< ��¼ip
#define CMD_ARG_CLIENT_VER					"remote_client_ver"	//< �ͻ��˰汾��
#define CMD_ARG_CLIENT_ROOT_URL				"client_root_url"	//< �ͻ���cdn��ַ
#define CMD_ARG_SIGN						"sign"				//< token
#define CMD_ARG_CLIENT_TYPE					"client"			//< ΢��(pc)/��ҳ(web)
#define CMD_ARG_ANCESTOR_WND				"ANCESTORWND"
#define CMD_ARG_SERVER_VER					"server_ver"		//< �������汾��
#define CMD_ARG_HOST1						"host1"				//< ���ص�ַ

//< tx arg begin
#define CMD_ARG_TX_SID						"osid"
#define CMD_ARG_OPENKEY						"openkey"
#define CMD_ARG_PFKEY						"pfkey"
#define CMD_ARG_APPENDINFO					"appendInfo"
#define CMD_ARG_PLATFORM_PARENT				"platform_parent"
#define CMD_ARG_PLATFORM_FULL				"platform_full"
#define CMD_ARG_PLATFORM_REGISTER			"platform_register"
#define CMD_ARG_PLATFORM_LOGIN				"platform_login"
//< tx arg end

//< sougou arg begin
#define CMD_ARG_SOUGOU_CHANNDEL				"channel"	//< channel=skinƤ����¼�� channel=mini������¼
//< sougou arg end


#define CMD_ARG_COM_ARG						L"com_arg"
#define CMD_ARG_CMDLINE						L"cmdline"
#define CMD_ARG_PATCH_CB					L"commitpatch"
#define CMD_ARG_FLASH_OBJ					L"flashplayer"

#define STR_LOGFILE_FILTER					"log\\Client_*.log"
#define STR_UI_LOGFILE_FILTER				"log\\Client_UI_*.log"
#define STR_LOADER_LOGFILE_FILTER			"log\\loader_*.log"
#define STR_DOWNLOADER_LOGFILE_FILTER		"log\\downloader_*.log"

#define STR_GUILDPROFIT_DIR					"guildprofit"
#define STR_GUILDPROFIT_DIR_FILTER			"guildprofit\\*.data"


#define STR_AUCTION_HISTORY_DIR				"auction"

//////////////////////////////////////////////////////////////////////////

static int STIGMAS_CHAR_TYPE = 50000;
static int BOSS_HALL_TYPE = 81033570;
static int ACTIVITY_HALL_CHAR_TYPE = 10203;
static int ARENA_HALL_CHAR_TYPE = 10212;
static int GUILD_HALL_CHAR_TYPE = 10214;
static int REINFORCE_CHAR_TYPE = 10210;
static int STARUP_CHAR_TYPE = 10211;
static int ALLIANCE_CHAR_TYPE = 10207;

enum CimeliaPayIndex
{
	PAY_INDEX_ITEM		= 0,
	PAY_INDEX_TYPE_1	= 1,
	PAY_INDEX_TYPE_2	= 2,
};

#define FIRST_TASK_ID	1002100

static int VEHICLE_DRUG_ITEM_USEFUNC[]			= {50508, 50501};	//����û���
static int GODWING_DRUG_ITEM_USEFUNC[]			= {50509, 50502};	//����Ǳ�ܵ�
static int DEVAEYE_DRUG_ITEM_USEFUNC[]			= {50510, 50503};	//����Ǳ�ܵ�
static int ARROW_DRUG_ITEM_USEFUNC[]			= {50511, 50504};	//����Ǳ�ܵ�
static int VEHICLEFIGHT_DRUG_ITEM_USEFUNC[]		= {50512, 50505};	//���Ǳ�ܵ�
static int SPIRITPET_DRUG_ITEM_USEFUNC[]		= {50513, 50506};	//���Ǳ�ܵ�
static int SPIRITWEAPON_DRUG_ITEM_USEFUNC[]		= {50514, 50507};	//����Ǳ�ܵ�
static int HIDDENWEAPON_DRUG_ITEM_USEFUNC[]		= {50516, 50515};	//����Ǳ�ܵ�
static int PLOUGH_DRUG_ITEM_USEFUNC[]			= {50518, 50517};	//���Ǳ�ܵ�
static int EVIL_DRUG_ITEM_USEFUNC[]				= {50520, 50519};	//��ɷǱ�ܵ�
static int FAN_DRUG_ITEM_USEFUNC[]				= {50522, 50521};	//����Ǳ�ܵ�
static int SUPERBOW_DRUG_ITEM_USEFUNC[]			= {50524, 50523};	//��Ǳ�ܵ�
static int HOOK_DRUG_ITEM_USEFUNC[]				= {50526, 50525};	//�乳Ǳ�ܵ�
static int SPIRITFAN_DRUG_ITEM_USEFUNC[]		= {50528, 50527};	//����Ǳ�ܵ�

enum PLAYER_INFO_SYSTEMTYPE
{
	PIS_NORMAL = 0,		//�鿴����
	PIS_GOLDBOSS = 1	//�ƽ�BOSS
};

struct platform_info
{
	platform_info()
		: is_vip(0)
		, is_year_vip(0)
		, vip_level(0)
		, is_high_vip(0)
		, is_expand_vip(0)
		, timestamp(0)
	{}

	inline uint8_t format_vip(void) const
	{
		return FormatTencentVipInfo(vip_level, is_vip, is_year_vip, is_high_vip, is_expand_vip);
	}

	inline void	decode_vip(const StringType& sPlatfom, uint8_t platinfo)
	{
		platform = sPlatfom;
		is_vip =(platinfo >> TENCENT_PF_VIP_IS) & 0x1;  
		vip_level = (platinfo >> TENCENT_PF_VIP_LEVEL) & TENCENT_PF_VIP_LEVEL_MASK;
		is_year_vip = platinfo >> TENCENT_PF_VIP_YEAR & 0x1;
		is_high_vip = platinfo >> TENCENT_PF_VIP_HIGH & 0x1;
		is_expand_vip = platinfo >> TENCENT_PF_VIP_EXPAND & 0x1;
	}

	StringType platform;
	StringType vipType;
	uint8_t is_vip;
	uint8_t is_year_vip;
	uint8_t vip_level;
	uint8_t is_high_vip;
	uint8_t is_expand_vip;
	uint32_t timestamp;
};

typedef mem::map<StringType, platform_info> PlatformMap;


#endif





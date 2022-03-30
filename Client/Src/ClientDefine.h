
#ifndef _CLIENT_DEFINE_H
#define _CLIENT_DEFINE_H 

#include "LM_LocaleManager.h"

typedef int32_t	time_type;


enum VideoType
{
	VIDEOTYPE_GAME_INTRO	= 0,		//游戏开场视频
	VIDEOTYPE_CHAR_DEMO		= 1,		//创建角色技能演示
	VIDEOTYPE_SKILL			= 2,		//技能演示
	VIDEOTYPE_SKILL_TOOLTIP	= 3,		//技能Tips演示

	VIDEOTYPE_COUNT,
};

//YES/NO对话框需要的逻辑类型
enum MessageDialogType
{
	MDT_INVALID						= 0,
	MDT_LEAVE_GAME_NOW				= 1,		//立刻退出游戏， Parm1 = 返回位置
	MDT_CONFIRM_CHAR_OPERATION		= 2,		//确认角色操作 Param1=操作类型，Parame2=角色ID
	MDT_CONFIRM_INVITE_TO_GROUP		= 3,		//确认邀请入队　Param1 = 邀请目标ID Param2 = 队伍ID Param3 = 被邀请目标ID
	MDT_CONFIRM_SKILL_RESPAWN		= 4,		//确认被复活　Param1 = 复活者ID	
	MDT_CONFIRM_FORGET_PRODUCTION	= 5,		//确认遗忘生产技能	Param1 = 生产类型
	//MDT_CONFIRM_COMPOSITION			= 10,		//确认合成真物品
	MDT_CONFIRM_BEADDFRIEND			= 11,		//确认被加为好友
	MDT_CONFIRM_USE_UNBINDCRASH_REDUCEPK =14, //用非绑定金减少pk值
	MDT_DIALOG_CONFIRM_TRANSFER_MAP	= 15,		//地图传送确认框
	MDT_DIALOG_CONFIRM_LOW_SCORE	= 16,		//相位副本战力不足提示
	MDT_DIALOG_CONFIRM_RECHARGE		= 17		//提示充值
};


//登录界面中与UI传递字符串用的分割符，用于分隔信息
#define LOGIN_UI_STRING_MARKER "+"
//竞技场地图ID
#define MAPID_ARENA 2039
//竞技场观战地图id
#define MAPID_ARENAPLAYBACK 2041
#define MAPID_SKYARENA 2042
//指挥官头衔uiopenlevel的key
#define COMMANDERTITLE_UIOPENLEVEL_KEY 900043
//五一活动任务类型
#define HOLIDAYACTIVITY_TYPE_WUYI 1

//使用快捷键在一定的群体中切换目标
enum ShortCutTargetType
{
	SCTT_ENEMY		= 1,//在怪物和敌对玩家中切换目标
	SCTT_FRIEND		= 2,//在右方中切换目标
};

//角色管理操作
enum CharManageActionType
{
	CMAT_DELETE_CHAR = 1,//删除角色
	CMAT_RECOVER_CHAR= 2,//恢复角色
};

#define SHORT_CUT_ENEMY_COUNT 5		//使用快捷键在一定数量的敌对目标中进行切换
#define SHORT_CUT_FRIEND_COUNT 5	//使用快捷键在一定数量的右方目标中进行切换

//背包UI单独打开时右键功能
enum BagUiRClickFuncType
{
	BAGUI_RCLICK_NONE			= 0, 	//无功能
	BAGUI_RCLICK_DRAG			= 1, 	//粘起物品
	BAGUI_RCLICK_USE			= 2, 	//直接使用
	BAGUI_RCLICK_ACTIVE_OP		= 3, 	//激活操作
	BAGUI_RCLICK_EQUIP			= 4, 	//移动到装备UI(装备物品)
	BAGUI_RCLICK_SHOP			= 5, 	//移动到商店UI
	BAGUI_RCLICK_SHORTCUT		= 6, 	//移动到快捷栏UI
	BAGUI_RCLICK_TRANSFER		= 7, 	//传送UI
	BAGUI_RCLICK_ACCEPTTASK		= 8, 	//接受任务UI
	BAGUI_RCLICK_RESET_TALENT	= 9, 	//重置天赋
	BAGUI_RCLICK_IDENTIFY_WORD	= 10,	//鉴定字帖
	BAGUI_RCLICK_MOUNTS			= 11,	//马具坐骑
	BAGUI_RCLICK_UI_INTERACTION	= 12,	//UI交互界面
	BAGUI_RCLICK_UI_OPENUI		= 13,	//打开对应UI
};

#define TOOLTIP_WINDOW_WIDTH 250.0		//Tooltip窗口宽度

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

enum CharTypeShowFlag	//显示标志位控制
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
	CUCS_TYPE_BOSS		= 3		//boss出现
};
enum RaidState	//枚举值和表中相对应，不要随便修改。
{
	CUCS_RAID_Enter			= 0,
	CUCS_RAID_BeforeQuit	= 1,
};
enum TaskState
{
	CUCS_TASK_Accept		= 0,	//正接取任务的时候
	CUCS_TASK_Processing	= 1,	//任务处理过程中
	CUCS_TASK_Finish		= 2,	//正完成任务的时候
	CUCS_TASK_Finished		= 3,	//任务曾经完成过
	CUCS_TASK_UnFinished	= 4,	//任务从未完成过
};
enum BossState
{
	CUCS_BOSS_Appear		= 0,	//Boss出场
	CUCS_BOSS_Die			= 1,	//Boss死亡
};

//特殊套装索引
enum SpecialKungfu
{
	SKF_GUILDWAR			= 1,
	SKF_CROSSGUILDWAR		= 2,
};

//特殊套装部位索引
enum SpecialKungfuDress
{
	SKFD_HEAD			= 0,
	SKFD_BODY			= 1,
	SKFD_FOOT			= 2,
};

void SetupMainWindow(HWND hWnd);
void GetDesktopCenterParamter(int& nWidth, int& nHeight, int& nOffsetX, int& nOffsetY);

#define		PLAYER_JUMP_END_ANIM_BEGIN_HEIGHT	150	//开始接跳跃结束动画时的高度

#define		SHORTCUT_INDEX_ITEM 5	//快捷栏里放道具(药水)的格子索引

#define		DAYLY_TASK_OPEN_LEVEL	21 //日常任务开启等级

#define		DAILY_RECHARGE_REWARD_NUMBER	1//每日首冲奖励数量

#define		PROMOTER_REMAIN_LEVEL	15 //推广员系统提醒等级

//坐骑，弩炮，宠物自动进阶间隔
static float AUTO_UPGRADE_INTERVAL = 0.2f;

#define		TRANSFER_LOADING_DISTANCE	3000	//超过这个距离才loading，否则直接切换位置；实际跟地形加载范围有关
#define		VEHICLE_PATH_TO_DISTANCE	2500	//超过这个寻路距离才上马
#define		DIRECTION_SHOW_DISTANCE		1000
#define		DIRECTION_SHOW_DISTANCE_GAMESTORY	1600
#define		AUTO_AVENUE_PATH_DISTANCE	1800

#define		OPEN_ABILITY_MAX_VIEW		7		//能力开启界面最大显示个数

#define		UpgradeSuccessState	32200001 //弩炮，天眼、坐骑，进阶成功获得经验用的State

//CF_WorldObjectDesc _Type
#define LINK_TYPE_NPC_LOCATION		 1	//NPC坐标
#define LINK_TYPE_ITEM_LOCATION		 2	//物品坐标
#define LINK_TYPE_MONSTER_LOCATION	 3	//怪坐标
#define LINK_TYPE_COLLECT_LOCATION	 4	//采集坐标
#define LINK_TYPE_ITEM_TIP			 5	//物品tips
#define LINK_TYPE_SKILL_TIP			 6	//技能tips
#define LINK_TYPE_ITEM_DETAILED_TIP	 7	//物品详细tips
#define LINK_TYPE_OPEN_UI			 8	//打开UI
#define LINK_TYPE_SIMPLEITEM_TIP	 9	//简单物品tips
#define LINK_TYPE_HOLYFLAM			 10	//圣火令tips
#define LINK_TYPE_TRAVEL_BOOTS		 11	//飞鞋
#define LINK_TYPE_ACHIEVEMENT		 12	//成就
#define LINK_TYPE_MAP_TRANSFER		 13	//直接传送到目标地图
#define LINK_TYPE_PLAYER_LOCATION	 14	//玩家坐标
#define LINK_TYPE_QUIT_TRANSCRIPTION		 15	//退出副本Transcription
#define LINK_TYPE_TRAVEL_BOOTS_CODINATE		 16	//飞鞋(坐标)
#define LINK_TYPE_TRAVEL_DAILYRECHARGE		 17 //每日首充
#define LINK_TYPE_TRAVEL_DIVINE				 18 //占卜
#define LINK_TYPE_GOLDEN_HOOK				 19 //黄金挂机点
#define LINK_TYPE_GAMESTORY					20
#define LINK_TYPE_MAP_TRANSFER_HOOK			21	//直接传送，战斗
#define LINK_TYPE_PARALLEL_SPACE			25	//相位副本

enum LinkParmType
{
	LINK_PARAM_TRAVEL_BOOTS			= 1,//飞鞋0x1
	LINK_PARAM_TRANSFER_NPC			= 4,//和传送阵NPC对话0x4
	LINK_PARAM_MAIN_MAP				= 8,//默认寻路或者飞鞋飞到主线中
	LINK_PARAM_MAP_POSITION			= 16,//默认获得的KEYNAME为地图名字和位置
	LINK_PARAM_NO_GAIN_JUDGE		= 32,//是否需要判断无收益
};

//角色寻路的目标行为，参照WorldObjectDesc.csv
enum	PathPendingActionType
{
	PPAT_NA = 0,
	PPAT_TALK_TO_NPC,	//交谈
	PPAT_PICK_ITEM,		//拾取物品
	PPAT_ATTACK_NPC,	//打怪
	PPAT_COLLECT_ITEM,	//采集物品
	PPAT_TRANSFER,	//传送
};

///签到奖励界面AVATAR类型
enum	SignInRewardAvatarType
{
	SIRAT_SKILL					= 0, //技能
	SIRAT_VEHICLE				= 1, //坐骑
	SIRAT_ARROW					= 2, //弩炮
	SIRAT_DEVA_EYE				= 3, //天眼
	SIRAT_VEHICLE_FIGHTING		= 4, //骑战
	SIRAT_MUHUN					= 5, //武魂
	SIRAT_MENTAL				= 6, //心法
	SIRAT_TREASURE				= 7, //藏宝
	SIRAT_BOSSHALL				= 8, //首领大厅
	SIRAT_SPIRITPET				= 9, //灵宠
	SIRAT_SPIRITWEAPON			= 10, //灵武
	SIRAT_TALISMAN				= 11, //法宝
	SIRAT_HIDDENWEAPON			= 12, //法器
	SIRAT_GODWEAPON				= 13, //神兵
	SIRAT_PLOUGH				= 14, //天罡
	SIRAT_FAN					= 15, //羽扇
	SIRAT_SUPERBOW				= 16, //神弓
	SIRAT_HOOK					= 17, //弯钩
	SIRAT_SPIRITFAN				= 18, //灵扇
	SIRAT_BUDDY					= 19, //角色
	SIRAT_STIGMAS				= 20, //圣痕
	SIRAT_TOY						= 21,//玩具
	SIRAT_END,	
};

///势力争夺战奖励领取
enum DomainRewardType
{
	DOMAIN_REWARD_TYPE_CANT		= 1, //不能领
	DOMAIN_REWARD_TYPE_DRAWED	= 2, //已领取
	DOMAIN_REWARD_TYPE_HAVE		= 3, //可以领
};

const char* SUB_TP_NAME(int index);

static int SUB_TP_CHARID[] = {1001, 2001, 3001, 6001, 0, 0, 0, 0, 5001};

//lua获取数据类型
enum DataStringType
{
	DataStringType_GuildRaid_Begin				= 1,		//公会boss 开始
	DataStringType_GuildRaid_BOSS_Detail		= 2,		//boss详情
	DataStringType_GuildRaid_Info				= 3,		//当前地图所有信息
	DataStringType_GuildRaid_Reward				= 4,		//奖励列表
	DataStringType_GuildRaid_Boss_TIP			= 5,		//boss tooltips
	DataStringType_GuildRaid_Boss_State_List	= 6,		//boss状态列表
	DataStringType_GuildRaid_Boss_CharTypeList	= 7,		//boss列表
	DataStringType_GuildRaid_Boss_Progress		= 8,		//获得当前进度
	DataStringType_GuildRaid_Boss_Title			= 9,		//title
	DataStringType_GuildRaid_Boss_EndTime		= 10,		//结束时间
	DataStringType_GuildRaid_End				= 20,		//公会boss 结束

	DataStringType_WildPrinces_Begin			= 21,		//野外boss 开始
	DataStringType_WildPrinces_BossList			= 22,		//野外boss活动
	DataStringType_WildPrinces_Boss_Info		= 23,		//野外boss详细信息
	DataStringType_WildPrinces_MapStateList		= 24,		//地图状态
	DataStringType_WildPrinces_Boss_Pos			= 25,		//boss位置
	DataStringType_WildPrinces_TimeStamp		= 26,		//下次刷新时间
	DataStringType_WildPrinces_End				= 30,		//野外boss 结束

	DataStringType_COURAGE_SHOP_LIST  			= 32,		//勇气兑换商店

	DataStringType_UIOpenLevel					= 35,		//雷达地图左侧按钮开启信息
	DataStringType_TrialShopList				= 36,		//试炼点数
	DataStringType_TaskListRewardList			= 37,		//任务奖励
	DataStringType_PurchaseBackReward			= 38,		//购回奖励

	DataStringType_Achievement_Begin			= 40,		//成就系统 开始
	DataStringType_Achievement_TitleList		= 50,		//称号列表
	DataStringType_Achievement_TitleCount		= 51,		//列表称号数量
	DataStringType_Achievement_CurrentTitle		= 52,		//当前称号
	DataStringType_Achievement_TitleStateList	= 53,		//称号状态
	DataStringType_Achievement_TitleTips		= 54,		//称号tips
	DataStringType_Achievement_End				= 60,		//成就系统 结束
	DataStringType_CommonStatRewardList_Confirm	= 61,		//CommonStat奖励显示，确定奖励
	DataStringType_CommonStatRewardList_Random	= 62,		//CommonStat奖励显示，随机奖励
	
	DataStringType_OpenLevel_Detail				= 64,		//OPENLEVEL表格的一些细节
	DataStringType_SlotMachineShop_ItemList		= 65,		//老虎机商店物品列表
	DataStringType_ShotActivity_Begin			= 70,		//冲榜活动开始
	DataStringType_ShotActivity_EntryInfo		= 71,		//冲榜活动子活动信息
	DataStringType_ShotActivity_RankTypeID		= 72,		//冲榜活动榜ID信息
	DataStringType_ShotActivity_BaseEntryInfo	= 73,		//冲榜活动子活动基础信息
	DataStringType_ShotActivity_MyRank			= 74,		//冲榜类活动我的排名
	DataStringType_ShotActivity_DirtyFlagRv		= 75,		//冲榜类活动Revision
	DataStringType_ShotActivity_RankInfoByIndex	= 76,		//冲榜类活动排行榜信息
	DataStringType_ShotActivity_SetBeginIndex	= 77,		//设置查询BEGININDEX
	DataStringType_ShotActivity_GetTotalSize	= 78,		//获得排行榜SIZE信息
	DataStringType_ShotActivity_GetValidDays	= 79,		//获得活动有效时间
	DataStringType_ShotActivity_DecreaseCount	= 80,		//冲榜活动减少数量
	DataStringType_CommonStatRewardList_MainTask= 81,		//赏金Boss任务奖励显示
	DataStringType_FunctionPreview				= 82,		//功能预览
	DataStringType_ShopList_EntryInfo			= 83,		

	DataStringType_GameStoryRaidList_BossInfo	= 84,		//明教禁地Boss信息

	DataStringType_WorldMapList_LimitInfo		= 85,		//地图进入限制相关信息
	
	DataStringType_SendFlower_PanelInfo			= 87,		//鲜花面板信息
	DataStringType_PopularReward_Info			= 88,

	DataStringType_DisassembleUse_Info			= 90,		//可拆分礼包开启信息
	DataStringType_OffLineExp_Info				= 91,		//离线经验配置信息
	DataStringType_NewBeeMap_Info				= 92,		//地图分流相关信息获取

	DataStringType_DiscountShopList				= 97,		//折扣商店列表

	DataStringType_ExchangeShop					= 99,		//橙装兑换商店
	DataStringType_Subordinage_Profit			= 100,		//进阶返利

	DataStringType_LimitGiftBag					= 101,		//绝版礼包
	
	DataStringType_CharAttribute				= 111,		//通用属性加成
	
	DataStringType_SubordinateSpecialAttribInfo	= 116,
	DataStringType_SubordinateExpireForbid		= 117,
	DataStringType_MulSelectGift_Info			= 118,		//多开礼包信息

	DataStringType_RaidExpInfo                  = 133,      //经验副本相关信息
	DataStringType_RaidSingleSole               = 134,      //进阶副本相关信息
	DataStringType_RailyActivity                = 135,      //活跃度相关信息
	DataStringType_RailySignReward              = 136,      //每日签到相关信息
	DataStringType_RailyCharge	                = 137,      //首冲七日相关信息
	DataStringType_BuddyUpgradCost				= 138,      //角色成长相关信息
	DataStringType_BuddyUpgradData				= 139,      //角色成长相关信息
	
	DataStringType_OffLineRwdRatio_Info			= 141,		//离线奖励倍率信息
	DataStringType_Collection					= 142,		//收藏相关信息
	DataStringType_BossFirstKill				= 143,		//首杀boss相关信息
	DataStringType_EndlessTower					= 144,		//无尽之塔相关信息
	DataStringType_Biography					= 145,		//传记相关信息
	DataStringType_BuddyBiography				= 146,		//小节相关信息
	DataStringType_Holydeed                     = 147,      //圣契相关信息
	DataStringType_SevenDaysExperiment			= 148,		//七日试炼相关信息
	DataStringType_EquipUpgrade					= 149,      //装备进阶相关信息
	DataStringType_BattlePassCard				= 150,		//神选狂欢季相关信息
	DataStringType_BattlePassCardRewardShow		= 151,      //神选狂欢季大奖一览相关信息
	DataStringType_BossInvade					= 152,		//异种侵袭相关信息
	DataStringType_BossInvadeRankReward			= 153,		//异种侵袭排名奖励相关信息
	DataStringType_VipNew						= 154,		//新VIP相关信息
	DataStringType_VipNewTask					= 155,		//新VIP任务信息
	DataStringType_RaidBossSpeciesStoryID		= 156,		//异种侵袭活动id
	DataStringType_RecommendHeroByID			= 157,		//推荐英雄信息
	
	DataStringType_ACTShopAAvatarInfo			= 160,		//ActShopA表 模型数据
	DataStringType_RaidBossPersonal				= 161,		//命运困境相关信息
	DataStringType_AnswerActivityPointReward    = 162,      //答题活动积分奖励相关信息
	DataStringType_AnswerActivityRankRewards	= 163,		//答题活动排名奖励相关信息
	DataStringType_CommanderTitleRewards		= 164,		//指挥官头衔奖励相关信息
	DataStringType_SpecialTrainingRewardInfo	= 165,		//特训奖励相关信息
	DataStringType_CharTypePosition				= 166,		//Chartype镜头位置相关信息
	DataStringType_PersonalHeadSetting			= 167,		//头像框相关信息
	DataStringType_GloryExpeditionReward		= 168,		//荣光远征
	DataStringType_GloryExpeditionMissionInfo	= 169,		//荣光远征关卡信息
	DataStringType_RecommendRuneInfo			= 170,		//宝具推荐信息
	DataStringType_GloryExpeditionMissionIndex	= 171,		//荣光远征关卡信息
	DataStringType_GloryExpeditionRaidInfo		= 172,		//荣光远征关卡信息
	DataStringType_RaidGloryNextMissionId		= 173,		//荣光远征下一关
	DataStringType_YijiTuxiInfo					= 174,		//遗迹突袭
	DataStringType_YijiXunbaoInfo				= 175,		//遗迹寻宝
	DataStringType_RotaryTableReward			= 176,		//幸运转盘奖励
	DataStringType_CombatCompareInfo			= 177,		//战力对比
	DataStringType_NonpareilItemInfo			= 178,		//极品白拿奖励信息
	DataStringType_BestGuildRankReward			= 179,      //最强公会排行奖励
	DataStringType_BestGuildInfo				= 180,		//最强公会bestguild表信息
	DataStringType_RaidBossPublic				= 181,		//浩劫秘境奖励信息
	DataStringType_ToyIslandRewardInfo			= 182,		//玩具岛捕捉次数奖励信息
	DataStringType_EscortRewards				= 183,		//物资护送奖励
	DataStringType_BestGuildGoalInfo			= 184,		//最强公会任务信息
	DataStringType_EscortRewardsById			= 185,		//物资护送奖励
	DataStringType_NewAchievementRewardInfo		= 186,		//新成就系统显示信息
	DataStringType_YijituxiRankRewards			= 187,		//遗迹突袭排名奖励相关信息
	DataStringType_EntrustTaskInfo				= 188,		//委派任务信息
	DataStringType_GuildBossActivityInfo		= 189,		//公会boss信息

	DataStringType_RaidSingleSub				= 190,      //升阶试炼副本相关信息
	DataStringType_GuildBossBefallStoryID		= 191,		//异种降临storyID
	DataStringType_GuildBossBefallReward		= 192,		//异种降临奖励信息
	DataStringType_GuildBossBefallInfo			= 193,		//异种降临Boss信息
	DataStringType_AnswerActivityGuildRankRewards	= 194,		//答题活动公会排名奖励相关信息
};

//物品可用类型
enum ItemUseType
{
	ITEM_USE_TYPE_NULL							= 0,		
	ITEM_USE_TYPE_LEVEL_LOW						= 1,		//等级太低
	ITEM_USE_TYPE_WRONG_COMMUNITY				= 2,		//门派不符
	ITEM_USE_TYPE_SUB_LEVEL_LOW					= 3,		//进阶线等级太低
	ITEM_USE_TYPE_CANUSE						= 4,		//可使用
};

static float SMALLTRUMPT_VIEW_TIME_UPPER				= 20.0f;		//银号角显示时间上限
static float SUPERTRUMPT_VIEW_TIME_UPPER				= 30.0f;		//金号角显示时间上限

enum GuildChannelType
{
	GUILD_DECLARATION		= 0,   //帮会宣言
	GUILD_NOTIFICATION		= 1,	//帮会公告
};

enum GuildWorshipType
{
	WORSHIP_TYPE_FREE			= 0,	//免费膜拜
	WORSHIP_TYPE_JINGBI			= 1,	//金钱膜拜
	WORSHIP_TYPE_YUANBAO		= 2,	//元宝膜拜
};

enum ProductionType
{
	PRODUCTION_TYPE_EQUIP		= 1,
	PRODUCTION_TYPE_GEM			= 2,
};

enum TopRankingError
{
	TOPRANKINGERROR_ERROR_NUM	= 1,	//荣耀之巅错误类型1,人数不足
	TOPRANKINGERROR_ERROR_ZONE	= 2,	//荣耀之巅错误类型2,赛区无数据
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

#define LINK_TYPE_OPEN_UI_FUNC				"OpenUIByLinkID"	//打开LinkUI的lua function name

//////////////////////////////////////////////////////////////////////////
//< cmdline arg to client
#define CMD_ARG_SID							"sid"				//< 服务器id
#define CMD_ARG_SIDX						"server_idx"		//< 服务器index
#define CMD_ARG_UID							"uid"				//< 用户id，非角色id
#define CMD_ARG_UID32						"uid32"				//< uid映射成uint32，由外部映射后传入
#define CMD_ARG_PLATFORM					"platform"			//< 平台名
#define CMD_ARG_IP_PORT						"ip_port"			//< 登录ip
#define CMD_ARG_CLIENT_VER					"remote_client_ver"	//< 客户端版本号
#define CMD_ARG_CLIENT_ROOT_URL				"client_root_url"	//< 客户端cdn地址
#define CMD_ARG_SIGN						"sign"				//< token
#define CMD_ARG_CLIENT_TYPE					"client"			//< 微端(pc)/网页(web)
#define CMD_ARG_ANCESTOR_WND				"ANCESTORWND"
#define CMD_ARG_SERVER_VER					"server_ver"		//< 服务器版本号
#define CMD_ARG_HOST1						"host1"				//< 下载地址

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
#define CMD_ARG_SOUGOU_CHANNDEL				"channel"	//< channel=skin皮肤登录， channel=mini大厅登录
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

static int VEHICLE_DRUG_ITEM_USEFUNC[]			= {50508, 50501};	//坐骑幻化丹
static int GODWING_DRUG_ITEM_USEFUNC[]			= {50509, 50502};	//神翼潜能丹
static int DEVAEYE_DRUG_ITEM_USEFUNC[]			= {50510, 50503};	//宠物潜能丹
static int ARROW_DRUG_ITEM_USEFUNC[]			= {50511, 50504};	//弩炮潜能丹
static int VEHICLEFIGHT_DRUG_ITEM_USEFUNC[]		= {50512, 50505};	//骑兵潜能丹
static int SPIRITPET_DRUG_ITEM_USEFUNC[]		= {50513, 50506};	//灵宠潜能丹
static int SPIRITWEAPON_DRUG_ITEM_USEFUNC[]		= {50514, 50507};	//灵武潜能丹
static int HIDDENWEAPON_DRUG_ITEM_USEFUNC[]		= {50516, 50515};	//法器潜能丹
static int PLOUGH_DRUG_ITEM_USEFUNC[]			= {50518, 50517};	//天罡潜能丹
static int EVIL_DRUG_ITEM_USEFUNC[]				= {50520, 50519};	//地煞潜能丹
static int FAN_DRUG_ITEM_USEFUNC[]				= {50522, 50521};	//羽扇潜能丹
static int SUPERBOW_DRUG_ITEM_USEFUNC[]			= {50524, 50523};	//神弓潜能丹
static int HOOK_DRUG_ITEM_USEFUNC[]				= {50526, 50525};	//弯钩潜能丹
static int SPIRITFAN_DRUG_ITEM_USEFUNC[]		= {50528, 50527};	//灵扇潜能丹

enum PLAYER_INFO_SYSTEMTYPE
{
	PIS_NORMAL = 0,		//查看资料
	PIS_GOLDBOSS = 1	//黄金BOSS
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





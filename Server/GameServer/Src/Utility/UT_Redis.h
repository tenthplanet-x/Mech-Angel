#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <stdio.h>
#include "redispp.h"

#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////////
#define UT_REDIS_KEY_SEPATATOR :
#define _UT_REDIS_STRINGFY(x) #x
#define UT_REDIS_STRINGFY(x) _UT_REDIS_STRINGFY(x)
#define _UT_REDIS_CONN(a, b) a##b
#define UT_REDIS_CONN(a, b) _UT_REDIS_CONN(a, b)
///////////////////////////////////////////////////////////////////////////////////////
#define UT_REDIS_KEY_CHAR_CACHE UT_REDIS_STRINGFY(:char_cache)					//玩家缓存
#define UT_REDIS_KEY_AVAIL_COUNTS UT_REDIS_STRINGFY(:avail_counts)				//可用计数
#define UT_REDIS_KEY_USER_SVRIDS UT_REDIS_STRINGFY(:user_svrids)				//玩家人数
#define UT_REDIS_KEY_GSNODE_SARVER_INFO UT_REDIS_STRINGFY(:gsnode_serverinfo) //gs节点服务器信息
#define UT_REDIS_KEY_MERGE_SARVER_INFO UT_REDIS_STRINGFY(:mergeserver_serverinfo) //mergeServerId 合服 serverid
#define UT_REDIS_KEY_TRANSCRIBE_FIGHT UT_REDIS_STRINGFY(:transcribe_fight) //transcribe_fight
#define UT_REDIS_KEY_CODE_DEFINES UT_REDIS_STRINGFY(:code_defines)				//激活码
#define UT_REDIS_KEY_CODE_AUTOID UT_REDIS_STRINGFY(:code_autoid)				//acitvacatio
#define UT_REDIS_KEY_GM_ACCOUNTS UT_REDIS_STRINGFY(:gm_accounts)				//GM账号 --- 已废弃
#define UT_REDIS_KEY_GM_CMD_REQS UT_REDIS_STRINGFY(:gm_reqs)					//gm请求
#define UT_REDIS_KEY_GM_CMD_RESPS UT_REDIS_STRINGFY(:gm_resps)					//gm回复
#define UT_REDIS_KEY_HTTP_REQS UT_REDIS_STRINGFY(:http_reqs)					//http请求
#define UT_REDIS_KEY_HTTP_RESPS UT_REDIS_STRINGFY(:http_resps)					//http回复
#define UT_REDIS_KEY_GM_CHARS UT_REDIS_STRINGFY(:gm_chars)						//gm角色列表 --- 集团使用
#define UT_REDIS_KEY_GM_CMD_SORT UT_REDIS_STRINGFY(:gm_infosort)				//玩家数据排序
#define UT_REDIS_KEY_CODE_USED_TIMES UT_REDIS_STRINGFY(:code_used_times)		//激活码次数
#define UT_REDIS_KEY_SHOT_ACTIVITY_ENTRY UT_REDIS_STRINGFY(:shot_act_entry)		//冲榜活动
#define UT_REDIS_KEY_SHOT_ACTIVITY_RANK UT_REDIS_STRINGFY(:shot_act_rank)		//冲榜活动 group
#define UT_REDIS_KEY_SHOT_REDIS_RANK UT_REDIS_STRINGFY(:shot_redis_rank)		//冲榜活动 redis
#define UT_REDIS_KEY_STORY_ACTIVITY_RANK UT_REDIS_STRINGFY(:story_act_rank)		//冲榜活动 gamestory
#define UT_REDIS_KEY_SHOT_ACTIVITY_GROUP_PREFIX UT_REDIS_STRINGFY(:group)		//冲榜活动
#define UT_REDIS_KEY_SOCIETY_PREFIX UT_REDIS_STRINGFY(:society)					//society
#define UT_REDIS_KEY_ACTIVITY_TREASURE_PUT UT_REDIS_STRINGFY(:treasure_put)				//treasure put
#define UT_REDIS_KEY_ACTIVITY_TREASURE_OCCUPY UT_REDIS_STRINGFY(:treasure_occupy)		//treasure occupy
#define UT_REDIS_KEY_ACTIVITY_TREASURE_CATAGORY UT_REDIS_STRINGFY(:treasure_catagory)	//treasure occupy
#define UT_REDIS_KEY_GAME_REPORT UT_REDIS_STRINGFY(:game_report)						//游戏上报
#define UT_REDIS_KEY_ITEM_FLOPS UT_REDIS_STRINGFY(:item_flops)							//分解开包
#define UT_REDIS_KEY_DAILY_DATA_REPORT UT_REDIS_STRINGFY(:daily_data_report)			//数据日报
#define UT_REDIS_KEY_ONLINE_RECORD UT_REDIS_STRINGFY(:online_record)					//在线人数统计
#define UT_REDIS_KEY_HOURLY_MAX_ONLINE_IP UT_REDIS_STRINGFY(:hourly_max_online_ip)		//每小时最大在线IP和最大人数统计
#define UT_REDIS_KEY_PREDAY_ACTIVE UT_REDIS_STRINGFY(:preday_active)					//前一天的活跃人数
#define UT_REDIS_KEY_UPDATE_BULLETIN UT_REDIS_STRINGFY(:update_bulletin)				//更新公告
#define UT_REDIS_KEY_HANDLE_ASSET UT_REDIS_STRINGFY(:handle_asset)						//处理玩家资产
#define UT_REDIS_KEY_ACTIVITY_SNOWMAN_SCORE UT_REDIS_STRINGFY(:act_snowman_score)		//雪人狂欢活动积分
#define UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD UT_REDIS_STRINGFY(:act_limit_reward:)				//全服限次活动
#define UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD_SUB_B UT_REDIS_STRINGFY(:act_limit_reward_sub_b:)				//开服争霸榜全局限次活动
#define UT_REDIS_KEY_BOSS_FIRST_KILL UT_REDIS_STRINGFY(:act_server_first_kill_boss)					//首杀
#define UT_REDIS_KEY_ACT_SUBORDINATE_TIME UT_REDIS_STRINGFY(:act_sub_time)//普通达标活动结束时的时间
#define UT_REDIS_KEY_ACT_SUBORDINATE_TIME_PLUS UT_REDIS_STRINGFY(:act_sub_time_plus)//进阶达标活动结束时的时间
#define UT_REDIS_KEY_ACT_SUB_PLAYER_INFO UT_REDIS_STRINGFY(:act_sub_player_info)//普通达标活动结束时，玩家的基础数据
#define UT_REDIS_KEY_ACT_SUB_PLAYER_INFO_PLUS UT_REDIS_STRINGFY(:act_sub_player_info_plus)//高级达标活动结束时，玩家的基础数据
#define UT_REDIS_KEY_ACT_SHOP_START_TIME UT_REDIS_STRINGFY(:act_shop_start_time)//商城的开启时间
#define UT_REDIS_KEY_ACT_SEVENDAY_TRIAL UT_REDIS_STRINGFY(:act_sevenday_trial)//七日试炼活动的结束时间
#define UT_REDIS_KEY_INVESTMENTS UT_REDIS_STRINGFY(:investments)//星券商城投资计划活动1
#define UT_REDIS_KEY_INVESTMENTS_CARD_STARTTIME UT_REDIS_STRINGFY(:investment_card_starttime)//投资卡活动开启时间
#define UT_REDIS_KEY_INVESTMENTS_CARD_ENDTIME UT_REDIS_STRINGFY(:investment_card_endtime)//投资卡活动结束时间
#define UT_REDIS_KEY_CURRENT_REFRESH_REWARDTASK_TIME UT_REDIS_STRINGFY(:current_refresh_rewardtask_time)//悬赏活动当前刷新时间

#define UT_REDIS_KEY_RANK_STATE UT_REDIS_STRINGFY(:rank_state)//排行榜榜单结算状态

#define UT_REDIS_KEY_YIZHONGJIANGLIN_STORY UT_REDIS_STRINGFY(:yizhongjianglin_story)//异种奖励要发奖的storyID
#define UT_REDIS_KEY_COMBAT_INFO UT_REDIS_STRINGFY(:combat_info)//伤害信息
#define UT_REDIS_KEY_DAMAGE_RANK UT_REDIS_STRINGFY(:damage_player_rank)//玩家伤害排名
#define UT_REDIS_KEY_BATTLE_INFO UT_REDIS_STRINGFY(:battle_player_info)//伤害排名的统计信息
#define UT_REDIS_KEY_STORT_END_TIME UT_REDIS_STRINGFY(:story_endtime)//活动结束时间

#define UT_REDIS_KEY_GLOBAL_STORT_END_TIME UT_REDIS_STRINGFY(:global_story_endtime)//global节点活动的结束时间
#define UT_REDIS_KEY_GUILD_DAMAGE_RANK UT_REDIS_STRINGFY(:guild_damage_rank)//公会伤害排名
#define UT_REDIS_KEY_GUILD_DAMAGE_INFO UT_REDIS_STRINGFY(:guild_damage_info)//公会伤害信息


#define UT_REDIS_KEY_GUILDMAPBOSS_DAMAGE_RANK UT_REDIS_STRINGFY(:guildmapboss_damage_rank)//公会秘境对boss的伤害
#define UT_REDIS_KEY_GUILDMAPBOSS_PLAYER_INFO UT_REDIS_STRINGFY(:guildmapboss_player_info)//公会秘境玩家信息
#define UT_REDIS_KEY_GUILDMAPBOSS_STATE_INFO UT_REDIS_STRINGFY(:guildmapboss_state_info)//公会秘境状态信息

#define UT_REDIS_KEY_ANSWER_STORY_END_TIME UT_REDIS_STRINGFY(:answer_story_endtime)//答题活动的结束时间
#define UT_REDIS_KEY_ANSWER_STORY_TOPIC UT_REDIS_STRINGFY(:answer_story_topic)//答题活动的选题
#define UT_REDIS_KEY_ANSWER_ACCOUNT_POINT UT_REDIS_STRINGFY(:answer_account_point)//答题活动各玩家的积分
#define UT_REDIS_KEY_ANSWER_GUILD_POINT UT_REDIS_STRINGFY(:answer_guild_point)//答题活动工会的积分
#define UT_REDIS_KEY_ANSWER_GUILD_INFO UT_REDIS_STRINGFY(:answer_guild_info)//答题活动工会的信息
#define UT_REDIS_KEY_ANSWER_PLAYER_INFO UT_REDIS_STRINGFY(:answer_player_info)//答题活动各玩家的信息
#define UT_REDIS_KEY_ANSWER_CHECKAWARD_TIME UT_REDIS_STRINGFY(:answer_checkaward_time)//答题奖励的派发时间
#define UT_REDIS_KEY_ANSWER_CLOSE UT_REDIS_STRINGFY(:answer_close)//答题奖励的派发
//公会战  个人榜  4个阶段 + 总排名 
#define UT_REDIS_KEY_PERSONAL_RANK_POINT_BY_TYPE UT_REDIS_STRINGFY(:player_score)//公会战活动 各玩家的积分
#define UT_REDIS_KEY_PERSONAL_RANK_PLAYER_INFO_BY_TYPE UT_REDIS_STRINGFY(:player_info)//公会战活动 各玩家的信息
//公会战  公会榜  4个阶段 + 总排名 
#define UT_REDIS_KEY_GUILD_RANK_POINT_BY_TYPE UT_REDIS_STRINGFY(:guild_score)//公会战活动  各公会的积分
#define UT_REDIS_KEY_GUILD_RANK_PLAYER_INFO_BY_TYPE UT_REDIS_STRINGFY(:guild_info)//公会战活动  各公会信息
// 公会战 抢夺哨塔
#define UT_REDIS_KEY_WATCH_TOWER_INFO UT_REDIS_STRINGFY(:watch_tower_info)//哨塔相关信息
#define UT_REDIS_KEY_BOSS_DAMAGE_INFO UT_REDIS_STRINGFY(:boss_damage_info)//boss伤害信息

#define UT_REDIS_KEY_GUILD_FIGHT_DEFEND_INFO UT_REDIS_STRINGFY(:guild_defend_info)//抢分防守信息

//
#define UT_REDIS_KEY_SOLODULE_ROBOT UT_REDIS_STRINGFY(:solo_dule_robot)//竞技场机器人的数量
#define UT_REDIS_KEY_SOLOINIT_ROBOT UT_REDIS_STRINGFY(:solo_init_robot)//竞技场机器人初始key
#define UT_REDIS_KEY_SOLO_OPEN_TIME UT_REDIS_STRINGFY(:solo_open_time)//挑战时间
#define UT_REDIS_KEY_SOLO_HALLOFFAME UT_REDIS_STRINGFY(:solo_halloffame)//名人堂
#define UT_REDIS_KEY_SUMMITARENA_GROUPSTAGE UT_REDIS_STRINGFY(:summit_arena_group)//巅峰竞技场数据
#define UT_REDIS_KEY_SUMMITARENA_HALLOFFAME UT_REDIS_STRINGFY(:summit_halloffame)//名人堂
#define UT_REDIS_KEY_SKYARENA_ZONESEASON UT_REDIS_STRINGFY(:skyarena_zoneseason)//赛季数据
#define UT_REDIS_KEY_SUMMITARENA_OPEN_TIME UT_REDIS_STRINGFY(:summit_open_time)//挑战时间
#define UT_REDIS_KEY_SINGLE_SUMMITARENA_HALLOFFAME UT_REDIS_STRINGFY(:single_summit_halloffame)//单战区名人堂
#define UT_REDIS_KEY_SUB_INFO_VEHICLE UT_REDIS_STRINGFY(:sub_info_vehicle)//开服争霸榜，幻兽数据
#define UT_REDIS_KEY_SUB_INFO_VEHICLE_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_vehicle_player_info)//开服争霸榜，幻兽，玩家数据
#define UT_REDIS_KEY_SUB_INFO_WING UT_REDIS_STRINGFY(:sub_info_wing)//开服争霸榜，背饰数据
#define UT_REDIS_KEY_SUB_INFO_WING_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_wing_player_info)//开服争霸榜，背饰，玩家数据
#define UT_REDIS_KEY_SUB_INFO_DEVAEYE UT_REDIS_STRINGFY(:sub_info_devaeye)//开服争霸榜，萌宠数据
#define UT_REDIS_KEY_SUB_INFO_DEVAEYE_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_devaeye_player_info)//开服争霸榜，萌宠，玩家数据
#define UT_REDIS_KEY_SUB_INFO_ALL_RUNE_ITEM UT_REDIS_STRINGFY(:sub_info_rune_item)//开服争霸榜，所有宝具总等级
#define UT_REDIS_KEY_SUB_INFO_ALL_RUNE_ITEM_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_rune_item_player_info)//开服争霸榜，所有宝具总等级，玩家数据
#define UT_REDIS_KEY_SUB_INFO_COMMANDER_LEVEL UT_REDIS_STRINGFY(:sub_info_commander_level)//开服争霸榜，指挥官等级
#define UT_REDIS_KEY_SUB_INFO_COMMANDER_LEVEL_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_commander_level_player_info)//开服争霸榜，指挥官等级，玩家数据
#define UT_REDIS_KEY_SUB_INFO_COMMANDER_TITLE UT_REDIS_STRINGFY(:sub_info_commander_title)//开服争霸榜，指挥官头衔
#define UT_REDIS_KEY_SUB_INFO_COMMANDER_TITLE_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_commander_title_player_info)//开服争霸榜，指挥官头衔，玩家数据
#define UT_REDIS_KEY_SUB_INFO_CONTROLLER_EQUIP_COMBATSCORE UT_REDIS_STRINGFY(:sub_info_controller_equip_combatscore)//开服争霸榜，指挥官装备战力
#define UT_REDIS_KEY_SUB_INFO_CONTROLLER_EQUIP_COMBATSCORE_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_controller_equip_combatscore_player_info)//开服争霸榜，指挥官装备战力，玩家数据

#define UT_REDIS_KEY_BESTGUILD_ENDTIME UT_REDIS_STRINGFY(:bestguild_endtime)//最强公会结束时间
#define UT_REDIS_KEY_BESTGUILD_ACTIVITY_ENDTIME UT_REDIS_STRINGFY(:bestguild_activity_endtime)//最强公会阶段活动结束时间
#define UT_REDIS_KEY_REGULARBOSS_KILLER_INFO UT_REDIS_STRINGFY(:regularboss_killer_info) //浩劫秘境击杀者信息
#define UT_REDIS_KEY_REWARD_TASK_ENDTIME UT_REDIS_STRINGFY(:reward_task_endtime)//悬赏任务的结束时间
#define UT_REDIS_KEY_SUBB_RANK_RECORD UT_REDIS_STRINGFY(:subb_rank_record)//已经结算了开服争霸帮的groupid

#define UT_REDIS_KEY_SUMMITARENA_GUESS UT_REDIS_STRINGFY(:summit_arena_guess)//巅峰竞技场竞猜者的竞猜信息
#define UT_REDIS_KEY_SUMMITARENA_GUESS_COUNT UT_REDIS_STRINGFY(:summit_arena_guess_count)//巅峰竞技场被竞猜者的压中人数

#define UT_REDIS_KEY_MONEYPOINT_YIJITUXI UT_REDIS_STRINGFY(:yijituxi_moneypoint) //遗迹突袭积分榜
#define UT_REDIS_KEY_MONEYPOINT_YIJITUXI_PLAYER_INFO UT_REDIS_STRINGFY(:yijituxi_moneypoint_player_info)//遗迹突袭各玩家的信息
#define UT_REDIS_KEY_YIJITUXI_STORY_END_TIME UT_REDIS_STRINGFY(:yijituxi_story_endtime)//遗迹突袭活动的结束时间

#define UT_REDIS_KEY_WATCHTOWER_TIME_INFO UT_REDIS_STRINGFY(:watch_tower_time_info)//公会战活动时间

#define UT_REDIS_KEY_RANK_START_END_TIME UT_REDIS_STRINGFY(:rank_start_endtime_)//排行榜 存储 开始，结束时间

#define UT_REDIS_KEY_OFFLINE_ANSWER_RESULT UT_REDIS_STRINGFY(:offline_answer_result)//离线玩家的答题排名数据

///////////////////////////////////////////////////////////////////////////////////////
#define UT_REDIS_KEY_RUMOR_INFO UT_REDIS_STRINGFY(:rumor)						//广播模块
#define UT_REDIS_KEY_RUMOR_DIRTY UT_REDIS_STRINGFY(:rumor_dirty)				//广播模块
#define UT_REDIS_KEY_INIT_VERSION UT_REDIS_STRINGFY(:init_version)				//服务器初始化脚本版本号
#define UT_REDIS_KEY_SERVER_RANK_TOPS UT_REDIS_STRINGFY(:rank_tops)				//排行榜快照
#define UT_REDIS_KEY_TOP10_AVG_POWER UT_REDIS_STRINGFY(:top10_avg_power)		//前十平均战力
#define UT_REDIS_KEY_GLOBAL_VALUES UT_REDIS_STRINGFY(:global_values)			//全局数据
#define UT_REDIS_KEY_CUS_RES_VERSION UT_REDIS_STRINGFY(:cus_res_version)		//服务器reload相关
#define UT_REDIS_KEY_PLAYER_ONLINE_SNAPSHOT UT_REDIS_STRINGFY(:player_online_snapshot)			//玩家在线快照数据 索引依据玩家账号
///////////////////////////////////////////////////////////////////////////////////////
#define DEFAULT_REDIS_AUTH_PASS "jzwldev"
#define PLATFORM_AUTH_NUM 100													//登陆key哈希数量
#define PLATFORM_INNERUIDKEY UT_REDIS_STRINGFY(:auth_userinnerindex)			//登陆自增id
#define PLATFORM_AUTH_KEY UT_REDIS_STRINGFY(:auth_game2inner)					//用户登录，jzext cmm之间交互
///////////////////////////////////////////////////////////////////////////////////////
#define BALANCE_INFO_KEY UT_REDIS_STRINGFY(:cmm_balanceinfo)					//负载均衡key
#define GLOBAL_ACTIVE_SERVER_KEY UT_REDIS_STRINGFY(:activeserverlist_flag)		//活跃服务器 publish
#define GLOBAL_ACTIVE_SERVER_SKEY UT_REDIS_STRINGFY(:activeserverlist_save_flag)//活跃服务器 save
#define GLOBAL_GROUP_SERVER_KEY UT_REDIS_STRINGFY(:serverlistgroup_flag)		//服务器分组 publish
#define GLOBAL_GROUP_SERVER_SKEY UT_REDIS_STRINGFY(:serverlistgroup_save_flag)	//服务器分组 save
#define GLOBAL_GROUP_SERVER_BKEY UT_REDIS_STRINGFY(:serverlistgroup_base_flag)	//服务器分组 原始数据
///////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_AUTH_GAME_ACCOUNTCHARID UT_REDIS_STRINGFY(:auth_accountcharid)			//玩家账号和charid关联
#define PLATFORM_AUTH_GAME_USERINFOKEY UT_REDIS_STRINGFY(:auth_gameuserinfo)			//玩家数据 索引依据玩家账号
#define PLATFORM_AUTH_GAME_NAMEINFOKEY UT_REDIS_STRINGFY(:auth_gamenameinfo)			//玩家账号和角色名索引
#define PLATFORM_AUTH_GAME_CHAR2USERKEY UT_REDIS_STRINGFY(:auth_gamechar2user)			//玩家账号和角色id索引
#define PLATFORM_AUTH_GAME_PLATFORMINFOKEY UT_REDIS_STRINGFY(:auth_gameplatfrominfo)	//玩家平台信息，类似黄钻等
#define PLATFORM_AUTH_GAME_USERLOGINKEY UT_REDIS_STRINGFY(:auth_userloginplat)			//玩家登陆平台信息
#define PLATFORM_HTTP_PUBLISH_GAME_KEY UT_REDIS_STRINGFY(:pub_gamepublish2http)			//http上报
///////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_BONUS_POOL_KEY UT_REDIS_STRINGFY(:bonus_pool)							//奖金池
#define PLATFORM_BONUS_ROLE UT_REDIS_STRINGFY(:bonus_role_set)							//福利号
#define PLATFORM_STAT_RECHARGE UT_REDIS_STRINGFY(:st_recharge)							//充值
#define PLATFORM_STAT_CONSUME UT_REDIS_STRINGFY(:st_cousume)							//消费
#define PLATFORM_STAT_CREATE_CHAR UT_REDIS_STRINGFY(:st_create_char)					//创角
#define PLATFORM_STAT_LOGIN_TIMES UT_REDIS_STRINGFY(:st_login_times)					//登录人次
#define PLATFORM_STAT_LOGIN_COUNT UT_REDIS_STRINGFY(:st_login_count)					//登录人数
#define PLATFORM_STAT_ONLINE_COUNT UT_REDIS_STRINGFY(:st_online_count)					//在线
///////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_360_RECHARGE UT_REDIS_STRINGFY(:360_operation_recharge:user:)					// 充值 360
#define PLATFORM_360_RECHARGE_ORDER_ID UT_REDIS_STRINGFY(:360_operation_recharge:orderId)					// 充值订单记录
///////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_AUTH_GAME_GLOBALSHOP UT_REDIS_STRINGFY(:auth_gameglobalshop)			//商店购买全局限制信息
//#define PLATFORM_AUTH_GAME_MYSTICALSHOP UT_REDIS_STRINGFY(:auth_gamemysticalshop)		//神秘商店购买信息
#define SERVER_SURVIVE_CHECK_KEY UT_REDIS_STRINGFY(:subscribe_server_survive_check)		//服务器异常邮件系统
#define SERVER_LOCAL_GUILD_WAR_KEY UT_REDIS_STRINGFY(:war_local_guild_save)				//服务器圣火令数据
#define CHAT_FORBID_WORD_LIST UT_REDIS_STRINGFY(:chat_forbid_word_list)					//聊天屏蔽字列表
#define CROSS_REALM_WAR_TIME UT_REDIS_STRINGFY(:cross_realm_war_time)					//屠龙刀相关时间
///////////////////////////////////////////////////////////////////////////////////////
#define MONITOR_CHAT UT_REDIS_STRINGFY(:monitor_chat)	
///////////////////////////////////////////////////////////////////////////////////////
#define UT_REDIS_GLOBAL_LADDER_SEASON UT_REDIS_STRINGFY(:global_ladder_season)	
#define UT_REDIS_GLOBAL_LADDER_ID UT_REDIS_STRINGFY(:global_ladder_id)	
#define UT_REDIS_GLOBAL_LADDER_TIME UT_REDIS_STRINGFY(:global_ladder_time)	
#define UT_REDIS_GLOBAL_LADDER_RECORD UT_REDIS_STRINGFY(:global_ladder_record)
///////////////////////////////////////////////////////////////////////////////////////
#define UT_REDIS_BONFIREPARTY_ITEMTIMES UT_REDIS_STRINGFY(:bonfireparty_itemtimes)	
#define UT_REDIS_BONFIREPARTY_ENDTIME UT_REDIS_STRINGFY(:bonfireparty_entime)
////////////////////////////////////////////////////////////////////////////////////////
namespace Utility
{
	enum
	{
		UT_REDIS_PEER_FG_MASTER = 0,
	};
	typedef boost::shared_ptr<redispp::Connection> redisconn_ptr;
	///////////////////////////////////////////////////////////////
	class Redis_Peer
	{
		friend class Redis_Handler;
	public:
		Redis_Peer(): m_uiFlag(0), m_sPwd(DEFAULT_REDIS_AUTH_PASS) {}
		int32_t Init(const std::string& _host, const std::string& _port, const std::string& _pwd)
		{
			m_sHost = _host;
			m_sPort = _port;
			if (!_pwd.empty())
			{
				m_sPwd = _pwd;
			}
			return 0;
		}

		int32_t Unit(void) { return _disConnect();}
		inline redisconn_ptr GetConection(bool _refresh=true, bool _heartbeat=true)
		{
			if (m_conn)
			{
				int32_t ret = 0;
				try
				{
					if (_refresh){
						_refreshInfo();
					}
					else if(_heartbeat){
						ping();
					}
				}
				catch (std::exception& e)
				{
					char buf[512];
					sprintf(buf, "Refresh Info[%s][%s] PWD[%s] Failed[%s]", m_sHost.c_str(), m_sPort.c_str(), m_sPwd.c_str(), e.what());
					m_sError = buf;
					ret = -1;
				}
				catch (...)
				{
					char buf[512];
					sprintf(buf, "Refresh Info[%s][%s] PWD[%s] Failed[%s]", m_sHost.c_str(), m_sPort.c_str(), m_sPwd.c_str(), "some uncatched err happened!");
					m_sError = buf;
					ret = -1;
				}
				if (0 != ret)
				{
					_disConnect();
				}
				return m_conn;
			}
			else
			{
				_doConnect();
				return m_conn;
			}
		}

		inline bool CheckFlag(uint32_t uiMask, uint32_t uiFlag) const { return (m_uiFlag & uiMask) == (uiMask & uiFlag); }
		inline bool IsMaster(void) const { return (m_uiFlag & (1 << UT_REDIS_PEER_FG_MASTER)) > 0; }
		inline void SetMaster(bool flag)
		{
			if (flag)
				m_uiFlag |= (1 << UT_REDIS_PEER_FG_MASTER);
			else
				m_uiFlag &= ~(1 << UT_REDIS_PEER_FG_MASTER);
		}

		inline void SetLastError(const std::string& err) { m_sError = err; }
		const std::string& GetLastError(void) {	return m_sError; }
	protected:
		int32_t _doConnect(void)
		{
			_disConnect();
			if (m_sHost.empty() || m_sPort.empty())
			{
				return -1;
			}
			int32_t ret = 0;
			try
			{
				m_conn.reset(new redispp::Connection(m_sHost, m_sPort, m_sPwd));
				_refreshInfo();
			}
			catch (std::exception& e)
			{
				char buf[512];
				sprintf(buf, "Connect To[%s][%s] PWD[%s] Failed[%s]", m_sHost.c_str(), m_sPort.c_str(), m_sPwd.c_str(), e.what());
				m_sError = buf;
				ret = -1;
			}
			catch (...)
			{
				char buf[512];
				sprintf(buf, "Connect To[%s][%s] PWD[%s] Failed[%s]", m_sHost.c_str(), m_sPort.c_str(), m_sPwd.c_str(), "some uncatched err happened!");
				m_sError = buf;
				ret = -1;
			}
			if (0 != ret)
			{
				_disConnect();
			}
			return ret;
		}

		int32_t _disConnect(void)
		{
			if (m_conn)
			{
				try
				{
					m_conn->quit();
				}
				catch (std::exception* e)
				{
					m_sError = e->what();
					m_conn.reset();
					return -1;
				}
				catch (...)
				{
					m_sError = "some uncatched err happened while disconnect!";
					m_conn.reset();
					return -1;
				}
				m_conn.reset();
			}
			m_uiFlag = 0;
			return 0;
		}

		void _refreshInfo(void)
		{
			if (m_conn)
			{
				std::string _info = m_conn->info();
				size_t _pos = _info.find("role:");
				bool bMaster = false;
				if (_pos != std::string::npos && _info.compare(_pos + 5, 6, "master") == 0)
				{
					bMaster = true;
				}
				SetMaster(bMaster);
			}
		}

		void ping(void)
		{
			if (m_conn)
			{
				redispp::VoidReply kPong = m_conn->ping();
				if (kPong.result().compare("PONG") != 0)
				{
					std::cout << "redis ping fail" << std::endl;
				}
			}
		}
	protected:
		uint32_t m_uiFlag;
		std::string m_sHost;
		std::string m_sPort;
		std::string m_sPwd;

		redisconn_ptr m_conn;
		std::string m_sError;
	};
	typedef std::vector<Redis_Peer> Redis_Peer_Vector;
	///////////////////////////////////////////////////////////////
	class Redis_Handler
	{
	public:
		Redis_Handler(void): m_iCurIndex(0), m_sError("NO Redis Peer Available!") {}
		inline int32_t AddPeer(const std::string& _host, const std::string& _port, const std::string& _pwd)
		{
			m_kPeers.resize(m_kPeers.size() + 1);
			return m_kPeers.back().Init(_host, _port, _pwd);
		}

		int32_t Init(void)
		{
			Redis_Peer_Vector::iterator it = m_kPeers.begin();
			for (m_iCurIndex = 0; it != m_kPeers.end(); ++it, ++m_iCurIndex)
			{
				if (it->GetConection(true) && it->IsMaster())
				{
					return 0;
				}
			}
			return -1;
		}

		int32_t Unit(void)
		{
			Redis_Peer_Vector::iterator it = m_kPeers.begin();
			for (; it != m_kPeers.end(); ++it)
			{
				it->Unit();
			}
			m_kPeers.clear();
			m_iCurIndex = 0;
			return 0;
		}

		redisconn_ptr GetConection(bool _heartbeat=true)
		{
			bool _refresh = false;
			if (m_iCurIndex < 0 || m_iCurIndex >= (int32_t)m_kPeers.size())
			{
				m_iCurIndex = 0;
				_refresh = true;
			}
			while (m_iCurIndex < (int32_t)m_kPeers.size())
			{
				redisconn_ptr pkConn = m_kPeers[m_iCurIndex].GetConection(_refresh, _heartbeat);
				if (pkConn)
				{
					if (m_kPeers[m_iCurIndex].IsMaster())
					{
						return pkConn;
					}
				}
				else
				{
					m_sError = m_kPeers[m_iCurIndex].GetLastError();
				}
				++m_iCurIndex;
				_refresh = true;
			}
			return redisconn_ptr();
		}

		void Disconnect(void)
		{
			if (m_iCurIndex >= 0 && m_iCurIndex < (int32_t)m_kPeers.size())
			{
				m_kPeers[m_iCurIndex]._disConnect();
			}
		}

		const void SetLastError(const std::string& err)
		{
			if (m_iCurIndex >= 0 && m_iCurIndex < (int32_t)m_kPeers.size())
			{
				m_kPeers[m_iCurIndex].SetLastError(err);
			}
			m_sError = err;
		}
		inline const std::string& GetLastError(void) { return m_sError;}
	protected:
		Redis_Peer_Vector m_kPeers;
		int32_t	m_iCurIndex;
	private:
		std::string m_sError;
	};
	///////////////////////////////////////////////////////////////
	inline std::string RedisGlobalPrefix(int iUnionID)
	{
		std::stringstream tmp;
		tmp << UT_REDIS_STRINGFY(UT_REDIS_KEY_SEPATATOR) << iUnionID;
		return tmp.str();
	}

	inline std::string RedisGroupPrefix(int iUnionID, int iGroupID)
	{
		std::stringstream tmp;
		tmp << UT_REDIS_STRINGFY(UT_REDIS_KEY_SEPATATOR) << iUnionID << UT_REDIS_STRINGFY(UT_REDIS_KEY_SEPATATOR) << iGroupID;
		return tmp.str();
	}

	inline std::string RedisServerPrefix(int iUnionID, int iGroupID, int iServerID)
	{
		std::stringstream tmp;
		tmp << UT_REDIS_STRINGFY(UT_REDIS_KEY_SEPATATOR) << iUnionID << "_" << iGroupID << UT_REDIS_STRINGFY(UT_REDIS_KEY_SEPATATOR) << iServerID;
		return tmp.str();
	}
}

#define UT_REDIS_CMD_BEGIN(ret, handler) {\
	Utility::redisconn_ptr pkRedisConn = handler->GetConection(); \
	if(pkRedisConn) \
	{\
		try {\

#define UT_REDIS_CMD_END(ret, handler) \
			ret = 0;\
		}catch(redispp::NullReplyException&){\
			handler->SetLastError("null reply!");\
			ret = -1;\
		}catch(std::exception& e){\
			handler->SetLastError(e.what());\
			handler->Disconnect();\
			ret = -1;\
		}catch(...){\
			handler->SetLastError("some uncatched err happened!");\
			handler->Disconnect();\
			ret = -1;\
		}\
	}\
	else{\
		ret = -1;\
	}}

#define UT_REDIS_CMD(ret, handler, cmd, ...) \
	UT_REDIS_CMD_BEGIN(ret, handler) \
		pkRedisConn->cmd(__VA_ARGS__); \
	UT_REDIS_CMD_END(ret, handler)

#define UT_REDIS_CMD_WITH_REPLY(ret, handler, reply, cmd, ...) \
	UT_REDIS_CMD_BEGIN(ret, handler) \
		reply = pkRedisConn->cmd(__VA_ARGS__); \
	UT_REDIS_CMD_END(ret, handler)


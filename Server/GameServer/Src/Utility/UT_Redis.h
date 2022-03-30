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
#define UT_REDIS_KEY_CHAR_CACHE UT_REDIS_STRINGFY(:char_cache)					//��һ���
#define UT_REDIS_KEY_AVAIL_COUNTS UT_REDIS_STRINGFY(:avail_counts)				//���ü���
#define UT_REDIS_KEY_USER_SVRIDS UT_REDIS_STRINGFY(:user_svrids)				//�������
#define UT_REDIS_KEY_GSNODE_SARVER_INFO UT_REDIS_STRINGFY(:gsnode_serverinfo) //gs�ڵ��������Ϣ
#define UT_REDIS_KEY_MERGE_SARVER_INFO UT_REDIS_STRINGFY(:mergeserver_serverinfo) //mergeServerId �Ϸ� serverid
#define UT_REDIS_KEY_TRANSCRIBE_FIGHT UT_REDIS_STRINGFY(:transcribe_fight) //transcribe_fight
#define UT_REDIS_KEY_CODE_DEFINES UT_REDIS_STRINGFY(:code_defines)				//������
#define UT_REDIS_KEY_CODE_AUTOID UT_REDIS_STRINGFY(:code_autoid)				//acitvacatio
#define UT_REDIS_KEY_GM_ACCOUNTS UT_REDIS_STRINGFY(:gm_accounts)				//GM�˺� --- �ѷ���
#define UT_REDIS_KEY_GM_CMD_REQS UT_REDIS_STRINGFY(:gm_reqs)					//gm����
#define UT_REDIS_KEY_GM_CMD_RESPS UT_REDIS_STRINGFY(:gm_resps)					//gm�ظ�
#define UT_REDIS_KEY_HTTP_REQS UT_REDIS_STRINGFY(:http_reqs)					//http����
#define UT_REDIS_KEY_HTTP_RESPS UT_REDIS_STRINGFY(:http_resps)					//http�ظ�
#define UT_REDIS_KEY_GM_CHARS UT_REDIS_STRINGFY(:gm_chars)						//gm��ɫ�б� --- ����ʹ��
#define UT_REDIS_KEY_GM_CMD_SORT UT_REDIS_STRINGFY(:gm_infosort)				//�����������
#define UT_REDIS_KEY_CODE_USED_TIMES UT_REDIS_STRINGFY(:code_used_times)		//���������
#define UT_REDIS_KEY_SHOT_ACTIVITY_ENTRY UT_REDIS_STRINGFY(:shot_act_entry)		//���
#define UT_REDIS_KEY_SHOT_ACTIVITY_RANK UT_REDIS_STRINGFY(:shot_act_rank)		//��� group
#define UT_REDIS_KEY_SHOT_REDIS_RANK UT_REDIS_STRINGFY(:shot_redis_rank)		//��� redis
#define UT_REDIS_KEY_STORY_ACTIVITY_RANK UT_REDIS_STRINGFY(:story_act_rank)		//��� gamestory
#define UT_REDIS_KEY_SHOT_ACTIVITY_GROUP_PREFIX UT_REDIS_STRINGFY(:group)		//���
#define UT_REDIS_KEY_SOCIETY_PREFIX UT_REDIS_STRINGFY(:society)					//society
#define UT_REDIS_KEY_ACTIVITY_TREASURE_PUT UT_REDIS_STRINGFY(:treasure_put)				//treasure put
#define UT_REDIS_KEY_ACTIVITY_TREASURE_OCCUPY UT_REDIS_STRINGFY(:treasure_occupy)		//treasure occupy
#define UT_REDIS_KEY_ACTIVITY_TREASURE_CATAGORY UT_REDIS_STRINGFY(:treasure_catagory)	//treasure occupy
#define UT_REDIS_KEY_GAME_REPORT UT_REDIS_STRINGFY(:game_report)						//��Ϸ�ϱ�
#define UT_REDIS_KEY_ITEM_FLOPS UT_REDIS_STRINGFY(:item_flops)							//�ֽ⿪��
#define UT_REDIS_KEY_DAILY_DATA_REPORT UT_REDIS_STRINGFY(:daily_data_report)			//�����ձ�
#define UT_REDIS_KEY_ONLINE_RECORD UT_REDIS_STRINGFY(:online_record)					//��������ͳ��
#define UT_REDIS_KEY_HOURLY_MAX_ONLINE_IP UT_REDIS_STRINGFY(:hourly_max_online_ip)		//ÿСʱ�������IP���������ͳ��
#define UT_REDIS_KEY_PREDAY_ACTIVE UT_REDIS_STRINGFY(:preday_active)					//ǰһ��Ļ�Ծ����
#define UT_REDIS_KEY_UPDATE_BULLETIN UT_REDIS_STRINGFY(:update_bulletin)				//���¹���
#define UT_REDIS_KEY_HANDLE_ASSET UT_REDIS_STRINGFY(:handle_asset)						//��������ʲ�
#define UT_REDIS_KEY_ACTIVITY_SNOWMAN_SCORE UT_REDIS_STRINGFY(:act_snowman_score)		//ѩ�˿񻶻����
#define UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD UT_REDIS_STRINGFY(:act_limit_reward:)				//ȫ���޴λ
#define UT_REDIS_KEY_ACTIVITY_LIMIT_REWARD_SUB_B UT_REDIS_STRINGFY(:act_limit_reward_sub_b:)				//�������԰�ȫ���޴λ
#define UT_REDIS_KEY_BOSS_FIRST_KILL UT_REDIS_STRINGFY(:act_server_first_kill_boss)					//��ɱ
#define UT_REDIS_KEY_ACT_SUBORDINATE_TIME UT_REDIS_STRINGFY(:act_sub_time)//��ͨ�������ʱ��ʱ��
#define UT_REDIS_KEY_ACT_SUBORDINATE_TIME_PLUS UT_REDIS_STRINGFY(:act_sub_time_plus)//���״������ʱ��ʱ��
#define UT_REDIS_KEY_ACT_SUB_PLAYER_INFO UT_REDIS_STRINGFY(:act_sub_player_info)//��ͨ�������ʱ����ҵĻ�������
#define UT_REDIS_KEY_ACT_SUB_PLAYER_INFO_PLUS UT_REDIS_STRINGFY(:act_sub_player_info_plus)//�߼��������ʱ����ҵĻ�������
#define UT_REDIS_KEY_ACT_SHOP_START_TIME UT_REDIS_STRINGFY(:act_shop_start_time)//�̳ǵĿ���ʱ��
#define UT_REDIS_KEY_ACT_SEVENDAY_TRIAL UT_REDIS_STRINGFY(:act_sevenday_trial)//����������Ľ���ʱ��
#define UT_REDIS_KEY_INVESTMENTS UT_REDIS_STRINGFY(:investments)//��ȯ�̳�Ͷ�ʼƻ��1
#define UT_REDIS_KEY_INVESTMENTS_CARD_STARTTIME UT_REDIS_STRINGFY(:investment_card_starttime)//Ͷ�ʿ������ʱ��
#define UT_REDIS_KEY_INVESTMENTS_CARD_ENDTIME UT_REDIS_STRINGFY(:investment_card_endtime)//Ͷ�ʿ������ʱ��
#define UT_REDIS_KEY_CURRENT_REFRESH_REWARDTASK_TIME UT_REDIS_STRINGFY(:current_refresh_rewardtask_time)//���ͻ��ǰˢ��ʱ��

#define UT_REDIS_KEY_RANK_STATE UT_REDIS_STRINGFY(:rank_state)//���а�񵥽���״̬

#define UT_REDIS_KEY_YIZHONGJIANGLIN_STORY UT_REDIS_STRINGFY(:yizhongjianglin_story)//���ֽ���Ҫ������storyID
#define UT_REDIS_KEY_COMBAT_INFO UT_REDIS_STRINGFY(:combat_info)//�˺���Ϣ
#define UT_REDIS_KEY_DAMAGE_RANK UT_REDIS_STRINGFY(:damage_player_rank)//����˺�����
#define UT_REDIS_KEY_BATTLE_INFO UT_REDIS_STRINGFY(:battle_player_info)//�˺�������ͳ����Ϣ
#define UT_REDIS_KEY_STORT_END_TIME UT_REDIS_STRINGFY(:story_endtime)//�����ʱ��

#define UT_REDIS_KEY_GLOBAL_STORT_END_TIME UT_REDIS_STRINGFY(:global_story_endtime)//global�ڵ��Ľ���ʱ��
#define UT_REDIS_KEY_GUILD_DAMAGE_RANK UT_REDIS_STRINGFY(:guild_damage_rank)//�����˺�����
#define UT_REDIS_KEY_GUILD_DAMAGE_INFO UT_REDIS_STRINGFY(:guild_damage_info)//�����˺���Ϣ


#define UT_REDIS_KEY_GUILDMAPBOSS_DAMAGE_RANK UT_REDIS_STRINGFY(:guildmapboss_damage_rank)//�����ؾ���boss���˺�
#define UT_REDIS_KEY_GUILDMAPBOSS_PLAYER_INFO UT_REDIS_STRINGFY(:guildmapboss_player_info)//�����ؾ������Ϣ
#define UT_REDIS_KEY_GUILDMAPBOSS_STATE_INFO UT_REDIS_STRINGFY(:guildmapboss_state_info)//�����ؾ�״̬��Ϣ

#define UT_REDIS_KEY_ANSWER_STORY_END_TIME UT_REDIS_STRINGFY(:answer_story_endtime)//�����Ľ���ʱ��
#define UT_REDIS_KEY_ANSWER_STORY_TOPIC UT_REDIS_STRINGFY(:answer_story_topic)//������ѡ��
#define UT_REDIS_KEY_ANSWER_ACCOUNT_POINT UT_REDIS_STRINGFY(:answer_account_point)//��������ҵĻ���
#define UT_REDIS_KEY_ANSWER_GUILD_POINT UT_REDIS_STRINGFY(:answer_guild_point)//��������Ļ���
#define UT_REDIS_KEY_ANSWER_GUILD_INFO UT_REDIS_STRINGFY(:answer_guild_info)//�����������Ϣ
#define UT_REDIS_KEY_ANSWER_PLAYER_INFO UT_REDIS_STRINGFY(:answer_player_info)//��������ҵ���Ϣ
#define UT_REDIS_KEY_ANSWER_CHECKAWARD_TIME UT_REDIS_STRINGFY(:answer_checkaward_time)//���⽱�����ɷ�ʱ��
#define UT_REDIS_KEY_ANSWER_CLOSE UT_REDIS_STRINGFY(:answer_close)//���⽱�����ɷ�
//����ս  ���˰�  4���׶� + ������ 
#define UT_REDIS_KEY_PERSONAL_RANK_POINT_BY_TYPE UT_REDIS_STRINGFY(:player_score)//����ս� ����ҵĻ���
#define UT_REDIS_KEY_PERSONAL_RANK_PLAYER_INFO_BY_TYPE UT_REDIS_STRINGFY(:player_info)//����ս� ����ҵ���Ϣ
//����ս  �����  4���׶� + ������ 
#define UT_REDIS_KEY_GUILD_RANK_POINT_BY_TYPE UT_REDIS_STRINGFY(:guild_score)//����ս�  ������Ļ���
#define UT_REDIS_KEY_GUILD_RANK_PLAYER_INFO_BY_TYPE UT_REDIS_STRINGFY(:guild_info)//����ս�  ��������Ϣ
// ����ս ��������
#define UT_REDIS_KEY_WATCH_TOWER_INFO UT_REDIS_STRINGFY(:watch_tower_info)//���������Ϣ
#define UT_REDIS_KEY_BOSS_DAMAGE_INFO UT_REDIS_STRINGFY(:boss_damage_info)//boss�˺���Ϣ

#define UT_REDIS_KEY_GUILD_FIGHT_DEFEND_INFO UT_REDIS_STRINGFY(:guild_defend_info)//���ַ�����Ϣ

//
#define UT_REDIS_KEY_SOLODULE_ROBOT UT_REDIS_STRINGFY(:solo_dule_robot)//�����������˵�����
#define UT_REDIS_KEY_SOLOINIT_ROBOT UT_REDIS_STRINGFY(:solo_init_robot)//�����������˳�ʼkey
#define UT_REDIS_KEY_SOLO_OPEN_TIME UT_REDIS_STRINGFY(:solo_open_time)//��սʱ��
#define UT_REDIS_KEY_SOLO_HALLOFFAME UT_REDIS_STRINGFY(:solo_halloffame)//������
#define UT_REDIS_KEY_SUMMITARENA_GROUPSTAGE UT_REDIS_STRINGFY(:summit_arena_group)//�۷徺��������
#define UT_REDIS_KEY_SUMMITARENA_HALLOFFAME UT_REDIS_STRINGFY(:summit_halloffame)//������
#define UT_REDIS_KEY_SKYARENA_ZONESEASON UT_REDIS_STRINGFY(:skyarena_zoneseason)//��������
#define UT_REDIS_KEY_SUMMITARENA_OPEN_TIME UT_REDIS_STRINGFY(:summit_open_time)//��սʱ��
#define UT_REDIS_KEY_SINGLE_SUMMITARENA_HALLOFFAME UT_REDIS_STRINGFY(:single_summit_halloffame)//��ս��������
#define UT_REDIS_KEY_SUB_INFO_VEHICLE UT_REDIS_STRINGFY(:sub_info_vehicle)//�������԰񣬻�������
#define UT_REDIS_KEY_SUB_INFO_VEHICLE_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_vehicle_player_info)//�������԰񣬻��ޣ��������
#define UT_REDIS_KEY_SUB_INFO_WING UT_REDIS_STRINGFY(:sub_info_wing)//�������԰񣬱�������
#define UT_REDIS_KEY_SUB_INFO_WING_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_wing_player_info)//�������԰񣬱��Σ��������
#define UT_REDIS_KEY_SUB_INFO_DEVAEYE UT_REDIS_STRINGFY(:sub_info_devaeye)//�������԰��ȳ�����
#define UT_REDIS_KEY_SUB_INFO_DEVAEYE_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_devaeye_player_info)//�������԰��ȳ裬�������
#define UT_REDIS_KEY_SUB_INFO_ALL_RUNE_ITEM UT_REDIS_STRINGFY(:sub_info_rune_item)//�������԰����б����ܵȼ�
#define UT_REDIS_KEY_SUB_INFO_ALL_RUNE_ITEM_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_rune_item_player_info)//�������԰����б����ܵȼ����������
#define UT_REDIS_KEY_SUB_INFO_COMMANDER_LEVEL UT_REDIS_STRINGFY(:sub_info_commander_level)//�������԰�ָ�ӹٵȼ�
#define UT_REDIS_KEY_SUB_INFO_COMMANDER_LEVEL_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_commander_level_player_info)//�������԰�ָ�ӹٵȼ����������
#define UT_REDIS_KEY_SUB_INFO_COMMANDER_TITLE UT_REDIS_STRINGFY(:sub_info_commander_title)//�������԰�ָ�ӹ�ͷ��
#define UT_REDIS_KEY_SUB_INFO_COMMANDER_TITLE_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_commander_title_player_info)//�������԰�ָ�ӹ�ͷ�Σ��������
#define UT_REDIS_KEY_SUB_INFO_CONTROLLER_EQUIP_COMBATSCORE UT_REDIS_STRINGFY(:sub_info_controller_equip_combatscore)//�������԰�ָ�ӹ�װ��ս��
#define UT_REDIS_KEY_SUB_INFO_CONTROLLER_EQUIP_COMBATSCORE_PLAYER_INFO UT_REDIS_STRINGFY(:sub_info_controller_equip_combatscore_player_info)//�������԰�ָ�ӹ�װ��ս�����������

#define UT_REDIS_KEY_BESTGUILD_ENDTIME UT_REDIS_STRINGFY(:bestguild_endtime)//��ǿ�������ʱ��
#define UT_REDIS_KEY_BESTGUILD_ACTIVITY_ENDTIME UT_REDIS_STRINGFY(:bestguild_activity_endtime)//��ǿ����׶λ����ʱ��
#define UT_REDIS_KEY_REGULARBOSS_KILLER_INFO UT_REDIS_STRINGFY(:regularboss_killer_info) //�ƽ��ؾ���ɱ����Ϣ
#define UT_REDIS_KEY_REWARD_TASK_ENDTIME UT_REDIS_STRINGFY(:reward_task_endtime)//��������Ľ���ʱ��
#define UT_REDIS_KEY_SUBB_RANK_RECORD UT_REDIS_STRINGFY(:subb_rank_record)//�Ѿ������˿������԰��groupid

#define UT_REDIS_KEY_SUMMITARENA_GUESS UT_REDIS_STRINGFY(:summit_arena_guess)//�۷徺���������ߵľ�����Ϣ
#define UT_REDIS_KEY_SUMMITARENA_GUESS_COUNT UT_REDIS_STRINGFY(:summit_arena_guess_count)//�۷徺�����������ߵ�ѹ������

#define UT_REDIS_KEY_MONEYPOINT_YIJITUXI UT_REDIS_STRINGFY(:yijituxi_moneypoint) //�ż�ͻϮ���ְ�
#define UT_REDIS_KEY_MONEYPOINT_YIJITUXI_PLAYER_INFO UT_REDIS_STRINGFY(:yijituxi_moneypoint_player_info)//�ż�ͻϮ����ҵ���Ϣ
#define UT_REDIS_KEY_YIJITUXI_STORY_END_TIME UT_REDIS_STRINGFY(:yijituxi_story_endtime)//�ż�ͻϮ��Ľ���ʱ��

#define UT_REDIS_KEY_WATCHTOWER_TIME_INFO UT_REDIS_STRINGFY(:watch_tower_time_info)//����ս�ʱ��

#define UT_REDIS_KEY_RANK_START_END_TIME UT_REDIS_STRINGFY(:rank_start_endtime_)//���а� �洢 ��ʼ������ʱ��

#define UT_REDIS_KEY_OFFLINE_ANSWER_RESULT UT_REDIS_STRINGFY(:offline_answer_result)//������ҵĴ�����������

///////////////////////////////////////////////////////////////////////////////////////
#define UT_REDIS_KEY_RUMOR_INFO UT_REDIS_STRINGFY(:rumor)						//�㲥ģ��
#define UT_REDIS_KEY_RUMOR_DIRTY UT_REDIS_STRINGFY(:rumor_dirty)				//�㲥ģ��
#define UT_REDIS_KEY_INIT_VERSION UT_REDIS_STRINGFY(:init_version)				//��������ʼ���ű��汾��
#define UT_REDIS_KEY_SERVER_RANK_TOPS UT_REDIS_STRINGFY(:rank_tops)				//���а����
#define UT_REDIS_KEY_TOP10_AVG_POWER UT_REDIS_STRINGFY(:top10_avg_power)		//ǰʮƽ��ս��
#define UT_REDIS_KEY_GLOBAL_VALUES UT_REDIS_STRINGFY(:global_values)			//ȫ������
#define UT_REDIS_KEY_CUS_RES_VERSION UT_REDIS_STRINGFY(:cus_res_version)		//������reload���
#define UT_REDIS_KEY_PLAYER_ONLINE_SNAPSHOT UT_REDIS_STRINGFY(:player_online_snapshot)			//������߿������� ������������˺�
///////////////////////////////////////////////////////////////////////////////////////
#define DEFAULT_REDIS_AUTH_PASS "jzwldev"
#define PLATFORM_AUTH_NUM 100													//��½key��ϣ����
#define PLATFORM_INNERUIDKEY UT_REDIS_STRINGFY(:auth_userinnerindex)			//��½����id
#define PLATFORM_AUTH_KEY UT_REDIS_STRINGFY(:auth_game2inner)					//�û���¼��jzext cmm֮�佻��
///////////////////////////////////////////////////////////////////////////////////////
#define BALANCE_INFO_KEY UT_REDIS_STRINGFY(:cmm_balanceinfo)					//���ؾ���key
#define GLOBAL_ACTIVE_SERVER_KEY UT_REDIS_STRINGFY(:activeserverlist_flag)		//��Ծ������ publish
#define GLOBAL_ACTIVE_SERVER_SKEY UT_REDIS_STRINGFY(:activeserverlist_save_flag)//��Ծ������ save
#define GLOBAL_GROUP_SERVER_KEY UT_REDIS_STRINGFY(:serverlistgroup_flag)		//���������� publish
#define GLOBAL_GROUP_SERVER_SKEY UT_REDIS_STRINGFY(:serverlistgroup_save_flag)	//���������� save
#define GLOBAL_GROUP_SERVER_BKEY UT_REDIS_STRINGFY(:serverlistgroup_base_flag)	//���������� ԭʼ����
///////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_AUTH_GAME_ACCOUNTCHARID UT_REDIS_STRINGFY(:auth_accountcharid)			//����˺ź�charid����
#define PLATFORM_AUTH_GAME_USERINFOKEY UT_REDIS_STRINGFY(:auth_gameuserinfo)			//������� ������������˺�
#define PLATFORM_AUTH_GAME_NAMEINFOKEY UT_REDIS_STRINGFY(:auth_gamenameinfo)			//����˺źͽ�ɫ������
#define PLATFORM_AUTH_GAME_CHAR2USERKEY UT_REDIS_STRINGFY(:auth_gamechar2user)			//����˺źͽ�ɫid����
#define PLATFORM_AUTH_GAME_PLATFORMINFOKEY UT_REDIS_STRINGFY(:auth_gameplatfrominfo)	//���ƽ̨��Ϣ�����ƻ����
#define PLATFORM_AUTH_GAME_USERLOGINKEY UT_REDIS_STRINGFY(:auth_userloginplat)			//��ҵ�½ƽ̨��Ϣ
#define PLATFORM_HTTP_PUBLISH_GAME_KEY UT_REDIS_STRINGFY(:pub_gamepublish2http)			//http�ϱ�
///////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_BONUS_POOL_KEY UT_REDIS_STRINGFY(:bonus_pool)							//�����
#define PLATFORM_BONUS_ROLE UT_REDIS_STRINGFY(:bonus_role_set)							//������
#define PLATFORM_STAT_RECHARGE UT_REDIS_STRINGFY(:st_recharge)							//��ֵ
#define PLATFORM_STAT_CONSUME UT_REDIS_STRINGFY(:st_cousume)							//����
#define PLATFORM_STAT_CREATE_CHAR UT_REDIS_STRINGFY(:st_create_char)					//����
#define PLATFORM_STAT_LOGIN_TIMES UT_REDIS_STRINGFY(:st_login_times)					//��¼�˴�
#define PLATFORM_STAT_LOGIN_COUNT UT_REDIS_STRINGFY(:st_login_count)					//��¼����
#define PLATFORM_STAT_ONLINE_COUNT UT_REDIS_STRINGFY(:st_online_count)					//����
///////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_360_RECHARGE UT_REDIS_STRINGFY(:360_operation_recharge:user:)					// ��ֵ 360
#define PLATFORM_360_RECHARGE_ORDER_ID UT_REDIS_STRINGFY(:360_operation_recharge:orderId)					// ��ֵ������¼
///////////////////////////////////////////////////////////////////////////////////////
#define PLATFORM_AUTH_GAME_GLOBALSHOP UT_REDIS_STRINGFY(:auth_gameglobalshop)			//�̵깺��ȫ��������Ϣ
//#define PLATFORM_AUTH_GAME_MYSTICALSHOP UT_REDIS_STRINGFY(:auth_gamemysticalshop)		//�����̵깺����Ϣ
#define SERVER_SURVIVE_CHECK_KEY UT_REDIS_STRINGFY(:subscribe_server_survive_check)		//�������쳣�ʼ�ϵͳ
#define SERVER_LOCAL_GUILD_WAR_KEY UT_REDIS_STRINGFY(:war_local_guild_save)				//������ʥ��������
#define CHAT_FORBID_WORD_LIST UT_REDIS_STRINGFY(:chat_forbid_word_list)					//�����������б�
#define CROSS_REALM_WAR_TIME UT_REDIS_STRINGFY(:cross_realm_war_time)					//���������ʱ��
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


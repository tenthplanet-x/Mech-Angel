#ifndef _UT_OPERATE_LOG_H
#define _UT_OPERATE_LOG_H
//Windows环境下 日志生成工具
#include "UT_Log.h"

using namespace  Utility;
namespace GameLogic{
	class LC_PlayerBase;
}

class LC_ServerPlayer;

namespace OPERATELOG
{
	//埋点统计lv1大类
	enum LogTypeLv1
	{
		FUBEN_LOGTYPE   = 1,   //副本玩法
		SYSTEM_LOGTYPE  = 2,   //系统玩法
		ACTIVITY_LOGTYPE = 3,   //活动玩法
		SHOP_LOGTYPE    = 4,   //商城玩法
	};

	//运营统计lv2中类
	enum LogTypeLv2
	{
		FUBEN1_OPERATE				    = 1,         //无尽之塔
		FUBEN2_OPERATE                  = 2,         //首领试炼
		FUBEN3_OPERATE                  = 3,         //遗迹突袭
		FUBEN4_OPERATE                  = 4,         //遗迹寻宝
		FUBEN5_OPERATE                  = 5,         //神域地宫
		FUBEN6_OPERATE                  = 6,         //浩劫秘境
		FUBEN7_OPERATE                  = 7,         //重返考场
		FUBEN8_OPERATE                  = 8,         //经验副本
		FUBEN9_OPERATE                  = 9,         //荣光远征
		FUBEN10_OPERATE                 = 10,        //元素试炼
		FUBEN11_OPERATE                 = 11,        //竞技场
		FUBEN12_OPERATE                 = 12,        //命运困境
		FUBEN13_OPERATE                 = 13,        //首领禁地
		FUBEN14_OPERATE                 = 14,        //首领要塞
		FUBEN15_OPERATE                 = 15,        //异种侵袭
		FUBEN16_OPERATE                 = 16,        //幻境试炼
		FUBEN17_OPERATE					= 17,        //工会秘境
		FUBEN18_OPERATE					= 18,        //荒野战场
		FUBEN19_OPERATE					= 19,			//异种降临个人奖励
		FUBEN20_OPERATE					= 20,			//异种降临公会奖励

		SPECIALTRAIN_OPERATE			= 201,       //特训
		TITLES_OPERATE					= 202,       //头衔
		MINGHAO_OPERATE					= 203,       //名号
		USERPACK_OPERATE				= 204,       //背包
		BAOJU_OPERATE					= 205,		 //宝具
		ITEMFORGE_OPERATE				= 206,       //装备锻造
		PETS_OPERATE					= 207,       //萌宠
		MOUNTS_OPERATE					= 208,       //幻兽
		BEISHI_OPERATE					= 209,       //背饰
		WANJU_OPERATE					= 210,       //玩具
		COLLECTSUIT_OPERATE				= 211,		 //套装
		WEIPAITASK_OPERATE              = 212,       //委派任务
		BUDDY_OPERATE					= 213,		 //英雄操作
		WENZHANG_OPERATE                = 214,       //纹章操作
		SHENGHEN_OPERATE                = 215,       //圣痕操作
		GUILD_OPERATE					= 216,       //工会
		GUILDTASK_OPERATE               = 217,       //工会任务
		MAINTASK_OPERATE                = 218,       //主线任务
		DAILYTASK_OPERATE               = 219,       //日常任务
		LEADERTASK_OPERATE              = 220,       //每日任务
		ACHIEVEMENT_OPERATE             = 221,       //成就
		COLLECTION_OPERATE              = 222,       //收藏
		PRIVILEGE_OPERATE               = 223,       //特权
		VIP_OPERATE						= 224,       //vip
		SIGNIN_OPERATE				    = 225,       //签到
		WEIDUANSIGNIN_OPERATE		    = 226,       //微端签到
		ONLINE_OPERATE					= 227,       //在线奖励
		ZHAOHUIRESOURCE_OPERATE		    = 228,       //资源找回
		FRIENDS_OPERATE					= 229,       //好友
		CDK_OPERATE						= 230,       //cdk
		OFFLINE_OPERATE					= 231,       //离线收益
		NOTICE_OPERATE                  = 232,       //公告奖励
		MAIL_OPERATE                    = 233,       //邮件奖励
		HONOURCHALLENG_OPERATE          = 234,       //荣耀挑战
		OFFERAWARD_OPERATE              = 235,       //悬赏
		WABAO_OPERATE                   = 236,       //挖宝
		RELIVE_OPERATE                  = 237,       //复活
		CHAT_OPERATE                    = 238,       //聊天操作
		HANGUPFUBEN_OPERATE             = 239,       //挂机荒原
		PICTUREFRAME_OPERATE			= 240,       //头像框
		LEARNSKILL_OPERATE			    = 241,       //学习技能
		VIPTIMES_OPERATE                = 242,       //vip额外次数
		BUDDYTASK_OPERATE               = 243,       //英雄主题任务
		SELL_OPERATE					= 244,       //交易
		VIPPRIVILEGECARDTIMES_OPERATE                = 245,       //VIPPrivilegeCard额外次数

		ACTIVITY1_OPERATE			    = 401,		 //七日登录
		ACTIVITY2_OPERATE				= 402,		 //首充
		ACTIVITY3_OPERATE				= 403,		 //幸运转盘
		ACTIVITY4_OPERATE				= 404,		 //极品白拿
		ACTIVITY5_OPERATE				= 405,		 //绝版特卖
		ACTIVITY6_OPERATE				= 406,		 //绝版礼包
		ACTIVITY7_OPERATE				= 407,		 //BOSS首杀
		ACTIVITY8_OPERATE				= 408,		 //宝具召唤
		ACTIVITY9_OPERATE				= 409,		 //神秘召唤
		ACTIVITY10_OPERATE				= 410,		 //超值礼包
		ACTIVITY11_OPERATE				= 411,		 //新手礼包
		ACTIVITY12_OPERATE				= 412,		 //每日特惠
		ACTIVITY13_OPERATE			    = 413,		 //登录计划
		ACTIVITY14_OPERATE				= 414,		 //升级狂欢
		ACTIVITY15_OPERATE				= 415,		 //白银特权
		ACTIVITY16_OPERATE				= 416,		 //黄金特权
		ACTIVITY17_OPERATE				= 417,		 //特权领取
		ACTIVITY18_OPERATE				= 418,		 //排行榜
		ACTIVITY19_OPERATE				= 419,		 //机甲出击
		ACTIVITY20_OPERATE				= 420,		 //邂逅豪礼
		ACTIVITY21_OPERATE				= 421,		 //萝莉馈赠
		ACTIVITY22_OPERATE				= 422,		 //极品兑换
		ACTIVITY23_OPERATE				= 423,		 //神选魔盒
		ACTIVITY24_OPERATE				= 424,		 //平台累计登录天数
		ACTIVITY25_OPERATE				= 425,		 //角色登录
		ACTIVITY26_OPERATE				= 426,		 //防沉迷
		ACTIVITY27_OPERATE				= 427,		 //极品白拿返利

		DIAMOND_SHOP					= 601,       //钻石商店
		LIMITS_SHOP,							     //限购商店
		GUILD_SHOP,									 //工会商店
		BRAVERY_SHOP,                                //勇武商店
		SKIN_SHOP ,			                         //皮肤商店
		JP_SHOP,		                             //精魄商店
		NORMAL_SHOP,                                 //普通商店
	};


	//埋点统计lv3小类，使用时作区分
	enum LogTypeLv3
	{
		OPERATE_LOGTYPE1 = 1, //
		OPERATE_LOGTYPE2 = 2, //
		OPERATE_LOGTYPE3 = 3, //
		OPERATE_LOGTYPE4 = 4, //
		OPERATE_LOGTYPE5 = 5, //
		OPERATE_LOGTYPE6 = 6, //
		OPERATE_LOGTYPE7 = 7, //
		OPERATE_LOGTYPE8 = 8, //
		OPERATE_LOGTYPE9 = 9, //
		OPERATE_LOGTYPE10 = 10, //
		OPERATE_LOGTYPE11 = 11, //
		OPERATE_LOGTYPE12 = 12, //
		OPERATE_LOGTYPE13 = 13, //
	};

	//埋点统计lv3小类，使用时作区分
	enum OperateType
	{
		CONSUME_OPERATE = 1, //消耗
		GET_OPERATE     = 2, //获得
		BASE_OPERATE    = 3, //基本操作
		LOGIN_OPERATE   = 4,//登陆
	};

	struct BitField
	{
		unsigned int a:8;  //最低位;
		unsigned int b:8;
		unsigned int c:8;  //最高位;
	};
	union BitUnField
	{
		struct BitField bf;
		unsigned int nValue;
	};
	
	//////////////////////////////////////////////////////////////////////////
	//输出到OSS文件的log
	//////////////////////////////////////////////////////////////////////////
	class UT_OperateLogStreamFileOSS : public UT_LogStreamFile, public Utility::UT_Singleton<UT_OperateLogStreamFileOSS>
	{
	public:
		//如果选择bAppend = true，则使用追加方式填写文件内容
		UT_OperateLogStreamFileOSS(const wchar_t * pwPathName,const wchar_t * pwLogDir,int serverid = 0);
		virtual ~UT_OperateLogStreamFileOSS();

		virtual void LogMessage(const StringType& rkMessage) { }

		//写入操作日志数据
		void RecordLogMessage(char* pCLogMsg);

	protected:
		void MakeDirectory(const wchar_t * strLogDir);

		char * GetDateTimeStr(time_t *mytime);

		char * GetCurDateTimeStr(void);

		wchar_t * GetLogFileName();
	protected:
		wchar_t m_szFilePath[127];
		int m_nLogPathState;       //日志目录创建成功标识
		std::wstring m_strPathName;
		int m_serverId;
	};

	void GetOperateLogPrefix(char* sText, LC_ServerPlayer* pOwner);

	/* 玩家道具日志 gamesrvid | userid | username | accountname | LvType| CMD| itemid | itemcount | RemainCount | PARAMS|...
	*Param GameLogic::LC_ActorBase*：玩家指针
	*Param int：nOperateLogCode，序列化操作码
	*Param int：nType 类型：消耗:1, 获得 ：2
	*Param int：nItemId，道具id
	*Param int：nBeforCount，当前剩余数量
	*Param int：nItemCount，道具数量
	*Param vector<int>: 额外参数
	*/
	void RecordItemOperateLog(GameLogic::LC_PlayerBase* pkPlayer, int nOperateLogCode, int nType, int nItemId, int nItemCount, int nBeforCount, const std::vector<int>&Params = std::vector<int>());
	

	/* 玩家操作日志 gamesrvid | userid | username | accountname | LvType | PARAMS|...
	*Param GameLogic::LC_ActorBase*：玩家指针
	*Param int：nOperateLogCode，序列化操作码
	*Param vector<int>: 额外参数
	*/
	void RecordOperateLog(GameLogic::LC_PlayerBase* pkPlayer, int nType,  int nOperateLogCode = 0,const std::vector<uint64_t>&Params = std::vector<uint64_t>());
	
	/* 统计玩家操作日志 gamesrvid | userid | username | accountname | LvType| CMD| nCashType | nCount | RemainCount | PARAMS|...
	*Param GameLogic::LC_ActorBase*：玩家指针
	*Param int：nOperateLogCode，序列化操作码
	*Param int：nType 类型：消耗:1, 获得 ：2
	*Param int：nCashType，类型
	*Param int：nCount，数量
	*Param int：nBeforCount，当前剩余数量
	*Param vector<int>: 额外参数
	*/
	void RecordCashOperateLog(GameLogic::LC_PlayerBase* pkPlayer, int nOperateLogCode, int nType, int nCashType, int nCount, int nBeforCount,const std::vector<int>&Params = std::vector<int>());
	
}


#endif //_UT_OPERATE_LOG_H

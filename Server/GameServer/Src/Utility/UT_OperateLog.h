#ifndef _UT_OPERATE_LOG_H
#define _UT_OPERATE_LOG_H
//Windows������ ��־���ɹ���
#include "UT_Log.h"

using namespace  Utility;
namespace GameLogic{
	class LC_PlayerBase;
}

class LC_ServerPlayer;

namespace OPERATELOG
{
	//���ͳ��lv1����
	enum LogTypeLv1
	{
		FUBEN_LOGTYPE   = 1,   //�����淨
		SYSTEM_LOGTYPE  = 2,   //ϵͳ�淨
		ACTIVITY_LOGTYPE = 3,   //��淨
		SHOP_LOGTYPE    = 4,   //�̳��淨
	};

	//��Ӫͳ��lv2����
	enum LogTypeLv2
	{
		FUBEN1_OPERATE				    = 1,         //�޾�֮��
		FUBEN2_OPERATE                  = 2,         //��������
		FUBEN3_OPERATE                  = 3,         //�ż�ͻϮ
		FUBEN4_OPERATE                  = 4,         //�ż�Ѱ��
		FUBEN5_OPERATE                  = 5,         //����ع�
		FUBEN6_OPERATE                  = 6,         //�ƽ��ؾ�
		FUBEN7_OPERATE                  = 7,         //�ط�����
		FUBEN8_OPERATE                  = 8,         //���鸱��
		FUBEN9_OPERATE                  = 9,         //�ٹ�Զ��
		FUBEN10_OPERATE                 = 10,        //Ԫ������
		FUBEN11_OPERATE                 = 11,        //������
		FUBEN12_OPERATE                 = 12,        //��������
		FUBEN13_OPERATE                 = 13,        //�������
		FUBEN14_OPERATE                 = 14,        //����Ҫ��
		FUBEN15_OPERATE                 = 15,        //������Ϯ
		FUBEN16_OPERATE                 = 16,        //�þ�����
		FUBEN17_OPERATE					= 17,        //�����ؾ�
		FUBEN18_OPERATE					= 18,        //��Ұս��
		FUBEN19_OPERATE					= 19,			//���ֽ��ٸ��˽���
		FUBEN20_OPERATE					= 20,			//���ֽ��ٹ��ά��

		SPECIALTRAIN_OPERATE			= 201,       //��ѵ
		TITLES_OPERATE					= 202,       //ͷ��
		MINGHAO_OPERATE					= 203,       //����
		USERPACK_OPERATE				= 204,       //����
		BAOJU_OPERATE					= 205,		 //����
		ITEMFORGE_OPERATE				= 206,       //װ������
		PETS_OPERATE					= 207,       //�ȳ�
		MOUNTS_OPERATE					= 208,       //����
		BEISHI_OPERATE					= 209,       //����
		WANJU_OPERATE					= 210,       //���
		COLLECTSUIT_OPERATE				= 211,		 //��װ
		WEIPAITASK_OPERATE              = 212,       //ί������
		BUDDY_OPERATE					= 213,		 //Ӣ�۲���
		WENZHANG_OPERATE                = 214,       //���²���
		SHENGHEN_OPERATE                = 215,       //ʥ�۲���
		GUILD_OPERATE					= 216,       //����
		GUILDTASK_OPERATE               = 217,       //��������
		MAINTASK_OPERATE                = 218,       //��������
		DAILYTASK_OPERATE               = 219,       //�ճ�����
		LEADERTASK_OPERATE              = 220,       //ÿ������
		ACHIEVEMENT_OPERATE             = 221,       //�ɾ�
		COLLECTION_OPERATE              = 222,       //�ղ�
		PRIVILEGE_OPERATE               = 223,       //��Ȩ
		VIP_OPERATE						= 224,       //vip
		SIGNIN_OPERATE				    = 225,       //ǩ��
		WEIDUANSIGNIN_OPERATE		    = 226,       //΢��ǩ��
		ONLINE_OPERATE					= 227,       //���߽���
		ZHAOHUIRESOURCE_OPERATE		    = 228,       //��Դ�һ�
		FRIENDS_OPERATE					= 229,       //����
		CDK_OPERATE						= 230,       //cdk
		OFFLINE_OPERATE					= 231,       //��������
		NOTICE_OPERATE                  = 232,       //���潱��
		MAIL_OPERATE                    = 233,       //�ʼ�����
		HONOURCHALLENG_OPERATE          = 234,       //��ҫ��ս
		OFFERAWARD_OPERATE              = 235,       //����
		WABAO_OPERATE                   = 236,       //�ڱ�
		RELIVE_OPERATE                  = 237,       //����
		CHAT_OPERATE                    = 238,       //�������
		HANGUPFUBEN_OPERATE             = 239,       //�һ���ԭ
		PICTUREFRAME_OPERATE			= 240,       //ͷ���
		LEARNSKILL_OPERATE			    = 241,       //ѧϰ����
		VIPTIMES_OPERATE                = 242,       //vip�������
		BUDDYTASK_OPERATE               = 243,       //Ӣ����������
		SELL_OPERATE					= 244,       //����
		VIPPRIVILEGECARDTIMES_OPERATE                = 245,       //VIPPrivilegeCard�������

		ACTIVITY1_OPERATE			    = 401,		 //���յ�¼
		ACTIVITY2_OPERATE				= 402,		 //�׳�
		ACTIVITY3_OPERATE				= 403,		 //����ת��
		ACTIVITY4_OPERATE				= 404,		 //��Ʒ����
		ACTIVITY5_OPERATE				= 405,		 //��������
		ACTIVITY6_OPERATE				= 406,		 //�������
		ACTIVITY7_OPERATE				= 407,		 //BOSS��ɱ
		ACTIVITY8_OPERATE				= 408,		 //�����ٻ�
		ACTIVITY9_OPERATE				= 409,		 //�����ٻ�
		ACTIVITY10_OPERATE				= 410,		 //��ֵ���
		ACTIVITY11_OPERATE				= 411,		 //�������
		ACTIVITY12_OPERATE				= 412,		 //ÿ���ػ�
		ACTIVITY13_OPERATE			    = 413,		 //��¼�ƻ�
		ACTIVITY14_OPERATE				= 414,		 //������
		ACTIVITY15_OPERATE				= 415,		 //������Ȩ
		ACTIVITY16_OPERATE				= 416,		 //�ƽ���Ȩ
		ACTIVITY17_OPERATE				= 417,		 //��Ȩ��ȡ
		ACTIVITY18_OPERATE				= 418,		 //���а�
		ACTIVITY19_OPERATE				= 419,		 //���׳���
		ACTIVITY20_OPERATE				= 420,		 //���˺���
		ACTIVITY21_OPERATE				= 421,		 //��������
		ACTIVITY22_OPERATE				= 422,		 //��Ʒ�һ�
		ACTIVITY23_OPERATE				= 423,		 //��ѡħ��
		ACTIVITY24_OPERATE				= 424,		 //ƽ̨�ۼƵ�¼����
		ACTIVITY25_OPERATE				= 425,		 //��ɫ��¼
		ACTIVITY26_OPERATE				= 426,		 //������
		ACTIVITY27_OPERATE				= 427,		 //��Ʒ���÷���

		DIAMOND_SHOP					= 601,       //��ʯ�̵�
		LIMITS_SHOP,							     //�޹��̵�
		GUILD_SHOP,									 //�����̵�
		BRAVERY_SHOP,                                //�����̵�
		SKIN_SHOP ,			                         //Ƥ���̵�
		JP_SHOP,		                             //�����̵�
		NORMAL_SHOP,                                 //��ͨ�̵�
	};


	//���ͳ��lv3С�࣬ʹ��ʱ������
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

	//���ͳ��lv3С�࣬ʹ��ʱ������
	enum OperateType
	{
		CONSUME_OPERATE = 1, //����
		GET_OPERATE     = 2, //���
		BASE_OPERATE    = 3, //��������
		LOGIN_OPERATE   = 4,//��½
	};

	struct BitField
	{
		unsigned int a:8;  //���λ;
		unsigned int b:8;
		unsigned int c:8;  //���λ;
	};
	union BitUnField
	{
		struct BitField bf;
		unsigned int nValue;
	};
	
	//////////////////////////////////////////////////////////////////////////
	//�����OSS�ļ���log
	//////////////////////////////////////////////////////////////////////////
	class UT_OperateLogStreamFileOSS : public UT_LogStreamFile, public Utility::UT_Singleton<UT_OperateLogStreamFileOSS>
	{
	public:
		//���ѡ��bAppend = true����ʹ��׷�ӷ�ʽ��д�ļ�����
		UT_OperateLogStreamFileOSS(const wchar_t * pwPathName,const wchar_t * pwLogDir,int serverid = 0);
		virtual ~UT_OperateLogStreamFileOSS();

		virtual void LogMessage(const StringType& rkMessage) { }

		//д�������־����
		void RecordLogMessage(char* pCLogMsg);

	protected:
		void MakeDirectory(const wchar_t * strLogDir);

		char * GetDateTimeStr(time_t *mytime);

		char * GetCurDateTimeStr(void);

		wchar_t * GetLogFileName();
	protected:
		wchar_t m_szFilePath[127];
		int m_nLogPathState;       //��־Ŀ¼�����ɹ���ʶ
		std::wstring m_strPathName;
		int m_serverId;
	};

	void GetOperateLogPrefix(char* sText, LC_ServerPlayer* pOwner);

	/* ��ҵ�����־ gamesrvid | userid | username | accountname | LvType| CMD| itemid | itemcount | RemainCount | PARAMS|...
	*Param GameLogic::LC_ActorBase*�����ָ��
	*Param int��nOperateLogCode�����л�������
	*Param int��nType ���ͣ�����:1, ��� ��2
	*Param int��nItemId������id
	*Param int��nBeforCount����ǰʣ������
	*Param int��nItemCount����������
	*Param vector<int>: �������
	*/
	void RecordItemOperateLog(GameLogic::LC_PlayerBase* pkPlayer, int nOperateLogCode, int nType, int nItemId, int nItemCount, int nBeforCount, const std::vector<int>&Params = std::vector<int>());
	

	/* ��Ҳ�����־ gamesrvid | userid | username | accountname | LvType | PARAMS|...
	*Param GameLogic::LC_ActorBase*�����ָ��
	*Param int��nOperateLogCode�����л�������
	*Param vector<int>: �������
	*/
	void RecordOperateLog(GameLogic::LC_PlayerBase* pkPlayer, int nType,  int nOperateLogCode = 0,const std::vector<uint64_t>&Params = std::vector<uint64_t>());
	
	/* ͳ����Ҳ�����־ gamesrvid | userid | username | accountname | LvType| CMD| nCashType | nCount | RemainCount | PARAMS|...
	*Param GameLogic::LC_ActorBase*�����ָ��
	*Param int��nOperateLogCode�����л�������
	*Param int��nType ���ͣ�����:1, ��� ��2
	*Param int��nCashType������
	*Param int��nCount������
	*Param int��nBeforCount����ǰʣ������
	*Param vector<int>: �������
	*/
	void RecordCashOperateLog(GameLogic::LC_PlayerBase* pkPlayer, int nOperateLogCode, int nType, int nCashType, int nCount, int nBeforCount,const std::vector<int>&Params = std::vector<int>());
	
}


#endif //_UT_OPERATE_LOG_H

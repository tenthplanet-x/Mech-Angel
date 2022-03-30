#pragma once
//客户端邮件管理
#include "LC_MailAsset.h"

namespace GameLogic
{
	class LC_ClientMailManager :	public Utility::UT_Singleton<LC_ClientMailManager>,
									public Memory::MM_BaseObject
	{
	public:
		LC_ClientMailManager();
		virtual ~LC_ClientMailManager();
		DECL_RTTI_NAME( LC_ClientMailManager );

		// 客户端请求接收自己的邮件资产
		ResultType RequestMailAsset();
		static void PostGetGlobalMail(Utility::timer_id_type timerID);

		// 信件内容检查，不包括邮资：因为需要弹出框提示，所以分步检查.
		ResultType RequestWriteMailContentCheck(const StringType& strReceiverName, 
												const StringType& strSubject, const StringType& strContent,
												MoneyType iGold, MoneyType iSilver,	MoneyType iCopper, bool bIsPay);
		// 邮资检查
		ResultType RequestWriteMailPostageCheck(MoneyType iGold, MoneyType iSilver, MoneyType iCopper);

		// 执行写邮件请求
		void RequestWriteMailExcute(const StringType& strReceiverName, 
									const StringType& strSubject, const StringType& strContent,
									MoneyType iGold, MoneyType iSilver, MoneyType iCopper, bool bIsPay);
		

		// （收信）客户端请求取物品附件
		ResultType RequestItemAffix(int iIndex);

		// （收信）客户端请求取所有物品附件
		ResultType RequestAllItemAffix();
		ResultType RequestAllItemAffixByMailIndex(int iIndex);
		int		   GetItemAffixCount(int iIndex);

		// （收信）客户端请求获取金钱附件
		ResultType RequestCashAffix();

		// 客户端请求退信
		ResultType RequestUntreadMail();
		ResultType RequestUntreadMailByMailID(uint64_t MailID);

		// 客户端请求打开邮件
		ResultType RequestOpenMail();

		// 客户端请求关闭打开的信件
		ResultType RequestCloseMail();

		// （写信）客户端请求加入邮件附件
		ResultType RequestAddAffix(int32_t iPos, int32_t iPackType, int32_t iIndex);

		// （写信）客户端请求删除邮箱附件
		ResultType RequestDeleletAffix(int32_t iPos);
		
		// 清空邮件资产, 重置邮件
		void Reset();
		
		// 收到邮件头
		void ReceiveMailSimpleInfo(const Protocol::MG_RLT_MailAsset& rkMailInfo);

		// 收到一封邮件
		void ReceiveMail(const Protocol::PS_MailInfo& kMail);

		// 删除一封邮件
		void DeleteMail(uint64_t lMailID);

		// 成功获取一个物品附件
		void GetItemAffix(uint64_t iMailID, int iIndex);

		// 成功获取一个金钱附件
		void GetCashAffix(uint64_t iMailID);

		// 获取邮件资产
		LC_MailAsset* GetMailAsset();

		// 设置当前选中邮件
		void SetSelectedMailIndex(int iIndex);

		// 获取当前选中邮件
		int GetSelectedMailIndex();

		// 发信完成
		void CompleteWriteMail();

		// 清楚写信装备标志
		void ClearWriteFlag();

		bool GetSendingFlag();
		bool GetCompleteFlag();

		void SetSendingFlag(bool bFlag);
		void SetCompleteFlag(bool bFlag);

		// 获取物品附件信息
		LC_ClientItemAffixInfo* GetClientItemAffixInfo();

		// 名字输入框焦点失去后检查玩家名字是否存在
		void CheckReceiverName(const StringType& strName);
		
		// 获取邮件的邮资
		MoneyType ComputeMailCharge(MoneyType iGold, MoneyType iSilver, MoneyType iCopper);
		StringType GetMailCharge(MoneyType iGold, MoneyType iSilver, MoneyType iCopper);

	public:
		bool IsExistPendingName(const StringType& strName);
		void AddPendingName(const StringType& strName);
		void ErasePendingName(const StringType& strName);

		ResultType SendPendingMail();

		void OnLoginRequest()
		{
			if(!m_bIsLoginRequest)
			{
				RequestMailAsset();
				m_bIsLoginRequest = true;
			}
		}
	public:
		void	SetMailHintFlag(bool bHint);
		bool	GetMailHintFlag();

		void	SetMailboxFullFlag(bool bFlag);
		bool	GetMailboxFullFlag();
		void	SetIsReadingMailFlag(bool bFlag);
		bool	GetIsReadingMailFlag();

	private:

	private:
		LC_MailAsset			m_kMailAsset;		
		int						m_iSelectedIndex;
		uint64_t				m_iSelectedMailID;

		LC_ClientItemAffixInfo	m_kClientItemAffixInfo;

		bool					m_bIsREQMailAsset;
		bool					m_bIsSending;
		bool					m_bIsComplete;

		//为了登陆而发
		bool					m_bIsLoginRequest;
		int64_t					m_iPreRequestTimeStamp;

		TStringList				m_kPendingTargetList;		// 写信目标列表

		StringType				m_strPendingReceiverName;
		StringType				m_strPendingSubject;
		StringType				m_strPendingContent;
		MoneyType				m_iPendingGold;
		MoneyType				m_iPendingSilver;
		MoneyType				m_iPendingCopper;
		bool					m_bIsPay;
		bool					m_bMailHint;
		bool					m_bMailboxFull;
		bool					m_bIsReadingMail;
	};
}

#ifndef _NW_CLIENT_NET_MANAGER_H
#define _NW_CLIENT_NET_MANAGER_H
//监听和发送网络消息。


#define LOGIN_TIMEOUT 15				//登录超时阈值，以秒为单位

class NW_ClientNetManager : public NetWork::NW_NetManagerBase
{
public:
	NW_ClientNetManager();
	~NW_ClientNetManager();

	virtual bool Init();
	virtual void UnInit();
	virtual void Update();

	void StartCheckLogin(bool bCheck);

private:
	bool m_bCheckLogin;				//启动检查登录超时
	float m_fLoginStartTime;		//开始连接服务器的时间
};

#endif
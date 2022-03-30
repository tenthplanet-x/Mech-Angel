//视频播放器，只能以1x速播放
#pragma once

#include <bink.h>
#include <radbase.h>

class UT_VideoPlayer : public Memory::MM_BaseObject
{
public:
	//////////////////////////////////////////////////////////////////////////

	UT_VideoPlayer(void);
	~UT_VideoPlayer(void);

	bool Play(const char *cpFilename, unsigned int uLoopCount = 1);
	void End(void);
	bool Pause(bool b);

	//////////////////////////////////////////////////////////////////////////

	bool BeginUpdate(float fTime, float fDeltaTime);
	void EndUpdate(void);

	//////////////////////////////////////////////////////////////////////////

	void CopyImage(void* destBuf, S32 destpitch);
	void CopyImageRect(void* destBuf, 
		S32 destpitch, U32 destheight, U32 destx, U32 desty,
		U32 src_x, U32 src_y, U32 src_w, U32 src_h
		);

public:
	//////////////////////////////////////////////////////////////////////////

	int	getWidth(void) const;
	int	getHeight(void) const;
	const StringType & getFilename(void) const { return m_kFilename; }

	bool	isValid(void) const { return m_hBink != 0; }
	bool	isEnd(void) const { return m_uCurLoop >= m_uLoopCount; }
	bool	isCurLoopEnd(void) const { return m_bCurLoopEnd; }

private:
	//////////////////////////////////////////////////////////////////////////

	HBINK			m_hBink;	//bink句柄
    int				m_bPause;	//暂停标志位
	StringType		m_kFilename;//视频文件名

	unsigned int	m_uCurLoop;	//当前循环计数
	unsigned int	m_uLoopCount;//循环次数
	bool			m_bCurLoopEnd;	//当前循环是否结束
	bool			m_bInernalUpdateing;	//内部标志位
};

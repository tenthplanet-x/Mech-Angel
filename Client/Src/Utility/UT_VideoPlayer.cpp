#include "ClientPCH.h"
#include "UT_VideoPlayer.h"


UT_VideoPlayer::UT_VideoPlayer(void)
: m_hBink(0)
{
	this->End();
}

UT_VideoPlayer::~UT_VideoPlayer(void)
{
	this->End();
}

bool UT_VideoPlayer::Play(const char *cpFilename, unsigned int uLoopCount)
{
	T_ASSERT(m_hBink == 0);

	m_kFilename = cpFilename;
	m_uCurLoop = 0;
	m_uLoopCount = uLoopCount;
	m_bCurLoopEnd = false;

	//////////////////////////////////////////////////////////////////////////
	BinkSoundUseDirectSound( 0 );
	m_hBink = BinkOpen(cpFilename, 0);	//打开视频

	if (!m_hBink)		return false;
	
	return true;
}

void UT_VideoPlayer::End(void)
{
	if (m_hBink)
	{
		BinkClose(m_hBink);	//关闭视频
		m_hBink = 0;
	}

	m_bPause = 0;
	m_kFilename = "";
	m_uCurLoop = 0;
	m_uLoopCount = 0;
	m_bCurLoopEnd = false;
	m_bInernalUpdateing = false;
}

//////////////////////////////////////////////////////////////////////////

bool UT_VideoPlayer::Pause(bool b)
{
	if (!m_hBink)		return false;

	if (m_bPause != (int)b)
	{
		m_bPause = BinkPause(m_hBink, (int)b);	//暂停
	}

	return m_bPause != 0;
}

bool UT_VideoPlayer::BeginUpdate(float fTime, float fDeltaTime)
{
	if (!m_hBink)		return false;
	if (m_bInernalUpdateing)		return false;

	if ( !BinkWait( m_hBink ) )
	{
		m_bCurLoopEnd = false;
		BinkDoFrame( m_hBink );		//解码

		while ( BinkShouldSkip( m_hBink ) )
		{
			BinkNextFrame( m_hBink );
			BinkDoFrame( m_hBink );
		}

		if (m_hBink->FrameNum == m_hBink->Frames )	//循环计数
		{
			m_bCurLoopEnd = true;
			m_uCurLoop++;
		}

		m_bInernalUpdateing = true;
	}

	return m_bInernalUpdateing;
}

void UT_VideoPlayer::EndUpdate(void)
{
	if (!m_hBink)		return;
	if (!m_bInernalUpdateing)		return;

	if (m_uCurLoop < m_uLoopCount)
	{
		BinkNextFrame( m_hBink );	//移动到下一帧，如果在最后一帧调用此函数，会移动到前面
	}
	else
	{
		this->End();
	}

	m_bInernalUpdateing = false;
}

//////////////////////////////////////////////////////////////////////////

void UT_VideoPlayer::CopyImage(void* destBuf, S32 destpitch)
{
	if (!m_hBink)		return;
	if (!m_bInernalUpdateing)		return;

	if (destBuf)
	{
		BinkCopyToBuffer( m_hBink,		//拷贝到外部缓冲区，每像素4字节
			destBuf,
			destpitch,
			m_hBink->Height,
			0,0,
			BINKCOPYALL | BINKSURFACE32 );
	}
}

void UT_VideoPlayer::CopyImageRect(void* destBuf, 
				   S32 destpitch, U32 destheight, U32 destx, U32 desty,
				   U32 src_x, U32 src_y, U32 src_w, U32 src_h
				   )
{
	if (!m_hBink)		return;
	if (!m_bInernalUpdateing)		return;

	if (destBuf)
	{
		BinkCopyToBufferRect( m_hBink,		//拷贝到外部缓冲区，每像素4字节
			destBuf,
			destpitch, destheight, destx, desty,
			src_x, src_y, src_w, src_h,
			BINKCOPYALL | BINKSURFACE32 );
	}
}


//////////////////////////////////////////////////////////////////////////

int	UT_VideoPlayer::getWidth(void) const
{
	if (!m_hBink)		return 0;

	return m_hBink->Width;
}

int	UT_VideoPlayer::getHeight(void) const
{
	if (!m_hBink)		return 0;

	return m_hBink->Height;
}

//////////////////////////////////////////////////////////////////////////


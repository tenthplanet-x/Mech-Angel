#ifndef		__AYU_INPUT_DEFINE_H__
#define		__AYU_INPUT_DEFINE_H__


#include <memory.h>


/************************************************************************/

//按键不会超过256个

struct	MACHINE_WORD_LEN	//机器字长
{
	enum
	{
		_VALUE = sizeof(unsigned int) << 3,		//32
		_DIVIDE_BY256 = 256 / _VALUE,			//8
		_POW2_MASK = _VALUE - 1,				//31
		_POW2_EXPONENT = 5,						//5 = log2(32)
	};
};


/************************************************************************/


struct KEY_BIT_STATE
{
	enum
	{
		PAD_SHIFT = 0,
		TRG_SHIFT = 1,
		RLS_SHIFT = 2,

		PAD = 1 << PAD_SHIFT,
		TRG = 1 << TRG_SHIFT,
		RLS = 1 << RLS_SHIFT,
	};
};


/************************************************************************/


struct CKeyboardInputData 
{
public:

	CKeyboardInputData( void )
	{
		reset();
	}

	void	reset( void )
	{
		::memset(m_oldPad, 0, sizeof(m_oldPad));
		::memset(m_KeyPad, 0, sizeof(m_KeyPad));
		::memset(m_KeyTrg, 0, sizeof(m_KeyTrg));
		::memset(m_KeyRls, 0, sizeof(m_KeyRls));
	}

	void	update( void )
	{
		for (int i = 0; i < sizeof(m_KeyPad)/sizeof(m_KeyPad[0]); ++i)
		{
			m_KeyTrg[i] = (~m_oldPad[i]) & m_KeyPad[i];
			m_KeyRls[i] = m_oldPad[i] & (~m_KeyPad[i]);
		}

		::memcpy(m_oldPad, m_KeyPad, sizeof(m_oldPad));
	}

public:

	void		injectKeyDown( int dik )	//dik in [0, 256), no check here
	{
		m_KeyPad[dik >> MACHINE_WORD_LEN::_POW2_EXPONENT] |= 1 << (dik & MACHINE_WORD_LEN::_POW2_MASK);
	}

	void		injectKeyUp( int dik )		//dik in [0, 256), no check here
	{
		m_KeyPad[dik >> MACHINE_WORD_LEN::_POW2_EXPONENT] &= ~(1 << (dik & MACHINE_WORD_LEN::_POW2_MASK));
	}

public:

	unsigned	int		getKeyState( int dik ) const	//dik in [0, 256), no check here
	{
		unsigned	int		selector = dik >> MACHINE_WORD_LEN::_POW2_EXPONENT;
		unsigned	int		shift = dik & MACHINE_WORD_LEN::_POW2_MASK;
		unsigned	int		key = 0;

		key |= ((m_KeyPad[selector] >> shift) & 1) << KEY_BIT_STATE::PAD_SHIFT;
		key |= ((m_KeyTrg[selector] >> shift) & 1) << KEY_BIT_STATE::TRG_SHIFT;
		key |= ((m_KeyRls[selector] >> shift) & 1) << KEY_BIT_STATE::RLS_SHIFT;

		return	key;
	}

private:

	//dik in [0, 256), no check here
	unsigned	int		getKeyPad( int dik ) const
	{
		return	(m_KeyPad[dik >> MACHINE_WORD_LEN::_POW2_EXPONENT]) & (1 << (dik & MACHINE_WORD_LEN::_POW2_MASK));
	}
	unsigned	int		getKeyTrg( int dik ) const
	{
		return	(m_KeyTrg[dik >> MACHINE_WORD_LEN::_POW2_EXPONENT]) & (1 << (dik & MACHINE_WORD_LEN::_POW2_MASK));
	}
	unsigned	int		getKeyRls( int dik ) const
	{
		return	(m_KeyRls[dik >> MACHINE_WORD_LEN::_POW2_EXPONENT]) & (1 << (dik & MACHINE_WORD_LEN::_POW2_MASK));
	}

private:

	unsigned	int		m_oldPad[MACHINE_WORD_LEN::_DIVIDE_BY256];
	unsigned	int		m_KeyPad[MACHINE_WORD_LEN::_DIVIDE_BY256];
	unsigned	int		m_KeyTrg[MACHINE_WORD_LEN::_DIVIDE_BY256];
	unsigned	int		m_KeyRls[MACHINE_WORD_LEN::_DIVIDE_BY256];
};


/************************************************************************/


struct CMouseInputData
{
public:

	enum
	{
		LBUTTON_SHIFT = 0,
		RBUTTON_SHIFT = 1,
		MBUTTON_SHIFT = 2,

		LBUTTON = 1 << LBUTTON_SHIFT,
		RBUTTON = 1 << RBUTTON_SHIFT,
		MBUTTON = 1 << MBUTTON_SHIFT,
		BUTTON_MASK = LBUTTON | RBUTTON | MBUTTON,
	};

public:

	CMouseInputData( void )
	{
		reset();
	}

	void	reset( void )
	{
		m_oldPad = 0;
		m_KeyPad = 0;
		m_KeyTrg = 0;
		m_KeyRls = 0;

		m_Dx = 0;
		m_Dy = 0;
		m_Dz = 0;
	}

	void	update( void )
	{
		m_KeyTrg = (~m_oldPad) & m_KeyPad;
		m_KeyRls = m_oldPad & (~m_KeyPad);

		m_oldPad = m_KeyPad;
	}

public:

	void		injectLButtonDown( void )	{ m_KeyPad |= LBUTTON;  }
	void		injectLButtonUp( void )		{ m_KeyPad &= ~LBUTTON; }

	void		injectRButtonDown( void )	{ m_KeyPad |= RBUTTON;  }
	void		injectRButtonUp( void )		{ m_KeyPad &= ~RBUTTON; }

	void		injectMButtonDown( void )	{ m_KeyPad |= MBUTTON;  }
	void		injectMButtonUp( void )		{ m_KeyPad &= ~MBUTTON; }

	//idx in [0, 3), no check here
	void		injectKeyDown( int idx )	{ m_KeyPad |= 1 << idx;    }		//idx: 0, left; 1, right; 2, middle
	void		injectKeyUp( int idx ) 		{ m_KeyPad &= ~(1 << idx); }

	void		injectMove(int dx, int dy, int dz) { m_Dx = dx; m_Dy = dy; m_Dz = dz; }

public:

	unsigned	int		getLButtonState( void ) const { return this->getKeyStateImpl(LBUTTON_SHIFT, LBUTTON); }
	unsigned	int		getRButtonState( void ) const { return this->getKeyStateImpl(RBUTTON_SHIFT, RBUTTON); }
	unsigned	int		getMButtonState( void ) const { return this->getKeyStateImpl(MBUTTON_SHIFT, MBUTTON); }

	//idx in [0, 3), no check here
	unsigned	int		getKeyState( int idx ) const  { return this->getKeyStateImpl(idx, 1 << idx); }

	int		getDx( void ) const { return m_Dx; }
	int		getDy( void ) const { return m_Dy; }
	int		getDz( void ) const { return m_Dz; }

private:

	unsigned	int		getKeyStateImpl( int button_shift, int button_mask ) const
	{
		unsigned	int		pad = (m_KeyPad & button_mask) >> button_shift;
		unsigned	int		trg = (m_KeyTrg & button_mask) >> button_shift;
		unsigned	int		rls = (m_KeyRls & button_mask) >> button_shift;

		pad <<= KEY_BIT_STATE::PAD_SHIFT;
		trg <<= KEY_BIT_STATE::TRG_SHIFT;
		rls <<= KEY_BIT_STATE::RLS_SHIFT;

		return	pad | trg | rls;
	}

private:

	unsigned	int		m_oldPad;
	unsigned	int		m_KeyPad;
	unsigned	int		m_KeyTrg;
	unsigned	int		m_KeyRls;
				int		m_Dx;
				int		m_Dy;
				int		m_Dz;
};


#endif	//__AYU_INPUT_DEFINE_H__
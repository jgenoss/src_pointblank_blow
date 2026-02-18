#pragma once

#ifdef	DOMI_UTIL

class CCoolTimeGuage
{
private:
	bool			m_bEnable;

	REAL32			m_rRate;
	REAL32			m_rRemainCool;
	REAL32			m_rSetCool;

public:
	CCoolTimeGuage(void);
	~CCoolTimeGuage(void);

	bool	OnUpdate( REAL32 tm );
	bool	OnUpdate( REAL32 tm, REAL32 rRate);

	void	SetClose(void);
	void	SetCoolTime( REAL32 rTime);

	bool	IsEnable(void) const	{ return m_bEnable;	}
	REAL32	GetRate(void) const		{ return m_rRate;	}
};


class CCoolEffect
{
private:
	enum EFFECT_STATE	{ STATE_NONE = 0, STATE_INC, STATE_DEC	};

private:
	bool			m_bEnable;

	static REAL32	m_rRateMin;
	static REAL32	m_rRateMax;
	static REAL32	m_rSpeed;

	EFFECT_STATE	m_State;
	REAL32			m_rRate;

public:
	CCoolEffect(void);
	~CCoolEffect(void);

	static void	SetRateRange( REAL32 rMin, REAL32 rMax )	{ m_rRateMin = rMin;	m_rRateMax = rMax;	}
	static void	SetReteSpedd( REAL32 rSpeed )				{ m_rSpeed = rSpeed;						}

	bool	OnUpdate( REAL32 tm );

	void	StartEffect(void)		{ m_bEnable	= true;		m_rRate	= m_rRateMin;	m_State	= STATE_INC;	}
	REAL32	GetRate(void) const		{ return m_rRate;		}
	bool	IsEnable(void) const	{ return m_bEnable;		}
};

#endif
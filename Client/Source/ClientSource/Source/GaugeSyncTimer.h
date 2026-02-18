#if !defined(__GAUGE_SYNC_TIMER_H)
#define __GAUGE_SYNC_TIMER_H

#ifdef	DOMI_UTIL

class GaugeSyncTimer
{
public:
	void	InitSpeed(REAL32 speed);
	void	InitInterpolate(REAL32 ip);
	void	InitMaxSP(REAL32 maxSP);
	void	SetEnable(bool bEnable);
	bool	IsEnabled() const;
	REAL32	GetValue() const; 
	void	ForceValue(REAL32 val); 
	void	InterpolateValue(REAL32 val); 
	void	UpdateFrame(REAL32 fDeltaTime); 

	struct data;
	struct timer 
	{ 
		virtual void Force(data&, REAL32 val) = 0;
		virtual void Interpolate(data&, REAL32 val) = 0;
		virtual void Update(data&, REAL32) = 0; 
	};

	struct data
	{
		data(); 
		REAL32 currSP, targetSP, targetSpeed, normalSpeed, interpolate_time, maxSP;
		timer*	tm;
	};

private:
	data	m_data;
};

inline void	GaugeSyncTimer::InitSpeed(REAL32 speed) { m_data.normalSpeed = speed; }
inline void	GaugeSyncTimer::InitInterpolate(REAL32 ip) { m_data.interpolate_time = ip; }
inline void	GaugeSyncTimer::InitMaxSP(REAL32 maxSP) { m_data.maxSP = maxSP; }

inline REAL32 GaugeSyncTimer::GetValue() const { return m_data.currSP; }
inline void	GaugeSyncTimer::ForceValue(REAL32 val) { m_data.tm->Force(m_data, val); }
inline void	GaugeSyncTimer::InterpolateValue(REAL32 val) { m_data.tm->Interpolate(m_data, val); }
inline void	GaugeSyncTimer::UpdateFrame(REAL32 fDeltaTime) { m_data.tm->Update(m_data, fDeltaTime); }


#endif

#endif
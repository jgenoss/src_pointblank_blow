#include "pch.h"

#ifdef	DOMI_UTIL

#include "GaugeSyncTimer.h"

namespace
{

	struct timer_normal : GaugeSyncTimer::timer 
	{
		virtual void Force(GaugeSyncTimer::data& d, REAL32 val) override;
		virtual void Interpolate(GaugeSyncTimer::data& d, REAL32 val) override;
		virtual void Update(GaugeSyncTimer::data& d, REAL32 fDeltaTime) override;
	} t_normal;
	struct timer_sync : GaugeSyncTimer::timer
	{
		virtual void Force(GaugeSyncTimer::data& d, REAL32 val) override;
		virtual void Interpolate(GaugeSyncTimer::data& d, REAL32 val) override;
		virtual void Update(GaugeSyncTimer::data& d, REAL32 fDeltaTime) override;
	} t_sync;

	struct timer_max : GaugeSyncTimer::timer
	{
		virtual void Force(GaugeSyncTimer::data& d, REAL32 val) override;
		virtual void Interpolate(GaugeSyncTimer::data& d, REAL32 val) override;
		virtual void Update(GaugeSyncTimer::data& d, REAL32 fDeltaTime) override {}
	} t_max;

	struct timer_disable : GaugeSyncTimer::timer 
	{
		virtual void Force(GaugeSyncTimer::data& d, REAL32 val) override;
		virtual void Interpolate(GaugeSyncTimer::data& d, REAL32 val) override;
		virtual void Update(GaugeSyncTimer::data& d, REAL32 fDeltaTime) override {}
	} t_disable;

	void timer_normal::Force(GaugeSyncTimer::data& d, REAL32 val) 
	{  
		if (val > d.maxSP) val = d.maxSP; d.currSP = val;   
	}
	void timer_normal::Interpolate(GaugeSyncTimer::data& d, REAL32 val) 
	{   
		d.targetSP = val + d.normalSpeed * d.interpolate_time;	
		if (d.targetSP > d.maxSP) d.targetSP = d.maxSP;

		d.targetSpeed = ( d.targetSP - d.currSP ) / d.interpolate_time;
		if ( fabs(d.targetSpeed) > FLT_EPSILON )
			d.tm = &t_sync;
	}

	void timer_normal::Update(GaugeSyncTimer::data& d, REAL32 fDeltaTime)
	{
		d.currSP += d.normalSpeed * fDeltaTime;
		if (d.currSP >= d.maxSP)
		{
			d.currSP = d.maxSP;
			d.tm = &t_max;
		}
	}

	void timer_sync::Force(GaugeSyncTimer::data& d, REAL32 val) 
	{ 
		if (val > d.maxSP) val = d.maxSP;    d.currSP = val;   d.tm = &t_normal;  
	}

	void timer_sync::Interpolate(GaugeSyncTimer::data& d, REAL32 val) 
	{
		d.targetSP = val + d.normalSpeed * d.interpolate_time;	
		if (d.targetSP > d.maxSP) d.targetSP = d.maxSP;
		d.targetSpeed = ( d.targetSP - d.currSP ) / d.interpolate_time;		
		if ( fabs(d.targetSpeed) <= FLT_EPSILON )
			d.tm = &t_normal;
	}

	void timer_sync::Update(GaugeSyncTimer::data& d, REAL32 fDeltaTime)
	{
		d.currSP += d.targetSpeed * fDeltaTime;
		REAL32 diff = d.currSP - d.targetSP;

		if ( diff * d.targetSpeed >= 0.f ) // d.currSP >= d.targetSP)		// 속도 변위가 반대가 됨..
		{	
			REAL32 rem_time = (diff) / d.targetSpeed; 
			d.currSP = d.targetSP;
			d.tm = &t_normal;
			t_normal.Update(d, rem_time);
		}
	}

	void timer_max::Force(GaugeSyncTimer::data& d, REAL32 val) 
	{  
		if (val < d.maxSP)  {	d.currSP = val;		 d.tm = &t_normal;  }
	}

	void timer_max::Interpolate(GaugeSyncTimer::data& d, REAL32 val) 
	{
		if (val < d.maxSP )
		{
			d.targetSP = val + d.normalSpeed * d.interpolate_time;	
			if (d.targetSP > d.maxSP) d.targetSP = d.maxSP;
			d.targetSpeed = ( d.targetSP - d.currSP ) / d.interpolate_time;

			if ( fabs(d.targetSpeed) > FLT_EPSILON )
				d.tm = &t_sync;	
		}
	}

	void timer_disable::Force(GaugeSyncTimer::data& d, REAL32 val) 
	{  
		if (val > d.maxSP) val = d.maxSP; d.currSP = val;     
	}
	
	void timer_disable::Interpolate(GaugeSyncTimer::data& d, REAL32 val) 
	{  
		if (val > d.maxSP) val = d.maxSP; d.currSP = val;  
	}
}


void	GaugeSyncTimer::SetEnable(bool bEnable) 
{  
	timer* tmp_tbl[2] = { &t_disable , &t_normal }; m_data.tm = tmp_tbl[bEnable];
}

bool	GaugeSyncTimer::IsEnabled() const 
{  
	return m_data.tm != &t_disable ; 
}

GaugeSyncTimer::data::data() : currSP(0.f), normalSpeed(1.f), interpolate_time(1.f), 
tm(&t_disable), maxSP(100.f), targetSP(0.f), targetSpeed(0.f) 
{
}

#endif
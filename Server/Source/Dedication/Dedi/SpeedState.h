#ifndef _SPEED_STATE_H
#define _SPEED_STATE_H

#define MAX_SPEED_COUNT		5


class CSpeedState
{
public:
	void InsertSpeed( REAL32 r32Speed );
	BOOL IsSpeedHack( REAL32* pr32AvgSpeed );

	CSpeedState();
	~CSpeedState();
	void Reset();
private:
	REAL32 m_ar32SpeedData[ MAX_SPEED_COUNT];
	UINT32 m_ui32Count;
};

#endif 


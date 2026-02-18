#if !defined( __PROFILE_LOG_H)
#define __PROFILE_LOG_H

enum PFLOG_TYPE
{
	PFLOG_UPDATE = 0,
	PFLOG_TRACE3D,
	PFLOG_RENDER3D,
	PFLOG_PHYSX,
	PFLOG_END,
	PFLOG_EFFECT,
	PFLOG_SGCONTEXT,

	PFLOG_COUNT
};

class CProfileLog : public i3ElementBase
{
	I3_CLASS_DEFINE( CProfileLog, i3ElementBase);
protected:
	// Total
	REAL32		m_Total;
	REAL32		m_Total_Max;

	// OnUpdate
	REAL32		m_timeAll[ PFLOG_COUNT];
	REAL32		m_timeSec[ PFLOG_COUNT];
	REAL32		m_timeMax[ PFLOG_COUNT];

	INT32		m_FrameCnt;
	INT32		m_SecCnt;

	REAL32		m_timeLocal;
	
	bool		m_bStart;
	REAL32		m_timeStart;

public:
	CProfileLog(void);
	virtual ~CProfileLog(void);

	void		UpdateFrame( REAL32 tm);
	void		Log(void);

};

#endif

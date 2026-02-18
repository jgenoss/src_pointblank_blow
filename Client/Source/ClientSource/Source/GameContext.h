#ifndef __GAMECONTEXT_H__
#define __GAMECONTEXT_H__

//#if defined(BUILD_RELEASE_QA_VERSION) && defined(DEV_BUILD)
#define		CHECK_CONNECTION_STATE
//#endif

class CGameContext :	public i3GameNode
{
	I3_CLASS_DEFINE( CGameContext, i3GameNode );
public:
	CGameContext(void);
	virtual ~CGameContext(void);

	bool	OnCreate(void); 
	bool	OnDestroy(void);
	virtual void	OnUpdate( REAL32 rDeltatime ) override;

	void	ShutDownTimer();
	UINT16	GetPlayTime() const		{ return m_PlayTime; } 

private:
	UINT16	m_PlayTime = 0;		// 플레이 타임(시간 단위)
	bool	m_bShutDownTimeAlarm = true;
};


#endif //__GAMECONTEXT_H__
	
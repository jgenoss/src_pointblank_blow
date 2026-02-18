#if !defined( _CLEVELDESIGN_PROFILE_H__)
#define _CLEVELDESIGN_PROFILE_H__

class CLevelDesignProfile
{
public:
	char	m_szResPath[MAX_PATH];
	char	m_szWorkingDir[MAX_PATH];

	char	m_szStageType[128];

	BOOL	m_bSerializeRes;
	BOOL	m_bLaunchGame;

	REAL32	m_PosSnap;
	REAL32	m_ScaleSnap;
	REAL32	m_RotateSnap;

public:
	CLevelDesignProfile(void);
	~CLevelDesignProfile(void);

	void	Load( void);
	void	Save( void);
	void	Apply( void);
};

#endif
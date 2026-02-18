#if !defined( __FRAMEWORK_H)
#define __FRAMEWORK_H

#define			PERF_COUNT			5

class CFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( CFramework);
protected:
	bool		m_bCamDrag;
	POINT		m_DragStartPt;
	MATRIX		m_DragStartTx;

	i3TextNodeDX2	*		m_pInfoText;
	REAL32					m_timeInfo;

protected:
	void	_MoveToAt( REAL32 speed, REAL32 tm);
	void	_MoveToRight( REAL32 speed, REAL32 tm);

	void	_SetInfo( const char * pszText);

public:
	CFramework(void);
	virtual ~CFramework(void);

	virtual void	OnCreate(void);
	virtual void	OnSize( POINT2D * pSize);
	virtual void	OnChangeScene(void);

	virtual void	OnUpdate( REAL32 tm);

	virtual void	OnMButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point);

	virtual void	OnMouseMove( UINT32 nFlag, POINT point);
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point);

	virtual void	OnInput(void);
};

#endif

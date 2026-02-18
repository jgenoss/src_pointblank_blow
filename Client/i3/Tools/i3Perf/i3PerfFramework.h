#if !defined( __I3_PERF_FRAMEWORK_H)
#define __I3_PERF_FRAMEWORK_H

#define			PERF_COUNT			5

class i3PerfFramework : public i3ViewerFramework
{
	I3_CLASS_DEFINE( i3PerfFramework);

protected:
	enum PERF_STATE
	{
		PERF_STATE_NONE = 0,
		PERF_STATE_MEASURE
	};

protected:
	bool		m_bCamDrag;
	POINT		m_DragStartPt;
	MATRIX		m_DragStartTx;

	i3TextNodeDX2	*		m_pInfoText;
	REAL32					m_timeInfo;

	i3TextNodeDX2	*		m_pPerfText;

	PERF_STATE	m_PerfState;
	INT32		m_PerfCount;
	REAL32		m_timePerf;

	REAL32		m_FPSTable[PERF_COUNT];

protected:
	void	_MoveToAt( REAL32 speed, REAL32 tm);
	void	_MoveToRight( REAL32 speed, REAL32 tm);

	void	_MoveRecordedCam( INT32 idx);
	void	_RecordCam( INT32 idx);

	void	_SetInfo( const char * pszText);

	void	_SetPerfState( PERF_STATE state);
	void	_AddPerfReport(void);

	void	_RebuildAllShaders(void);

public:
	i3PerfFramework(void);
	virtual ~i3PerfFramework(void);

	virtual void	OnCreate(void);
	virtual void	OnSize( POINT2D * pSize);
	virtual void	OnChangeScene(void);

	virtual void	OnUpdate( REAL32 tm);

	//virtual void	OnLButtonDown( UINT32 nFlag, POINT point);
	//virtual void	OnLButtonUp( UINT32 nFlag, POINT point);
	virtual void	OnMButtonDown( UINT32 nFlag, POINT point);
	virtual void	OnMButtonUp( UINT32 nFlag, POINT point);
	//virtual void	OnRButtonDown( UINT32 nFlag, POINT point);
	//virtual void	OnRButtonUp( UINT32 nFlag, POINT point);

	virtual void	OnMouseMove( UINT32 nFlag, POINT point);
	virtual void	OnMouseWheel( UINT nFlag, short zDelta, POINT point);

	virtual void	OnInput(void);

	//virtual	void	OnKeyDown( UINT32 nKey);
	//virtual void	OnKeyUp( UINT32 nKey);
};

#endif

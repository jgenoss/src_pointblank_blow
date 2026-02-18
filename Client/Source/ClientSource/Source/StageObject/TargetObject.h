#if !defined( __TARGET_OBJECT_H)
#define __TARGET_OBJECT_H

class TargetObject : public i3Object
{
	I3_CLASS_DEFINE( TargetObject, i3Object);
public:
	enum TYPE
	{
		TYPE_ENEMY = 0,
		TYPE_FRIEND,
		TYPE_RANDOM
	};

	enum APPEAR_TYPE
	{
		APT_POPUP = 0,
		APT_SLIDE_LEFT,
		API_SLIDE_RIGHT,
	};

	enum STATE
	{
		STATE_SLEEP = 0,				// 누운 상태 (비활성화 상태)
		STATE_START_ACTIVE,				// 일어서고 있음.
		STATE_ACTIVE,					// 완전히 일어난 상태
		STATE_START_SLEEP,				// 파괴되지 않은 상태에서 눕고 있음.
		STATE_DESTROY,
		STATE_FINISH,
	};

protected:
	// Persistant members
	TYPE			m_Type;
	APPEAR_TYPE		m_Appear;
	REAL32			m_timeOffset;
	REAL32			m_timeStart;
	REAL32			m_timeKeep;
	REAL32			m_timeEnd;
	
	// Volatile members
	TYPE			m_ActualType;			// Random때문에...
	STATE			m_State;
	STATE			m_OldState;
	REAL32			m_timeLocal;
	bool			m_bSmall;
	bool			m_bDestroyed;

	i3Animation *	m_pAnimStart;
	i3Animation *	m_pAnimKeep;
	i3Animation *	m_pAnimEnd;

protected:
	void			_FindAnimForPopup(void);
	void			_FindAnimForSlide(void);

public:
	TargetObject(void);
	virtual ~TargetObject(void);

	TYPE			getType(void)						{ return m_Type; }
	void			setType( TYPE type)					{ m_Type = type; }

	APPEAR_TYPE		getAppearType(void)					{ return m_Appear; }
	void			setAppearType( APPEAR_TYPE type)	{ m_Appear = type; }

	REAL32			getOffsetTime(void)					{ return m_timeOffset; }
	void			setOffsetTime( REAL32 tm)			{ m_timeOffset = tm; }

	REAL32			getStartTime(void)					{ return m_timeStart; }
	void			setStartTime( REAL32 tm)			{ m_timeStart = tm; }

	REAL32			getKeepTime(void)					{ return m_timeKeep; }
	void			setKeepTime( REAL32 tm)				{ m_timeKeep = tm; }

	REAL32			getEndTime(void)					{ return m_timeEnd; }
	void			setEndTime( REAL32 tm)				{ m_timeEnd = tm; }

	STATE			getState(void)						{ return m_State; }

	void			Start(void);
	void			Reset(void);

	virtual void	Damage( REAL32 dmg, i3GameObj * pEventSender, i3PhysixShapeSet * pHitShapeSet) override;

	void			OnSleep( REAL32 tm, bool bFirst);
	void			OnStartActive( REAL32 tm, bool bFirst);
	void			OnActive( REAL32 tm, bool bFirst);
	void			OnStartSleep( REAL32 tm, bool bFirst);
	void			OnDestroy( REAL32 tm, bool bFirst);
	
	virtual void	OnBindResource(void) override;
	virtual void	OnChangeTime( REAL32 tm) override;
	virtual void	OnUpdate( REAL32 tm) override;
	virtual	void	CopyTo( i3ElementBase * pObj, I3_COPY_METHOD method) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

#if defined(I3_DEBUG)//XML
	virtual bool	OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
	virtual bool	OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML) override;
#endif

	//
	// Level Tool에서 게임 의존적인 추가 Data를 저장하기 위해 호출한다.
	// OnQueryGameData() 함수 내에서는 필요한 메모리를 할당해 반환해야 하며
	// 실제로 할당된 메모리의 크기를 반환한다.
	//
	// Level Tool은 반환된 메모리를 i3MemFree() 함수를 통해 해제하기 때문에
	// 반드시 i3MemAlloc() 함수를 통해 할당된 메모리여야 한다.
	//
	// Paramters
	//		pData	[OUT]		저장할 데이터를 위해 할당한 메모리 버퍼. 함수 내에서 할당된다.
	//
	// Return
	//		할당된 메모리 버퍼의 크기를 Byte 단위로 반환한다.
	//
	virtual UINT32		OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData) override;

	//
	// i3GameSceneInfo Class에서 게임용 Instance를 Loading하는 중에 호출한다.
	// OnQueryGameData() 함수에 전달된 것과 동일한 처리를 해야한다.
	// 실제로 읽어들인 메모리를 반환해야 하며, 만약 처리 중 Error가 발생했다면
	// STREAM_ERR을 반환해야 한다.
	virtual UINT32		OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz) override;
};

#endif

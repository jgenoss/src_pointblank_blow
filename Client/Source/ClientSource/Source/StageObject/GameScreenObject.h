#if !defined( _GAME_SCREEN_OBJECT_H__)
#define _GAME_SCREEN_OBJECT_H__

class GameScreenObject : public i3Object
{
	I3_CLASS_DEFINE( GameScreenObject, i3Object);

protected:
	i3CameraObject * m_pCamObj;

	//volatile
	i3Camera			*	m_pCam;
	i3TextureBindAttr	*	m_pTargetTextureAttr;

public:
	GameScreenObject(void);
	virtual ~GameScreenObject(void);

	virtual void	OnBindResource(void) override;

protected:
	void	_BindCameraObj();

public:
	void	SetCameraObj( i3CameraObject * pObj);
	i3CameraObject * getCameraObj( void)	{	return m_pCamObj;}

	virtual void	OnBuildObjectList( i3::vector<i3PersistantElement*>& List) override;
	virtual UINT32	OnSave( i3ResourceFile * pResFile) override;
	virtual UINT32	OnLoad( i3ResourceFile * pResFile) override;

#if defined(USE_EDITDLG)//XML
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

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
};

#endif
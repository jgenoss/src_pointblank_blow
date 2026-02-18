#if !defined ( _CGAMECAMERA_H__)
#define _CGAMECAMERA_H__

#define ATPOSSCALE -10.0f

class CGameCharaBase;

class CGameCamera :	public i3GameNode
{
	I3_CLASS_DEFINE( CGameCamera, i3GameNode);

public:
	CGameCamera();
	virtual ~CGameCamera(void);

protected:
	static bool m_bCameraInitForUSG;
	static VAR_NMLR(i3Camera*, 1) m_pCamera;
	static VAR_NMLR(MATRIX*, 2)	  m_pCamMatrix;

	static VEC3D		m_vMouseDelta;	//현재 프레임에서의 마우스 회전량 
	static UINT32		m_nMouseState;
	static UINT64		m_nKeyPress;
	static UINT64		m_nKeyStroke;

	static bool			m_IsCamLeft;
	
protected:
	bool				m_bNeedUpdate = true;
	REAL32				m_rDefaultFOV = 45.0f;
	REAL32				m_CamFOV = 45.0f;
	REAL32				m_CamNear = 1.0f;
	REAL32				m_CamFar = 1000.0f;
	
	VAR_LR(VEC3D*,3)				m_vBasePosition = new VEC3D();	//카메라의 기본 위치( 기준점으로부터)
	VAR_LR(CGameCharaBase*,4)		m_pTargetChara = nullptr;		//카메라와 연관된 캐릭터
	VAR_LR(MATRIX*,5)				m_pTargetMatrix = nullptr;	//카메라와 연관된 매트릭스
	VEC3D				m_vOldMouseDelta;	//이전 프레임에서의 마우스 회전량

	//Zoom
	bool				m_bEnabledZoom = false;
	REAL32				m_rZoomElapsedTime = 0.0f;
	REAL32				m_rTargetFOV = 45.0f;
	REAL32				m_rZoomTime = 0.0f;
	REAL32				m_rZoomRatio = 1.0f;

	//Tremble
	bool				m_bEnabledTremble = false;
	REAL32				m_TrembleAccTime = 0.0f;
	REAL32				m_TrembleTime = 0.0f;
	REAL32				m_rTrembAmp = 0.0f;
	REAL32				m_rTrembFreq = 0.0f;
	REAL32				m_rTrembPhase = 0.0f;

	//Interpolate
	bool				m_bEnabledInterpolate = false;
	REAL32				m_InterAccTime = 0.0f;
	REAL32				m_InterpolateTerm = 0.0f;
	VEC3D				m_vStart;
	VEC3D				m_vEnd;
	VEC3D				m_vStartAt;
	VEC3D				m_vEndAt;

protected:
	void				_SetCameraProjection();

	virtual void		ReCalculateMatrix();
	virtual void		_ResetCamStatus();
public:
	bool				isEnabledInterpolate(void)	{	return m_bEnabledInterpolate;	}
	i3Camera *			getCameraObject(void)		{	return m_pCamera; }
	MATRIX		*		getProjectionMatrix()		{	return m_pCamera->GetProjectionMatrix();}
	MATRIX		*		getViewMatrix()				{	return m_pCamera->GetViewMatrixAttr()->GetMatrix();}

	REAL32				GetCameraDefaultFOV( void)	{	return m_rDefaultFOV;	}
	REAL32				GetCameraFOV(void)			{	return m_CamFOV;}
	REAL32				GetCameraNear(void)			{	return m_CamNear;}
	REAL32				GetCameraFar(void)			{	return m_CamFar;}
	REAL32				GetCameraSide(void)			{	return i3Vector::GetX(m_vBasePosition);}
	REAL32				GetCameraHeight(void)		{	return i3Vector::GetY(m_vBasePosition);}
	REAL32				GetCameraDist(void)			{	return i3Vector::GetZ(m_vBasePosition);}

	void				SetTargetChara( CGameCharaBase* pChara)	
	{
		m_pTargetChara = pChara;
	}
	void				SetCameraDefaultFOV( REAL32 rDefaultFOV)	{	m_rDefaultFOV = rDefaultFOV;}
	void				SetCameraFOV(REAL32	rFov);
	void				SetCameraNear(REAL32 rNear);
	void				SetCameraFar(REAL32	rFar);
	void				SetCameraSide( REAL32 rSide)	{	i3Vector::SetX(m_vBasePosition, rSide);}
	void				SetCameraHeight( REAL32 rHeight){	i3Vector::SetY(m_vBasePosition, rHeight);}
	void				SetCameraDist( REAL32 rDist)	{	i3Vector::SetZ(m_vBasePosition, rDist);}

	void				SetCameraProjection(void)		{	_SetCameraProjection(); }

	VEC3D		*		getBasePosition()				{	return m_vBasePosition;}

	CGameCharaBase *	getCurTarget()					
	{
		return m_pTargetChara;
	}

	void				SlantWise( MATRIX * pMat, REAL32 rDelta);

	void				PivotRotate( MATRIX * pMat, REAL32 rDeltaX, REAL32 rDeltaY);
	MATRIX			*	getCamMatrix()	{	return m_pCamMatrix;}

	void				SetDefaultFOV(REAL32 rDelay);
	void				ZoomIn( REAL32 rFov, REAL32 rDelay);
	void				UpdateZoom( REAL32 rDeltaSeconds);
	REAL32				getZoomRatio()	{	return m_rZoomRatio;}

	void				InitTremble( void);
	void				SetTremble( REAL32 rAmp, REAL32 rFreq, REAL32 rPhase, REAL32 rTrembleTime);
	void				UpdateTremble( REAL32 rDeltaSeconds);

	void				SetInterPolate( REAL32 rInterpolateTime);
	REAL32				getInterpolateTerm( void)							{ return m_InterpolateTerm; }
	virtual void		UpdateInterpolate( REAL32 rDeltaSeconds);
	virtual void		OnEndInterpolate();

	virtual	void		ToggleCamSidePos();
public:
	virtual void		InitCamera(CGameCharaBase * pChar = nullptr, REAL32 rInterpolateTime = 0.0f/*, i3Camera * pUICameara = nullptr*/);
	virtual void		ResetCamera() {}
	virtual void		RespawnCamera() {}
	virtual void		SetCameraFromTrans(i3Body* pBody, INT32 idx, REAL32 rInterpolateTerm, i3Object* pObj) {}
	virtual void		OnInputKeyboard( UINT64 nKeyPress, UINT64 nKeyStroke);
	virtual void		OnInputMouse( VEC3D * pvecDelta, UINT32 nState);

	virtual void		OnUpdate(REAL32 rDeltaSeconds) override;

	void				ToggleToFPS_IfOnObject();

	void				SetCameraPos(VEC3D& v3Temp) { i3Matrix::SetPos( m_pCamMatrix, &v3Temp); }
	void				SetCameraDir(VEC3D& v3Temp) { i3Matrix::SetAt( m_pCamMatrix, &v3Temp); }
	void				SetCameraUp(VEC3D& v3Temp) { i3Matrix::SetUp( m_pCamMatrix, &v3Temp); }
	void				SetCameraRight(VEC3D& v3Temp) { i3Matrix::SetRight( m_pCamMatrix, &v3Temp); }
	void				SetMouseDelta(VEC3D& v3Temp) { i3Vector::Copy(&m_vMouseDelta, &v3Temp); }
	virtual void		AdjustTargetByCamera() {}

	VEC3D*				GetMouseDelta() const { return &m_vMouseDelta; }
};

#endif
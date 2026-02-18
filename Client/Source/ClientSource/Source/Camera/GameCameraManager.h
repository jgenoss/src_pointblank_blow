#if !defined ( _CGAMECAMERA_MANAGER_H__)
#define _CGAMECAMERA_MANAGER_H__

#include "GameCamera_FLY.h"
#include "GameCamera_Follow.h"
#include "GameCamera_FPS.h"
#include "GameCamera_Kill.h"
#include "GameCamera_Observer.h"
#include "GameCamera_Observer2.h"

#include "tinst.h"

const INT32 WRONG_SLOT_INDEX = 255;

enum CAMERA_TYPE
{
	CAMERA_TYPE_INVALID = -1,
	CAMERA_TYPE_FPS,
	CAMERA_TYPE_FOLLOW,
	CAMERA_TYPE_FLY,
	CAMERA_TYPE_KILL,
	CAMERA_TYPE_OBSERVER,		// 고정
	CAMERA_TYPE_ONOBJECT,
	CAMERA_TYPE_OBSERVER2,		// 프리

	CAMERA_TYPE_COUNT,
};

template<> struct is_tinst_manual<class CGameCameraManager> : std::tr1::true_type {}; 

class CGameCharaBase;

class CGameCameraManager :	public i3GameNode
{
	I3_CLASS_DEFINE( CGameCameraManager, i3GameNode);

protected:
	i3::vector<CGameCamera*> m_CameraList;
	CGameCharaBase*	m_pTargetChar = nullptr;

	VEC3D			m_vFPSBasePosition;

	CGameCamera	*	m_pCurCamera = nullptr;
	CGameCamera	*	m_pOldCamera = nullptr;
	INT32			m_nCurCamIndex = -1;

public:
	virtual ~CGameCameraManager(void);
	
	static CGameCameraManager*	i() { return tinst<CGameCameraManager>(); }

	void			Destory();
	void			Create( i3GameNode * pParentNode,  CGameCharaBase * pTargetChar = nullptr);
	void			setTargetCharacter( CGameCharaBase * pChar);
	CGameCharaBase * getTargetCharacter(void);

public:
	INT32	getCameraCount()		{	return (INT32)m_CameraList.size();}
	INT32	getCurCamIndex()		{	return m_nCurCamIndex;}
	
	CGameCamera * ChangeCamera( const char * pszCamName, REAL32 rTime = 0.0f);
	CGameCamera * ChangeToOldCamera(REAL32 rTime = 0.0f);
	CGameCamera * ChangeCamera( CAMERA_TYPE nCameraType, REAL32 rTime = 0.0f);
	CGameCamera * GetCurrentCamera() { return m_pCurCamera; }
    
	void		  SetCurrentCamera(CGameCamera* cam) { if (cam) m_pCurCamera = cam; }
	void		  SetOldCamera(CGameCamera* cam)	 { if (cam) m_pOldCamera = cam; }

	VEC3D	*	  getFPSBasePosition()	{	return &m_vFPSBasePosition;}

	//스테이지 별로 카메라의 Far값을 조절가능하게 하기위한 함수입니다.
	void			SetStageCamFar( const char * pszStageName);
	void			ResetCameraMgr();
	void			RespawnCameraMgr();

	//
	void			ApplyEnvSetFOV(void);

#if !defined( I3_NO_PROFILE)
	virtual void	OnReloadProperty() override;
#endif

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	void			OnChangeResolution( i3Viewer * pViewer);

	void			InitCameraByGiveupChara( CGameCharaBase * pChara);
	void			TraceCamera();
	void			RemoveTargetChara(CGameCharaBase* pChara);
};


#endif
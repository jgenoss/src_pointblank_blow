/**
 * \file	GameCharaCameraContext.h
 *
 * Declares the game chara camera context class.
 */


#if !defined( __GAME_CHARA_CAMERA_CONTEXT_H__)
#define __GAME_CHARA_CAMERA_CONTEXT_H__

#include "GameCharaContextBase.h"

class CGameCharaCameraControl;
class CGameCharaCameraControl1PV;
class CGameCharaCameraControl3PV;

class CGameCharaCameraContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaCameraContext, CGameCharaContextBase);

	friend class CGameCharaBase;
	friend class CGameCharaCameraControl;

protected:
	CGameCharaCameraControl *		m_pCurrentCameraControl = nullptr;
	CGameCharaCameraControl1PV *	m_p1PVCameraControl = nullptr;
	CGameCharaCameraControl3PV *	m_p3PVCameraControl = nullptr;

	//1인칭 카메라일때 앉은 자세와 선자세의 변경을 위해 ik값을 직접 변경합니다.
	CHARA_STATE_FOR_CAM m_nChangeFromState = CSFC_STAND;
	CHARA_STATE_FOR_CAM m_StateForCam = CSFC_STAND;				// 카메라를 위한 케릭터 상태

	REAL32				m_rChangePoseTime = 0.0f;			//자세 변경시 걸리는 시간
	REAL32				m_rChangePoseAccTime = 0.0f;		//자세 변경 누적시간
	bool				m_bChangeToStand = false;			//선자세로 변경인지 앉은자세로 변경인지.

	REAL32				m_rIKHeight = 0.0f;				// 캐릭터의 IK 높이

public:
	/** \brief IK에서부터 camera의 높이를 반환합니다. */
	REAL32				getIKHeightForCam( void);

	/** \brief IK 높이를 반환합니다. */
	REAL32				getIKHeight( void)					{ return m_rIKHeight; }

	/** \brief camera의 위치를 얻어옵니다.
		\param[in] pPos 반환되는 위치값
		\param[in] pMat 반환되는 Matrix값
		\param[in] bDump Trace 출력 유무 */
	void				getVirtualCamPosition( VEC3D * pPos, MATRIX * pMat = nullptr, bool bDump = false);

public:
	virtual ~CGameCharaCameraContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	// change view
	virtual void	OnChange1PV( void) override;
	virtual void	OnChange3PV( void) override;

	virtual void	OnChangeLocal( void) override;
	virtual void	OnChangeNetwork( void) override;
	virtual void	OnChangeNPC( void) override;
	
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief 앉기 서기에 대한 설정값을 설정합니다. */
	void			ProcessStandOn1PV( CHARA_STATE_FOR_CAM state, REAL32 rDeltatime);

	/** \brief 앉기 서기에 대한 처리를 합니다. */
	void			ProcessChangePose( REAL32 rDeltatime);

	/** \brief 견착 Camera로 설정합니다. */
	void			SetZoom( void);

	/** \brief Scope에 대한 Level을 설정합니다. */
	bool			SetScopeLevel( INT32 nScopeLevel);

	////////////////////////////////////////////////////////////////////////////////////
	// internal interface
protected:
	/** \brief 기본 Camera로 설정합니다. (FPS Camera) */
	void			_Idle_Camera();

	/** \brief Kill Camera로 설정한다.
		\param[in] killerIdx 죽인 유저 index	*/
	void			_Kill_Camera( INT32 killerIdx);

	/** \brief Observer mode에서 Zoom을 설정합니다
		\note 무기에 따라 자동으로 설정 */
	void			_Observer_Zoom( void);

	/** \brief Respawn시 Camera를 설정합니다.
		\note FPS camera */
	void			_Respawn_Camera();
};

#endif

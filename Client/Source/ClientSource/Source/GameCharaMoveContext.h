/**
 * \file	GameCharaMoveContext.h
 *
 * Declares the game chara move context class.
 */


#if !defined( __GAME_CHARA_MOVE_CONTEXT_H__)
#define __GAME_CHARA_MOVE_CONTEXT_H__

#include "GameCharaContextBase.h"

typedef UINT32	CHARA_MOVE_FLAG;
#define CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ	0x00000001
#define CHARA_MOVE_FLAG_TOUCHDOWN			0x00000002		//EscapeMission 연속된 충돌체크로 인한 스코어 폭딜을 막기 위해 설정합니다.
#define CHARA_MOVE_FLAG_IS_GRAVITY			0x00000004		// 중력핵 테스트
#define CHARA_MOVE_FLAG_PENETRATEWALL		0x00000010		// 벽 통과 커맨드

#define CONTROLLER_HIT_INFO_COUNT 128
#define CONTROLLER_WORLD_HIT_POS_COUNT 64

class CGameCharaBase;
class CGameCharaMoveControl;
struct LadderStateInfo;

class CGameCharaMoveContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaMoveContext, CGameCharaContextBase);

protected:
	CHARA_MOVE_FLAG		m_CharaMoveFlag = 0;

	CGameCharaMoveControl *			m_pCurrentMoveControl = nullptr;
	CGameCharaMoveControl *			m_pMoveControl = nullptr;
	
	VAR_LR(i3PhysixShapeSet*, 38)	m_pCCD_Box = nullptr;		// 이펙트 히트용 컨텍트 히트 박스

	VEC3D			m_MoveDir;			//	현재 이동 방향 (x,y,z가 0 이면 움직이지 않는다.) local
	VEC3D			m_MoveAt;			// world matrix
	VEC3D			m_MoveRight;		// world matrix

	VAR_LR(VEC3D*,39)	m_OldVel = new VEC3D();
	VAR_LR(VEC3D*,40)	m_vOldPos = new VEC3D();
	
	VAR_LR(VEC3D*,41)	m_Accel = new VEC3D();
	VAR_LR(VEC3D*,42)	m_VelCharacter = new VEC3D();

	VAR_LR(REAL32,43)	m_rVelocity = 0.0f;
	REAL32				m_tmStopingElapsed = 0.0f;
	REAL32				m_tmAccelElapsed = 0.0f;

	VAR_LR(REAL32,44)	m_rOldMaxMoveSpeed = 0.0f;
	VAR_LR(REAL32,45)	m_rMaxMoveSpeed = 0.0f;

	REAL32				m_rElapsedSpeedReduction = 0.0f;
	REAL32				m_rSpeedReductionRate = 0.0f;
	bool				m_bSpeedReductionEternal = false;

	VAR_LR(VEC3D*,46)	m_TakeOffPos = new VEC3D();				// Jump start position
	VAR_LR(REAL32,47)	m_rFallOldPosY = 0.0f;				// 자유 낙하 중 이전 프레임 높이값
	VAR_LR(REAL32,48)	m_rElapsedTakeOffTime = 0.0f;
	VAR_LR(REAL32,49)	m_rCurJumpHeight = 0.0f;
	VAR_LR(REAL32,50)	m_rCurJumpTime = 0.0f;
	VAR_LR(REAL32,51)	m_rHalfAccelOfGravity = 0.0f;		// 중력 가속도
	VEC3D			m_ForceJumpCharaVel;		// force Jump calc speed ( 점프대 )
	VEC3D			m_ForceJumpDir;				// force Jump start dir ( 점프대 )
	REAL32			m_rForceJumpSpeed = 0.0f;			// force start Max speed ( 점프대 )
	REAL32			m_rJumpUpOldTime = 0.0f;			// 점프 하강 시퀀스에 쓰는 이전 상승 시퀀스 마지막 시간 값.
	
	const LadderStateInfo*	m_ladder_info = nullptr;

	I3_TERRAIN_TYPE	m_CurFloorTerrainType;
	VAR_LR(REAL32,52)	m_rFloorCheckTimer = 0.0f;
	VAR_LR(REAL32,53)	m_rFloorLength = 0.0f;

	VAR_LR(REAL32, 54)	m_fDelayTime = 0.0f;				//EscapeMission 터치다운에 딜레이를 주어 연속적인 스코어 처리를 막습니다.
	VAR_LR(REAL32,55)	m_rRideObjectElapseTime = 0.0f;

	// for network assist
	VAR_LR(VEC3D*,56)	m_vPostPosition = new VEC3D();
	VAR_LR(VEC3D*,57)	m_vRecvPosition = new VEC3D();

	VEC3D			m_vRecvDir;
	VEC3D			m_vPostDir;

	REAL32			m_fRecvPosTime = 0.0f;

	REAL32			m_rReceivedLength = 0.0f;
	REAL32			m_fRecvThetaRhoTime = 0.0f;

	REAL32			m_fPostTheta = 0.0f;
	REAL32			m_fPostRho = 0.0f;
	REAL32			m_fRecvTheta = 0.0f;
	REAL32			m_fRecvRho = 0.0f;

	bool			m_bPushingRandomDir = false;

	bool			m_bGravityRecovery = false;

private:
	
	VAR_LR(i3PhysixController*, 58)	m_pController = nullptr;			// 컨트롤러

protected:
	void			_TestCollisionAtRespawn();
	REAL32			_GetMoveVelocity(VEC3D * pVecDir, EMoveSpeedType MoveType);
	INT32			_CalcFallingDamage( VEC3D * pTakeOffPos);

public:
	void			addMoveFlag( CHARA_MOVE_FLAG flag)		{ m_CharaMoveFlag |= flag; }
	void			removeMoveFlag( CHARA_MOVE_FLAG flag)	{ m_CharaMoveFlag &= ~flag; }
	bool			isMoveFlag( CHARA_MOVE_FLAG flag)		{ return (m_CharaMoveFlag & flag) == flag; }

	/** \brief PhysX controller를 반환합니다 */
	i3PhysixController *	getPhysixController( void)		{ return m_pController; }
	
	float	getDeleytime()									{return m_fDelayTime;};
	void	SetDeleyTime(float fTime)						{ m_fDelayTime = fTime;}
	void	SetTouchdown(bool bTrue)						{ if( bTrue) addMoveFlag( CHARA_MOVE_FLAG_TOUCHDOWN); else removeMoveFlag( CHARA_MOVE_FLAG_TOUCHDOWN);}
	bool	GetTouchdown()									{return isMoveFlag( CHARA_MOVE_FLAG_TOUCHDOWN);}

	/** \brief 이동값을 반환합니다.
		\note 입력에 의한 방향을 저장합니다.*/
	VEC3D *			getMoveDir( void)						{ return &m_MoveDir; }
	
	/** \brief 이동할 시의 방향을 반환합니다.
		\note 캐릭터의 방향입니다. */
	VEC3D *			getMoveAt( void)						{ return &m_MoveAt; }

	/** \brief 캐릭터의 Right 방향을 반환합니다. */
	VEC3D *			getMoveRight( void)						{ return &m_MoveRight; }

	/** \brief accelation을 반환합니다. */
	VEC3D *			getAccel( void)							{ return m_Accel;}

	/** \brief accelation을 설정합니다. */
	void			setAccel( REAL32 x, REAL32 y, REAL32 z)	{ i3Vector::Set( m_Accel, x, y, z); }
	void			setAccel( VEC3D * pVec)					{ i3Vector::Copy( m_Accel, pVec); }

	/** \brief velocity를 반환합니다. */
	VEC3D *			getVelocity( void)						{ return m_VelCharacter; }

	/** \brief velocity를 설정합니다. */
	void			setVelocity( REAL32 x, REAL32 y, REAL32 z)	{ i3Vector::Set( m_VelCharacter, x, y, z); }
	void			setVelocity( VEC3D * pVec)				{ i3Vector::Copy( m_VelCharacter, pVec); }

	/** \brief 이전 veloctiy를 반환합니다. */
	VEC3D *			getOldVelocity( void)					{ return m_OldVel; }

	/** \brief 이전 velocity를 설정합니다. */
	void			setOldVelocity( VEC3D * pVec)			{ i3Vector::Copy( m_OldVel, pVec); }

	/** \brief 이전 위치를 반환합니다. */
	VEC3D *			getOldPos( void)						{ return m_vOldPos; }

	/** \brief 이전 위치를 설정합니다. */
	void			setOldPos( VEC3D * pVec)				{ i3Vector::Copy( m_vOldPos, pVec); }

	/** \brief 현재 속력을 반환합니다. */
	REAL32			getSpeed( void)							{ return m_rVelocity; }

	/** \brief 현재 속력을 설정합니다. */
	void			setSpeed( REAL32 speed)					{ m_rVelocity = speed; }

	/** \brief 최대 속력을 반환합니다. */
	REAL32			getMaxSpeed( void)						{ return m_rMaxMoveSpeed; }

	/** \brief 최대 속력을 설정합니다. */
	void			setMaxSpeed( REAL32 speed)				{ m_rMaxMoveSpeed = speed; }

	/** \brief 최소 이동 절삭을 반환합니다. */
	REAL32			getStopingElapsed( void)				{ return m_tmStopingElapsed; }

	/** \brief 최소 이동 절삭을 설정합니다 */
	void			setStopingElapsed( REAL32 tm)			{ m_tmStopingElapsed = tm; }

	/** \brief 최소 accelation 절삭을 반환합니다. */
	REAL32			getAccelElapsed( void)					{ return m_tmAccelElapsed; }

	/** \brief 최소 acceleation 절삭을 설정합니다. */
	void			setAccelElapsed( REAL32 tm)				{ m_tmAccelElapsed = tm; }

	/** \brief 속도 절감 효과중인지 확인합니다. */
	bool			isEnabledSpeedReduction()				{ return m_bSpeedReductionEternal ? true : ( m_rElapsedSpeedReduction > 0.0f);}

	/** \brief 사다리와 접촉한 정보 Set/Get */
	void			setLadderStateInfo(const LadderStateInfo* info);
	const LadderStateInfo* getLadderStateInfo() const;
	bool			isJumpingOnLadder() const;

	/** \brief 바닥의 재질을 반환합니다. */
	I3_TERRAIN_TYPE	getFloorTerrainType( void)				{ return m_CurFloorTerrainType; }
	void			setFloorTerrainType( I3_TERRAIN_TYPE type)	{ m_CurFloorTerrainType = type; }

	/** \brief 바닥 재질 검사 시간을 반환합니다. */
	REAL32			getFloorCheckTimer( void)				{ return m_rFloorCheckTimer; }
	/** \brief 바닥 재질 검사 시간을 설정합니다. */
	void			setFloorCheckTimer( REAL32 tm)			{ m_rFloorCheckTimer = tm; }

	/** \brief 공중부양 시간 */
	REAL32			getElapsedTakeOffTime( void)			{ return m_rElapsedTakeOffTime; }
	void			setElapsedTakeOffTime( REAL32 tm)		{ m_rElapsedTakeOffTime = tm; }

	/** \brief 현재 점프한 높이를 반환합니다. */
	REAL32			getCurJumpHeight( void)					{ return m_rCurJumpHeight; }

	/** \brief 현재 점프한 높이를 설정합니다. */
	void			setCurJumpHeight( REAL32 height)		{ m_rCurJumpHeight = height; }

	/** \brief 현재 점프 후 시간을 반환한다. */
	REAL32			getCurJumpTime( void)					{ return m_rCurJumpTime; }

	/** \brief 현재 점프 후 시간을 설정합니다. */
	void			setCurJumpTime( REAL32 tm)				{ m_rCurJumpTime = tm; }

	/** \brief 캐릭터의 Network Index를 반환합니다. */
	INT32			getCharaNetIndex( void);

	/** \brief 오브젝트에서의 상대 좌표를 반환합니다.*/
	VEC3D *			getRideObjectLocalPos( void)			{ return m_RideLocalPos; }

	/** \brief 오브젝트에서의 상대 좌표를 설정합니다. */
	void			setRideObjectLocalPos(VEC3D* pPos);

	/** \brief 올라탄 오브젝트 인덱스를 반환합니다. */
	INT32			getRideObjectIndex( void)				{ return m_iRideObjectIndex; }

	/** \brief 공중에 있기전의 바닥의 위치를 설정합니다.
		\note 추락에 대한 계산을 하기 위한 용도*/
	void			setTakeOffPos( VEC3D * pPos)			{ i3Vector::Copy( m_TakeOffPos, pPos); m_rFallOldPosY = 0.f; }

	/** \brief 강제 점프 계산 x,z 속도를 저장합니다. 따라서 Y축 속도는 제거하여 저장합니다.*/
	void			setForceJumpCharaVel( VEC3D * pVel )	{ i3Vector::Copy(&m_ForceJumpDir, pVel); m_ForceJumpDir.y = 0.f; }
	void			setForceJumpDir( VEC3D * pVel)			{ i3Vector::Copy(&m_ForceJumpDir, pVel); m_ForceJumpDir.y = 0.f; }
	void			setForceJumpSpeed(REAL32 pSpeed)		{ m_rForceJumpSpeed = pSpeed; }

	void			setRideObjectElapseTime( REAL32 tm)		{ m_rRideObjectElapseTime = tm; }

	/** \brief 허리부터 바닥까지의 높이 */
	REAL32			getFloorLength( void)					{ return m_rFloorLength; }
	void			setFloorLength( REAL32 length)			{ m_rFloorLength = length; }

	bool			isStandOnMovingObject( void)			{ return isMoveFlag( CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ); }
	void			setStandOnMovingObject( bool bVal)		{ if( bVal) addMoveFlag( CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ); else removeMoveFlag(CHARA_MOVE_FLAG_STAND_ON_MOVEOBJ);}

	/** \brief 현재 위치를 반환합니다.
		\note 네트워크 보정용으로 사용 */
	VEC3D *			getPostPosition( void)					{ return m_vPostPosition; }
	void			setPostPosition( const VEC3D * vec)		{ i3Vector::Copy( m_vPostPosition, const_cast<VEC3D*>(vec)); }

	/** \brief 목표 위치를 반환합니다.
		\note 네트워크 보정용으로 사용 */
	const VEC3D *	getRecvPosition( void) const			{ return m_vRecvPosition; }
	void			setRecvPosition( const VEC3D * vec)		{ i3Vector::Copy( m_vRecvPosition, const_cast<VEC3D*>(vec)); }

	/** \brief 바라보는 목표 방향을 반환합니다. 
		\note 네트워크 보정용으로 사용 */
	VEC3D *			getRecvDirection( void)					{ return &m_vRecvDir; }

	/** \brief 현재 바라 보는 방향을 반환합니다.
		\note 네트워크 보정용으로 사용 */
	VEC3D *			getPostDirection( void)					{ return &m_vPostDir; }

	/** \brief 보정 타이머를 반환합니다. */
	REAL32			getRecvPosTime( void)					{ return m_fRecvPosTime; }

	/** \brief 보정 타이머를 설정합니다. */
	void			setRecvPosTime( REAL32 time)			{ m_fRecvPosTime = time; }

	/** \brief 목표 위치까지의 거리를 반환합니다.
		\note 네트워크 보정용으로 사용 */
	REAL32			getReceivedLength( void)				{ return m_rReceivedLength; }

	/** \brief 목표 위치까지의 거리를 설정합니다.
		\note 네트워크 보정용으로 사용 */
	void			setReceivedLength( REAL32 len)			{ m_rReceivedLength = len; }

	/** \brief 보정 타이머를 반환합니다.
		\note 방향 interpolate용으로 사용 */
	REAL32			getRecvThetaRhoTime( void)				{ return m_fRecvThetaRhoTime; }

	/** \brief 보정 타이머를 설정합니다.
		\note 네트워크 보정용으로 사용 */
	void			setRecvThetaRhoTime( REAL32 time)		{ m_fRecvThetaRhoTime = time; }

	/** \brief 현재 Theta를 반환합니다.
		\note 네트워크 보정용으로 사용 */
	REAL32			getPostTheta( void)						{ return m_fPostTheta; }

	/** \brief 현재 Theta를 설정합니다.
		\note 네트워크 보정용으로 사용 */
	void			setPostTheta( REAL32 theta)				{ m_fPostTheta = theta; }

	/** \brief 현재 Rho를 반환합니다.
		\note 네트워크 보정용으로 사용 */
	REAL32			getPostRho( void)						{ return m_fPostRho; }

	/** \brief 현재 Rho를 설정합니다.
		\note 네트워크 보정용으로 사용 */
	void			setPostRho( REAL32 rho)					{ m_fPostRho = rho; }

	/** \brief 목표 Theta를 반환합니다.
		\note 네트워크 보정용으로 사용 */
	REAL32			getRecvTheta( void)						{ return m_fRecvTheta; }

	/** \brief 목표 Theta를 설정합니다.
		\note 네트워크 보정용으로 사용 */
	void			setRecvTheta( REAL32 theta)				{ m_fRecvTheta = theta; }

	/** \brief 목표 Rho를 반환합니다.
		\note 네트워크 보정용으로 사용 */
	REAL32			getRecvRho( void)						{ return m_fRecvRho; }

	/** \brief 목표 Rho를 설정합니다.
		\note 네트워크 보정용으로 사용 */
	void			setRecvRho( REAL32 rho)					{ m_fRecvRho = rho; }

	bool			IsNetworkController( void);

	void			EnablePushingRandomDir()				{ m_bPushingRandomDir = true; }
	
	/** \brief 중력 복구 아이템을 사용합니다.
	\note 중력값 복구를 위한 Flag */
	void			SetGravityRecovery(bool bEnable)		{ m_bGravityRecovery = bEnable; }
public:
	CGameCharaMoveContext();
	virtual ~CGameCharaMoveContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;
	virtual void	OnDestroy( void) override;

	virtual void	OnReset( void) override;

	virtual void	OnChangeLocal( void) override;
	virtual void	OnChangeNetwork( void) override;
	virtual void	OnChangeNPC( void) override;
	virtual void	OnChangeAI( void) override;
	virtual void	OnChangeFullbody( void) override;
	virtual void	OnUpdate( REAL32 rDeltatime) override;

	/** \brief Collision Coltroller를 생성합니다. */
	void			CreateController( void);

	/** \brief Collision Controller를 해제합니다. */
	void			ReleaseController( void);

	/** \brief Collision Controller를 초기화합니다. */
	void			ResetController( void);

	/** \brief Collision 유무를 설정합니다. */
	void			EnableController( bool bVal = true);

	/** \brief Controller의 높이를 조절합니다. */
	void			ChangeControllerHeight( REAL32 rHeight);

	/** \brief Collision test를 합니다. */
	void			CheckWorldCollision( REAL32 rDeltatime);
	
	void			UpdateTouchDown(REAL32 rDeltatime); //EscapeMission 연속된 충돌체크로 인한 스코어 중복체크를 막기 위해 설정합니다.
	void			ResetJumping( void);

	/** \brief 점프 Update합니다. */
	void			UpdateJumping( REAL32 rDeltatime);

	/** \brief 낙하 damage를 계산합니다. */
	void			UpdateFallDamage( REAL32 rDeltatime);

	REAL32			CalrFallVel(VEC3D& newPos, REAL32 rFac, REAL32 rDeltatime);

	//셋 그래비티 함수
	void 			SetGravity(bool bTrue)					{ if( bTrue) addMoveFlag( CHARA_MOVE_FLAG_IS_GRAVITY); else removeMoveFlag( CHARA_MOVE_FLAG_IS_GRAVITY); }
	bool			isGravity()								{ return isMoveFlag( CHARA_MOVE_FLAG_IS_GRAVITY);}
	
	/** \brief 이동 속력 감소 효과를 적용합니다.
		\param[in] nHitPart 맞은 부위에 따라 적용되는 값이 다릅니다. */
	void			EnableSpeedReduction( UINT8 nHitPart);

	/** \brief 이동 속력 감소 효과를 적용합니다.
		\param[in] rRate 감소력
		\param[in] rTime 지속 시간 */
	void			EnableSpeedReduction( REAL32 rRate, REAL32 rTime);

	/** \brief 이동 속력 감소 효과를 Update합니다. */
	void			UpdateSpeedReduction( REAL32 rDeltaSeconds);

	CHARA_HIT_PART_SIMPLE PartKind(UINT8 nHitPart);

	// 오브젝트 타기 계산
	void			SetRideObject( i3Object * pObj);
	void			ResetRideObject( void);
	void			CheckObjectByController( void);
	void			UpdateRideObject( MATRIX * pNewTx, REAL32 tm, bool bPhysixUpdate);
	VEC3D *			TransformByRideObject( MATRIX * pAnimatedMtx, VEC3D * pPos, bool bUpdate);
	void			RotateThetaByRideObejct( MATRIX * pAnimatedMtx, MATRIX * pInvMtx);

	/** \brief 이동을 계산합니다. */
	void			Move( REAL32 rDeltatime);
	
	/** \brief 캐릭터의 현재 위치를 설정합니다. */
	void			SetPosition( const VEC3D * pPos);

	/** \brief 캐릭터의 현재 위치를 얻어 옵니다. */
	void			GetPosition( VEC3D * pPos);

	/** \brief accelation값을 계산합니다. */
	void			ProcessAccel( void);

	/** \brief 오브젝트 위에서의 이동을 계산합니다. */
	void			MoveCharaByObject( VEC3D * pPos);

	/** \brief Controller를 이동시킵니다.
		\param[in] pMoveVec 이동값
		\param[in] minDist 최소 이동 거리
		\param[in] pHintFlag 충돌에 대한 결과 flag
		\param[in] sharpness 물리 계산시 딱딱한 정도 (높을 수록 이동 결과가 딱딱하게 된다)*/
	void			MoveController( VEC3D * pMoveVec, REAL32 minDist, UINT32 * pHintFlag, REAL32 sharpness = 1.0f);

	/** \brief 미끄러지는 것을 계산합니다.
		\param[in] hitMask 충돌 결과 flag
		\param[in] minDist 최소 이동된 거리
		\param[in] vVel 이동 방향 속도*/
	void			SlipSlope(UINT32& hitMask, REAL32& minDist, VEC3D& vVel);

	// revision 54918
	/** \brief 예외 상황에서의 컬리전을 체크합니다. */
	void			CheckCollisionException(UINT32& hitMask, VEC3D& vVel);
	

	// 빠른 오브젝트 충돌 처리
public:
	void			GetNxCapsule(NxCapsule* pCapsule);
	void			CheckOverlapObject();
	void			CreateCCD(VEC3D* pv3Pos, VEC3D* pv3Size, int iCGRP);
	void			FromObjDamageToChara(NxShape* pShape, i3PhysixShapeSet* pi3Shape);
	bool			EnableKill();

	// 올라탄 오브젝트
private:
	VAR_LR(i3Object*, 59)	m_pRideObject = nullptr;
	VAR_LR(i3Object*, 60)	m_pOldRideObject = nullptr;
	VAR_LR(VEC3D*,	   61)	m_RideLocalPos = new VEC3D();
	VAR_LR(INT32,	   62)	m_iRideObjectIndex = 0;
	VAR_LR(MATRIX*,   63)	m_matRideObjectPose = new MATRIX();
	VAR_LR(VEC3D*,    64)	m_RideSyncPos = new VEC3D();
	VAR_LR(INT32,     65)	m_iRideObjectCountTime = 0;

	i3Object*		m_pPrevJumpHold = nullptr;

public:
	i3Object *		getRideObject() const			{ return m_pRideObject; }
	void			setRideObject( i3Object * pObj)	{ m_pRideObject = pObj; }

	// 부딪치면 데미지를 주는 오브젝트
private:
	REAL32			m_rDamageTime = 0.0f;
public:
	//void			FindDamageObjectAndDamageCharaSelf(NxShape* pShape, i3PhysixShapeSet* pi3Shape);
	void			DamageCharaSelf_IfOverlap(NxShape* pShape);

	// 움직이는 오브젝트와 충돌 시 밀어내기
private:
	
	i3Object*		m_pContactObject = nullptr;
public:
	
	VEC3D			m_v3ContactObjectVelToChara;

	void			OverlapObjectPush(VEC3D& vVel, REAL32 minDist, UINT32& hitMask);
	void			PushByRaycastResult( const I3_PHYSIX_HIT_RESULT * pResult, const NxCapsule * pCapsule, NxVec3 &vDir, REAL32 skinWidth);
	void			CalcOverlapLength(NxShape* pShape, NxCapsule* pCapsule, REAL32 rSkinWidth, NxVec3 &vDir);
	void			CalcContactObjectVel(NxVec3& nxExtent, NxVec3& nxPos, NxCapsule* pCapsule, REAL32 rSkinWidth);
	void			CalcContactObjectVelConvex(NxVec3& nxExtent, NxVec3& nxPos, NxCapsule* pCapsule, REAL32 rSkinWidth);
	void			PushProcessFromAnimObj(VEC3D& vVel, REAL32 minDist, UINT32& hitMask);

	void			PostUpdate( REAL32 rDeltatime);
	void			OverlapPush();
	// test
	


	// 업타 운 기차 처리
	bool			IsUpTownMoveTrainArea() { return (m_RideSyncPos->z > -2.0183 && m_RideSyncPos->z < 2.0033); }
};

class CHitInfo
{
private:
	NxShape* m_pHitShape = nullptr;
	VEC3D m_v3HitNormal;

public:
	CHitInfo() {}
	void SetHitInfo(NxShape* pShape, VEC3D* pV) { m_pHitShape = pShape; if( pV != nullptr) i3Vector::Copy(&m_v3HitNormal, pV); }
	NxShape* GetHitShape() { return m_pHitShape; }
	VEC3D* GetHitNormal() { return &m_v3HitNormal; }
};

struct SWorldHitInfo
{
private:
	VEC3D m_vHitPos;
	VEC3D m_vHitNormal;

public:
	SWorldHitInfo() {}
	void SetWorldHitInfo(VEC3D* pPos, VEC3D* pNormal)	{	i3Vector::Copy(&m_vHitPos, pPos); i3Vector::Copy(&m_vHitNormal, pNormal); }
	void InitWorldHitInfo()	{ i3Vector::Zero(&m_vHitPos); i3Vector::Zero(&m_vHitNormal);}
	VEC3D* GetHitWorldPos()	{ return &m_vHitPos; }
	VEC3D* GetHitWorldNormal()	{ return &m_vHitNormal; }
};


class PlayerController : public i3PhysixController
{
	I3_CLASS_DEFINE( PlayerController, i3PhysixController);

protected:
	CGameCharaBase *	m_pOwner = nullptr;
	CGameCharaBase *	m_pHitUser = nullptr;

	INT32				m_nHitShapeCount = 0;
	CHitInfo			m_HitInfo[CONTROLLER_HIT_INFO_COUNT];

	INT32				m_nHitWorldPosCount = 0;
	SWorldHitInfo		m_HitWorldPos[CONTROLLER_WORLD_HIT_POS_COUNT];

	bool				m_bHitObject = false;

public:
	PlayerController(void) {}
	virtual ~PlayerController(void);

	void				setOwner( CGameCharaBase * pPlayer);
	

	CGameCharaBase *	getOwner(void)			 {	return m_pOwner;}

	CGameCharaBase *	getHitUser( void)		{ return m_pHitUser; }

	// NxShape *			getHitShape( INT32 idx)		{ return m_pHitShapeList[ idx]; }
	CHitInfo*			GetHitInfo(INT32 idx) { I3_BOUNDCHK(idx, CONTROLLER_HIT_INFO_COUNT); return &m_HitInfo[idx]; }
	INT32				getHitShapeCount( void)		{ return m_nHitShapeCount; }
	void				setHitShapeCount( INT32 val)	{ m_nHitShapeCount = val; }

	SWorldHitInfo*		GetHitWorld(INT32 idx) { I3_BOUNDCHK(idx, CONTROLLER_WORLD_HIT_POS_COUNT); return &m_HitWorldPos[idx]; }
	INT32				GetHitWorldShapeCount(void)	{ return m_nHitWorldPosCount;}
	void				SetHitWorldShapeCount( INT32 val)	{ m_nHitWorldPosCount = val; }

	void InitWorldHitInfoArray()
	{
		for(INT32 i = 0; i < CONTROLLER_WORLD_HIT_POS_COUNT; i++)
		{
			m_HitWorldPos[i].InitWorldHitInfo();
		}
	}

	virtual NxControllerAction		OnShapeHit( const NxControllerShapeHit * pHit) override;
	virtual	NxControllerAction		OnControllerHit( const NxControllersHit * pHit) override;

	static void			releaseShape( i3ElementBase * pReceiver, NxShape * pShape);

	void				FlushHitShape( void);
};

#endif


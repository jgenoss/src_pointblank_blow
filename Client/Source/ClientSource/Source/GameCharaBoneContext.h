/**
 * \file	GameCharaBoneContext.h
 *
 * Declares the game chara bone context class.
 */

#if !defined( __GAME_CHARA_BONE_CONTEXT_H__)
#define __GAME_CHARA_BONE_CONTEXT_H__

#include "GameCharaContextBase.h"
#include "GameCharaBoneDefine.h"

class CWeaponInfo;
class CGunInfo;
class WeaponBase;

typedef UINT32	CHARA_BONE_FLAG;
#define CHARA_BONE_FLAG_VALID_PIVOT		0x00000001
#define CHARA_BONE_FLAG_SHOVE_ACTIVE	0x00000002
#define CHARA_BONE_FLAG_SHOVE_RETURN	0x00000004
#define CHARA_BONE_FLAG_AIM_ROTATE		0x00000008
#define CHARA_BONE_FLAG_DIFF_BONECOUNT	0x00000010		///< LOD0와 LOD1의 Bone Count가 다른지에 대한 유무 (다르다면 ShapeSet을 수동으로 Update해야한다.)


class CGameCharaBoneContext : public CGameCharaContextBase
{
	I3_CLASS_DEFINE( CGameCharaBoneContext, CGameCharaContextBase);

	friend class CGameCharaInputContext;

	friend class CGameCharaUpdater;
	friend class CGameCharaUpdater1PV;
	friend class CGameCharaUpdater3PV;
	friend class CGameCharaUpdater3PVProfile;

	friend class CGameCharaWeaponControl;
	friend class CGameCharaWeaponControl1PVLocal;
	friend class CGameCharaWeaponControl3PVLocal;
private:
	VAR_LR(REAL32, 26)	m_Theta = 0.0f;			//	캐릭터 좌우 회전 값
	VAR_LR(REAL32, 27) m_Rho = 0.0f;			//	캐릭터 위아래 회전 값

protected:
	CHARA_BONE_FLAG		m_CharaBoneFlag = 0;

	//	Recoil
	CHARAPLAYER_RECOIL	m_RecoilData;
	//	Deviation
	CHARAPLAYER_DEVIATION	m_DeviationData;
	//	Scope
	CHARAPLAYER_SCOPEWAVE	m_ScopeWaveData;
	
	//	Pivot
	REAL32				m_PivotYaw = 0.0f;		//	상체 Animation을 전방으로 보정하기 위한 각도
	REAL32				m_PivotYawTarget = 0.0f;
	REAL32				m_timeYawBlend = 0.0f;
	REAL32				m_timeTargetYawBlend = 0.0f;
	REAL32				m_PivotRoll = 0.0f;
	VEC3D				m_PivotAxis;

	VAR_LR(MATRIX*, 28)	m_mtxPivot = new MATRIX();
	VAR_LR(INT32, 29)		m_iBoneMax = 0;
	VAR_LR(i3Body*,30)		m_pBody = nullptr;

	VAR_LR(INT32*, 31)			m_idxBone = nullptr;//[BONE_MAX];
	VAR_LR(MATRIX*, 32)		m_Bone = nullptr;//[BONE_MAX];


	REAL32				m_velShove, m_acclShove;
	REAL32				m_tShove, m_timeShove;
	QUATERNION			m_quatShoveTarget, m_quatShoveStart, m_quatShove;
	MATRIX				m_ShoveMatrix;

	/////////////////////////////////////////////
	// Head에 대한 시선 처리
	REAL32				m_HeadInterpolateTime = 0.0f;
	REAL32				m_HeadTheta = 0.0f;
	REAL32				m_HeadRho = 0.0f;
	REAL32				m_HeadTargetTheta = 0.0f;
	REAL32				m_HeadTargetRho = 0.0f;
	INT32				m_HeadWatchUserIdx = -1;

	////////////////////////////////////////////
	REAL32				m_AimStartTheta = 0.0f;
	REAL32				m_AimTheta = 0.0f;
	REAL32				m_AimTime = 0.0f;
	REAL32				m_AimInterpolate = 0.0f;

	i3BoneRef *			m_pEyeDummyLeft = nullptr;
	i3BoneRef *			m_pEyeDummyRight = nullptr;

	///////////////////////////////////////////////////////////////////////////////////////////
	// internal functions
private:
	/** \brief deviation을 설정합니다. */
	void			__SetDeviation(bool bApplyExt, CWeaponInfo* pGunInfo, WeaponBase* pWeapon);

	/** \brief raptor의 rho를 계산합니다. */
	void			__CalcRho3PVRaptor( REAL32 rDeltatime);

	/** \brief rho를 계산합니다.
		\note 사람*/
	void			__CalcRho3PVCommon( REAL32 rDeltatime);

	//////////////////////////////////////////////////////////////////////////////////
	//		internal functions at character
	//		다음 함수를 외부 class에서 사용할 시 friend class로 등록해야 합니다.
protected:
	/** \brief Bone List를 설정합니다. */
	virtual void	__InitBone( void);

	virtual void	_DoUpdateRhoBone1PV( MATRIX * pMod);

	virtual void	_DoUpdateRhoBone3PV( MATRIX * pMod);

	// 발사 시의 본 처리
	virtual void	_ProcessFire( void);

	/** \brief Update Animation 후 Spine을 돌리는 작업 */
	void			_CalcSpineBones( void);

	/** \brief 1인칭에서의 rho를 계산합니다. */
	virtual void	_CalcRho( REAL32 rDeltatime);

	/** \brief 3인칭에서의 rho를 계산합니다. */
	void			_CalcRho3PV( REAL32 rDeltatime);

	/** \brief Theta를 계산합니다. */
	void			_CalcTheta( REAL32 rDeltatime);

	/** \brief Theta를 계산합니다.
		\note Network용 character */
	void			_CalcThetaNetwork( REAL32 rDeltatime);

	/** \brief 머리 본에 대해서 Update합니다.
		\note 3인칭만 해당		*/
	void			_CalcHeadBone( REAL32 rDeltatime);

	/** \brief 하체 고정에 대한 Update를 합니다.
		\note 3인칭만 해당 */
	virtual void	_CalcAimBone( REAL32 rDeltatime);

public:
	void			addBoneFlag( CHARA_BONE_FLAG flag)		{ m_CharaBoneFlag |= flag; }
	void			removeBoneFlag( CHARA_BONE_FLAG flag)	{ m_CharaBoneFlag &= ~flag; }
	bool			isBoneFlag( CHARA_BONE_FLAG flag)		{ return (m_CharaBoneFlag & flag) == flag; }

	/** \brief Recoil 정보를 반환합니다. */
	CHARAPLAYER_RECOIL * getRecoilData( void)				{ return &m_RecoilData; }

	/** \brief deviation 정보를 반환합니다. */
	CHARAPLAYER_DEVIATION *	getDeviationData( void)			{ return &m_DeviationData; }

	/** \brief scope 반동 정보를 반환합니다. */
	CHARAPLAYER_SCOPEWAVE *	getScopeWaveData( void)				{ return &m_ScopeWaveData; }

	/** \brief theta를 설정합니다. */
	void			setTheta( REAL32 fTheta )
	{
		m_Theta = fTheta;
	}
	/** \brief theta를 반환합니다. */
	REAL32			getTheta( void)
	{
		return m_Theta;
	}

	/** \brief rho를 설정합니다. */
	void			setRho( REAL32 fRho )
	{
		m_Rho = fRho;
	}
	/** \brief rho를 반환합니다.
		\note 편차 계산된 값입니다. */
	REAL32			getRho( void)
	{
		return m_Rho + I3_DEG2RAD(m_RecoilData.m_RecoilAngle + m_ScopeWaveData.m_ScopeWaveRho);
	}

	/** \brief rho를 반환합니다. */
	REAL32			getRhoValue( void)
	{
		return m_Rho;
	}


	/** \brief View상의 rho를 반환합니다.
		\note 무기에 의한 편차가 포함됩니다. */
	virtual REAL32	getViewRho( void);

	/** \brief 흔들어주는 Matrix값을 반환합니다. */
	MATRIX *		getShoveMatrix(void)				{ return &m_ShoveMatrix; }

	/** \brief 캐릭터 방향을 설정합니다. */
	void			SetCharaDir(REAL32 rX, REAL32 rY, REAL32 rZ);

	/** \brief IK bone의 Matrix를 반환합니다. */
	MATRIX *		getIKMatrix(void)					{ return &m_Bone[ BONE_IK];			}

	/** \brief bone의 Matrix를 반환합니다 */
	MATRIX *		getBoneMatrix( INT32 idx)			{ return &m_Bone[ idx];				}

	/** \brief head bone의 matrix를 반환합니다*/
	MATRIX *		getHeadMatrix(void)					{ return &m_Bone[ BONE_HEAD];		}	
	
	/** \brief recoil angle을 반환합니다. */
	REAL32			getRecoilAngle(void)				{ return m_RecoilData.m_RecoilAngle; }

	/** \brief deviation angle을 반환합니다. */
	REAL32			getDeviationAngle(void)				{ return m_DeviationData.m_DeviationAngle; }

	/** \brief fire deviation을 반환합니다. */
	REAL32			getFireDeviation(void)				{ return m_DeviationData.m_FireDeviation; }
	
	/** \brief bone list의 Bone Index를 반환합니다. */
	INT32			getBoneIndex( INT32 idx)			
	{ 
#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
		if(m_pOwner->IsDino())
		{
			if( m_pOwner->IsRaptorBase()){			I3_BOUNDCHK( idx, RAPTOR_BONE_MAX); }
			else if( m_pOwner->IsRex()){			I3_BOUNDCHK( idx, TREX_BONE_MAX); }
		}
	#ifdef DOMI_DINO
		else if (m_pOwner->IsDomiDino())
		{
			if (m_pOwner->IsDomiRaptorBoneBase())	{ I3_BOUNDCHK(idx, DOMI_RAPTOR_BONE_MAX);	}
			else if (m_pOwner->IsDomiMutantRex())	{ I3_BOUNDCHK(idx, DOMI_MUTANTREX_BONE_MAX);}
		}
	#endif	// DOMI_DINO
		else
		{
			I3_BOUNDCHK( idx, BONE_MAX); 
		}
#endif	// I3_DEBUG
		
		return m_idxBone[ idx]; 
	}

	/** \brief Yaw blend 목표 시간을 설정합니다. */
	void			setTimeTargetYawBlend( REAL32 tm)	{ m_timeTargetYawBlend = tm; }

	/** \brief Yaw blend 시간을 설정합니다. */
	void			setTimeYawBlend( REAL32 tm)			{ m_timeYawBlend = tm; }

	/** \brief pivot matrix를 반환합니다. */
	MATRIX *		getPivotMatrix( void)				{ return m_mtxPivot; }

	/** \brief pivot matrix에 의한 spine 계산 여부를 설정합니다.
		\note 3인칭의 경우 pivot에 의한 편차를 계산하여 떨림을 줄여줘야 합니다. */
	void			setValidPivot( bool bVal)			{ if(bVal) addBoneFlag( CHARA_BONE_FLAG_VALID_PIVOT); else removeBoneFlag(CHARA_BONE_FLAG_VALID_PIVOT); }

	/** \brief pivot yaw의 목표값을 설정합니다 .*/
	void			setPivotYawTarget( REAL32 rVal)		{ m_PivotYawTarget = rVal; }

	/** \brief head bone의 주시하는 user index를 설정합니다.
		\note -1이면 주시하는 유저가 없다. */
	void			setHeadWatchUserIdx( INT32 idx);

	virtual INT32	GetRootBone( void)		{ return getBoneIndex( BONE_ROOT); }
	virtual INT32	GetSpine3Bone( void)	{ return getBoneIndex( BONE_SPINE3); }

	void			setEyeDummyLeft( i3BoneRef * pBoneRef)		{ I3_REF_CHANGE( m_pEyeDummyLeft, pBoneRef); }
	i3BoneRef *		getEyeDummyLeft( void) const				{ return m_pEyeDummyLeft; }
	void			setEyeDummyRight( i3BoneRef * pBoneRef)		{ I3_REF_CHANGE( m_pEyeDummyRight, pBoneRef); }
	i3BoneRef *		getEyeDummyRight( void) const				{ return m_pEyeDummyRight; }

public:
	CGameCharaBoneContext();
	virtual ~CGameCharaBoneContext();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0) override;

	virtual void	OnReset( void) override;

	virtual void	OnChange1PV( void) override;
	virtual void	OnChange3PV( void) override;

	void			InitBone( void)			{ __InitBone(); }
	
	/** \brief Animation을 Update합니다.
		\note Local 좌표상의 Update입니다. */
	void			UpdateAnim( REAL32 rDeltatime);

	/** \brief World 좌표로 각 Bone을 Update합니다. */
	void			TransformWorldSpace( REAL32 rDeltatime);

	/** \brief Pivot에 대해서 보정을 합니다.
		\note 허리가 흔들리는 것을 보정*/
	void			AdjustPivot( REAL32 rDeltatime, MATRIX * pInvMtx);
	
	/** \brief 흔드는 것..
		\note 사용안함 */
	void			Shove( VEC3D * pDir, REAL32 force);

	/** \brief Deviation에 대한 허리 축을 계산합니다. */
	virtual void	CalcDeviation( REAL32 rDeltatime);

	/** \brief Recoil에 대한 허리 축을 계산합니다. */
	void			CalcRecoil( REAL32 rDeltatime);

	/** \brief Scope 흔들림에 대한 계산을 합니다. */
	void			CalcScopeWave( REAL32 rDeltatime);

	// 수평 반동 회복(연사 후 원래 자리로 복귀)을 취소한다.
	void			cancelRecoilHorz(void)						{ m_RecoilData.cancelRecoilHorz(); }

	void			SetUpdateShapeSet(BONES eBone);

};

#endif

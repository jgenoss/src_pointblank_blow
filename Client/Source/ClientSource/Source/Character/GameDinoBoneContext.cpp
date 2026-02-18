#include "pch.h"
#include "GameDinoBoneContext.h"

#include "../GameCharaCameraContext.h"
#include "../GameCharaActionContext.h"
#include "GameDinoAnimContext.h"

I3_CLASS_INSTANCE( CGameDinoBoneContext);

void CGameDinoBoneContext::__InitBone()
{
	static char s_szRaptorBones[ RAPTOR_BONE_MAX][32] =
	{
		"Velociraptor_Root",
		"Velociraptor Pelvis",
		"IK",
		"Velociraptor Spine",
		"Velociraptor Spine1",
		"Velociraptor Spine2",
		"Velociraptor Neck",
		"Velociraptor Neck1",
		"Velociraptor Neck2",
		"Velociraptor Head",
		"Velociraptor L Toe0",
		"Velociraptor R Toe0",
		"Pivot_Camera",
	};

	static char s_szStingBones[ RAPTOR_BONE_MAX][32] =
	{
		"Sting_Root",
		"Sting_Root Pelvis",
		"IK",
		"Sting_Root Spine",
		"Sting_Root Spine1",
		"Sting_Root Spine2",
		"Sting_Root Neck",
		"Sting_Root Neck1",
		"Sting_Root Neck2",
		"Sting_Root Head",
		"Sting_Root L Toe0",
		"Sting_Root R Toe0",
		"Pivot_Camera",
	};

	static char s_szAcidBones[ RAPTOR_BONE_MAX][32] =
	{
		"Acid_Root",
		"Acid_Root Pelvis",
		"IK",
		"Acid_Root Spine",
		"Acid_Root Spine1",
		"Acid_Root Spine2",
		"Acid_Root Neck",
		"Acid_Root Neck1",
		"Acid_Root Neck2",
		"Acid_Root Head",
		"Acid_Root L Toe0",
		"Acid_Root R Toe0",
		"Pivot_Camera",
	};

	static char s_szEliteBones[ RAPTOR_BONE_MAX][32] =
	{
		"Elite_Root",
		"Elite_Root Pelvis",
		"IK",
		"Elite_Root Spine",
		"Elite_Root Spine1",
		"Elite_Root Spine2",
		"Elite_Root Neck",
		"Elite_Root Neck1",
		"Elite_Root Neck2",
		"Elite_Root Head",
		"Elite_Root L Toe0",
		"Elite_Root R Toe0",
		"Pivot_Camera",
	};

	static char s_szTankBones[ RAPTOR_BONE_MAX][32] =
	{
		"Tank_Root",
		"Tank_Root Pelvis",
		"IK",
		"Tank_Root Spine",
		"Tank_Root Spine1",
		"Tank_Root Spine2",
		"Tank_Root Neck",
		"Tank_Root Neck1",
		"Tank_Root Neck2",
		"Tank_Root Head",
		"Tank_Root L Toe0",
		"Tank_Root R Toe0",

		"Pivot_Camera",
	};

	static char s_szTRexBones[ TREX_BONE_MAX][32] =
	{
		"Dino_T-Rex",
		"Dino_T-Rex Pelvis",
		"IK",
		"Dino_T-Rex Spine",
		"Dino_T-Rex Spine1",
		"Dino_T-Rex Spine2",

		"IK_Neck",
		"Dino_T-Rex Neck",
		"Dino_T-Rex Neck1",
		"Dino_T-Rex Neck2",
		
		"IK_Head",
		"Dino_T-Rex Head",

		"Dino_T-Rex L Toe1",
		"Dino_T-Rex R Toe1",
		"Pivot_Camera",
	};

#ifdef DOMI_DINO
	static char s_szDomiRaptorBones[ DOMI_RAPTOR_BONE_MAX][32] =
	{
		"Velociraptor_Root",
		"Velociraptor Pelvis",
		"IK",
		"Velociraptor Spine",
		"Velociraptor Spine1",
		"Velociraptor Neck",
		"Velociraptor Neck1",
		"Velociraptor Head",
		"Velociraptor L Toe0",
		"Velociraptor R Toe0",
		"Pivot_Camera",
	};

	static char s_szDomiAcidBones[ DOMI_RAPTOR_BONE_MAX][32] =
	{
		"Acid_Root",
		"Acid_Root Pelvis",
		"IK",
		"Acid_Root Spine",
		"Acid_Root Spine1",
		"Acid_Root Neck",
		"Acid_Root Neck1",
		"Acid_Root Head",
		"Acid_Root L Toe0",
		"Acid_Root R Toe0",
		"Pivot_Camera",
	};

	static char s_szDomiStingBones[ DOMI_RAPTOR_BONE_MAX][32] =
	{
		"Sting_Root",
		"Sting_Root Pelvis",
		"IK",
		"Sting_Root Spine",
		"Sting_Root Spine1",
		"Sting_Root Neck",
		"Sting_Root Neck1",
		"Sting_Root Head",
		"Sting_Root L Toe0",
		"Sting_Root R Toe0",
		"Pivot_Camera",
	};

	static char s_szDomiMutantRexBones[ DOMI_MUTANTREX_BONE_MAX][32] =
	{
		"Dino_T-Rex",
		"Dino_T-Rex Pelvis",
		"IK",
		"Dino_T-Rex Spine",
		"Dino_T-Rex Spine1",


		"IK_Neck",
		"Dino_T-Rex Neck",
		"Dino_T-Rex Neck1",


		"IK_Head",
		"Dino_T-Rex Head",

		"Dino_T-Rex L Toe1",
		"Dino_T-Rex R Toe1",
		"Pivot_Camera",
	};

	static char s_szDomiTankBones[ DOMI_RAPTOR_BONE_MAX][32] = 
	{
		"Tank_Root",
		"Tank_Root Pelvis",
		"IK",
		"Tank_Root Spine",
		"Tank_Root Spine1",
		"Tank_Root Neck",
		"Tank_Root Neck1",
		"Tank_Root Head",
		"Tank_Root L Toe0",
		"Tank_Root R Toe0",

		"Pivot_Camera",
	};
#endif

	char (*pszBoneNames)[32] = nullptr;

	switch( m_pOwner->getCharaInfo()->GetResType())
	{
	case CHARA_RES_ID_DINO_CC_RAPTOR :
	case CHARA_RES_ID_DINO_RAPTOR_MERCURY :
	case CHARA_RES_ID_DINO_RAPTOR :				pszBoneNames = s_szRaptorBones;		break;
	case CHARA_RES_ID_DINO_TREX :				pszBoneNames = s_szTRexBones;		break;
	case CHARA_RES_ID_DINO_CC_STING :
	case CHARA_RES_ID_DINO_STING_MARS :
	case CHARA_RES_ID_DINO_STING :				pszBoneNames = s_szStingBones;		break;
	case CHARA_RES_ID_DINO_CC_ACID:
	case CHARA_RES_ID_DINO_ACID_VULCAN :
	case CHARA_RES_ID_DINO_ACID :				pszBoneNames = s_szAcidBones;		break;
	case CHARA_RES_ID_DINO_ELITE :				pszBoneNames = s_szEliteBones;		break;
	case CHARA_RES_ID_DINO_TANK :				pszBoneNames = s_szTankBones;		break;
#ifdef DOMI_DINO
	case CHARA_RES_ID_DINO_DOMI_RAPTOR :
	case CHARA_RES_ID_DINO_DOMI_RAPTOR_GREEN :
	case CHARA_RES_ID_DINO_DOMI_RAPTOR_BLUE :	
	case CHARA_RES_ID_DINO_DOMI_RAPTOR_MERCURY : pszBoneNames = s_szDomiRaptorBones;	break;
	case CHARA_RES_ID_DINO_DOMI_ACID :
	case CHARA_RES_ID_DINO_DOMI_ACID_NOT_BOMB :	
	case CHARA_RES_ID_DINO_DOMI_ACID_ITEM :		
	case CHARA_RES_ID_DINO_DOMI_ACID_VULCAN:	pszBoneNames = s_szDomiAcidBones;		break;
	case CHARA_RES_ID_DINO_DOMI_STING :			
	case CHARA_RES_ID_DINO_DOMI_STING_MARS:		pszBoneNames = s_szDomiStingBones;		break;
	case CHARA_RES_ID_DINO_DOMI_MUTANTREX :		pszBoneNames = s_szDomiMutantRexBones;	break;
	case CHARA_RES_ID_DINO_DOMI_TANK :			pszBoneNames = s_szDomiTankBones;		break;
#endif
	default :
		I3PRINTLOG(I3LOG_FATAL,  "unknown res type");
		break;
	}

	INT32 i;

	// Bones
	for( i = 0; i < m_iBoneMax; i++)
	{
		m_idxBone[i] = m_pBody->FindBoneByName( pszBoneNames[i]);
				
		I3ASSERT( m_idxBone[i] != -1);

		i3Matrix::Identity( &m_Bone[i]);
	}

	i3Matrix::Identity( m_mtxPivot);
	
	m_PivotYaw = 0.0f;
	m_PivotYawTarget = 0.0f;
	m_timeYawBlend = 0.0f;
	m_timeTargetYawBlend = 0.f;
	removeBoneFlag( CHARA_BONE_FLAG_VALID_PIVOT);
}

void CGameDinoBoneContext::_DoUpdateRhoBone1PV( MATRIX * pMod)
{
	if( m_pOwner->IsRaptorBase() )
	{
		VEC3D vHeight;
		MATRIX mtxTemp;
		VEC3D * pDir = i3Matrix::GetUp( m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_PELVIS]));

		MATRIX * pMat = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_IK]);
		i3Matrix::Mul( &mtxTemp, m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_SPINE]), pMat);
		i3Matrix::Mul( &mtxTemp, m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_SPINE1]), &mtxTemp);
		i3Matrix::Mul( &mtxTemp, m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_SPINE2]), &mtxTemp);
		i3Matrix::Mul( &mtxTemp, m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_NECK]), &mtxTemp);

		i3Matrix::Transpose( &mtxTemp, &mtxTemp);
		i3Vector::TransformNormal( &vHeight, pDir, &mtxTemp);
		i3Vector::Normalize( &vHeight, &vHeight);
		i3Vector::Scale( &vHeight, &vHeight, getCameraContext()->getIKHeight());
		i3Vector::Add( i3Matrix::GetPos( pMod), i3Matrix::GetPos( pMod), &vHeight);
		MATRIX * pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_NECK]);
		i3Matrix::Mul( pOut, pMod, pOut);
	}
	else
	{
		CGameCharaBoneContext::_DoUpdateRhoBone1PV( pMod);
	}
}

void CGameDinoBoneContext::_DoUpdateRhoBone3PV( MATRIX * pMod)
{
	MATRIX * pOut;

	if( m_pOwner->IsRex())
	{
		pOut = m_pBody->getCacheMatrix( m_idxBone[ TREX_BONE_IKNECK]);
		i3Matrix::Mul( pOut, pMod, pOut);

		if( getRhoValue() >= 0.0f)
		{
			pOut = m_pBody->getCacheMatrix( m_idxBone[ TREX_BONE_NECK]);
			i3Matrix::Mul( pOut, pMod, pOut);
			pOut = m_pBody->getCacheMatrix( m_idxBone[ TREX_BONE_NECK1]);
			i3Matrix::Mul( pOut, pMod, pOut);
			pOut = m_pBody->getCacheMatrix( m_idxBone[ TREX_BONE_NECK2]);
			i3Matrix::Mul( pOut, pMod, pOut);
		}
	}
	else if( m_pOwner->IsRaptorBase())
	{
		pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_NECK]);
		i3Matrix::Mul( pOut, pMod, pOut);

		if( getRhoValue() >= 0.0f)
		{
			pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_NECK1]);
			i3Matrix::Mul( pOut, pMod, pOut);
			pOut = m_pBody->getCacheMatrix( m_idxBone[ RAPTOR_BONE_NECK2]);
			i3Matrix::Mul( pOut, pMod, pOut);
		}
	}
	else
	{
		CGameCharaBoneContext::_DoUpdateRhoBone3PV( pMod);
	}
}

void CGameDinoBoneContext::_CalcAimBone( REAL32 rDeltatime)
{	
	MATRIX * pOut;
	MATRIX tmp;
	VEC3D axis;
	REAL32 rTheta;
	REAL32 rLimit[2];

	I3ASSERT( m_pOwner->IsDino() );
	
	if( (getActionContext() && getActionContext()->isLower( CHARA_LOWER_IDLE)) || m_pOwner->IsRex())
	{
		rTheta = i3Math::NormalizeAngle( getTheta() - m_AimTheta);
	
		if(m_pOwner->IsRaptorBase()) {
			rLimit[0] = 0.8f;
			rLimit[1] = -0.8f;
		}
		else if( m_pOwner->IsRex())
		{
			rLimit[0] = 0.9f;
			rLimit[1] = -0.9f;
		}
		else {
			rLimit[0] = 0.4f;
			rLimit[1] = -0.9f;
		}

		if( (rTheta > rLimit[0] || rTheta < rLimit[1]))
		{
			m_AimStartTheta = i3Math::NormalizeAngle( m_AimTheta);
			m_AimTime = 0.f;
			m_AimInterpolate = rTheta;

			if( !isBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE) )
			{
				if( !getActionContext()->isLower( CHARA_LOWER_RUN) )
				{
					if( rTheta > 0.f)
						((CGameDinoAnimContext*)getAnimContext())->_Play_AnimDino_Lower( ID_LOWER_DINO_MOVE_LEFT);
					else
						((CGameDinoAnimContext*)getAnimContext())->_Play_AnimDino_Lower( ID_LOWER_DINO_MOVE_RIGHT);
				}

				if( getActionContext()->isLower( CHARA_LOWER_RUN))	removeBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE);
				else												addBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE);
			}
		}
		else if( m_pOwner->IsRex())
		{
			if( getActionContext()->isLower( CHARA_LOWER_RUN) || getActionContext()->isLower( CHARA_LOWER_WALK))
			{
				m_AimStartTheta = i3Math::NormalizeAngle( m_AimTheta);
				m_AimTime = 0.f;
				m_AimInterpolate = rTheta;
				removeBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE);
			}
		}

		{	
			m_AimTime += rDeltatime;

			if( m_AimTime >= 0.2f)
			{
				m_AimTime = 0.2f;

				if( isBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE) )
				{
					((CGameDinoAnimContext*)getAnimContext())->_Play_AnimDino_Lower( ID_LOWER_DINO_IDLE);
					removeBoneFlag( CHARA_BONE_FLAG_AIM_ROTATE);
				}
			}
			
			REAL32 rSlope = m_AimInterpolate * m_AimTime * 5.f;

			m_AimTheta = i3Math::NormalizeAngle( m_AimStartTheta + rSlope);
		}
	}
	else
	{
		m_AimInterpolate = rTheta = 0.f;
		m_AimTheta = m_AimStartTheta = getTheta();
	}

	if( m_pBody->getAnimationContext() != nullptr)
	{
		MATRIX mod;
		i3Matrix::Inverse( &tmp, nullptr, getBoneMatrix( BONE_IK));
		i3Vector::Copy( &axis, i3Matrix::GetRight( getBoneMatrix( BONE_IK)));
		
		i3Vector::TransformNormal( &axis, &axis, &tmp);

		i3Matrix::SetRotateAxis( &mod, &axis, -rTheta );
	
		pOut = m_pBody->getCacheMatrix( m_idxBone[ BONE_PELVIS]);

		i3Matrix::Mul( pOut, &mod, pOut);
	}
}

REAL32 CGameDinoBoneContext::getViewRho( void)
{
	return getRhoValue();
}

INT32 CGameDinoBoneContext::GetRootBone( void)
{
	if(m_pOwner->IsRaptorBase())		return getBoneIndex( RAPTOR_BONE_ROOT);
	else if(m_pOwner->IsRex())			return getBoneIndex( TREX_BONE_ROOT);
#ifdef DOMI_DINO
	else if (m_pOwner->IsDomiRaptorBoneBase())
		return getBoneIndex(DOMI_RAPTOR_BONE_ROOT);
	else if (m_pOwner->IsDomiMutantRex())
		return getBoneIndex(DOMI_MUTANTREX_BONE_ROOT);
#endif

	return CGameCharaBoneContext::GetRootBone();
}

INT32 CGameDinoBoneContext::GetSpine3Bone( void)
{
	if(m_pOwner->IsRaptorBase())		return getBoneIndex( RAPTOR_BONE_SPINE1);
	else if(m_pOwner->IsRex())			return getBoneIndex( TREX_BONE_SPINE1);
#ifdef DOMI_DINO
	else if (m_pOwner->IsDomiRaptorBoneBase())
		return getBoneIndex(DOMI_RAPTOR_BONE_SPINE1);
	else if (m_pOwner->IsDomiMutantRex())
		return getBoneIndex(DOMI_MUTANTREX_BONE_SPINE1);
#endif

	return CGameCharaBoneContext::GetSpine3Bone();
}

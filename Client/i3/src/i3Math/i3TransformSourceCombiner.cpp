#include "i3MathType.h"
#include "i3TransformSourceCombiner.h"
#include "i3TransformSourceBind.h"
#include "i3Matrix.h"
#include "i3Quat.h"
#include "i3MathGlobalVariable.h"

//I3_CLASS_INSTANCE( i3TransformSourceCombiner, i3PersistantElement);
I3_CLASS_INSTANCE( i3TransformSourceCombiner);

#define		I3_TRANSFORMSOURCECOMBINER_ID_1		"TSC1"
#define		I3_TRANSFORMSOURCECOMBINER_ID_2		"TSC2"

i3TransformSourceCombiner::~i3TransformSourceCombiner(void)
{
	for( UINT8 i = 0; i < m_BindCount; i++)
	{
		m_Binds[i].Reset();
	}

	I3_SAFE_RELEASE( m_pModifier);
}

void i3TransformSourceCombiner::ShiftBind(void)
{
	if( m_BindCount > 1)
	{
		// 이전에 있던 Bind를 초기화 한다.
		m_Binds[0].Reset();

		memcpy( &m_Binds[0], &m_Binds[1], sizeof(i3TransformSourceBind));
		m_BindCount = 1;

		m_Binds[1].Init();
	}
}

void i3TransformSourceCombiner::AddAnimation( i3Animation * pAnim, INT32 idxBone,
								REAL32 rStartTime, 
								REAL32 interval,
								REAL32 rManualClampTime,
								I3_ANIM_BLEND_MODE blendMode, 
								I3_ANIM_PLAY_MODE destPlayMode, 
								I3_ANIM_PLAY_MODE srcPlayMode)
{
	i3TransformSourceBind * pBind, * pOldBind;

	if( ((blendMode == I3_ANIM_BLEND_MODE_BLEND) && (interval == 0.0f)) || (m_BindCount == 0))
		blendMode = I3_ANIM_BLEND_MODE_REPLACE;

	if( m_BindCount > 1)
	{
		// 2개 이상의 Slot이 마련되어 있지 않기 때문에
		// 또다시 추가로 Add가 발생했다면, 이전까지의 Transform Source들을 하나씩 지운다.
		ShiftBind();
	}

	m_timeCur = 0.0f;

	switch( blendMode)
	{
		case I3_ANIM_BLEND_MODE_REPLACE :
			{
				// 새로운 Animation으로 완전히 대체되기 때문에
				// 이전에 있던 animation은 아무런 의미가 없다.
				
				// 이전 Animation 삭제
				RemoveAllAnimation();

				m_timeBlend = 0.0f;
			}
			break;

		case I3_ANIM_BLEND_MODE_BLEND :
			{
				pOldBind = (i3TransformSourceBind *) &m_Binds[m_BindCount - 1];

				pOldBind->SetPlayMode( srcPlayMode);

				m_timeBlend = interval;
			}
			break;
	}

	pBind = & m_Binds[ m_BindCount];

	pBind->Init( rStartTime);
	pBind->setAnimation( pAnim, idxBone);
	pBind->SetBlendMode( blendMode);
	pBind->SetPlayMode( destPlayMode);
	pBind->SetManualClampTime(rManualClampTime);
	
	m_BindCount++;
}

void i3TransformSourceCombiner::SetPlayCount(UINT32 count)
{
	for( UINT8 i = 0; i < m_BindCount; i++)
	{
		m_Binds[i].SetPlayCount(count);
	}
}

void i3TransformSourceCombiner::SetPlayTime( REAL32 tm)
{
	for( UINT8 i = 0; i < m_BindCount; i++)
	{
		m_Binds[i].SetLocalTime( tm);
	}
}

void i3TransformSourceCombiner::AddPlayTime( REAL32 tm)
{
	for( UINT8 i = 0; i < m_BindCount; i++)
	{
		m_Binds[i].AddLocalTime( tm);
	}
}

REAL32 i3TransformSourceCombiner::GetDuration(void)
{
	REAL32 maxdur = 0.0f, dur;

	for( UINT8 i = 0; i < m_BindCount; i++)
	{
		dur = m_Binds[i].GetDuration();

		maxdur = MAX( dur, maxdur);
	}

	return maxdur;
}

REAL32 i3TransformSourceCombiner::GetActualDuration( void)
{
	REAL32 rate = i3Math::Reciprocal( m_timeScale);

	//	현재 Time Scale을 반영한 애니메이션 길이
	return GetDuration() * rate;
}

void i3TransformSourceCombiner::SetPlayMode( I3_ANIM_PLAY_MODE mode)
{
	for( UINT8 i = 0; i < m_BindCount; i++)
	{
		m_Binds[i].SetPlayMode( mode);
	}
}

void i3TransformSourceCombiner::RemoveAnimation( INT32 idx)
{
	if( idx == 0)
	{
		ShiftBind();
	}
	else
	{
		m_Binds[1].Reset();
		m_BindCount = 1;
	}
}

void i3TransformSourceCombiner::RemoveAllAnimation(void)
{
	for( UINT8 i = 0; i < m_BindCount; i++)
	{
		m_Binds[i].Reset();
	}

	m_BindCount = 0;
}

void i3TransformSourceCombiner::GetMatrix( I3MATRIXINFO * pInfo)
{
	i3TransformSourceBind * pBind = nullptr;
	QUATERNION	quat[2], quato;
	VEC3D		pos[2], poso;
	VEC3D		scale[2], scaleo;
	UINT32		key[2], keyo;
	MATRIX		mtx[2], * pOut;

	if( m_lastFrameID == pInfo->m_FrameID)
	{
		i3Matrix::Copy( pInfo->m_pOut, &m_CacheMatrix);
		return;
	}

	m_lastFrameID = pInfo->m_FrameID;

	//	time scale 적용
	pInfo->m_Time = pInfo->m_Time *m_timeScale;

	m_timeCur += pInfo->m_Time;

	if( m_BindCount == 0)
		return;

	pOut = pInfo->m_pOut;

	if( m_BindCount < 2)
	{
		pBind = (i3TransformSourceBind *) & m_Binds[0];

		pInfo->setOutVectors( &poso, &quato, &scaleo);

		pBind->GetInterpolatedKeyframe( pInfo);

		keyo = pInfo->m_KeyChannel;
	}
	else
	{
		REAL32 t;

		t = i3Math::div( m_timeCur , m_timeBlend);

		if( t > 1.0f)
		{
			// Blending이 끝난 상태이기 때문에
			// 두번째 Animation만 계산하면 된다.
			pBind = (i3TransformSourceBind *) & m_Binds[1];

			pInfo->setOutVectors( &poso, &quato, &scaleo);

			pBind->GetInterpolatedKeyframe( pInfo);

			keyo = pInfo->m_KeyChannel;

			ShiftBind();
			m_Binds[0].SetBlendMode( I3_ANIM_BLEND_MODE_REPLACE);
		}
		else
		{
			// Weighted Blend 상태라면...
			for( UINT8 i = 0; i < m_BindCount; i++)
			{
				pBind = (i3TransformSourceBind *) & m_Binds[i];

				pInfo->m_pOut = &mtx[i];

				pInfo->setOutVectors( &pos[i], &quat[i], &scale[i]);

				// 첫번째이거나, Replace로 Blend Mode가 설정되어 있다면,
				// 다른 모든 Matrix의 값을 무시하고 복사하면 된다.
				pBind->GetInterpolatedKeyframe( pInfo);

				key[i] = pInfo->m_KeyChannel;
			}

			// Position의 Interpolation
			if((key[0] & I3MATH_KEY_TRANSLATE) && (key[1] & I3MATH_KEY_TRANSLATE))
			{
				// Position의 Interpolation
				i3Vector::Lerp( &poso, &pos[0], &pos[1], t);
			}
			else if( key[0] & I3MATH_KEY_TRANSLATE)
			{
				i3Vector::Copy( &poso, &pos[0]);
			}
			else if( key[1] & I3MATH_KEY_TRANSLATE)
			{
				i3Vector::Copy( &poso, &pos[1]);
			}

			if((key[0] & I3MATH_KEY_SCALE) && (key[1] & I3MATH_KEY_SCALE))
			{
				// Scale의 Interpolation
				i3Vector::Lerp( &scaleo, &scale[0], &scale[1], t);
			}
			else if( key[0] & I3MATH_KEY_SCALE)
			{
				i3Vector::Copy( &scaleo, &scale[0]);
			}
			else if( key[1] & I3MATH_KEY_SCALE)
			{
				i3Vector::Copy( &scaleo, &scale[1]);
			}
			
			// Rotation Interpolation (SLERP)
			if((key[0] & I3MATH_KEY_ROTATE) && (key[1] & I3MATH_KEY_ROTATE))
			{
				// Rotation의 Interpolation
				//i3Quat::Lerp( &quato, &quat[0], &quat[1], t);
				//i3Quat::Normalize( &quato, &quato);
				i3Quat::Slerp( &quato, &quat[0], &quat[1], t);
			}
			else if( key[0] & I3MATH_KEY_ROTATE)
			{
				i3Quat::Copy( &quato, &quat[0]);
			}
			else if( key[1] & I3MATH_KEY_ROTATE)
			{
				i3Quat::Copy( &quato, &quat[1]);
			}

			keyo = key[0] | key[1];			// Channel
		}
	}

	pInfo->m_pOut = pOut;

	// 최종 Matrix
	if( keyo & I3MATH_KEY_ROTATE)
	{
		i3Matrix::SetRotateQuat( pOut, &quato);
	}
	else
	{
		i3Matrix::Identity( pOut);
	}

	if( keyo & I3MATH_KEY_SCALE)
	{
		i3Matrix::PreScale( pOut, i3Vector::GetX( &scaleo), i3Vector::GetY( &scaleo), i3Vector::GetZ( &scaleo));
	}

	if( keyo & I3MATH_KEY_TRANSLATE)
	{
		i3Matrix::SetPos( pOut, &poso);
	}
	else if( pInfo->m_pIn != nullptr)
	{	// 기본 Bone의 위치를 설정하도록 한다.
		i3Matrix::SetPos( pOut, i3Matrix::GetPos( pInfo->m_pIn));
	}

	if( m_pModifier)
	{
		m_pModifier->Apply( pOut);
	}

	i3Matrix::Copy( &m_CacheMatrix, pOut);
}

i3Animation * i3TransformSourceCombiner::getAnimation( INT32 idx)
{
	I3_BOUNDCHK( idx, m_BindCount);

	return m_Binds[idx].getAnimation();
}

INT32 i3TransformSourceCombiner::getAnimationBoneIndex( INT32 idx)
{
	I3_BOUNDCHK( idx, m_BindCount);

	return m_Binds[idx].getBoneIndex();
}

void i3TransformSourceCombiner::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3Animation * pAnim;

	for( UINT8 i = 0; i < m_BindCount; i++)
	{
		pAnim = m_Binds[i].getAnimation();

		pAnim->OnBuildObjectList( List);
	}

	i3PersistantElement::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED ANIMATION_COMBINER_1
	{
		INT8		m_ID[4] = { 'T', 'S', 'C', '2' };
		UINT8		m_BindCount = 0;
		REAL32		m_timeBlend = 0.0f;
	};

	struct PACKED TRANSFORMSOURCEBINDINFO
	{
		OBJREF				m_refTransformSource = 0;
		UINT8				m_PlayMode = 0;
		UINT8				m_BlendMode = 0;
	};

	struct PACKED TRANSFORMSOURCEBINDINFO2
	{
		OBJREF				m_refAnim = 0;
		INT32				m_idxBone = 0;
		UINT8				m_PlayMode = 0;
		UINT8				m_BlendMode = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3TransformSourceCombiner::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::ANIMATION_COMBINER_1 data;

	data.m_BindCount = m_BindCount;
	data.m_timeBlend = m_timeBlend;

	Rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	// i3TransformSourceBind reference
	for( UINT8 i = 0; i < data.m_BindCount; i++)
	{
		pack::TRANSFORMSOURCEBINDINFO2 info;

		info.m_refAnim		= pResFile->GetObjectPersistID( m_Binds[i].getAnimation());
		info.m_idxBone		= m_Binds[i].getBoneIndex();
		info.m_PlayMode		= (UINT8) m_Binds[i].GetPlayMode();
		info.m_BlendMode	= (UINT8) m_Binds[i].GetBlendMode();

		Rc = pResFile->Write( &info, sizeof(pack::TRANSFORMSOURCEBINDINFO2));
		I3_CHKIO( Rc);
		Result += Rc;
	}

	return Result;
}

UINT32 i3TransformSourceCombiner::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::ANIMATION_COMBINER_1 data;
	bool bOldVersion = false;

	Rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_BindCount = data.m_BindCount;
	m_timeBlend = data.m_timeBlend;

	if( memcmp( data.m_ID, I3_TRANSFORMSOURCECOMBINER_ID_1, 4) == 0)
		bOldVersion = true;

	if( bOldVersion == false)
	{
		for( UINT8 i = 0; i < data.m_BindCount; i++)
		{
			pack::TRANSFORMSOURCEBINDINFO2 info;

			Rc = pResFile->Read( &info, sizeof(info));
			I3_CHKIO( Rc);
			Result += Rc;

			// Animation
			{
				i3Animation * pAnim;

				pAnim = (i3Animation *) pResFile->FindObjectByID( info.m_refAnim);
				I3ASSERT( pAnim != nullptr);

				m_Binds[i].setAnimation( pAnim, info.m_idxBone);
			}

			// Play Mode
			m_Binds[i].SetPlayMode( (I3_ANIM_PLAY_MODE) info.m_PlayMode);

			// Blend mode
			m_Binds[i].SetBlendMode( (I3_ANIM_BLEND_MODE) info.m_BlendMode);
		}
	}
	else
	{
		m_BindCount = 0;
	}

	return Result;
}

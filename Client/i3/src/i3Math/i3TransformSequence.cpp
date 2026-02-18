#include "i3MathType.h"
#include "i3Matrix.h"
#include "i3Vector.h"
#include "i3Quat.h"
#include "i3TransformSequence.h"

I3_CLASS_INSTANCE( i3TransformSequence);

i3TransformSequence::i3TransformSequence(void)
{
	m_SourceType = I3MATH_TRANSFORM_SOURCE_QUATERNION;
}

i3TransformSequence::~i3TransformSequence(void)
{
	if( m_pBindBuf == nullptr)
	{
		i3::static_pool_free( m_pBuffer);
	}
	else
	{
		I3ASSERT(m_pBuffer == nullptr);
		I3_SAFE_RELEASE( m_pBindBuf);
	}
}

UINT32 i3TransformSequence::CalcBufferSize( INT32 KeyCount, I3_KEY_CHANNEL Channels)
{
	UINT32 Size = 0;

	if( Channels & TRANSLATE)
	{ 
		if( Channels & FIXED_TRANSLATE)
			Size += sizeof(VEC3D);
		else
			Size += sizeof(VEC3D) * KeyCount;
	}

	if( Channels & ROTATE)
	{
		if( Channels & FIXED_ROTATE)
			Size += sizeof(QUATERNION);
		else
			Size += sizeof(QUATERNION) * KeyCount;
	}

	if( Channels & SCALE)
	{
		if( Channels & FIXED_SCALE)
			Size += sizeof(VEC3D);
		else
			Size += sizeof(VEC3D) * KeyCount;
	}

	if( Channels & TIME)
	{
		Size += sizeof(REAL32) * KeyCount;
	}

	return Size;
}

INT32	i3TransformSequence::getTranslateKeyCount(void)
{
	if( HasTranslation())
	{
		if( m_Channels & FIXED_TRANSLATE)
			return 1;

		return m_KeyCount;
	}

	return 0;
}

INT32	i3TransformSequence::getRotateKeyCount(void)
{
	if( HasRotation())
	{
		if( m_Channels & FIXED_ROTATE)
			return 1;

		return m_KeyCount;
	}

	return 0;
}

INT32	i3TransformSequence::getScaleKeyCount(void)
{
	if( HasScale())
	{
		if( m_Channels & FIXED_SCALE)
			return 1;

		return m_KeyCount;
	}

	return 0;
}


INT32	i3TransformSequence::getTimeKeyCount(void)
{
	if( HasTime())
	{
		return m_KeyCount;
	}

	return 0;
}


void i3TransformSequence::Create( INT32 KeyCount, REAL32 Duration, I3_KEY_CHANNEL Channels)
{
	UINT32 BuffSz;

	BuffSz = CalcBufferSize( KeyCount, Channels);
	
	I3ASSERT(m_pBuffer == nullptr);
	m_pBuffer = (char *) i3::static_pool_malloc( BuffSz );
	
	Bind( m_pBuffer, KeyCount, Duration, Channels, nullptr);
}

void i3TransformSequence::Bind( char * pBuffer, INT32 KeyCount, REAL32 Duration, I3_KEY_CHANNEL Channels, i3MemoryBuffer * pMemBuf)
{
	UINT32 Accm = 0;

	m_KeyCount = KeyCount;
	m_Channels = Channels;
	m_Duration = Duration;

	if( Channels & TRANSLATE)
	{
		m_pTranslation = (VEC3D *) (pBuffer + Accm);

		if( Channels & FIXED_TRANSLATE)
			Accm += sizeof(VEC3D);
		else
			Accm += sizeof(VEC3D) * KeyCount;
	}

	if( Channels & ROTATE)
	{
		m_pRotation = (QUATERNION *) ( pBuffer + Accm);

		if( Channels & FIXED_ROTATE)
			Accm += sizeof(QUATERNION);
		else
			Accm += sizeof(QUATERNION) * KeyCount;
	}

	if( Channels & SCALE)
	{
		m_pScale = (VEC3D *) ( pBuffer + Accm);

		if( Channels & FIXED_SCALE)
			Accm += sizeof(VEC3D);
		else
			Accm += sizeof(VEC3D) * KeyCount;
	}

	if( Channels & TIME)
	{
		m_pTime = (REAL32 *) ( pBuffer + Accm);
	}

	I3_REF_CHANGE( m_pBindBuf, pMemBuf);
}

void i3TransformSequence::SetMatrix( INT32 Idx, MATRIX * pMatrix)
{
	VEC3D pos, scale;
	QUATERNION quat;

	if( m_Channels & TRANSLATE)
	{
		i3Vector::Copy( &pos, i3Matrix::GetPos( pMatrix));

		I3ASSERT( (Idx < 1) || !(m_Channels & FIXED_TRANSLATE));

		SetTranslation( Idx, &pos);
	}

	if( m_Channels & ROTATE)
	{
		i3Matrix::GetRotateQuat( &quat, pMatrix);

		I3ASSERT( (Idx < 1) || !(m_Channels & FIXED_ROTATE));

		SetRotation( Idx, &quat);
	}

	if( m_Channels & SCALE)
	{
		i3Matrix::GetScale( pMatrix, &scale);

		I3ASSERT( (Idx < 1) || !(m_Channels & FIXED_SCALE));

		SetScale( Idx, &scale);
	}
}

void i3TransformSequence::SetTranslation( INT32 Idx, VEC3D * pVec)
{
	VEC3D * pDest;

	I3ASSERT( m_pTranslation != nullptr);
	I3ASSERT( m_Channels & TRANSLATE);
	I3ASSERT( (Idx < 1) || !(m_Channels & FIXED_TRANSLATE));

	pDest = m_pTranslation + Idx;
	
	i3Vector::Copy( pDest, pVec);
}

void i3TransformSequence::SetRotation( INT32 Idx, QUATERNION * pQuat)
{
	QUATERNION * pDest;

	I3ASSERT( m_pRotation != nullptr);
	I3ASSERT( m_Channels & ROTATE);
	I3ASSERT( (Idx < 1) || !(m_Channels & FIXED_ROTATE));

	pDest = m_pRotation + Idx;

	i3Quat::Copy( pDest, pQuat);
}

void i3TransformSequence::SetScale( INT32 Idx, VEC3D * pVec)
{
	VEC3D * pDest;

	I3ASSERT( m_pScale != nullptr);
	I3ASSERT( m_Channels & SCALE);
	I3ASSERT( (Idx < 1) || !(m_Channels & FIXED_SCALE));

	pDest = m_pScale + Idx;

	i3Vector::Copy( pDest, pVec);
}

void i3TransformSequence::SetTime( INT32 Idx, REAL32 tm)
{
	REAL32 * pDest;

	I3ASSERT( m_pTime != nullptr);
	I3ASSERT( m_Channels & TIME);
	I3_BOUNDCHK( Idx, m_KeyCount);

	pDest = m_pTime + Idx;

	*pDest = tm;

	if((tm - m_pTime[0]) > m_Duration)
		m_Duration = tm - m_pTime[0];
}

void i3TransformSequence::GetKeyMatrix( INT32 Idx, MATRIX * pMatrix)
{
	i3Matrix::Identity( pMatrix);

	// Rotation
	if( m_Channels & ROTATE)
	{
		i3Matrix::SetRotateQuat( pMatrix, &m_pRotation[Idx]);
	}

	// Translation
	if( m_Channels & TRANSLATE)
	{
		i3Matrix::SetPos( pMatrix, &m_pTranslation[Idx]);
	}

	// Scale
	if( m_Channels & SCALE)
	{
		VEC3D * pVec = & m_pScale[Idx];

		i3Matrix::PreScale( pMatrix, i3Vector::GetX(pVec), i3Vector::GetY(pVec), i3Vector::GetZ(pVec));
	}
}

REAL32 i3TransformSequence::GetDuration(void)
{
	return m_Duration;
}

void i3TransformSequence::GetTranslation( INT32 Idx, VEC3D * pVec)
{
	I3ASSERT( m_pTranslation != nullptr);

	i3Vector::Copy( pVec, & m_pTranslation[Idx]);
}

void i3TransformSequence::GetRotation( INT32 Idx, QUATERNION * pQuat)
{
	I3ASSERT( m_pRotation != nullptr);

	i3Quat::Copy( pQuat, & m_pRotation[Idx]);
}
void i3TransformSequence::GetScale( INT32 Idx, VEC3D * pVec)
{
	I3ASSERT( m_pScale != nullptr);

	i3Vector::Copy( pVec, & m_pScale[Idx]);
}

REAL32 i3TransformSequence::GetTime( INT32 Idx)
{
	I3ASSERT( m_pTime != nullptr);

	return m_pTime[ Idx];
}

void i3TransformSequence::SetInterpolateMode( I3_KEY_CHANNEL Channel, I3_INTERPOLATE_MODE mode)
{
	if( Channel & TRANSLATE)
		m_TranslateInterpolateMode = mode;

	if( Channel & ROTATE)
		m_RotateInterpolateMode = mode;

	if( Channel & SCALE)
		m_ScaleInterpolateMode = mode;
}

UINT32 i3TransformSequence::_GetTimeIndex( REAL32 tm)
{
	register INT32 si, ei, mi;

	si = 0;
	ei = GetKeyframeCount() - 1;

	if( tm >= m_pTime[ei] )
	{
		return ei;
	}

	do
	{
		mi = (si + ei) >> 1;

		if((ei - si) <= 1)
		{
			break;
		}
		else if( m_pTime[mi] < tm)
		{
			si = mi;
		}
		else if( m_pTime[mi] > tm)
		{
			ei = mi;
		}
		else
		{
			si = mi;
			break;
		}
	} while( (ei - si) > 1);

	return si;
}

void  _copyPos( I3MATRIXINFO * pInfo, VEC3D * pSrc)
{
	if( pInfo->m_pOutTrans != nullptr)
		i3Vector::Copy( pInfo->m_pOutTrans, pSrc);
}

void  _lerpPos( I3MATRIXINFO * pInfo, VEC3D * pSrc1, VEC3D * pSrc2, REAL32 t)
{
	if( pInfo->m_pOutTrans != nullptr)
		i3Vector::Lerp( pInfo->m_pOutTrans, pSrc1, pSrc2, t);
}

void  _copyRot( I3MATRIXINFO * pInfo, QUATERNION * pSrc)
{
	if( pInfo->m_pOutRot != nullptr)
		i3Quat::Copy( pInfo->m_pOutRot, pSrc);
}

void  _lerpRot( I3MATRIXINFO * pInfo, QUATERNION * pQ1, QUATERNION * pQ2, REAL32 t)
{
	if( pInfo->m_pOutRot != nullptr)
	{
	#if 1
		QUATERNION qo;

		i3Quat::Lerp( &qo, pQ1, pQ2, t);
		i3Quat::Normalize( pInfo->m_pOutRot, &qo);
	#else
		i3Quat::Lerp( pInfo->m_pOutRot, pQ1, pQ2, t);
		i3Quat::Normalize( pInfo->m_pOutRot, pInfo->m_pOutRot);
	#endif
	}
}

void  _slerpRot( I3MATRIXINFO * pInfo, QUATERNION * pQ1, QUATERNION * pQ2, REAL32 t)
{
	if( pInfo->m_pOutRot != nullptr)
		i3Quat::Slerp( pInfo->m_pOutRot, pQ1, pQ2, t);
}

void  _copyScale( I3MATRIXINFO * pInfo, VEC3D * pSrc)
{
	if( pInfo->m_pOutScale != nullptr)
		i3Vector::Copy( pInfo->m_pOutScale, pSrc);
}

void  _lerpScale( I3MATRIXINFO * pInfo, VEC3D * pSrc1, VEC3D * pSrc2, REAL32 t)
{
	if( pInfo->m_pOutScale != nullptr)
		i3Vector::Lerp( pInfo->m_pOutScale, pSrc1, pSrc2, t);
}

UINT32 i3TransformSequence::GetInterpolatedKeyframe( I3MATRIXINFO * pInfo, bool bFirst)
{
	INT32 Idx, nextIdx;
	REAL32 t;
	REAL32 rTime = pInfo->m_Time;
	REAL32 _time;
	REAL32 interv = m_KeyInterval;
	UINT32 Key = 0;

	if( rTime < 0.0f)
		rTime = m_Duration + rTime;

	_time = rTime - m_TimeOffset;

	if (pInfo->m_PlayMode == I3_ANIM_PLAY_MODE_CLAMP ||
		pInfo->m_PlayMode == I3_ANIM_PLAY_MODE_MANUAL_CLAMP)
	{
		_time = i3Math::Min( m_Duration, _time );
	}
	else if( pInfo->m_PlayMode != I3_ANIM_PLAY_MODE_STOP)
	{
		if( _time >= m_Duration)
			_time = i3Math::fmod( _time, m_Duration);
	}

	if( m_Channels & TIME)
		Idx = _GetTimeIndex( _time);
	else
	{
		Idx = (UINT32)( i3Math::div( rTime, interv));
	}

	if( Idx >= (INT32)	GetKeyframeCount() - 1 )
	{
		Idx = GetKeyframeCount() - 1;

		// Key Optimize시 뒤의 키값이 삭제되어 일부 본만 다시 돌아가는 문제가 생긴다..
		//if( pInfo->m_PlayMode != I3_ANIM_PLAY_MODE_REPEAT)
		{
			t = 0.0f;
			nextIdx = Idx;
		}
		/*else
		{
			t = __RT_ONE;
			nextIdx = 0;
		}*/
	}
	else
	{
		nextIdx = Idx + 1;

		if( m_Channels & TIME )
		{
			REAL32 t1, t2;

			t1 = ( m_pTime[Idx]);
			t2 = ( m_pTime[nextIdx]);

			t = i3Math::div(( _time - t1), (( t2 - t1)));
			//I3ASSERT( (t >= __RT_0) && (t <= __RT_1));
		}
		else
		{
			t = i3Math::div( i3Math::fmod( _time, m_KeyInterval) , m_KeyInterval);
			//I3ASSERT( (t >= __RT_0) && (t <= __RT_1));
		}
	}

	// Translation
	if( m_Channels & TRANSLATE)
	{
		if( m_Channels & FIXED_TRANSLATE)
		{
			_copyPos( pInfo, &m_pTranslation[0]);
		}
		else
		{
			switch( m_TranslateInterpolateMode)
			{
				default :		_copyPos( pInfo, &m_pTranslation[Idx]);	break;
				case LINEAR :	_lerpPos( pInfo, &m_pTranslation[Idx], &m_pTranslation[nextIdx], t);	break;
				case SPLINE :
				case BEZIER :
				case SLERP :
					i3Log( "i3TransformSequence::GetMatrix()", "SPLINE and BEZIER interpolation is not implemented for translation.");
					break;
			}
		}

		Key |= I3MATH_KEY_TRANSLATE;
	}

	// Rotation
	if( m_Channels & ROTATE)
	{
		if( m_Channels & FIXED_ROTATE)
		{
			_copyRot( pInfo, &m_pRotation[0]);
		}
		else
		{
			switch( m_RotateInterpolateMode)
			{
				default :		_copyRot( pInfo, &m_pRotation[Idx]);	break;
				case LINEAR :	//_lerpRot( pInfo, pQ, &m_pRotation[Idx], &m_pRotation[nextIdx], t);	break;
				case SLERP  :
					_slerpRot( pInfo, &m_pRotation[Idx], &m_pRotation[nextIdx], t);
					break;
			}
		}

		Key |= I3MATH_KEY_ROTATE;
	}

	// Scale
	if( m_Channels & SCALE)
	{
		if( m_Channels & FIXED_SCALE)
		{
			_copyScale( pInfo, &m_pScale[0]);
		}
		else
		{
			switch( m_ScaleInterpolateMode)
			{
				default :		_copyScale( pInfo, &m_pScale[Idx]);		break;
				case LINEAR :	_lerpScale( pInfo, &m_pScale[Idx], &m_pScale[nextIdx], t);	break;
			}
		}

		Key |= I3MATH_KEY_SCALE;
	}

	pInfo->m_KeyChannel = Key;

	return Key;
}

void i3TransformSequence::GetMatrix( I3MATRIXINFO * pInfo, bool bFirst)
{
	QUATERNION Q, * pOldQ;
	VEC3D Pos, Scale, * pOldPos, * pOldScale;
	UINT32 Key;

	pOldPos		= pInfo->m_pOutTrans;
	pOldQ		= pInfo->m_pOutRot;
	pOldScale	= pInfo->m_pOutScale;

	pInfo->m_pOutTrans	= & Pos;
	pInfo->m_pOutRot	= & Q;
	pInfo->m_pOutScale	= & Scale;

	Key = i3TransformSequence::GetInterpolatedKeyframe( pInfo, bFirst);

	if( Key & I3MATH_KEY_ROTATE)
		i3Matrix::SetRotateQuat( pInfo->m_pOut, &Q);
	else
		i3Matrix::Identity( pInfo->m_pOut);

	if( Key & I3MATH_KEY_TRANSLATE)
		i3Matrix::SetPos( pInfo->m_pOut, &Pos);

	if( Key & I3MATH_KEY_SCALE)
		i3Matrix::PreScale( pInfo->m_pOut, i3Vector::GetX(&Scale), i3Vector::GetY(&Scale), i3Vector::GetZ(&Scale));

	pInfo->m_pOutTrans	= pOldPos;
	pInfo->m_pOutRot	= pOldQ;
	pInfo->m_pOutScale	= pOldScale;
}

void i3TransformSequence::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3TransformSource::CopyTo( pDest, method);

	if( !i3::same_of<i3TransformSequence*>(pDest) )
		return;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#pragma warning( push)
#pragma warning( disable : 4121)
#endif

namespace pack
{
	struct PACKED TRANSFORMSEQUENCE_1
	{
		UINT8			m_Version[2] = { 1, 0 };
		UINT8			m_Channels = 0;
		UINT8			m_TranslateInterpolateMode = 0;
		UINT8			m_RotateInterpolateMode = 0;
		UINT8			m_ScaleInterpolateMode = 0;
		INT64			m_KeyInterval = 0;
		INT32			m_KeyCount = 0;
		INT64			m_TimeOffset = 0;
		INT64			m_Duration = 0;
	};

	struct PACKED TRANSFORMSEQUENCE_2
	{
		UINT8			m_Version[2] = { 2, 0 };
		UINT8			m_Channels = 0;
		UINT8			m_TranslateInterpolateMode = 0;
		UINT8			m_RotateInterpolateMode = 0;
		UINT8			m_ScaleInterpolateMode = 0;
		REAL32			m_KeyInterval = 0.0f;
		INT32			m_KeyCount = 0;
		REAL32			m_TimeOffset = 0.0f;
		REAL32			m_Duration = 0.0f;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#pragma warning( pop)
#endif

UINT32 i3TransformSequence::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	INT32 i, Count;
	pack::TRANSFORMSEQUENCE_2 Info;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Info.m_Channels = (UINT8) m_Channels;
	Info.m_TranslateInterpolateMode = (UINT8) m_TranslateInterpolateMode;
	Info.m_RotateInterpolateMode = (UINT8) m_RotateInterpolateMode;
	Info.m_ScaleInterpolateMode = (UINT8) m_ScaleInterpolateMode;
	Info.m_KeyCount = m_KeyCount;

	Info.m_KeyInterval = m_KeyInterval;
	Info.m_TimeOffset = m_TimeOffset;
	Info.m_Duration = m_Duration;

	Rc = pStream->Write( &Info, sizeof(pack::TRANSFORMSEQUENCE_2));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TransformSequence::OnSave()", "Could not write animation information.");
		return STREAM_ERR;
	}
	Result += Rc;

	// Translation
	if( HasTranslation())
	{
		i3::pack::VEC3D pvec;
		VEC3D vec;

		if( m_Channels & FIXED_TRANSLATE)
			Count = 1;
		else
			Count = m_KeyCount;

		for( i = 0; i < Count; i++)
		{
			GetTranslation( i, &vec);

			pvec.x = getX( &vec);
			pvec.y = getY( &vec);
			pvec.z = getZ( &vec);

			Rc = pStream->Write( &pvec, sizeof(i3::pack::VEC3D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TransformSequence::OnSave()", "Could not write translation keyframe data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	// Rotation
	if( HasRotation())
	{
		i3::pack::VEC4D pvec;
		QUATERNION quat;

		if( m_Channels & FIXED_ROTATE)
			Count = 1;
		else
			Count = m_KeyCount;

		for( i = 0; i < Count; i++)
		{
			GetRotation( i, &quat);

			pvec.x = i3Quat::GetA( &quat);
			pvec.y = i3Quat::GetB( &quat);
			pvec.z = i3Quat::GetC( &quat);
			pvec.w = i3Quat::GetD( &quat);

			Rc = pStream->Write( &pvec, sizeof(i3::pack::VEC4D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TransformSequence::OnSave()", "Could not write rotation keyframe data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	// Scale
	if( HasScale())
	{
		i3::pack::VEC3D pvec;
		VEC3D vec;

		if( m_Channels & FIXED_SCALE)
			Count = 1;
		else
			Count = m_KeyCount;

		for( i = 0; i < Count; i++)
		{
			GetScale( i, &vec);

			pvec.x = getX( &vec);
			pvec.y = getY( &vec);
			pvec.z = getZ( &vec);

			Rc = pStream->Write( &pvec, sizeof(i3::pack::VEC3D));
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TransformSequence::OnSave()", "Could not write scale keyframe data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
	}

	// Time
	if( HasTime())
	{
		Rc = pStream->Write( m_pTime, sizeof(REAL32) * m_KeyCount);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TransformSequence::OnSave()", "Could not write time keyframe data.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3TransformSequence::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result, Pos;
	INT32 i, Count;
	pack::TRANSFORMSEQUENCE_2 Info;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3NamedElement::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	Pos = pStream->GetPosition();

	Rc = pStream->Read( &Info, sizeof(pack::TRANSFORMSEQUENCE_2));
	if( Rc == STREAM_ERR)
	{
		i3Log( "i3TransformSequence::OnLoad()", "Could not read animation information.");
		return STREAM_ERR;
	}

	if( Info.m_Version[0] == 1)
	{
		// 예전 Version
		pack::TRANSFORMSEQUENCE_1 Info1;

		pStream->SetPosition( Pos, STREAM_BEGIN);

		Rc = pStream->Read( &Info1, sizeof(pack::TRANSFORMSEQUENCE_1));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TransformSequence::OnLoad()", "Could not read animation information.");
			return STREAM_ERR;
		}

		Info.m_Version[0]				= Info1.m_Version[0];
		Info.m_Version[1]				= Info1.m_Version[1];
		Info.m_Channels					= Info1.m_Channels;
		Info.m_KeyCount					= Info1.m_KeyCount;
		Info.m_RotateInterpolateMode	= Info1.m_RotateInterpolateMode;
		Info.m_TranslateInterpolateMode = Info1.m_TranslateInterpolateMode;
		Info.m_ScaleInterpolateMode		= Info1.m_ScaleInterpolateMode;
		Info.m_Duration					= (REAL32)(Info1.m_Duration * 1.0e-9);
		Info.m_KeyInterval				= (REAL32)(Info1.m_KeyInterval * 1.0e-9);
		Info.m_TimeOffset				= (REAL32)(Info1.m_TimeOffset * 1.0e-9);
	}
	Result += Rc;

	Create( Info.m_KeyCount, Info.m_Duration, (I3_KEY_CHANNEL) Info.m_Channels);

	SetInterpolateMode( TRANSLATE, (I3_INTERPOLATE_MODE) Info.m_TranslateInterpolateMode);
	SetInterpolateMode( ROTATE, (I3_INTERPOLATE_MODE) Info.m_RotateInterpolateMode);
	SetInterpolateMode( SCALE, (I3_INTERPOLATE_MODE) Info.m_ScaleInterpolateMode);

	SetKeyframeInterval( Info.m_KeyInterval);
	SetTimeOffset( Info.m_TimeOffset);

	// Translation
	if( HasTranslation())
	{
		if( m_Channels & FIXED_TRANSLATE)
			Count = 1;
		else
			Count = m_KeyCount;

		Rc = pStream->Read( m_pTranslation, sizeof(i3::pack::VEC3D) * Count);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TransformSequence::OnLoad()", "Could not read translation keyframe data.");
			return STREAM_ERR;
		}
		Result += Rc;

		#if !defined( I3_WINDOWS) || defined( I3_FIXED_POINT)

			if( sizeof(VEC3D) != sizeof(i3::pack::VEC3D))
			{
				VEC3D * pDest;
				i3::pack::VEC3D * pSrc;

				pDest = (VEC3D *)((char *)m_pTranslation + (sizeof(VEC3D) * (m_KeyCount - 1)));
				pSrc = (i3::pack::VEC3D *)((char *)m_pTranslation + (sizeof(i3::pack::VEC3D) * (m_KeyCount - 1)));

				for( i = 0; i < m_KeyCount; i++)
				{
					i3Vector::Set( pDest, pSrc->x, pSrc->y, pSrc->z);

					pDest--;
					pSrc--;
				}
			}
		#endif
	}

	// Rotation
	if( HasRotation())
	{
		if( m_Channels & FIXED_ROTATE)
			Count = 1;
		else
			Count = m_KeyCount;

		Rc = pStream->Read( m_pRotation, sizeof(i3::pack::VEC4D) * Count);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TransformSequence::OnSave()", "Could not read rotation keyframe data.");
			return STREAM_ERR;
		}
		Result += Rc;

		#if !defined( I3_WINDOWS)
			if( sizeof(VEC4D) != sizeof(i3::pack::VEC4D))
			{
				QUATERNION * pDest;
				i3::pack::VEC4D * pSrc;

				pDest = (QUATERNION *)((char *) m_pRotation + (sizeof(QUATERNION) * (m_KeyCount - 1)));
				pSrc = (i3::pack::VEC4D *)((char *)m_pRotation + (sizeof(i3::pack::VEC4D) * (m_KeyCount - 1)));

				for( i = 0; i < m_KeyCount; i++)
				{
					i3Quat::Set( pDest, pSrc->x, pSrc->y, pSrc->z, pSrc->w);

					pDest--;
					pSrc--;
				}
			}
		#endif
	}

	// Scale
	if( HasScale())
	{
		if( m_Channels & FIXED_SCALE)
			Count = 1;
		else
			Count = m_KeyCount;

		Rc = pStream->Read( m_pScale, sizeof(i3::pack::VEC3D) * Count);
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3TransformSequence::OnLoad()", "Could not read scale keyframe data.");
			return STREAM_ERR;
		}
		Result += Rc;

		#if !defined( I3_WINDOWS) || defined( I3_FIXED_POINT)
			if( sizeof(VEC3D) != sizeof(i3::pack::VEC3D))
			{
				VEC3D * pDest;
				i3::pack::VEC3D * pSrc;

				pDest = (VEC3D *)((char *)m_pScale + (sizeof(VEC3D) * (m_KeyCount - 1)));
				pSrc = (i3::pack::VEC3D *)((char *)m_pScale + (sizeof(i3::pack::VEC3D) * (m_KeyCount - 1)));

				for( i = 0; i < m_KeyCount; i++)
				{
					i3Vector::Set( pDest, pSrc->x, pSrc->y, pSrc->z);

					pDest--;
					pSrc--;
				}
			}
		#endif

	}

	// Time
	if( HasTime())
	{
		if( Info.m_Version[0] == 1)
		{
			INT64 * pTime;
			
			//INT32 mIdx;
			//mIdx = i3MemAlloc( sizeof(INT64) * m_KeyCount);
			pTime = (INT64 *) i3MemAlloc( sizeof(INT64) * m_KeyCount);
			I3ASSERT( pTime);

			// 예전 INT64 형식을 사용하는 Time Data
			Rc = pStream->Read( pTime, sizeof(INT64) * m_KeyCount);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TransformSequence::OnLoad()", "Could not read time keyframe data.");
				i3MemFree(pTime);
				return STREAM_ERR;
			}
			Result += Rc;

			for( i = 0; i < m_KeyCount; i++)
			{
				m_pTime[i] = (REAL32)(pTime[i] * 1.0e-9);
			}

			//i3mem::Free( mIdx );
			i3MemFree(pTime); 
		}
		else
		{
			// REAL32 형식을 사용하는 Newer Version
			Rc = pStream->Read( m_pTime, sizeof(REAL32) * m_KeyCount);
			if( Rc == STREAM_ERR)
			{
				i3Log( "i3TransformSequence::OnSave()", "Could not read time keyframe data.");
				return STREAM_ERR;
			}
			Result += Rc;
		}
/*
		if( m_KeyCount > 2)
		{
			REAL32 step, gap;

			SameCount = 2;
			step = m_pTime[1] - m_pTime[0];

			for( i = 2; i < m_KeyCount; i++)
			{
				gap = m_pTime[i] - m_pTime[i - 1];

				if( i3Math::abs( gap - step) < I3_EPS)
				{
					SameCount++;
				}
			}

			if( SameCount == m_KeyCount)
			{
				// Time Key 제거
				m_KeyInterval = step;

				//m_Channels = (I3_KEY_CHANNEL)((UINT32) m_Channels & ~TIME);
			}
		}
		*/

		#if defined( I3_DEBUG)
			CheckTimeTable();
		#endif
	}

	return Result;
}

#if defined( I3_DEBUG)
void i3TransformSequence::CheckTimeTable(void)
{
	if( m_Channels & TIME)
	{
		for( INT32 i = 0; i < m_KeyCount - 1; i++)
		{
			I3ASSERT( m_pTime[i] < m_pTime[i + 1]);
		}
	}
	else
	{
		I3ASSERT( m_KeyInterval > I3_EPS);
	}
}
#endif

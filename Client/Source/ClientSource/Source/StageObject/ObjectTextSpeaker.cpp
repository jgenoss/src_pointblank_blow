#include "pch.h"
#include "GlobalVariables.h"
#include "ObjectTextSpeaker.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "i3Base/string/ext/contain_string_head.h"

I3_CLASS_INSTANCE( CGameObjectTextSpeaker);//, CGameControledObject);

CGameObjectTextSpeaker::CGameObjectTextSpeaker()
{
	m_nTextType			= 0;
	m_rTextShowTime		= 0.f;
	m_rViewLength		= 0.f;
	
	m_nTextViewIndex	= -1;
	m_rAccumulateTime	= 0.f;
	m_bUpdate			= false;
	m_bFirstUpdate		= false;
	i3Vector::Zero( &m_vCenterPos);
}

CGameObjectTextSpeaker::~CGameObjectTextSpeaker()
{
}

struct PERSIST_OBJTEXTSPEAKER
{
	UINT32			nVer;
	UINT32			textType;			// 텍스트 종류
	REAL32			textShowTime;		// 텍스트 변경 타임
	REAL32			viewLength;			// 텍스트 활성화 거리
	UINT32			pad[4];
} ;

UINT32 CGameObjectTextSpeaker::OnQueryGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, void * * ppData)
{	
	/*
	(*ppData) = i3MemAlloc( sizeof( PERSIST_OBJTEXTSPEAKER));

	i3mem::FillZero( *ppData, sizeof(PERSIST_OBJTEXTSPEAKER));

	PERSIST_OBJTEXTSPEAKER * pInfo = (PERSIST_OBJTEXTSPEAKER*) (*ppData);

	pInfo->nVer			= 101;
	pInfo->textType		= m_nTextType;
	pInfo->textShowTime	= m_rTextShowTime;
	pInfo->viewLength	= m_rViewLength;

	return sizeof( PERSIST_OBJTEXTSPEAKER);
	*/
	return 0;
}

UINT32 CGameObjectTextSpeaker::OnLoadGameData( i3GameSceneInfo * pInfo, i3GameObjRef * pRefObj, UINT32 sz)
{
	/*
	UINT32 Rc;

	PERSIST_OBJTEXTSPEAKER info;
	
	Rc = pStream->Read( &info, sizeof( PERSIST_OBJTEXTSPEAKER));
	I3_CHKIO( Rc);

	m_nTextType		= info.textType;
	m_rTextShowTime = info.textShowTime;
	m_rViewLength	= info.viewLength;

	return Rc;
	*/
	return 0;
}

void CGameObjectTextSpeaker::InitObject()
{
	//MATRIX mat;
	i3Vector::Zero( &m_vCenterPos);
	if( (i3::contain_string_head( getSgRes()->GetName(), "Liviona_Setting.i3s") ))
	{
		m_nTextType = 0;
	}
	else if( (i3::contain_string_head( getSgRes()->GetName(), "Dark_Horror_Setting.i3s") ))
	{
		m_nTextType = 1;
		i3Vector::Set( &m_vCenterPos, 0.f, 1.4f, 0.8f);
		MATRIX mat;

		i3Matrix::Copy( &mat, GetMatrix());
		i3Matrix::SetPos( &mat, 0.f, 0.f, 0.f, 1.f);
		i3Vector::TransformCoord( &m_vCenterPos, &m_vCenterPos, &mat);
	}


	/*
	i3Body * pBody = getSceneObject()->GetBody();
	i3Skeleton * pSkel = pBody->getFrameLOD()->getSkeleton();
	if( pSkel->getBoneCount() > 0)
	{
		for( INT32 i = 0; i < pSkel->getBoneCount(); i++)
		{
			if( pSkel->isShapeSet( i))
			{
				i3PhysixShapeSet * pShapeSet = pSkel->getShapeSet( i);
				pShapeSet->getMassLocalPose( &mat);

				i3Vector::Add( &m_vCenterPos, &m_vCenterPos, i3Matrix::GetPos( &mat));
			}
		}

		i3Vector::Scale( &m_vCenterPos, &m_vCenterPos, 1.f / (REAL32) pSkel->getBoneCount());
	}
	*/
}

void CGameObjectTextSpeaker::ResetObject()
{
}

void CGameObjectTextSpeaker::EnableObject()
{
}

void CGameObjectTextSpeaker::SetControledObjectState( INT32 state)
{
}

void CGameObjectTextSpeaker::ProcessObject( REAL32 rDeltaSeconds)
{	
	CGameControledObject::ProcessObject(rDeltaSeconds);
	// 리니지 맵
	VEC3D vLen;
	REAL32 rPosX, rPosY;

	if( !g_pFramework->IsBattleStage())
		return;

	m_rAccumulateTime += rDeltaSeconds;
	
	if( !m_bUpdate )
	{
		i3Vector::Sub( &vLen, i3Matrix::GetPos( g_pCamera->getCamMatrix()), GetPos() );
		//i3Vector::Sub( &vLen, g_pPlayer->GetPos(), GetPos() );
		REAL32 rLen = i3Vector::Length( &vLen);

		if( rLen <= 5.f) //m_rViewLength)
		{
			m_bUpdate = true;
			if( !m_bFirstUpdate )
			{
				m_rAccumulateTime = 10.f; //m_rTextShowTime;
				m_bFirstUpdate = true;
			}
		}
		else
		{
			m_bFirstUpdate = false;
			if( m_nTextViewIndex != -1)
			{
				if(CHud::i() == nullptr)
					CHud::i()->DisableObjectReply( m_nTextViewIndex);
				m_nTextViewIndex = -1;
			}
		}
	}


	if( m_bUpdate) 
	{
		VEC3D	vTemp;
		VEC3D	vTempIn;
		MATRIX	CamMat;
		VEC3D*	pvCamAt;
		VEC3D	vSub;

		pvCamAt = i3Matrix::GetAt( g_pCamera->getCamMatrix());

		i3Vector::Sub( &vSub, i3Matrix::GetPos( g_pCamera->getCamMatrix()), GetPos() );
//		i3Vector::Sub( &vSub, g_pPlayer->GetPos(), GetPos() );
		i3Vector::Normalize( &vSub, &vSub );

		if( i3Vector::Dot( pvCamAt, &vSub) < 0.f ) 
		{
			if( m_nTextViewIndex != -1)
			{
				if(CHud::i() == nullptr)
					CHud::i()->DisableObjectReply( m_nTextViewIndex);
				m_nTextViewIndex = -1;
			}
			m_bFirstUpdate = false;
			return;
		}	
		
		i3Matrix::Mul( &CamMat, g_pCamera->getViewMatrix(), g_pCamera->getProjectionMatrix());

		i3Vector::Set( &vTempIn, i3Vector::GetX( GetPos() ), i3Vector::GetY( GetPos() ) + 1.9f, i3Vector::GetZ( GetPos() ) );
		i3Vector::Add( &vTempIn, &vTempIn, &m_vCenterPos);
		
		i3Vector::TransformCoord( &vTemp, &vTempIn, &CamMat );

		rPosX	= (getX( &vTemp ) * 0.5f) + 0.5f;
		rPosY	= (getY( &vTemp ) * -0.5f) + 0.5f;

		if(CHud::i() == nullptr)
		{
			if(	m_rAccumulateTime >= 10.f )// m_rTextShowTime )
			{
				if( m_nTextViewIndex != -1)
				{
					CHud::i()->DisableObjectReply( m_nTextViewIndex);
				}

				m_nTextViewIndex = CHud::i()->SetObjectReply( m_nTextType, rPosX, rPosY);
				m_rAccumulateTime = 0.f;
			}

			if( m_nTextViewIndex != -1)
			{
				CHud::i()->SetObjectReplyPos( m_nTextViewIndex, rPosX, rPosY);
			}
		}

		m_bUpdate = false;
	}
}


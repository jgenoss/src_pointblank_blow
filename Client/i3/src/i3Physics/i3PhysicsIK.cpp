#include "i3PhysicsType.h"
#include "i3PhysicsAdvanceBone.h"
#include "i3PhysicsIK.h"

I3_CLASS_INSTANCE( i3PhysicsIK, i3ElementBase );

i3PhysicsIK::i3PhysicsIK()
{
	m_bDamping = FALSE;
	m_bRestriction = FALSE;

	m_Thresh = 0.0f;
}

i3PhysicsIK::~i3PhysicsIK()
{
	RemoveAllBone();
}

void i3PhysicsIK::Create()
{
}

void i3PhysicsIK::AddBone( i3PhysicsAdvanceBone * pObj, i3Transform * pBone, i3Transform * pParent )
{
	pObj->AddRef();

	pObj->setBone( pBone );
	pObj->setParentBone( pParent );

	m_IKBoneList.Add( pObj );
}

void i3PhysicsIK::RemoveAllBone()
{
	INT32 i;
	i3PhysicsAdvanceBone * pObj;

	for( i = 0; i < getBoneCount(); i++ )
	{
		pObj = getAdvanceBone( i );

		pObj->Release();
	}
	m_IKBoneList.Clear();
}

INT32 i3PhysicsIK::_FindAdvanceBoneIndexByBoneAddress( i3Transform * pBoneAddress )
{
	INT32 i;
	i3Transform * pBone;

	for( i = 0; i < getBoneCount(); i++ )
	{
		pBone = getBone(i);

		if( pBone == pBoneAddress )
			return i;
	}

	return -1;
}

i3PhysicsAdvanceBone * i3PhysicsIK::_FindAdvanceBoneByBoneAddress( i3Transform * pBoneAddress )
{
	INT32 i;
	i3PhysicsAdvanceBone * pAdvanceBone;
	i3Transform * pBone;

	for( i = 0; i < getBoneCount(); i++ )
	{
		pAdvanceBone = getAdvanceBone(i);
		I3ASSERT( pAdvanceBone != NULL );
		pBone = pAdvanceBone->getBone();

		if( pBone == pBoneAddress )
			return pAdvanceBone;
	}

	return NULL;
}


///////////////////////////////////////////////////////////////////////////
// 
void i3PhysicsIK::CalcCCDLink( VEC3D * pEffectPoint, i3Transform * pEffectBone )
{	
	i3PhysicsAdvanceBone * pAdvanceBone;
	i3Transform * pBone;
	i3Transform * pLinkBone;
	MATRIX Mat;
	VEC3D *pVecRoot;
	VEC3D *pVecCurrentEnd;
	VEC3D Vec, VecCurrent, VecTarget, VecCross;
	REAL32 effectLength = m_Thresh;
	REAL32 cosAngle, turnAngle, turnDeg;
	INT32 linkCount = 0;
	BOOL bCalcloof = FALSE;

	pLinkBone = pEffectBone;
	if( pLinkBone == NULL )
		I3WARN( "[i3PhysicsIK::CalcCCDLink()] - pLinkBone == NULL" );

	// 영향을 받는 Bone의 좌표를 가져온다.
	pVecCurrentEnd = i3Matrix::GetPos( pEffectBone->GetMatrix() );

	do
	{	
		pAdvanceBone = _FindAdvanceBoneByBoneAddress( pLinkBone );
		I3ASSERT( pAdvanceBone != NULL );

		// 영향을 받는 Bone의 길이 계산을 통하여 도달 정도를 비교한다.
		i3Vector::Sub( &Vec, pVecCurrentEnd, pEffectPoint );
		effectLength = i3Vector::Length( &Vec );

		if( effectLength > m_Thresh )
		{
			// RootBone의 좌표를 가져온다.
			pBone = pAdvanceBone->getParentBone();
			I3ASSERT( pBone != NULL );
			pVecRoot = i3Matrix::GetPos( pBone->GetMatrix() );

			// 현재 영향을 받는 Bone의 현재 위치 벡터를 구한다.
			i3Vector::Sub( &Vec, pVecCurrentEnd, pVecRoot );
			i3Vector::Normalize( &VecCurrent, &Vec );

			// 영향을 받는 위치로의 벡터
			i3Vector::Sub( &Vec, pEffectPoint, pVecRoot );
			i3Vector::Normalize( &VecTarget, &Vec );

			// 현재 벡터에서 영향을 받는 벡터로의 각도 계산
			cosAngle = i3Vector::Dot( &VecCurrent, &VecTarget );

			// IF THE DOT PRODUCT RETURNS 1.0, I DON'T NEED TO ROTATE AS IT IS 0 DEGREES
			if( cosAngle < 0.99999f )
			{
				// 회전 방향을 구하기 위한 Cross product
				i3Vector::Cross( &VecCross, &VecCurrent, &VecTarget );
				
				turnAngle = i3Math::acos( cosAngle );	// GET THE ANGLE
				turnDeg = I3_RAD2DEG( turnAngle );		// COVERT TO DEGREES

				// DAMPING
				if( m_bDamping)
				{
					turnDeg = i3Math::Mul( turnDeg, pAdvanceBone->getDampStrength() );

					if( turnDeg > pAdvanceBone->getDampWidth() )
						turnDeg = pAdvanceBone->getDampWidth();
				}	

				// DOF RESTRICTIONS
				if( m_bRestriction )
				{
					i3Matrix::Identity( &Mat );

					if( i3Vector::GetZ( &VecCross ) > 0.0f )
					{
						i3Vector::Scale( pAdvanceBone->getDirection(), &VecCross, -turnDeg );

						if( i3Vector::GetX( pAdvanceBone->getDirection() ) < pAdvanceBone->getMinRotationX() )
							i3Vector::SetX( pAdvanceBone->getDirection(), pAdvanceBone->getMinRotationX() );
						if( i3Vector::GetY( pAdvanceBone->getDirection() ) < pAdvanceBone->getMinRotationY() )
							i3Vector::SetY( pAdvanceBone->getDirection(), pAdvanceBone->getMinRotationY() );
						if( i3Vector::GetZ( pAdvanceBone->getDirection() ) < pAdvanceBone->getMinRotationZ() )
							i3Vector::SetZ( pAdvanceBone->getDirection(), pAdvanceBone->getMinRotationZ() );
					}
					else if( i3Vector::GetZ( &VecCross ) < 0.0f )
					{
						i3Vector::Scale( pAdvanceBone->getDirection(), &VecCross, turnDeg );

						if( i3Vector::GetX( pAdvanceBone->getDirection() ) > pAdvanceBone->getMaxRotationX() )
							i3Vector::SetX( pAdvanceBone->getDirection(), pAdvanceBone->getMaxRotationX() );
						if( i3Vector::GetY( pAdvanceBone->getDirection() ) > pAdvanceBone->getMaxRotationY() )
							i3Vector::SetY( pAdvanceBone->getDirection(), pAdvanceBone->getMaxRotationY() );
						if( i3Vector::GetZ( pAdvanceBone->getDirection() ) > pAdvanceBone->getMaxRotationZ() )
							i3Vector::SetZ( pAdvanceBone->getDirection(), pAdvanceBone->getMaxRotationZ() );
					}
					
					turnDeg = i3Vector::Normalize( pAdvanceBone->getDirection(), pAdvanceBone->getDirection() );
				}
				else
					i3Vector::Copy( pAdvanceBone->getDirection(), &VecCross );
				
				if( i3Vector::GetZ( pAdvanceBone->getDirection() ) > 0.0f )	// CD
					i3Matrix::PostRotateAxisDeg( pBone->GetMatrix(), pAdvanceBone->getDirection(), -turnDeg );
				else if( i3Vector::GetZ( pAdvanceBone->getDirection() ) < 0.0f ) // CCD
					i3Matrix::PostRotateAxisDeg( pBone->GetMatrix(), pAdvanceBone->getDirection(), turnDeg );
			}
		}
		
		pLinkBone = pAdvanceBone->getParentBone();
		if( pLinkBone == NULL )
		{
			pLinkBone = pEffectBone;
			bCalcloof = TRUE;
		}
		else if( !bCalcloof )
			linkCount++;
		else
			linkCount--;

	} while ( effectLength > m_Thresh && linkCount > 0);
}

void i3PhysicsIK::StepSimulation( REAL32 dTime )
{
}


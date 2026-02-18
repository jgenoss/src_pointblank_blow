#include "i3SceneDef.h"
#include "i3Collidee.h"
#include "i3CollideeSphere.h"
#include "i3ColliderSet.h"
#include "i3CollideeMesh.h"

I3_CLASS_INSTANCE( i3CollideeSphere);

void i3CollideeSphere::Transform( MATRIX * pMatrix, i3Collidee * pCol)
{
	i3CollideeSphere * pDest = (i3CollideeSphere *) pCol;

	I3ASSERT( i3::same_of<i3CollideeSphere*>(pDest));

	i3Matrix::TransformPoints( pDest->GetCenter(), &m_Center, 1, pMatrix);
	pDest->m_Radius = m_Radius;
	pDest->m_RadiusSqr = m_RadiusSqr;
}

/////////////////////////////////////////////////////////////////////////////////////////////
/*
INT32	i3CollideeSphere::TestWithNode( i3Node * pNode, I3_COLLISION_RESULT * pResultBuf, INT32 MaxResult)
{
	UINT32 c = 0, accm = 0, i;

	if( pNode->GetBound() != nullptr)
	{
		i3BoundBox * pBox = pNode->GetBound();

		if( ContainAABBoxSphere( GetCenter(), GetRadius(), pBox->GetMin(), pBox->GetMax()) == false)
		{
			return 0;
		}
	}

	if( pNode->IsExactTypeOf( i3ColliderSet::GetClassMeta()))
	{
		i3ColliderSet * pColSet = (i3ColliderSet *) pNode;
		i3Collider * pCol;
		i3CollideeMesh * pMesh;

		for( i = 0; i < (UINT32) pColSet->GetColliderCount(); i++)
		{
			pCol = pColSet->GetCollider(i);
			pMesh = (i3CollideeMesh *) pCol->GetCollidee();

			I3ASSERT( pMesh != nullptr );
			c = pMesh->TestSphere( this, pResultBuf, MaxResult, 0);

			accm += c;
			pResultBuf += c;

			MaxResult -= c;
			if( MaxResult <= 0)
				return accm;
		}
	}

	for( i = 0; i < (UINT32) pNode->GetChildCount(); i++)
	{
		i3Node * pChild = pNode->GetChild( i);

		c = TestWithNode( pChild, pResultBuf, MaxResult);
		
		accm += c;
		pResultBuf += c;

		MaxResult -= c;
		if( MaxResult <= 0)
			return accm;

	}

	return accm;
}
*/

UINT32 i3CollideeSphere::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Collidee::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Center
	{
		i3::pack::VEC3D vec;

		vec.x = i3Vector::GetX( &m_Center);
		vec.y = i3Vector::GetY( &m_Center);
		vec.z = i3Vector::GetZ( &m_Center);
		
		Rc = pStream->Write( &vec, sizeof(i3::pack::VEC3D));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeSphere::OnSave()", "Could not write center information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// Radius
	{
		Rc = pStream->Write( &m_Radius, sizeof(REAL32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeSphere::OnSave()", "Could not write radius information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3CollideeSphere::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Collidee::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Center
	{
		i3::pack::VEC3D vec;

		Rc = pStream->Read( &vec, sizeof( i3::pack::VEC3D));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeSphere::OnLoad()", "Could not read center information.");
			return STREAM_ERR;
		}
		Result += Rc;

		i3Vector::Set( &m_Center, vec.x, vec.y, vec.z);
	}

	// Radius
	{
		Rc = pStream->Read( &m_Radius, sizeof(REAL32));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeSphere::OnLoad()", "Could not read radius information.");
			return STREAM_ERR;
		}
		Result += Rc;

		SetRadius( m_Radius);
	}

	return Result;
}


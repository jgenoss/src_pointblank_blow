#include "i3SceneDef.h"
#include "i3Collidee.h"
#include "i3CollideeLine.h"
#include "i3ColliderSet.h"
#include "i3CollideeMesh.h"

I3_CLASS_INSTANCE( i3CollideeLine);

void i3CollideeLine::Transform( MATRIX * pMatrix, i3Collidee * pCol)
{
	i3CollideeLine * pDest = (i3CollideeLine *) pCol;

	I3ASSERT( i3::same_of<i3CollideeLine*>(pDest));

	i3Matrix::TransformPoints( pDest->GetStart(), &m_Start, 1, pMatrix);
	i3Matrix::TransformVectors( &pDest->m_Dir, &m_Dir, 1, pMatrix);
	i3Vector::Add( pDest->GetEnd(), pDest->GetStart(), pDest->GetDir());
}

/////////////////////////////////////////////////////////////////////////////////////////////
/*
INT32	i3CollideeLine::TestWithNode( i3Node * pNode, I3_COLLISION_RESULT * pResultBuf, INT32 MaxResult, UINT32 Style )
{
	UINT32 c = 0, accm = 0, i;

	if( pNode->GetBound() != nullptr)
	{
		i3BoundBox * pBox = pNode->GetBound();

		if( ContainAABBoxLine( &m_Start, &m_End, pBox->GetMin(), pBox->GetMax(), 10000000000000.0f) == false)
		{
			return 0;
		}
	}

	if( Style & I3_COLLISION_STYLE_NEAREST_ONE )
	{
		pResultBuf->m_T = 1.0f;
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
			c = pMesh->TestLine( this, pResultBuf, MaxResult, Style );

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

		c = TestWithNode( pChild, pResultBuf, MaxResult, Style );

		accm += c;
		pResultBuf += c;

		MaxResult -= c;
		if( MaxResult <= 0)
			return accm;

	}

	return accm;
}
*/

UINT32 i3CollideeLine::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Collidee::OnSave( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Start
	{
		i3::pack::VEC3D vec;

		vec.x = i3Vector::GetX( &m_Start);
		vec.y = i3Vector::GetY( &m_Start);
		vec.z = i3Vector::GetZ( &m_Start);
		
		Rc = pStream->Write( &vec, sizeof(i3::pack::VEC3D));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeLine::OnSave()", "Could not write center information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	// End
	{
		i3::pack::VEC3D vec;

		vec.x = i3Vector::GetX( &m_End);
		vec.y = i3Vector::GetY( &m_End);
		vec.z = i3Vector::GetZ( &m_End);
		
		Rc = pStream->Write( &vec, sizeof(i3::pack::VEC3D));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeLine::OnSave()", "Could not write center information.");
			return STREAM_ERR;
		}
		Result += Rc;
	}

	return Result;
}

UINT32 i3CollideeLine::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3Collidee::OnLoad( pResFile);
	if( Result == STREAM_ERR)
	{
		return STREAM_ERR;
	}

	// Start
	{
		i3::pack::VEC3D vec;

		Rc = pStream->Read( &vec, sizeof( i3::pack::VEC3D));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeLine::OnLoad()", "Could not read center information.");
			return STREAM_ERR;
		}
		Result += Rc;

		i3Vector::Set( &m_Start, vec.x, vec.y, vec.z);
	}

	// End
	{
		i3::pack::VEC3D vec;

		Rc = pStream->Read( &vec, sizeof( i3::pack::VEC3D));
		if( Rc == STREAM_ERR)
		{
			i3Log( "i3CollideeLine::OnLoad()", "Could not read center information.");
			return STREAM_ERR;
		}
		Result += Rc;

		i3Vector::Set( &m_End, vec.x, vec.y, vec.z);
	}

	return STREAM_ERR;
}


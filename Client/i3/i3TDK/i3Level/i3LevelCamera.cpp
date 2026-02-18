#include "stdafx.h"
#include "i3LevelCamera.h"
#include "i3LevelViewport.h"

#include "i3LevelGlobalVariable.h"
#include "i3LevelScene.h"

I3_CLASS_INSTANCE( i3LevelCamera);

i3LevelCamera::i3LevelCamera(void)
{
	m_pCamera = i3Camera::new_object();
	m_pCamera->SetMatrixObject( m_pScene->GetMatrixObject());
}

i3LevelCamera::~i3LevelCamera(void)
{
	I3_SAFE_RELEASE( m_pCamera);
}

void i3LevelCamera::SetViewport( i3LevelViewport * pView)
{
	UpdateCamera( pView->getFramework());
}

void i3LevelCamera::UpdateCamera( i3LevelFramework * pFramework)
{
	i3Camera * pCam;
	i3Viewer * pViewer = pFramework->GetViewer();

	pCam = GetCameraNode();

	I3TRACE( "RESIZE : %d, %d\n", pViewer->GetViewWidth(), pViewer->GetViewHeight());

	switch( pCam->GetMode())
	{
		case i3Camera::I3_CAMERA_MODE_ORTHOGONAL :
			{
				REAL32 w, h, l;
				MATRIX * pCamera = GetTransform();
				VEC3D vec, pc;

				i3Vector::Copy( &vec, i3Matrix::GetAt( pCamera));
				i3Vector::Normalize( &vec, &vec);
				i3Vector::Scale( &vec, &vec, -1.0f);

				l = NearestPlanePoint( pFramework->GetBasePosition(), &vec, i3Matrix::GetPos( pCamera), &pc);

				l = i3Math::abs( l);

				w = l;
				h = l * ((REAL32) pViewer->GetViewWidth() / pViewer->GetViewWidth());

				pCam->SetOrthogonal( w, h, pCam->GetNear(), pCam->GetFar());
			}
			break;

		default :
			pCam->SetPerspective( pCam->GetFOV(), (REAL32)pViewer->GetViewWidth(), (REAL32)pViewer->GetViewHeight(), 
				pCam->GetNear(), pCam->GetFar());
			break;
	}
}

void	i3LevelCamera::OnMoving(void)
{
	//i3Matrix::Copy( m_pCamera->GetMatrix(), GetTransform());
}

void	i3LevelCamera::OnScaling(void)
{
	//i3Matrix::Copy( m_pCamera->GetMatrix(), GetTransform());
}

void	i3LevelCamera::OnRotating(void)
{
	//i3Matrix::Copy( m_pCamera->GetMatrix(), GetTransform());
}


void i3LevelCamera::OnDrive( REAL32 time)
{
}

void	i3LevelCamera::SaveDragStartMatrix( I3_LEVEL_EDIT_CONTEXT& ctx )
{	
	i3Matrix::Copy( &ctx.m_DragStartMatrix, GetTransform() );
}

void	i3LevelCamera::CameraPivotRotate( I3_LEVEL_EDIT_CONTEXT& ctx, REAL32 RotX, REAL32 RotY )
{
	i3LevelScene* scene = i3Level::GetScene();

	if( nullptr == scene )
	{
		return;
	}

	VEC3D vmin, vmax;
	VEC3D vecRight, localPos, vAt;

	RotX *= 5.0f;
	RotY *= 5.0f;

	bool bRes = false;

	switch(  ctx.m_pFramework->GetLevelEditMode() )
	{
	case	LEM_NORMAL:
			{
				bRes = scene->GetWrappingBoundBoxForSelect( &vmin, &vmax);

				if( true == bRes )
				{
					setX( &vAt, (getX( &vmax) + getX( &vmin)) * 0.5f);	
					setY( &vAt, (getY( &vmax) + getY( &vmin)) * 0.5f);	
					setZ( &vAt, (getZ( &vmax) + getZ( &vmin)) * 0.5f);	
				}
			}
			break;

	case	LEM_NAVMESH_POINT:
			{
				if( i3Level::GetNavMesh() )
					bRes = i3Level::GetNavMesh()->GetSelectedCenterPosition( true, vAt );
			}
			break;

	case	LEM_NAVMESH_POLYGON:
			{
				if( i3Level::GetNavMesh() )
					bRes = i3Level::GetNavMesh()->GetSelectedCenterPosition( false, vAt );
			}
			break;
	}

	if( false == bRes )
		i3Vector::Copy( &vAt, &ctx.m_DragStartPos );

	MATRIX * pCamera = GetTransform();

	i3Matrix::Copy( pCamera, &ctx.m_DragStartMatrix );
	i3Vector::Sub( &localPos, i3Matrix::GetPos( pCamera), &vAt);

	// make it in local space
	i3Matrix::SetPos( pCamera, &localPos);

	// rotate around local right vector
	i3Vector::Copy( &vecRight, i3Matrix::GetRight( pCamera));
	i3Vector::Normalize( &vecRight, &vecRight);
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &vecRight, RotY);

	// rotate around world y axis
	i3Matrix::PostRotateAxis( pCamera, (const VEC3D *) &I3_YAXIS, RotX);

	// return to in world-space
	i3Vector::Add( &localPos, i3Matrix::GetPos( pCamera), &vAt);
	i3Matrix::SetPos( pCamera, &localPos);
}

void	i3LevelCamera::CameraPan( I3_LEVEL_EDIT_CONTEXT& ctx, REAL32 dx, REAL32 dy, REAL32 dz )
{
	VEC3D vecRight, vecUp, vecAt;
	MATRIX * pCam;

	pCam = &ctx.m_DragStartMatrix;

	i3Vector::Copy( &vecRight,	i3Matrix::GetRight( pCam ) );
	i3Vector::Copy( &vecUp,		i3Matrix::GetUp( pCam ) );
	i3Vector::Copy( &vecAt,		i3Matrix::GetAt( pCam ) );

	i3Vector::Normalize( &vecRight, &vecRight );
	i3Vector::Normalize( &vecUp, &vecUp );
	i3Vector::Normalize( &vecAt, &vecAt );

	i3Vector::Scale( &vecRight, &vecRight, -dx );
	i3Vector::Scale( &vecUp, &vecUp, dy );
	i3Vector::Scale( &vecAt, &vecAt, dz );

	i3Vector::Add( &vecRight, &vecRight, &vecUp );

	{
		MATRIX tmp;

		i3Matrix::SetPos( &tmp, &vecRight );
		i3Matrix::Mul( GetTransform(), &ctx.m_DragStartMatrix, &tmp );
	}
}

void	i3LevelCamera::CameraRotate( I3_LEVEL_EDIT_CONTEXT& ctx, REAL32 RotX, REAL32 RotY )
{
	D3DXVECTOR3 scale, trans;
	D3DXQUATERNION quat, qx, qy;
	D3DXMATRIX mat;

	::D3DXMatrixDecompose( &scale, &quat, &trans, (const D3DXMATRIX*) &ctx.m_DragStartMatrix );	

	D3DXVECTOR3 vx, vy;

	vx = D3DXVECTOR3( 1, 0, 0 );
	vy = D3DXVECTOR3( 0, 1, 0 );

	::D3DXMatrixRotationQuaternion( &mat, &quat );

	::D3DXQuaternionRotationAxis( &qx, &vx, RotY );
	::D3DXQuaternionRotationAxis( &qy, &vy, RotX );

	quat = qx * quat * qy ;
	::D3DXMatrixRotationQuaternion( &mat, &quat );

	mat._41 = trans.x;
	mat._42 = trans.y;
	mat._43 = trans.z;

	i3Matrix::Copy( GetTransform(), (MATRIX*) &mat );
}


void	i3LevelCamera::CameraZoom( I3_LEVEL_EDIT_CONTEXT& ctx, float fzoom )
{
	VEC3D vec;	

	i3Camera * pCam		= GetCameraNode();
	MATRIX * pCamera	= GetTransform();

	MATRIX tmp;

	i3Vector::Copy( &vec, i3Matrix::GetAt( pCamera) );
	i3Vector::Normalize( &vec, &vec );
	i3Vector::Scale( &vec, &vec, -1.0f );
	i3Vector::Scale( &vec, &vec, fzoom );

	i3Matrix::SetTranslate( &tmp, getX(&vec), getY(&vec), getZ(&vec) );
	i3Matrix::Mul( pCamera, pCamera, &tmp );

	if( i3Camera::I3_CAMERA_MODE_ORTHOGONAL == pCam->GetMode() )
	{
		UpdateCamera( ctx.m_pFramework );
	}
}

void	i3LevelCamera::CameraMove( I3_LEVEL_EDIT_CONTEXT& ctx, REAL32 dx, REAL32 dy, REAL32 dz )
{
	D3DXVECTOR3 scale, trans;
	D3DXQUATERNION quat;
	D3DXMATRIX mat;

	::D3DXMatrixDecompose( &scale, &quat, &trans, (const D3DXMATRIX*) &ctx.m_DragStartMatrix );	

	D3DXVECTOR3 vx, vy, vz;

	vx = D3DXVECTOR3( 1, 0, 0 );
	vy = D3DXVECTOR3( 0, 1, 0 );
	vz = D3DXVECTOR3( 0, 0, 1 );

	::D3DXMatrixRotationQuaternion( &mat, &quat );

	::D3DXVec3TransformCoord( &vx, &vx, &mat );
	::D3DXVec3TransformCoord( &vy, &vy, &mat );
	::D3DXVec3TransformCoord( &vz, &vz, &mat );

	if( 0.0f != dx )
		trans += (vx * dx );

	if( 0.0f != dy )
		trans += (vy * dy );

	if( 0.0f != dz )
		trans += (vz * dz );

	mat._41 = trans.x;
	mat._42 = trans.y;
	mat._43 = trans.z;

	i3Matrix::Copy( GetTransform(), (MATRIX*) &mat );
}
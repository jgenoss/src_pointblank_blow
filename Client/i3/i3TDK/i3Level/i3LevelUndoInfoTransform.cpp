#include "stdafx.h"
#include "i3LevelScene.h"
#include "i3LevelUndoInfoTransform.h"

I3_CLASS_INSTANCE( i3LevelUndoInfoTransform);

void i3LevelUndoInfoTransform::setObject( i3LevelElement * pElement)
{
	//ASSERT에 걸린다면 어차피 동작불가!!! 세워버린다.
	I3ASSERT( pElement != nullptr);
	I3ASSERT( i3::kind_of<i3LevelElement3D*>(pElement));

	i3LevelUndoInfo::setObject( pElement);

	i3LevelElement3D * pElm = (i3LevelElement3D*)pElement;

	if( pElm != nullptr)
	{
		MATRIX * pmtx = pElm->getSavedMatrix();

		I3ASSERT( pmtx != nullptr);

		i3Matrix::Copy( &m_mMatrix, pmtx);
	}
	else
	{
		i3Matrix::Identity( &m_mMatrix);
	}
}

bool i3LevelUndoInfoTransform::Undo(i3LevelScene * pScene)
{
	if( m_pElement == nullptr)	return false;

	i3LevelElement3D * pElm = (i3LevelElement3D*)m_pElement;

	MATRIX	mtx;
	MATRIX * pmtx = pElm->GetTransform();

	i3Matrix::Copy( &mtx, pmtx);
	i3Matrix::Copy( pmtx, &m_mMatrix);

	i3Matrix::Copy( &m_mMatrix, &mtx);

	pElm->OnScaling();
	pElm->OnRotating();
	pElm->OnMoving();

	i3LevelUndoInfo::Undo(pScene);
	
	return false;
}

bool i3LevelUndoInfoTransform::Redo(i3LevelScene * pScene)
{
	if( m_pElement == nullptr)	return false;

	i3LevelElement3D * pElm = (i3LevelElement3D*)m_pElement;

	MATRIX	mtx;
	MATRIX * pmtx = pElm->GetTransform();

	i3Matrix::Copy( &mtx, pmtx);
	i3Matrix::Copy( pmtx, &m_mMatrix);

	i3Matrix::Copy( &m_mMatrix, &mtx);

	pElm->OnScaling();
	pElm->OnRotating();
	pElm->OnMoving();

	i3LevelUndoInfo::Redo( pScene);

	return false;
}
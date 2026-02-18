#include "stdafx.h"
#include "SgBuilder.h"
#include "i3Export.h"
#include "Util.h"

void SgBuilder::_OnJobCamera( JOB_INFO * pJob)
{
	i3Camera * pCamera;

	pJob->m_bProcessed = true;

	pCamera = m_pExport->CreateCamera( pJob->m_pINode);
	I3ASSERT( pCamera != NULL);

	MATRIX mtx;
	::ExtractTransform( pJob->m_pINode, &mtx, m_pExport->getStartTime());

	i3Matrix::Identity( &mtx);
	pCamera->SetMatrix( &mtx);

	// Attach
	ATTACH_NODE( pCamera, pJob->m_pINode);
}


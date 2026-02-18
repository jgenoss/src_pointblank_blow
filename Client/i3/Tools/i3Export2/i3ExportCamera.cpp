#include "stdafx.h"
#include "i3Export.h"
#include "Util.h"

i3Camera * i3Export::CreateCamera( INode * pNode)
{
	CameraObject * pCamObj = (CameraObject *) pNode->GetObjOrWSMRef();
	i3Camera * pCam;
	TimeValue time = m_timeStart;

	pCam = i3Camera::new_object_ref();

	if( pCamObj->IsOrtho())
		pCam->SetMode( i3Camera::I3_CAMERA_MODE_ORTHOGONAL);
	else
		pCam->SetMode( i3Camera::I3_CAMERA_MODE_PERSPECTIVE);

	pCam->SetFOV( pCamObj->GetFOV( time));
	pCam->SetNear( pCamObj->GetClipDist( time, CAM_HITHER_CLIP));
	pCam->SetFar( pCamObj->GetClipDist( time, CAM_YON_CLIP));
	pCam->SetEnable( TRUE);

	return pCam;
}

void i3Export::_OnExportCamera( INode * pNode, ObjectState * pState)
{
	i3Camera * pCam;

	pCam = CreateCamera( pNode);

	m_pCurProcessInfo->pushi3Node( pCam, true);
}

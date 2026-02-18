// OptionDialog.cpp : implementation file
//

#include "stdafx.h"
#include "OptionDialog.h"
#include "resource.h"
#include "i3Export.h"
#include "OptionInfo.h"
#include "SelectBoneDialog.h"
#include "i3Base/string/ext/ftoa.h"

inline bool _GetCheckCtrl( HWND hwnd, UINT id)
{
	HWND hwndCtrl;

	hwndCtrl = GetDlgItem( hwnd, id);
	if( hwndCtrl != NULL)
	{
		if( SendMessage( hwndCtrl, BM_GETCHECK, 0, 0) == BST_CHECKED)
			return true;
	}

	return false;
}

inline void _SetCheckCtrl( HWND hwndDialog, UINT id, bool bFlag)
{
	HWND hwndCtrl;

	hwndCtrl = GetDlgItem( hwndDialog, id);
	if( hwndCtrl != NULL)
	{
		if( bFlag)
			SendMessage( hwndCtrl, BM_SETCHECK, BST_CHECKED, 0);
		else
			SendMessage( hwndCtrl, BM_SETCHECK, BST_UNCHECKED, 0);
	}
}

inline bool _GetPushCtrl( HWND hwnd, UINT id)
{
	HWND hwndCtrl;

	hwndCtrl = GetDlgItem( hwnd, id);

	if( hwndCtrl != NULL)
	{
		if( SendMessage( hwndCtrl, BM_GETSTATE, 0, 0) == BST_PUSHED)
			return true;
	}

	return false;
}

inline void _SetPushCtrl( HWND hwndDialog, UINT id, bool bFlag)
{
	HWND hwndCtrl;

	hwndCtrl = GetDlgItem( hwndDialog, id);
	if( hwndCtrl != NULL)
	{
		if( bFlag)
			SendMessage( hwndCtrl, BM_SETSTATE, BST_PUSHED, 0);
		else
			SendMessage( hwndCtrl, BM_SETSTATE, 0, 0);
	}
}

inline void _SetTextCtrl( HWND hwndDialog, UINT id, char * pszText)
{
	HWND hwndCtrl = GetDlgItem( hwndDialog, id);
	if( hwndCtrl != NULL)
	{
		::SetWindowText( hwndCtrl, pszText);
	}
}

inline void _GetTextCtrl( HWND hwndDialog, UINT id, char * pszText, INT32 len)
{
	HWND hwndCtrl = GetDlgItem( hwndDialog, id);
	if( hwndCtrl != NULL)
	{
		::GetWindowText( hwndCtrl, pszText, len);
	}
}

static void _SetOptionToDialog( HWND hwnd, i3tool::exporter::pack::OPTION_INFO * pOption)
{
	char conv[128];
	HWND hwndCtrl;

	_SetCheckCtrl( hwnd, IDC_CHK_EXPORT_I3S,		(pOption->m_bExportI3S != 0));

	// 선택한 것만 Export 한다.
	_SetCheckCtrl( hwnd, IDC_CHK_SELECTONLY,		pOption->m_bExportSelectedOnly);

	// 문제점을 파악하여 Report 한다.
	_SetCheckCtrl( hwnd, IDC_CHK_REPORTERR,			pOption->m_bReportError);

	// Exclude Node Name
	_SetTextCtrl( hwnd, IDC_ED_EXCLUDE_NODENAME,	pOption->m_szExcludeNodeName);

	// Animation을 Export 한다.
	_SetCheckCtrl( hwnd, IDC_CHK_EXPORT_ANIM,		pOption->m_bExportAnim);

	// Object Type : Mesh
	_SetCheckCtrl( hwnd, IDC_CHK_EXPORT_MESH,		pOption->m_bExportMesh);

	// Object Type : Camera
	_SetCheckCtrl( hwnd, IDC_CHK_EXPORT_CAMERA,		pOption->m_bExportCamera);

	// Object Type : Light
	_SetCheckCtrl( hwnd, IDC_CHK_EXPORT_LIGHT,		pOption->m_bExportLight);

	// Rotate Axis
	_SetCheckCtrl( hwnd, IDC_CHK_ROTATEAXIS,		pOption->m_bRotateAxis == TRUE);

	// Treat Transform Controls as Bone
	_SetCheckCtrl( hwnd, IDC_CHK_TREATASBONE,		pOption->m_bTreatControlAsBone != 0);

	// Node Name set by Material Name
	_SetCheckCtrl( hwnd, IDC_CHK_NODENAMEFM,		pOption->m_bUseMaterialName == TRUE);

	// Export Shapes as Convex
	_SetCheckCtrl( hwnd, IDC_CHK_EXPORT_SHAPE_AS_CONVEX,	pOption->m_bExportShapeAsConvex == TRUE);

	_SetCheckCtrl( hwnd, IDC_CHK_CREATE_RAPPING_CONVEX,	pOption->m_bCreateRappingConvex == TRUE);

	_SetCheckCtrl( hwnd, IDC_CHECK_OPTION_SM, pOption->m_bCreateForSimulate == TRUE);

	// Key Sampling Type
	switch( pOption->m_KeySampleType)
	{
		case i3tool::exporter::KEYSAMPLE_SAMPLE :	
			_SetPushCtrl( hwnd, IDC_RAD_KEY_SAMPLE, true);
			break;

		default :
			_SetPushCtrl( hwnd, IDC_RAD_KEY_SAMPLE, false);
			break;
	}

	// Sampling Interval6
	hwndCtrl = GetDlgItem( hwnd, IDC_ED_INTERVAL);
	sprintf( conv, "%d", pOption->m_SamplingInterval);
	::SetWindowText( hwndCtrl, conv);

	// Force Export Animation
	_SetCheckCtrl( hwnd, IDC_CHK_FORCE_EXPORT_ANIM,	pOption->m_bForceExportAnim != 0);

	_SetCheckCtrl( hwnd, IDC_CHK_PREVIEW, pOption->m_bViewAfterExport != 0);


	// Physics Geometyr
	_SetCheckCtrl( hwnd, IDC_CHK_EXPORT_PHYSICSGEOM,	pOption->m_bExportPhysicsGeometry);

	hwndCtrl = GetDlgItem( hwnd, IDC_ED_SKINWIDTH);
	i3::ftoa( pOption->m_SkinWidth, conv);
	::SetWindowText( hwndCtrl, conv);
}

static void _GetOptionFromDialog( HWND hwnd, i3tool::exporter::pack::OPTION_INFO * pOption)
{
	char conv[128];
	HWND hwndCtrl;

	pOption->m_bExportI3S			= _GetCheckCtrl( hwnd, IDC_CHK_EXPORT_I3S);

	// 선택한 것만 Export 한다.
	pOption->m_bExportSelectedOnly	= _GetCheckCtrl( hwnd, IDC_CHK_SELECTONLY);

	// 문제점을 파악하여 Report 한다.
	pOption->m_bReportError			= _GetCheckCtrl( hwnd, IDC_CHK_REPORTERR);

	// Exclude Node Name
	_GetTextCtrl( hwnd, IDC_ED_EXCLUDE_NODENAME, pOption->m_szExcludeNodeName, sizeof(pOption->m_szExcludeNodeName) - 1);

	// Animation을 Export 한다.
	pOption->m_bExportAnim			= _GetCheckCtrl( hwnd, IDC_CHK_EXPORT_ANIM);

	// Object Type : Mesh
	pOption->m_bExportMesh			= _GetCheckCtrl( hwnd, IDC_CHK_EXPORT_MESH);

	// Object Type : Camera
	pOption->m_bExportCamera		= _GetCheckCtrl( hwnd, IDC_CHK_EXPORT_CAMERA);

	// Object Type : Light
	pOption->m_bExportLight			= _GetCheckCtrl( hwnd, IDC_CHK_EXPORT_LIGHT);

	// Rotate Axis
	pOption->m_bRotateAxis			= _GetCheckCtrl( hwnd, IDC_CHK_ROTATEAXIS);

	// Treat Transform Controls as Bone
	pOption->m_bTreatControlAsBone	= _GetCheckCtrl( hwnd, IDC_CHK_TREATASBONE);

	pOption->m_bUseMaterialName		= _GetCheckCtrl( hwnd, IDC_CHK_NODENAMEFM);

	// Export shapes as convex
	pOption->m_bExportShapeAsConvex	= _GetCheckCtrl( hwnd, IDC_CHK_EXPORT_SHAPE_AS_CONVEX);

	pOption->m_bCreateRappingConvex = _GetCheckCtrl( hwnd, IDC_CHK_CREATE_RAPPING_CONVEX );

	pOption->m_bCreateForSimulate	= _GetCheckCtrl( hwnd, IDC_CHECK_OPTION_SM );

	// Key Sampling Type
	if( _GetPushCtrl( hwnd, IDC_RAD_KEY_SAMPLE))
		pOption->m_KeySampleType = i3tool::exporter::KEYSAMPLE_SAMPLE;
	else
		pOption->m_KeySampleType = i3tool::exporter::KEYSAMPLE_PICKEDONLY;

	// Sampling Interval
	hwndCtrl = GetDlgItem( hwnd, IDC_ED_INTERVAL);
	::GetWindowText( hwndCtrl, conv, sizeof(conv) - 1);
	pOption->m_SamplingInterval = atoi( conv);

	// Force Export Animation
	pOption->m_bForceExportAnim		= _GetCheckCtrl( hwnd, IDC_CHK_FORCE_EXPORT_ANIM);

	pOption->m_bViewAfterExport		= _GetCheckCtrl( hwnd, IDC_CHK_PREVIEW);

	// Export Physics Geometry
	pOption->m_bExportPhysicsGeometry	= _GetCheckCtrl( hwnd, IDC_CHK_EXPORT_PHYSICSGEOM);

	// Skin width
	hwndCtrl = GetDlgItem( hwnd, IDC_ED_SKINWIDTH);
	::GetWindowText( hwndCtrl, conv, sizeof(conv) - 1);
	pOption->m_SkinWidth = REAL32(atof( conv));
}

BOOL CALLBACK  _OptionDialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	UINT id, code;
	i3Export *pExport;
	i3tool::exporter::pack::OPTION_INFO * pOption;

	switch( msg) 
	{
		case WM_INITDIALOG:
			{
				pExport = (i3Export *)lParam;
				SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG)pExport);

				CenterWindow( hwnd, GetParent( hwnd));

				pOption = pExport->getOption();
				_SetOptionToDialog( hwnd, pOption);
			}
			break;

		case WM_COMMAND:
			id = LOWORD(wParam);
			code = HIWORD( wParam);

			pExport = (i3Export *)GetWindowLongPtr( hwnd, GWLP_USERDATA);
			pOption = pExport->getOption();

			if( code == BN_CLICKED)
			{
				switch( id) 
				{
					case  IDC_BTN_SELBONE :
						{
							DialogBoxParam( hInstance, MAKEINTRESOURCE( IDD_SELECTBONE), hwnd, _SelectBoneDialogProc, (LPARAM)pExport);
						}
						break;

					case IDOK:
						_GetOptionFromDialog( hwnd, pOption);
						// break;		<- 의도적으로 없앤 것.

					case IDCANCEL :
						EndDialog( hwnd, id);
						break;
				}
			}
			break;

		default:
			return false;
	}
	return true;
}

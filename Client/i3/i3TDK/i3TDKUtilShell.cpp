#include "stdafx.h"
#include "i3TDKUtilShell.h"
#include "i3TDKGlobalVariable.h"

#include "mmc.h"
#include "resource.h"

typedef HRESULT (FAR WINAPI *SHPROC)( int, REFIID, void **);

static void AddIcon( CImageList * pList, INT32 id)
{
	HICON hIcon;

	hIcon = ::LoadIcon( g_hInstTDK, MAKEINTRESOURCE( id));
	pList->Add( hIcon);
	::DestroyIcon( hIcon);
}

I3_EXPORT_TDK
INT32 i3TDK::AddTDKImageListTo( CImageList * pList)
{
	I3ASSERT( pList != nullptr);

	INT32 nFirstIndex = pList->GetImageCount();

	AddIcon( pList, IDI_ANIM);
	AddIcon( pList, IDI_ANIM_UNSEL);
	AddIcon( pList, IDI_ATTR);
	AddIcon( pList, IDI_ATTR_UNSEL);
	AddIcon( pList, IDI_BONE);
	AddIcon( pList, IDI_BONE_UNSEL);
	AddIcon( pList, IDI_CAMERA);
	AddIcon( pList, IDI_CAMERA_UNSEL);
	AddIcon( pList, IDI_GEOMETRY);
	AddIcon( pList, IDI_GEOMETRY_UNSEL);
	AddIcon( pList, IDI_NODE);
	AddIcon( pList, IDI_NODE_UNSEL);
	AddIcon( pList, IDI_RESOURCE);
	AddIcon( pList, IDI_RESOURCE_UNSEL);
	AddIcon( pList, IDI_SOUND);
	AddIcon( pList, IDI_SOUND_UNSEL);
	AddIcon( pList, IDI_TEXTURE);
	AddIcon( pList, IDI_TEXTURE_UNSEL);

	return nFirstIndex;
}

static CImageList *		s_pSysImageList[SHIL_LAST + 1] = { nullptr, };

I3_EXPORT_TDK
CImageList * i3TDK::GetSystemImageList( INT32 type)
{
	HRESULT rv;
	SHPROC _SHGetImageList;
	HANDLE			hModule;
	IImageList *	pIImageList;
	CImageList *	pImageList;

	if( s_pSysImageList[ type] == nullptr)
	{
		{
			hModule = LoadLibrary( _T("Shell32.dll") );
		}
		
		_SHGetImageList = (SHPROC) GetProcAddress( (HMODULE) hModule, "SHGetImageList" );
		I3ASSERT( _SHGetImageList != nullptr);

		rv = _SHGetImageList( type, IID_IImageList, (void **) &pIImageList);
		if( FAILED( rv))
		{
			AfxMessageBox( _T("Could not get the System Image List interface object.") );
			return nullptr;
		}

		{
			pImageList = new CImageList;

			pImageList->Attach( (HIMAGELIST) pIImageList);
		}

		s_pSysImageList[ type] = pImageList;
	}
	else
	{
		pImageList = s_pSysImageList[ type];
	}

	return pImageList;
}

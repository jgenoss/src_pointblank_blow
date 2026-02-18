#include "StdAfx.h"
#include "UIMergedTexture.h"
/*
I3_CLASS_INSTANCE( CUIMergedTexture); //, i3ElementBase);

CUIMergedTexture::CUIMergedTexture(void)
{
	m_plistRect			= NULL;
	m_pMergedTexture	= NULL;
}

CUIMergedTexture::~CUIMergedTexture(void)
{
	if( m_plistRect != NULL)
	{
		INT32 i;
		for( i = 0;i < m_plistRect->GetCount(); ++i)
		{
			I3UI_VTRECT_INFO * pInfo = (I3UI_VTRECT_INFO*)m_plistRect->GetItem( i);

			i3MemFree( pInfo);
		}
		I3_SAFE_RELEASE( m_plistRect);
	}

	I3_SAFE_RELEASE( m_pMergedTexture);
}

void CUIMergedTexture::Create(void)
{
	I3ASSERT( m_pMergedTexture == NULL);

	UINT32				size	= UI_MERGEDTEXTURE_SIZE;		/// 기본 사이즈.UI_MERGEDTEXTURE_SIZE
	UINT32				level	= 1;							/// UI에 사용될 Texture라서 Mipmap이 필요 없는듯.
	I3G_IMAGE_FORMAT	format	= I3G_IMAGE_FORMAT_RGBA_8888;	/// 역시나 Alpha값을 가지고 있어야 할듯.
	I3G_USAGE			Usage	= I3G_USAGE_FREQUENT_UPDATE;	/// todo: 요건..음..자주 썼다 지웠다 하긴 해야하는데..실제 사용될때는 UPDATE필요 없는 상태로 생성해야 할듯.

	m_pMergedTexture = i3Texture::new_object();
	m_pMergedTexture->Create( size, size, level, format, Usage);

	I3ASSERT( m_plistRect == NULL);
	m_plistRect = List::new_object();
}

I3UI_VTRECT_INFO * CUIMergedTexture::_FindRectByHandle( UINT32 handle, List * plist)
{
	INT32 i;
	for( i = 0;i < plist->GetCount(); ++i)
	{
		I3UI_VTRECT_INFO * pInfo = (I3UI_VTRECT_INFO*)plist->GetItem( i);

		if( pInfo->idHandle == handle)
			return pInfo;
	}

	return NULL;
}

UINT32 CUIMergedTexture::_FindRectByPath( char * pszFilePath, List * plist, List * pRectList)
{
	if( m_plistRect == NULL || pszFilePath == NULL || pRectList == NULL)
		return 0;

	INT32 i;
	UINT32 checkSum = CRC32( 0xFFFFFFFF, (UINT8 *) pszFilePath, strlen( pszFilePath));

	for( i = 0;i < pRectList->GetCount(); ++i)
	{
		I3UI_VTRECT_INFO * pInfo = (I3UI_VTRECT_INFO*)pRectList->GetItem( i);

		if( pInfo->source.hashCode == checkSum)
			plist->Add( pInfo);
	}

	return plist->GetCount();
}

UINT32 CUIMergedTexture::_FindContainerRectByRect( i3::pack::RECT * pRect, List * poList, List * pInSrcList)
{
	if( m_plistRect == NULL || pRect == NULL || pInSrcList == NULL)
		return 0;

	INT32 i;
	for( i = 0;i < pInSrcList->GetCount(); ++i)
	{
		I3UI_VTRECT_INFO * pInfo = (I3UI_VTRECT_INFO*)pInSrcList->GetItem( i);

		if( i3Math::RectInRect( pRect, &pInfo->source.uvwh))
			poList->Add( pInfo);
	}

	return poList->GetCount();
}

UINT32 CUIMergedTexture::_FindIncludedRectByRect( i3::pack::RECT * pRect, List * pIncludedList, List * pInSrcList)
{
	if( m_plistRect == NULL || pRect == NULL || pInSrcList == NULL)
		return 0;

	INT32 i;
	for( i = 0;i < pInSrcList->GetCount(); ++i)
	{
		I3UI_VTRECT_INFO * pInfo = (I3UI_VTRECT_INFO*)pInSrcList->GetItem( i);

		if( i3Math::RectInRect( &pInfo->source.uvwh, pRect))
			pIncludedList->Add( pInfo);
	}

	return pIncludedList->GetCount();
}

UINT32 CUIMergedTexture::_AddRectToContainer( I3UI_RESSRC_INFO * pInfo, I3UI_VTRECT_INFO * pContainer)
{
	//todo: 포함하는 영역의 정보를 이용하여 재배치 하지 않고 info를 배치한다.
	return 0;
}

UINT32 CUIMergedTexture::_AddNewRect( I3UI_RESSRC_INFO * pInfo)
{
	//todo: 현재의 영역들 밖에 새로 배치한다.
	return 0;
}

UINT32 CUIMergedTexture::_AddContainerRect( I3UI_RESSRC_INFO * pInfo, List * plistIncluded)
{
	//todo: 새로만든 영역에 include된 rect를 붙여준다.
	return 0;
}

// 원본 이미지 정보를 토대로 영역 설정후 handle을 반환 한다.
UINT32 CUIMergedTexture::AddRect( I3UI_RESSRC_INFO * pInfo)
{
	List listSameRes, listContainer, listIncluded;

	// 동일한 원본 이미지를 가지고 있는 rect들을 고른다.
	INT32 cnt = _FindRectByPath( pInfo->m_Path, &listSameRes, m_plistRect);
	if( cnt == 0)
	{
		return _AddNewRect( pInfo);
	}

	// 동일 이미지를 가진 rect중에 info의 rect를 포함하는 rect들을 찾아낸다.
	cnt = _FindContainerRectByRect( &pInfo->uvwh, &listContainer, &listSameRes);
	if( cnt > 0)
	{
		return _AddRectToContainer( pInfo, (I3UI_VTRECT_INFO*)listContainer.GetItem( 0));
	}

	// 새로 추가되는 이미지 영역에 포함되는 rect들을 찾게되면, 리스트에서 떼어낸후, container에 붙여준다.
	cnt = _FindIncludedRectByRect( &pInfo->uvwh, &listIncluded, &listSameRes);
	if( cnt > 0)
	{
		return _AddContainerRect( pInfo, &listIncluded);
	}

	// 새로 추가하여 배치한다.
	return _AddNewRect( pInfo);
}

//지울때는 영역의 변경을 처리하지 않는다.
bool	CUIMergedTexture::RemoveRect( UINT32 handle)
{
	I3UI_VTRECT_INFO * pInfo = _FindRectByHandle( handle, m_plistRect);
	if( pInfo == NULL)
		return false;

	m_plistRect->Remove( pInfo);

	I3MEM_SAFE_FREE( pInfo);

	return true;
}

I3UI_VTRECT_INFO * CUIMergedTexture::GetRectInfoByIndex( UINT32 index)
{
	I3_BOUNDCHK( 0, m_plistRect->GetCount());

	return (I3UI_VTRECT_INFO*)m_plistRect->GetItem( index);
}

I3UI_VTRECT_INFO * CUIMergedTexture::GetRectInfo( UINT32 handle)
{
	return _FindRectByHandle( handle, m_plistRect);
}	

bool CUIMergedTexture::VerifyHandle(void)								// 겹치는 핸들을 사용하고 있는지를 검증.
{
	return true;
}

void	CUIMergedTexture::OnBuildObjectList( List * pList)
{
	
}

UINT32	CUIMergedTexture::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	return Result;
}

UINT32 CUIMergedTexture::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result, Rc = 0;

	return Result;
}

bool CUIMergedTexture::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	return true;
}

bool CUIMergedTexture::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	return false;
}
*/
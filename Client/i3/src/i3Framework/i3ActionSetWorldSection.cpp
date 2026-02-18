#include "i3FrameworkPCH.h"
#include "i3ActionSetWorldSection.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3ActionSetWorldSection);
I3_CLASS_INSTANCE( i3ActionSetMapSector);

void i3ActionSetWorldSection::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Action::CopyTo( pDest, method);

	i3ActionSetWorldSection * pAction = (i3ActionSetWorldSection *) pDest;

	pAction->setWorldSection( getWorldSection());
}

void i3ActionSetWorldSection::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		sprintf_s( pszStr, len, "World-Section 설정 (%d)", m_idxSection);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "Object 및 Character가 위치한 World-Section을 설정합니다.", len);
	}
}

void	i3ActionSetWorldSection::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3Action::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_SET_WORLD_SECTION
	{
		UINT8	m_ID[4] = { 'A', 'W', 'S', '1' };
		INT32	m_idxSection = 0;
		UINT32	pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack( pop)				// [test required] 패킹 팝 누락으로 추가합니다. 2017.02.23. soon9
#endif

UINT32 i3ActionSetWorldSection::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_SET_WORLD_SECTION data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_idxSection			= getWorldSection();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionSetWorldSection::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_SET_WORLD_SECTION data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setWorldSection( data.m_idxSection);

	return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3ActionSetMapSector::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3Action::CopyTo( pDest, method);

	i3ActionSetMapSector * pAction = (i3ActionSetMapSector *) pDest;

	pAction->setMapSector( getMapSector());
}

void i3ActionSetMapSector::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	if( bShort)
	{
		sprintf_s( pszStr, len, "MAP_SECTOR 설정 (%d)", m_idxSector);
	}
	else
	{
		i3::string_ncopy_nullpad( pszStr, "Object 및 Character가 위치한 MapSector를 설정합니다.", len);
	}
}

void	i3ActionSetMapSector::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
	i3Action::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 ACTION_SET_MAP_SECTOR
	{
		UINT8	m_ID[4] = { 'A', 'M', 'S', '1' };
		INT32	m_idxSector = 0;
		UINT32	pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC) && defined(PACKING_RIGHT)
#pragma pack( pop)				// [test required] 패킹 팝 누락으로 추가합니다. 2017.02.23. soon9
#endif

UINT32 i3ActionSetMapSector::OnSave( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_SET_MAP_SECTOR data;

	result = i3Action::OnSave( pResFile);
	I3_CHKIO( result);

	data.m_idxSector			= getMapSector();
	
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	return result;
}

UINT32 i3ActionSetMapSector::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 rc, result;
	pack::ACTION_SET_MAP_SECTOR data;

	result = i3Action::OnLoad( pResFile);
	I3_CHKIO( result);
	
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	setMapSector( data.m_idxSector);

	return result;
}

#include "i3FrameworkPCH.h"

#include "i3WorldSectionTable.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3WorldSectionTable);
I3_CLASS_INSTANCE( i3MapSectorTable);

bool i3::pack::WORLD_SECTION_INFO::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->SetAttribute( "Name", m_szName);
	pXML->SetLongLongAttribute( "ShadowMask", m_ShadowMask);
	pXML->SetLongLongAttribute( "VisibilityMask", m_VisibilityMask);
	pXML->SetAttribute( "Floor", m_Floor);

	return true;
}

bool i3::pack::WORLD_SECTION_INFO::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	const char * pszName = pXML->getAttr( "Name");
	i3::safe_string_copy( m_szName, pszName, 128);

	pXML->getAttr( "ShadowMask", &m_ShadowMask);
	pXML->getAttr( "VisibilityMask", &m_VisibilityMask);
	pXML->getAttr( "Floor", &m_Floor);

	return true;
}


i3WorldSectionTable::~i3WorldSectionTable(void)
{
	I3MEM_SAFE_FREE( m_pTable);
}

void i3WorldSectionTable::SetSectionCount( INT32 cnt)
{
	if( cnt != getCount())
	{
		if( m_pTable == nullptr)
		{
			m_pTable = (i3::pack::WORLD_SECTION_INFO *) i3MemAlloc( sizeof(i3::pack::WORLD_SECTION_INFO) * cnt);
		}
		else
		{
			m_pTable = (i3::pack::WORLD_SECTION_INFO *) i3MemReAlloc( m_pTable, sizeof(i3::pack::WORLD_SECTION_INFO) * cnt);
		}

		I3ASSERT( m_pTable != nullptr);

		if( cnt > m_Count)
		{
			i3mem::FillZero( &m_pTable[ m_Count], cnt - m_Count);
		}
	}

	m_Count = cnt;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 2)
#endif

namespace pack
{
	struct ALIGN2 WORLD_SECTION_TABLE
	{
		UINT8		m_ID[4] = { 'W', 'S', 'T', '1' };
		INT32		m_Count = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3WorldSectionTable::OnSave( i3ResourceFile * pResFile)
{
	pack::WORLD_SECTION_TABLE data;
	UINT32 rc, result = 0;

	data.m_Count = getCount();

	// header
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// World-Section Table data
	if( getCount() > 0)
	{
		I3ASSERT( m_pTable != nullptr);

		rc = pResFile->Write( m_pTable, sizeof(i3::pack::WORLD_SECTION_INFO) * getCount());
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

UINT32	i3WorldSectionTable::OnLoad( i3ResourceFile * pResFile)
{
	pack::WORLD_SECTION_TABLE data;
	UINT32 rc, result = 0;

	// header
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// World-Section Table data
	if( data.m_Count > 0)
	{
		SetSectionCount( data.m_Count);
		
		rc = pResFile->Read( m_pTable, sizeof(i3::pack::WORLD_SECTION_INFO) * getCount());
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

bool i3WorldSectionTable::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr("SectionCount", getCount());

	for( INT32 i = 0;i < getCount(); ++i)
	{
		i3::pack::WORLD_SECTION_INFO *	sectionTable = (i3::pack::WORLD_SECTION_INFO*)getWorldSection( i);

		sectionTable->OnSaveXML( pFile, pXML);
	}

	return true;
}

bool i3WorldSectionTable::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 sectionCount = 0;
	pXML->getAttr("SectionCount", &sectionCount);

	SetSectionCount( sectionCount);

	for( INT32 i = 0;i < getCount(); ++i)
	{
		i3::pack::WORLD_SECTION_INFO *	sectionTable = (i3::pack::WORLD_SECTION_INFO*)getWorldSection( i);

		sectionTable->OnLoadXML( pFile, pXML);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
//	MapSection Info
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool i3::pack::MAP_SECTOR_INFO::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->SetAttribute( "Name", m_szName);
	pXML->SetAttribute( "Floor", m_Floor);

	return true;
}

bool i3::pack::MAP_SECTOR_INFO::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	const char * pszName = pXML->getAttr( "Name");
	i3::safe_string_copy( m_szName, pszName, 128);

	pXML->getAttr( "Floor", &m_Floor);

	return true;
}


i3MapSectorTable::~i3MapSectorTable(void)
{
	I3MEM_SAFE_FREE( m_pTable);
}

void i3MapSectorTable::SetSectorCount( INT32 cnt)
{
	if( cnt != getCount())
	{
		if( m_pTable == nullptr)
		{
			m_pTable = (i3::pack::MAP_SECTOR_INFO *) i3MemAlloc( sizeof(i3::pack::MAP_SECTOR_INFO) * cnt);
		}
		else
		{
			m_pTable = (i3::pack::MAP_SECTOR_INFO *) i3MemReAlloc( m_pTable, sizeof(i3::pack::MAP_SECTOR_INFO) * cnt);
		}

		I3ASSERT( m_pTable != nullptr);

		if( cnt > m_Count)
		{
			i3mem::FillZero( &m_pTable[ m_Count], cnt - m_Count);
		}
	}

	m_Count = cnt;
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 2)
#endif

namespace pack
{
	struct ALIGN2 MAP_SECTOR_TABLE
	{
		UINT8		m_ID[4] = { 'M', 'S', 'T', '1' };
		INT32		m_Count = 0;
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32	i3MapSectorTable::OnSave( i3ResourceFile * pResFile)
{
	pack::MAP_SECTOR_TABLE data;
	UINT32 rc, result = 0;

	data.m_Count = getCount();

	// header
	rc = pResFile->Write( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// World-Sector Table data
	if( getCount() > 0)
	{
		I3ASSERT( m_pTable != nullptr);

		rc = pResFile->Write( m_pTable, sizeof(i3::pack::MAP_SECTOR_INFO) * getCount());
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

UINT32	i3MapSectorTable::OnLoad( i3ResourceFile * pResFile)
{
	pack::MAP_SECTOR_TABLE data;
	UINT32 rc, result = 0;

	// header
	rc = pResFile->Read( &data, sizeof(data));
	I3_CHKIO( rc);
	result += rc;

	// World-Sector Table data
	if( data.m_Count > 0)
	{
		SetSectorCount( data.m_Count);
		
		rc = pResFile->Read( m_pTable, sizeof(i3::pack::MAP_SECTOR_INFO) * getCount());
		I3_CHKIO( rc);
		result += rc;
	}

	return result;
}

bool i3MapSectorTable::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	pXML->addAttr("SectorCount", getCount());

	for( INT32 i = 0;i < getCount(); ++i)
	{
		i3::pack::MAP_SECTOR_INFO *	SectorTable = (i3::pack::MAP_SECTOR_INFO*)getMapSector( i);

		SectorTable->OnSaveXML( pFile, pXML);
	}

	return true;
}

bool i3MapSectorTable::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	INT32 SectorCount = 0;
	pXML->getAttr("SectorCount", &SectorCount);

	SetSectorCount( SectorCount);

	for( INT32 i = 0;i < getCount(); ++i)
	{
		i3::pack::MAP_SECTOR_INFO *	SectorTable = (i3::pack::MAP_SECTOR_INFO*)getMapSector( i);

		SectorTable->OnLoadXML( pFile, pXML);
	}

	return true;
}

bool i3MapSectorTable::SaveSectorString( const char * pszFileName)
{
	i3IniParser parser;

	if( parser.Create( pszFileName) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Couldn't load file: %s", pszFileName);
	}

	parser.WriteSectionName("Default");
	parser.WriteValue( "SectorCount", (INT32)getCount());

	char conv[MAX_PATH];
	for( INT32 i = 0;i < getCount(); ++i)
	{
		i3::pack::MAP_SECTOR_INFO *	SectorTable = (i3::pack::MAP_SECTOR_INFO*)getMapSector( i);

		sprintf( conv, "sec_%d", i);

		parser.WriteValue( conv, SectorTable->m_szName);
	}

	parser.Close();

	return true;
}

bool i3MapSectorTable::LoadSectorString( const char * pszFileName)
{
	i3IniParser parser;

	if( parser.OpenFromFile( pszFileName) == false)
	{
		I3PRINTLOG(I3LOG_WARN,  "Couldn't load file: %s", pszFileName);
		return false;
	}

	parser.ReadSection("Default");

	INT32 nCount;
	if( parser.GetValue( "SectorCount", &nCount) == false)
		return false;

	I3ASSERT( getCount() == nCount);

	char conv[MAX_PATH], conv2[MAX_PATH];
	for( INT32 i = 0;i < getCount(); ++i)
	{
		i3::pack::MAP_SECTOR_INFO *	SectorTable = (i3::pack::MAP_SECTOR_INFO*)getMapSector( i);

		sprintf( conv, "sec_%d", i);

		if( parser.GetString( conv, conv2, sizeof(conv2)))
			i3::string_ncopy_nullpad( SectorTable->m_szName, conv2, MAX_PATH -1);
	}

	parser.Close();

	return true;
}
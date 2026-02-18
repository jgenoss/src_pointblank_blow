#include "stdafx.h"
#include "RSCGenDefine.h"
#include "i3Base/i3ParserHelper.h"
//#include "GlobalVar.h"

I3_CLASS_INSTANCE( CFieldPack); //, i3NamedElement);
I3_CLASS_INSTANCE( RSC_FIELD_INFO); //, i3ElementBase);
I3_CLASS_INSTANCE( RSC_DATA); //, i3ElementBase);

///////////////////////////////////////////////////////////////
RSC_FIELD_INFO::RSC_FIELD_INFO()
{
	m_nFieldStyle	= I3RSC_TYPE_COUNT;
	m_pFieldData	= NULL;
}

RSC_FIELD_INFO::~RSC_FIELD_INFO()
{
	if( m_nFieldStyle & I3RSC_FIELD_TYPE_INDEX)
	{
		i3MemFree((INT32*)m_pFieldData);
	}
	else if( m_nFieldStyle & I3RSC_FIELD_TYPE_STRING)
	{
		i3MemFree((char*)m_pFieldData);
	}
	else if( m_nFieldStyle & I3RSC_FIELD_TYPE_VALUE)
	{
		i3MemFree((INT32*)m_pFieldData);
	}
	else if( m_nFieldStyle & I3RSC_FIELD_TYPE_REALVALUE)
	{
		i3MemFree( (REAL32*)m_pFieldData);
	}
}

////////////////////////////////////////////////////////////////
CFieldPack::CFieldPack()
{
	m_nFieldCount	= 0;
	//m_pFieldInfo	= NULL;

	m_pIniParser	= NULL;
}

CFieldPack::~CFieldPack()
{
	I3ASSERT( (INT32)m_FieldList.size() == m_nFieldCount);

	INT32 i;

	for( i = 0; i< m_nFieldCount; ++i)
	{
		RSC_FIELD_INFO * pInfo = m_FieldList[i];

		I3_SAFE_RELEASE( pInfo);
	}

	m_FieldList.clear();
	m_pIniParser	= NULL;
}

void CFieldPack::Create( const char *szName, INT32 nFieldCount)
{
	static INT32 nCreateCount = 0;
	I3ASSERT( nFieldCount != 0);

	INT32 i;

	m_FieldList.clear();
	m_nFieldCount	= nFieldCount;

	for( i = 0; i < m_nFieldCount; ++i)
	{
		RSC_FIELD_INFO * pInfo = RSC_FIELD_INFO::new_object();
	   
		m_FieldList.push_back( pInfo);

		nCreateCount ++;
	}
}

void CFieldPack::Create( UINT32	nRSCType)
{
	INT32 nType		= nRSCType & I3RSC_TYPE_MASK;
	INT32 nIndex	= 0;

	INT32 nStyle	= g_RSCStyleDef[nType].m_nFieldPackStyle;
	INT32 nCount	= g_RSCStyleDef[nType].m_nFieldCount;

	Create( g_RSCStyleDef[nType].m_szFieldPackName, nCount);

	INT32 nSt = nStyle & I3RSC_FIELDPACK_MASK;
	nSt = nSt >> 16;

	for( INT32 i = 0; i< I3RSC_FIELDPACK_COUNT; ++i)
	{
		if( nSt & (1 << i))
		{
			RSC_FIELD_INFO * pInfo = m_FieldList[nIndex];

			InitField( pInfo, g_szFieldInfoDef[i].m_szFieldName, g_szFieldInfoDef[i].m_nFieldStyle);

			//Type Field Set
			if( i == 0)
			{
				strcpy((char*)pInfo->m_pFieldData, g_RSCStyleDef[nType].m_szFieldPackName);
			}

			nIndex++;
		}
	}
}

void CFieldPack::InitField( RSC_FIELD_INFO * pInfo, const char * szName, UINT32 nStyle)
{
	I3ASSERT( pInfo != NULL);

	strcpy( pInfo->m_szFieldName, szName);
	
	pInfo->m_nFieldStyle	= nStyle;

	if( nStyle & I3RSC_FIELD_TYPE_INDEX)
	{
		pInfo->m_pFieldData = i3MemAlloc( sizeof( INT32));
		ZeroMemory( pInfo->m_pFieldData, sizeof( INT32));
	}
	else if( nStyle & I3RSC_FIELD_TYPE_STRING)
	{
		pInfo->m_pFieldData = i3MemAlloc( sizeof( char) * MAX_PATH);
		ZeroMemory( pInfo->m_pFieldData, sizeof( char) * MAX_PATH);
	}
	else if( nStyle & I3RSC_FIELD_TYPE_VALUE)
	{
		pInfo->m_pFieldData = i3MemAlloc( sizeof( INT32));
		ZeroMemory( pInfo->m_pFieldData, sizeof( INT32));
	}
	else if( nStyle & I3RSC_FIELD_TYPE_REALVALUE)
	{
		pInfo->m_pFieldData = i3MemAlloc( sizeof( REAL32));
		ZeroMemory( pInfo->m_pFieldData, sizeof( REAL32));
	}
	else
	{
		pInfo->m_pFieldData = NULL;
	}
}

RSC_FIELD_INFO * CFieldPack::GetField( INT32 nIndex)
{
//	if( m_pFieldInfo == NULL)	
//		return NULL;

	if( nIndex >= m_nFieldCount)	
		return NULL;

	//return m_pFieldInfo + nIndex;
	return m_FieldList[nIndex];
}

void CFieldPack::Save(i3IniParser * pParser)
{
	I3ASSERT( pParser != NULL);

	INT32 i = 0;

	m_pIniParser = pParser;


	RSC_FIELD_INFO * pInfo = m_FieldList[1];
	//Section Name이 Relative Path가 됨
	pParser->WriteSectionName( (char*)pInfo->m_pFieldData);


	//공통 항목인 Type Field
	_SaveField( m_FieldList[0]);				//Type

	for( i = 2; i < m_nFieldCount; ++i)
	{
		_SaveField( m_FieldList[i]);		//Type
	}
}

void CFieldPack::_SaveField( RSC_FIELD_INFO * pInfo)
{
	if( pInfo == NULL)
	{
		return ;
	}

	i3IniParser * pParser = m_pIniParser;

	INT32 nStyle = pInfo->m_nFieldStyle;

	if( nStyle & I3RSC_FIELD_TYPE_INDEX)
	{
		pParser->WriteValue( pInfo->m_szFieldName, *((INT32*)pInfo->m_pFieldData));
	}
	else if( nStyle & I3RSC_FIELD_TYPE_STRING)
	{
		pParser->WriteValue( pInfo->m_szFieldName, (char*)pInfo->m_pFieldData);
	}
	else if( nStyle & I3RSC_FIELD_TYPE_VALUE)
	{
		pParser->WriteValue( pInfo->m_szFieldName, *((INT32*)pInfo->m_pFieldData));
	}
	else if( nStyle & I3RSC_FIELD_TYPE_REALVALUE)
	{
		pParser->WriteValue( pInfo->m_szFieldName, *((REAL32*)pInfo->m_pFieldData));
	}
}

#pragma warning(push)
#pragma warning(disable:4533)

void CFieldPack::Load( i3IniParser * pParser, i3IniSectionInfo * pSectionInfo)
{
	I3ASSERT( pParser != NULL);
	I3ASSERT( pSectionInfo != NULL);

	INT32	nType;
	INT32	nIndex;

	m_pIniParser = pParser;

	char szString[MAX_PATH];

	//
	GET_IMPORTANT_VALUE( "Type", 0, szString);


	i3::iterator_range<const char*> strString = i3::as_literal(szString);


	if( i3::generic_is_iequal( strString, "SCENE") )			nType = I3RSC_TYPE_SCENE;
	else if( i3::generic_is_iequal( strString, "CHARA") )		nType = I3RSC_TYPE_CHARA;
	else if( i3::generic_is_iequal( strString, "WORLD") )		nType = I3RSC_TYPE_WORLD;
	else if( i3::generic_is_iequal( strString, "ANIMATION") )	nType = I3RSC_TYPE_ANIMATION;
	else if( i3::generic_is_iequal( strString, "TEXTURE") )		nType = I3RSC_TYPE_TEXTURE;
	else if( i3::generic_is_iequal( strString, "SOUND") )		nType = I3RSC_TYPE_SOUND;
	else if( i3::generic_is_iequal( strString, "BINARY") )		nType = I3RSC_TYPE_BINARY;
	else if( i3::generic_is_iequal( strString, "ANIMPACK") )		nType = I3RSC_TYPE_ANIMPACK;
	else if( i3::generic_is_iequal( strString, "OBJECT") )		nType = I3RSC_TYPE_OBJECT;
	else if( i3::generic_is_iequal( strString, "UI") )			nType = I3RSC_TYPE_UI;
	else														nType = nType = I3RSC_TYPE_BINARY;

	Create( nType);

	//SectionName 이 File Path
	RSC_FIELD_INFO * pInfo = m_FieldList[1];
	
	INT32 nSt = g_RSCStyleDef[nType].m_nFieldPackStyle & I3RSC_FIELDPACK_MASK;
	nSt = nSt >> 16;

	strcpy( (char*)pInfo->m_pFieldData, pSectionInfo->Name);

	nIndex	= 2;
	for( INT32 i = 2; i< I3RSC_FIELDPACK_COUNT; ++i)
	{
		if( nSt & (1 << i))
		{
			pInfo = m_FieldList[nIndex];

			_LoadField( pInfo, i);

			nIndex++;
		}
	}

ExitLabel:
	return ;
}
#pragma warning(pop)

void CFieldPack::_LoadField( RSC_FIELD_INFO * pInfo, INT32 nType)
{
	I3ASSERT( pInfo != NULL);

	char conv[32];
	char szFieldName[32];

	i3IniParser * pParser = m_pIniParser;

	INT32 nStyle = g_szFieldInfoDef[nType].m_nFieldStyle ;
	strcpy( szFieldName, g_szFieldInfoDef[nType].m_szFieldName);

	if( nStyle & I3RSC_FIELD_TYPE_INDEX)
	{
		GET_VALUE( szFieldName, 0, (INT32*)pInfo->m_pFieldData);
	}
	else if( nStyle & I3RSC_FIELD_TYPE_STRING)
	{
		GET_VALUE( szFieldName, "", conv);

		strcpy( (char*)pInfo->m_pFieldData, conv);
	}
	else if( nStyle & I3RSC_FIELD_TYPE_VALUE)
	{
		/*
		if( i3String::Compare( szFieldName, "ListenerVolume") == 0)
		{
			*((INT32*)pInfo->m_pFieldData) = 60;
		}
		else if( i3String::Compare( szFieldName, "DefaultVolume") == 0)
		{
			*((INT32*)pInfo->m_pFieldData) = 100;
		}
		else*/
		{
			GET_VALUE( szFieldName, 0, (INT32*)pInfo->m_pFieldData);
		}
	}
	else if( nStyle & I3RSC_FIELD_TYPE_REALVALUE)
	{
		/*
		if( i3String::Compare( szFieldName, "Sound_MaxDist") == 0)
		{
			*((REAL32*)pInfo->m_pFieldData) = 30.0f;
		}
		else if( i3String::Compare( szFieldName, "Sound_MinDist") == 0)
		{
			*((REAL32*)pInfo->m_pFieldData) = 5.0f;
		}
		else*/
		{
			GET_VALUE( szFieldName, 0.0f, (REAL32*)pInfo->m_pFieldData);
		}
	}

ExitLabel:
	return ;
}

UINT32 CFieldPack::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 CFieldPack::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}

void  CFieldPack::OnBuildObjectList(i3::vector<i3PersistantElement*>& List)
{
}

/////////////////////////////////////////////////////////////////////////////////
RSC_DATA::RSC_DATA()
{
	m_Attr			= 0;
	m_szFilePath[0] = 0;
	m_pField		= NULL;
}

RSC_DATA::~RSC_DATA()
{
	I3_SAFE_RELEASE( m_pField);
}


INT32		GetResType( const char * pszExt)
{
	INT32 type = I3RSC_TYPE_BINARY;

	if( i3::generic_is_iequal( pszExt, "I3S") )
	{
		type = I3RSC_TYPE_SCENE;
	}
	else if( i3::generic_is_iequal( pszExt, "i3Chr") )
	{
		type = I3RSC_TYPE_CHARA;
	}
	else if( i3::generic_is_iequal( pszExt, "i3wrd") )
	{
		type = I3RSC_TYPE_WORLD;
	}
	else if( i3::generic_is_iequal( pszExt, "I3A") )
	{
		type = I3RSC_TYPE_ANIMATION;
	}
	else if( i3::generic_is_iequal( pszExt, "I3I") ||
		i3::generic_is_iequal( pszExt, "TGA")  ||
		i3::generic_is_iequal( pszExt, "BMP") )
	{
		type = I3RSC_TYPE_TEXTURE;
	}
	else if(i3::generic_is_iequal( pszExt, "WAV") ||
		i3::generic_is_iequal( pszExt, "VAG")  ||
		i3::generic_is_iequal( pszExt, "I3SND") )
	{
		type = I3RSC_TYPE_SOUND;
	}
	else if(i3::generic_is_iequal( pszExt, "i3AnimPack"))
	{
		type = I3RSC_TYPE_ANIMPACK;
	}
	else if( i3::generic_is_iequal( pszExt, "i3Obj"))
	{
		type = I3RSC_TYPE_OBJECT;
	}
	else if( i3::generic_is_iequal( pszExt, "i3UI"))
	{
		type = I3RSC_TYPE_UI;
	}
	
	return type;
}


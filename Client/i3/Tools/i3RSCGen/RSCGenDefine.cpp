#include "stdafx.h"
#include "RSCGenDefine.h"
#include "i3Base/i3ParserHelper.h"
#include "GlobalVar.h"

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

	m_FieldList.clear();
	m_pIniParser	= NULL;
}

void CFieldPack::CreateInternal( INT32 nFieldCount)
{
	static INT32 nCreateCount = 0;
	I3ASSERT( nFieldCount != 0);

	m_FieldList.clear();

	m_nFieldCount	= nFieldCount;

	for(INT32 i = 0; i < m_nFieldCount; ++i)
	{
		RSC_FIELD_INFO_PTR spInfo = i3::make_shared<RSC_FIELD_INFO>();
			   
		m_FieldList.push_back( spInfo);

		nCreateCount ++;
	}
}

void CFieldPack::Create( UINT32	nRSCType)
{
	INT32 nType		= nRSCType & I3RSC_TYPE_MASK;
	INT32 nIndex	= 0;

	INT32 nStyle	= g_RSCStyleDef[nType].m_nFieldPackStyle;
	INT32 nCount	= g_RSCStyleDef[nType].m_nFieldCount;

	CFieldPack::CreateInternal( nCount);

	INT32 nSt = nStyle & I3RSC_FIELDPACK_MASK;
	nSt = nSt >> 16;

	for( INT32 i = 0; i< I3RSC_FIELDPACK_COUNT; ++i)
	{
		if( nSt & (1 << i))
		{
			RSC_FIELD_INFO * pInfo = m_FieldList[nIndex].get();		
					// g_szFieldInfoDef의 플래그 인덱스가 아니라 쌓인 순서대로 들어가게된다.

			InitField( pInfo, g_szFieldInfoDef[i]); 

			//Type Field Set		// 0인 경우 자기 자신이 들어가는 모냥..(2014.04.15.수빈)
			if( i == 0)
			{
				pInfo->m_FieldData = g_RSCStyleDef[nType].m_strFieldPackName; 
			}

			nIndex++;

			if (nIndex >= INT32(m_FieldList.size()) )			// 크래시 버그문제로 삽입한다.(2014.04.14.수빈)
				break;
		}
	}
}

void CFieldPack::InitField( RSC_FIELD_INFO * pInfo, const I3RSC_FIELD_STYLE_DEF& def)
{
	I3ASSERT( pInfo != NULL);
	
	pInfo->m_strFieldName = def.m_strFieldName;
	pInfo->m_nFieldStyle	= def.m_nFieldStyle;


	if( pInfo->m_nFieldStyle & I3RSC_FIELD_TYPE_STRING)
	{
		pInfo->m_FieldData = i3::rc_string();
	}
	else if( pInfo->m_nFieldStyle & I3RSC_FIELD_TYPE_INDEX)
	{
		pInfo->m_FieldData = 0;
	}
	else if( pInfo->m_nFieldStyle & I3RSC_FIELD_TYPE_VALUE)
	{
		pInfo->m_FieldData = 0;
	}
	else if( pInfo->m_nFieldStyle & I3RSC_FIELD_TYPE_REALVALUE)
	{
		pInfo->m_FieldData = 0.f;
	}
}

RSC_FIELD_INFO * CFieldPack::GetField( INT32 nIndex)
{
//	if( m_pFieldInfo == NULL)	
//		return NULL;

	if( nIndex >= m_nFieldCount)	
		return NULL;

	//return m_pFieldInfo + nIndex;
	return m_FieldList[nIndex].get();
}

void CFieldPack::Save(i3IniParser * pParser)
{
	I3ASSERT( pParser != NULL);

	INT32 i = 0;

	m_pIniParser = pParser;


	RSC_FIELD_INFO * pInfo = m_FieldList[1].get();
	//Section Name이 Relative Path가 됨
	i3::rc_string* strVal = i3::unsafe_any_cast<i3::rc_string>(&pInfo->m_FieldData);

	pParser->WriteSectionName( strVal->c_str() );


	//공통 항목인 Type Field
	_SaveField( m_FieldList[0].get());				//Type

	for( i = 2; i < m_nFieldCount; ++i)			// 아니... i == 1에 대해선 위에서 섹션이름으로 저장 ( 2014.04.15.수빈)
	{											
		_SaveField( m_FieldList[i].get());		//Type
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
		pParser->WriteValue( pInfo->m_strFieldName.c_str(), *i3::unsafe_any_cast<int>(&pInfo->m_FieldData));
	}
	else if( nStyle & I3RSC_FIELD_TYPE_STRING)
	{
		pParser->WriteValue(  pInfo->m_strFieldName.c_str(), (*i3::unsafe_any_cast<i3::rc_string>(&pInfo->m_FieldData)).c_str());
	}
	else if( nStyle & I3RSC_FIELD_TYPE_VALUE)
	{
		pParser->WriteValue(  pInfo->m_strFieldName.c_str(), *i3::unsafe_any_cast<int>(&pInfo->m_FieldData));
	}
	else if( nStyle & I3RSC_FIELD_TYPE_REALVALUE)
	{
		pParser->WriteValue(  pInfo->m_strFieldName.c_str(), *i3::unsafe_any_cast<float>(&pInfo->m_FieldData));
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
	GET_IMPORTANT_STRING( "Type", 0, szString, sizeof(szString));
	//

	i3::iterator_range<const char*> strString = i3::as_literal(szString);

	if( i3::generic_is_iequal( strString, "SCENE") )			nType = I3RSC_TYPE_SCENE;
	else if( i3::generic_is_iequal( strString, "CHARA") )		nType = I3RSC_TYPE_CHARA;
	else if( i3::generic_is_iequal( strString, "WORLD"))		nType = I3RSC_TYPE_WORLD;
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
	RSC_FIELD_INFO * pInfo = m_FieldList[1].get();
	
	INT32 nSt = g_RSCStyleDef[nType].m_nFieldPackStyle & I3RSC_FIELDPACK_MASK;
	nSt = nSt >> 16;

	pInfo->m_FieldData = i3::rc_string(pSectionInfo->Name);

	nIndex	= 2;
	for( INT32 i = 2; i< I3RSC_FIELDPACK_COUNT; ++i)
	{
		if( nSt & (1 << i))
		{
			pInfo = m_FieldList[nIndex].get();

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
	i3::generic_string_copy(szFieldName, g_szFieldInfoDef[nType].m_strFieldName);


	if( nStyle & I3RSC_FIELD_TYPE_INDEX)
	{
		INT32 val;
		GET_VALUE( szFieldName, 0, &val);
		pInfo->m_FieldData = val;
	}
	else if( nStyle & I3RSC_FIELD_TYPE_STRING)
	{
		GET_STRING( szFieldName, "", conv, sizeof( conv));
		pInfo->m_FieldData = i3::rc_string(conv);
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
			INT32 val;
			GET_VALUE( szFieldName, 0, &val);
			pInfo->m_FieldData = val;
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
			REAL32 val;
			GET_VALUE( szFieldName, 0.0f, &val);
			pInfo->m_FieldData = (float)val;
		}
	}

ExitLabel:
	return ;
}


/////////////////////////////////////////////////////////////////////////////////
INT32		Geti3RscTypeWithFileExt( const char * pszExt)
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
	else if( i3::generic_is_iequal( pszExt, "I3I")  ||
		i3::generic_is_iequal( pszExt, "TGA")  ||
		i3::generic_is_iequal( pszExt, "BMP") )
	{
		type = I3RSC_TYPE_TEXTURE;
	}
	else if(i3::generic_is_iequal( pszExt, "WAV")  ||
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


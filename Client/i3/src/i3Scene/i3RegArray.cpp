#include "i3SceneDef.h"

#include "i3RegArray.h"

#include "i3RegINT32.h"
#include "i3RegREAL32.h"
#include "i3RegString.h"
#include "i3RegVEC2D.h"
#include "i3RegVEC3D.h"
#include "i3RegVEC4D.h"
#include "i3RegCOLOR.h"
#include "i3RegMATRIX.h"

#include "i3Registry.h"
#include "i3Base/string/compare/generic_is_niequal.h"
#include "i3Base/string/compare/generic_is_nequal.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/string/ext/mb_to_utf16.h"


I3_CLASS_INSTANCE( i3RegArray);

std::vector< std::pair< INT32, INT32 > >* i3RegArray::m_pSkipFPBank = 0;
bool i3RegArray::m_bUpdateSkipFP = true;

namespace 
{
	struct Delete_For_m_pSkipFPBank
	{
		~Delete_For_m_pSkipFPBank()
		{
			i3RegArray::ReleaseSkipFPBank();
		}
	} _any_;
}

i3RegArray::i3RegArray(void)
{
	m_nDataType = I3REG_TYPE_ARRAY;
}

i3RegArray::~i3RegArray(void)
{
	clearRegData();
}

void i3RegArray::ReleaseSkipFPBank()		// 이 함수를 빼먹는 경우가 있어, RAII처리 한번 더 함..(2012.09.11.수빈)
{
	if( m_pSkipFPBank )
	{
		m_pSkipFPBank->clear();
		I3_SAFE_DELETE( m_pSkipFPBank);
	}
}

void i3RegArray::set( i3RegArray & Value)
{
	m_nElementDataType = Value.m_nElementDataType;

	size_t size = Value.getSize();
	for(size_t i = 0; i < size; ++i)
	{
		addRegData( createRegData() );
		Value[i]->CopyTo( m_Value[i], I3_COPY_INSTANCE );
	}
}

void i3RegArray::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegArray*>(pDest));

	i3RegArray * pArray = ( i3RegArray*)pDest;

	pArray->set( *this );
}

void	i3RegArray::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

void i3RegArray::GetValueByString( i3::wstring& out )
{
	i3::sprintf(out, L"%5d", m_Value.size() );			// 일단, 사이즈라도 리턴할수 있게 바꾼다..
}														// 원 소스코드는 오류이다..

void i3RegArray::clearRegData()
{
	size_t size = m_Value.size();
	for(size_t i = 0; i < size; i++)
	{
		I3_SAFE_RELEASE( m_Value[i] );
	}
	m_Value.clear();
}

i3RegData* i3RegArray::createRegData() const
{
	i3RegData* pNewData = nullptr;

	switch( m_nElementDataType )
	{
	case	I3REG_TYPE_INT32:	pNewData = (i3RegData*)i3RegINT32::new_object();	break;
	case	I3REG_TYPE_REAL32:	pNewData = (i3RegData*)i3RegREAL32::new_object();	break;
	case	I3REG_TYPE_STRING:	pNewData = (i3RegData*)i3RegString::new_object();	break;
	case	I3REG_TYPE_VEC2D:	pNewData = (i3RegData*)i3RegVEC2D::new_object();	break;
	case	I3REG_TYPE_VEC3D:	pNewData = (i3RegData*)i3RegVEC3D::new_object();	break;
	case	I3REG_TYPE_VEC4D:	pNewData = (i3RegData*)i3RegVEC4D::new_object();	break;
	case	I3REG_TYPE_COLOR:	pNewData = (i3RegData*)i3RegCOLOR::new_object();	break;
	case	I3REG_TYPE_MATRIX:	pNewData = (i3RegData*)i3RegMATRIX::new_object();	break;
	default:
		break;
	}

	return pNewData;
}

void i3RegArray::addRegData(i3RegData * pData)
{
	I3ASSERT( m_nElementDataType ==  pData->getDataType() );

	m_Value.push_back( pData );
}

void i3RegArray::removeRegData(size_t Index)
{
	I3ASSERT( Index < getSize() );

	m_Value.erase( m_Value.begin() + Index );
}

void i3RegArray::replaceRegData(size_t Index, i3RegData * data)
{
	I3ASSERT( m_nElementDataType ==  data->getDataType() );
	I3ASSERT( Index < m_Value.size() );

	I3_SAFE_RELEASE( m_Value[ Index ] );
	m_Value[ Index ] = data;
}

void i3RegArray::replaceRegData(size_t Index, INT32 nValue)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_INT32 );
	I3ASSERT( Index < m_Value.size() );

	((i3RegINT32*)m_Value[ Index ])->setValue( nValue );
}

void i3RegArray::replaceRegData(size_t Index, REAL32 fValue)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_REAL32 );
	I3ASSERT( Index < m_Value.size() );

	((i3RegREAL32*)m_Value[ Index ])->setValue( fValue );
}

void i3RegArray::replaceRegData(size_t Index, const i3::rc_wstring& val)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_STRING );
	I3ASSERT( Index < m_Value.size() );

	((i3RegString*)m_Value[ Index ])->setValue( val );
}

void i3RegArray::replaceRegData(size_t Index, const VEC2D & Vec2D)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_VEC2D );
	I3ASSERT( Index < m_Value.size() );

	*((i3RegVEC2D*)m_Value[ Index ]) = (VEC2D)Vec2D;
}

void i3RegArray::replaceRegData(size_t Index, const VEC3D & Vec3D)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_VEC3D );
	I3ASSERT( Index < m_Value.size() );

	*((i3RegVEC3D*)m_Value[ Index ]) = Vec3D;
}

void i3RegArray::replaceRegData(size_t Index, const VEC4D & Vec4D)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_VEC4D );
	I3ASSERT( Index < m_Value.size() );

	*((i3RegVEC4D*)m_Value[ Index ]) = Vec4D;
}

void i3RegArray::replaceRegData(size_t Index, const I3COLOR & Col)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_COLOR );
	I3ASSERT( Index < m_Value.size() );

	*((i3RegCOLOR*)m_Value[ Index ]) = Col;
}


void i3RegArray::replaceRegData(size_t Index, const MATRIX & Matrix)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_MATRIX );
	I3ASSERT( Index < m_Value.size() );

	*((i3RegMATRIX*)m_Value[ Index ]) = Matrix;
}



bool i3RegArray::isSameRegData(size_t Index, INT32 nValue)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_INT32 );
	I3ASSERT( Index < m_Value.size() );

	if(*(( (i3RegINT32*)m_Value[ Index ])->getValue())==nValue)
		return true;
	return false;
}

bool i3RegArray::isSameRegData(size_t Index, REAL32 fValue)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_REAL32 );
	I3ASSERT( Index < m_Value.size() );

	if(*(( (i3RegREAL32*)m_Value[ Index ])->getValue())==fValue)
		return true;
	return false;
}

bool i3RegArray::isSameRegData(size_t Index, const char * Value)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_STRING );
	I3ASSERT( Index < m_Value.size() );

	INT32 len;
	len = i3::generic_string_size( Value);
	if( i3::generic_is_niequal( ((i3RegString*)m_Value[ Index ])->getValue(), Value, len) == 0)
		return true;

	return false;
}

bool i3RegArray::isSameRegData(size_t Index, const VEC2D & Vec2D)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_VEC2D );
	I3ASSERT( Index < m_Value.size() );

	if( memcmp(((i3RegVEC2D*)m_Value[ Index ]), &Vec2D, sizeof(VEC2D)) == 0)
		return true;
	return false;
}

bool i3RegArray::isSameRegData(size_t Index, const VEC3D & Vec3D)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_VEC3D );
	I3ASSERT( Index < m_Value.size() );

	if( memcmp(((i3RegVEC3D*)m_Value[ Index ]), &Vec3D, sizeof(i3RegVEC3D)) == 0)
		return true;
	return false;
}

bool i3RegArray::isSameRegData(size_t Index, const VEC4D & Vec4D)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_VEC4D );
	I3ASSERT( Index < m_Value.size() );

	if( memcmp(((i3RegVEC4D*)m_Value[ Index ]), &Vec4D, sizeof(i3RegVEC4D)) == 0)
		return true;

	return false;
}

bool i3RegArray::isSameRegData(size_t Index, const I3COLOR & Col)
{
	I3ASSERT( m_nElementDataType ==  I3REG_TYPE_COLOR );
	I3ASSERT( Index < m_Value.size() );

	if( memcmp(((i3RegCOLOR*)m_Value[ Index ]), &Col, sizeof(i3RegCOLOR)) == 0)
		return true;
	return false;
}


UINT32	i3RegArray::OnSave( i3ResourceFile * pResFile)
{
	i3Stream * pStream = pResFile->GetStream();
	UINT32	Result = 0, Rc = 0;

	Result = i3RegData::OnSave( pResFile );
	I3_CHKIO( Result);

	//Element Type 을 저장한다.
	Rc = pStream->Write( &m_nElementDataType, sizeof( INT32));
	I3_CHKIO( Rc);
	Result += Rc;

	//Element Size 를 저장한다..
	size_t size = m_Value.size();
	Rc = pStream->Write( &size, sizeof( INT32));
	I3_CHKIO( Rc);
	Result += Rc;
	
	Result += _SaveValue(pStream, size);

	return Result;
}


#if defined ( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED COLOR
	{
		UINT8 Col[4] = { 0 };
	};

	struct PACKED STRING
	{
		UINT8	m_ID[4] = { 'R', 'G', 'S', '3' };	// RSG2 --> RGS3로 변경...(UTF-8과 저울질 중 일단 wchar_t를 취하기로 했다.) (2014.07.01.수빈)
		UINT32	m_nStrLength = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3RegArray::_SaveValue(i3Stream * pStream, size_t size)
{
	UINT32	Result = 0, Rc = 0;

	switch( m_nElementDataType )
	{
	case	I3REG_TYPE_INT32:
		{
			UINT32 Value = 0;
			for(size_t i = 0; i < size; i++)
			{
				Value = *((i3RegINT32*)m_Value[i])->getValue();
				Rc = pStream->Write( &Value, sizeof( INT32));
				I3_CHKIO( Rc);
				Result += Rc;
			}
		}
		break;
	case	I3REG_TYPE_REAL32:	
		{
			REAL32 Value;
			for(size_t i = 0; i < size; i++)
			{
				Value = *((i3RegREAL32*)m_Value[i])->getValue();
				Rc = pStream->Write( &Value, sizeof( REAL32 ));
				I3_CHKIO( Rc);
				Result += Rc;
			}
		}
		break;
	case	I3REG_TYPE_STRING:
		{
		//	const char* Value;
			
			pack::STRING data;

			for(size_t i = 0; i < size; i++)
			{
				const i3::rc_wstring& strValue = ((i3RegString*)m_Value[i])->getValue();

				data.m_nStrLength = strValue.size();

				Rc = pStream->Write( &data, sizeof( data ) );
				I3_CHKIO( Rc);
				Result += Rc;

				if( data.m_nStrLength > 0)
				{
					Rc = pStream->Write( strValue.c_str(), data.m_nStrLength * sizeof(wchar_t) );
					I3_CHKIO( Rc);
					Result += Rc;
				}
			}
		}		
		break;
	case	I3REG_TYPE_VEC2D:
		{
			VEC2D Value;
			i3::pack::VEC2D	data;

			for(size_t i = 0; i < size; i++)
			{
				Value = *((i3RegVEC2D*)m_Value[i])->getValue();
				{
					data.x = Value.x;
					data.y = Value.y;
				}
				Rc = pStream->Write( &data, sizeof( i3::pack::VEC2D ));
				I3_CHKIO( Rc);
				Result += Rc;
			}
		}		
		break;
	case	I3REG_TYPE_VEC3D:
		{
			VEC3D Value;
			i3::pack::VEC3D	data;

			for(size_t i = 0; i < size; i++)
			{
				Value = *((i3RegVEC3D*)m_Value[i])->getValue();
				{
					data.x = Value.x;
					data.y = Value.y;
					data.z = Value.z;
				}
				Rc = pStream->Write( &data, sizeof( i3::pack::VEC3D ));
				I3_CHKIO( Rc);
				Result += Rc;
			}		
		}
		break;
	case	I3REG_TYPE_VEC4D:
		{
			VEC4D Value;
			i3::pack::VEC4D	data;

			for(size_t i = 0; i < size; i++)
			{
				Value = *((i3RegVEC4D*)m_Value[i])->getValue();
				{
					data.x = Value.x;
					data.y = Value.y;
					data.z = Value.z;
					data.w = Value.w;
				}
				Rc = pStream->Write( &data, sizeof( i3::pack::VEC4D ));
				I3_CHKIO( Rc);
				Result += Rc;
			}		
		}
		break;
	case	I3REG_TYPE_COLOR:
		{
			I3COLOR Value;
			pack::COLOR data;

			for(size_t i = 0; i < size; i++)
			{
				Value = *((i3RegCOLOR*)m_Value[i])->getValue();
				{
					data.Col[0] = Value.r;
					data.Col[1] = Value.g;
					data.Col[2] = Value.b;
					data.Col[3] = Value.a;
				}
				Rc = pStream->Write( &data, sizeof( pack::COLOR ));
				I3_CHKIO( Rc);
				Result += Rc;
			}		
		}
		break;
	//case	I3REG_TYPE_MATRIX:	pNewData = (i3RegData*)i3RegMATRIX::new_object();	break;
	default:
		break;
	}

	return Result;
}

UINT32	i3RegArray::OnLoad( i3ResourceFile * pResFile)
{
	if( m_bUpdateSkipFP )
		_Internal_UpdateSkipFP();

	UINT32				Result = 0, Rc = 0;
	i3Stream	*		pStream = pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &m_nElementDataType, sizeof( INT32));
	I3_CHKIO( Rc);
	Result += Rc;

	size_t size;
	Rc = pStream->Read( &size, sizeof( INT32));
	I3_CHKIO( Rc);
	Result += Rc;

	bool SpecificNation = (i3Registry::Get_LocationIndex() != 0);

	bool IsStamp = i3Registry::IsStampString( GetName() );

	Result += _LoadValue(pStream, size, SpecificNation, IsStamp );


	return Result;
}

void i3RegArray::_Internal_UpdateSkipFP()
{
	m_bUpdateSkipFP = false;

	INT32 MaxNationCount	= i3Registry::Get_MaxLocationCount();
	INT32 NationIndex		= i3Registry::Get_LocationIndex();

	if( !m_pSkipFPBank )
	{
		m_pSkipFPBank = new std::vector< std::pair< INT32, INT32 > >;
		NetworkDump_Malloc( m_pSkipFPBank, sizeof( std::vector< std::pair<INT32,INT32>>), __FILE__, __LINE__);
		(*m_pSkipFPBank).resize( I3REG_TYPE_MAX );
	}

	INT32 SizeOf = 0;
	for(size_t i = 0; i < I3REG_TYPE_MAX; ++i)
	{
		switch( i )
		{
		case	I3REG_TYPE_INT32:	SizeOf = sizeof( UINT32 );	break;
		case	I3REG_TYPE_REAL32:	SizeOf = sizeof( REAL32 );	break;
//		case	I3REG_TYPE_STRING:	SizeOf = sizeof( );	break;
		case	I3REG_TYPE_VEC2D:	SizeOf = sizeof( i3::pack::VEC2D );	break;
		case	I3REG_TYPE_VEC3D:	SizeOf = sizeof( i3::pack::VEC3D );	break;
		case	I3REG_TYPE_VEC4D:	SizeOf = sizeof( i3::pack::VEC4D );	break;
		case	I3REG_TYPE_COLOR:	SizeOf = sizeof( pack::COLOR );	break;
//		case	I3REG_TYPE_MATRIX:	SizeOf = sizeof( );	break;
		default:
			SizeOf = 0;
			break;
		}

		(*m_pSkipFPBank)[i].first = SizeOf * NationIndex;
		(*m_pSkipFPBank)[i].second = SizeOf * (MaxNationCount - (NationIndex + 1) ) ;
	}
}
/*
static void ReplaceString(char* strDesc, const char* strSrc,const char* strOld,const char* strNew)
{
	I3ASSERT(strDesc);
	I3ASSERT(strSrc);
	I3ASSERT(strOld);
	I3ASSERT(strNew);
	
    UINT32 srcPosition= 0;
	UINT32 oldPosition = 0;

	// 원본 끝까지
	while(*strSrc)
	{
		// 교체할 문자열 비교
		if (strSrc[srcPosition] == strOld[oldPosition])
		{
			srcPosition++;
			oldPosition++;

			// 모든 교체 문자열이 일치하면 새 문자열로 치환
			if (nullptr == strOld[oldPosition])
			{
				UINT32 i = 0;
				
				while(strNew[i])
				{
					*strDesc = strNew[i];
					strDesc++;
					i++;
				}
				
				strSrc += srcPosition;
				srcPosition = 0;
				oldPosition = 0;
			}
		}
		else
		{
			srcPosition++;
			oldPosition = 0;

			// 일치하지 않으면 일반 복사
			for(UINT32 i = 0; i < srcPosition; i++)
			{
				*strDesc = *strSrc;
				strDesc++;
				strSrc++;
			}

			srcPosition = 0;
		}
	}

	*strDesc = nullptr;
}
*/

//static char regStringTemp[MAX_PATH * 4];

UINT32 i3RegArray::_LoadValue(i3Stream * pStream, size_t size, bool SpecificNation, bool IsStamp)
{
	UINT32	Result = 0, Rc = 0;

	switch( m_nElementDataType )
	{
	case	I3REG_TYPE_INT32:
		{
			UINT32 Value;
			i3RegData* pNewData;

			if( SpecificNation )
			{
				pNewData = createRegData();
				addRegData( pNewData );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].first, STREAM_CURRENT );

				Rc = pStream->Read( &Value, sizeof( INT32));
				I3_CHKIO( Rc);
				Result += Rc;

				((i3RegINT32*)pNewData)->setValue( Value );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].second, STREAM_CURRENT );
			}
			else
			{
				for(size_t i = 0; i < size; i++)
				{
					pNewData = createRegData();
					addRegData( pNewData );

					Rc = pStream->Read( &Value, sizeof( INT32));
					I3_CHKIO( Rc);
					Result += Rc;

					((i3RegINT32*)pNewData)->setValue( Value );
				}
			}
		}
		break;
	case	I3REG_TYPE_REAL32:	
		{
			REAL32 Value;
			i3RegData* pNewData;
			if( SpecificNation )
			{
				pNewData = createRegData();
				addRegData( pNewData );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].first, STREAM_CURRENT );

				Rc = pStream->Read( &Value, sizeof( REAL32 ));
				I3_CHKIO( Rc);
				Result += Rc;

				((i3RegREAL32*)pNewData)->setValue( Value );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].second, STREAM_CURRENT );
			}
			else
			{
				for(size_t i = 0; i < size; i++)
				{
					pNewData = createRegData();
					addRegData( pNewData );

					Rc = pStream->Read( &Value, sizeof( REAL32 ));
					I3_CHKIO( Rc);
					Result += Rc;

					((i3RegREAL32*)pNewData)->setValue( Value );
				}
			}
		}
		break;
	case	I3REG_TYPE_STRING:
		{
			pack::STRING data;
			i3RegData* pNewData = 0;

			INT32 NationIndex = i3Registry::Get_LocationIndex();

			for(size_t i = 0; i < size; i++)
			{
				if( !IsStamp && (!SpecificNation || i == (UINT32)NationIndex)  )
				{
					pNewData = createRegData();
					addRegData( pNewData );
				}

				Rc = pStream->Read( &data, sizeof( pack::STRING ));
				I3_CHKIO( Rc);
				Result += Rc;

				if( !IsStamp && (!SpecificNation || i == (UINT32)NationIndex)  )
				{
					if( data.m_nStrLength > 0)
					{

//						if( i3String::NCompare((const char*)data.m_ID, "RGS2", 4) == 0)
						if (i3::generic_is_nequal((const char*)data.m_ID, "RGS3", 4) )
						{
							i3::rc_wstring strTemp( data.m_nStrLength, 0); 
							Rc = pStream->Read( const_cast<wchar_t*>(&strTemp[0]), data.m_nStrLength * sizeof(wchar_t) );
							I3_CHKIO( Rc);
							Result += Rc;

							((i3RegString*)pNewData)->setValue( strTemp );
						}
						else
						if (i3::generic_is_nequal((const char*)data.m_ID, "RGS2", 4) )
						{			
							i3::stack_string	strTemp(data.m_nStrLength, 0); 
							Rc = pStream->Read( const_cast<char*>(&strTemp[0]), data.m_nStrLength);
							I3_CHKIO( Rc);
							Result += Rc;
							
							i3::rc_wstring wstrTemp;	i3::mb_to_utf16(strTemp, wstrTemp);
							((i3RegString*)pNewData)->setValue( wstrTemp );
						}
						else
						{
							i3::stack_string	regStringTemp( data.m_nStrLength, 0);
							Rc = pStream->Read( &regStringTemp[0], data.m_nStrLength);

							if( Rc == STREAM_ERR)
							{
								I3PRINTLOG(I3LOG_FATAL,  "");
								return STREAM_ERR;
							}
							Result += Rc;

							i3::generic_string_replace_all(regStringTemp, "\\n", "\n");
						
							i3::rc_wstring wstrTemp;	i3::mb_to_utf16(regStringTemp, wstrTemp);
							((i3RegString*)pNewData)->setValue( wstrTemp );
						}
				
					}
				}
				else
				{
					if( data.m_nStrLength > 0)
					{
						UINT32 size_char = sizeof(char);

						if (i3::generic_is_nequal((const char*)data.m_ID, "RGS3", 4) )
							size_char = sizeof(wchar_t);

						pStream->SetPosition( data.m_nStrLength * size_char, STREAM_CURRENT );
					}
				}
			}
		}
		break;
	case	I3REG_TYPE_VEC2D:
		{
			i3::pack::VEC2D Value;
			i3RegData* pNewData;
			if( SpecificNation )
			{
				pNewData = createRegData();
				addRegData( pNewData );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].first, STREAM_CURRENT );

				Rc = pStream->Read( &Value, sizeof( i3::pack::VEC2D ));
				I3_CHKIO( Rc);
				Result += Rc;

				((i3RegVEC2D*)pNewData)->setValue( Value.x, Value.y );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].second, STREAM_CURRENT );
			}
			else
			{
				for(size_t i = 0; i < size; i++)
				{
					pNewData = createRegData();
					addRegData( pNewData );

					Rc = pStream->Read( &Value, sizeof( i3::pack::VEC2D ));
					I3_CHKIO( Rc);
					Result += Rc;

					((i3RegVEC2D*)pNewData)->setValue( Value.x, Value.y );
				}
			}
		}
		break;
	case	I3REG_TYPE_VEC3D:
		{
			i3::pack::VEC3D Value;
			i3RegData* pNewData;

			if( SpecificNation )
			{
				pNewData = createRegData();
				addRegData( pNewData );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].first, STREAM_CURRENT );

				Rc = pStream->Read( &Value, sizeof( i3::pack::VEC3D ));
				I3_CHKIO( Rc);
				Result += Rc;

				((i3RegVEC3D*)pNewData)->setValue( Value.x, Value.y, Value.z );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].second, STREAM_CURRENT );
			}
			else
			{
				for(size_t i = 0; i < size; i++)
				{
					pNewData = createRegData();
					addRegData( pNewData );

					Rc = pStream->Read( &Value, sizeof( i3::pack::VEC3D ));
					I3_CHKIO( Rc);
					Result += Rc;

					((i3RegVEC3D*)pNewData)->setValue( Value.x, Value.y, Value.z );
				}
			}
		}
		break;
	case	I3REG_TYPE_VEC4D:
		{
			i3::pack::VEC4D Value;
			i3RegData* pNewData;

			if( SpecificNation )
			{
				pNewData = createRegData();
				addRegData( pNewData );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].first, STREAM_CURRENT );

				Rc = pStream->Read( &Value, sizeof( i3::pack::VEC4D ));
				I3_CHKIO( Rc);
				Result += Rc;

				((i3RegVEC4D*)pNewData)->setValue( Value.x, Value.y, Value.z, Value.w );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].second, STREAM_CURRENT );
			}
			else
			{
				for(size_t i = 0; i < size; i++)
				{
					pNewData = createRegData();
					addRegData( pNewData );

					Rc = pStream->Read( &Value, sizeof( i3::pack::VEC4D ));
					I3_CHKIO( Rc);
					Result += Rc;

					((i3RegVEC4D*)pNewData)->setValue( Value.x, Value.y, Value.z, Value.w );
				}
			}
		}
		break;
	case	I3REG_TYPE_COLOR:
		{
			pack::COLOR Value;
			i3RegData* pNewData;

			if( SpecificNation )
			{
				pNewData = createRegData();
				addRegData( pNewData );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].first, STREAM_CURRENT );

				Rc = pStream->Read( &Value, sizeof( pack::COLOR ));
				I3_CHKIO( Rc);
				Result += Rc;

				((i3RegCOLOR*)pNewData)->setValue( Value.Col[0], Value.Col[1], Value.Col[2], Value.Col[3] );

				pStream->SetPosition( (*m_pSkipFPBank)[m_nElementDataType].second, STREAM_CURRENT );
			}
			else
			{
				for(size_t i = 0; i < size; i++)
				{
					pNewData = createRegData();
					addRegData( pNewData );

					Rc = pStream->Read( &Value, sizeof( pack::COLOR ));
					I3_CHKIO( Rc);
					Result += Rc;

					((i3RegCOLOR*)pNewData)->setValue( Value.Col[0], Value.Col[1], Value.Col[2], Value.Col[3] );
				}
			}
		}
		break;
	//case	I3REG_TYPE_MATRIX:	pNewData = (i3RegData*)i3RegMATRIX::new_object();	break;
	default:
		break;
	}

	return Result;
}
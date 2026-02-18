#include "pch.h"
#include "LocaleValue.h"

#include <string>
#include <algorithm>
#include "i3Base/itl/container_util.h"
#include "i3Base/string/algorithm/to_lower.h"
#include "i3Base/string/ext/lrtrim.h"

static	i3::vector<i3RegKey*>	s_RegKeyList;


/*	국가별 설정 값 저장소
std::string : 속성 스트링 인덱스
i3RegData* : 속성 값
*/
typedef i3::vector_map< i3::string, i3RegData* > LocaleContext;
static LocaleContext g_LocaleContext;

/*	트리 구조에 Pef 에 저장된 값들을 재귀적으로 구성한다.
속성 스트링 인덱스 구성 방식은 부모 스트링/자식 스트링 으로 구성된다. (구분자 '/')
pKey	: 레지스트리 키
Name	: 속성 스트링 인덱스
*/
void _SpreadRegistry(i3RegKey* pKey, const i3::string & Name)
{
	i3::string IdName, TempName;
	i3RegData* pData = nullptr;

	INT32 DataCount = pKey->GetDataCount();
	for(INT32 i = 0; i < DataCount; ++i)
	{
		pData = pKey->GetData(i);

		TempName = pData->GetNameString();

		if( TempName.find_first_of("__") == 0 && 
			TempName.rfind("Time") != i3::string::npos )
			continue;

		IdName = Name;
		IdName += TempName;

		g_LocaleContext.insert( 
			LocaleContext::value_type( IdName, pData)
			);
	}


	INT32 ChildCount = pKey->getChildCount();
	for(INT32 i = 0; i < ChildCount; ++i)
	{
		i3RegKey * pChildRegKey = (i3RegKey *)pKey->getChild(i);

		TempName = pChildRegKey->GetNameString();
		TempName += "/";

		_SpreadRegistry( pChildRegKey, TempName );
	}
}

/*	국가별 설정 값 저장소에 저장된 속성 값을 읽어 온다.
ValueID	: 읽어 올 해당 스트링 인덱스
return	:	해당 스트링 인덱스에 속성 데이터 값
				만약, 스트링 인덱스에 속성 값이 없는 경우 0 을 리턴한다.
*/
i3RegData* 	_GetLocaleContextIterator( const i3::string & ValueID )
{
	LocaleContext::iterator It = g_LocaleContext.find( ValueID );

	if( It == g_LocaleContext.end() )
	{
		return 0;
	}

	return It->second;
}

size_t _ParseNationFile(size_t & RefIndex)
{
	RefIndex = size_t(-1);
	size_t RetMaxIndex = 0;
	//서비스 지역 얻어오기
	
	i3::stack_string LocationName( GetConfigLocale());

	LocationName = 	LocationName.substr( 1, LocationName.size() - 2 );


	char* szNationPath = "config/Nation.nlf";

	i3FileStream	kStringStream;
	if( !kStringStream.Open(szNationPath, STREAM_READ | STREAM_SHAREREAD) )
	{
#ifdef NC_BUILD
		I3PRINTLOG(I3LOG_FATAL,  "Stream IO Error! %s", "config/Nation.nlf" );
#else
		I3PRINTLOG(I3LOG_FATAL,  "%s 파일을 열 수 없습니다. working directory를 확인하세요", szNationPath );
#endif
		goto ExitLabel;
	}

	int nLineCount = 1;
	bool bContinue = true;

	// 라벨 파일을 끝까지 읽자.
	RefIndex = 0;
	while( bContinue )
	{
		char szBuffer[2048];

		UINT32 uStringReaded = kStringStream.ReadLine( szBuffer, _countof(szBuffer) );
		if( uStringReaded == 0 )
		{
			if( kStringStream.GetLastError() != i3Stream::STREAM_EOS )
			{
				// ERROR
				I3PRINTLOG(I3LOG_FATAL,  "Failed to read nation from file. (line = %d) ( file = %s )", nLineCount, szNationPath );
				goto ExitLabel;
			}
			else
			{
				bContinue = false;
			}
		}
		else
		{
			if( strlen(szBuffer) == 0 )
				continue;
		}

		if( bContinue )
		{
			i3::stack_string NationString(szBuffer);

			//주석 제거
			if( !NationString.empty() )
			{
				size_t Index = NationString.rfind(";");
				if( Index != i3::stack_string::npos )
					NationString.erase( Index );
			}
			
			//양쪽 Trim
			i3::lrtrim(NationString);

			
			if( !NationString.empty() )
			{

				if( i3::generic_is_iequal(LocationName, NationString) )
					RefIndex = RetMaxIndex;

				SERVICECOUNTRY Country;
				Country._index = RetMaxIndex;
				i3::generic_string_copy(Country._countryName, NationString);
				i3UI::getCountryList()->push_back(Country);

				++RetMaxIndex;
			}
		}
	}

ExitLabel:
	return RetMaxIndex;
}

bool LocaleValue::Init()
{
	//통합 Pef에 대한 국가를 지정한다.
	size_t MaxCount = 0;
	size_t Index;
	{
		MaxCount = _ParseNationFile( Index );
		
		if( Index == size_t(-1) )
		{
			I3PRINTLOG(I3LOG_FATAL, "Not Found Nation File!");
		}

		i3Registry::Set_LoadStamp( false );	//Stamp 데이터를 읽지 않는다.
		i3Registry::Set_LocationIndex( Index, MaxCount );
	}

	{
		i3RegistrySet RegFile;
		RegFile.SetCodePage( GetCodePage() );

		INT32 nReadBytes = RegFile.LoadRegistrySet( "config/LocaleValue.pef", REGISTRY_FILE_BINARY);
		if( nReadBytes == STREAM_ERR)
		{
			I3PRINTLOG(I3LOG_FATAL,  "Failed to load LocaleValue.pef" );		//	코드 막지마세요. 여기서 파일 로드 실패하면 다음 게임 초기화에서 실패합니다. komet
			return false;
		}

		i3RegKey * pRootRegKey = RegFile.getRoot();

		if ( ! pRootRegKey )
		{
			I3PRINTLOG(I3LOG_FATAL, "Root Registry Invalided [ LocaleValue.pef ]");
			return false;
		}

		I3_MUST_ADDREF(pRootRegKey);
		s_RegKeyList.push_back(pRootRegKey);
	}

	i3RegKey * regKey = (i3RegKey *)i3TreeNode::FindNodeByName( s_RegKeyList[0], "LocaleValue" );

	_SpreadRegistry( regKey, "");

	{
		//각 국가별 font를 읽어들인다
		i3RegistrySet RegFile;
		i3Registry::Set_LocationIndex(0, MaxCount);

		RegFile.LoadRegistrySet("config/LocaleValue.pef", REGISTRY_FILE_BINARY);
		regKey = (i3RegKey *)i3TreeNode::FindNodeByName(RegFile.getRoot(), "LocaleValue");

		i3RegArray * pData;
		i3::string FieldName;

		INT32 DataCount = regKey->GetDataCount();
		for (INT32 k = 0; k < DataCount; ++k)
		{
			pData = (i3RegArray*)regKey->GetData(k);
			FieldName = pData->GetNameString();
		
			if (FieldName == "_Font" || FieldName == "_FontExtendSize" || FieldName == "_FontFile")
			{
				std::vector<i3RegData* > vecData = *pData->getValue();
				i3::rc_wstring str;
				i3::string temp;
				INT32 value;
				for (int j = 0; j < (int)i3UI::getCountryList()->size(); j++)
				{
					if (FieldName == "_Font")
					{
						GET_REG_DATA(vecData[j], str);
						i3::utf16_to_mb(str, temp);
						i3::generic_string_copy((*i3UI::getCountryList())[j]._fontName, temp);
					}
					else if (FieldName == "_FontExtendSize")
					{
						GET_REG_DATA(vecData[j], &value);
						(*i3UI::getCountryList())[j]._extendSize = value;
					}
					else if (FieldName == "_FontFile")
					{
						GET_REG_DATA(vecData[j], str);
						i3::utf16_to_mb(str, temp);
						i3::generic_string_copy((*i3UI::getCountryList())[j]._fontFileName, temp);
					}
				}	//for Countrylist
			} //FieldName check
		}	//for DataCount
	}

	i3Registry::Set_LocationIndex(Index, MaxCount);

	return true;
}


void	 LocaleValue::Release()
{
	i3::cu::for_each_safe_release_clear(s_RegKeyList, &i3RegKey::Release);
}


bool LocaleValue::Enable(const i3::string & ValueID)
{
	return LocaleValue::GetBOOL( ValueID );
}

bool LocaleValue::GetBOOL(const i3::string & ValueID)
{
	INT32 Ret = LocaleValue::GetInt32( ValueID );

	return !(Ret == 0);
}

i3::rc_wstring LocaleValue::GetString(const i3::string & ValueID)
{
	i3RegData* pData = _GetLocaleContextIterator( ValueID );

	if( !pData )
	{
		I3PRINTLOG(I3LOG_WARN, "속성 값이 없습니다. [%s] - LocaleValue.pef 파일을 참조", ValueID);
	}

	i3::rc_wstring wstr;
	GET_REG_DATA( pData, wstr );
	return wstr;
}

INT32 LocaleValue::GetInt32(const i3::string & ValueID)
{
	i3RegData* pData = _GetLocaleContextIterator( ValueID );

	if( !pData )
	{
		I3PRINTLOG(I3LOG_WARN, "속성 값이 없습니다. [%s] - LocaleValue.pef 파일을 참조", ValueID );
	}

	INT32 Ret = 0;
	GET_REG_DATA( pData, &Ret );
	return Ret;
}

REAL32 LocaleValue::GetReal(const i3::string & ValueID)
{
	i3RegData* pData = _GetLocaleContextIterator( ValueID );

	if( !pData )
	{
		I3PRINTLOG(I3LOG_WARN, "속성 값이 없습니다. [%s] - LocaleValue.pef 파일을 참조", ValueID );
	}

	REAL32 Ret = 0.f;
	GET_REG_DATA( pData, &Ret );
	return Ret;
}

VEC2D LocaleValue::GetVec2d(const i3::string & ValueID)
{
	i3RegData* pData = _GetLocaleContextIterator( ValueID );

	if( !pData )
	{
		I3PRINTLOG(I3LOG_WARN, "속성 값이 없습니다. [%s] - LocaleValue.pef 파일을 참조", ValueID );
	}

	VEC2D Ret;
	memset( &Ret, 0, sizeof( VEC2D ) );
	GET_REG_DATA( pData, &Ret );
	return Ret;
}

VEC3D LocaleValue::GetVec3d(const i3::string & ValueID)
{
	i3RegData* pData = _GetLocaleContextIterator( ValueID );

	if( !pData )
	{
		I3PRINTLOG(I3LOG_WARN, "속성 값이 없습니다. [%s] - LocaleValue.pef 파일을 참조", ValueID );
	}

	VEC3D Ret;
	memset( &Ret, 0, sizeof( VEC3D ) );
	GET_REG_DATA( pData, &Ret );
	return Ret;
}

VEC4D LocaleValue::GetVec4d(const i3::string & ValueID)
{
	i3RegData* pData = _GetLocaleContextIterator( ValueID );

	if( !pData )
	{
		I3PRINTLOG(I3LOG_WARN, "속성 값이 없습니다. [%s] - LocaleValue.pef 파일을 참조", ValueID );
	}

	VEC4D Ret;
	memset( &Ret, 0, sizeof( VEC4D ) );
	GET_REG_DATA( pData, &Ret );
	return Ret;
}

I3COLOR LocaleValue::GetColor(const i3::string & ValueID)
{
	i3RegData* pData = _GetLocaleContextIterator( ValueID );

	if( !pData )
	{
		I3PRINTLOG(I3LOG_WARN, "속성 값이 없습니다. [%s] - LocaleValue.pef 파일을 참조", ValueID );
	}

	I3COLOR Ret;
	memset( &Ret, 0, sizeof( I3COLOR ) );
	GET_REG_DATA( pData, &Ret );
	return Ret;
}

MATRIX LocaleValue::GetMatrix(const i3::string & ValueID)
{
	i3RegData* pData = _GetLocaleContextIterator( ValueID );

	if( !pData )
	{
		I3PRINTLOG(I3LOG_WARN, "속성 값이 없습니다. [%s] - LocaleValue.pef 파일을 참조", ValueID );
	}

	MATRIX Ret;
	memset( &Ret, 0, sizeof( MATRIX ) );
	GET_REG_DATA( pData, &Ret );
	return Ret;
}

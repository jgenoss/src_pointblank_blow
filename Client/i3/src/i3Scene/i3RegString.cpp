#include "i3SceneDef.h"
#include "i3RegString.h"
#include "i3Base/string./compare/generic_is_nequal.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"
#include "i3Base/string/ext/generic_string_size.h"
#include "i3Base/string/ext/generic_string_replace_all.h"
#include "i3Base/string/ext/mb_to_utf16.h"


I3_CLASS_INSTANCE( i3RegString);

i3RegString::i3RegString(void)
{
	m_nDataType = I3REG_TYPE_STRING;
}

i3RegString::i3RegString( const i3::rc_wstring& strString)
{
	m_nDataType = I3REG_TYPE_STRING;
	Set( strString);
}
	
i3RegString::i3RegString( const i3RegString &	OtherString)
{
	m_nDataType = I3REG_TYPE_STRING;
	Set( OtherString.m_wstrString);
}

i3RegString::~i3RegString(void)
{
}

void i3RegString::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3RegData::CopyTo( pDest, method);

	I3ASSERT( i3::kind_of<i3RegString*>(pDest)); 

	i3RegString * pString = ( i3RegString*)pDest;

	pString->Set( m_wstrString);			// 클론작업이 많다면, 이것이 최적화에 도움이 될것이다..
}

void i3RegString::Set( const i3::rc_wstring& strString)
{
	m_nDataType = I3REG_TYPE_STRING;
	m_wstrString = strString;
};

void	i3RegString::OnBuildObjectList( i3::vector<i3PersistantElement*>& List)
{
	i3RegData::OnBuildObjectList( List);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 1)
#endif

namespace pack
{
	struct PACKED STRING
	{
		UINT8	m_ID[4] = { 'R', 'G', 'S', '3' };
		UINT32	m_nStrLength = 0;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

//
//  파일쪽은 기존버전과 호환성이 맞아야하므로 신경을 잘 써야한다..
//  RGS2옵션 분기가 있는데, RGS2가 신버전, 아니면 구버전인듯 하며,
//  구버전의 경우 ReplaceString함수를 거치는데, 로딩시에는 다소 시간제약을 받는다..
//

//
//  RGS3를 도입한다..  (RGS3의 경우 wchar_t(UTF-16) 혹은 UTF-8 둘 중 하나를 고민할수도 있는데..
//                     일단은 곧바로 wchar_t로 저장하도록 변경)
//

UINT32	i3RegString::OnSave( i3ResourceFile * pResFile)
{
	pack::STRING data;
	UINT32	Result, Rc = 0;
	i3Stream * pStream = pResFile->GetStream();

	Result = i3RegData::OnSave( pResFile);
	I3_CHKIO( Result);
	
	data.m_nStrLength = m_wstrString.size();		// 

	Rc = pStream->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_nStrLength > 0)
	{
		Rc = pStream->Write( m_wstrString.data(), data.m_nStrLength * sizeof(wchar_t) );		
		I3_CHKIO( Rc);														
		Result += Rc;
	}

	return Result;
}


UINT32	i3RegString::OnLoad( i3ResourceFile * pResFile)
{
	pack::STRING data;
	UINT32	Result, Rc = 0;
	i3Stream * pStream	= pResFile->GetStream();

	Result = i3RegData::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_nStrLength > 0)
	{

		// i3::rc_string은 공유문자열이기 때문에 바로 쓰여선 안됨..
		// 새 i3::rc_string을 먼저 만들고, 캐스팅해서 접근하도록 함..
		if (i3::generic_is_nequal((const char*)data.m_ID, "RGS3", 4) )
		{
			i3::rc_wstring str_buff( data.m_nStrLength, 0);

			Rc = pStream->Read(const_cast<wchar_t*>(&str_buff[0]), data.m_nStrLength * sizeof(wchar_t) );
			I3_CHKIO( Rc);
			Result += Rc;
			m_wstrString = str_buff;
		}
		else
		if (i3::generic_is_nequal((const char*)data.m_ID, "RGS2", 4) )
		{			
			i3::stack_string str_buff( data.m_nStrLength, 0);
			
			Rc = pStream->Read(const_cast<char*>(&str_buff[0]), data.m_nStrLength);

			I3_CHKIO( Rc);
			Result += Rc;
			i3::mb_to_utf16(str_buff, m_wstrString);
		}
		else
		{
			// 이 경우엔 i3::stack_string으로 먼저 만들고 replace할때 i3::rc_string으로 넘긴다..
			
			i3::stack_string str_buff(data.m_nStrLength, 0);
					
			I3ASSERT( data.m_nStrLength + 1 < MAX_PATH * 4);

			Rc = pStream->Read( &str_buff[0], data.m_nStrLength);
			I3_CHKIO( Rc);
			Result += Rc;
			
			i3::generic_string_replace_all(str_buff, "\\n", "\n");

			i3::mb_to_utf16(str_buff, m_wstrString);

		}
	}

	return Result;
}

void i3RegString::GetValueByString( i3::wstring& out )
{
	out = m_wstrString;
}															

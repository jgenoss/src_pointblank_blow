#include "i3CommonType.h"
#include "i3Action.h"
#include "i3String.h"
#include "i3Base/string/ext/string_ncopy_nullpad.h"

I3_CLASS_INSTANCE( i3Action);

void	i3Action::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	
	i3ClassMeta * pMeta = meta();

	if( pMeta->class_name().size() > 0)
	{
		i3::string_ncopy_nullpad( pszStr, pMeta->class_name().c_str(), len);
	}
	else
	{
		pszStr = "<No Info.>";			// 이렇게 처리하는건 아무것도 안하는 것과 같다..(2012.07.31.수빈)
	}

}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

namespace pack
{
	struct ACTION
	{
		UINT32		m_Style = 0;
		UINT8		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif



UINT32 i3Action::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::ACTION data;

	Result = i3NamedElement::OnSave( pResFile);
	I3_CHKIO( Result);

	memset( &data, 0, sizeof(data));

	data.m_Style = getStyle();
	
	Rc = pStream->Write( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3Action::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	pack::ACTION data;

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	setStyle( data.m_Style);

	return Result;
}

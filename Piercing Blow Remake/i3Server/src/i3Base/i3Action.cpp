#include "i3CommonType.h"
#include "i3Action.h"
#include "i3String.h"

I3_CLASS_INSTANCE( i3Action, i3NamedElement);

i3Action::i3Action(void)
{
	m_Style = 0;
	m_State = 0;
}

i3Action::~i3Action(void)
{
}

void i3Action::OnUpdate( REAL32 tm)
{
}

void i3Action::OnEvent( i3EventReceiver * pObj, REAL32 tm, I3_EVT_CODE code)
{
}

void i3Action::OnChangeTime( REAL32 tm)
{
}

void	i3Action::GetInfoString( char * pszStr, bool bShort, INT32 len)
{
	i3ClassMeta * pMeta = GetMeta();

	if( pMeta->GetClassName() != NULL)
	{
		i3String::NCopy( pszStr, pMeta->GetClassName(), len);
	}
	else
	{
		pszStr = "<No Info.>";
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 8)
#endif

typedef struct _tagi3PersistAction
{
	UINT32		m_Style;
	UINT8		pad[32];
} I3_PERSIST_ACTION ALIGN8;

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif



UINT32 i3Action::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result;
	i3Stream * pStream = pResFile->GetStream();
	I3_PERSIST_ACTION data;

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
	I3_PERSIST_ACTION data;

	Result = i3NamedElement::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof(data));
	I3_CHKIO( Rc);
	Result += Rc;

	setStyle( data.m_Style);

	return Result;
}

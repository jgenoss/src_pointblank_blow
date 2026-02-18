#include "i3FrameworkPCH.h"
#include "i3UIStatic.h"
#include "i3UITemplate_Static.h"

// ============================================================================
//
// i3UIStatic : 이미지 혹은 글자
//
// ============================================================================
I3_CLASS_INSTANCE( i3UIStatic, i3UIControl );

i3UIStatic::i3UIStatic()
{
}

i3UIStatic::~i3UIStatic()
{
}

void i3UIStatic::OnEnabled( bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	setShapeVisible( I3UI_STATIC_SHAPE_NORMAL, bEnable == TRUE);

	if( GetShapeEnable( I3UI_STATIC_SHAPE_DISABLED))
		setShapeVisible( I3UI_STATIC_SHAPE_DISABLED, bEnable == FALSE);
}

#if defined( I3_COMPILER_VC )
#pragma pack(push, 4)
#endif

typedef struct ALIGN4 _tag_i3Persist_GuiStatic
{
	UINT8	m_ID[4];
	UINT32	pad[4];
}I3_PERSIST_UISTATIC;

#if defined( I3_COMPILER_VC )
#pragma pack(pop)
#endif

UINT32 i3UIStatic::OnSave( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	I3_PERSIST_UISTATIC data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);

	i3mem::FillZero( &data, sizeof(data));
	i3mem::Copy( data.m_ID, "UST1", 4);

	Rc = pResFile->Write( &data, sizeof( data ));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIStatic::OnLoad( i3ResourceFile * pResFile )
{
	UINT32	Rc, Result = 0;
	I3_PERSIST_UISTATIC data;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

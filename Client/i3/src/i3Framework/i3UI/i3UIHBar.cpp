#include "i3FrameworkPCH.h"
#include "i3UIHBar.h"
#include "i3UITemplate_HBar.h"

//I3_CLASS_INSTANCE( i3UIHBar, i3UIControl);
I3_CLASS_INSTANCE( i3UIHBar);

i3UIHBar::i3UIHBar()
{
	m_bIgnoreInput = true;
}


void i3UIHBar::_UpdateShapeState( void)
{
	for( INT32 i=0; i< I3UI_HBAR_SHAPE_COUNT; i++)
		setShapeVisible( i, true);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UIHBAR
	{
		UINT8		m_ID[4] = { 'U', 'P', 'B', '1' };
		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UIHBar::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIHBAR data;

	Result = i3UIControl::OnSave( pResFile);
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIHBar::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIHBAR data;

	Result = i3UIControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	
	Result += Rc;

	return Result;
}

bool i3UIHBar::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UIControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	return bResult;
}

bool i3UIHBar::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UIControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	return bResult;
}

#include "i3FrameworkPCH.h"
#include "i3UIStaticText.h"
#include "i3UIManager.h"	
#include "i3UITemplate_StaticText.h"
#include "../i3Framework.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

I3_CLASS_INSTANCE( i3UIStaticText);

i3UIStaticText::i3UIStaticText()
{
	m_bIgnoreInput = true;
}

void i3UIStaticText::_UpdateShapeState(void)
{
	i3UICaptionControl::_UpdateShapeState();

	setShapeVisible( I3UI_STATICTEXT_SHAPE_IMAGE, true);
}

void i3UIStaticText::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	OnUpdateTextPos();
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UISTATICTEXT
	{
		UINT8		m_ID[4] = { 'U', 'I', 'B', '1' };
		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UIStaticText::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UISTATICTEXT data;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIStaticText::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32 Rc, Result = 0;
	pack::UISTATICTEXT data;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	
	Result += Rc;

	return Result;
}

bool i3UIStaticText::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	return bResult;
}

bool i3UIStaticText::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	return bResult;
}


void i3UIStaticText::OnLButtonDown( UINT32 nFlag, POINT point)
{
	if( isActivated())
	{
		i3UICaptionControl::OnLButtonDown( nFlag, point);

		// Lua Event가 잡히지 않아 ParenctNotify -> Notify로 수정 (2011.09.15 양승천)
		//ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);	//I3UI_EVT_CHANGED ? 
		this->Notify(I3UI_EVT_BTN_PUSHED, 0, 0);	//I3UI_EVT_CHANGED ? 

		addState( I3UI_STATE_NEED_UPDATE);
	}
}

#include "i3FrameworkPCH.h"
#include "i3UIImageBox.h"
#include "i3UIManager.h"	
#include "i3UITemplate_ImageBox.h"
#include "../i3Framework.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

//I3_CLASS_INSTANCE( i3UIImageBox, i3UICaptionControl);
I3_CLASS_INSTANCE( i3UIImageBox);

i3UIImageBox::i3UIImageBox()
{
	m_bIgnoreInput = true;
}

void i3UIImageBox::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);
}

void i3UIImageBox::_UpdateShapeState(void)
{
	i3UICaptionControl::_UpdateShapeState();

	setShapeVisible( I3UI_IMAGEBOX_SHAPE_IMAGE, true);
}

void i3UIImageBox::OnLButtonDown( UINT32 nFlag, POINT point)
{
	//i3UICaptionControl::OnLButtonDown( nFlag, point);

	//if( isControlSelected())
	//	removeControlState( I3UI_CTRL_STATE_SELECTED);
	//else
	//	addControlState( I3UI_CTRL_STATE_SELECTED);

	if ( this->isStyle(I3UI_STYLE_SLAVE) )
	{
		i3UIControl* pParentCtrl = static_cast<i3UIControl*>(getParent());

		if (pParentCtrl)
		{
			i3UIManager * pUI = i3UI::getManager();
			pUI->SetFocusControl(pParentCtrl);
			pParentCtrl->OnLButtonDown(nFlag, point);
			return;
		}
	}

	i3UICaptionControl::OnLButtonDown( nFlag, point);

	//_UpdateShape();
	//i3Framework::SetCapture( this);
	// ParentNotify( I3UI_EVT_BTN_PUSHED, point.x, point.y);
}

void	i3UIImageBox::OnLButtonUp( UINT32 nFlag, POINT point)
{

	if ( this->isStyle(I3UI_STYLE_SLAVE) )
	{
		i3UIControl* pParentCtrl = static_cast<i3UIControl*>(getParent());

		if (pParentCtrl)
		{
			i3UIManager * pUI = i3UI::getManager();
			pUI->SetFocusControl(pParentCtrl);
			pParentCtrl->OnLButtonUp(nFlag, point);
			return;
		}
	}

	i3UICaptionControl::OnLButtonUp( nFlag, point);

}

void i3UIImageBox::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	//_UpdateShape();
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UIIMAGEBOX
	{
		UINT8		m_ID[4] = { 'U', 'I', 'B', '1' };
		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UIImageBox::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIIMAGEBOX data;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIImageBox::OnLoad( i3ResourceFile * pResFile)
{
i3_prof_func();

	UINT32 Rc, Result = 0;
	pack::UIIMAGEBOX data;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	
	Result += Rc;

	return Result;
}

bool i3UIImageBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	return bResult;
}

bool i3UIImageBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	return bResult;
}
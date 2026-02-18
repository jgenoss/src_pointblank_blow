#include "i3FrameworkPCH.h"
#include "i3UICheckBox.h"
#include "i3UIManager.h"	
#include "i3UITemplate_CheckBox.h"
#include "../i3Framework.h"
#include "i3Base/string/compare/generic_is_iequal.h"

I3_CLASS_INSTANCE( i3UICheckBox);

i3UICheckBox::i3UICheckBox()
{
	// Default Caption
	if( i3UI::getToolDevMode())
		SetCaption( L"Check Box");
}


void	 i3UICheckBox::setCheck(bool bCheck /*= true*/, bool bNotifyEnable /*= false*/ )
{
	if( !isActivated() ) return;
	if ( bCheck == isSelected() ) return;			// 상태가 같다면 아무짓도 하지 않음.

	if ( bCheck == false)		// 체크를 해제하고 자기 자신의 해제만 공지하고 종료합니다.
	{
		this->removeState(I3UI_STATE_SELECTED); 
		this->setModifyState(true); 

		if (bNotifyEnable)
			this->Notify( I3UI_EVT_BTN_PUSHED, 0, 0);

		return;
	}

	if( !isSelected())
	{
		addState( I3UI_STATE_SELECTED);
		this->setModifyState(true); 
		if (bNotifyEnable)
			this->Notify( I3UI_EVT_BTN_PUSHED, 0, 0);
	}
}

void i3UICheckBox::_UpdateShapeState( void)
{
	i3UICaptionControl::_UpdateShapeState();

	I3UI_CHECKBUTTON_SHAPE eShape = I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED;

	bool bDisabled	= !isActivated();
	bool bHold = isHoldImage();
	bool bChecked	= isSelected();					// LClick
	bool bOnMouse	= isState( I3UI_STATE_ONMOUSE);	// Enter

	if( bDisabled)
	{
		if( bChecked)
			eShape = I3UI_CHECKBUTTON_SHAPE_DISABLED_CHECKED;
		else
			eShape = I3UI_CHECKBUTTON_SHAPE_DISABLED_UNCHECKED;
	}
	else if(bHold)
	{
		eShape = (I3UI_CHECKBUTTON_SHAPE)GetHoldImage();
	}
	else
	{
		if( bChecked)
		{
			if( bOnMouse)
				eShape = I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED_ONMOUSE;
			else
				eShape = I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED;
		}
		else
		{
			if( bOnMouse)
				eShape = I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED_ONMOUSE;
			else
				eShape = I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED;
		}
	}

	if(isPushFlag() == true)
		eShape = I3UI_CHECKBUTTON_SHAPE_ENABLED_CHECKED_ONMOUSE;

	for( INT32 i=0; i< I3UI_CHECKBUTTON_SHAPE_COUNT; i++)
	{
		setShapeVisible( i, ((i == eShape) && (isValidShape( i))));
	}


	_UpdateTextPosition();
}

void i3UICheckBox::_UpdateTextPosition( void)
{
	if((m_pTemplate != nullptr) && (m_pRects != nullptr))
	{
		I3ASSERT( i3::kind_of< i3UITemplate_CheckBox*>(m_pTemplate)); 

		i3UITemplate_CheckBox * pCheckBoxTemplate = (i3UITemplate_CheckBox*)m_pTemplate;

		pCheckBoxTemplate->SetFreeSize(m_bFreeSize);
		pCheckBoxTemplate->SetRightText( m_bRightText);
		pCheckBoxTemplate->AdjustLayout( getWidth(), getHeight(), m_pRects, m_nShapeCount);
	}
}

void i3UICheckBox::SetFreeSize(bool bEnable)
{
	m_bFreeSize = bEnable;
}

void i3UICheckBox::OnEnabled( bool bEnable)
{
	i3UICaptionControl::OnEnabled( bEnable);
}

void i3UICheckBox::OnLButtonDown( UINT32 nFlag, POINT point)
{
	if( isActivated() || IsForceEnableInput() )
	{
		i3UICaptionControl::OnLButtonDown( nFlag, point);

		if( isSelected())	removeState( I3UI_STATE_SELECTED);
		else				addState( I3UI_STATE_SELECTED);

		this->Notify(I3UI_EVT_BTN_PUSHED, 0, 0);

		addState( I3UI_STATE_NEED_UPDATE);
	}
}

/*virtual*/ void i3UICheckBox::OnLButtonUp( UINT32 nFlag, POINT point)
{
	if( isActivated() || IsForceEnableInput() )
	{
		i3UICaptionControl::OnLButtonUp( nFlag, point);
	}
}

void i3UICheckBox::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UICheckBox::OnLeaveMouse(void)
{
	i3UICaptionControl::OnLeaveMouse();

	addState( I3UI_STATE_NEED_UPDATE);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UICHECKBOX
	{
		UINT8		m_ID[4] = { 'U', 'C', 'B', '1' };
		UINT32		m_CheckStyle = 0;
		UINT32		pad[31] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UICheckBox::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UICHECKBOX data;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);
	
	if( m_bRightText)
		data.m_CheckStyle = 1;

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UICheckBox::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UICHECKBOX data;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	if( data.m_CheckStyle == 1)
		m_bRightText = true;

	return Result;
}

bool i3UICheckBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->addAttr( "RightText", (INT32) m_bRightText?1:0);

	return bResult;
}

bool i3UICheckBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	INT32 val;
	pXML->getAttr( "RightText", &val);
	if( val == 1)
		m_bRightText = true;

	return bResult;
}

void i3UICheckBox::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of<i3UICheckBox* >(pDest));

	i3UICheckBox* pCheck = (i3UICheckBox*)pDest;
	
	pCheck->m_bRightText = m_bRightText;
}

void i3UICheckBox::Init( void)
{
	addState( I3UI_STATE_NEED_UPDATE);
}

#if defined( I3_DEBUG)
void i3UICheckBox::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);

	AddMFCProperty_BeginGroup( List, "i3UICheckBox");
	AddMFCProperty_Bool( List, &m_bRightText, "RightText");
	AddMFCProperty_Bool( List, &m_bFreeSize, "Free Size", "가로 세로 비율을 임의로 설정할 수 있습니다.");
	AddMFCProperty_EndGroup( List);
}

void i3UICheckBox::OnTDKPropertyChanged( const char* szName)
{
	if(i3::generic_is_iequal( szName, "RightText") )
	{
		addState( I3UI_STATE_NEED_UPDATE);
	}
	else
	{
		i3UICaptionControl::OnTDKPropertyChanged( szName);
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" int uiCheck_getChecked(lua_State *L)
{
	i3UICheckBox * pCtrl = (i3UICheckBox*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		i3Lua::PushBoolean( L, pCtrl->isSelected());
		return 1;
	}

	i3Lua::PushBoolean( L, false);
	return 1;
}

extern "C" int uiCheck_setChecked(lua_State *L)
{
	i3UICheckBox * pCtrl = (i3UICheckBox*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 check = i3Lua::GetIntegerArg( L, 2, 0);
		bool bCheck = (check > 0)? true : false;

		if( bCheck != pCtrl->isSelected())
		{
			if( pCtrl->isSelected())
				pCtrl->removeState( I3UI_STATE_SELECTED);
			else						
				pCtrl->addState( I3UI_STATE_SELECTED);

			pCtrl->setModifyState();
		}
	}

	return 0;
}

LuaFuncReg i3UICheckGlue[] = 
{
	{"uiCheck_getChecked",	uiCheck_getChecked},
	{"uiCheck_setChecked",	uiCheck_setChecked},

	{nullptr, nullptr}
};

void i3UICheckBox::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UICheckGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UICheckGlue[i].name, i3UICheckGlue[i].func);
	}
}


#include "i3FrameworkPCH.h"
#include "i3UIRadio.h"
#include "i3UIManager.h"	
#include "i3UITemplate_CheckBox.h"
#include "../i3Framework.h"
#include "i3Base/string/compare/generic_is_iequal.h"

I3_CLASS_INSTANCE( i3UIRadio);

i3UIRadio::i3UIRadio()
{
	// Default Caption
	if( i3UI::getToolDevMode())
		SetCaption( L"Radio Button");
}

void i3UIRadio::_UpdateShapeState( void)
{
	i3UICaptionControl::_UpdateShapeState();

	I3UI_CHECKBUTTON_SHAPE eShape = I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED;

	bool bDisabled = !isActivated();
	bool bChecked = isSelected();	// LClick
	bool bOnMouse = isState( I3UI_STATE_ONMOUSE);	// Enter
	
	if( bDisabled)
	{
		if( bChecked)
			eShape = I3UI_CHECKBUTTON_SHAPE_DISABLED_CHECKED;
		else
			eShape = I3UI_CHECKBUTTON_SHAPE_DISABLED_UNCHECKED;
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

	//I3ASSERT( getTemplateShapeEnable( eShape));
	if( !isValidShape( eShape))
	{
		eShape = I3UI_CHECKBUTTON_SHAPE_ENABLED_UNCHECKED;
	}

	for( INT32 i=0; i< I3UI_CHECKBUTTON_SHAPE_COUNT; i++)
		setShapeVisible( i, i == eShape);

	_UpdateTextPosition();
}

void i3UIRadio::_UpdateTextPosition( void)
{
	//m_bRightText
	if((m_pTemplate != nullptr) && (m_pRects != nullptr))
	{
		I3ASSERT( i3::kind_of<i3UITemplate_Radio*>(m_pTemplate));

		i3UITemplate_Radio * pRadioTemplate = (i3UITemplate_Radio*)m_pTemplate;

		pRadioTemplate->SetRightText( m_bRightText);
		pRadioTemplate->AdjustLayout( getWidth(), getHeight(), m_pRects, m_nShapeCount);
	}
}

void i3UIRadio::OnLButtonDown( UINT32 nFlag, POINT point)
{
	if(isActivated() == false)
		return;

	i3UICaptionControl::OnLButtonDown( nFlag, point);

	//
	i3GameNode * pParent = getParent();
	while( pParent != nullptr && !i3::same_of<i3UIFrameWnd*>(pParent))
	{
		pParent = pParent->getParent();		//부모 FrameWnd를 찾는다.
	}

	if( pParent != nullptr)		// FrameWnd를 Parent로 가지고 있다면
	{
		I3ASSERT( i3::same_of< i3UIFrameWnd* >(pParent));
		i3::vector<i3UIControl*> childList;
		i3UIFrameWnd* pFrameWnd = (i3UIFrameWnd*)pParent;
		pFrameWnd->GetChildControls( childList, true);

		for( size_t i=0; i< childList.size(); i++)
		{
			i3UIControl* pChild = childList[i];
			if( i3::same_of<i3UIRadio*>(pChild))
			{
				i3UIRadio* pRadio = (i3UIRadio*)pChild;
				if( pRadio->getGroupID() == m_nGroupIdx)
				{
					// 설정된 것이 해제되었을때의 notify가 있으면 유용할 것이다. (11.05.31 수빈)
					if (pRadio->isSelected() )
					{
						pRadio->removeState( I3UI_STATE_SELECTED);
						pRadio->setModifyState( true);
						I3TRACE("%s\n", pChild->GetName());
						pRadio->Notify( I3UI_EVT_RADIO_UNCHECKED, 0, 0 );
					}
				}
			}
		}
	}

	//
	if( !isSelected())
	{
		addState( I3UI_STATE_SELECTED);
		this->Notify( I3UI_EVT_RADIO_CHECKED, 0, 0);
	}

	ParentNotify( I3UI_EVT_BTN_PUSHED, 0, 0);	//I3UI_EVT_CHANGED ? 

	addState( I3UI_STATE_NEED_UPDATE);
}

void	 i3UIRadio::setCheck(bool bCheck /*= true*/, bool bNotifyEnable /*= false*/ )
{
	if ( bCheck == isSelected() ) return;			// 상태가 같다면 아무짓도 하지 않음.
	
	if ( bCheck == false)		// 체크를 해제하고 자기 자신의 해제만 공지하고 종료합니다.
	{
		this->removeState(I3UI_STATE_SELECTED); 
		this->setModifyState(true); 

		if (bNotifyEnable)
			this->Notify( I3UI_EVT_RADIO_UNCHECKED, 0, 0);

		return;
	}
	
	i3GameNode * pParent = getParent();
	while( pParent != nullptr && !i3::same_of<i3UIFrameWnd*>(pParent))
	{
		pParent = pParent->getParent();		//부모 FrameWnd를 찾는다.
	}
	
	if( pParent != nullptr)		// FrameWnd를 Parent로 가지고 있다면
	{
		I3ASSERT( i3::same_of<i3UIFrameWnd*>(pParent));
		i3::vector<i3UIControl*> childList;
		i3UIFrameWnd* pFrameWnd = (i3UIFrameWnd*)pParent;
		pFrameWnd->GetChildControls( childList, true);

		for( size_t i=0; i< childList.size(); i++)
		{
			i3UIControl* pChild = childList[i];
			if( i3::same_of<i3UIRadio*>(pChild))
			{
				i3UIRadio* pRadio = (i3UIRadio*)pChild;
				if( pRadio->getGroupID() == m_nGroupIdx)
				{
					// 설정된 것이 해제되었을때의 notify가 있으면 유용할 것이다. (11.05.31 수빈)
					if (pRadio->isSelected() )
					{
						pRadio->removeState( I3UI_STATE_SELECTED);
						pRadio->setModifyState( true);
						I3TRACE("%s\n", pChild->GetName());
						if (bNotifyEnable)
							pRadio->Notify( I3UI_EVT_RADIO_UNCHECKED, 0, 0 );
					}
				}
			}
		}
	}

	if( !isSelected())
	{
		addState( I3UI_STATE_SELECTED);
		this->setModifyState(true); 
		if (bNotifyEnable)
			this->Notify( I3UI_EVT_RADIO_CHECKED, 0, 0);
	}
}


void i3UIRadio::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIRadio::OnLeaveMouse(void)
{
	i3UICaptionControl::OnLeaveMouse();

	addState( I3UI_STATE_NEED_UPDATE);
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UIRADIO
	{
		UINT8		m_ID[4] = { 'U', 'R', 'B', '1' };
		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UIRadio::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIRADIO data;

	Result = i3UICaptionControl::OnSave( pResFile);
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIRadio::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIRADIO data;

	Result = i3UICaptionControl::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	
	Result += Rc;

	return Result;
}

bool i3UIRadio::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->addAttr( "GroupID", m_nGroupIdx);

	return bResult;
}

bool i3UIRadio::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->getAttr( "GroupID", &m_nGroupIdx);

	return bResult;
}

void i3UIRadio::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of<i3UIRadio*>(pDest));

	i3UIRadio* pRadio = (i3UIRadio*)pDest;

	pRadio->setGroupID( m_nGroupIdx);
	pRadio->m_bRightText = m_bRightText;
}

void i3UIRadio::Init( void)
{
	addState( I3UI_STATE_NEED_UPDATE);
}

#if defined( I3_DEBUG)
void i3UIRadio::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);

	AddMFCProperty_BeginGroup( List, "i3UIRadio");
	AddMFCProperty_Int32( List, &m_nGroupIdx, "Group Id");
	AddMFCProperty_Bool( List, &m_bRightText, "Right Text");
	AddMFCProperty_EndGroup( List);
}

void i3UIRadio::OnTDKPropertyChanged( const char* szName)
{
	if( i3::generic_is_iequal( szName, "Right Text" ) )
	{
		addState( I3UI_STATE_NEED_UPDATE);
	}
	else
	{
		i3UICaptionControl::OnTDKPropertyChanged( szName);
	}
}
#endif	// I3_DEBUG

extern "C"
{

	int i3UIRadio_setCheck( LuaState * L)
	{
		i3UIRadio* pRadio = (i3UIRadio*) i3Lua::GetUserDataArg( L, 1);
		if( pRadio != nullptr)
		{
			bool bChecked = i3Lua::GetBooleanArg( L, 2, true);	// 인자가 없으면 체크하는 것으로 결정..
			bool bNotify = i3Lua::GetBooleanArg(L, 3, false);		// 인자가 없으면 이벤트공지기능은 끈다..
			pRadio->setCheck(bChecked, bNotify);
		}
		return 0;
	}

	int i3UIRadio_isChecked(  LuaState * L)
	{
		i3UIRadio* pRadio = (i3UIRadio*) i3Lua::GetUserDataArg( L, 1);
		if (pRadio != nullptr)
		{
			i3Lua::PushBoolean(L, pRadio->isChecked());
		}
		return 1;		
	}

}

LuaFuncReg i3UIRadio_Glue[] = 
{
	{"uiRadio_setCheck",		i3UIRadio_setCheck		},
	{"uiRadio_isChecked",		i3UIRadio_isChecked		},
	{nullptr, nullptr}
};

void i3UIRadio::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIRadio_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIRadio_Glue[i].name, i3UIRadio_Glue[i].func);
	}
}


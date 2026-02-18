#include "i3FrameworkPCH.h"
#include "i3UIFrameWnd.h"
#include "i3UIManager.h"
#include "i3UITemplate_MessageBox.h"
#include "i3UIMessageBox.h"
#include "../i3Framework.h"

// ============================================================================
//
// i3UIButton : 버튼
//
// ============================================================================
I3_CLASS_INSTANCE( i3UIMessageBox);

i3UIMessageBox::~i3UIMessageBox()
{
	INT32 i;
	for( i = 0;i < I3UI_MBTYPE_COUNT;++i)
	{
		if( m_pBtns[i] != nullptr)
		{
			RemoveChild( m_pBtns[i]);

			I3_SAFE_RELEASE( m_pBtns[i]);
		}
	}

	if( m_pMessageText != nullptr)
	{
		RemoveChild( m_pMessageText);

		I3_SAFE_RELEASE( m_pMessageText);
	}
}

void i3UIMessageBox::Clear( void)
{
	for( INT32 i=0; i< I3UI_MBTYPE_COUNT; i++)
	{
		if( m_pBtns[i] != nullptr)
		{
			m_pBtns[i]->EnableCtrl( false);
		}
	}

	if( m_pMessageText != nullptr)
	{
		m_pMessageText->EnableCtrl( false);
	}

	I3TRACE( getText());
}

void i3UIMessageBox::OnEnabled( bool bEnable)
{
	i3UIFrameWnd::OnEnabled( bEnable);

	INT32 i;
	for( i = 0;i < I3UI_MBTYPE_COUNT;++i)
	{
		if( m_pBtns[i] != nullptr)
		{
			m_pBtns[i]->EnableCtrl( bEnable);
		}
	}

	if( m_pMessageText != nullptr)
	{
		m_pMessageText->EnableCtrl( bEnable);
	}

	I3TRACE( getText());
}

void i3UIMessageBox::OnBindTemplate(void)
{
	i3UIFrameWnd::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	{
		i3UITemplate * pTemplate = m_pTemplate->getSubTemplate(I3UI_MSGBOX_SUBTEMPLATE_TEXT);
		I3ASSERT( pTemplate != nullptr);

		if( m_pMessageText == nullptr)
		{
			m_pMessageText = i3UIStaticText::new_object();
			m_pMessageText->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
			m_pMessageText->addStyle( I3UI_STYLE_SLAVE);
			AddControl( m_pMessageText);	
		}

		m_pMessageText->SetTextAlign( I3UI_TEXT_ALIGN_X_CENTER, I3UI_TEXT_ALIGN_Y_MIDDLE);
		m_pMessageText->SetTemplate( pTemplate);
	}

	for( INT32 i = 0;i < I3UI_MBTYPE_COUNT; ++i)
	{
		if( m_pBtns[i] == nullptr)
		{
			m_pBtns[i] = i3UIButton::new_object();
			AddControl( m_pBtns[i]);
			m_pBtns[i]->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
			m_pBtns[i]->addStyle( I3UI_STYLE_SLAVE);	
		}

		m_pBtns[i]->SetTextAlign( I3UI_TEXT_ALIGN_X_CENTER, I3UI_TEXT_ALIGN_Y_MIDDLE);
		m_pBtns[i]->SetTemplate( m_pTemplate->getSubTemplate( I3UI_MSGBOX_SUBTEMPLATE_BUTTON));
	}
}

void i3UIMessageBox::_UpdateShapeState(void)
{
	/*_CalcBoxSize();
	_CalcButtonPosition();*/

	/*{
		m_pBtns[I3UI_MBTYPE_YESNOCANCEL]->EnableCtrl( m_Type >= I3UI_MBTYPE_YESNOCANCEL);
		m_pBtns[I3UI_MBTYPE_YESNO]->EnableCtrl( m_Type >= I3UI_MBTYPE_YESNO);
		m_pBtns[I3UI_MBTYPE_OK]->EnableCtrl( m_Type >= I3UI_MBTYPE_OK);
	}*/

	i3UIFrameWnd::_UpdateShapeState();
}

void i3UIMessageBox::OnSize( REAL32 cx, REAL32 cy)
{
	i3UIFrameWnd::OnSize( cx, cy);

	addState( I3UI_STATE_NEED_UPDATE);
}

//void i3UIMessageBox::setMessagePos( REAL32 x, REAL32 y)
//{
//	I3ASSERT( m_pMessageText != nullptr);
//
//	m_pMessageText->setPos(x, y);
//}

void i3UIMessageBox::SetMsgBoxType( I3UI_MESSAGEBOX_TYPE type)
{
	if( m_Type == type)
	{
		return;
	}

	m_Type = type;

	m_pBtns[I3UI_MBTYPE_YESNOCANCEL]->EnableCtrl( type >= I3UI_MBTYPE_YESNOCANCEL);
	m_pBtns[I3UI_MBTYPE_YESNO]->EnableCtrl( type >= I3UI_MBTYPE_YESNO);
	m_pBtns[I3UI_MBTYPE_OK]->EnableCtrl( type >= I3UI_MBTYPE_OK);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIMessageBox::SetMsgString(const char * pText)
{
	I3ASSERT( m_pMessageText != nullptr);

	m_pMessageText->SetText( pText);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIMessageBox::SetBtnCaption( INT32 idx, const char * pszString)
{
	I3_BOUNDCHK( idx, I3UI_MBTYPE_COUNT);

	I3ASSERT( m_pBtns[idx] != nullptr);

	m_pBtns[idx]->SetText( pszString);

	//_CalcButtonSize( idx);

	addState( I3UI_STATE_NEED_UPDATE);
}

//void i3UIMessageBox::_CalcButtonSize( INT32 idx)
//{
//	I3_BOUNDCHK( idx, I3UI_MBTYPE_COUNT);
//
//	I3ASSERT( m_pBtns[idx] != nullptr);
//
//	i3UIText * pText = m_pBtns[idx]->getUIText();
//	if( pText != nullptr)
//	{
//		REAL32 width = max( m_vMINBtnSize.x, (REAL32)(pText->getExtentWidth() + 20.0f));
//		REAL32 height = max( m_vMINBtnSize.y, (REAL32)(pText->getExtentHeight() + 10.0f));
//		m_pBtns[idx]->setSize( width, height);
//	}
//}

void i3UIMessageBox::_CalcBoxSize(void)
{
	i3UIText * pText = m_pMessageText->getUIText();
	if( pText != nullptr)
	{
		REAL32 width = max( m_vMinSize.x, m_pMessageText->getSize()->x + m_leftSpace + m_rightSpace);
		REAL32 height = max( m_vMinSize.y, m_topSpace + m_pMessageText->getSize()->y + m_innerSpace + m_pBtns[0]->getSize()->y + m_bottomSpace);

		// button caption에 의해 size가 더 커져야할 지 여부 판단.
		REAL32 btnwidth = m_pBtns[I3UI_MBTYPE_OK]->getSize()->x;
		if(	m_Type >= I3UI_MBTYPE_YESNO)	btnwidth = btnwidth + m_pBtns[I3UI_MBTYPE_YESNO]->getSize()->x;
		if(	m_Type >= I3UI_MBTYPE_YESNOCANCEL)	btnwidth = btnwidth + m_pBtns[I3UI_MBTYPE_YESNOCANCEL]->getSize()->x;

		width = max( width, btnwidth + 30.0f);

		setSize( width, height);

		i3Framework * pFramework = i3Framework::getCurrentFramework();
		i3Viewer * pViewer = pFramework->GetViewer();
		if( pViewer != nullptr)
		{
			REAL32 x = (REAL32)(pViewer->GetViewWidth() >> 1) - width * 0.5f;
			REAL32 y = (REAL32)(pViewer->GetViewHeight() >> 1) - height * 0.5f;

			setPos( x, y);
		}
	}
}

void i3UIMessageBox::_CalcMessagePosition( void)
{
	REAL32 left = (getSize()->x - m_pMessageText->getSize()->x) / 2.0f;
	REAL32 top = m_topSpace;

	m_pMessageText->setPos( left, top);
}

#define MSGBTN_MAX_SLICE 45.0f

void i3UIMessageBox::_CalcButtonPosition(void)
{
	REAL32 left = 0.0f;
	REAL32 top  = getSize()->y - m_pBtns[I3UI_MBTYPE_OK]->getSize()->y - m_bottomSpace;

	REAL32 btnwidth = m_pBtns[I3UI_MBTYPE_OK]->getSize()->x;
	if(	m_Type >= I3UI_MBTYPE_YESNO)	btnwidth = btnwidth + m_pBtns[I3UI_MBTYPE_YESNO]->getSize()->x;
	if(	m_Type >= I3UI_MBTYPE_YESNOCANCEL)	btnwidth = btnwidth + m_pBtns[I3UI_MBTYPE_YESNOCANCEL]->getSize()->x;

	if( m_Type >= I3UI_MBTYPE_YESNOCANCEL)
	{
		REAL32 slice	= 0.0f;
		REAL32 remain	= m_Size.x - btnwidth;
		REAL32 rate		= btnwidth / m_Size.x;

		slice = min( remain * 0.15f, MSGBTN_MAX_SLICE * rate);
			
		remain = remain - slice * 2.0f;

		left = remain * 0.5f;
		m_pBtns[I3UI_MBTYPE_OK]->setPos( left, top);

		left = left + slice + m_pBtns[I3UI_MBTYPE_OK]->getSize()->x;
		m_pBtns[I3UI_MBTYPE_YESNO]->setPos( left, top);

		left = left + slice + m_pBtns[I3UI_MBTYPE_YESNO]->getSize()->x;
		m_pBtns[I3UI_MBTYPE_YESNOCANCEL]->setPos( left, top);
	}
	else if( m_Type >= I3UI_MBTYPE_YESNO)
	{
		REAL32 slice	= 0.0f;
		REAL32 remain	= 0.0f;

		REAL32 rate		= btnwidth / m_Size.x;

		slice = min( m_Size.x * 0.15f, MSGBTN_MAX_SLICE * rate);

		remain = m_Size.x * (1.0f - rate) - slice;

		left = remain * 0.5f;
		m_pBtns[I3UI_MBTYPE_OK]->setPos( left, top);

		left = left + slice + m_pBtns[I3UI_MBTYPE_OK]->getSize()->x;
		m_pBtns[I3UI_MBTYPE_YESNO]->setPos( left, top);
	}
	else
	{
		left = m_Size.x * 0.5f - m_pBtns[I3UI_MBTYPE_OK]->getSize()->x * 0.5f;
		m_pBtns[I3UI_MBTYPE_OK]->setPos( left, top);
	}
}

void i3UIMessageBox::PreCalcBoxSize( void)
{
	//Component의 size를 미리 예측하고, component들 사이의 margine을 계산하여
	//box의 크기를 결정한다.
	
	_PreCalcComponentsSize();

	_CalcBoxSize();
	
	_CalcMessagePosition();
	_CalcButtonPosition();

	SetText( getText());
}

void i3UIMessageBox::_PreCalcComponentsSize( void)
{
	//Text Extent를 계산하기 위한 용도
	i3UIStaticText* pSt = i3UIStaticText::new_object();

	pSt->setSize( &m_vMaxSize);

	//잘못된 Text Extent를 얻지 않기 위해서는 Control의 size가 충분히 커야한다.
	pSt->SetMaxTextCount( 1024);

	i3UITemplate * pTemplate = m_pTemplate->getSubTemplate(I3UI_MSGBOX_SUBTEMPLATE_TEXT);
	I3ASSERT( pTemplate != nullptr);

	pSt->SetTextAlign( I3UI_TEXT_ALIGN_X_LEFT, I3UI_TEXT_ALIGN_Y_TOP);
	pSt->SetTemplate( pTemplate);

	i3::pack::RECT rt;
	rt.left = 0.f;
	rt.top = 0.f;
	rt.right = m_vMaxSize.x;
	rt.bottom = m_vMaxSize.y;
	
	pSt->SetTextRect( &rt);
	
	
	//{
	//	//Caption
	//	const STR16 pwszText = getText();

	//	pSt->SetText( pwszText);
	//	
	//	i3UIText* pUIText = pSt->getUIText();
	//	
	//	INT32 w = pUIText->getExtentWidth();
	//	
	//	setSize( (REAL32)w, m_Size.y);
	//}
	
	{
		//Message
		const WCHAR16 * pwszText = m_pMessageText->getText();

		pSt->SetText( pwszText);
		
		i3UIText* pUIText = pSt->getUIText();
		
		REAL32 w = (REAL32)pUIText->getExtentWidth() + 15.0f;
		REAL32 h = (REAL32)pUIText->getExtentHeight() + 15.0f;

		m_pMessageText->setSize( w, h);
	}

	{
		//Buttons
		for( INT32 i=0; i< I3UI_MBTYPE_COUNT; i++)
		{
			const WCHAR16 * pwszText = m_pBtns[i]->getText();

			pSt->SetText( pwszText);
			
			i3UIText* pUIText = pSt->getUIText();
			
			REAL32 w = max( m_vMINBtnSize.x, (REAL32)(pUIText->getExtentWidth() + 20.0f));
			REAL32 h = max( m_vMINBtnSize.y, (REAL32)(pUIText->getExtentHeight() + 10.0f));

			m_pBtns[i]->setSize( w, h);
		}
	}

	I3_MUST_RELEASE( pSt);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void i3UIMessageBox::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	i3UIFrameWnd::OnNotify( pCtrl, event, param1, param2);

	switch( event)
	{
	case I3UI_EVT_CLICKED:	// Button이 눌려졌다.
		{
			I3UI_MODALRESULT result = (I3UI_MODALRESULT)-1;

			INT32 i;
			for( i = 0; i < I3UI_MBTYPE_COUNT; ++i)
			{
				if( m_pBtns[i] == pCtrl)
				{
					result = (I3UI_MODALRESULT)i;
					break;
				}
			}

			i3UI::EndModal( this, (i3UIControl*)getEventReceiver(), result);
		}
		break;
	default:
		break;
	}
}

#if defined( I3_COMPILER_VC)
#pragma pack( push, 4)
#endif

namespace pack
{
	struct ALIGN4 UIMESSAGEBOX
	{
		UINT8		m_ID[4] = { 'U', 'M', 'B', '1' };
		UINT32		pad[32] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack( pop)
#endif

UINT32 i3UIMessageBox::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIMESSAGEBOX data;

	Result = i3UIFrameWnd::OnSave( pResFile);
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);

	Result += Rc;

	return Result;
}

UINT32 i3UIMessageBox::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Rc, Result = 0;
	pack::UIMESSAGEBOX data;

	Result = i3UIFrameWnd::OnLoad( pResFile);
	I3_CHKIO( Result);

	Rc = pResFile->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	
	Result += Rc;

	return Result;
}

bool i3UIMessageBox::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UIFrameWnd::OnSaveXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->addAttr( "TopSpace", m_topSpace);
	pXML->addAttr( "BottomSpace", m_bottomSpace);
	pXML->addAttr( "LeftSpace", m_leftSpace);
	pXML->addAttr( "RightSpace", m_rightSpace);
	pXML->addAttr( "InnerSpace", m_innerSpace);

	return bResult;
}

bool i3UIMessageBox::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = false;

	bResult = i3UIFrameWnd::OnLoadXML( pFile, pXML);
	if( bResult == false)
		return false;

	pXML->getAttr( "TopSpace", &m_topSpace);
	pXML->getAttr( "BottomSpace", &m_bottomSpace);
	pXML->getAttr( "LeftSpace", &m_leftSpace);
	pXML->getAttr( "RightSpace", &m_rightSpace);
	pXML->getAttr( "InnerSpace", &m_innerSpace);

	return bResult;
}


#if defined( I3_DEBUG)
void i3UIMessageBox::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3UIFrameWnd::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIMessageBox" );
	
	AddMFCProperty_Real32( List, &m_topSpace, "Top Margin");
	AddMFCProperty_Real32( List, &m_bottomSpace, "Bottom Margin");
	AddMFCProperty_Real32( List, &m_leftSpace, "Left Margin");
	AddMFCProperty_Real32( List, &m_rightSpace, "Right Margin");
	AddMFCProperty_Real32( List, &m_innerSpace, "Inner Margin");
	
	AddMFCProperty_EndGroup( List);
}
#endif	// I3_DEBUG

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Glue
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
extern "C" int i3UIMB_SetType( lua_State* L)
{
	i3UIMessageBox* pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg( L, 1);
	if( pMsgBox != nullptr)
	{
		I3UI_MESSAGEBOX_TYPE type = (I3UI_MESSAGEBOX_TYPE)i3Lua::GetIntegerArg( L, 2, -1);

		pMsgBox->SetMsgBoxType( type);
	}

	return 0;
}

//extern "C" int i3UIMB_SetMessagePos(lua_State* L)
//{
//	i3UIMessageBox * pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg(L, 1);
//	if( pMsgBox != nullptr)
//	{
//		REAL32 x = (REAL32)i3Lua::GetNumberArg(L, 2, 0.0f);
//		REAL32 y = (REAL32)i3Lua::GetNumberArg(L, 3, 0.0f);
//
//		pMsgBox->setMessagePos( x, y);
//	}
//
//	return 0;
//}

extern "C" int i3UIMB_SetMessage( lua_State* L)
{
	char szStr[ 260];

	i3UIMessageBox* pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg( L, 1);

	if( pMsgBox != nullptr)
	{
		if( i3Lua::GetStringArg( L, 2, szStr, sizeof( szStr)) != nullptr)
		{
			pMsgBox->SetMsgString( szStr);
			return 1;
		}
	}

	return 0;
}

extern "C" int i3UIMB_SetEventReceiver( lua_State* L)
{
	i3UIMessageBox* pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg( L, 1);
	if( pMsgBox != nullptr)
	{
		i3ElementBase * pEvRecv = (i3ElementBase*)i3Lua::GetUserDataArg( L, 2);
		if( i3::kind_of<i3EventReceiver*>(pEvRecv))
		{
			pMsgBox->setEventReceiver( (i3EventReceiver*)pEvRecv);
		}
	}

	return 0;
}

extern "C" int i3UIMB_Initialize( lua_State* L)
{
	char szStr[ 260];

	i3UIMessageBox* pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg( L, 1);

	if( pMsgBox != nullptr)
	{
		// type
		I3UI_MESSAGEBOX_TYPE type = (I3UI_MESSAGEBOX_TYPE)i3Lua::GetIntegerArg( L, 2, -1);

		pMsgBox->SetMsgBoxType( type);

		// string
		if( i3Lua::GetStringArg( L, 3, szStr, sizeof( szStr)) != nullptr)
		{
			pMsgBox->SetMsgString( szStr);

			// receiver
			i3ElementBase * pEvRecv = (i3ElementBase*)i3Lua::GetUserDataArg( L, 4);

			if( i3::kind_of<i3EventReceiver*>(pEvRecv))
			{
				pMsgBox->setEventReceiver( (i3EventReceiver*)pEvRecv);
			}

			// btn caption
			if( i3Lua::GetStringArg(L, 5, szStr, sizeof( szStr), "Yes") != nullptr)
				pMsgBox->SetBtnCaption( I3UI_MBTYPE_OK, szStr);

			if( i3Lua::GetStringArg(L, 6, szStr, sizeof( szStr), "No") != nullptr)
				pMsgBox->SetBtnCaption( I3UI_MBTYPE_YESNO, szStr);

			if( i3Lua::GetStringArg(L, 7, szStr, sizeof( szStr), "Cancel") != nullptr)
				pMsgBox->SetBtnCaption( I3UI_MBTYPE_YESNOCANCEL, szStr);

			pMsgBox->PreCalcBoxSize();

			return 1;
		}
	}

	return 0;
}

extern "C" int i3UIMB_SetButtonCaption(lua_State* L)
{
	char szText[ 260];

	i3UIMessageBox* pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg( L, 1);

	if( pMsgBox != nullptr)
	{
		if( i3Lua::GetStringArg(L, 2, szText, sizeof( szText), "Yes") != nullptr)
			pMsgBox->SetBtnCaption( I3UI_MBTYPE_OK, szText);

		if( i3Lua::GetStringArg(L, 3, szText, sizeof( szText), "No") != nullptr)
			pMsgBox->SetBtnCaption( I3UI_MBTYPE_YESNO, szText);

		if( i3Lua::GetStringArg(L, 4, szText, sizeof( szText), "Cancel") != nullptr)
			pMsgBox->SetBtnCaption( I3UI_MBTYPE_YESNOCANCEL, szText);
	}

	return 0;
}

extern "C" int i3UIMB_SetButtonBottomSpace(lua_State *L)
{
	i3UIMessageBox* pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg( L, 1);
	if( pMsgBox != nullptr)
	{
		REAL32 bottomSpace = (REAL32)i3Lua::GetNumberArg(L, 2, 0.0f);	

		pMsgBox->setBottomSpace( bottomSpace);
	}

	return 0;
}

//extern "C" int i3UIMB_RestoreInitialState( lua_State* L)
//{
//	i3UIMessageBox* pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg( L, 1);
//	if( pMsgBox != nullptr)
//	{
//		pMsgBox->RestoreInitialState();
//	}
//
//	return 0;
//}

extern "C" int i3UIMB_Clear(lua_State *L)
{
	i3UIMessageBox* pMsgBox = (i3UIMessageBox*)i3Lua::GetUserDataArg( L, 1);
	if( pMsgBox != nullptr)
	{
		pMsgBox->Clear();
	}

	return 0;
}

LuaFuncReg i3UIMessageBox_Glue[] = 
{
	{"uiMB_initialize",			i3UIMB_Initialize		},
	{"uiMB_setType",			i3UIMB_SetType			},
	{"uiMB_setMessage",			i3UIMB_SetMessage		},
	{"uiMB_setEventReceiver",	i3UIMB_SetEventReceiver	},
	{"uiMB_setButtonCaption",	i3UIMB_SetButtonCaption	},
	//{"uiMB_setMessagePos",		i3UIMB_SetMessagePos	},
	{"uiMB_setBottomSpace",		i3UIMB_SetButtonBottomSpace	},

	//{"uiMB_restoreInitialState",	i3UIMB_RestoreInitialState	},

	{	"uiMB_Clear",			i3UIMB_Clear	},
	
	{nullptr, nullptr}
};

void i3UIMessageBox::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIMessageBox_Glue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIMessageBox_Glue[i].name, i3UIMessageBox_Glue[i].func);
	}
}

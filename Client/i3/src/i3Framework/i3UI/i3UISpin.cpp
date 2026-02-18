#include "i3FrameworkPCH.h"
#include "i3UISpin.h"
#include "i3UIManager.h"
#include "../i3Framework.h"

//I3_CLASS_INSTANCE( i3UISpin, i3UIControl );
I3_CLASS_INSTANCE( i3UISpin);

i3UISpin::~i3UISpin()
{
	I3_SAFE_RELEASE( m_pBtnUp);
	I3_SAFE_RELEASE( m_pBtnDown);
}

void i3UISpin::_UpdateToBuddy( void)
{
	if( m_bAutoUpdateBuddy)
	{
		if( i3::same_of<i3UIEditBox*>(m_pBuddyControl))
		{
			i3UIEditBox* pBuddyEdit = (i3UIEditBox*)m_pBuddyControl;

			char szCurPos[32];	// float -> string
			i3::snprintf( szCurPos, sizeof( szCurPos), "%f", m_fCurValue);

			wchar_t wszCurPos[32];
			i3String::MultiByteToWideChar( CP_UTF8, 0, szCurPos, sizeof( szCurPos), wszCurPos, _countof( wszCurPos));
			
			pBuddyEdit->SetText( wszCurPos);
		}
	}
}

void i3UISpin::InitBuddy( void)
{
	m_fCurValue = m_fValueMin;
	m_pBuddyControl = nullptr;

	if( !m_strBuddyName.empty())
	{
		i3UIControl* pBuddy = i3UI::FindControlByName( nullptr, m_strBuddyName.c_str());
		m_pBuddyControl = pBuddy;
	}

	_UpdateToBuddy();
}

void i3UISpin::_UpdateShapeState(void)
{
	i3UIControl::_UpdateShapeState();

	setShapeVisible( I3UI_SPIN_SHAPE_BKGND, true);
}

void	i3UISpin::_UpdateComponents(void)
{
	m_pBtnUp->setPos( 0.0f, 0.0f);
	m_pBtnDown->setPos( 0.0f, m_Size.y - m_pBtnUp->getHeight());
}

void	i3UISpin::setSpinPos( REAL32 pos)
{
	//가정: m_ValueMin, m_ValueMax는 정수이고, m_ValueMin < m_ValueMax. 
	/*I3ASSERT( m_ValueMin < m_ValueMax);

	if( m_bWrap)
	{
		if( pos > m_ValueMax)
		{
			INT32 range = m_ValueMax - m_ValueMin + 1;
			m_CurValue = (m_ValueMin - 1) + ((pos - m_ValueMax) % range);
		}
		else if( pos < m_ValueMin)
		{
			INT32 range = m_ValueMax - m_ValueMin + 1;
			m_CurValue = (m_ValueMax + 1) - ((m_ValueMin - pos) % range);
		}
		else
		{
			m_CurValue = pos;
		}
	}
	else
	{
		m_CurValue = MINMAX( m_ValueMin, pos, m_ValueMax);
	}*/

	//m_fValueMin, m_fValueMax가 REAL32일때
	I3ASSERT( m_fValueMin < m_fValueMax);

	if( m_bAutoWrap)
	{
		REAL32 range = m_fValueMax - m_fValueMin;

		while( pos > m_fValueMax)
		{
			pos -= range;
		}

		while( pos < m_fValueMin)
		{
			pos += range;
		}
		
		m_fCurValue = pos;
	}
	else
	{
		m_fCurValue = MINMAX( m_fValueMin, pos, m_fValueMax);
	}

	I3TRACE( "i3UISpin::setSpinPos: %f\n", m_fCurValue);

	_UpdateToBuddy();
}

void i3UISpin::OnEnabled(bool bEnable)
{
	i3UIControl::OnEnabled( bEnable);

	if( m_pBtnUp != nullptr)	m_pBtnUp->EnableCtrl( bEnable);
	if( m_pBtnDown != nullptr) m_pBtnDown->EnableCtrl( bEnable);
}

//처음에만 생성하고 두번째부터는 SetTemplate만 해준다. 
void i3UISpin::OnBindTemplate(void)
{
	i3UIControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	// Btn Up
	if( m_pBtnUp == nullptr)
	{
		m_pBtnUp = i3UIButton::new_object();
		m_pBtnUp->SetName("Up");
		AddControl( m_pBtnUp);
		m_pBtnUp->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pBtnUp->addStyle( I3UI_STYLE_SLAVE);
	}
	
	// Btn Down
	if( m_pBtnDown == nullptr)
	{
		m_pBtnDown = i3UIButton::new_object();
		m_pBtnDown->SetName("Down");
		AddControl( m_pBtnDown);
		m_pBtnDown->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pBtnDown->addStyle( I3UI_STYLE_SLAVE);
	}

	// Set Template
	m_pBtnUp->SetTemplate( m_pTemplate->getSubTemplate(I3UI_SPIN_BTN_PREV));
	m_pBtnDown->SetTemplate( m_pTemplate->getSubTemplate(I3UI_SPIN_BTN_NEXT));

	// Update
	_UpdateComponents();

}

void i3UISpin::OnMove( REAL32 x, REAL32 y)
{
	i3UIControl::OnMove( x, y);

	_UpdateComponents();
}

void i3UISpin::OnSize( REAL32 cx, REAL32 cy)
{
	i3UIControl::OnSize( cx, cy);

	m_pBtnUp->setSize( cx, cy / 2.0f);
	m_pBtnDown->setSize( cx, cy / 2.0f);

	// See i3UIScrollBar::OnSize
	m_Size.x = cx;
	m_Size.y = cy;

	_UpdateComponents();
}

void i3UISpin::OnNotify( i3UIControl * pCtrl, I3UI_EVT event, INT32 param1, INT32 param2)
{
	switch( event)
	{
	case I3UI_EVT_BTN_PUSHED:
		{
			if( pCtrl == m_pBtnUp)
				_OnBtnUpPushed();
			else if( pCtrl == m_pBtnDown)
				_OnBtnDownPushed();
		}
		break;
	default:
		break;
	}

	i3UIControl::OnNotify( pCtrl, event, param1, param2);
}

void i3UISpin::_OnBtnUpPushed( void)
{
	setSpinPos( getSpinPos() + m_fValueTick);

	Notify( I3UI_EVT_BTN_PUSHED, 0, 0);
}

void i3UISpin::_OnBtnDownPushed( void)
{
	setSpinPos( getSpinPos() - m_fValueTick);

	Notify( I3UI_EVT_BTN_PUSHED, 1, 0);
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UISPIN
	{
		INT8		m_ID[4] = { 'U', 'I', 'S', 'P' };
		UINT32		pad[8] = { 0 };

		REAL32		m_fValueMin = 0.0f;
		REAL32		m_fValueMax = 0.0f;
		REAL32		m_fValueTick = 0.0f;
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif
UINT32 i3UISpin::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UISPIN data;

	Result = i3UIControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	data.m_fValueMin = m_fValueMin;
	data.m_fValueMax = m_fValueMax;
	data.m_fValueTick = m_fValueTick;

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UISpin::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UISPIN data;

	Result = i3UIControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	m_fValueMin = data.m_fValueMin;
	m_fValueMax = data.m_fValueMax;
	m_fValueTick = data.m_fValueTick;

	return Result;
}

bool i3UISpin::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnSaveXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->addAttr( "MinValue", m_fValueMin);
	pXML->addAttr( "MaxValue", m_fValueMax);
	pXML->addAttr( "TickSize", m_fValueTick);
	
	pXML->addAttr( "AutoWrap", m_bAutoWrap == false ? 0 : 1);
	pXML->addAttr( "AutoUpdateBuddy", m_bAutoUpdateBuddy == false ? 0 : 1);
	
	pXML->addAttr( "BuddyName", m_strBuddyName.c_str());

	return bResult;
}

bool i3UISpin::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UIControl::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->getAttr( "MinValue", &m_fValueMin);
	pXML->getAttr( "MaxValue", &m_fValueMax);
	pXML->getAttr( "TickSize", &m_fValueTick);
	
	INT32 nTemp = 0;
	pXML->getAttr( "AutoWrap", &nTemp);
	m_bAutoWrap = (nTemp == 0) ? false : true;

	pXML->getAttr( "AutoUpdateBuddy", &nTemp);
	m_bAutoUpdateBuddy = (nTemp == 0) ? false : true;
	
	const char* pszTemp = pXML->getAttr( "BuddyName");
	if( pszTemp != nullptr)
		m_strBuddyName = pszTemp;

	return bResult;
}

void i3UISpin::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of<i3UISpin*>(pDest));

	i3UISpin* pSpin = (i3UISpin*)pDest;
	
	pSpin->setSpinRange( m_fValueMin, m_fValueMax);
	pSpin->setTickSize( m_fValueTick);
	pSpin->setSpinPos( m_fCurValue);

	pSpin->m_strBuddyName = m_strBuddyName;

	pSpin->m_bAutoWrap = m_bAutoWrap;		//any problem?
	pSpin->m_bAutoUpdateBuddy = m_bAutoUpdateBuddy;
}

#if defined( I3_DEBUG)
void i3UISpin::GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List)
{
	i3UIControl::GetTDKGridProperties( List);

	AddMFCProperty_BeginGroup( List, "i3UISpin", "Spin Control은 버튼이 눌렸을 경우 I3UI_EVT_BTN_PUSHED 이벤트를 보냅니다\
												  Up인경우 첫번째 인자로 0, Down인 경우 첫번째 인자로 1을 넘겨줍니다.");
	AddMFCProperty_Real32( List, &m_fValueMin, "Minimum Value");
	AddMFCProperty_Real32( List, &m_fValueMax, "Maximum Value");
	AddMFCProperty_Real32( List, &m_fValueTick, "Tick Size");
	AddMFCProperty_Bool( List, &m_bAutoWrap, "AutoWrap");
	AddMFCProperty_RcString( List, m_strBuddyName, "Buddy Control");
	AddMFCProperty_Bool( List, &m_bAutoUpdateBuddy, "Auto Udpate Buddy", "Buddy Control이 EditBox인 경우 이 속성을 true로 하면 spin의 position을 자동으로 \
																			Buddy Control에 업데이트 합니다.");
	AddMFCProperty_EndGroup( List);
}

void i3UISpin::OnTDKPropertyChanged( const char* szName)
{
	/*if( i3String::NCompare( szName, "Tick Size", MAX_PATH) == 0)
	{
	}
	else*/
	{
		i3UIControl::OnTDKPropertyChanged( szName);
	}
}
#endif

/////////////////////////////////////////////////////////////////////////////////////
extern "C" int i3UISpin_getCurPos(lua_State *L)
{
	i3UISpin* pSpin = (i3UISpin*)i3Lua::GetUserDataArg( L, 1);
	if( pSpin != nullptr)
	{
		i3Lua::PushNumber( L, pSpin->getSpinPos());
		//i3Lua::PushInteger( L, pSpin->getSpinPos());
	}

	return 1;
}

extern "C" int i3UISpin_init( lua_State* L)
{
	i3UISpin* pSpin = (i3UISpin*)i3Lua::GetUserDataArg( L, 1);
	if( pSpin != nullptr)
	{
		pSpin->InitBuddy();
	}
	return 0;
}

LuaFuncReg i3UISpinGlue[] = 
{
	{"uiSpin_getCurPos",				i3UISpin_getCurPos},
	{"uiSpin_init",						i3UISpin_init},
	
	{nullptr, nullptr}
};

void i3UISpin::RegisterLuaFunction(LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UISpinGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UISpinGlue[i].name, i3UISpinGlue[i].func);
	}
}
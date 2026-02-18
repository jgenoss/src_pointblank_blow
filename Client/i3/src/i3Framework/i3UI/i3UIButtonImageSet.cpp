#include "i3FrameworkPCH.h"
#include "i3UIButtonImageSet.h"
#include "i3UIManager.h"
#include "i3UITemplate_ButtonImageSet.h"
#include "../i3Framework.h"
#include "i3Base/string/compare/generic_is_iequal.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

// ============================================================================
//
// i3UIButton : 버튼
//
// ============================================================================

I3_CLASS_INSTANCE( i3UIButtonImageSet);

i3UIButtonImageSet::i3UIButtonImageSet()
{
#if defined( I3_DEBUG)
	SetName( "ButtonImageSet");
#endif
}

i3UIButtonImageSet::~i3UIButtonImageSet()
{
	if( m_pIBOver != nullptr) {
		this->RemoveChild( m_pIBOver);
		I3_SAFE_RELEASE( m_pIBOver);
	}

	if( m_pIBDown != nullptr) {
		this->RemoveChild( m_pIBDown);
		I3_SAFE_RELEASE( m_pIBDown);
	}

	if( m_pIBDisabled != nullptr) {
		this->RemoveChild( m_pIBDisabled);
		I3_SAFE_RELEASE( m_pIBDisabled);
	}
}

void i3UIButtonImageSet::_UpdateShapeState(void)
{
	i3UICaptionControl::_UpdateShapeState();

	for(INT32 i=0; i<this->getShapeCount(); i++) 
	{
		setShapeVisible( i, (i == (INT32)m_CurShape) && (isValidShape( i)));	//C4389
	}

	if(m_pIBOver == nullptr || m_pIBDown == nullptr || m_pIBDisabled == nullptr)
		return;

	if( !m_bViewOnly)
	{
		m_pIBOver->SetPosNoNotify( 0.0f, 0.0f);
		m_pIBOver->SetSizeNoNotify( &m_Size);

		m_pIBDown->SetPosNoNotify( 0.0f, 0.0f);
		m_pIBDown->SetSizeNoNotify( &m_Size);

		m_pIBDisabled->SetPosNoNotify( 0.0f, 0.0f);
		m_pIBDisabled->SetSizeNoNotify( &m_Size);

		if( isActivated()) 
		{
			m_pIBDisabled->EnableCtrl( false);

			if ( isState(I3UI_STATE_SELECTED) )
			{
				m_pIBOver->EnableCtrl( false) ;
				m_pIBDown->EnableCtrl( true );
			}
			else
			if (isState(I3UI_STATE_ONMOUSE))
			{
				m_pIBOver->EnableCtrl( true) ;
				m_pIBDown->EnableCtrl( false );
			}
			else
			{
				m_pIBOver->EnableCtrl( false) ;
				m_pIBDown->EnableCtrl( false );
			}

//			m_pIBOver->EnableCtrl( isState( I3UI_STATE_ONMOUSE));
//			m_pIBDown->EnableCtrl( isState( I3UI_STATE_SELECTED));

			this->OnSetFocus();
		}
		else 
		{
			if( m_pIBDisabled->isEnable() == false) m_pIBDisabled->EnableCtrl( true);
			if( m_pIBOver->isEnable()) m_pIBOver->EnableCtrl( false);
			if( m_pIBDown->isEnable()) m_pIBDown->EnableCtrl( false);
		}
	}
	else
	{
		if( m_pIBDisabled->isEnable()) m_pIBDisabled->EnableCtrl( false);
		if( m_pIBOver->isEnable()) m_pIBOver->EnableCtrl( false);
		if( m_pIBDown->isEnable()) m_pIBDown->EnableCtrl( false);
	}
}


void i3UIButtonImageSet::SetShapeIdx( INT32 nIdx )
{
	//I3_BOUNDCHK( nIdx, m_nShapeCount);
	if( nIdx > m_nShapeCount - 1)
		nIdx = 0;

	//I3ASSERT( nIdx >= 0);

	if( m_CurShape != (UINT32) nIdx)
	{
		m_CurShape = nIdx;
		addState( I3UI_STATE_NEED_UPDATE);
	}
}

void i3UIButtonImageSet::SetShapeIdxByString( char * pszName )
{
	// 임시 : 다 찾아봄. Template부분에서 String에 해당하는 SHAPE INFO를 검색해서 넘겨주는 방식으로 바꾸기
	// list -> map
	for( INT32 i=0; i<m_nShapeCount; i++) {
		if( i3::generic_is_iequal( m_pTemplate->getShapeInfoString( i), pszName) ) {
			m_CurShape = i;
			addState( I3UI_STATE_NEED_UPDATE);
			break;
		}
	}
}

bool i3UIButtonImageSet::_PrepareTexForSprite(void)
{
	bool bResult = true;
	VEC4D	vCoord, vTexRange;

	if( isValidShape( m_CurShape))
	{
		i3UIShape * pShape = getShape(m_CurShape);

		if( calcTexCoordAndRange( pShape, &vCoord, &vTexRange))
		{
			bResult = bResult && g_pUIRenderer->PrepareTextureForSprite( &vCoord, &vTexRange);
		}
	}

	return bResult;
}

INT32 i3UIButtonImageSet::GetShapeIdx( void )
{
	return m_CurShape;
}

const char * i3UIButtonImageSet::GetShapeString( void )
{
	return m_pTemplate->getShapeInfoString( m_CurShape);
}



void i3UIButtonImageSet::OnUpdate( REAL32 rDeltaSeconds)
{
	i3UICaptionControl::OnUpdate( rDeltaSeconds);
}



void i3UIButtonImageSet::OnBindTemplate( void )
{
	i3UICaptionControl::OnBindTemplate();

	if( m_pTemplate == nullptr)
		return;

	if( m_pIBOver == nullptr) {
		m_pIBOver = i3UIImageBox::new_object();
		m_pIBOver->SetName( "MouseOver");
		AddControl( m_pIBOver);
		m_pIBOver->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pIBOver->addStyle( I3UI_STYLE_SLAVE | I3UI_STYLE_IGNOREINPUT );
	}

	if( m_pIBDown == nullptr) {
		m_pIBDown = i3UIImageBox::new_object();
		m_pIBDown->SetName( "MouseDown");
		AddControl( m_pIBDown);
		m_pIBDown->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pIBDown->addStyle( I3UI_STYLE_SLAVE | I3UI_STYLE_IGNOREINPUT );
	}

	if( m_pIBDisabled == nullptr) {
		m_pIBDisabled = i3UIImageBox::new_object();
		m_pIBDisabled->SetName( "Disabled");
		AddControl( m_pIBDisabled);
		m_pIBDisabled->addGNodeStyle( I3_GAMENODE_STYLE_VOLATILE);
		m_pIBDisabled->addStyle( I3UI_STYLE_SLAVE | I3UI_STYLE_IGNOREINPUT );
	}

	m_pIBOver->SetTemplate( m_pTemplate->getSubTemplate( I3UI_IMAGESET_IB_OVER));
	m_pIBDown->SetTemplate( m_pTemplate->getSubTemplate( I3UI_IMAGESET_IB_DOWN));
	m_pIBDisabled->SetTemplate( m_pTemplate->getSubTemplate( I3UI_IMAGESET_IB_DISABLED));
}

void i3UIButtonImageSet::OnEnabled( bool bEnable )
{
	i3UICaptionControl::OnEnabled( bEnable);

	if( m_pIBOver != nullptr)
		m_pIBOver->EnableCtrl( bEnable);
	if( m_pIBDown != nullptr)
		m_pIBDown->EnableCtrl( bEnable);
	if( m_pIBDisabled != nullptr)
		m_pIBDisabled->EnableCtrl( bEnable);
}

void i3UIButtonImageSet::OnVisibleScene( void)
{
	i3UICaptionControl::OnVisibleScene();

	if( m_pIBOver != nullptr)
		m_pIBOver->OnVisibleScene();
	if( m_pIBDown != nullptr)
		m_pIBDown->OnVisibleScene();
	if( m_pIBDisabled != nullptr)
		m_pIBDisabled->OnVisibleScene();
}

void i3UIButtonImageSet::OnInvisibleScene( void)
{
	i3UICaptionControl::OnInvisibleScene();

	if( m_pIBOver != nullptr)
		m_pIBOver->OnInvisibleScene();
	if( m_pIBDown != nullptr)
		m_pIBDown->OnInvisibleScene();
	if( m_pIBDisabled != nullptr)
		m_pIBDisabled->OnInvisibleScene();
}

void i3UIButtonImageSet::OnDraw( i3UIRenderer * pRenderer)
{

	if( i3UI::getToolDevMode())
	{
		i3UIControl::OnDraw( pRenderer);
	}
	else
	{
		if( this->isEnable() == false)
			return;

		if(m_CurShape < (UINT32)m_nShapeCount)
		{
			if( isAllocedSprite())
			{
				INT32 h = getSpriteHandle( m_CurShape);
				if( h != -1)
					pRenderer->Draw_Kick( h);
			}

			m_bCalcDoneAccumPos = false;	// 초기화

			if( m_pText != nullptr && isEnable() )
				m_pText->OnDraw( pRenderer);

			if( m_pIBOver != nullptr)
				m_pIBOver->OnDraw( pRenderer);
			if( m_pIBDown != nullptr)
				m_pIBDown->OnDraw( pRenderer);
			if( m_pIBDisabled != nullptr)
				m_pIBDisabled->OnDraw( pRenderer);

			i3UIControl * pChild = (i3UIControl *) getFirstChild();

			while( pChild != nullptr)
			{
				pChild->OnDraw( pRenderer);
				pChild = (i3UIControl *) pChild->getNext();
			} 
		}
	}
}

void i3UIButtonImageSet::OnLButtonDblClk( UINT32 nFlag, POINT point)
{
//	i3UICaptionControl::OnLButtonDblClk( nFlag, point);

	ParentNotify( I3UI_EVT_DBL_CLICKED, point.x, point.y);
}


void i3UIButtonImageSet::OnLButtonDown( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnLButtonDown( nFlag, point);

	addState( I3UI_STATE_SELECTED | I3UI_STATE_NEED_UPDATE);

	_UpdateShapeState();

	ParentNotify( I3UI_EVT_BTN_PUSHED, point.x, point.y);
}


void i3UIButtonImageSet::OnLButtonUp( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnLButtonUp( nFlag, point);
	
	removeState( I3UI_STATE_SELECTED);
	m_PassedTime = 0.0f;

//	ParentNotify( I3UI_EVT_CLICKED, point.x, point.y);
//  OnClicked에 의해서도 point.x,y가 제대로 전달되므로 일단 생략.(11.10.22.수빈)
	addState( I3UI_STATE_NEED_UPDATE);
}


void i3UIButtonImageSet::OnRButtonDown( UINT32 nFlag, POINT point)
{
	i3UICaptionControl::OnRButtonDown( nFlag, point);

	ParentNotify( I3UI_EVT_R_CLICKED, point.x, point.y);

	addState( I3UI_STATE_NEED_UPDATE);
}

bool i3UIButtonImageSet::OnKillFocus( i3UIControl * pByCtrl /*= nullptr*/)
{
	bool bRet = i3UICaptionControl::OnKillFocus( pByCtrl);

	//Button의 State는 Focus와는 무관. 오직 Mouse에만 영향을 받는다.
	//removeState( I3UI_STATE_SELECTED | I3UI_STATE_ONMOUSE );

	removeState( I3UI_STATE_LBTN_DRAG);

	addState( I3UI_STATE_NEED_UPDATE);

	return bRet;
}

bool i3UIButtonImageSet::OnSetFocus()
{
	i3UICaptionControl::OnSetFocus();

	addState( I3UI_STATE_NEED_UPDATE);

	return true;
}

void i3UIButtonImageSet::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButtonImageSet::OnLeaveMouse(void)
{
	i3UICaptionControl::OnLeaveMouse();

	m_PassedTime = 0.0f;
	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButtonImageSet::OnKeyDown( UINT32 nKey)
{
	switch( nKey)
	{
		case VK_RETURN :
		case VK_SPACE :
			{
				addState( I3UI_STATE_SELECTED | I3UI_STATE_NEED_UPDATE);
			}
			break;
	}

	i3UICaptionControl::OnKeyDown( nKey);
}

void i3UIButtonImageSet::OnKeyUp( UINT32 nKey)
{
	i3UICaptionControl::OnKeyUp( nKey);
}

void i3UIButtonImageSet::OnMove( REAL32 x, REAL32 y)
{
	i3UICaptionControl::OnMove( x, y);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIButtonImageSet::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	addState( I3UI_STATE_NEED_UPDATE);

	//I3TRACE( "Image Size update %s\n", GetName());
}

i3UIControl* i3UIButtonImageSet::OnSelect( void)
{
	// Possible Cases: 
	// Child of i3UIListView_Header, ScrollBar, Spin, Combo
	if( (getGNodeStyle() & I3_GAMENODE_STYLE_VOLATILE) && (getStyle() & I3UI_STYLE_SLAVE))
	{
		i3GameNode* pParent = getParent();
		if( pParent != nullptr)
		{
			if(   i3::same_of<i3UIListView_Header* >(pParent) 
				|| i3::same_of<i3UIScrollBar* >(pParent) 
				|| i3::same_of<i3UISpin* >(pParent) 
				|| i3::kind_of<i3UIComboBase* >(pParent) )  	/* add Here */ 
			{
				return (i3UIControl*)pParent;
			}
		}
	}

	return this;
}


#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UIBUTTON
	{
		INT8		m_ID[4] = { 'U', 'B', 'T', '1' };
		UINT8		m_ViewOnly = 0;
		UINT8		pad8[3] = { 0 };
		UINT32		pad[7] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#pragma pack(pop)
#endif

UINT32 i3UIButtonImageSet::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UIBUTTON data;

	Result = i3UICaptionControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	data.m_ViewOnly = m_bViewOnly?1:0;

	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIButtonImageSet::OnLoad( i3ResourceFile * pResFile)
{
	i3_prof_func();

	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UIBUTTON data;

	Result = i3UICaptionControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	SetViewOnly( data.m_ViewOnly?true:false);

	return Result;
}

bool i3UIButtonImageSet::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);

	if( bResult == false)
		return false;

	INT32 viewonly = m_bViewOnly ? 1:0;
	pXML->addAttr( "ViewOnly", viewonly);

	return bResult;
}

bool i3UIButtonImageSet::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;

	INT32 viewonly;
	pXML->getAttr( "ViewOnly", &viewonly);

	SetViewOnly( viewonly?true:false);

	return bResult;
}

void i3UIButtonImageSet::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UIControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of<i3UIButtonImageSet*>(pDest));

	i3UIButtonImageSet* pButton = (i3UIButtonImageSet*)pDest;

	pButton->SetViewOnly( m_bViewOnly);

	pButton->m_CurShape = m_CurShape;
}

#if defined( I3_DEBUG)
void i3UIButtonImageSet::GetTDKGridProperties(i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);	

	AddMFCProperty_BeginGroup( List, "i3UIButtonImageSet");
	AddMFCProperty_Bool( List, &m_bViewOnly, "View Only", "Mouse Event를 처리하지 않을 경우 True로 설정하십시오.");
	AddMFCProperty_EndGroup( List);
}
#endif

void i3UIButtonImageSet::Init( void)
{
	addState( I3UI_STATE_NEED_UPDATE);
}

INT32 i3UIButtonImageSet::OnCommand(LuaState * pState)
{
	i3UICaptionControl::OnCommand( pState);

	const char * pszCom = i3Lua::GetStringArg(pState, 2, nullptr, 0);

	I3TRACE( "DO %s\n", pszCom);

	return 0;
}



//////////////////////////////////////////////////////////////////////////
// Lua glue

extern "C" int i3UIImageSet_setShape( lua_State* L)
{
	i3UIButtonImageSet* pCtrl = (i3UIButtonImageSet*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32	nIdx = i3Lua::GetIntegerArg( L, 2, -1);

		if( nIdx == -1)
		{
			pCtrl->EnableCtrl( false);
		}
		else
		{
			pCtrl->SetShapeIdx( nIdx);

			if( pCtrl->isEnable() == false)
				pCtrl->EnableCtrl( true);
		}
	}
	return 0;
}

extern "C" int i3UIImageSet_setShapeByString( lua_State* L)
{
	i3UIButtonImageSet* pCtrl = (i3UIButtonImageSet*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		char szName[MAX_PATH];
		i3Lua::GetStringArg( L, 2, szName, MAX_PATH);
		pCtrl->SetShapeIdxByString( szName);
	}
	return 0;
}

extern "C" int i3UIImageSet_getShapeIndex( lua_State* L)
{
	i3UIButtonImageSet* pCtrl = (i3UIButtonImageSet*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		INT32 nIdx = pCtrl->GetShapeIdx();
		i3Lua::PushInteger( L, nIdx);

		return 1;
	}
	return 0;
}

extern "C" int i3UIImageSet_getShapeString( lua_State* L)
{
	i3UIButtonImageSet* pCtrl = (i3UIButtonImageSet*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{
		const char * pszName = pCtrl->GetShapeString();
		i3Lua::PushString( L, pszName);

		return 1;
	}
	return 0;
}

extern "C" int i3UIImageSet_SetViewOnly( LuaState * L)
{
	i3UIButtonImageSet* pCtrl = (i3UIButtonImageSet*)i3Lua::GetUserDataArg( L, 1);
	if( pCtrl != nullptr)
	{	
		bool bView = i3Lua::GetBooleanArg( L, 2);

		pCtrl->SetViewOnly( bView);

		return 1;
	}

	return 0;
}

LuaFuncReg i3UIImageSetGlue[] = 
{
	{	"uiImageSet_setShape",			i3UIImageSet_setShape			},
	{	"uiImageSet_setShapeByString",	i3UIImageSet_setShapeByString	},
	{	"uiImageSet_getShapeIndex",		i3UIImageSet_getShapeIndex		},
	{	"uiImageSet_getShapeString",	i3UIImageSet_getShapeString	},
	{	"uiImageSet_setViewOnly",		i3UIImageSet_SetViewOnly},

	{nullptr, nullptr}
};

void i3UIButtonImageSet::RegisterLuaFunction( LuaState * pState)
{
	if( pState == nullptr)
		return;

	for(int i=0; i3UIImageSetGlue[i].name; i++)
	{
		i3Lua::RegisterFunc( pState, i3UIImageSetGlue[i].name, i3UIImageSetGlue[i].func);
	}
}

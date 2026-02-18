#include "i3FrameworkPCH.h"
#include "i3UIToolTip.h"
#include "i3UIManager.h"
#include "i3UITemplate_ToolTip.h"
#include "../i3Framework.h"

// ============================================================================
//
// i3UIToolTip : 버튼
//
// ============================================================================

I3_CLASS_INSTANCE( i3UIToolTip);

i3UIToolTip::i3UIToolTip()
{
#if defined( I3_DEBUG)
	SetText( L"This is Tool Tip");
#endif

	setTextAutoLineFeed( true);
	setTextLineFeedSplitWord( true);
}

void i3UIToolTip::_UpdateShapeState( void)
{
	i3UICaptionControl::_UpdateShapeState();


	// 무조건 true로 하지 말고...enable/disable을 따질 것
	
	bool bVisible = i3UIToolTip::isEnable();

	for( INT32 i=0; i< I3UI_TOOLTIP_SHAPE_COUNT; i++)		//getShapeCount() 이걸로 하면?
		setShapeVisible( i, bVisible);
}

void i3UIToolTip::OnEnterMouse( REAL32 x, REAL32 y, UINT32 keys)
{
	i3UICaptionControl::OnEnterMouse( x, y, keys);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIToolTip::OnLeaveMouse(void)
{
	i3UICaptionControl::OnLeaveMouse();

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIToolTip::OnSize( REAL32 cx, REAL32 cy)
{
	i3UICaptionControl::OnSize( cx, cy);

	addState( I3UI_STATE_NEED_UPDATE);
}

void i3UIToolTip::OnMove( REAL32 x, REAL32 y)
{
	i3UICaptionControl::OnMove( x, y);
}

// cursorPos must be client coordinate
void i3UIToolTip::ChangePos( POINT cursorPos)
{
	INT32 x = cursorPos.x + m_nPosOffsetX;
	INT32 y = cursorPos.y + m_nPosOffsetY;

	POINT pPt;
	pPt.x = i3UI::getViewer()->GetViewWidth();	pPt.y = i3UI::getViewer()->GetViewHeight();
	i3UI::getManager()->ConvertMouseCoord(&pPt);

	if( i3UI::getViewer() != nullptr)
	{	// 화면에서 벗어나는 것을 안쪽으로 땡겨~
		if( x + getWidth() > pPt.x)
			x = pPt.x - (INT32)getWidth();

		if( y + getHeight() > pPt.y)
			y = pPt.y - (INT32)getHeight();
	}

	//Tooltip은 scene의 child라는 가정. 그렇지 않으면 m_AccPos를 고려해주어야 한다. 
	setPos( (REAL32)x, (REAL32)y);
}

void i3UIToolTip::EnableCtrl( bool bTrue, bool bRecursive)
{
	i3UICaptionControl::EnableCtrl( bTrue, bRecursive);

	// tool tip은 framewnd단위가 아닌, 컨트롤 단위로 enable/disable되는 특수한 경우 이며
	// 대부분 template을 공유 하기 때문에, 굳이 wait_load state를 추가할 필요가 없으며,
	// wait_load를 넣는 경우엔 정상적으로 보여지지 않는다.
	if( bTrue) removeState( I3UI_STATE_WAITLOAD);
}

void i3UIToolTip::OnDraw( i3UIRenderer * pRenderer)
{
	pRenderer->Draw_SetTopMost( true);
	//pRenderer->DepthPlus();

	i3UICaptionControl::OnDraw( pRenderer);

	//pRenderer->DepthMinus();

	pRenderer->Draw_SetTopMost( false);
}

#if defined( I3_COMPILER_VC)
#pragma pack(push, 4)
#endif

namespace pack
{
	struct UITOOLTIP
	{
		INT8		m_ID[4] = { 'U', 'T', 'T', '1' };
		UINT32		pad[8] = { 0 };
	};
}

#if defined( I3_COMPILER_VC)
#if defined(PACKING_RIGHT)
	#pragma pack(pop)					// [test required] push를 pop으로 정상 수정합니다. 이후 패킹에서 사이즈가 잘 못 처리될 수 있음. 2017.02.09. soon9
#else
	#pragma pack(push, 4)
#endif
#endif

UINT32 i3UIToolTip::OnSave( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result= 0;
	pack::UITOOLTIP data;

	Result = i3UICaptionControl::OnSave( pResFile );
	I3_CHKIO( Result);
	
	Rc = pResFile->Write( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

UINT32 i3UIToolTip::OnLoad( i3ResourceFile * pResFile)
{
	UINT32	Rc, Result =0;
	i3Stream * pStream = pResFile->GetStream();
	pack::UITOOLTIP data;

	Result = i3UICaptionControl::OnLoad( pResFile );
	I3_CHKIO( Result);

	Rc = pStream->Read( &data, sizeof( data));
	I3_CHKIO( Rc);
	Result += Rc;

	return Result;
}

bool i3UIToolTip::OnSaveXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UICaptionControl::OnSaveXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->addAttr( "OffsetX", m_nPosOffsetX);
	pXML->addAttr( "OffsetY", m_nPosOffsetY);
	pXML->addAttr( "MaxWidth", m_nMaxWidth);

	return bResult;
}

bool i3UIToolTip::OnLoadXML( i3XMLFile * pFile, i3XMLElement * pXML)
{
	bool bResult = i3UICaptionControl::OnLoadXML( pFile, pXML);

	if( bResult == false)
		return false;

	pXML->getAttr( "OffsetX", &m_nPosOffsetX);
	pXML->getAttr( "OffsetY", &m_nPosOffsetY);
	pXML->getAttr( "MaxWidth", &m_nMaxWidth);

	return bResult;
}

void i3UIToolTip::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3UICaptionControl::CopyTo( pDest, method );

	I3ASSERT( i3::same_of<i3UIToolTip*>(pDest));

	i3UIToolTip* pToolTip = (i3UIToolTip*)pDest;

	pToolTip->setPosOffsetX( m_nPosOffsetX);
	pToolTip->setPosOffsetY( m_nPosOffsetY);
	pToolTip->setMaxWidth( m_nMaxWidth);
}

#if defined( I3_DEBUG)
void i3UIToolTip::GetTDKGridProperties( i3::vector<PROPERTYINFO*>& List)
{
	i3UICaptionControl::GetTDKGridProperties( List);

	AddMFCProperty_BeginGroup( List, "i3UIToolTip");
	
	AddMFCProperty_Int32( List, &m_nPosOffsetX, "Offset X");
	AddMFCProperty_Int32( List, &m_nPosOffsetY, "Offset Y");
	AddMFCProperty_Int32( List, &m_nMaxWidth, "Maximum Width");

	AddMFCProperty_EndGroup( List);
}
#endif	// I3_DEBUG


template<class StringType>									
void	i3UIToolTip::SetTextImpl( const StringType& str )
{	
	if( m_pRects == nullptr)
	{
		i3UICaptionControl::SetText( str);
		return;
	}

	//Max Width에 맞춰서 Text를 출력해본다. text extent 확인.
	REAL32 width = (REAL32)m_nMaxWidth;
	width += m_pRects[I3UI_TOOLTIP_SHAPE_LEFT].right;
	width += m_pRects[I3UI_TOOLTIP_SHAPE_RIGHT].right;

	setSize( width, 50.0f);	//50.0f is a templorary value.. the appropriate value will be set.

	i3UICaptionControl::SetText( str);

	//Adjust the size of control
	if( m_pText != nullptr)
	{
		setTextAutoLineFeed( true);
		setTextLineFeedSplitWord( true);

		//Center
		INT32 nExtW = m_pText->getExtentWidth();
		INT32 nExtH = m_pText->getExtentHeight();

		REAL32 w = (REAL32)min( nExtW, m_nMaxWidth);
		REAL32 h = (REAL32)nExtH;

		//if( nExtW > m_nMaxWidth)
		//	h += m_pText->getDefFontHeight();

		/*i3::pack::RECT rt;
		rt.left = 0;
		rt.top = 0;
		rt.right = w;
		rt.bottom = h;*/
		
		//SetTextRect( &rt);

		//Left & Right
		w += m_pRects[I3UI_TOOLTIP_SHAPE_LEFT].right;
		w += m_pRects[I3UI_TOOLTIP_SHAPE_RIGHT].right;

		////Top & Bottom
		h += m_pRects[I3UI_TOOLTIP_SHAPE_TOP].bottom;
		h += m_pRects[I3UI_TOOLTIP_SHAPE_BOTTOM].bottom;

		//I3TRACE("w: %f, h: %f, nExtW: %d\n", w, h, nExtW);
		setSize( w+2, h+2);

		i3UICaptionControl::SetText( str);

		ShowTop();
	}
	
}

void	i3UIToolTip::SetText( const wchar_t* pText)
{
	i3UIToolTip::SetTextImpl( i3::wliteral_range(pText) );
}

void	i3UIToolTip::SetText( const i3::wliteral_range& wTextRng)
{	
	i3UIToolTip::SetTextImpl(wTextRng);
}

void	i3UIToolTip::SetText( const i3::rc_wstring& strText)
{
	i3UIToolTip::SetTextImpl(strText);
}

void	i3UIToolTip::SetText( const i3::wstring& wstrText)
{
	i3UIToolTip::SetTextImpl( i3::wliteral_range(wstrText) );
}

void	i3UIToolTip::SetText( const i3::stack_wstring& wstrText)
{
	i3UIToolTip::SetTextImpl( i3::wliteral_range(wstrText) );
}

/*
void i3UIToolTip::SetText( const WCHAR16 * pText)
{
	if( m_pRects == nullptr)
	{
		i3UICaptionControl::SetText( pText);
		return;
	}

	//Max Width에 맞춰서 Text를 출력해본다. text extent 확인.
	REAL32 width = (REAL32)m_nMaxWidth;
	width += m_pRects[I3UI_TOOLTIP_SHAPE_LEFT].right;
	width += m_pRects[I3UI_TOOLTIP_SHAPE_RIGHT].right;

	setSize( width, 50.0f);	//50.0f is a templorary value.. the appropriate value will be set.

	i3UICaptionControl::SetText( pText);

	//Adjust the size of control
	if( m_pText != nullptr)
	{
		//Center
		INT32 nExtW = m_pText->getExtentWidth();
		INT32 nExtH = m_pText->getExtentHeight();

		REAL32 w = (REAL32)min( nExtW, m_nMaxWidth);
		REAL32 h = (REAL32)nExtH;

		//if( nExtW > m_nMaxWidth)
		//	h += m_pText->getDefFontHeight();

		i3::pack::RECT rt;
//		rt.left = 0;
//		rt.top = 0;
//		rt.right = w;
//		rt.bottom = h;
		
		//SetTextRect( &rt);

		//Left & Right
		w += m_pRects[I3UI_TOOLTIP_SHAPE_LEFT].right;
		w += m_pRects[I3UI_TOOLTIP_SHAPE_RIGHT].right;

		////Top & Bottom
		h += m_pRects[I3UI_TOOLTIP_SHAPE_TOP].bottom;
		h += m_pRects[I3UI_TOOLTIP_SHAPE_BOTTOM].bottom;

		//I3TRACE("w: %f, h: %f, nExtW: %d\n", w, h, nExtW);
		setSize( w+2, h+2);

		i3UICaptionControl::SetText( pText);

		ShowTop();
	}
}
*/


#if !defined( __I3_GIC_DEVICE_CONTEXT_H)
#define __I3_GIC_DEVICE_CONTEXT_H

class i3GICDiagram;
class i3GICDiagramView;

#include "i3GICOffsetStack.h"

#define		I3GDC_FLAG_NOCONTROL		0x00000001

class I3_EXPORT_GIC i3GICDeviceContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GICDeviceContext, i3ElementBase);
protected:
	i3GICDiagramView *	m_pView = nullptr;

	HDC			m_hDC = nullptr;
	RECT		m_UpdateRect = { 0, 0, 0, 0 };
	UINT32		m_Flags = 0;

	SIZE				m_SavedWndExt;
	POINT				m_SavedWndOrg;
	SIZE				m_SavedViewExt;
	POINT				m_SavedViewOrg;
	int					m_SavedMapMode;

	i3GICOffsetStack	m_OffStack;

public:
	i3GICDeviceContext(void) {}
	virtual ~i3GICDeviceContext(void) {}

	/** \brief 다이어그램 뷰를 반환합니다. ????
		\return 다이어그램 뷰		*/
	i3GICDiagramView *	getView(void)						{ return m_pView; }

	/** \brief 다이어그램 뷰를 설정합니다. 
		\param [in] pView i3GICDiagramView객체의 포인터
		\return none		*/
	void				setView( i3GICDiagramView * pView)	{ m_pView = pView; }

	/** \brief 플래그를 반환합니다. 
		\return 플래그		*/
	UINT32				getFlags(void)						{ return m_Flags; }

	/** \brief 플래그를 설정합니다. 
		\param flag 플래그
		\return none		*/
	void				setFlags( UINT32 flag)				{ m_Flags = flag; }

	/** \brief 플래그를 추가합니다. 
		\param flag 플래그
		\return none		*/
	void				addFlags( UINT32 flag)				{ m_Flags |= flag; }

	/** \brief 플래그를 삭제합니다. 
		\param flag 플래그
		\return none		*/
	void				removeFlags( UINT32 flag)			{ m_Flags &= ~flag; }

	/** \brief Device Context의 핸들을 반환합니다.	
		\return Device Context의 핸들		*/
	HDC					getDC(void)							{ return m_hDC; }

	/** \brief Device Context의 핸들을 설정합니다.	
		\param dc Device Context의 핸들		
		\return none		*/
	void				setDC( HDC dc)						{ m_hDC = dc; }

	RECT *				getUpdateRect(void)					{ return &m_UpdateRect; }
	void				setUpdateRect( RECT * pRect)
	{
		m_UpdateRect.left = pRect->left;
		m_UpdateRect.top = pRect->top;
		m_UpdateRect.right = pRect->right;
		m_UpdateRect.bottom = pRect->bottom;
	}

	void				setUpdateRect( INT32 left, INT32 top, INT32 right, INT32 bottom)
	{
		m_UpdateRect.left = left;
		m_UpdateRect.top = top;
		m_UpdateRect.right = right;
		m_UpdateRect.bottom = bottom;
	}

	void				reset( REAL32 offX, REAL32 offY)
	{
		m_OffStack.reset( offX, offY);
	}


	void				SetModeToDevice(void);
	void				SetModeToDraw(void);

	void				_LP( POINT * pPt)						{ ::DPtoLP( m_hDC, pPt, 1); }
	void				_LP( RECT * pRect)						{ ::DPtoLP( m_hDC, (LPPOINT) pRect, 2);	}

	void				_DP( POINT * pPt)						{ ::LPtoDP( m_hDC, pPt, 1); }
	void				_DP( RECT * pRect)						{ ::LPtoDP( m_hDC, (LPPOINT) pRect, 2);	}

	i3GICOffsetStack *	getStack(void)							{ return &m_OffStack; }
	VEC2D *				getStackTop(void)						{ return m_OffStack.getTop(); }
	void				GetDrawPos( POINT * pPt)
	{
		pPt->x = (INT32)( pPt->x + m_OffStack.getTop()->x);
		pPt->y = (INT32)( pPt->y + m_OffStack.getTop()->y);
	}


	//////////////////////////////////////////////////////////////////////////////////////
	// GDI Interface Functions
	HGDIOBJ				SelectObject( HGDIOBJ hObj)				{ return ::SelectObject( m_hDC, hObj); }
	COLORREF			SetTextColor( COLORREF ref)				{ return ::SetTextColor( m_hDC, ref); }
	COLORREF			SetBkColor( COLORREF ref)				{ return ::SetBkColor( m_hDC, ref); }
	int					SetBkMode( int mode)					{ return ::SetBkMode( m_hDC, mode); }
	int					DrawText( const char * pszText, LPRECT pRect, UINT nFormat = DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE)
	{
		RECT rt;
		VEC2D * pOff = m_OffStack.getTop();

		rt.left = (LONG) (pRect->left + pOff->x);
		rt.top	= (LONG) (pRect->top + pOff->y);
		rt.right = (LONG) (pRect->right + pOff->x);
		rt.bottom = (LONG) (pRect->bottom + pOff->y);

		return ::DrawText( m_hDC, pszText, -1, &rt, nFormat);
	}
	

	void				FillRect( INT32 left, INT32 top, INT32 right, INT32 bottom, HBRUSH hBrush)
	{
		RECT rt;
		VEC2D * pOff = m_OffStack.getTop();

		rt.left = (INT32)(left + pOff->x);
		rt.top = (INT32)(top + pOff->y);
		rt.right = (INT32)(right + pOff->x);
		rt.bottom = (INT32)(bottom + pOff->y);

		::FillRect( m_hDC, &rt, hBrush);
	}

	void				FillRect( RECT * pRect, HBRUSH hBrush)
	{
		RECT rt;
		VEC2D * pOff = m_OffStack.getTop();

		rt = * pRect;
		::OffsetRect( &rt, (INT32) pOff->x, (INT32) pOff->y);

		::FillRect( m_hDC, &rt, hBrush);
	}

	void				MoveTo( INT32 x, INT32 y)
	{
		VEC2D * pOff = m_OffStack.getTop();
		::MoveToEx( m_hDC, (INT32)(x + pOff->x), (INT32)(y + pOff->y), nullptr);
	}

	void				LineTo( INT32 x, INT32 y)
	{
		VEC2D * pOff = m_OffStack.getTop();
		::LineTo( m_hDC, (INT32)(x + pOff->x), (INT32)(y + pOff->y));
	}

	void				Rectangle( INT32 left, INT32 top, INT32 right, INT32 bottom)
	{
		VEC2D * pOff = m_OffStack.getTop();
		::Rectangle( m_hDC, (INT32)(left + pOff->x), (INT32)(top + pOff->y), 
							(INT32)(right + pOff->x), (INT32)(bottom + pOff->y));
	}

	void				Rectangle( RECT * pRect)
	{
		VEC2D * pOff = m_OffStack.getTop();
		::Rectangle( m_hDC, (INT32)(pRect->left + pOff->x), (INT32)(pRect->top + pOff->y), 
							(INT32)(pRect->right + pOff->x), (INT32)(pRect->bottom + pOff->y));
	}

	void				RoundRect( INT32 l, INT32 t, INT32 r, INT32 b, INT32 rw, INT32 rh)
	{
		VEC2D * pOff = m_OffStack.getTop();

		::RoundRect( m_hDC, (INT32)(l + pOff->x), (INT32)(t + pOff->y),
							(INT32)(r + pOff->x), (INT32)(b + pOff->y),
							rw, rh);
	}

	void				DP_RoundRect( INT32 l, INT32 t, INT32 r, INT32 b, INT32 rw, INT32 rh)
	{
		::RoundRect( m_hDC, l, t, r, b, rw, rh);
	}

	void				Ellipse( INT32 l, INT32 t, INT32 r, INT32 b)
	{
		VEC2D * pOff = m_OffStack.getTop();
		::Ellipse( m_hDC, (INT32)(l + pOff->x), (INT32)(t + pOff->y),
							(INT32)(r + pOff->x), (INT32)(b + pOff->y));
	}

	void				DP_Ellipse( INT32 l, INT32 t, INT32 r, INT32 b)
	{
		::Ellipse( m_hDC, l, t, r, b);
	}
};

#endif

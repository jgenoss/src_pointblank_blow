#if !defined( __I3_GIC_DIAGRAM_CONTEXT_H__)
#define __I3_GIC_DIAGRAM_CONTEXT_H__

#include "i3GICDiagram.h"
#include "i3GICOffsetStack.h"
#include "i3GICDefine.h"

#if defined( I3_DEBUG)

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

typedef UINT32 I3GIC_CTX_FLAG;
#define		I3GIC_CTX_FLAG_NOCONTROL			0x00000001		///< Control Point를 제외하고 처리한다.
#define		I3GIC_CTX_FLAG_NOSNAP				0x00000002		///< Snap 기능을 끄고 처리
#define		I3GIC_CTX_FLAG_CENTERLINKER_ONLY	0x00000004		///< Center Link point만 찾는다.
#define		I3GIC_CTX_FLAG_NOLINKER				0x00000008		///< Link Point를 제외하고 처리한다.

enum GIC_MODE
{
	GIC_MODE_DESIGN = 0,			///< 일반적인 Shape 생성 및 배치
	GIC_MODE_DESIGNTREENODE,		///< Shape의 Tree 구조를 설정하는 모드
};

enum GIC_HIT_RESULT_NUM
{
	GIC_HIT_RESULT_NONE = 0,
	GIC_HIT_RESULT_MOUSEDOWN,
	GIC_HIT_RESULT_MOUSEUP,
	GIC_HIT_RESULT_MOUSEMOVE,
	GIC_HIT_RESULT_MOUSEDBLCLK,

	GIC_HIT_RESULT_MAX
};

class i3GICShapeLine;
class i3GICDiagramView;

class I3_EXPORT_GIC i3GICDiagramContext : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3GICDiagramContext, i3ElementBase);

protected:
	i3GICDiagram *		m_pDiagram = nullptr;				// 현재 편집되고 있는 Diagram

	// Volatile
	i3GICDiagramView *	m_pView = nullptr;

	GIC_MODE			m_Mode = GIC_MODE_DESIGN;
	
	i3::pack::RECT				m_UpdateRect;			/// Update 영역
	I3GIC_CTX_FLAG		m_Flags = 0;

	SIZE				m_SavedWndExt;
	POINT				m_SavedWndOrg;
	SIZE				m_SavedViewExt;
	POINT				m_SavedViewOrg;
	int					m_SavedMapMode;
	
	i3GICOffsetStack	m_OffStack;				/// 각 Shape에 대한 Offset

	CPoint				m_DragStartPt;
	RECT				m_DragOldRect;
	GIC_MOUSEMODE		m_MouseMode = GIC_MOUSEMODE_NONE;
	i3ClassMeta *		m_pMetaToCreate = nullptr;

	REAL32				m_rDeltaX;				///< Point의 X 이동량
	REAL32				m_rDeltaY;				///< Point의 Y 이동량
	VEC2D				m_vMousePos;
	GIC_HIT_RESULT_NUM	m_iCurrentHitIdx = GIC_HIT_RESULT_NONE;
	GIC_HIT_RESULT		m_HitResult[GIC_HIT_RESULT_MAX];		// Mouse point hit test에 대한 결과

	i3GICShapeLine *	m_pLinkLine = nullptr;

	HPEN				m_hPenCP;		// CP용 hPen

	//----------------------------------------------------------------------------------//
	//							Mouse Event process										//
	//----------------------------------------------------------------------------------//
protected:
	GIC_MOUSE_CURSOR	_GetCursorTypeByHit( void);
	void				_LBtnDown_ModeNone( UINT32 nFlags, POINT * pPoint);

	void				_EnableLinkLine( i3GICShape * pShape);

	//----------------------------------------------------------------------------------//
	//							public Function											//
	//----------------------------------------------------------------------------------//
public:
	i3GICDiagram *		getDiagram( void)					{ return m_pDiagram; }
	void				setDiagram( i3GICDiagram * pDiagram)
	{
		if( m_pDiagram != nullptr)
			m_pDiagram->BindContext( false);

		I3_REF_CHANGE( m_pDiagram, pDiagram);

		if( m_pDiagram != nullptr)
			m_pDiagram->BindContext( true);
	}

	i3GICDiagramView *	getView( void)						{ return m_pView; }

	GIC_MODE			getMode( void)						{ return m_Mode; }
	void				setMode( GIC_MODE mode)				{ m_Mode = mode; }

	i3::pack::RECT *			getUpdateRect(void)					{ return &m_UpdateRect; }
	void				setUpdateRect( RECT * pRect)
	{
		I3ASSERT( pRect != nullptr);

		m_UpdateRect.left = (REAL32) pRect->left;
		m_UpdateRect.top = (REAL32) pRect->top;
		m_UpdateRect.right = (REAL32) pRect->right;
		m_UpdateRect.bottom = (REAL32) pRect->bottom;
	}

	void				setUpdateRect( INT32 left, INT32 top, INT32 right, INT32 bottom)
	{
		m_UpdateRect.left = (REAL32) left;
		m_UpdateRect.top = (REAL32) top;
		m_UpdateRect.right = (REAL32) right;
		m_UpdateRect.bottom = (REAL32) bottom;
	}

	/** \brief 플래그를 반환합니다. 
		\return 플래그		*/
	I3GIC_CTX_FLAG		getFlags(void)						{ return m_Flags; }

	/** \brief 플래그를 설정합니다. 
		\param flag 플래그
		\return none		*/
	void				setFlags( I3GIC_CTX_FLAG flag)		{ m_Flags = flag; }

	/** \brief 플래그를 추가합니다. 
		\param flag 플래그
		\return none		*/
	void				addFlags( I3GIC_CTX_FLAG flag)		{ m_Flags |= flag; }

	/** \brief 플래그를 삭제합니다. 
		\param flag 플래그
		\return none		*/
	void				removeFlags( I3GIC_CTX_FLAG flag)	{ m_Flags &= ~flag; }

	void				resetOffstack( REAL32 offX, REAL32 offY)
	{
		m_OffStack.reset( offX, offY);
	}

	i3GICOffsetStack *	getStack(void)							{ return &m_OffStack; }
	VEC2D *				getStackTop(void)						{ return m_OffStack.getTop(); }
	void				GetDrawPos( POINT * pPt)
	{
		pPt->x = (INT32)( pPt->x + m_OffStack.getTop()->x);
		pPt->y = (INT32)( pPt->y + m_OffStack.getTop()->y);
	}

	GIC_MOUSEMODE		getMouseMode( void)						{ return m_MouseMode; }

	i3ClassMeta *		getMetaToCreate( void)					{ return m_pMetaToCreate; }
	void				setMetaToCreate( i3ClassMeta * pClass)	{ m_pMetaToCreate = pClass; }

	REAL32				getDeltaX( void)						{ return m_rDeltaX; }
	void				setDeltaX( REAL32 dx)					{ m_rDeltaX = dx; }

	REAL32				getDeltaY( void)						{ return m_rDeltaY; }
	void				setDeltaY( REAL32 dy)					{ m_rDeltaY = dy; }

	void				setDelta( REAL32 dx, REAL32 dy)			{ m_rDeltaX = dx; m_rDeltaY = dy; }

	VEC2D *				getMousePos( void)						{ return &m_vMousePos; }
	void				setMousePos( REAL32 x, REAL32 y)		{ i3Vector::Set( &m_vMousePos, x, y); }

	void				setCurrentHitNum( GIC_HIT_RESULT_NUM num)	{ m_iCurrentHitIdx = num; }
	GIC_HIT_RESULT *	getCurrentHitResult( void)				{ return &m_HitResult[ m_iCurrentHitIdx]; }
	GIC_HIT_RESULT *	getHitResult( GIC_HIT_RESULT_NUM num)	{ return &m_HitResult[ num]; }

	HPEN 				getCPPen( void)							{ return m_hPenCP; }
	
public:
	i3GICDiagramContext();
	virtual ~i3GICDiagramContext();

	void			Create( i3GICDiagramView * pView, RECT * pRect);
	void			DrawShapes( RECT * pRect = nullptr);

	void			ObserveShapeState( void);
	
	void			ProcessLButtonDown(UINT nFlags, CPoint point);
	void			ProcessLButtonUp(UINT nFlags, CPoint point);
	GIC_MOUSE_CURSOR	ProcessMouseMove(UINT nFlags, CPoint point);
	void				ProcessPopupMenu( UINT32 cmd);
	void				ProcessKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags);
	
	GIC_MOUSE_CURSOR	SetMouseMode( GIC_MOUSEMODE mode);

	void				SetModeToDevice(void);
	void				SetModeToDraw(void);

	
	//////////////////////////////////////////////////////////////////////////////////////////
	//										Shape Edit Function								//
public:
	void			CopyToClipboard( void);
	void			PasteFromClipboard( void);
	bool			IsClipboardData( void);

	void			ShapeToBack( void);
	void			ShapeToLastBack( void);
	void			ShapeToFront( void);
	void			ShapeToFirstFront( void);

	void			AlignToCenter( void);
	void			AlignToLeft( void);
	void			AlignToRight( void);
	void			AlignToTop( void);
	void			AlignToMiddle( void);
	void			AlignToBottom( void);

	void			PreSelectShape( i3GICShape * pShape);
	void			SelectShape( i3GICShape * pShape);
	void			UnselectShape( void);

	void			DeleteShape( void);

	//////////////////////////////////////////////////////////////////////////////////////////
	//										Diagram Edit									//
public:
	bool			FindShapeByPos( REAL32 x, REAL32 y);
	void			FindShapeByRect( i3::pack::RECT * pRect, i3::vector<i3GICShape*>& List);
	void			FindShapeByRect( RECT * pRect, i3::vector<i3GICShape*>& List )
	{
		i3::pack::RECT rt;

		rt.left = (REAL32) pRect->left;		rt.top = (REAL32) pRect->top;
		rt.right = (REAL32) pRect->right;	rt.bottom = (REAL32) pRect->bottom;

		FindShapeByRect( &rt, List);
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//										Grid											//
private:
	bool				m_bGrid = false;
	bool				m_bLinkSnap = false;
	bool				m_bGridSnap = false;
	bool				m_bLinkUpdate = false;			// Link된 Shape에 대한 Update 여부
	INT32				m_iGridX = 30;
	INT32				m_iGridY = 30;
	INT32				m_iSnapEps = 3;

protected:
	bool				_GridSnap( REAL32 * pX, REAL32 * pY, INT32 iSnapX, INT32 iSnapY);

public:
	bool				isGridEnable( void)					{ return m_bGrid; }
	void				setGridEnable( bool bVal)			{ m_bGrid = bVal; }

	bool				isLinkSnapEnable( void)				{ return m_bLinkSnap; }
	void				setLinkSnapEnable( bool bVal)		{ m_bLinkSnap = bVal; }

	bool				isGridSnapEnable( void)				{ return m_bGridSnap; }
	void				setGridSnapEnable( bool bVal)		{ m_bGridSnap = bVal; }

	bool				isLinkUpdate( void)					{ return m_bLinkUpdate; }
	void				setLinkUpdate( bool bVal)			{ m_bLinkUpdate = bVal; }

	INT32				getGridX( void)						{ return m_iGridX; }
	void				setGridX( INT32 x)					{ m_iGridX = x; }

	INT32				getGridY( void)						{ return m_iGridY; }
	void				setGridY( INT32 y)					{ m_iGridY = y; }

	INT32				getSnapEps( void)					{ return m_iSnapEps; }
	void				setSnapEps( INT32 iEps)				{ m_iSnapEps = iEps; }

	bool				IsGridSnapPos( INT32 x, INT32 y);

	/** \brief Grid Snap Position을 계산한다.
		\param[in,out] pX
		\param[in,out] pY
		\return true : Snapped */
	bool				GridSnapPos( REAL32 * pX, REAL32 * pY);


	bool				LinkSnapPos( i3GICShape * pShape, REAL32 * pX, REAL32 * pY, i3GICLinkPoint * pPointer = nullptr);



	//////////////////////////////////////////////////////////////////////////////////////////////////
	//										GDI Interface Functions									//
	//////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void			toLP( POINT * pPt);
	void			toLP( RECT * pRect);

	void			toDP( POINT * pPt);
	void			toDP( RECT * pRect);
	
	HGDIOBJ			selectObject( HGDIOBJ hObj);
	COLORREF		setTextColor( COLORREF ref);
	COLORREF		setBkColor( COLORREF ref);
	INT32			setBkMode( INT32 mode);
	INT32			drawText( const char * pszText, LPRECT pRect, UINT nFormat = DT_END_ELLIPSIS | DT_LEFT | DT_SINGLELINE);

	void			fillRect( INT32 left, INT32 top, INT32 right, INT32 bottom, HBRUSH hBrush);

	void			fillRect( RECT * pRect, HBRUSH hBrush);

	void			moveTo( INT32 x, INT32 y);

	void			lineTo( INT32 x, INT32 y);

	void			rectangle( INT32 left, INT32 top, INT32 right, INT32 bottom);

	void			rectangle( RECT * pRect);

	void			roundRect( INT32 l, INT32 t, INT32 r, INT32 b, INT32 rw, INT32 rh);

	void			DP_RoundRect( INT32 l, INT32 t, INT32 r, INT32 b, INT32 rw, INT32 rh);

	void			ellipse( INT32 l, INT32 t, INT32 r, INT32 b);

	void			DP_Ellipse( INT32 l, INT32 t, INT32 r, INT32 b);

	void			drawBitmap( HDC memDC, RECT * pDest, RECT * pSrc);

	void			penDashStyle( Pen * pPen, GIC_LINE_STYLE dashStyle);

	void			drawBezierLine( GIC_LINE_STYLE dashStyle, REAL32 rWidth, COLORREF color,
						POINT &p1, POINT &p2, POINT &p3, POINT &p4);

	void			drawPolygon( GIC_LINE_STYLE dashStyle, REAL32 rWidth, COLORREF color,
						COLORREF fillColor, POINT * pts, INT32 count, bool bFill = true);

};

#endif	// I3_DEBUG

#endif

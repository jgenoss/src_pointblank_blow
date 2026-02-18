#pragma once

#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#include "i3GICShape.h"
#include "i3GICDiagramContext.h"

#if defined( I3_DEBUG)

#define	I3GIC_CLIPBOARDFORMAT			"I3GIC_CLIPBOARD"

typedef UINT32 I3GIC_POPUP_FLAG;
#define I3GIC_POPUP_FLAG_NO_ALIGN		0x00000001		///< Align 항목을 사용하지 않는다.
#define I3GIC_POPUP_FLAG_NO_DEPTHALIGN	0x00000002		///< Depth Align을 사용하지 않는다.

class i3GICDiagram;

class I3_EXPORT_GIC i3GICDiagramView : public CWnd
{
	DECLARE_DYNAMIC(i3GICDiagramView)
	
	friend	class	i3GICDiagramContext;

protected:
	HCURSOR				m_hCursors[GIC_CURSOR_MAX];			// [initialize at _Init()]
	GIC_MOUSE_CURSOR	m_CurCursor;

	HDC					m_hDrawDC = nullptr;
	HBITMAP				m_hbmpDraw = nullptr, m_hbmpOld = nullptr;

	UINT32				m_ClientWidth;					///< Panel size
	UINT32				m_ClientHeight;					///< panel size
	REAL32				m_rViewPosX, m_rViewPosY;
	REAL32				m_rOffsetX = 0.0f, m_rOffsetY = 0.0f;
	REAL32				m_ViewWidth, m_ViewHeight;
	
	REAL32				m_Zoom;
	REAL32				m_factX, m_factY;

	HBRUSH				m_hbrBack = nullptr;

	i3GICDiagramContext * m_pDiagramCtx;

	GIC_MOUSEMODE		m_PrevMouseMode;
	POINT				m_ScrollStart;

	I3GIC_POPUP_FLAG	m_PopupFlag = 0;

protected:
	void				_Init(void);
	void				_Close(void);
	virtual void		_AdjustView(void);
	void				_SetViewSize( UINT32 cx, UINT32 cy);

	void				_SetCursor( GIC_MOUSE_CURSOR cursor);

	void				_DrawPage( void);
	void				_DrawShapes( RECT * pRect = nullptr);
	void				_DrawSelBox( RECT * pRect);

	/** \breif Clipboard에 저장된 data가 있는지 확인한다.
		\return true : empty, false : used clipboard */
	bool				_CheckClipboardEmpty( const char * pszFormat);

	void				OnUpdate( I3_TDK_UPDATE_INFO * pInfo);

	//////////////////////////////////////////////////////////////////////////////////////////////
	//											Draw functions									//
public:
	HDC					getDrawDC( void)						{ return m_hDrawDC; }
	void				setDrawDC( HDC dc)						{ m_hDrawDC = dc; }

	REAL32				getViewWidth( void)						{ return m_ViewWidth; }
	REAL32				getViewHeight( void)					{ return m_ViewHeight; }
	
	REAL32				getViewPosX( void)						{ return m_rViewPosX + m_rOffsetX; }
	REAL32				getViewPosY( void)						{ return m_rViewPosY + m_rOffsetY; }

	REAL32				getOffsetX( void)						{ return m_rOffsetX; }
	REAL32				getOffsetY( void)						{ return m_rOffsetY; }

	REAL32				getZoom( void)							{ return m_Zoom; }
	REAL32				getFactX( void)							{ return m_factX; }
	REAL32				getFactY( void)							{ return m_factY; }
	
	void				_LP( POINT * pPt)						
	{ 
		::DPtoLP( m_hDrawDC, pPt, 1); 
		pPt->x -= (INT32) (m_rViewPosX + m_rOffsetX);
		pPt->y -= (INT32) (m_rViewPosY + m_rOffsetY);
	}

	void				_LP( RECT * pRect)						
	{ 
		::DPtoLP( m_hDrawDC, (LPPOINT) pRect, 2);
		pRect->left		-= (INT32) (m_rViewPosX + m_rOffsetX);
		pRect->top		-= (INT32) (m_rViewPosY + m_rOffsetY);
		pRect->right	-= (INT32) (m_rViewPosX + m_rOffsetX);
		pRect->bottom	-= (INT32) (m_rViewPosY + m_rOffsetY);
	}

	void				_LP_SIZE( POINT * pPt)
	{ 
		::DPtoLP( m_hDrawDC, pPt, 1); 
	}

	void				_LP_SIZE( RECT * pRect)						
	{ 
		::DPtoLP( m_hDrawDC, (LPPOINT) pRect, 2);	
	}

	void				_DP_SIZE( POINT * pPt)					{ ::LPtoDP( m_hDrawDC, pPt, 1); }
	void				_DP_SIZE( RECT * pRect)					{ ::LPtoDP( m_hDrawDC, (LPPOINT) pRect, 2);	}

	void				_DP( POINT * pPt)						
	{ 
		::LPtoDP( m_hDrawDC, pPt, 1); 
		pPt->x -= (INT32) (m_rViewPosX + m_rOffsetX);
		pPt->y -= (INT32) (m_rViewPosY + m_rOffsetY);
	}

	void				_DP( RECT * pRect)						
	{ 
		::LPtoDP( m_hDrawDC, (LPPOINT) pRect, 2);	
		pRect->left		-= (INT32) (m_rViewPosX + m_rOffsetX);
		pRect->top		-= (INT32) (m_rViewPosY + m_rOffsetY);
		pRect->right	-= (INT32) (m_rViewPosX + m_rOffsetX);
		pRect->bottom	-= (INT32) (m_rViewPosY + m_rOffsetY);
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////
	//								Interface to Diagram Context
	void				setDiagram( i3GICDiagram* pDiagram)
	{
		m_pDiagramCtx->setDiagram( pDiagram);
	}
	i3GICDiagram *		getDiagram( void);

	i3GICDiagramContext *	getDiagramContext( void)		{ return m_pDiagramCtx; }

	bool				isPopupFlag( I3GIC_POPUP_FLAG flag)		{ return (bool)((m_PopupFlag & flag)==flag); }
	void				addPopupFlag( I3GIC_POPUP_FLAG flag)	{ m_PopupFlag |= flag; }
	void				removePopupFlag( I3GIC_POPUP_FLAG flag)	{ m_PopupFlag &= ~flag; }

public:
	i3GICDiagramView();
	virtual ~i3GICDiagramView();

	BOOL				Create( LPCTSTR pszCaption, DWORD dwStyle, const RECT & rect, CWnd * pParentWnd, UINT32 nID);

	GIC_MOUSEMODE		getMouseMode(void)						{ return m_PrevMouseMode; }
	void				SetMouseMode( GIC_MOUSEMODE mode);

	virtual void		OnDraw( void);

	/** \brief Shape 생성시 호출된다.
		\note Override하여 외부 Tool에서 추가 작업을 하는 함수*/
	virtual	bool				OnCreateShape( i3GICShape * pShape) { return true; }

	/** \brief Shape 제거시 호출된다.
		\note Override하여 외부 Tool에서 추가 작업*/
	virtual bool				OnDeleteShape( i3GICShape * pShape)	{ return true; }

	/** \brief Shape 선택시 바로 호출됩니다.
		\note GIC_MOUSEMODE를 설정할 수 있습니다. */
	virtual GIC_MOUSEMODE		OnPreSelectShape( i3GICShape * pShape, GIC_MOUSEMODE mode) { return mode;}

	/** \brief Shape 선택시 호출된다.
		\note Override하여 외부 Tool에서 추가 작업을 하는 함수*/
	virtual void				OnSelectShape( i3GICShape * pShape, GIC_MOUSEMODE mode) {}

	/** \brief Shape 선택 취소시 호출된다.
		\note Override하여 외부 Tool에서 추가 작업을 하는 함수*/
	virtual void				OnUnselectShape( void) {}

	/** \brief Shape Mouse over시 호출된다.
		\note Override하여 외부 Tool에서 추가 작업을 하는 함수*/
	virtual GIC_MOUSE_CURSOR	OnOverlapShape( i3GICShape * pShape, GIC_MOUSE_CURSOR cursor) { return cursor;}

	/** \brief 선택된 Shape의 Mouse Move */
	virtual bool		OnMouseMoveForSeletedShape( i3GICShape * pShape)	{ return true; }

	/** \brief Link용 Shape을 생성하는 함수를 외부 Tool에서 정의하기 위한 함수.
		\note Override하여 외부 Tool에서 추가 작업을 하는 함수*/
	virtual bool		OnMouseMoveForLink( i3GICShape * pShape);

	/** \brief Link Move시 다른 Shape 위에 선택된 경우 호출된다.
		\note 다른 Shape에 Link를 다시 연결하는 기능을 구현한다. */
	virtual bool		OnMouseUpForLink( i3GICShape * pShape) { return true; }

	/** \brief Link line을 생성한다.
		\param[in] pShape Hit된 Shape
		\return i3GICShape 생성된 Link Line		*/
	virtual i3GICShape * OnCreateLinkShape( i3GICShape * pShape) { return nullptr;}

	/** \brief Link Line의 Point 위치를 움직인다.
	*/
	virtual void		OnMoveLinkShape( i3GICShape * pShape) {}

	/** \brief Popup Menu에 Menu를 추가합니다.
		\note 상속된 class에서 추가로 menu를 추가할 수 있는 함수
		*/
	virtual void		OnQueryPopupMenu( CMenu * pMenu) {}

	/** \brief Menu를 선택시 호출되는 함수
		\note 상속 class에서 추가된 menu에 대한 구현 사항을 정의합니다.
		*/
	virtual void		OnSelectPopupMenu( UINT32 cmd) {}

	virtual void		OnBeforeCopyToClipboard(i3::vector<i3PersistantElement*>& pShapeList) {}
	virtual void		OnAfterCopyToClipboard(i3::vector<i3PersistantElement*>& pShapeList) {}

	/** \brief Paste시 ShapeList에서 제외 항목을 외부에서 control할 수 있는 함수*/
	virtual void		OnBeforePasteFromClipboard( i3::vector<i3PersistantElement*>& pShapeList) {}
	virtual void		OnAfterPasteFromClipboard( i3::vector<i3PersistantElement*>& pShapeList) {}
	
	virtual void		OnRButtonUpProcess( UINT nFlags, CPoint point);

	///////////////////////////////////////////////////////////////////////////////////////////////////
	//								for Grid & Magnatic pos
private:
	HPEN				m_hGridPen = nullptr;
	HPEN				m_hCenterPen = nullptr;

	void				__DrawGrid( INT32 x, INT32 y);

public:
	bool				isGridEnable( void)					{ return m_pDiagramCtx->isGridEnable(); }
	void				enableGrid( void)					{ m_pDiagramCtx->setGridEnable( true); }
	void				disableGrid( void)					{ m_pDiagramCtx->setGridEnable( false); }

	bool				isLinkSnapEnable( void)				{ return m_pDiagramCtx->isLinkSnapEnable(); }
	void				enableLinkSnap( void)				{ m_pDiagramCtx->setLinkSnapEnable( true); }
	void				disableLinkSnap( void)				{ m_pDiagramCtx->setLinkSnapEnable( false); }

	bool				isLinkUpdate( void)					{ return m_pDiagramCtx->isLinkUpdate(); }
	void				enableLinkUpdate( void)				{ m_pDiagramCtx->setLinkUpdate( true); }
	void				disableLinkUpdate( void)			{ m_pDiagramCtx->setLinkUpdate( false); }

	bool				isGridSnapEnable( void)				{ return m_pDiagramCtx->isGridSnapEnable(); }
	void				enableGridSnap( void)				{ m_pDiagramCtx->setGridSnapEnable( true); }
	void				disableGridSnap( void)				{ m_pDiagramCtx->setGridSnapEnable( false); }

	INT32				getGridX( void)						{ return m_pDiagramCtx->getGridX(); }
	void				setGridX( INT32 x)					{ m_pDiagramCtx->setGridX( x); }

	INT32				getGridY( void)						{ return m_pDiagramCtx->getGridY(); }
	void				setGridY( INT32 y)					{ m_pDiagramCtx->setGridY( y); }

	INT32				getSnapEps( void)					{ return m_pDiagramCtx->getSnapEps(); }
	void				setSnapEps( INT32 iEps)				{ m_pDiagramCtx->setSnapEps( iEps); }

	bool				isGridSnapPos( INT32 x, INT32 y)	{ return m_pDiagramCtx->IsGridSnapPos( x, y); }

	HBRUSH				getBkBrush(void)					{ return m_hbrBack; }
	void				setBkBrush( HBRUSH hbr)				{ m_hbrBack = hbr; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	

	////////////////////////////////////////////////////////////////////////////
	//						Keyboard input
protected:
	i3InputKeyboard *	m_pKeyboard;

protected:
	void	_ResetKeyinput( void);


	// Version 1.0
private:
	i3ClassMeta *		m_pMetaToCreate;

public:
	void				SetMetaToCreate( i3ClassMeta * pClass);
};

#endif


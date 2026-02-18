#if !defined( __I3_TDK_DOCK_INFO_H)
#define __I3_TDK_DOCK_INFO_H

#define	I3TDK_DOCK_STYLE_HORZ			0x00000001
#define	I3TDK_DOCK_STYLE_FIXED_HORZ		0x00000002			// 수형 방향으로 크기 조절 불가
#define I3TDK_DOCK_STYLE_FIXED_VERT		0x00000004			// 세로 방향으로 크기 조절 불가

#define	I3TDK_DOCK_STATE_DOCKED			0x00000001
#define I3TDK_DOCK_STATE_FLOATED		0x00000002
#define I3TDK_DOCK_STATE_DRAGGING		0x00000004
#define I3TDK_DOCK_STATE_FOLDED			0x00000008
#define I3TDK_DOCK_STATE_HIDE			0x00000010

enum I3TDK_DOCK_SIDE
{
	I3TDK_DOCK_SIDE_NA = 0,
	I3TDK_DOCK_SIDE_LEFT,
	I3TDK_DOCK_SIDE_RIGHT,
	I3TDK_DOCK_SIDE_CENTER
};

class i3TDKDockWnd;

class i3TDKDockContext
{
public:
	INT32		m_X;
	INT32		m_Y;
};

class i3TDKDockInfo : public i3PersistantElement
{
	I3_CLASS_DEFINE( i3TDKDockInfo);
protected:
	UINT32				m_Style;
	UINT32				m_State;

	i3TDKDockInfo *		m_pParent;
	I3TDK_DOCK_SIDE		m_ParentSide;

	i3TDKDockInfo *		m_pChild[2];

	i3TDKDockWnd *			m_pWnd;

	INT32				m_Width, m_Height;
	INT32				m_X,	m_Y;

protected:
	void				Reset(void);
	i3TDKDockInfo *		SplitDown( I3TDK_DOCK_SIDE side1, I3TDK_DOCK_SIDE side2);
	void				AdjustDock( RECT * pRect = NULL);

public:
	i3TDKDockInfo(void);
	virtual ~i3TDKDockInfo(void);

	bool				IsEmpty(void)							
	{ 
		return (m_pChild[0] == NULL) && (m_pChild[1] == NULL) && (m_pWnd == NULL);
	}

	bool				IsStyle( UINT32 mask)					{ return (m_Style & mask) == mask; }
	void				AddStyle( UINT32 style)					{ m_Style |= style; }
	void				RemoveStyle( UINT32 style)				{ m_Style &= ~style; }

	bool				IsState( UINT32 mask)					{ return (m_State & mask) == mask; }
	void				AddState( UINT32 state)					{ m_State |= state; }
	void				RemoveState( UINT32 state)				{ m_State &= ~state; }

	i3TDKDockInfo *		GetParent(void)							{ return m_pParent; }
	void				AddChild( i3TDKDockInfo * pInfo, I3TDK_DOCK_SIDE side);
	void				RemoveChild( i3TDKDockInfo * pInfo);
	void				RemoveAllChild(void);

	i3TDKDockWnd *		GetWindow(void)							{ return m_pWnd; }
	void				SetWindow( i3TDKDockWnd * pWnd);

	void				GetRect( RECT * pRect)
	{
		pRect->left =  m_X;				pRect->top = m_Y;
		pRect->right = m_X + m_Width;	pRect->bottom = m_Y + m_Height;
	}

	INT32				GetWidth(void)							{ return m_Width; }
	void				SetWidth( INT32 w);
	INT32				GetHeight(void)							{ return m_Height; }
	void				SetHeight( INT32 h);

	CWnd *				FindParent(void);
	i3TDKDockInfo *		FindDockInfoByPos( POINT pt);
	I3TDK_DOCK_SIDE		FindDockSide( POINT pt);
	void				AddDockWindow( CWnd * pWnd, char * pszLoc, INT32 width, INT32 height);

	void				OnSize( INT32 cx, INT32 cy);

	virtual void		CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method);
	virtual void		OnBuildObjectList( i3List * pList);
	virtual UINT32		OnSave( i3ResourceFile * pResFile);
	virtual UINT32		OnLoad( i3ResourceFile * pResFile);
};

#endif

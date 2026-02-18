#if !defined( __I3_VIRTUAL_TEXTURE_CACHE_ALLOCATOR_H)
#define __I3_VIRTUAL_TEXTURE_CACHE_ALLOCATOR_H

enum I3VT_AN_CHILD
{
	I3VT_AN_CHILD_NW = 0,
	I3VT_AN_CHILD_NE,
	I3VT_AN_CHILD_SW,
	I3VT_AN_CHILD_SE,

	I3VT_AN_MAX
};

//
// - 각 Node는 사용된 VRAM의 영역을 나타낸다.
struct I3VT_ALLOC_NODE
{
	INT32				m_idxRefTex = -1;

	UINT8				m_R = 0;
	UINT8				m_G = 0;
	UINT8				m_B = 0;

	VEC2D				m_LeftTop;
	VEC2D				m_Size;

	VEC2D				m_SplitPoint;

	I3VT_ALLOC_NODE *	m_pParent = nullptr;
	I3VT_ALLOC_NODE *	m_pChild[I3VT_AN_MAX] = { nullptr, nullptr, nullptr, nullptr };

	void	reset(I3VT_ALLOC_NODE * pParent = nullptr)
	{
		INT32 i;

		m_idxRefTex = -1;

		m_pParent = pParent;

		m_R = m_G = m_B = 0;

		for( i = 0; i < I3VT_AN_MAX; i++)
			m_pChild[i] = nullptr;

	}

	void	getChildArea( I3VT_AN_CHILD area, i3::pack::RECT * pRect)
	{
		switch( area)
		{
			case I3VT_AN_CHILD_NW :
				pRect->left = m_LeftTop.x;
				pRect->top = m_LeftTop.y;
				pRect->right = m_SplitPoint.x - 1.0f;
				pRect->bottom = m_SplitPoint.y - 1.0f;
				break;

			case I3VT_AN_CHILD_NE :
				pRect->left = m_SplitPoint.x;
				pRect->top = m_LeftTop.y;
				pRect->right = (m_LeftTop.x + m_Size.x - 1.0f);
				pRect->bottom = m_SplitPoint.y - 1.0f;
				break;

			case I3VT_AN_CHILD_SW :
				pRect->left = m_LeftTop.x;
				pRect->top = m_SplitPoint.y;
				pRect->right = m_SplitPoint.x - 1.0f;
				pRect->bottom = (m_LeftTop.y + m_Size.y - 1.0f);
				break;

			case I3VT_AN_CHILD_SE :
				pRect->left = m_SplitPoint.x;
				pRect->top = m_SplitPoint.y;
				pRect->right = (m_LeftTop.x + m_Size.x - 1.0f);
				pRect->bottom = (m_LeftTop.y + m_Size.y - 1.0f);
				break;
		}
	}
};

struct I3VT_ALLOC_CTX
{
	INT32	idxRefTex = 0;
	REAL32	w = 0;
	REAL32	h = 0;
	VEC2D	alloced;
};

class I3_EXPORT_FRAMEWORK i3VirtualTextureCache_Allocator : public i3ElementBase
{
	I3_EXPORT_CLASS_DEFINE( i3VirtualTextureCache_Allocator, i3ElementBase);
protected:
	I3VT_ALLOC_NODE *		m_pNodes = nullptr;
	I3VT_ALLOC_NODE *		m_pRoot = nullptr;
	INT32					m_MaxCount = 0;
	INT32					m_Count = 0;
	REAL32					m_Width = 0.0f;
	REAL32					m_Height = 0.0f;

protected:
	I3VT_ALLOC_NODE *		_Alloc( I3VT_ALLOC_NODE * pParent, I3VT_AN_CHILD side, REAL32 cx, REAL32 cy);
	I3VT_ALLOC_NODE *		_CreateChildArea( I3VT_ALLOC_NODE * pParent, I3VT_AN_CHILD side, I3VT_ALLOC_CTX * pCtx);
	I3VT_ALLOC_NODE *		_Rec_LookForFreeArea( I3VT_ALLOC_NODE * pNode, I3VT_ALLOC_CTX * pCtx);

public:
	virtual ~i3VirtualTextureCache_Allocator(void);

	I3VT_ALLOC_NODE *		getRoot(void)				{ return m_pRoot; }

	REAL32					getWidth(void)				{ return m_Width; }
	REAL32					getHeight(void)				{ return m_Height; }

	void				Create( INT32 maxCount = 1024);
	void				SetSpace( REAL32 w, REAL32 h);

	bool				Alloc( REAL32 w, REAL32 h, INT32 idxRefTex, VEC2D * pAlloced);
	void				Reset(void);

};

#endif

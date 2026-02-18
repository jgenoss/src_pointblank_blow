//#if !defined( __I3_UI_FONT_CACHE_H)
//#define __I3_UI_FONT_CACHE_H
//
//#include "i3UIGlyph.h"
//
//#define		NUM_FCN_CHILD		4
//
////Node를 관리하는 방식(Pointer)때문에
////i3MemRealloc에서 문제가 발생하여
////m_pNodes array의 index를 이용하여 Node를 관리하도록 수정( i3UIFontCache2.h)
//
//struct I3UI_FONT_CACHE_NODE
//{
//protected:
//	I3UI_FONT_CACHE_NODE *	m_pChild[NUM_FCN_CHILD];
//
//public:
//	INT32	m_X;
//	INT32	m_Y;
//
//	INT32	m_ID;
//	INT32	m_Depth;
//	i3UIGlyph *				m_pGlyph;
//
//	I3UI_FONT_CACHE_NODE *	m_pParent;
//
//	void	reset(void)
//	{
//		INT32 i;
//
//		m_X = m_Y = 0;
//		
//		m_Depth = 0;
//		
//		m_pGlyph = nullptr;
//		m_pParent = nullptr;
//
//		for( i = 0; i < NUM_FCN_CHILD; i++)
//		{
//			m_pChild[i] = nullptr;
//			
//		}
//	}
//
//	I3UI_FONT_CACHE_NODE *	getChild( INT32 idx)		{ return m_pChild[idx]; }
//	void					setChild( INT32 idx, I3UI_FONT_CACHE_NODE * pChild)
//	{
//		m_pChild[idx] = pChild;
//
//		if( pChild != nullptr)
//			pChild->m_pParent = this;
//	}
//};
//
//class I3_EXPORT_FRAMEWORK i3UIFontCache: public i3ElementBase
//{
//	I3_CLASS_DEFINE( i3UIFontCache);
//protected:
//	I3UI_FONT_CACHE_NODE *	m_pNodes;
//	INT32			m_MaxNodes;
//
//	I3UI_FONT_CACHE_NODE *	m_pFreeHead;
//
//	I3UI_FONT_CACHE_NODE *	m_pRoot;
//
//	INT32					m_SpaceWidth;
//	INT32					m_SpaceHeight;
//
//	List					m_List_NodesToDelete;		// Realloc시 m_pRoot와 포인터연결이 몽땅 유실된다..쓰지 말것..
//														// 여기서는 Delete할것을 쌓아놓는다.
//
//
//protected:
//	void					_ResizeNodes( INT32 cnt);
//
//	void					getSplittedSpace( I3UI_FONT_CACHE_NODE * pNode, i3Rect * pRectSpace, i3Rect * pRects);
//
//	I3UI_FONT_CACHE_NODE *	Alloc( INT32 x, INT32 y, INT32 depth);
//	void					Free( I3UI_FONT_CACHE_NODE * pNode);
//
//	I3UI_FONT_CACHE_NODE *	_Rec_AddGlyph( I3UI_FONT_CACHE_NODE * pNode, i3UIGlyph * pGlyph, i3Rect * pSpace = nullptr);
//
//	void					_Rotate( I3UI_FONT_CACHE_NODE * pNode);
//	void					_LeftRotate( I3UI_FONT_CACHE_NODE * pNode);
//	void					_RightRotate( I3UI_FONT_CACHE_NODE * pNode);
//
//	void					_AddChild( I3UI_FONT_CACHE_NODE * pParent, I3UI_FONT_CACHE_NODE * pChild, INT32 idx, bool bRotate = false);
//public:
//	i3UIFontCache(void);
//	virtual ~i3UIFontCache(void);
//
//	bool						Create( INT32 spaceW, INT32 spaceH);
//	void						Reset(void);
//	void						ChangeSpace( INT32 spaceW, INT32 spaceH);
//
//	I3UI_FONT_CACHE_NODE *		getRoot(void)								{ return m_pRoot; }
//
//	I3UI_FONT_CACHE_NODE *		AddGlyph( i3UIGlyph * pGlyph);
//
//};
//#endif

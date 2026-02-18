#include "i3FrameworkPCH.h"
//
//#include "i3UIFontCache.h"
//
//I3_CLASS_INSTANCE( i3UIFontCache, i3ElementBase);
//
//i3UIFontCache::i3UIFontCache(void)
//{
//	m_pNodes = nullptr;
//	m_MaxNodes = 0;
//
//	m_pFreeHead = 0;
//
//	m_pRoot = nullptr;
//}
//
//i3UIFontCache::~i3UIFontCache(void)
//{
////	I3MEM_SAFE_FREE( m_pNodes);
//
//	const INT32 count = m_List_NodesToDelete.GetCount();
//	for (INT32 i = 0 ; i < count ; ++i)
//	{
//		I3UI_FONT_CACHE_NODE * pNodes = (I3UI_FONT_CACHE_NODE *)m_List_NodesToDelete.Items[count - i - 1];
//		I3MEM_SAFE_FREE( pNodes);
//	}
//
//}
//
//void i3UIFontCache::_ResizeNodes( INT32 cnt)
//{
//	INT32 newSize;
//
//	I3ASSERT( m_MaxNodes < cnt);
//
//	newSize = sizeof(I3UI_FONT_CACHE_NODE) * cnt;
//
//	/*
//	if( m_pNodes == nullptr)
//	{
//		m_pNodes = (I3UI_FONT_CACHE_NODE *) i3MemAlloc( newSize);
//	}
//	else
//	{
//		m_pNodes = (I3UI_FONT_CACHE_NODE *) i3MemReAlloc( m_pNodes, newSize);
//	}
//	*/
//	m_pNodes = (I3UI_FONT_CACHE_NODE *) i3MemAlloc( newSize);
//	m_List_NodesToDelete.Add(m_pNodes);
//		
//	// 새로 추가된 분량의 Node들을 Free List로 연결
//	// Free-List는 Single-Linked-List로 Right-Link를 이용해 연결된다.
///*
//	INT32 i;
//
//	for( i = m_MaxNodes; i < cnt; i++)
//	{
//		m_pNodes[i].reset();
//		m_pNodes[i].m_ID = i;
//
//		m_pNodes[i].setChild( 0, m_pFreeHead);
//
//		m_pFreeHead = & m_pNodes[i];
//	}
//
//	m_MaxNodes = cnt;
//*/
//
//	m_MaxNodes = cnt;
//
//	INT32 i;
//
//	for( i = 0; i < m_MaxNodes; ++i )
//	{
//		m_pNodes[i].reset();
//		m_pNodes[i].m_ID = i;
//
//		m_pNodes[i].setChild( 0, m_pFreeHead);
//
//		m_pFreeHead = & m_pNodes[i];
//	}
//
//	
//}
//
//bool i3UIFontCache::Create( INT32 spaceW, INT32 spaceH)
//{
//	m_SpaceWidth = spaceW;
//	m_SpaceHeight = spaceH;
//
//	// Nodes의 개수는 가변적이다.
//
//	_ResizeNodes( 512) ; //2048);   512로 복귀하고, 리사이징이 돌도록 처리할것..
//
//	return true;
//}
//
//void i3UIFontCache::ChangeSpace( INT32 spaceW, INT32 spaceH)
//{
//	m_SpaceWidth = spaceW;
//	m_SpaceHeight = spaceH;
//}
//
//void i3UIFontCache::Reset(void)
//{
//	// 모든 노드들을 Free List로 옮기고, Root Node를 nullptr로
//
//	INT32 i;
//
//	m_pFreeHead = nullptr;
//
//	for( i = 0; i < m_MaxNodes; i++)
//	{
//		m_pNodes[i].reset();
//
//		m_pNodes[i].setChild( 0, m_pFreeHead);
//
//		m_pFreeHead = & m_pNodes[i];
//	}
//
//	m_pRoot = nullptr;
//}
//
//I3UI_FONT_CACHE_NODE *	i3UIFontCache::Alloc( INT32 x, INT32 y, INT32 depth)
//{
//	if( m_pFreeHead == nullptr)
//	{
//		_ResizeNodes( m_MaxNodes + 512);
//
//		I3ASSERT( m_pFreeHead != nullptr);
//		I3TRACE( "not enough font node...\n");
//	}
//
//	I3UI_FONT_CACHE_NODE * pNode = m_pFreeHead;
//
//	m_pFreeHead = pNode->getChild( 0);
//
//	pNode->setChild( 0, nullptr);
//
//	pNode->m_X		= x;
//	pNode->m_Y		= y;
//	pNode->m_Depth	= depth;
//
//	return pNode;
//}
//
//void i3UIFontCache::Free( I3UI_FONT_CACHE_NODE * pNode)
//{
//	pNode->reset();
//
//	pNode->setChild( 0, m_pFreeHead);
//
//	m_pFreeHead = pNode;
//}
//
//void i3UIFontCache::_AddChild( I3UI_FONT_CACHE_NODE * pParent, I3UI_FONT_CACHE_NODE * pChild, INT32 idx, bool bRotate)
//{
//	pChild->m_pParent = pParent;
//	pParent->setChild( idx, pChild);
//
//	/*
//	if( bRotate)
//	{
//		_Rotate( pChild);
//	}
//	*/
//}
//
//void i3UIFontCache::getSplittedSpace( I3UI_FONT_CACHE_NODE * pNode, i3Rect * pSpace, i3Rect * pRects)
//{
//	// pNode의 분할점은 반드시 pSpace의 영역 안에 포함되어야 한다.
//	// 이 가정이 깨졌다면 버그!!!
//
//	//  +-----+-----+
//	//  |  0  |  1  |
//	//  +-----+-----+
//	//  |  2  |  3  |
//	//  +-----+-----+
//	pRects[0].left		= pSpace->left;
//	pRects[0].top		= pSpace->top;
//	pRects[0].right		= pNode->m_X - 1;
//	pRects[0].bottom	= pNode->m_Y - 1;
//
//	pRects[1].left		= pNode->m_X;
//	pRects[1].top		= pSpace->top;
//	pRects[1].right		= pSpace->right;
//	pRects[1].bottom	= pNode->m_Y - 1;
//
//	pRects[2].left		= pSpace->left;
//	pRects[2].top		= pNode->m_Y;
//	pRects[2].right		= pNode->m_X - 1;
//	pRects[2].bottom	= pSpace->bottom;
//
//	pRects[3].left		= pNode->m_X;
//	pRects[3].top		= pNode->m_Y;
//	pRects[3].right		= pSpace->right;
//	pRects[3].bottom	= pSpace->bottom;
//}
//
//I3UI_FONT_CACHE_NODE *	i3UIFontCache::_Rec_AddGlyph( I3UI_FONT_CACHE_NODE * pNode, i3UIGlyph * pGlyph, i3Rect * pSpace)
//{
//	I3UI_FONT_CACHE_NODE * pNew, * pChild;
//	i3Rect rcCur, rc[NUM_FCN_CHILD];
//	INT32 i, w, h;
//
//	// Glyph가 이미 배치된 Node는 분할 대상이 되어선 안된다.
//	I3ASSERT( pNode->m_pGlyph == nullptr);
//
//	if( pSpace == nullptr)
//	{
//		rcCur.set( 0, 0, m_SpaceWidth - 1, m_SpaceHeight - 1);
//
//		pSpace = &rcCur;
//	}
//
//	getSplittedSpace( pNode, pSpace, rc);
//
//	w = pGlyph->getWidth() + 1;
//	h = pGlyph->getHeight() + 1;
//
//	for( i = 0; i < NUM_FCN_CHILD; i++)
//	{
//		if( rc[i].canContain( w, h))
//		{
//			pChild = pNode->getChild( i);
//
//			if( pChild != nullptr)
//			{
//				if( pChild->m_pGlyph == nullptr)
//				{
//					pNew = _Rec_AddGlyph( pChild, pGlyph, &rc[i]);
//
//					if( pNew != nullptr)
//					{
//						// 제대로 추가했다.
//						return pNew;
//					}
//				}
//
//				// 추가하지 못했다면, 다른 분할 영역에 넣을 수 있는지 확인한다.
//			}
//			else
//			{
//				// 해당 영역에 들어갈 수 있다.
//
//				// 영역이 좀 남는지 확인한다.
//				// 새로운 Node를 만들어 분할해 준다.
//				INT32 spaceCX, spaceCY, idxAdd = i;
//
//				spaceCX = rc[i].right - rc[i].left + 1;
//				spaceCY = rc[i].bottom - rc[i].top + 1;
//
//				if( spaceCX > w || (spaceCY > h))
//				{
//					// 세로 공간이 남는다. 
//					// Horz Split를 추가하여 세로 분할해준다.
//					INT32 sx, sy;
//
//					sx = rc[i].left + w;
//					sy = rc[i].top + h;
//
//					pNew = Alloc( sx, sy, pNode->m_Depth + 1);
//
//					// Link
//					_AddChild( pNode, pNew, i, false);
//
//					// 다시 가로 분할 Node가 추가될 수 있으므로...
//					pNode = pNew;
//
//					idxAdd = 0;
//				}
//
//				// Glyph 추가
//				{
//					pNew = Alloc( rc[i].left, rc[i].top, pNode->m_Depth + 1);
//
//					pNew->m_pGlyph = pGlyph;
//
//					_AddChild( pNode, pNew, idxAdd, false);
//				}
//
//				return pNew;
//			}
//		}
//	}
//
//	return nullptr;
//}
//
//enum UNBALANCE_TYPE
//{
//	UNBALANCE_LL = 0,
//	UNBALANCE_LR,
//	UNBALANCE_RR,
//	UNBALANCE_RL,
//};
//
//void i3UIFontCache::_LeftRotate( I3UI_FONT_CACHE_NODE * pNode)
//{
//
//}
//
//void i3UIFontCache::_RightRotate( I3UI_FONT_CACHE_NODE * pNode)
//{
//
//}
//
//void i3UIFontCache::_Rotate( I3UI_FONT_CACHE_NODE * pNode)
//{
//	//I3UI_FONT_CACHE_NODE * pChild;
//	//UNBALANCE_TYPE type;
//
//	while( pNode != nullptr)
//	{
//		/*
//		if( i3Math::abs( pNode->m_BFactor) >= 2)
//		{
//			if( pNode->m_BFactor <= -2)
//			{
//				pChild = pNode->getLeft();
//
//				if( pChild->m_BFactor < 0)
//					type = UNBALANCE_LL;
//				else
//					type = UNBALANCE_LR;
//			}
//			else
//			{
//				pChild = pNode->getRight();
//				
//				if( pChild->m_BFactor < 0)
//					type = UNBALANCE_RL;
//				else
//					type = UNBALANCE_RR;
//			}
//
//			switch( type)
//			{
//				case UNBALANCE_LL :
//					_RightRotate( pNode);
//					break;
//
//				case UNBALANCE_RR :
//					_LeftRotate( pNode);
//					break;
//
//				case UNBALANCE_LR :
//					_LeftRotate( pChild);
//					_RightRotate( pNode);
//					break;
//
//				case UNBALANCE_RL :
//					_RightRotate( pChild);
//					_LeftRotate( pNode);
//					break;
//			}
//		}
//		*/
//
//		// Parent로....
//		pNode = pNode->m_pParent;
//	}
//}
//
//I3UI_FONT_CACHE_NODE * i3UIFontCache::AddGlyph( i3UIGlyph * pGlyph)
//{
//	if( m_pRoot == nullptr)
//	{
//		// 분할 영역이 전혀 없다.
//		m_pRoot = Alloc( pGlyph->getWidth() + 1, pGlyph->getHeight() + 1, 0);
//	}
//
//	return _Rec_AddGlyph( m_pRoot, pGlyph);
//}
//

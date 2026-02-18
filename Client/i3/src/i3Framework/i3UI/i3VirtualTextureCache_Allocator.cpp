#include "i3FrameworkPCH.h"
#include "i3VirtualTextureCache_Allocator.h"

//I3_CLASS_INSTANCE( i3VirtualTextureCache_Allocator, i3ElementBase);
I3_CLASS_INSTANCE( i3VirtualTextureCache_Allocator);

// Allocator는 Defragment() 함수 호출시 잠깐 쓰고 버리는 곳으로 보관되는 곳은 없다..(Tex 포지셔닝에만 쓰고 아웃됨)
// 툴상에서는 임시로 버추얼텍스쳐에 등록됨..(임시적으로 계속 쓰이며 여기엔 얼로케이터가 없다..)
// AdjustVirtualTexture,  등을 통해서 다시 재배치된다...(얼로케이터는 여기서만 쓰임)
// 이게 파일로 저장되는 것임...(좋지 않은 것 같다...)
//

i3VirtualTextureCache_Allocator::~i3VirtualTextureCache_Allocator(void)
{
	I3MEM_SAFE_FREE( m_pNodes);
}

void i3VirtualTextureCache_Allocator::Create( INT32 maxCount)
{
	m_MaxCount = maxCount;

	m_pNodes = (I3VT_ALLOC_NODE *) i3MemAlloc( sizeof( I3VT_ALLOC_NODE) * m_MaxCount);
	m_Count = 0;

	m_pRoot = nullptr;
}

void i3VirtualTextureCache_Allocator::SetSpace( REAL32 w, REAL32 h)
{
	m_Width = w;
	m_Height = h;

	Reset();
}

I3VT_ALLOC_NODE * i3VirtualTextureCache_Allocator::_Alloc( I3VT_ALLOC_NODE * pParent, I3VT_AN_CHILD side, REAL32 cx, REAL32 cy)
{
	i3::pack::RECT bound;
	I3ASSERT( m_Count < m_MaxCount);

	if( pParent != nullptr)
		pParent->getChildArea( side, &bound);
	else
	{
		bound.left = 0.0f;
		bound.top = 0.0f;
		bound.right = m_Width - 1.0f;
		bound.bottom = m_Height - 1.0f;
	}

	I3VT_ALLOC_NODE * pNew = &m_pNodes[ m_Count];

	pNew->reset( pParent);

	pNew->m_LeftTop.x		= bound.left;
	pNew->m_LeftTop.y		= bound.top;
	pNew->m_Size.x			= bound.Width();
	pNew->m_Size.y			= bound.Height();
	pNew->m_SplitPoint.x	= bound.left + cx;
	pNew->m_SplitPoint.y	= bound.top + cy;

	if( pParent != nullptr)
	{
		pParent->m_pChild[ side] = pNew;
	}

	m_Count++;

	return pNew;
}

I3VT_ALLOC_NODE *	i3VirtualTextureCache_Allocator::_CreateChildArea( I3VT_ALLOC_NODE * pParent, I3VT_AN_CHILD side, I3VT_ALLOC_CTX * pCtx)
{
	i3::pack::RECT bound;
	REAL32 cx, cy, rx, ry, temp;
	I3VT_ALLOC_NODE * pChild, * pLeaf;

	pParent->getChildArea( side, &bound);

	cx = MIN( pCtx->w, bound.Width());
	cy = MIN( pCtx->h, bound.Height());

	rx = bound.Width() - pCtx->w;
	ry = bound.Height() - pCtx->h;

	switch( side)
	{
		case I3VT_AN_CHILD_NE :
			I3ASSERT( rx >= 0.0f);
			break;

		case I3VT_AN_CHILD_SW :
			I3ASSERT( ry >= 0.0f);
			temp = rx;	rx = ry;	ry = temp;
			break;

		case I3VT_AN_CHILD_SE :
			I3ASSERT( rx >= 0.0f);
			I3ASSERT( ry >= 0.0f);
			break;
	}

	{
		pChild = _Alloc( pParent, side, cx, cy);

		if( (rx == 0.0f) && (ry <= 0.0f))
		{
			// 해당 영역을 다 차지한다.
			pChild->m_idxRefTex = pCtx->idxRefTex;
		}
		else
		{
			// 영역을 자른다.
			pLeaf = _Alloc( pChild, I3VT_AN_CHILD_NW, cx, cy);

			pLeaf->m_idxRefTex = pCtx->idxRefTex;
		}

		//I3TRACE( "Alloc texIdx:%d, spaceX:%d, spaceY:%d, dir:%d, w:%d, h:%d\n", pCtx->idxRefTex, (INT32)cx, (INT32)cy, side, (INT32)rx, (INT32)ry);

		if( ry < 0.0f)
		{
			// 영역이 SE 영역까지 필요하다.
			REAL32 oldw = pCtx->w, oldh = pCtx->h;

			if( side == I3VT_AN_CHILD_NE)
				pCtx->h = -ry;
			else
				pCtx->w = -ry;		// <- 위에서 rx와 ry의 값이 swap 되었기 때문에, ry가 맞다.

			I3ASSERT( pParent->m_pChild[I3VT_AN_CHILD_SE] == nullptr);

			_CreateChildArea( pParent, I3VT_AN_CHILD_SE, pCtx);

			pCtx->w = oldw;
			pCtx->h = oldh;
		}

		pCtx->alloced.x = pChild->m_LeftTop.x;
		pCtx->alloced.y = pChild->m_LeftTop.y;
	}

	return pChild;
}

I3VT_ALLOC_NODE * i3VirtualTextureCache_Allocator::_Rec_LookForFreeArea( I3VT_ALLOC_NODE * pNode, I3VT_ALLOC_CTX * pCtx)
{
	REAL32 spaceX, spaceY;
	i3::pack::RECT bound, bound2;
	INT32 i;
	I3VT_ALLOC_NODE * pAllocBlock;
	I3VT_AN_CHILD dir;
	REAL32 w, h;

	if( (pNode->m_Size.x < pCtx->w) || (pNode->m_Size.y < pCtx->h))
		return nullptr;

	INT32		direction[3];

	w = pCtx->w;
	h = pCtx->h;

	{
		// 공간 활용성을 올리기 위해...
		pNode->getChildArea( I3VT_AN_CHILD_NE, &bound);
		pNode->getChildArea( I3VT_AN_CHILD_SW, &bound2);

		if( (bound.Height() < h) && (bound2.Width() >= w) && (bound2.Height() >= h))
		{
			// 오른쪽에 놓이기엔 높이가 크고, 아래쪽에 더 나은 공간이 있을 때를 위한...
			direction[0] = I3VT_AN_CHILD_SW;
			direction[1] = I3VT_AN_CHILD_NE;
			direction[2] = I3VT_AN_CHILD_SE;
		}
		else
		{
			direction[0] = I3VT_AN_CHILD_NE;
			direction[1] = I3VT_AN_CHILD_SW;
			direction[2] = I3VT_AN_CHILD_SE;
		}
	}

	for( i = 0; i < 3; i++)
	{
		dir = (I3VT_AN_CHILD) direction[i];

		if( pNode->m_pChild[ dir] != nullptr)
		{
			//I3TRACE( "RecAlloc -> %d\n", dir);
			pAllocBlock = _Rec_LookForFreeArea( pNode->m_pChild[ dir], pCtx);
			if( pAllocBlock != nullptr)
				return pAllocBlock;
		}
		else
		{
			// 영역이 비었다.
			pNode->getChildArea( dir, &bound);

			spaceX = bound.Width();
			spaceY = bound.Height();

			if( dir != I3VT_AN_CHILD_SE)
			{
				if( pNode->m_pChild[ I3VT_AN_CHILD_SE] == nullptr)
				{
					pNode->getChildArea( I3VT_AN_CHILD_SE, &bound2);

					if( dir == I3VT_AN_CHILD_NE)
						spaceY += bound2.Height();
					else
						spaceX += bound2.Width();
				}
			}

			if((spaceX >= pCtx->w) && (spaceY >= pCtx->h))
			{	
				// NE만으로 할당할 수 있다.
				return _CreateChildArea( pNode, dir, pCtx);
			}
		}
	}

	//I3TRACE( "no alloc..\n");

	return nullptr;
}

bool i3VirtualTextureCache_Allocator::Alloc( REAL32 w, REAL32 h, INT32 idxRefTex, VEC2D * pAlloced)
{
	I3VT_ALLOC_CTX ctx;
	I3VT_ALLOC_NODE * pNew;

	if( m_pRoot == nullptr)
	{
		m_pRoot = _Alloc( nullptr, I3VT_AN_CHILD_NW, w, h);

		pNew = _Alloc( m_pRoot, I3VT_AN_CHILD_NW, w, h);
		pNew->m_idxRefTex = idxRefTex;
		
		*pAlloced = pNew->m_LeftTop;
	}
	else
	{
		ctx.w = w;
		ctx.h = h;
		ctx.idxRefTex = idxRefTex;

		pNew = _Rec_LookForFreeArea( m_pRoot, &ctx);

		if( pNew == nullptr)
			return false;

		*pAlloced = ctx.alloced;
	}

	return true;
}

void i3VirtualTextureCache_Allocator::Reset(void)
{
	m_Count = 0;
	m_pRoot = nullptr;
}

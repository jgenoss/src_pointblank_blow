#include "stdafx.h"
#include "i3TDKDockInfo.h"
#include "i3TDKDockWnd.h"
#include "i3TDKDockFrameWnd.h"

I3_CLASS_INSTANCE( i3TDKDockInfo, i3PersistantElement);

#define		MIN_DOCK		32
#define		SPLITCX			6

i3TDKDockInfo::i3TDKDockInfo(void)
{
	m_Style = 0;
	m_State = 0;

	m_pParent = NULL;
	m_ParentSide = I3TDK_DOCK_SIDE_NA;

	m_pChild[0] = NULL;
	m_pChild[1] = NULL;

	m_pWnd = NULL;
}

i3TDKDockInfo::~i3TDKDockInfo(void)
{
	RemoveAllChild();
}

void i3TDKDockInfo::AddChild( i3TDKDockInfo * pInfo, I3TDK_DOCK_SIDE side)
{
	I3ASSERT( pInfo != NULL);
	I3ASSERT( side != I3TDK_DOCK_SIDE_NA);

	switch( side)
	{
		case I3TDK_DOCK_SIDE_LEFT :
			I3ASSERT( m_pChild[0] == NULL);

			m_pChild[0] = pInfo;
			break;

		case I3TDK_DOCK_SIDE_RIGHT :
			I3ASSERT( m_pChild[1] == NULL);

			m_pChild[1] = pInfo;
			break;
	}

	pInfo->AddRef();
	m_ParentSide = side;
}

void i3TDKDockInfo::RemoveChild( i3TDKDockInfo * pInfo)
{
	if( m_pChild[0] == pInfo)
	{
		m_pChild[0] = NULL;
		pInfo->Release();
	}
	else if( m_pChild[1] == pInfo)
	{
		m_pChild[1] = NULL;
		pInfo->Release();
	}
	else
	{
		I3ASSERT_0;
	}
}

void i3TDKDockInfo::RemoveAllChild(void)
{
	I3_SAFE_RELEASE( m_pChild[0]);
	I3_SAFE_RELEASE( m_pChild[1]);
}

void i3TDKDockInfo::SetWindow( i3TDKDockWnd * pWnd)
{
	m_pWnd = pWnd;
}

CWnd *	i3TDKDockInfo::FindParent(void)
{
	i3TDKDockInfo * pTemp = m_pParent;

	while( pTemp != NULL)
	{
		if( m_pWnd != NULL)
			return m_pWnd;

		pTemp = pTemp->GetParent();
	}

	return NULL;
}

i3TDKDockInfo *	 i3TDKDockInfo::FindDockInfoByPos( POINT pt)
{
	i3TDKDockInfo * pRv = NULL;

	if( m_pChild[0] != NULL)
	{
		pRv = m_pChild[0]->FindDockInfoByPos( pt);
	}

	if((pRv == NULL) && (m_pChild[1] != NULL))
	{
		pRv = m_pChild[1]->FindDockInfoByPos( pt);
	}

	if( pRv == NULL)
	{
		RECT rt;

		GetRect( &rt);

		if( PtInRect( &rt, pt))
			return this;
	}

	return NULL;
}

I3TDK_DOCK_SIDE i3TDKDockInfo::FindDockSide( POINT pt)
{
	RECT rt;
	INT32 step, i;
	static I3TDK_DOCK_SIDE s_SideTbl[4] = 
	{
		I3TDK_DOCK_SIDE_LEFT,
		I3TDK_DOCK_SIDE_CENTER,
		I3TDK_DOCK_SIDE_CENTER,
		I3TDK_DOCK_SIDE_RIGHT
	};

	// 가로의 경우에는 X 좌표를 기준으로, 세로의 경우에는 Y 좌표를 기준으로
	// 전체를 4등분하고, 각 1/4 영역 내의 경우에는 Left, 3/4 영역 내의 경우에는 Right,
	// 가운데에 해당하면 중간으로 반환한다.

	GetRect( &rt);

	if( IsStyle( I3TDK_DOCK_STYLE_HORZ))
	{
		// 가로 방향의 Dock
		step = m_Width >> 2;

		for( i = 0; i < 4; i++)
		{
			rt.right = rt.left + step;

			if( PtInRect( &rt, pt))
				return s_SideTbl[i];

			rt.left += step;
		}
	}
	else
	{
		// 세로 방향의 Dock
		step = m_Height >> 2;

		for( i = 0; i < 4; i++)
		{
			rt.bottom = rt.top + step;

			if( PtInRect( &rt, pt))
				return s_SideTbl[i];

			rt.top += step;
		}
	}

	return I3TDK_DOCK_SIDE_NA;
}

i3TDKDockInfo * i3TDKDockInfo::SplitDown( I3TDK_DOCK_SIDE side1, I3TDK_DOCK_SIDE side2)
{
	i3TDKDockInfo * pNewDock;

	{
		// 자신을 Split하기 위해 기존의 내용을 대피시킨다.
		pNewDock = i3TDKDockInfo::NewObjectRef();
		
		// 자신을 pNewDock으로 복사
		CopyTo( pNewDock, I3_COPY_INSTANCE);

		// 초기화
		Reset();

		if( IsStyle(I3TDK_DOCK_STYLE_HORZ))
		{
			RemoveStyle( I3TDK_DOCK_STYLE_HORZ);
		}
		else
		{
			AddStyle( I3TDK_DOCK_STYLE_HORZ);
		}

		AddChild( pNewDock, side2);
	}

	// 새로운 Dock을 만들어 자신의 Left Child로 등록
	{
		pNewDock = i3TDKDockInfo::NewObjectRef();

		AddChild( pNewDock, side1);
	}

	return pNewDock;
}

void i3TDKDockInfo::AddDockWindow( CWnd * pWnd, char * pszLoc, INT32 width, INT32 height)
{
	i3TDKDockInfo * pTemp = this;
	INT32 idx = 0;
	bool bHorz = false;

	switch( *pszLoc)
	{
		case 'L' :
		case 'l' :		idx = 0;		bHorz = true;		break;
		case 'R' :
		case 'r' :		idx = 1;		bHorz = true;		break;
		case 'U' :
		case 'u' :		idx = 0;		bHorz = false;		break;
		case 'D' :
		case 'd' :		idx = 1;		bHorz = false;		break;

		case 0 :
			// 해당 위치에 추가!
			if( GetWindow() == NULL)
			{
				RECT rt;

				m_pWnd = new i3TDKDockWnd;
				I3ASSERT( m_pWnd != NULL);

				rt.left = 0;		rt.top = 0;
				rt.right = width;	rt.bottom = height;

				m_pWnd->Create( "", WS_CHILD | WS_VISIBLE, rt, g_pDockFrameWnd, (UINT) m_pWnd);
			}

			m_pWnd->AddPane( pWnd, "sdf");
			return;
	}

	if( IsEmpty())
	{
		// 만약 빈 DockInfo라면 가로/세로 Style을 자유롭게 변경할 수 있는
		// 상태이기 때문에...

		if( bHorz)
			AddStyle( I3TDK_DOCK_STYLE_HORZ);
		else
			RemoveStyle( I3TDK_DOCK_STYLE_HORZ);
	}

	if( IsStyle( I3TDK_DOCK_STYLE_HORZ) == bHorz)
	{
		// 원하는 방향과 현재 자신의 Style이 같다면..
		// 단순 추가.
		if( m_pChild[ idx] == NULL)
		{
			m_pChild[ idx] = i3TDKDockInfo::NewObject();
		}

		pTemp = m_pChild[ idx];
	}
	else
	{
		// 요구되는 방향과 자신의 Style이 같지 않다면...
		// 새로운 Dock Info를 생성해, Tree를 Split Down한다.
		if( idx == 0)
			pTemp = SplitDown( I3TDK_DOCK_SIDE_LEFT, I3TDK_DOCK_SIDE_RIGHT);
		else
			pTemp = SplitDown( I3TDK_DOCK_SIDE_RIGHT, I3TDK_DOCK_SIDE_LEFT);
	}

	if( pTemp != NULL)
	{
		pTemp->AddDockWindow( pWnd, pszLoc + 1, width, height);
	}
}

void i3TDKDockInfo::Reset(void)
{
	m_Style = 0;
	m_State = 0;

	if( m_pChild[0] != NULL)
	{
		RemoveChild( m_pChild[0]);
		m_pChild[0] = NULL;
	}

	if( m_pChild[1] != NULL)
	{
		RemoveChild( m_pChild[1]);
		m_pChild[1] = NULL;
	}

	m_pWnd = NULL;
}

void i3TDKDockInfo::CopyTo( i3ElementBase * pDest, I3_COPY_METHOD method)
{
	i3PersistantElement::CopyTo( pDest, method);

	if( pDest->IsTypeOf( i3TDKDockInfo::GetClassMeta()) == FALSE)
		return;

	i3TDKDockInfo * pDock = (i3TDKDockInfo *) pDest;

	pDock->m_Style = m_Style;
	pDock->m_State = m_State;
	pDock->m_Width = m_Width;
	pDock->m_Height = m_Height;
	pDock->m_X = m_X;
	pDock->m_Y = m_Y;
	
	if( m_pChild[0] != NULL)
		pDock->AddChild( m_pChild[0], I3TDK_DOCK_SIDE_LEFT);

	if( m_pChild[1] != NULL)
		pDock->AddChild( m_pChild[1], I3TDK_DOCK_SIDE_RIGHT);

	pDock->m_pWnd = m_pWnd;
}

// 이미 각 DockBar들의 크기가 적절하게 설정된 것으로 판단하고 아래의
// 원칙에 따라 위치를 재정렬한다.
//
// 1. 배치의 주도권은 항상 Parent가 가진다.
// 2. Parent는 Child들에게 크기를 Query하고, 그 크기를 방향 및 위치에
//	  맞게 배치한다.
// 3. 얻어진 각 Bar의 크기가 전체 영역보다 크다면, 아래의 우선 순위에
//    따라 배치한다.
//		- Left
//		- Right
//		- Window
//
void i3TDKDockInfo::AdjustDock( RECT * pRect)
{
	RECT rcLeft, rcRight, rcWnd, rc;
	INT32 span, MaxCX, MaxCY;

	if( pRect == NULL)
	{
		rc.left = 0;		rc.top = 0;
		rc.right = m_Width;	rc.bottom = m_Height;
	}
	else
	{
		rc = *pRect;
	}

	MaxCX = rc.right - rc.left;
	MaxCY = rc.bottom - rc.top;

	if( m_pChild[0] != NULL)
	{
		// Left/Up Side
		SetRect( &rcLeft, 0, 0, m_pChild[0]->GetWidth(), m_pChild[0]->GetHeight());
	}
	else
	{
		SetRect( &rcLeft, 0, 0, 0, 0);
	}

	if( m_pChild[1] != NULL)
	{
		// Right/Down Side
		//SetRect( &rcLeft, 0, 0, m_pChild[1]->GetWindow(), m_pChild[1]->GetHeight());
	}
	else
	{
		SetRect( &rcLeft, 0, 0, 0, 0);
	}

	if( IsStyle( I3TDK_DOCK_STYLE_HORZ))
	{
		// Left, Right, Window(Minimum)의 크기를 모두 더한 크기가
		// 제시된 크기인 rc.Width 보다 작다면 재조정한다.
		if( GetWindow() == NULL)
			span = rcLeft.right + rcRight.right + SPLITCX;
		else
			span = rcLeft.right + rcRight.right + MIN_DOCK + (SPLITCX << 1);

		if( span > MaxCX)
			rcRight.right -= span - MaxCX;

		rcLeft.bottom = MaxCY;
		rcRight.bottom = MaxCY;

		OffsetRect( &rcLeft, rc.left, rc.top);
		//OffsetRect( &rcRight, MaxCX - rcRight.right);

		rcWnd.left = rcLeft.right + SPLITCX;
		rcWnd.top = rcLeft.top;
		rcWnd.right = rcRight.right - SPLITCX;
		rcWnd.bottom = rcLeft.bottom;
	}
	else
	{
		// Up, Down, Window(Minimum)의 크기를 모두 더한 크기가 제시된
		// 크기인 rc.Height 보다 작다면 재조정한다.
		span = rcLeft.bottom + rcRight.bottom + MIN_DOCK;
		if( span > MaxCY)
			rcRight.bottom -= span - MaxCY;

		rcLeft.right = MaxCX;
		rcRight.right = MaxCX;

		OffsetRect( &rcLeft, rc.left, rc.top);
	}

	// 각 위치 조정
	if( m_pChild[0] != NULL)
	{
		
		m_pChild[0]->AdjustDock( &rcLeft);
	}

	if( m_pChild[1] != NULL)
	{
		
		//OffsetRect( &rcRight, MaxCX - (rcLeft.
	}
}

void i3TDKDockInfo::OnSize( INT32 cx, INT32 cy)
{
	m_Width = cx;
	m_Height = cy;
}

void i3TDKDockInfo::OnBuildObjectList( i3List * pList)
{
	i3PersistantElement::OnBuildObjectList( pList);

	if( m_pChild[0] != NULL)
		m_pChild[0]->OnBuildObjectList( pList);

	if( m_pChild[1] != NULL)
		m_pChild[1]->OnBuildObjectList( pList);
}

#pragma pack( push, 4)

typedef struct _tagi3TDKPersistDockInfo
{
	OBJREF		m_pParent;
	UINT32		m_ParentSide;
	OBJREF		m_pWnd;
	UINT32		m_WndID;
	INT8		pad[32];
} I3TDK_PERSIST_DOCK_INFO;

#pragma pack( pop)

UINT32 i3TDKDockInfo::OnSave( i3ResourceFile * pResFile)
{
	return 0;
}

UINT32 i3TDKDockInfo::OnLoad( i3ResourceFile * pResFile)
{
	return 0;
}

#include "stdafx.h"
#include "i3LevelViewportControl.h"
#include "i3LevelCamera.h"
#include "i3LevelViewport.h"
#include "i3LevelGlobalVariable.h"

//I3_ABSTRACT_CLASS_INSTANCE( i3LevelViewportControl, i3ElementBase);
I3_CLASS_INSTANCE( i3LevelViewportControl);

static i3LevelAxis *		s_pDefaultAxis = nullptr;
static HCURSOR				s_hCursor = nullptr;
/*
struct RAII_s_pDefaultAxis
{
	~RAII_s_pDefaultAxis()
	{
		I3_SAFE_RELEASE(s_pDefaultAxis);			// 디버깅을 위해, 코드삽입했는데...버그성 처리였음..
	}												// 이와 같은 RAII가 통하지 않는다...(Direct3DDevice가 유효해야하는데..
													// 이것은 Main()함수 내부에서 처리되므로, 함수바깥에서 삭제는 안된다..
} _RAII_s_pDefaultAxis_;							// (2012.09.13.수빈)
*/
// 유니크하게 잘 처리하려면 at_delay_exit()와 유사하게, at_d3d_device_exit()를 구성해서 거기에 넣고 처리하는게 
// 더 나을것 같다..(2012.09.13.수빈)
namespace
{
	void at_d3d_exit_s_pDefaultAxis()
	{
		I3_SAFE_RELEASE(s_pDefaultAxis);	
	}
}


i3LevelViewportControl::i3LevelViewportControl(void)
{
	if( s_pDefaultAxis == nullptr)
	{
		s_pDefaultAxis	= i3LevelAxis::new_object();
		s_pDefaultAxis->Create();
		i3::at_3d_device_exit(&at_d3d_exit_s_pDefaultAxis);
		s_hCursor = ::LoadCursor( nullptr, IDC_ARROW );
	}
}

i3LevelViewportControl::~i3LevelViewportControl(void)
{
	I3_SAFE_RELEASE( m_pAxis);

	if( m_hCursor != nullptr)
	{
		DestroyCursor( m_hCursor);
		m_hCursor = nullptr;
	}
}

void i3LevelViewportControl::Create(void)
{
}

void i3LevelViewportControl::setAxis( i3LevelAxis * pAxis)
{
	I3_REF_CHANGE( m_pAxis, pAxis);

	if( m_pAxis != nullptr)
	{
		i3Matrix::Identity( m_pAxis->GetTransform());
	}
}

i3LevelAxis *	i3LevelViewportControl::GetDefaultAxis(void)
{
	return s_pDefaultAxis;
}

HCURSOR i3LevelViewportControl::GetDefaultCursor(void)
{
	return s_hCursor;
}

void i3LevelViewportControl::OnUpdate( REAL32 tm)
{
	if( m_pAxis != nullptr)
	{
		m_pAxis->OnUpdate( tm);
	}
}

bool i3LevelViewportControl::OnLButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{	
	return false;
}

bool i3LevelViewportControl::OnLButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	return false;
}

bool i3LevelViewportControl::OnMButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	return false;
}

bool i3LevelViewportControl::OnMButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	return false;
}

bool i3LevelViewportControl::OnRButtonDown( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	return false;
}

bool i3LevelViewportControl::OnRButtonUp( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	return false;
}

bool i3LevelViewportControl::OnMouseMove( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, CPoint point)
{
	return false;
}

bool i3LevelViewportControl::OnMouseWheel( I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nFlags, short zDelta, CPoint pt)
{
	return false;
}

bool i3LevelViewportControl::OnKeyDown(  I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	return false;
}

static HANDLE		g_hThread = nullptr;

struct SPF_INFO
{
	i3LevelPath *		m_pPath = nullptr;
	i3LevelPathPoint *	m_pStart = nullptr;
	i3LevelPathPoint *	m_pEnd = nullptr;
} ;

SPF_INFO g_Info;

DWORD WINAPI _DoSPF( LPVOID pData)
{
	SPF_INFO * pInfo = (SPF_INFO *) pData;
	i3LevelPathPoint * Table[100];

	INT32 cnt = pInfo->m_pPath->FindShortestPath( pInfo->m_pStart, pInfo->m_pEnd, Table, 100);

	I3TRACE( "COUNT : %d\n", cnt);

	return cnt;
}

static void _Clear(void)
{
	i3::vector<i3LevelElement3D*> list;
		
	g_pScene->FindElementByType( list, i3LevelPathPoint::static_meta());

	for(size_t i = 0; i < list.size(); i++)
	{
		i3LevelPathPoint * p = (i3LevelPathPoint *) list[i];

		p->Untargeted();
	}
}

bool i3LevelViewportControl::OnKeyUp(  I3_LEVEL_EDIT_CONTEXT * pCtx, UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// Debug
	if( nChar == '5')
	{
		INT32 i;
		i3::vector<i3LevelElement3D*> list;
		i3LevelPath * pPath = nullptr;

		{
			g_pScene->FindElementByType( list, i3LevelPath::static_meta());

			if( list.size() <= 0)				// 사이즈는 현재 음수개념이 없다..버그로 판단되며 < 대신 <= 을 넣는다...(2012.05.16.수빈)
				return false;

			pPath = (i3LevelPath *) list[0];
		}

		{
			list.clear();
			g_pScene->GetSelectedElements( list);

			if( list.size() != 2)
				return false;
		}

		i3LevelElement3D * pObj1, * pObj2;

		pObj1 = list[0];
		pObj2 = list[1];

		if( !i3::same_of<i3LevelPathPoint* >(pObj1)
			|| !i3::same_of<i3LevelPathPoint* >(pObj2))
			return false;

		i3LevelPathPoint * p1, * p2;

		p1 = (i3LevelPathPoint *) pObj1;
		p2 = (i3LevelPathPoint *) pObj2;

		_Clear();

		#if defined( __THREAD__)
		{
			if( g_hThread != nullptr)
			{
				::TerminateThread( g_hThread, 0);
			}

			DWORD id;

			g_Info.m_pPath = pPath;
			g_Info.m_pStart = p1;
			g_Info.m_pEnd = p2;

			g_hThread = ::CreateThread( nullptr, 0, _DoSPF, &g_Info, 0, &id);
		}
		#else
		{
			i3LevelPathPoint * Table[100];

			INT32 cnt = pPath->FindShortestPath( p1, p2, Table, 100);

			I3TRACE( "COUNT : %d\n", cnt);

			if( cnt > 0)
			{
				p1->SetMaterialDiffuse( 0.0f, 0.0f, 1.0f, 1.0f);
				p2->SetMaterialDiffuse( 1.0f, 0.0f, 0.0f, 1.0f);

				REAL32 t = 0.0f, step = 1.0f / cnt;

				for( i = 0; i < cnt; i++)
				{
					i3LevelPathPoint * p = Table[i];

					p->SetMaterialDiffuse( t, 0.0f, 1.0f - t, 1.0f);

					t += step;
				}
			}
			else
			{
				p1->SetMaterialDiffuse( 0.0f, 0.0f, 0.0f, 1.0f);
				p2->SetMaterialDiffuse( 0.0f, 0.0f, 0.0f, 1.0f);
			}
		}
		#endif
	}
	else if( nChar == '6')
	{
		_Clear();
	}

	return false;
}

bool i3LevelViewportControl::OnStart(  I3_LEVEL_EDIT_CONTEXT * pCtx)
{
	return true;
}

bool i3LevelViewportControl::OnEnd(  I3_LEVEL_EDIT_CONTEXT * pCtx)
{
	return false;
}

bool i3LevelViewportControl::OnPopupMenu( I3_LEVEL_EDIT_CONTEXT * pCtx, CMenu * pMenu)
{
	return false;
}

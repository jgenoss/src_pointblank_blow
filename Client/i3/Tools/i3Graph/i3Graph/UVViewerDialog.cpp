// UVViewerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "UVViewerDialog.h"
#include ".\uvviewerdialog.h"

// CUVViewerDialog dialog

IMPLEMENT_DYNAMIC(CUVViewerDialog, CDialog)
CUVViewerDialog::CUVViewerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUVViewerDialog::IDD, pParent)
{
	m_hBitmap = NULL;
}

CUVViewerDialog::~CUVViewerDialog()
{
	if( m_hBitmap != NULL)
	{
		DeleteObject( m_hBitmap);
	}
}

void CUVViewerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_UV_VIEW, m_ImageViewCtrl);
}

void CUVViewerDialog::Execute( i3Node  * pNode)
{
	m_pRoot = pNode;

	DoModal();
}

void CUVViewerDialog::DrawUV(void)
{
	HDC hMemDC;
	HBITMAP hOldBitmap;
	HPEN hPen[4];
	INT32 i, j, k;
	INT32 sz;
	i3List GeoAttrList;

	{
		CRect rt;

		GetClientRect( &rt);

		sz = min( (INT32) rt.Width(), (INT32) rt.Height());
	}

	{
		//HDC dc = ::GetDC( NULL);

		//hMemDC = CreateCompatibleDC( dc);

		//::ReleaseDC( NULL, dc);
		hMemDC = ::GetDC( NULL);
	}

	{
		m_hBitmap = CreateBitmap( sz, sz, 1, 32, NULL);
		hOldBitmap = (HBITMAP) SelectObject( hMemDC, m_hBitmap);
	}

	{
		hPen[0] = CreatePen( PS_SOLID, 1, RGB( 255, 0, 0));
		hPen[1] = CreatePen( PS_SOLID, 1, RGB( 0, 255, 0));
		hPen[2] = CreatePen( PS_SOLID, 1, RGB( 0, 0, 255));
		hPen[3] = CreatePen( PS_SOLID, 1, RGB( 0, 128, 128));
	}

	// Clear
	{
		PatBlt( hMemDC, 0, 0, sz, sz, WHITENESS);
	}

	// 존재하는 모든 i3GeometryAttr들을 구한다.
	{
		i3List GeoList;

		i3Scene::FindNodeByType( m_pRoot, i3Geometry::GetClassMeta(), &GeoList);

		for( i = 0; i < GeoList.GetCount(); i++)
		{
			i3Geometry * pGeo = (i3Geometry *) GeoList.Items[i];

			for( j = 0; j < pGeo->GetGeometryAttrCount(); j++)
			{
				i3RenderAttr * pAttr = pGeo->GetGeometryAttr( j);

				if( pAttr->IsExactTypeOf( i3GeometryAttr::GetClassMeta()))
					GeoAttrList.Add( pAttr);
			}
		}
	}

	for( i = 0; i < GeoAttrList.GetCount(); i++)
	{
		i3GeometryAttr * pGeoAttr = (i3GeometryAttr *) GeoAttrList.Items[i];
		i3VertexArray * pVA;
		INT32 _x, _y;
		VEC2D uv;

		pVA = pGeoAttr->GetVertexArray();

		if( pVA->GetFormat()->GetTextureCoordSetCount() <= 0)
			continue;

		pVA->Lock();

		for( j = 0; j < pVA->GetFormat()->GetTextureCoordSetCount(); j++)
		{
			::SelectObject( hMemDC, hPen[j]);

			for( k = 0; k < (INT32) pVA->GetCount(); k++)
			{
				pVA->GetTextureCoord( j, k, &uv);

				_x = (INT32)(sz * i3Math::fmod(i3Vector::GetX( &uv), 1.0f));
				_y = (INT32)(sz * i3Math::fmod(i3Vector::GetY( &uv), 1.0f));

				if( j == 1)
				{
					I3TRACE( "%d, %d\n", _x, _y);
				}

				if( k == 0)
				{
					MoveToEx( hMemDC, _x, _y, NULL);
				}
				else
				{
					LineTo( hMemDC, _x, _y);
				}
			}
		}

		pVA->Unlock();
	}

	//////////////////////
	for( i = 0; i < 4; i++)
	{
		DeleteObject( hPen[i]);
	}

	SelectObject( hMemDC, hOldBitmap);
	::ReleaseDC( NULL, hMemDC);

	m_ImageViewCtrl.SetBitmap( m_hBitmap);
}

BEGIN_MESSAGE_MAP(CUVViewerDialog, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CUVViewerDialog message handlers

BOOL CUVViewerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	DrawUV();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CUVViewerDialog::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if( ::IsWindow( m_ImageViewCtrl.m_hWnd))
	{
		INT32 sz = min( cx, cy) - 6;

		m_ImageViewCtrl.SetWindowPos( NULL, 3, 3, sz, sz, SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

// GeometryToolDialog.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "i3GraphDlg.h"
#include "GeometryToolDialog.h"
#include ".\geometrytooldialog.h"


// CGeometryToolDialog dialog

IMPLEMENT_DYNAMIC(CGeometryToolDialog, CDialog)
CGeometryToolDialog::CGeometryToolDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CGeometryToolDialog::IDD, pParent)
{
}

CGeometryToolDialog::~CGeometryToolDialog()
{
}

void CGeometryToolDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GEOLIST, m_GeoListCtrl);
}

void CGeometryToolDialog::Execute( i3Node * pRoot)
{
	m_pRoot = pRoot;

	DoModal();
}

void CGeometryToolDialog::AddGeometry( i3GeometryAttr * pGeoAttr)
{
	char conv[1024];
	INT32 idx, i;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	INT32 primcount;
	char szPrim[32], szX[32], szY[32], szZ[32];
	VEC3D vmin, vmax;
	i3VertexFormat fmt;

	pVA = pGeoAttr->GetVertexArray();
	pIA = pGeoAttr->GetIndexArray();

	if( pVA != NULL)
		fmt = * pVA->GetFormat();

	pGeoAttr->CalcBoundBox( &vmin, &vmax);

	switch( pGeoAttr->GetPrimitiveType())
	{
		case I3G_PRIM_POINTLIST :		
			strcpy( szPrim, "Points");	

			if( pIA != NULL)
				primcount = pIA->GetCount();
			else
				primcount = pVA->GetCount();
			break;

		case I3G_PRIM_LINELIST :		
			strcpy( szPrim, "Lines");

			if( pIA != NULL)
				primcount = pIA->GetCount() >> 1;
			else
				primcount = pVA->GetCount() >> 1;
			break;

		case I3G_PRIM_LINESTRIP :		
			strcpy( szPrim, "Line Strips");

			if( pGeoAttr->GetPrimitiveLengthArray() == NULL)
			{
				if( pIA != NULL)
					primcount = pIA->GetCount() - 1;
				else
					primcount = pVA->GetCount() - 1;
			}
			else
			{
				primcount = 0;
				for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
				{
					primcount += pGeoAttr->GetPrimitiveLength( i);
				}
			}
			break;

		case I3G_PRIM_TRILIST :			
			strcpy( szPrim, "Triangles");

			if( pIA != NULL)
				primcount = pIA->GetCount() / 3;
			else
				primcount = pVA->GetCount() / 3;
			break;

		case I3G_PRIM_TRISTRIP :
			strcpy( szPrim, "Triangle Strips");	

			if( pGeoAttr->GetPrimitiveLengthArray() == NULL)
			{
				if( pIA != NULL)
					primcount = pIA->GetCount() - 2;
				else
					primcount = pVA->GetCount() - 2;
			}
			else
			{
				primcount = 0;
				for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
				{
					primcount += pGeoAttr->GetPrimitiveLength( i);
				}
			}
			break;

		case I3G_PRIM_TRIFAN :	
			strcpy( szPrim, "Triangle Fan");

			if( pGeoAttr->GetPrimitiveLengthArray() == NULL)
			{
				if( pIA != NULL)
					primcount = pIA->GetCount() - 2;
				else
					primcount = pVA->GetCount() - 2;
			}
			else
			{
				primcount = 0;
				for( i = 0; i < (INT32) pGeoAttr->GetPrimitiveCount(); i++)
				{
					primcount += pGeoAttr->GetPrimitiveLength( i);
				}
			}
			break;

		default :	
			strcpy( szPrim, "Unknown");	
			primcount = 0;
			break;
	}

	// Instance
	sprintf( conv, "%08X", pGeoAttr);

	idx = m_GeoListCtrl.InsertItem( 0, conv, 0);

	m_GeoListCtrl.SetItemData( idx, (DWORD_PTR) pGeoAttr);

	// Primitive Type
	m_GeoListCtrl.SetItemText( idx, 1, szPrim);

	// # Prims
	sprintf( conv, "%d", primcount);
	m_GeoListCtrl.SetItemText( idx, 2, conv);

	// # Vertex
	if( pVA != NULL)
	{
		sprintf( conv, "%d", pVA->GetCount());
		m_GeoListCtrl.SetItemText( idx, 3, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 3, "-");
	}

	// # Index
	if( pIA != NULL)
	{
		sprintf( conv, "%d", pIA->GetCount());
		m_GeoListCtrl.SetItemText( idx, 4, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 4, "-");
	}

	// Position
	if( fmt.GetHasPosition())
	{
		switch( fmt.GetPositionType())
		{
			case I3VF_TYPE_REAL32 :			strcpy( conv, "F 32");	break;
			case I3VF_TYPE_FIXED32_16 :		strcpy( conv, "I 32:16");	break;
			case I3VF_TYPE_FIXED16_8 :		strcpy( conv, "I 16:8");	break;
			case I3VF_TYPE_FIXED16_12 :		strcpy( conv, "I 16:12");	break;
			case I3VF_TYPE_FIXED16_15 :		strcpy( conv, "I 16:15");	break;
			case I3VF_TYPE_FIXED8_4 :		strcpy( conv, "I 8:4");	break;
			case I3VF_TYPE_FIXED8_7 :		strcpy( conv, "I 8:7");	break;
			case I3VF_TYPE_FIXED16_0 :		strcpy( conv, "I 16:0");	break;
			case I3VF_TYPE_FIXED8_0 :		strcpy( conv, "I 8:0");	break;
			case I3VF_TYPE_FIXED16_15_U :	strcpy( conv, "UI 16:15");	break;
			case I3VF_TYPE_FIXED8_7_U :		strcpy( conv, "UI 8:7");	break;
			default :						strcpy( conv, "Unknown");	break;
		}
		m_GeoListCtrl.SetItemText( idx, 5, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 5, "-");
	}

	// Color (format)
	if( fmt.GetHasColor())
	{
		switch( fmt.GetColorType())
		{
			case I3VF_TYPE_COLOR32_RGBA_8888 :	strcpy( conv, "32");	break;
			case I3VF_TYPE_COLOR16_RGBA_1555 :	strcpy( conv, "16:1555");	break;
			case I3VF_TYPE_COLOR16_RGBX_565 :	strcpy( conv, "16:565");	break;
			case I3VF_TYPE_COLOR16_RGBA_4444 :	strcpy( conv, "16:4444");	break;
			default :							strcpy( conv, "Unknown");	break;
		}
		m_GeoListCtrl.SetItemText( idx, 6, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 6, "-");
	}

	// Normal
	if( fmt.GetHasNormal())
	{
		switch( fmt.GetNormalType())
		{
			case I3VF_TYPE_REAL32 :			strcpy( conv, "F 32");	break;
			case I3VF_TYPE_FIXED32_16 :		strcpy( conv, "I 32:16");	break;
			case I3VF_TYPE_FIXED16_8 :		strcpy( conv, "I 16:8");	break;
			case I3VF_TYPE_FIXED16_12 :		strcpy( conv, "I 16:12");	break;
			case I3VF_TYPE_FIXED16_15 :		strcpy( conv, "I 16:15");	break;
			case I3VF_TYPE_FIXED8_4 :		strcpy( conv, "I 8:4");	break;
			case I3VF_TYPE_FIXED8_7 :		strcpy( conv, "I 8:7");	break;
			case I3VF_TYPE_FIXED16_0 :		strcpy( conv, "I 16:0");	break;
			case I3VF_TYPE_FIXED8_0 :		strcpy( conv, "I 8:0");	break;
			case I3VF_TYPE_FIXED16_15_U :	strcpy( conv, "UI 16:15");	break;
			case I3VF_TYPE_FIXED8_7_U :		strcpy( conv, "UI 8:7");	break;
			default :						strcpy( conv, "Unknown");	break;
		}
		m_GeoListCtrl.SetItemText( idx, 7, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 7, "-");
	}

	// Texture Coord.
	if( fmt.GetTextureCoordCount() > 0)
	{
		switch( fmt.GetTextureCoordType())
		{
			case I3VF_TYPE_REAL32 :			strcpy( conv, "F 32");	break;
			case I3VF_TYPE_FIXED32_16 :		strcpy( conv, "I 32:16");	break;
			case I3VF_TYPE_FIXED16_8 :		strcpy( conv, "I 16:8");	break;
			case I3VF_TYPE_FIXED16_12 :		strcpy( conv, "I 16:12");	break;
			case I3VF_TYPE_FIXED16_15 :		strcpy( conv, "I 16:15");	break;
			case I3VF_TYPE_FIXED8_4 :		strcpy( conv, "I 8:4");	break;
			case I3VF_TYPE_FIXED8_7 :		strcpy( conv, "I 8:7");	break;
			case I3VF_TYPE_FIXED16_0 :		strcpy( conv, "I 16:0");	break;
			case I3VF_TYPE_FIXED8_0 :		strcpy( conv, "I 8:0");	break;
			case I3VF_TYPE_FIXED16_15_U :	strcpy( conv, "UI 16:15");	break;
			case I3VF_TYPE_FIXED8_7_U :		strcpy( conv, "UI 8:7");	break;
			default :						strcpy( conv, "Unknown");	break;
		}
		m_GeoListCtrl.SetItemText( idx, 8, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 8, "-");
	}

	// Blend Index
	if( fmt.GetBlendIndexCount() > 0)
	{
		sprintf( conv, "%d", fmt.GetBlendIndexCount());
		m_GeoListCtrl.SetItemText( idx, 9, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 9, "-");
	}

	// Weight
	if( fmt.GetBlendWeightCount() > 0)
	{
		switch( fmt.GetBlendWeightType())
		{
			case I3VF_TYPE_REAL32 :			strcpy( conv, "F 32");		break;
			case I3VF_TYPE_FIXED32_16 :		strcpy( conv, "I 32:16");	break;
			case I3VF_TYPE_FIXED16_8 :		strcpy( conv, "I 16:8");	break;
			case I3VF_TYPE_FIXED16_12 :		strcpy( conv, "I 16:12");	break;
			case I3VF_TYPE_FIXED16_15 :		strcpy( conv, "I 16:15");	break;
			case I3VF_TYPE_FIXED8_4 :		strcpy( conv, "I 8:4");		break;
			case I3VF_TYPE_FIXED8_7 :		strcpy( conv, "I 8:7");		break;
			case I3VF_TYPE_FIXED16_0 :		strcpy( conv, "I 16:0");	break;
			case I3VF_TYPE_FIXED8_0 :		strcpy( conv, "I 8:0");		break;
			case I3VF_TYPE_FIXED16_15_U :	strcpy( conv, "UI 16:15");	break;
			case I3VF_TYPE_FIXED8_7_U :		strcpy( conv, "UI 8:7");	break;
			default :						strcpy( conv, "Unknown");	break;
		}
		m_GeoListCtrl.SetItemText( idx, 10, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 10, "-");
	}

	// Bound Min
	i3String::ftoa( i3Vector::GetX( &vmin), szX);
	i3String::ftoa( i3Vector::GetY( &vmin), szY);
	i3String::ftoa( i3Vector::GetZ( &vmin), szZ);
	sprintf( conv, "%s, %s, %s", szX, szY, szZ);
	m_GeoListCtrl.SetItemText( idx, 11, conv);

	// Bound Max
	i3String::ftoa( i3Vector::GetX( &vmax), szX);
	i3String::ftoa( i3Vector::GetY( &vmax), szY);
	i3String::ftoa( i3Vector::GetZ( &vmax), szZ);
	sprintf( conv, "%s, %s, %s", szX, szY, szZ);
	m_GeoListCtrl.SetItemText( idx, 12, conv);

	// Bound Size
	i3Vector::Sub( &vmin, &vmax, &vmin);
	i3String::ftoa( i3Math::abs( i3Vector::GetX( &vmin)), szX);
	i3String::ftoa( i3Math::abs( i3Vector::GetY( &vmin)), szY);
	i3String::ftoa( i3Math::abs( i3Vector::GetZ( &vmin)), szZ);
	sprintf( conv, "%s, %s, %s", szX, szY, szZ);
	m_GeoListCtrl.SetItemText( idx, 13, conv);
}

BEGIN_MESSAGE_MAP(CGeometryToolDialog, CDialog)
	ON_BN_CLICKED(IDC_BTN_SELECTALL, OnBnClickedBtnSelectall)
	ON_BN_CLICKED(IDC_BTN_CLEARALL, OnBnClickedBtnClearall)
	ON_BN_CLICKED(IDC_BTN_SELINVERSE, OnBnClickedBtnSelinverse)
	ON_BN_CLICKED(IDC_BTN_MAKEVERTEXCOLORGRAYSCALE, OnBnClickedBtnMakevertexcolorgrayscale)
	ON_BN_CLICKED(IDC_BTN_REMOVENORMAL, OnBnClickedBtnRemovenormal)
	ON_BN_CLICKED(IDC_BTN_REMOVEVERTEXCOLOR, OnBnClickedBtnRemovevertexcolor)
	ON_BN_CLICKED(IDC_BTN_MAKEINDEXEDGEOMETRY, OnBnClickedBtnMakeindexedgeometry)
	ON_BN_CLICKED(IDC_BTN_MAKE_TRIANGLE_STRIP, OnBnClickedBtnMakeTriangleStrip)
	ON_BN_CLICKED(IDC_BTN_MAKENONINDEXEDGEOMETRY, OnBnClickedBtnMakenonindexedgeometry)
END_MESSAGE_MAP()


// CGeometryToolDialog message handlers

BOOL CGeometryToolDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_GeoListCtrl.InsertColumn( 0, "Instance", LVCFMT_LEFT, 80);
		m_GeoListCtrl.InsertColumn( 1, "Primitive", LVCFMT_LEFT, 80);
		m_GeoListCtrl.InsertColumn( 2, "# Prims", LVCFMT_LEFT, 53);
		m_GeoListCtrl.InsertColumn( 3, "# Vertex", LVCFMT_LEFT, 58);
		m_GeoListCtrl.InsertColumn( 4, "# Index", LVCFMT_LEFT, 55);
		m_GeoListCtrl.InsertColumn( 5, "Position", LVCFMT_LEFT, 55);
		m_GeoListCtrl.InsertColumn( 6, "Color", LVCFMT_LEFT, 50);
		m_GeoListCtrl.InsertColumn( 7, "Normal", LVCFMT_LEFT, 50);
		m_GeoListCtrl.InsertColumn( 8, "Texture Coord.", LVCFMT_LEFT, 60);
		m_GeoListCtrl.InsertColumn( 9, "Blend Index", LVCFMT_LEFT, 60);
		m_GeoListCtrl.InsertColumn( 10, "Weight", LVCFMT_LEFT, 60);
		m_GeoListCtrl.InsertColumn( 11, "Bound Min", LVCFMT_LEFT, 100);
		m_GeoListCtrl.InsertColumn( 12, "Bound Max", LVCFMT_LEFT, 100);
		m_GeoListCtrl.InsertColumn( 13, "Bound Size", LVCFMT_LEFT, 100);

		m_GeoListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	}

	UpdateAllGeometry();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CGeometryToolDialog::UpdateAllGeometry(void)
{
	m_GeoListCtrl.DeleteAllItems();

	if( m_pRoot != NULL)
	{
		INT32 i, j;
		i3List List;
		i3Geometry * pGeo;
		i3RenderAttr * pAttr;

		i3Scene::FindNodeByType( m_pRoot, i3Geometry::GetClassMeta(), &List);

		for( i = 0; i < List.GetCount(); i++)
		{
			pGeo = (i3Geometry *) List.Items[i];

			for( j = 0; j < pGeo->GetGeometryAttrCount(); j++)
			{
				pAttr = pGeo->GetGeometryAttr( j);

				if( ! pAttr->IsTypeOf( i3GeometryAttr::GetClassMeta()))
					continue;

				AddGeometry( (i3GeometryAttr *) pAttr);
			}
		}

		// i3Skin Node
		List.Clear();
		i3Scene::FindNodeByType( m_pRoot, i3Skin::GetClassMeta(), &List);

		for( i = 0; i < List.GetCount(); i++)
		{
			i3Skin * pSkin = (i3Skin *) List.Items[i];

			for( j = 0; j < (INT32) pSkin->getShapeCount(); j++)
			{
				i3Shape * pShape = (i3Shape *) pSkin->getShape( j);

				for( INT32 k = 0; k < pShape->getGeometryAttrCount(); k++)
				{
					AddGeometry( pShape->getGeometryAttr( k));
				}
			}
		}
	}
}

void CGeometryToolDialog::OnBnClickedBtnSelectall()
{
	INT32 i;

	for( i = 0; i < m_GeoListCtrl.GetItemCount(); i++)
	{
		m_GeoListCtrl.SetCheck( i, TRUE);
	}
}

void CGeometryToolDialog::OnBnClickedBtnClearall()
{
	INT32 i;

	for( i = 0; i < m_GeoListCtrl.GetItemCount(); i++)
	{
		m_GeoListCtrl.SetCheck( i, FALSE);
	}
}

void CGeometryToolDialog::OnBnClickedBtnSelinverse()
{
	INT32 i;

	for( i = 0; i < m_GeoListCtrl.GetItemCount(); i++)
	{
		m_GeoListCtrl.SetCheck( i, !m_GeoListCtrl.GetCheck( i));
	}
}

void CGeometryToolDialog::OnBnClickedBtnMakevertexcolorgrayscale()
{
	INT32 i, j;
	i3GeometryAttr * pGeoAttr;
	i3VertexArray * pVA;
	COLORREAL col;
	REAL32 c, a;

	for( i = 0; i < m_GeoListCtrl.GetItemCount(); i++)
	{
		pGeoAttr = (i3GeometryAttr *) m_GeoListCtrl.GetItemData( i);

		pVA = pGeoAttr->GetVertexArray();

		pVA->Lock();

		for( j = 0; j < (INT32) pVA->GetCount(); j++)
		{
			pVA->GetColor( j, &col);

			a = i3Color::GetA( &col);
			c = MAX( i3Color::GetR( &col), i3Color::GetG( &col));
			c = MAX( i3Color::GetB( &col), c);

			i3Color::Set( &col, c, c, c, a);

			pVA->SetColor( j, &col);
		}

		pVA->Unlock();
	}
}

void CGeometryToolDialog::OnBnClickedBtnRemovenormal()
{ 
	INT32 i;     
	i3GeometryAttr * pGeoAttr;   
	i3VertexArray * pVA; 
 
	for( i = 0; i < m_GeoListCtrl.GetItemCount(); i++) 
	{ 
		pGeoAttr = (i3GeometryAttr *) m_GeoListCtrl.GetItemData( i);   

		pVA = pGeoAttr->GetVertexArray();

		pVA->GetFormat()->SetHasNormal( false);
		pVA->GetPersistFormat()->SetHasNormal( false); 
	}

	UpdateAllGeometry();
}

void CGeometryToolDialog::OnBnClickedBtnRemovevertexcolor()
{
	INT32 i;
	i3GeometryAttr * pGeoAttr; 
	i3VertexArray * pVA;

	for( i = 0; i < m_GeoListCtrl.GetItemCount(); i++)
	{
		pGeoAttr = (i3GeometryAttr *) m_GeoListCtrl.GetItemData( i);
 
		pVA = pGeoAttr->GetVertexArray();

		pVA->GetFormat()->SetHasColor( false);
		pVA->GetPersistFormat()->SetHasColor( false);
	}

	UpdateAllGeometry();
}

void CGeometryToolDialog::OnBnClickedBtnMakeindexedgeometry() 
{
	i3OptBuildIndexedGeometry opt; 

	opt.SetBuildIndex( TRUE);
       
	if( m_pRoot)   
	{ 
		opt.Trace( m_pRoot);  

		UpdateAllGeometry();	   

		if( opt.GetError()->GetResult() == 0)
		{
			g_pMainWnd->Log( I3GRAP_LOG_NORMAL, "Make indexed geometry");

			MessageBox("Done.", "IndexedGeometry", MB_OK);
		}  
		else 
		{
			g_pMainWnd->Log( I3GRAP_LOG_NORMAL, "Can not make indexed geometry");

			char szTemp[256];
			sprintf( szTemp, "Fail.\n\nResult:      %s\nFirst Error: %s", opt.GetError()->GetResultMsg(), opt.GetError()->GetFirstErrorMsg() );
			MessageBox( szTemp, "IndexedGeometry", MB_OK);
		} 
	}
}

void CGeometryToolDialog::OnBnClickedBtnMakenonindexedgeometry()
{
	i3OptBuildIndexedGeometry opt; 

	opt.SetBuildIndex( FALSE);
       
	if( m_pRoot)   
	{ 
		opt.Trace( m_pRoot);  

		UpdateAllGeometry();
	}
}


void CGeometryToolDialog::OnBnClickedBtnMakeTriangleStrip()   
{   
	i3OptBuildTriStrip strip;    
	strip.SetAutoSwitching( TRUE);
	strip.SetStripStitch( TRUE);
	strip.SetStripValidateEnabled( FALSE); 
	strip.SetStripMinSize( MIN_STRIP_SIZE);  

	if( m_pRoot) 
	{   
		strip.Trace( m_pRoot);   

		UpdateAllGeometry();	 

		if( strip.GetError()->GetResult() == 0)        
		{
			g_pMainWnd->Log( I3GRAP_LOG_NORMAL, "Convert strip geometry"); 

			MessageBox("Done.", "TriStrip", MB_OK);
		}
		else
		{
			g_pMainWnd->Log( I3GRAP_LOG_NORMAL, "Can not convert strip geometry");

			char szTemp[256];
			sprintf( szTemp, "Fail.\n\nResult:      %s\nFirst Error: %s", strip.GetError()->GetResultMsg(), strip.GetError()->GetFirstErrorMsg() );
			MessageBox( szTemp, "TriStrip", MB_OK);
		}		
	} 
}


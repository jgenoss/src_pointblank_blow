// i3TDKDlgGeometryTool.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKDlgGeometryTool.h"
#include ".\i3tdkdlggeometrytool.h"


// i3TDKDlgGeometryTool dialog

IMPLEMENT_DYNAMIC(i3TDKDlgGeometryTool, CDialog)


void i3TDKDlgGeometryTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_LIST_GEO, m_GeoListCtrl);
	DDX_Control(pDX, IDC_TDK_ED_TOTALPRIM, m_ED_TotalPrim);
	DDX_Control(pDX, IDC_TDK_ED_TOTALVERTEX, m_ED_TotalVertex);
	DDX_Control(pDX, IDC_TDK_ED_VACOUNT, m_ED_VACount);
	DDX_Control(pDX, IDC_TDK_ED_IACOUNT, m_ED_IACount);
}

void i3TDKDlgGeometryTool::Execute( i3Node * pRoot)
{
	m_pRoot = pRoot;

	i3TDK::SetResInstance();

	DoModal();

	i3TDK::RestoreResInstance();
}

void i3TDKDlgGeometryTool::AddGeometry( i3GeometryAttr * pGeoAttr)
{
	TCHAR conv[1024];
	INT32 idx, i;
	i3VertexArray * pVA;
	i3IndexArray * pIA;
	INT32 primcount;
	TCHAR szPrim[32];
	VEC3D vmin, vmax;
	i3VertexFormat fmt;

	pVA = pGeoAttr->GetVertexArray();
	pIA = pGeoAttr->GetIndexArray();

	if( pVA != nullptr)
	{
		fmt = * pVA->GetFormat();

		i3::vu::push_back_once(m_VAList, pVA);
	}

	if( pIA != nullptr)
		i3::vu::push_back_once(m_IAList, pIA);

	pGeoAttr->CalcBoundBox( &vmin, &vmax);

	switch( pGeoAttr->GetPrimitiveType())
	{
		case I3G_PRIM_POINTLIST :		
			i3::generic_string_copy( szPrim, _T("Points") );	

			if( pIA != nullptr)
				primcount = pIA->GetCount();
			else
				primcount = pVA->GetCount();
			break;

		case I3G_PRIM_LINELIST :		
			i3::generic_string_copy( szPrim, _T("Lines") );

			if( pIA != nullptr)
				primcount = pIA->GetCount() >> 1;
			else
				primcount = pVA->GetCount() >> 1;
			break;

		case I3G_PRIM_LINESTRIP :		
			i3::generic_string_copy( szPrim, _T("Line Strips") );

			if( pGeoAttr->GetPrimitiveLengthArray() == nullptr)
			{
				if( pIA != nullptr)
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
			i3::generic_string_copy( szPrim, _T("Triangles") );

			if( pIA != nullptr)
				primcount = pIA->GetCount() / 3;
			else
				primcount = pVA->GetCount() / 3;
			break;

		case I3G_PRIM_TRISTRIP :
			i3::generic_string_copy( szPrim, _T("Triangle Strips") );	

			if( pGeoAttr->GetPrimitiveLengthArray() == nullptr)
			{
				if( pIA != nullptr)
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
			i3::generic_string_copy( szPrim, _T("Triangle Fan") );

			if( pGeoAttr->GetPrimitiveLengthArray() == nullptr)
			{
				if( pIA != nullptr)
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
			i3::generic_string_copy( szPrim, _T("Unknown") );	
			primcount = 0;
			break;
	}

	// Instance
	i3::snprintf( conv, 1024, _T("%08X"), pGeoAttr->GetVertexArray());

	idx = m_GeoListCtrl.InsertItem( 0, conv, 0);

	m_GeoListCtrl.SetItemData( idx, (DWORD_PTR) pGeoAttr);

	// Primitive Type
	m_GeoListCtrl.SetItemText( idx, 1, szPrim);

	// # Prims
	i3::snprintf( conv, 1024, _T("%d"), primcount);
	m_GeoListCtrl.SetItemText( idx, 2, conv);

	m_PrimCount += primcount;

	// # Vertex
	if( pVA != nullptr)
	{
		i3::snprintf( conv, 1024, _T("%d"), pVA->GetCount());
		m_GeoListCtrl.SetItemText( idx, 3, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 3, _T("-") );
	}

	// # Index
	if( pIA != nullptr)
	{
		i3::snprintf( conv, 1024, _T("%d"), pIA->GetCount());
		m_GeoListCtrl.SetItemText( idx, 4, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 4, _T("-") );
	}

	// Position
	if( fmt.GetHasPosition())
	{
		switch( fmt.GetPositionType())
		{
			case I3VF_TYPE_REAL32 :			i3::generic_string_copy( conv, _T("F 32") );	break;
			case I3VF_TYPE_FIXED32_16 :		i3::generic_string_copy( conv, _T("I 32:16") );	break;
			case I3VF_TYPE_FIXED16_8 :		i3::generic_string_copy( conv, _T("I 16:8") );	break;
			case I3VF_TYPE_FIXED16_12 :		i3::generic_string_copy( conv, _T("I 16:12") );	break;
			case I3VF_TYPE_FIXED16_15 :		i3::generic_string_copy( conv, _T("I 16:15") );	break;
			case I3VF_TYPE_FIXED8_4 :		i3::generic_string_copy( conv, _T("I 8:4") );	break;
			case I3VF_TYPE_FIXED8_7 :		i3::generic_string_copy( conv, _T("I 8:7") );	break;
			case I3VF_TYPE_FIXED16_0 :		i3::generic_string_copy( conv, _T("I 16:0") );	break;
			case I3VF_TYPE_FIXED8_0 :		i3::generic_string_copy( conv, _T("I 8:0") );	break;
			case I3VF_TYPE_FIXED16_15_U :	i3::generic_string_copy( conv, _T("UI 16:15") );	break;
			case I3VF_TYPE_FIXED8_7_U :		i3::generic_string_copy( conv, _T("UI 8:7") );	break;
			default :						i3::generic_string_copy( conv, _T("Unknown") );	break;
		}
		m_GeoListCtrl.SetItemText( idx, 5, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 5, _T("-") );
	}

	// Color (format)
	if( fmt.GetHasColor())
	{
		switch( fmt.GetColorType())
		{
			case I3VF_TYPE_COLOR32_RGBA_8888 :	i3::generic_string_copy( conv, _T("32") );	break;
			case I3VF_TYPE_COLOR16_RGBA_1555 :	i3::generic_string_copy( conv, _T("16:1555") );	break;
			case I3VF_TYPE_COLOR16_RGBX_565 :	i3::generic_string_copy( conv, _T("16:565") );	break;
			case I3VF_TYPE_COLOR16_RGBA_4444 :	i3::generic_string_copy( conv, _T("16:4444") );	break;
			default :							i3::generic_string_copy( conv, _T("Unknown") );	break;
		}
		m_GeoListCtrl.SetItemText( idx, 6, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 6, _T("-"));
	}

	// Normal
	if( fmt.GetHasNormal())
	{
		switch( fmt.GetNormalType())
		{
			case I3VF_TYPE_REAL32 :			i3::generic_string_copy( conv, _T("F 32") );	break;
			case I3VF_TYPE_FIXED32_16 :		i3::generic_string_copy( conv, _T("I 32:16") );	break;
			case I3VF_TYPE_FIXED16_8 :		i3::generic_string_copy( conv, _T("I 16:8") );	break;
			case I3VF_TYPE_FIXED16_12 :		i3::generic_string_copy( conv, _T("I 16:12") );	break;
			case I3VF_TYPE_FIXED16_15 :		i3::generic_string_copy( conv, _T("I 16:15") );	break;
			case I3VF_TYPE_FIXED8_4 :		i3::generic_string_copy( conv, _T("I 8:4") );	break;
			case I3VF_TYPE_FIXED8_7 :		i3::generic_string_copy( conv, _T("I 8:7") );	break;
			case I3VF_TYPE_FIXED16_0 :		i3::generic_string_copy( conv, _T("I 16:0") );	break;
			case I3VF_TYPE_FIXED8_0 :		i3::generic_string_copy( conv, _T("I 8:0") );	break;
			case I3VF_TYPE_FIXED16_15_U :	i3::generic_string_copy( conv, _T("UI 16:15") );	break;
			case I3VF_TYPE_FIXED8_7_U :		i3::generic_string_copy( conv, _T("UI 8:7") );	break;
			default :						i3::generic_string_copy( conv, _T("Unknown") );	break;
		}
		m_GeoListCtrl.SetItemText( idx, 7, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 7, _T("-") );
	}

	// Texture Coord.
	if( fmt.GetTextureCoordCount() > 0)
	{
		switch( fmt.GetTextureCoordType())
		{
			case I3VF_TYPE_REAL32 :			i3::generic_string_copy( conv, _T("F 32") );	break;
			case I3VF_TYPE_FIXED32_16 :		i3::generic_string_copy( conv, _T("I 32:16") );	break;
			case I3VF_TYPE_FIXED16_8 :		i3::generic_string_copy( conv, _T("I 16:8") );	break;
			case I3VF_TYPE_FIXED16_12 :		i3::generic_string_copy( conv, _T("I 16:12") );	break;
			case I3VF_TYPE_FIXED16_15 :		i3::generic_string_copy( conv, _T("I 16:15") );	break;
			case I3VF_TYPE_FIXED8_4 :		i3::generic_string_copy( conv, _T("I 8:4") );	break;
			case I3VF_TYPE_FIXED8_7 :		i3::generic_string_copy( conv, _T("I 8:7") );	break;
			case I3VF_TYPE_FIXED16_0 :		i3::generic_string_copy( conv, _T("I 16:0") );	break;
			case I3VF_TYPE_FIXED8_0 :		i3::generic_string_copy( conv, _T("I 8:0") );	break;
			case I3VF_TYPE_FIXED16_15_U :	i3::generic_string_copy( conv, _T("UI 16:15") );	break;
			case I3VF_TYPE_FIXED8_7_U :		i3::generic_string_copy( conv, _T("UI 8:7") );	break;
			default :						i3::generic_string_copy( conv, _T("Unknown") );	break;
		}
		m_GeoListCtrl.SetItemText( idx, 8, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 8, _T("-") );
	}

	// Blend Index
	if( fmt.GetBlendIndexCount() > 0)
	{
		i3::snprintf( conv, _countof(conv), _T("%d"), fmt.GetBlendIndexCount());
		m_GeoListCtrl.SetItemText( idx, 9, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 9, _T("-") );
	}

	// Weight
	if( fmt.GetBlendWeightCount() > 0)
	{
		switch( fmt.GetBlendWeightType())
		{
			case I3VF_TYPE_REAL32 :			i3::generic_string_copy( conv, _T("F 32") );		break;
			case I3VF_TYPE_FIXED32_16 :		i3::generic_string_copy( conv, _T("I 32:16") );	break;
			case I3VF_TYPE_FIXED16_8 :		i3::generic_string_copy( conv, _T("I 16:8") );	break;
			case I3VF_TYPE_FIXED16_12 :		i3::generic_string_copy( conv, _T("I 16:12") );	break;
			case I3VF_TYPE_FIXED16_15 :		i3::generic_string_copy( conv, _T("I 16:15") );	break;
			case I3VF_TYPE_FIXED8_4 :		i3::generic_string_copy( conv, _T("I 8:4") );		break;
			case I3VF_TYPE_FIXED8_7 :		i3::generic_string_copy( conv, _T("I 8:7") );		break;
			case I3VF_TYPE_FIXED16_0 :		i3::generic_string_copy( conv, _T("I 16:0") );	break;
			case I3VF_TYPE_FIXED8_0 :		i3::generic_string_copy( conv, _T("I 8:0") );		break;
			case I3VF_TYPE_FIXED16_15_U :	i3::generic_string_copy( conv, _T("UI 16:15") );	break;
			case I3VF_TYPE_FIXED8_7_U :		i3::generic_string_copy( conv, _T("UI 8:7") );	break;
			default :						i3::generic_string_copy( conv, _T("Unknown") );	break;
		}
		m_GeoListCtrl.SetItemText( idx, 10, conv);
	}
	else
	{
		m_GeoListCtrl.SetItemText( idx, 10, _T("-") );
	}
}

void i3TDKDlgGeometryTool::UpdateAllGeometry(void)
{
	m_GeoListCtrl.DeleteAllItems();

	m_PrimCount = 0;
	m_VAList.clear();
	m_IAList.clear();

	if( m_pRoot != nullptr)
	{
		INT32 j;
		i3::vector<i3Node*> List;
		i3Geometry * pGeo;
		i3RenderAttr * pAttr;

		i3Scene::FindNodeByType( m_pRoot, i3Geometry::static_meta(), List);

		for(size_t i = 0; i < List.size(); i++)
		{
			pGeo = (i3Geometry *) List[i];

			for( j = 0; j < pGeo->GetGeometryAttrCount(); j++)
			{
				pAttr = pGeo->GetGeometryAttr( j);

				if( ! i3::kind_of<i3GeometryAttr*>(pAttr))
					continue;

				AddGeometry( (i3GeometryAttr *) pAttr);
			}
		}

		// i3Skin Node
		List.clear();
		i3Scene::FindNodeByType( m_pRoot, i3Skin::static_meta(), List);

		for(size_t i = 0; i < List.size(); i++)
		{
			i3Skin * pSkin = (i3Skin *) List[i];

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

	{
		TCHAR conv[256];

		INT32 vtxCount = 0;

		for(size_t i = 0; i < m_VAList.size(); i++)
		{
			i3VertexArray * pVA = m_VAList[i];

			vtxCount += pVA->GetCount();
		}

		i3::snprintf( conv, _countof(conv), _T("%d"), m_PrimCount);
		m_ED_TotalPrim.SetWindowText( conv);

		i3::snprintf( conv, _countof(conv), _T("%d"), vtxCount);
		m_ED_TotalVertex.SetWindowText( conv);

		i3::snprintf( conv, _countof(conv), _T("%d"), (INT32)m_VAList.size());
		m_ED_VACount.SetWindowText( conv);

		i3::snprintf( conv, _countof(conv), _T("%d"), (INT32)m_IAList.size());
		m_ED_IACount.SetWindowText( conv);
	}
}

BEGIN_MESSAGE_MAP(i3TDKDlgGeometryTool, CDialog)
	ON_BN_CLICKED(IDC_TDK_BTN_REMOVE_COLOR, OnBnClickedTdkBtnRemoveColor)
	ON_BN_CLICKED(IDC_TDK_BTN_REMOVE_NORMAL, OnBnClickedTdkBtnRemoveNormal)
	ON_BN_CLICKED(IDC_TDK_BTN_NONINDEXEDGEO, OnBnClickedTdkBtnNonindexedgeo)
	ON_BN_CLICKED(IDC_TDK_BTN_INDEXEDGEO, OnBnClickedTdkBtnIndexedgeo)
	ON_BN_CLICKED(IDC_TDK_BTN_TRISTRIP, OnBnClickedTdkBtnTristrip)
	ON_BN_CLICKED(IDC_TDK_BTN_TRYLIST, OnBnClickedTdkBtnTrylist)
//	ON_BN_CLICKED(IDC_TDK_BTN_DUMPGEOMETRY, OnBnClickedTdkBtnDumpGeometry)
END_MESSAGE_MAP()


// i3TDKDlgGeometryTool message handlers

BOOL i3TDKDlgGeometryTool::OnInitDialog()
{
	CDialog::OnInitDialog();

	{
		m_GeoListCtrl.InsertColumn( 0, _T("Instance"), LVCFMT_LEFT, 80);
		m_GeoListCtrl.InsertColumn( 1, _T("Primitive"), LVCFMT_LEFT, 80);
		m_GeoListCtrl.InsertColumn( 2, _T("# Prims"), LVCFMT_LEFT, 53);
		m_GeoListCtrl.InsertColumn( 3, _T("# Vertex"), LVCFMT_LEFT, 58);
		m_GeoListCtrl.InsertColumn( 4, _T("# Index"), LVCFMT_LEFT, 55);
		m_GeoListCtrl.InsertColumn( 5, _T("Position"), LVCFMT_LEFT, 55);
		m_GeoListCtrl.InsertColumn( 6, _T("Color"), LVCFMT_LEFT, 50);
		m_GeoListCtrl.InsertColumn( 7, _T("Normal"), LVCFMT_LEFT, 50);
		m_GeoListCtrl.InsertColumn( 8, _T("Texture Coord."), LVCFMT_LEFT, 60);
		m_GeoListCtrl.InsertColumn( 9, _T("Blend Index"), LVCFMT_LEFT, 60);
		m_GeoListCtrl.InsertColumn( 10, _T("Weight"), LVCFMT_LEFT, 60);

		m_GeoListCtrl.SetExtendedStyle( LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	}

	UpdateAllGeometry();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKDlgGeometryTool::OnBnClickedTdkBtnRemoveColor()
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

void i3TDKDlgGeometryTool::OnBnClickedTdkBtnRemoveNormal()
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

void i3TDKDlgGeometryTool::OnBnClickedTdkBtnNonindexedgeo()
{
	i3OptBuildIndexedGeometry opt; 

	opt.SetBuildIndex( FALSE);
       
	if( m_pRoot)   
	{ 
		opt.Trace( m_pRoot);  

		UpdateAllGeometry();
	}
}

void i3TDKDlgGeometryTool::OnBnClickedTdkBtnIndexedgeo()
{
	i3OptBuildIndexedGeometry opt; 

	opt.SetBuildIndex( TRUE);
       
	if( m_pRoot)   
	{ 
		opt.Trace( m_pRoot);  

		UpdateAllGeometry();	   

		if( opt.GetError()->GetResult() == 0)
		{
			MessageBox(_T("Done."), _T("IndexedGeometry"), MB_OK);
		}  
		else 
		{
			TCHAR szTemp[256];
			i3::snprintf( szTemp, _countof(szTemp), _T("Fail.\n\nResult:      %s\nFirst Error: %s"), 
				opt.GetError()->GetResultMsg(), opt.GetError()->GetFirstErrorMsg() );
			MessageBox( szTemp, _T("IndexedGeometry"), MB_OK);
		} 
	}
}

void i3TDKDlgGeometryTool::OnBnClickedTdkBtnTristrip()
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
			MessageBox(_T("Done."), _T("TriStrip"), MB_OK);
		}
		else
		{
			TCHAR szTemp[256];
			i3::snprintf( szTemp, _countof(szTemp), _T("Fail.\n\nResult:      %s\nFirst Error: %s"), 
				strip.GetError()->GetResultMsg(), strip.GetError()->GetFirstErrorMsg() );
			MessageBox( szTemp, _T("TriStrip"), MB_OK);
		}		
	} 
}

void i3TDKDlgGeometryTool::OnBnClickedTdkBtnTrylist()
{
	i3OptBuildTriStrip strip;    
	strip.SetAutoSwitching( TRUE);
	strip.SetStripStitch( TRUE);
	strip.SetStripValidateEnabled( FALSE); 
	strip.SetStripMinSize( MIN_STRIP_SIZE); 
	strip.SetTargetPrimType( I3G_PRIM_TRILIST);

	if( m_pRoot) 
	{   
		strip.Trace( m_pRoot);   

		UpdateAllGeometry();	 

		if( strip.GetError()->GetResult() == 0)        
		{
			MessageBox(_T("Done."), _T("TriStrip"), MB_OK);
		}
		else
		{
			TCHAR szTemp[256];
			i3::snprintf( szTemp, _countof(szTemp), _T("Fail.\n\nResult:      %s\nFirst Error: %s"), 
				strip.GetError()->GetResultMsg(), strip.GetError()->GetFirstErrorMsg() );
			MessageBox( szTemp, _T("TriStrip"), MB_OK);
		}		
	} 
}

void i3TDKDlgGeometryTool::OnBnClickedTdkBtnDumpGeometry()
{
	if( m_pRoot)
	{
		HANDLE hFile = ::CreateFile("D:\\DumpGeometry.txt", GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr );
		if(hFile == INVALID_HANDLE_VALUE)
		{			
			::CloseHandle( hFile );
			return;
		}

		i3::vector<i3RenderAttr*> list;
		i3Scene::FindAttrByExactType( m_pRoot, i3GeometryAttr::static_meta(), list);

		for( UINT32 i = 0; i < list.size(); i++)
		{
			i3GeometryAttr * pGeo = (i3GeometryAttr*) list.at(i);
			if( pGeo->GetVertexArray() != nullptr)
			{
				VEC3D v, n, b, t;
				VEC2D uv;

				{
					i3::string str;

					i3::sprintf(str, "--- (%d) Vertex Arrray \r\n", pGeo->GetVertexArray()->GetCount());
					DWORD Len = str.length();
					::WriteFile(hFile, str.c_str(), Len, (LPDWORD)&Len, nullptr);
				}

				pGeo->GetVertexArray()->Lock();

				for( UINT32 j = 0; j < pGeo->GetVertexArray()->GetCount(); j++)
				{
					pGeo->GetVertexArray()->GetPosition( j, &v);
					pGeo->GetVertexArray()->GetNormal( j, &n);
					pGeo->GetVertexArray()->GetBinormal( j, &b);
					pGeo->GetVertexArray()->GetTangent( j, &t);
					pGeo->GetVertexArray()->GetTextureCoord(0, j, &uv);

					i3::string strP;
					i3::sprintf( strP, "p(%.4f %.4f %.4f)", v.x, v.y, v.z);

					i3::string strN;
					i3::sprintf( strN, "n(%.3f %.3f %.3f)", n.x, n.y, n.z);

					i3::string strB;
					i3::sprintf( strB, "b(%.3f %.3f %.3f)", b.x, b.y, b.z);

					i3::string strT;
					i3::sprintf( strT, "t(%.3f %.3f %.3f)", t.x, t.y, t.z);

					i3::string strUV;
					i3::sprintf( strUV, "uv(%.3f %.3f)", uv.x, uv.y);

					i3::string str;
					i3::sprintf( str, "%d - %s %s %s %s %s\r\n", j, strP, strN, strB, strT, strUV);
					DWORD Len = str.length();
					::WriteFile( hFile, str.c_str(), Len, (LPDWORD)&Len, nullptr ); 
				}

				pGeo->GetVertexArray()->Unlock();
			}
		}

		::CloseHandle( hFile );
	}
}

// i3TDKSpec_Attr_Geometry.cpp : implementation file
//

#include "stdafx.h"
#include "i3TDK.h"
#include "i3TDKSpec_Attr_Geometry.h"
#include ".\i3tdkspec_attr_geometry.h"
#include "i3Base/string/ext/atoi.h"


// i3TDKSpec_Attr_Geometry dialog

IMPLEMENT_DYNAMIC(i3TDKSpec_Attr_Geometry, i3TDKDialogBase)

i3TDKSpec_Attr_Geometry::~i3TDKSpec_Attr_Geometry()
{
	I3_SAFE_RELEASE( m_pAttr);
}

void i3TDKSpec_Attr_Geometry::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TDK_CB_PRIMTYPE, m_CB_PrimType);
	DDX_Control(pDX, IDC_TDK_ED_PRIMCOUNT, m_ED_PrimCount);
	DDX_Control(pDX, IDC_CHK_FORMAT_POS, m_CHK_Pos);
	DDX_Control(pDX, IDC_CHK_FORMAT_NORMAL, m_CHK_Normal);
	DDX_Control(pDX, IDC_CHK_FORMAT_TANGENT, m_CHK_Tangent);
	DDX_Control(pDX, IDC_CHK_FORMAT_BINORMAL, m_CHK_Binormal);
	DDX_Control(pDX, IDC_CHK_FORMAT_COLOR, m_CHK_Color);
	DDX_Control(pDX, IDC_CHK_FORMAT_TRANSFORMED, m_CHK_Transformed);
	DDX_Control(pDX, IDC_TDK_ED_TEXSET, m_ED_TexSet);
	DDX_Control(pDX, IDC_TDK_ED_TEXCOORD, m_ED_TexCoord);
	DDX_Control(pDX, IDC_TDK_ED_BLENDIDX, m_ED_BlendIndex);
	DDX_Control(pDX, IDC_TDK_ED_BLENDWEIGHT, m_ED_BlendWeight);
	DDX_Control(pDX, IDC_TDK_ED_INDEXBUFFER, m_ED_IndexBuffer);
	DDX_Control(pDX, IDC_TDK_ED_STARTIDX, m_ED_StartIndex);
}

void i3TDKSpec_Attr_Geometry::SetObject( i3ElementBase * pObj)
{
	i3TDKDialogBase::SetObject( pObj);

	I3_REF_CHANGE( m_pAttr, ((i3GeometryAttr *) pObj));

	if( m_pAttr != nullptr)
	{
		TCHAR conv[256];

		// Primitive Type
		{
			INT32 idx = -1;

			switch( m_pAttr->GetPrimitiveType())
			{
				case I3G_PRIM_POINTLIST :	idx = 0;		break;
				case I3G_PRIM_LINELIST :	idx = 1;		break;
				case I3G_PRIM_LINESTRIP :	idx = 2;		break;
				case I3G_PRIM_TRILIST :		idx = 3;		break;
				case I3G_PRIM_TRISTRIP :	idx = 4;		break;
				case I3G_PRIM_TRIFAN :		idx = 5;		break;
			}

			m_CB_PrimType.SetCurSel( idx);
		}

		// Primitive Count
		{
			i3::snprintf( conv, _countof(conv), _T("%d"), m_pAttr->GetPrimitiveCount());
			m_ED_PrimCount.SetWindowText( conv);
		}

		// Vertex Format
		{
			i3VertexFormat fmt;
			i3VertexArray * pVA = m_pAttr->GetVertexArray();

			fmt = *pVA->GetFormat();

			// Pos
			if( fmt.GetHasPosition())		m_CHK_Pos.SetCheck( BST_CHECKED);
			else							m_CHK_Pos.SetCheck( BST_UNCHECKED);

			// Normal
			if( fmt.GetHasNormal())			m_CHK_Normal.SetCheck( BST_CHECKED);
			else							m_CHK_Normal.SetCheck( BST_UNCHECKED);

			// Tangent
			if( fmt.GetHasTangent())		m_CHK_Tangent.SetCheck( BST_CHECKED);
			else							m_CHK_Tangent.SetCheck( BST_UNCHECKED);

			// Binormal
			if( fmt.GetHasBinormal())		m_CHK_Binormal.SetCheck( BST_CHECKED);
			else							m_CHK_Binormal.SetCheck( BST_UNCHECKED);

			// Color
			if( fmt.GetHasColor())			m_CHK_Color.SetCheck( BST_CHECKED);
			else							m_CHK_Color.SetCheck( BST_UNCHECKED);

			// Transformed
			if( fmt.IsTransformed())		m_CHK_Transformed.SetCheck( BST_CHECKED);
			else							m_CHK_Transformed.SetCheck( BST_UNCHECKED);

			// Tex Set
			i3::snprintf( conv, _countof(conv), _T("%d"), fmt.GetTextureCoordSetCount());
			m_ED_TexSet.SetWindowText( conv);

			// Tex Coord Count
			i3::snprintf( conv, _countof(conv) - 1, _T("%d"), fmt.GetTextureCoordCount());
			m_ED_TexCoord.SetWindowText( conv);

			// Blend Index
			i3::snprintf( conv, _countof(conv) -1, _T("%d"), fmt.GetBlendIndexCount());
			m_ED_BlendIndex.SetWindowText( conv);

			// Blend Weight
			i3::snprintf( conv, _countof(conv) - 1, _T("%d"), fmt.GetBlendWeightCount());
			m_ED_BlendWeight.SetWindowText( conv);
		}

		// Index Buffer
		{
			i3IndexArray * pIA = m_pAttr->GetIndexArray();

			if( pIA != nullptr)
			{
				i3::snprintf( conv, _countof(conv), _T("%d"), pIA->GetCount());
				m_ED_IndexBuffer.SetWindowText( conv);
			}
			else
			{
				m_ED_IndexBuffer.SetWindowText( _T("(None)") );
			}
		}

		// Start Index
		{
			i3::snprintf( conv, _countof(conv), _T("%d"), m_pAttr->GetStartIndex());
			m_ED_StartIndex.SetWindowText( conv);
		}
	}
	else
	{
		m_CB_PrimType.SetCurSel( -1);
		m_ED_PrimCount.SetWindowText( _T("") );
		m_CHK_Pos.SetCheck( BST_UNCHECKED);
		m_CHK_Normal.SetCheck( BST_UNCHECKED);
		m_CHK_Tangent.SetCheck( BST_UNCHECKED);
		m_CHK_Binormal.SetCheck( BST_UNCHECKED);
		m_CHK_Color.SetCheck( BST_UNCHECKED);
		m_CHK_Transformed.SetCheck( BST_UNCHECKED);
		m_ED_TexSet.SetWindowText( _T("") );
		m_ED_TexCoord.SetWindowText( _T("") );
		m_ED_BlendIndex.SetWindowText( _T("") );
		m_ED_BlendWeight.SetWindowText( _T("") );
		m_ED_IndexBuffer.SetWindowText( _T("") );
		m_ED_StartIndex.SetWindowText( _T("") );
	}

	m_CB_PrimType.EnableWindow( m_pAttr != nullptr);
	m_ED_PrimCount.EnableWindow( m_pAttr != nullptr);
	m_CHK_Pos.EnableWindow( m_pAttr != nullptr);
	m_CHK_Normal.EnableWindow( m_pAttr != nullptr);
	m_CHK_Tangent.EnableWindow( m_pAttr != nullptr);
	m_CHK_Binormal.EnableWindow( m_pAttr != nullptr);
	m_CHK_Color.EnableWindow( m_pAttr != nullptr);
	m_CHK_Transformed.EnableWindow( m_pAttr != nullptr);
	m_ED_TexSet.EnableWindow( m_pAttr != nullptr);
	m_ED_TexCoord.EnableWindow( m_pAttr != nullptr);
	m_ED_BlendIndex.EnableWindow( m_pAttr != nullptr);
	m_ED_BlendWeight.EnableWindow( m_pAttr != nullptr);
	m_ED_IndexBuffer.EnableWindow( m_pAttr != nullptr);
	m_ED_StartIndex.EnableWindow( m_pAttr != nullptr);
}

BEGIN_MESSAGE_MAP(i3TDKSpec_Attr_Geometry, i3TDKDialogBase)
	ON_CBN_SELCHANGE(IDC_TDK_CB_PRIMTYPE, OnCbnSelchangeTdkCbPrimtype)
	ON_EN_KILLFOCUS(IDC_TDK_ED_PRIMCOUNT, OnEnKillfocusTdkEdPrimcount)
	ON_EN_KILLFOCUS(IDC_TDK_ED_STARTIDX, OnEnKillfocusTdkEdStartidx)
END_MESSAGE_MAP()


// i3TDKSpec_Attr_Geometry message handlers

BOOL i3TDKSpec_Attr_Geometry::OnInitDialog()
{
	i3TDKDialogBase::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void i3TDKSpec_Attr_Geometry::OnOK()
{
}

void i3TDKSpec_Attr_Geometry::OnCancel()
{
}

LRESULT i3TDKSpec_Attr_Geometry::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return i3TDKDialogBase::WindowProc(message, wParam, lParam);
}

void i3TDKSpec_Attr_Geometry::OnCbnSelchangeTdkCbPrimtype()
{
	if( m_pAttr == nullptr)
		return;

	switch( m_CB_PrimType.GetCurSel())
	{
		case 0 :		m_pAttr->SetPrimitiveType( I3G_PRIM_POINTLIST);		break;

		case 1 :		m_pAttr->SetPrimitiveType( I3G_PRIM_LINELIST);		break;
		case 2 :		m_pAttr->SetPrimitiveType( I3G_PRIM_LINESTRIP);		break;

		case 3 :		m_pAttr->SetPrimitiveType( I3G_PRIM_TRILIST);		break;
		case 4 :		m_pAttr->SetPrimitiveType( I3G_PRIM_TRISTRIP);		break;
		case 5 :		m_pAttr->SetPrimitiveType( I3G_PRIM_TRIFAN);		break;
	}
}

void i3TDKSpec_Attr_Geometry::OnEnKillfocusTdkEdPrimcount()
{
	if( m_pAttr == nullptr)
		return;

	if( m_ED_PrimCount.GetModify())
	{
		TCHAR conv[256];

		m_ED_PrimCount.GetWindowText( conv, _countof(conv) - 1);

		m_pAttr->SetPrimitiveCount( (UINT32) i3::atoi( conv));

		m_ED_PrimCount.SetModify( FALSE);
	}
}

void i3TDKSpec_Attr_Geometry::OnEnKillfocusTdkEdStartidx()
{
	if( m_pAttr == nullptr)
		return;
}

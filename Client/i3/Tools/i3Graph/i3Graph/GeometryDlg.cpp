// GeometryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "i3Graph.h"
#include "GeometryDlg.h"
#include ".\geometrydlg.h"

// CGeometryDlg dialog

IMPLEMENT_DYNAMIC(CGeometryDlg, CDialog)
CGeometryDlg::CGeometryDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGeometryDlg::IDD, pParent)
{
}

CGeometryDlg::~CGeometryDlg()
{
}

void CGeometryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT2, m_EditString);
}


BEGIN_MESSAGE_MAP(CGeometryDlg, CDialog)
END_MESSAGE_MAP()


// CGeometryDlg message handlers

void CGeometryDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}

void CGeometryDlg::OnCancel()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnCancel();
}

BOOL CGeometryDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	//Information
	SetGeometryInfo();

	//Dump
	DumpGeometry();

	PutText();
	UpdateData( FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

///////////////////////////////////////////////////////////////////////////////////
//SetInfo
void	CGeometryDlg::SetGeometryInfo()
{
	SList.Add("=====================    Geometry Info   =====================");
	SList.Add("");
	SList.Add("");

	SetVertexInfo();
	SetVertexFormat();


	SetIndexInfo();


	SList.Add("");
	SList.Add("");
}

void CGeometryDlg::SetVertexInfo()
{
	i3VertexArray*  pVA = m_pGeomAttr->GetVertexArray();

	char			cBuf[256];
	int				nVertexCount = 0;

	SList.Add("*******************    Vertex Info   *******************");
	nVertexCount = pVA->GetCount();
	sprintf(cBuf,".>Vertex Count : %d",nVertexCount);
	SList.Add(cBuf);
	SList.Add("");
}

void CGeometryDlg::SetIndexInfo()
{
	i3IndexArray*	pIA = m_pGeomAttr->GetIndexArray();

	char			cBuf[256];

	SList.Add("*******************   Index Info   *******************");
	if(pIA == NULL)		sprintf( cBuf, ".>Indexed : No");
	else
	{
		sprintf( cBuf, ".>Indexed    : Yes");							SList.Add(cBuf);
		sprintf( cBuf, ".>IndexCount : %d",pIA->GetCount());
	}
	
	SList.Add(cBuf);
	SList.Add("");

}

///////////////////////////////////////////////////////////////////////////////////////////////
//SetVertexFormat
void	CGeometryDlg::SetVertexFormat()
{
	i3VertexArray*	pVA = m_pGeomAttr->GetVertexArray();	//get vertex array
	i3VertexFormat*	pFormat = pVA->GetPersistFormat();

	SetVertexFormat_Position(pFormat);
	SetVertexFormat_Normal(pFormat);
	SetVertexFormat_Tangent(pFormat);
	SetVertexFormat_Color(pFormat);
	SetVertexFormat_TextureCoord_Count(pFormat);
	SetVertexFormat_TextureCoord_Set_Count(pFormat);
	SetVertexFormat_BlendIndex_Count(pFormat);
	SetVertexFormat_BlendWeight_Count(pFormat);
	SetVertexFormat_PointSprite_SizeCount(pFormat);

	SList.Add("");
}

void	CGeometryDlg::SetVertexFormat_Position(i3VertexFormat*	pFormat)
{
	char			cBuf[256];

	if(pFormat->GetHasPosition())
	{
		sprintf(cBuf , ".> Have Position	:	YES");	
		SetType(pFormat->GetPositionType(),cBuf);
	}
	else
	{
		sprintf(cBuf,".> Have Position	:	NO");			SList.Add(cBuf);
	}
}

void	CGeometryDlg::SetVertexFormat_Normal(i3VertexFormat*	pFormat)
{
	char			cBuf[256];

	if(pFormat->GetHasNormal())
	{
		sprintf(cBuf , ".> Have Normal	:	YES");	
		SetType(pFormat->GetNormalType(),cBuf);
	}
	else
	{
		sprintf(cBuf,  ".> Have Normal	:	NO");		SList.Add(cBuf);
	}
}

void	CGeometryDlg::SetVertexFormat_Tangent(i3VertexFormat*	pFormat)
{
	char			cBuf[256];
	if(pFormat->GetHasTangent())
	{
		sprintf(cBuf , ".> Have Tangent	:	YES");
		SetType(pFormat->GetTangentType(),cBuf);
	}
	else
	{
		sprintf(cBuf,  ".> Have Tangent	:	NO");		SList.Add(cBuf);
	}
}
void	CGeometryDlg::SetVertexFormat_Color(i3VertexFormat*	pFormat)
{
	char			cBuf[256];
	if(pFormat->GetHasColor())
	{
		sprintf(cBuf , ".> Have Color	:	YES");
		SetType(pFormat->GetColorType(),cBuf);
	}
	else
	{
		sprintf(cBuf,  ".> Have Color	:	NO");		SList.Add(cBuf);
	}
}
void	CGeometryDlg::SetVertexFormat_TextureCoord_Count(i3VertexFormat*	pFormat)
{
	char cBuf[256];
	sprintf(cBuf , ".> TextureCoordCount	: %d",pFormat->GetTextureCoordCount());
	
	SetType(pFormat->GetTextureCoordType(),cBuf);
}
void	CGeometryDlg::SetVertexFormat_TextureCoord_Set_Count(i3VertexFormat*	pFormat)
{
	char cBuf[256];
	sprintf(cBuf , ".> TextureCoord_Set_Count	: %d",pFormat->GetTextureCoordSetCount());
	SList.Add(cBuf);

}
void	CGeometryDlg::SetVertexFormat_BlendIndex_Count(i3VertexFormat*	pFormat)
{
	char cBuf[256];
	sprintf(cBuf , ".> Blend Index Count	: %d",pFormat->GetBlendIndexCount());
	SList.Add(cBuf);
}
void	CGeometryDlg::SetVertexFormat_BlendWeight_Count(i3VertexFormat*	pFormat)
{
	char cBuf[256];
	sprintf(cBuf , ".> Blend Weight Count	: %d",pFormat->GetBlendWeightCount());

	SetType(pFormat->GetBlendWeightType(),cBuf);
}

void	CGeometryDlg::SetVertexFormat_PointSprite_SizeCount(i3VertexFormat*	pFormat)
{
}

void	CGeometryDlg::SetType(int nType,char* cBuf)
{
	switch(nType)
	{
		case I3VF_TYPE_REAL32				:	strcat(cBuf , "\tType : REAL32");				break;
//		case I3VF_TYPE_FIXED16_4			:	strcat(cBuf , "\tType : FIXED16 : 4");			break;
		case I3VF_TYPE_FIXED16_8			:	strcat(cBuf , "\tType : FIXED16 : 8");			break;
		case I3VF_TYPE_FIXED16_12			:	strcat(cBuf , "\tType : FIXED16 : 12");		break;
		case I3VF_TYPE_FIXED16_15			:	strcat(cBuf , "\tType : FIXED16 : 15");		break;
		case I3VF_TYPE_FIXED8_4				:	strcat(cBuf , "\tType : FIXED8 : 4");			break;
		case I3VF_TYPE_FIXED8_7				:	strcat(cBuf , "\tType : FIXED8 : 7");			break;
		case I3VF_TYPE_COLOR32_RGBA_8888	:	strcat(cBuf , "\tType : COLOR32_RGBA_8888");	break;
		case I3VF_TYPE_COLOR16_RGBA_1555	:	strcat(cBuf , "\tType : COLOR16_RGBA_1555");	break;
		case I3VF_TYPE_COLOR16_RGBX_565		:	strcat(cBuf , "\tType : COLOR16_RGBX_565");	break;
		case I3VF_TYPE_COLOR16_RGBA_4444	:	strcat(cBuf , "\tType : COLOR16_RGBA_4444");	break;
	}

	SList.Add(cBuf);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
//Dump

void CGeometryDlg::DumpGeometry()
{
	i3VertexArray*	pVA = m_pGeomAttr->GetVertexArray();	//get vertex array
	i3IndexArray*	pIA = m_pGeomAttr->GetIndexArray();		//get vertex array

	SList.Add("=====================  Dump Geometry  =====================");
	SList.Add("");

	DumpVertexArray(pVA);
	DumpNormal(pVA);
	DumpTextureCoord( pVA);
	DumpColor(pVA);
	DumpIndex(pIA);

	SList.Add("");
	SList.Add("=====================  End Dump  =====================");
}


void CGeometryDlg::DumpVertexArray(i3VertexArray*	pVA)
{
	char			cBuf[256];
	VEC3D			Vec;

	int nVertexCount = pVA->GetCount();
	SList.Add("// Vertex Array");
	SList.Add("");

	pVA->Lock();
	for(INT32 i = 0 ;i < nVertexCount ; ++i)
	{
		pVA->GetPosition(i , &Vec);
		sprintf(cBuf , "\t{\t%.4f,\t%.4f,\t%.4f\t},\t\t// %d",i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),
			i3Vector::GetZ(&Vec) ,  i  );
		SList.Add(cBuf);
	}
	pVA->Unlock();
}

void CGeometryDlg::DumpNormal(i3VertexArray*	pVA)
{
	char			cBuf[256];
	VEC3D			Vec;
	int				nVertexCount = pVA->GetCount();

	//normal draw
	if(pVA->HasNormal())
	{
		pVA->Lock();
		SList.Add("");
		SList.Add("// Normal");

		for(INT32 i = 0 ; i < nVertexCount ; ++i)
		{
			pVA->GetNormal(i,&Vec);
			sprintf(cBuf , "\t{\t%.4f,\t%.4f,\t%.4f\t},\t\t// %d",i3Vector::GetX(&Vec),i3Vector::GetY(&Vec),
					i3Vector::GetZ(&Vec) ,  i  );
			SList.Add(cBuf);
		}

		pVA->Unlock();
	}

	SList.Add("");
}

void CGeometryDlg::DumpTextureCoord(i3VertexArray*	pVA)
{
	char			cBuf[256];
	VEC2D			Vec;
	int				nVertexCount = pVA->GetCount();

	//texturecoord draw
	if(pVA->GetFormat()->GetTextureCoordSetCount() > 0)
	{
		pVA->Lock();

		SList.Add("");
		SList.Add("// TextureCoord");

		for(INT32 i = 0 ; i < nVertexCount ; ++i)
		{
			pVA->GetTextureCoord( 0, i,&Vec);
			sprintf(cBuf , "\t{\t%.4f,\t%.4f,\t},\t\t// %d",i3Vector::GetX(&Vec),i3Vector::GetY(&Vec), i  );
			SList.Add(cBuf);
		}

		pVA->Unlock();
	}

	SList.Add("");
}

void CGeometryDlg::DumpColor(i3VertexArray*	pVA)
{
	char			cBuf[256];
	int				nVertexCount = pVA->GetCount();


	if(pVA->HasColor())
	{
		pVA->Lock();
		SList.Add("");
		SList.Add("// Color");

		COLORREAL Color;

		for(INT32 i = 0 ; i < nVertexCount ; ++i)
		{
			pVA->GetColor(i,&Color);
			sprintf( cBuf, "\t{\t%.2f,\t%.2f,\t%.2f,\t%.2f\t},\t\t// %d", 
										i3Color::GetR(&Color), 
										i3Color::GetR(&Color),
										i3Color::GetB( &Color), 
										i3Color::GetA( &Color),
										i);
			SList.Add(cBuf);
		} 
		pVA->Unlock();
	}

	SList.Add(""); 
}  
 
void CGeometryDlg::DumpIndex( i3IndexArray * pIA)		//	Ãß°¡ - KOMET
{ 
	if( pIA == NULL)	return;

	char			cBuf[256]; 
	UINT16			Idx;
	int				nCount = pIA->GetCount();

	//index draw
	if(nCount > 0)
	{
		pIA->Lock();

		SList.Add("");
		SList.Add("// Index");

		for(INT32 i = 0 ; i < nCount ; ++i)
		{
			Idx = pIA->GetIndex( i);
			sprintf(cBuf , "\t{\t%d\t},\t\t// %d",Idx, i  );
			SList.Add(cBuf);
		}
 
		pIA->Unlock();
	}

	SList.Add("");	
}

/////////////////////////////////////////////////////////////////////////////////////////
//Put Text
void CGeometryDlg::PutText()
{
	INT32 Size;
	char * pTemp;

	Size = SList.GetBufferSize();
	pTemp = new char[ Size];

	SList.CopyToBuffer( pTemp, Size);

	SList.Clear();

	m_EditString.SetWindowText( pTemp);

	delete pTemp;
}


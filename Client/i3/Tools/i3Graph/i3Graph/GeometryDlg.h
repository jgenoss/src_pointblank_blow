#pragma once

#include "afxwin.h"
// CGeometryDlg dialog

class CGeometryDlg : public CDialog
{
	DECLARE_DYNAMIC(CGeometryDlg)

public:
	CGeometryDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGeometryDlg();

public:
	i3GeometryAttr*	m_pGeomAttr;

protected:
	i3StringList	SList;

private:
	//info
	void	SetGeometryInfo();

	void	SetVertexInfo();
	void	SetIndexInfo();

	//vertexformat
	void	SetVertexFormat();

	void	SetVertexFormat_Position(i3VertexFormat* pFormat);
	void	SetVertexFormat_Normal(i3VertexFormat* pFormat);
	void	SetVertexFormat_Tangent(i3VertexFormat* pFormat);
	void	SetVertexFormat_Color(i3VertexFormat* pFormat);
	void	SetVertexFormat_TextureCoord_Count(i3VertexFormat* pFormat);
	void	SetVertexFormat_TextureCoord_Set_Count(i3VertexFormat* pFormat);
	void	SetVertexFormat_BlendIndex_Count(i3VertexFormat* pFormat);
	void	SetVertexFormat_BlendWeight_Count(i3VertexFormat* pFormat);
	void	SetVertexFormat_PointSprite_SizeCount(i3VertexFormat* pFormat);

	//dump
	void	DumpGeometry();

	void	DumpVertexArray	(i3VertexArray*	pVA);
	void	DumpNormal		(i3VertexArray*	pVA);
	void	DumpColor		(i3VertexArray*	pVA);
	void	DumpTextureCoord( i3VertexArray * pVA);
	void	DumpIndex( i3IndexArray * pIA);				//	- KOMET

	//put
	void	PutText();
	

	//Set
	void	SetType(int nType,char* cBuf);

// Dialog Data
	enum { IDD = IDD_DIALOG_GEOMETRY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();
public:
	virtual BOOL OnInitDialog();
protected:
	CEdit m_EditString;
};

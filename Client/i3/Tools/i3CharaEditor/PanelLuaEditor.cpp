// PanelLuaEditor.cpp : implementation file
//

#include "stdafx.h"
#include "i3CharaEditor.h"
#include "PanelLuaEditor.h"


// CPanelLuaEditor dialog

IMPLEMENT_DYNAMIC(CPanelLuaEditor, i3TDKDialogBase)
CPanelLuaEditor::CPanelLuaEditor(CWnd* pParent /*=NULL*/)
	: i3TDKDialogBase(CPanelLuaEditor::IDD, pParent)
{
}

CPanelLuaEditor::~CPanelLuaEditor()
{
}

void CPanelLuaEditor::DoDataExchange(CDataExchange* pDX)
{
	i3TDKDialogBase::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPanelLuaEditor, i3TDKDialogBase)
END_MESSAGE_MAP()


// CPanelLuaEditor message handlers

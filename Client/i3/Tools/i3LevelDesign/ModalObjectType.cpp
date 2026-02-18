// ModalObjectType.cpp : implementation file
//

#include "pch.h"
#include "i3LevelDesign.h"
#include "ModalObjectType.h"
#include ".\modalobjecttype.h"


// CModalObjectType dialog

IMPLEMENT_DYNAMIC(CModalObjectType, CDialog)
CModalObjectType::CModalObjectType(CWnd* pParent /*=NULL*/)
	: CDialog(CModalObjectType::IDD, pParent)
{
}

CModalObjectType::~CModalObjectType()
{
}

void CModalObjectType::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CB_TYPE, m_ObjectTypeCtrl);
}

i3ClassMeta *	CModalObjectType::Execute(void)
{
	m_Selected = 0;

	if( DoModal() == IDCANCEL)
		return NULL;

	switch( m_Selected)
	{
		case 0 :		return i3LevelChara::static_meta();
		case 1 :		return i3LevelStaticObject::static_meta();
		//case 2 :		return i3LevelChara::static_meta();
		//case 3 :		return i3LevelWorld::static_meta();
	}

	return NULL;
}

BEGIN_MESSAGE_MAP(CModalObjectType, CDialog)
END_MESSAGE_MAP()


// CModalObjectType message handlers

void CModalObjectType::OnOK()
{
	m_Selected = m_ObjectTypeCtrl.GetCurSel();

	CDialog::OnOK();
}
